rm -rf output04
mkdir output04
AFL_BENCH_UNTIL_CRASH=1 /home/fengqian/workdir/jie/RG-fuzzing/ijon/afl-fuzz -S loop04 -m 200 -t 250 -i ./input -o ./output04  -- ./loop04
