rm -rf build
CC=clang CFLAGS="-fsanitize-coverage=edge,no-prune,trace-pc-guard -fsanitize=address" ./build.sh
