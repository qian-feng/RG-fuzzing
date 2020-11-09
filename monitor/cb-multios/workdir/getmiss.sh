prog=${1}
start=${2}
end=${3}
python3 parse.py -n ${prog} -p ${start} -m ${end}
./sancov.py missing ./targ_prog/${prog} < ${prog}-covered.txt > ${prog}-missed.txt

rm ${prog}-res.txt
input="./${prog}-missed.txt"
while IFS= read -r line
do
    srcinfo=$(addr2line -e ./targ_prog/${prog} $line)
    #echo "$line, $srcinfo" >> ${prog}-res.txt
    json='{"addr":'${line}',"line":'${srcinfo}'}'
    echo $json >> ${prog}-res.txt
done < "$input"
