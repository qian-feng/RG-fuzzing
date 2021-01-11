# e.g: $ ./run_monitor.sh Recipe_Database AFL 1
# e.g: $ ./run_monitor.sh Recipe_Database RG 1
# e.g. $ ./run_monitor.sh Recipe_Database angora 1

mkdir -p ./plot/$1
python monitor.py -l ./plot/$1/$2_$3.log