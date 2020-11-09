# ./getcb.sh Accel
pname=${1}
cp ../build/challenges/$pname/${pname}_patched ./targ_prog/${pname}

mkdir -p seed_corpus/$pname/

# cp ../build/challenges/$pname/*.pov seed_corpus/$pname/
