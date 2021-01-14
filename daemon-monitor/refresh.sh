# bash refresh.sh RG Childs_Game
# $1 is the mode
# $2 is the program 

# the fuzzer need .so file in the dir to load. 
if [[ $1 == *"AFL"* ]]; then
    echo "get vani"
    cp ../cb-multios-vani/build/challenges/$2/$2 ./targets/$2.vani
    cp ../cb-multios-vani/build/include/libcgc.so lib-vani/
    cp ../cb-multios-vani/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-vani/
fi

if [[ $1 == *"RG"* ]]; then
    echo "get aif"
    cp ../cb-multios-aif/build/challenges/$2/$2 ./targets/$2.aif
    cp ../cb-multios-aif/build/include/libcgc.so lib-aif/
    cp ../cb-multios-aif/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-aif/
fi

if [[ $1 == *"TAINT"* ]]; then
    echo "get angora.taint"
    cp ../cb-multios-taint/build/challenges/$2/$2 ./targets/$2.taint
    cp ../cb-multios-taint/build/include/libcgc.so lib-taint/
    cp ../cb-multios-taint/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-taint/
fi

if [[ $1 == *"FAST"* ]]; then
    echo "get angora.fast"
    cp ../cb-multios-fast/build/challenges/$2/$2 ./targets/$2.fast
    cp ../cb-multios-fast/build/include/libcgc.so lib-fast/
    cp ../cb-multios-fast/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-fast/
fi

# --------------------------------------------------
# monitor uses .cov, and it load .so file internally
if [[ $1 == *"COV"* ]]; then
    echo "get cov"
    cp ../cb-multios-cov/build/challenges/$2/$2 ./targets/$2.cov
    cp ../cb-multios-cov/build/include/libcgc.so lib-cov/
    cp ../cb-multios-cov/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-cov/
fi