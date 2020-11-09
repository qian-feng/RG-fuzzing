CC=/home/fengqian/workdir/jie/RG-fuzzing/ijon/afl-clang-fast
rm ijon.maze.*
$CC	small.c -o ijon.maze.small.bt
$CC -DMAZE_NO_BT small.c -o ijon.maze.small.nobt
# $CC	big.c   -o afl.maze.big.bt
# $CC -DMAZE_NO_BT	big.c   -o afl.maze.big.nobt