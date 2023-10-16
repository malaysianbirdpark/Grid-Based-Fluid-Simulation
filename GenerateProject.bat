call 3rd_party\premake\premake5.exe vs2019
@echo OFF 
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
echo "Starting Build for all Projects with proposed changes"
echo .  
devenv "3rd_party\DirectXTK-main\DirectXTK_Desktop_2019.sln" /build Debug|x64
devenv "3rd_party\DirectXTK-main\DirectXTK_Desktop_2019.sln" /build Release|x64
devenv "3rd_party\libfreetype2\freetype2\builds\windows\vc2010\freetype.sln" /build Debug|x64
devenv "3rd_party\libfreetype2\freetype2\builds\windows\vc2010\freetype.sln" /build Release|x64
devenv "Grid.sln" /build Debug|x64
devenv "Grid.sln" /build Release|x64
echo . 
echo "All builds completed." 
PAUSE