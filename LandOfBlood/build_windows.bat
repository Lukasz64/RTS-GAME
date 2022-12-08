echo "------------Clear files------------"
rm LandOfBlood/x64/Debug/LandOfBlood.exe
echo "---------------Build---------------"
"C:\Dok\TDM-GCC\bin\g++.exe" LandOfBlood/*.cpp LandOfBlood/*.h -o LandOfBlood/x64/Debug/LandOfBlood.exe -fdiagnostics-color=always
echo "----------------Done---------------"
"LandOfBlood/x64/Debug/LandOfBlood.exe"