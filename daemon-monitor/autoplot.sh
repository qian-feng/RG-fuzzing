targpro=${1}
# opt=${2}
for CONF in RG #AFL Angora
do 
    for ATTEMPT in 1 #2 3
    do
        # input: 
        # 1. dst dir to find a queue dir to scan
        # 2. csv file to write result into
        # 3. configuration (RG AFL Angora)
        # 4. $targ 
        # 5. $opt
        ./dirscanto.sh workdir_${targpro}/${CONF}_${ATTEMPT}/${CONF}/queue ${targpro}_${CONF}_${ATTEMPT} ${CONF} ${targpro} & 
    done
done