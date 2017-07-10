cd c:\Libraries\

if(!(Test-Path -Path "mpir-3.0.0" )){
	echo "Downloading and building GMP / MPIR"
	appveyor DownloadFile http://mpir.org/mpir-3.0.0.zip
	
	7z x -y mpir-3.0.0.zip
	
	cd mpir-3.0.0\build.vc15

	.\msbuild.bat gc dll x64 RELEASE
	.\msbuild.bat gc dll x64 DEBUG
	.\msbuild.bat gc lib x64 RELEASE
	.\msbuild.bat gc lib x64 DEBUG
	
	cd c:\Libraries\mpir-3.0.0\dll\x64\Release
	cp mpir.dll gmp.dll
	cp mpir.dll gmpxx.dll
	cd c:\Libraries\mpir-3.0.0\dll\x64\Debug
	cp mpir.dll gmp.dll
	cp mpir.dll gmpxx.dll
	cd c:\Libraries\mpir-3.0.0\lib\x64\Release
	cp mpirxx.lib gmpxx.lib
	cp mpir.lib gmp.lib
	cd c:\Libraries\mpir-3.0.0\lib\x64\Debug
	cp mpir.lib gmp.lib
	cp mpirxx.lib gmpxx.lib
	
	echo "Finished building GMP / MPIR"
}

$env:PATH += ";c:\Libraries\mpir-3.0.0\dll\x64\Debug"
$env:PATH += ";c:\Libraries\mpir-3.0.0\lib\x64\Debug"

echo "Looking for preinstalled Boost"
$env:PATH += ";C:\Libraries\boost_1_64_0"
$env:PATH += ";C:\Libraries\boost_1_64_0\lib64-msvc-14.1"

cd c:\projects\carl-windows
md build
cd build

cmake -G "Visual Studio 14 2015 Win64" -DBUILD_STATIC="ON" -DBOOST_ROOT="%BOOST_ROOT%" -DBOOST_LIBRARYDIR="%BOOST_LIBRARYDIR%" -DCMAKE_BUILD_TYPE=DEBUG -DBoost_COMPILER="-vc141" -DLOGGING="ON" ..
cmake --build . --config Debug --target lib_carl
cmake --build . --config Debug --target lib_carl_static
cmake --build . --config Debug --target all-tests
.\bin\Debug\runCoreTests.exe
#cmake --build . --config Debug --target RUN_TESTS
