# e.g: $ ./run_fuzzer.sh AFL 1
# e.g: $ ./run_fuzzer.sh RG 1
modelname=$1
prefixname=$1_$2

if [[ $1 == *"AFL"* ]]; then
    #echo "get vani"
    LD_LIBRARY_PATH=./lib-vani/ AFL_NO_UI=1 /data/RG-IJON/afl-fuzz -S $modelname -m 200 -i ./input -o ./${prefixname} -- ./Recipe_Database.vani
else if [[ $1 == *"RG"* ]]; then
    # echo "get aif"
    LD_LIBRARY_PATH=./lib-aif/ AFL_NO_UI=1 /data/RG-IJON/afl-fuzz -S $modelname -m 200 -i ./input -o ./${prefixname} -- ./Recipe_Database.aif
    fi
fi

