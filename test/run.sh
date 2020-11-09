rm -rf output
mkdir output
AFL_BENCH_UNTIL_CRASH=1 /home/fengqian/workdir/jie/RG-fuzzing/ijon/afl-fuzz -S smallmaze -m 200 -t 250 -i ./input -o ./output  -- ./ijon.maze.small.nobt
