echo "------------Clear files------------"
rm GameClient/BIN/GameClient.lin
rm GameClient/BIN/ClientConfig.txt
cp ClientConfig.txt GameClient/BIN/ClientConfig.txt
echo "---------------Build---------------"
# may need Additionally: depending on the platform, 
# you may need (a) a different compiler for threads, (b) a 
# different libc for threads (i.e. -lc_r), (c) -thread or -threads or other, 
# instead of or in addition to -lpthread.

g++ GameClient/*.cpp GameClient/*.h GameClient/LIBS/*.cpp GameClient/LIBS/*.h  -o GameClient/BIN/GameClient.lin -pthread
#-save-temps
echo "----------------Done---------------"
./GameClient/BIN/GameClient.lin
echo "----------------Exe finish--------------"