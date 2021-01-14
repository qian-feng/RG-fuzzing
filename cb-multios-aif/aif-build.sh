rm -rf build
CC=`pwd`/../RG-IJON/afl-clang-fast ./build.sh

# LINK=STATIC

# refresh the binary in targets
cp build/include/tiny-AES128-C/libtiny-AES128-C.so ../daemon-monitor/lib-aif/
cp build/include/libcgc.so ../daemon-monitor/lib-aif/

for prog in Board_Game  Childs_Game  Recipe_Database
do
    cp build/challenges/${prog}/${prog} ../daemon-monitor/targets/${prog}.aif 
    echo "[log]: Successfully update aif binary of ${prog}"
done