dataset=$1
chaname=$2
/data/clang-llvm/llvm-project/build/bin/RGchecker -p ${dataset}/challenges/Recipe_Database/compile_commands_RGchecker.json  $(<${dataset}/challenges/Recipe_Database/src.txt)
