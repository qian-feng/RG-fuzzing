# input: 
# 1. dst dir to find a queue dir to scan
# 2. csv file to write result into
# 3. configuration (RG AFL Angora)
# 4. $targ 
# 5. $opt
dstdir=`pwd`/${1} # .../queue
csvfile=${2}.csv
workdir=${2}
confn=${3}
targ=${4}
# opt=${5}

# ensure we have access to it
# chown -R $(whoami) ${dstdir}

# refresh workdir if exists
if [ -d $workdir ]; then
    rm -rf $workdir
fi 
mkdir $workdir
cd $workdir

# set up workdir
cp ../sancov.py ./
cp ../aggregate_edge.sh ./
cp /data/cb-multios-cov/build/challenges/${targ}/${targ} ./${targ}.cov

echo "${dstdir}"
# obtain the parse_result_${targ}
./aggregate_edge.sh ${dstdir} ${targ} 

# now we are ready to parse the $workdir/parse_result_${targ}
cd ../
rm $csvfile
python3 parse.py -p $workdir/parse_result_${targ} -f $csvfile