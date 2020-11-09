prog=${1}

export LD_LIBRARY_PATH=$PWD

echo $(rm -rf output/${prog})
echo $(mkdir -p output/${prog}) # obtain all covered log here
count=0

# enumerate 3 dir:
# seed_corpus/${prog}/MQfilter/queue seed_corpus/${prog}/MQfilter-path/_queue
find seed_corpus/${prog}/queue seed_corpus/${prog}/ijon_max -maxdepth 1  -name "id\:*"  | while read newentry; do
    echo $newentry
    LD_LIBRARY_PATH=$PWD timeout 30s ./targ_prog/${prog} < ${newentry} 1>output/${prog}/${count}.txt   
    #./targ_prog/${prog} < ${newentry} | sort | uniq -u | xargs -i grep {} output/${prog}/${count}.txt
    count=`expr $count + 1`
    #rm ${newentry}
done

