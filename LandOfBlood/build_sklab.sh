echo "------------Clear files------------"
rm LandOfBlood/x64/Debug/LandOfBlood.lin
rm LandOfBlood/x64/Debug/ServerConfig.txt
cp ServerConfig.txt LandOfBlood/x64/Debug/ServerConfig.txt
echo "---------------Build---------------"
# may need Additionally: depending on the platform, 
# you may need (a) a different compiler for threads, (b) a 
# different libc for threads (i.e. -lc_r), (c) -thread or -threads or other, 
# instead of or in addition to -lpthread.

g++ LandOfBlood/*.cpp LandOfBlood/*.h -o LandOfBlood/x64/Debug/LandOfBlood.lin -pthread
#-save-temps
echo "----------------Done---------------"
