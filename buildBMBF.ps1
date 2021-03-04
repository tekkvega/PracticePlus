# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libPracticePlus.so", "./libs/arm64-v8a/libbeatsaber-hook_1_1_2.so", "./bmbfmod.json" -DestinationPath "./PracticePlus_v0.4.0.zip" -Update
}
