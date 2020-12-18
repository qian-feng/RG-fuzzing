# build with the taint tracking support
if [[ $1 == *"taint"* ]]; then
    rm -rf build
    BITNESS=64 CC=/angora/bin/angora-clang CXX=/angora/bin/angora-clang++ USE_TRACK=1 ./build.sh
    # 
fi

# build with light instrumentation support
if [[ $1 == *"fast"* ]]; then
    rm -rf build
    BITNESS=64 CC=/angora/bin/angora-clang CXX=/angora/bin/angora-clang++ USE_FAST=1 ./build.sh
fi
