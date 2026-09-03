param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)

Add-Type -AssemblyName System.Drawing

$sourcePath = Join-Path $ProjectRoot 'src\main.cpp'
$narrationPath = Join-Path $ProjectRoot 'assets\narration.json'
$outputDirectory = Join-Path $ProjectRoot 'assets\fonts'
$userFontDirectory = Join-Path $env:LOCALAPPDATA 'Microsoft\Windows\Fonts'

$characters = [System.Collections.Generic.HashSet[char]]::new()
for ($codePoint = 32; $codePoint -le 126; ++$codePoint) {
    [void]$characters.Add([char]$codePoint)
}

$source = [System.IO.File]::ReadAllText($sourcePath)
$wideStrings = [regex]::Matches($source, 'L"((?:\\.|[^"\\])*)"')
foreach ($match in $wideStrings) {
    foreach ($character in $match.Groups[1].Value.ToCharArray()) {
        if (-not [char]::IsControl($character) -and -not [char]::IsWhiteSpace($character)) {
            [void]$characters.Add($character)
        }
    }
}

$narration = [System.IO.File]::ReadAllText($narrationPath)
foreach ($character in $narration.ToCharArray()) {
    if (([int]$character -gt 126) -and -not [char]::IsControl($character) -and -not [char]::IsWhiteSpace($character)) {
        [void]$characters.Add($character)
    }
}

$orderedCharacters = @($characters | Sort-Object { [int]$_ })
$characterMap = -join $orderedCharacters
[System.IO.Directory]::CreateDirectory($outputDirectory) | Out-Null

function New-BitmapFontAtlas {
    param(
        [string]$FontPath,
        [string]$OutputName
    )

    $fontCollection = [System.Drawing.Text.PrivateFontCollection]::new()
    $fontCollection.AddFontFile($FontPath)
    $font = [System.Drawing.Font]::new(
        $fontCollection.Families[0],
        32,
        [System.Drawing.FontStyle]::Regular,
        [System.Drawing.GraphicsUnit]::Pixel)

    $columns = 16
    $cellWidth = 40
    $cellHeight = 44
    $rows = [int][math]::Ceiling($orderedCharacters.Count / $columns)
    $bitmap = [System.Drawing.Bitmap]::new(
        $columns * $cellWidth,
        $rows * $cellHeight,
        [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.Clear([System.Drawing.Color]::Transparent)
    $graphics.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::SingleBitPerPixelGridFit
    $brush = [System.Drawing.SolidBrush]::new([System.Drawing.Color]::White)
    $format = [System.Drawing.StringFormat]::GenericTypographic.Clone()
    $format.FormatFlags = $format.FormatFlags -bor [System.Drawing.StringFormatFlags]::NoClip

    for ($index = 0; $index -lt $orderedCharacters.Count; ++$index) {
        $x = ($index % $columns) * $cellWidth
        $y = [math]::Floor($index / $columns) * $cellHeight
        $graphics.DrawString(
            [string]$orderedCharacters[$index],
            $font,
            $brush,
            [single]($x + 2),
            [single]($y + 2),
            $format)
    }

    $pngPath = Join-Path $outputDirectory ($OutputName + '.png')
    $mapPath = Join-Path $outputDirectory ($OutputName + '.chars.txt')
    $infoPath = Join-Path $outputDirectory ($OutputName + '.info.txt')
    $bitmap.Save($pngPath, [System.Drawing.Imaging.ImageFormat]::Png)
    [System.IO.File]::WriteAllText(
        $mapPath, $characterMap, [System.Text.UTF8Encoding]::new($false))
    [System.IO.File]::WriteAllLines(
        $infoPath,
        @(
            'font_pixel_size=32',
            'cell_width=40',
            'cell_height=44',
            'columns=16',
            ('glyph_count=' + $orderedCharacters.Count)
        ),
        [System.Text.UTF8Encoding]::new($false))

    $format.Dispose()
    $brush.Dispose()
    $graphics.Dispose()
    $bitmap.Dispose()
    $font.Dispose()
    $fontCollection.Dispose()
}

New-BitmapFontAtlas `
    -FontPath (Join-Path $userFontDirectory 'PF스타더스트.ttf') `
    -OutputName 'pf_stardust_32'
New-BitmapFontAtlas `
    -FontPath (Join-Path $userFontDirectory 'DOSPilgi.ttf') `
    -OutputName 'dos_pilgi_32'

Get-ChildItem -LiteralPath $outputDirectory -File |
    Select-Object Name, Length
