param(
    [switch]$SkipSuites,
    [switch]$SkipLean
)

$ErrorActionPreference = "Stop"
$proj  = Split-Path -Parent $MyInvocation.MyCommand.Path
$root  = "$env:USERPROFILE\.elan\toolchains\leanprover--lean4---v4.35.0"
$clang = "$root\bin\clang.exe"
$lean  = "$root\bin\lean.exe"

if (-not (Test-Path -LiteralPath $clang)) {
    throw "Bundled Clang toolchain not found at $clang"
}

[System.IO.Directory]::CreateDirectory("$proj\bin") | Out-Null
[System.IO.Directory]::CreateDirectory("$proj\results") | Out-Null
[System.IO.Directory]::CreateDirectory("$proj\lean\NoeticProofs") | Out-Null

$core = @(
    "$proj\src\noetic_rng.c",
    "$proj\src\noetic_stats.c",
    "$proj\src\noetic_organism.c",
    "$proj\src\noetic_gates_core.c",
    "$proj\src\noetic_hw24.c",
    "$proj\src\noetic_soft40.c",
    "$proj\src\noetic_freak100.c",
    "$proj\src\noetic_novel100.c",
    "$proj\src\noetic_life500.c",
    "$proj\src\noetic_milestones100.c",
    "$proj\src\noetic_eval.c",
    "$proj\src\main_cli.c"
)

$cflags = @(
    "-std=c11", "-O3", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
    "-march=native", "-mavx2", "-mavxvnni", "-mbmi", "-mbmi2", "-mpopcnt",
    "-ffp-contract=off", "-flto=thin",
    "--target=x86_64-w64-windows-gnu",
    "-DNOETIC_BARE_WIN64",
    "-I$proj\include",
    "-isystem", "$root\include\clang"
)
$ldflags = @("-L$root\lib", "-fuse-ld=lld")
$libs = @("-lkernel32", "-lucrtbase", "-lm", "-lmingw32", "-lmingwex")

Write-Host "[1/7] Building bin\noetic_engine.exe v2.0.0 (12 C11 modules, AVX2/VNNI + ThinLTO, -Wall -Wextra -Werror) ..."
& $clang @cflags @core -o "$proj\bin\noetic_engine.exe" @ldflags @libs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[2/7] Running M7 Unit, Invariant & 5 Anti-LARP Mutant Suite ..."
& "$proj\bin\noetic_engine.exe" test
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[3/7] Running Clean-Slate v3.0 Native Release Gates Suite ..."
& "$proj\bin\noetic_engine.exe" gates "$proj\results\m9_native_release_gates.json"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not $SkipSuites) {
    Write-Host "[4/7] Running Full 9-Suite Clean-Slate v3.0 Native Battery (HW24 + SOFT40 + FREAK100 + NOVEL100 + LIFE500 + M100) ..."
    & "$proj\bin\noetic_engine.exe" hw24 "$proj\results\m10_hw24_receipt.json"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" soft40 48 "$proj\results\m10_soft40_receipt.json"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" freak100 48 "$proj\results\m10_freak100_receipt.json"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" novel100 48 "$proj\results\m10_novel100_receipt.json"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" life500 48 "$proj\results\m10_life500_receipt.json"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" milestones100 "$proj\results\m10_milestones_100_receipt.json"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Write-Host "[5/7] Running M8 E1 (18 Ablations), E2 (49 Cognitive/CQM Battery, N=48 locked seeds), E3 (Scaling) ..."
    & "$proj\bin\noetic_engine.exe" e1 48 "$proj\results\m8_e1_ablation_sweep.json" "$proj\results\m8_e1_ablation_sweep.csv"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" e2 48 "$proj\results\m8_e2_cognitive_49.json" "$proj\results\m8_e2_cognitive_49.csv"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & "$proj\bin\noetic_engine.exe" e3 "$proj\results\m8_e3_scaling_bench.json" "$proj\results\m8_e3_scaling_bench.csv"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

Write-Host "[6/7] Exporting Live C11 -> Lean 4 Receipt (seed 5001) ..."
& "$proj\bin\noetic_engine.exe" export-lean 5001 "$proj\lean\NoeticProofs\LiveC11Receipt.lean"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not $SkipLean -and (Test-Path -LiteralPath $lean)) {
    Write-Host "[7/7] Verifying 48 Pure-Kernel Lean 4 Mathematical Proofs (strict axiom allowlist) ..."
    Push-Location -LiteralPath "$proj\lean"
    $env:LEAN_PATH = "$proj\lean"
    & $lean -R "$proj\lean" -o "$proj\lean\NoeticProofs\LiveC11Receipt.olean" "$proj\lean\NoeticProofs\LiveC11Receipt.lean"
    if ($LASTEXITCODE -ne 0) { Pop-Location; throw "Failed compiling LiveC11Receipt.lean" }

    $leanOut = & $lean -R "$proj\lean" "$proj\lean\NoeticProofs.lean" 2>&1
    $leanRc = $LASTEXITCODE
    Pop-Location
    [System.IO.File]::WriteAllText("$proj\results\lean4-noetic-audit.txt", (($leanOut | ForEach-Object { "$_" }) -join "`n") + "`n", [System.Text.Encoding]::UTF8)
    foreach ($line in $leanOut) { Write-Host $line }
    if ($leanRc -ne 0) {
        throw "Lean 4 verification failed with exit code $leanRc"
    }

    $allowed = @("propext", "Classical.choice", "Quot.sound")
    $audited = 0
    foreach ($raw in $leanOut) {
        $line = "$raw".Trim()
        if ($line -eq "") { continue }
        if ($line -match "sorryAx|_native\.bv_decide|warning:") {
            throw "FORBIDDEN Lean output detected: $line"
        }
        if ($line -match "^'([^']+)' does not depend on any axioms$") {
            $audited++
        } elseif ($line -match "^'([^']+)' depends on axioms: \[([^\]]+)\]$") {
            $audited++
            $axs = $Matches[2].Split(",") | ForEach-Object { $_.Trim() }
            foreach ($ax in $axs) {
                if ($allowed -notcontains $ax) {
                    throw "FORBIDDEN Lean axiom '$ax' in theorem $($Matches[1])"
                }
            }
        }
    }
    if ($audited -lt 48) {
        throw "Expected at least 48 audited Lean theorems, found $audited"
    }
    Write-Host "PURE LEAN 4 AUDIT PASSED: $audited / 48 theorems strictly within {propext, Classical.choice, Quot.sound}!"
}
