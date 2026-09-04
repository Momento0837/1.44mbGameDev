param(
    [Parameter(Mandatory = $true)]
    [string]$ExecutablePath,

    [Parameter(Mandatory = $true)]
    [string]$AssetsPath,

    [long]$MaximumBytes = 1474560
)

$executable = Get-Item -LiteralPath $ExecutablePath -ErrorAction Stop
$assetFiles = @(Get-ChildItem -LiteralPath $AssetsPath -File -Recurse)
$assetBytes = if ($assetFiles.Count -gt 0) {
    ($assetFiles | Measure-Object -Property Length -Sum).Sum
} else {
    0
}
$totalBytes = [long]$executable.Length + [long]$assetBytes
$percentage = $totalBytes / $MaximumBytes * 100.0

Write-Host ("Distribution size: {0:N0} / {1:N0} bytes ({2:N2}%)" -f `
    $totalBytes, $MaximumBytes, $percentage)

if ($totalBytes -gt $MaximumBytes) {
    Write-Error ("The executable and assets exceed the 1.44 MiB limit by " `
        + "{0:N0} bytes." -f ($totalBytes - $MaximumBytes))
    exit 1
}
