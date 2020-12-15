if [[ $1 == *"AFL"* ]]; then
    echo "get vani"
    cp ../cb-multios-vani/build/challenges/Recipe_Database/Recipe_Database ./Recipe_Database.vani
    cp ../cb-multios-vani/build/include/libcgc.so lib-vani/
    cp ../cb-multios-vani/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-vani/
fi

if [[ $1 == *"RG"* ]]; then
    echo "get aif"
    cp ../cb-multios-aif/build/challenges/Recipe_Database/Recipe_Database ./Recipe_Database.aif
    cp ../cb-multios-aif/build/include/libcgc.so lib-aif/
    cp ../cb-multios-aif/build/include/tiny-AES128-C/libtiny-AES128-C.so lib-aif/
fi

if [[ $1 == *"COV"* ]]; then
    echo "get cov"
    cp ../cb-multios-cov/build/challenges/Recipe_Database/Recipe_Database ./Recipe_Database.cov
fi
