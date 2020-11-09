opt=${1}

# clang++ -g  -fsanitize-coverage=trace-pc-guard trace-pc-guard-example.cc -c

clang++ -g -fsanitize-coverage=bb,no-prune,trace-pc-guard -fsanitize=address trace-pc-guard-example.cc -c
clang++ trace-pc-guard-cb.cc trace-pc-guard-example.o -fsanitize=address
ASAN_OPTIONS=strip_path_prefix=`pwd`/ ./a.out $opt