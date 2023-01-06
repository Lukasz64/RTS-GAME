echo "------------Clear files------------"
rm LandOfBlood/x64/Debug/LandOfBlood.lin
rm LandOfBlood/x64/Debug/ServerConfig.txt
cp ServerConfig.txt LandOfBlood/x64/Debug/ServerConfig.txt
echo "---------------Build---------------"
g++ LandOfBlood/*.cpp LandOfBlood/*.h -o LandOfBlood/x64/Debug/LandOfBlood.lin
#-save-temps
echo "----------------Done---------------"
