input="./list.txt"
while IFS= read -r line
do
	rm -rf challenges/$line
done < "$input"
