# rm /data/debug.log
# rm /data/cover.log
rm -rf res_dir/VN/
mkdir -p res_dir/VN/queue
python monitor.py -i res_dir/VN/queue -o vanimoni-output -m vanitmp_output -l /data/VN.log -- ./Recipe_Database.cov