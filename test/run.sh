rm -rf output
mkdir output
# AFL_NO_UI=1
AFL_BENCH_UNTIL_CRASH=1  /home/fengqian/workdir/jie/RG-fuzzing/RG-IJON/afl-fuzz -S smallmaze -m 200 -t 250 -i ./input -o ./output  -- ./ijon.maze.small.nobt
