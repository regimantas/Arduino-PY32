[CmdletBinding()]
param(
  [string]$OutFile,
  [string]$Version
)

$ErrorActionPreference = 'Stop'

function Assert-Command([string]$Name) {
  if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
    throw "Required command not found in PATH: $Name"
  }
}

$HasGit = [bool](Get-Command git -ErrorAction SilentlyContinue)

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
Push-Location $RepoRoot
try {
  if ([string]::IsNullOrWhiteSpace($Version)) {
    $platformTxt = Join-Path $RepoRoot 'platform.txt'
    if (Test-Path $platformTxt) {
      $m = (Select-String -Path $platformTxt -Pattern '^version\s*=\s*(.+)\s*$' -ErrorAction SilentlyContinue | Select-Object -First 1)
      if ($m -and $m.Matches.Count -gt 0) {
        $Version = $m.Matches[0].Groups[1].Value.Trim()
      }
    }
    if ([string]::IsNullOrWhiteSpace($Version)) {
      $Version = 'dev'
    }
  }

  if ($HasGit) {
    # If this folder is not a git working tree (e.g. downloaded zip / copied directory),
    # skip git-specific steps but still allow packaging.
    $IsGitWorkTree = $false
    try {
      & git rev-parse --is-inside-work-tree *> $null
      $IsGitWorkTree = ($LASTEXITCODE -eq 0)
    } catch {
      $IsGitWorkTree = $false
    }

    if ($IsGitWorkTree) {
      Write-Host "Updating submodules..." -ForegroundColor Cyan
      git submodule update --init --recursive
    } else {
      Write-Host "Not a git working tree; skipping submodule update." -ForegroundColor Yellow
    }
  } else {
    Write-Host "git not found; skipping submodule update." -ForegroundColor Yellow
  }

  if ([string]::IsNullOrWhiteSpace($OutFile)) {
    $dist = Join-Path $RepoRoot 'dist'
    New-Item -ItemType Directory -Force -Path $dist | Out-Null
    $OutFile = Join-Path $dist ("Arduino-PY32-{0}.zip" -f $Version)
  } else {
    $OutFile = (Resolve-Path (Split-Path -Parent $OutFile)).Path + "\" + (Split-Path -Leaf $OutFile)
  }

  $TempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("arduino-py32-release-{0}" -f ([Guid]::NewGuid().ToString('N')))
  $StageDir = Join-Path $TempRoot "Arduino-PY32"

  New-Item -ItemType Directory -Force -Path $StageDir | Out-Null

  Write-Host "Staging working tree (excluding .git and local artifacts)..." -ForegroundColor Cyan
  robocopy $RepoRoot $StageDir /E /XD .git .github dist .vscode _build_cli _build_sanity _cli_config _release_out _sketchbook _sketches _tmp_* /XF *.user *.zip *.bak *.bak_* /NFL /NDL /NJH /NJS /NP | Out-Null
    # Exclude any working folders that start with '_' (e.g. _tmp_*, _work, etc.)
    Get-ChildItem -LiteralPath $StageDir -Directory -Force -Recurse |
      Where-Object { $_.Name -like '_*' } |
      ForEach-Object { Remove-Item -LiteralPath $_.FullName -Recurse -Force }

  if (Test-Path $OutFile) { Remove-Item -Force $OutFile }

  Write-Host "Creating zip: $OutFile" -ForegroundColor Cyan
  # Important: Arduino CLI expects the archive to have exactly one top-level directory.
  # Zipping "$StageDir\*" would put multiple entries at the archive root and can break installation.
  $maxAttempts = 6
  for ($attempt = 1; $attempt -le $maxAttempts; $attempt++) {
    try {
      Compress-Archive -Path $StageDir -DestinationPath $OutFile
      break
    } catch [System.IO.IOException] {
      if ($attempt -ge $maxAttempts) { throw }
      Write-Host ("Zip creation hit a transient file lock; retrying ({0}/{1})..." -f $attempt, $maxAttempts) -ForegroundColor Yellow
      Start-Sleep -Milliseconds 750
    }
  }

  Write-Host "Done." -ForegroundColor Green
}
finally {
  Pop-Location
  if ($TempRoot -and (Test-Path $TempRoot)) {
    Remove-Item -Recurse -Force $TempRoot
  }
}
