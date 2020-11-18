rm /data/debug.log
# rm -rf RD_output
# mkdir RD_output
#AFL_BENCH_UNTIL_CRASH=1 /home/fengqian/workdir/jie/RG-fuzzing/RG-IJON/afl-fuzz -S RD -m 200 -i ./RD_input -o ./RD_output  -- /home/fengqian/workdir/jie/RG-fuzzing/cb-multios-aif/build/challenges/Recipe_Database/Recipe_Database
AFL_NO_UI=1 AFL_BENCH_UNTIL_CRASH=1 /data/RG-IJON/afl-fuzz -S RD -m 200 -i ./RD_input -o ./RD_output  -- /data/cb-multios-aif/build/challenges/Recipe_Database/Recipe_Database
