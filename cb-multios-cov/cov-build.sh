rm -rf build
CC=clang CXX=clang++ CFLAGS="-fsanitize-coverage=edge,no-prune,trace-pc-guard -fsanitize=address"  ./build.sh

