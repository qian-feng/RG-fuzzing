# e.g: $ ./run_monitor.sh Recipe_Database AFL 1
# e.g: $ ./run_monitor.sh Recipe_Database RG 1
# e.g. $ ./run_monitor.sh Recipe_Database angora 1
targ=$1
modelname=$2
modelround=$3
prefixname=$2_$3

mkdir -p ./workdir_${targ}/${prefixname}_tmp
mkdir -p ./plot/${targ} # logs for plotting 

if [[ $1 == *"Angora"* ]]; then
    seed_queue=./workdir_${targ}/${prefixname}/queue
    
else
    seed_queue=./workdir_${targ}/${prefixname}/${modelname}/queue
    rm -rf ./workdir_${targ}/${prefixname}/${modelname}
    mkdir -p ${seed_queue}
fi

tmp_dir=./workdir_${targ}/${prefixname}_tmp
logfile=./plot/${targ}/${prefixname}.log

# here is to start monitor 
python monitor.py -i ${seed_queue} -m ${tmp_dir} -l ${logfile} -- ./targets/${targ}.cov
