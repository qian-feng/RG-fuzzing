# e.g: $ ./run_fuzzer.sh Recipe_Database AFL 1 
# e.g: $ ./run_fuzzer.sh Recipe_Database RG 1 
targ=$1
modelname=$2
prefixname=$2_$3

if [[ ${modelname} == *"AFL"* ]]; then
    #echo "get vani"
    LD_LIBRARY_PATH=./lib-vani/ AFL_NO_UI=1 /data/RG-IJON/afl-fuzz -S $modelname -m 200 -i ./input -o ./workdir_${targ}/${prefixname} -- ./targets/${targ}.vani
fi

if [[ ${modelname} == *"RG"* ]]; then
    # echo "get aif"
    LD_LIBRARY_PATH=./lib-aif/ AFL_NO_UI=1 /data/RG-IJON/afl-fuzz -S $modelname -m 200 -i ./input -o ./workdir_${targ}/${prefixname} -- ./targets/${targ}.aif
fi

if [[ ${modelname} == *"Angora"* ]]; then
    # echo "run angora fuzzer"
    /angora/angora_fuzzer -i ./input -o ./workdir_${targ}/${prefixname}/ -t ./targets/${targ}.taint -- ./targets/${targ}.fast 
fi