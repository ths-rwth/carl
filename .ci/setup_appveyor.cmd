cd c:\Libraries\

IF NOT EXIST mpir-3.0.0 (
	echo Downloading and building GMP / MPIR
	appveyor DownloadFile http://mpir.org/mpir-3.0.0.zip
	
	7z x -y mpir-3.0.0.zip
	
	cd mpir-3.0.0\build.vc15

	msbuild.bat gc dll x64 RELEASE
	msbuild.bat gc dll x64 DEBUG
	msbuild.bat gc lib x64 RELEASE
	msbuild.bat gc lib x64 DEBUG
	
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
	
	echo Finished building GMP / MPIR
)

set PATH=%PATH%;c:\Libraries\mpir-3.0.0\dll\x64\Debug
set PATH=%PATH%;c:\Libraries\mpir-3.0.0\lib\x64\Debug

echo Looking for preinstalled Boost
set PATH=%PATH%;C:\Libraries\boost_1_64_0
set PATH=%PATH%;C:\Libraries\boost_1_64_0\lib64-msvc-14.1

cd c:\projects\carl
md build
cd build

cmake -G "Visual Studio 14 2015 Win64" -DBUILD_STATIC="ON" -DBOOST_ROOT="%BOOST_ROOT%" -DBOOST_LIBRARYDIR="%BOOST_LIBRARYDIR%" -DBUILD_TYPE=DEBUG -DBoost_COMPILER="-vc141" ..
cmake --build . --config Debug --target lib_carl
cmake --build . --config Debug --target lib_carl_static
cmake --build . --config Debug --target all-tests
cmake --build . --config Debug --target RUN_TESTS
