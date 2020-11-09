rm -rf RD_output
mkdir RD_output
AFL_BENCH_UNTIL_CRASH=1 /home/fengqian/workdir/jie/RG-fuzzing/ijon/afl-fuzz -S RD -m 200 -i ./RD_input -o ./RD_output  -- /home/fengqian/workdir/jie/RG-fuzzing/cb-multios/build/challenges/Recipe_Database/Recipe_Database
