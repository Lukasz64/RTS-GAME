echo "------------Clear files------------"
rm LandOfBlood/x64/Debug/LandOfBlood.lin
echo "---------------Build---------------"
g++ LandOfBlood/*.cpp LandOfBlood/*.h -o LandOfBlood/x64/Debug/LandOfBlood.lin
#-save-temps
echo "----------------Done---------------"
LandOfBlood/x64/Debug/LandOfBlood.lin
