dataset=$1
chaname=$2
cbmultios=$1/cb-multios
cbtis=$1/cb-multios-tis
rm -r ${cbtis}
cp -r ${cbmultios} ${cbtis}
#build
rm -r ${cbmultios}/build
mkdir ${cbmultios}/build
cd ${cbmultios}/build
cmake -G "Unix Makefiles" ../
cd ${cbmultios}/build/challenges/${chaname}/
compiledb make 
sed -i 's/cb-multios/cb-multios-tis/g'  ${cbmultios}/build/challenges/${chaname}/compile_commands.json
mv ${cbmultios}/build/challenges/${chaname}/compile_commands.json ${cbtis}/challenges/${chaname}/compile_commands.json
make clean
bear make
sed -i 's/cb-multios/cb-multios-tis/g'  ${cbmultios}/build/challenges/${chaname}/compile_commands.json
sed -i 's/\/data\//\/home\/data\//g'  ${cbmultios}/build/challenges/${chaname}/compile_commands.json 
mv ${cbmultios}/build/challenges/${chaname}/compile_commands.json ${cbtis}/challenges/${chaname}/compile_commands_tis.json
