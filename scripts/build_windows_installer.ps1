param(
    [Parameter(Mandatory = $true)]
    [string]$Vst3Path,

    [Parameter(Mandatory = $true)]
    [string]$OutputExe,

    [string]$Version = "1.0.0"
)

$ErrorActionPreference = "Stop"

$resolvedVst3 = (Resolve-Path $Vst3Path).Path
$resolvedOutput = [System.IO.Path]::GetFullPath($OutputExe)
$outputDir = Split-Path -Parent $resolvedOutput
$outputBaseName = [System.IO.Path]::GetFileNameWithoutExtension($resolvedOutput)

New-Item -ItemType Directory -Force -Path $outputDir | Out-Null

$compiler = @(
    "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
    "C:\Program Files\Inno Setup 6\ISCC.exe"
) | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not $compiler) {
    throw "Inno Setup compiler not found."
}

$tempDir = Join-Path $env:RUNNER_TEMP "vayu-installer"
New-Item -ItemType Directory -Force -Path $tempDir | Out-Null
$issPath = Join-Path $tempDir "VayuInstaller.iss"

$iss = @"
#define Vst3Source "$resolvedVst3"
#define OutputDir "$outputDir"
#define OutputBaseName "$outputBaseName"
#define AppVersion "$Version"

[Setup]
AppId={{E0D26AB9-30AE-4122-9830-31FA703B744A}
AppName=Vayu
AppVersion={#AppVersion}
AppPublisher=Supash Bhat
AppPublisherURL=https://supashbhat.github.io
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir={#OutputDir}
OutputBaseFilename={#OutputBaseName}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#Vst3Source}\*"; DestDir: "{commoncf64}\VST3\Vayu.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
"@

Set-Content -Path $issPath -Value $iss -Encoding UTF8
& $compiler $issPath
