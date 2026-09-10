$ErrorActionPreference = 'Stop'
$expected = if ($env:target_platform -eq 'win-arm64') { 0xaa64 } else { 0x8664 }
$dlls = @(Get-ChildItem "$env:LIBRARY_BIN\gsl*.dll")
if ($dlls.Count -eq 0) { throw 'No installed GSL DLL found' }
$files = @((Get-Item regression_test.exe)) + $dlls
foreach ($file in $files) {
    $bytes = [IO.File]::ReadAllBytes($file.FullName)
    $offset = [BitConverter]::ToInt32($bytes, 0x3c)
    $machine = [BitConverter]::ToUInt16($bytes, $offset + 4)
    Write-Host "$($file.Name): PE machine 0x$($machine.ToString('x4'))"
    if ($machine -ne $expected) { throw "Unexpected architecture in $($file.Name)" }
}
