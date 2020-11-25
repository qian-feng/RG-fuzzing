dataset=$1
chaname=$2
/data/clang-llvm/llvm-project/build/bin/RGchecker -p ${dataset}/challenges/Recipe_Database/compile_commands_RGchecker.json --unvisited=/data/clang-llvm/llvm-project/clang-tools-extra/RGchecker/data/cgc-AI-list/Recipe_Database-res.txt $(<${dataset}/challenges/Recipe_Database/src.txt)
