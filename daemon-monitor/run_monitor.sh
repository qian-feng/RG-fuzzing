# e.g: $ ./run_monitor.sh AFL 1
# e.g: $ ./run_monitor.sh RG 1
modelname=$1
modelround=$2
prefixname=$1_$2

rm -rf ${prefixname}/${modelname}
mkdir -p ${prefixname}/${modelname}/queue
python monitor.py -i ${prefixname}/${modelname}/queue -o ${prefixname}_fortis -m ${prefixname}_tmp -l /data/${prefixname}.log -- ./Recipe_Database.cov