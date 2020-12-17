dataset=$1
chaname=$2
/data/clang-llvm/llvm-project/build/bin/RGchecker -p ${dataset}/challenges/${chaname}/compile_commands.json --unvisited=/data/clang-llvm/llvm-project/clang-tools-extra/RGchecker/data/cgc-AI-list/Board_Game-res.txt $(<${dataset}/challenges/$2/src.txt)
