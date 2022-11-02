if ( $args.Length -ne 1) {
    Write-Host "Provide root location of a Python installation"
    Return
}
if (-Not (Test-Path -Path $args[0])){
    Write-Host "Root location of a Python installation doesnt exist"
    Return
}

$msbuild = "c:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\amd64\msbuild"
$pythonroot = $args[0]
$project = "..\pythonapi.vcxproj"
$config1 = "/p:Configuration=Release"
$config2 = "/p:Platform=x64"
$config3 = "/p:PYTHONROOT=$pythonroot"
Set-Location -Path $PSScriptRoot

$collectionOfArgs = @("$project","$config1", "$config2", "$config3")

& "$msbuild" $collectionOfArgs


$parts = $pythonroot.Split("\\")
$folder = $parts[$parts.Count-1]
$currentP = $PSScriptRoot + "\\..\\..\\output\\release\\ilwispy"
$ilwispyout = $currentP + "\\" + "$folder"
if (-Not (Test-Path -Path $ilwispyout)) {
    New-Item -Path $ilwispyout -ItemType Directory
}
$ilwisFolder = $currentP + "\\ilwis"
Copy-Item -Path $ilwisFolder -Destination $ilwispyout -recurse -Force 
$outsetup = $currentP + "\\setup.py"
$outfolder =  $ilwispyout + "\\"
Copy-Item $outsetup $outfolder
Set-Location -Path $ilwispyout
$py = "./setup.py"
$wheel = "bdist_wheel"
#$platform = "--plat-name win_amd64"
$univ = "-universal"
$command = $pythonroot + "\" + "python.exe"
$collectionOfArgs = @($py, $wheel, $univ)
& "$command" $collectionOfArgs






