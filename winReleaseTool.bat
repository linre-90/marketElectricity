CLS
ECHO off
ECHO Interactive release folder build tool for market electricity.
ECHO:
ECHO Starting copying build files from ".\out\build\x64-release" to ".\release\windows"
ECHO:
ECHO Copying assets...
xcopy ".\out\build\x64-release\resources" ".\release\windows\resources\"
ECHO:
ECHO Copying .exe build...
xcopy ".\out\build\x64-release\marketElectricity.exe" ".\release\windows\"
ECHO:
ECHO Deleting cache...
DEL ".\release\windows\cache.mecache"
ECHO:
ECHO Deleting log...
DEL ".\release\windows\me_logs.txt"
ECHO:
ECHO Succesfully copied all needed assets to ".\release\windows".