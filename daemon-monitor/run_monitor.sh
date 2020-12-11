rm /data/debug.log
rm /data/cover.log
rm -rf res_dir/RD/
mkdir -p res_dir/RD/queue
python monitor.py -i res_dir/RD/queue -o monitor-output -m tmp_output -l /data/RG.log -- ./Recipe_Database.cov