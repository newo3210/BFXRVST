param(
    [Parameter(Mandatory = $true)]
    [string] $Bfxr2WebRoot
)

function Write-Utf8NoBom([string] $Path, [string] $Text) {
    $utf8NoBom = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($Path, $Text, $utf8NoBom)
}

$indexJs = Join-Path $Bfxr2WebRoot "js\index.js"
if (-not (Test-Path $indexJs)) {
    Write-Warning "patch-bfxr2-vst: no se encontro $indexJs"
    exit 0
}

$js = [System.IO.File]::ReadAllText($indexJs)

if ($js -notmatch 'BFXRVST: sin Footsteppr') {
    $js = [regex]::Replace(
        $js,
        'var footsteppr_tab = new Tab\(new Footsteppr\(\)\);',
        "// BFXRVST: sin Footsteppr`n    var footsteppr_tab = null;")
}

if ($js -notmatch 'active_tab_index >= tabs\.length') {
    $js = [regex]::Replace(
        $js,
        'if \(active_tab_index>=0\)\{\r?\n\s*tabs\[active_tab_index\]\.set_active_tab\(\);\r?\n\s*\}',
        "if (active_tab_index>=0){`n        if (active_tab_index >= tabs.length) { active_tab_index = 0; }`n        tabs[active_tab_index].set_active_tab();`n    }")
}

Write-Utf8NoBom $indexJs $js

$saveLoad = Join-Path $Bfxr2WebRoot "js\SaveLoad.js"
if (Test-Path $saveLoad) {
    $sl = [System.IO.File]::ReadAllText($saveLoad)
    # Parche antiguo erroneo (una sola linea con backtick-n literal)
    if ($sl -match '// BFXRVST clamp.*`n') {
        $sl = [regex]::Replace(
            $sl,
            '(?m)^(\s*)// BFXRVST clamp.*tabs\[active_tab_index\]\.set_active_tab\(\);\s*$',
            '${1}tabs[active_tab_index].set_active_tab();')
    }
    if ($sl -notmatch '(?m)if \(active_tab_index >= tabs\.length\) \{ active_tab_index = 0; \}') {
        $pat = '(?m)^(\s*)tabs\[active_tab_index\]\.set_active_tab\(\);\s*$'
        $rep = '$1// BFXRVST clamp active_tab_index (colecciones guardadas con Footsteppr)' + "`r`n" + '$1if (active_tab_index >= tabs.length) { active_tab_index = 0; }' + "`r`n" + '$1tabs[active_tab_index].set_active_tab();'
        $sl = [regex]::Replace($sl, $pat, $rep, [System.Text.RegularExpressions.RegexOptions]::Multiline)
        Write-Utf8NoBom $saveLoad $sl
    }
}

$htmlPath = Join-Path $Bfxr2WebRoot "index.html"
if (Test-Path $htmlPath) {
    $html = [System.IO.File]::ReadAllText($htmlPath)
    if ($html -notmatch 'BFXRVST: omitido Footsteppr') {
        $html = $html.Replace(
            '<script src="js/synths/Footsteppr.js"></script>',
            '<!-- BFXRVST: omitido Footsteppr.js --><script>/* desactivado */</script>')
        Write-Utf8NoBom $htmlPath $html
    }
}

Write-Host "patch-bfxr2-vst: listo en $Bfxr2WebRoot"
