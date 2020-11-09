input="./list.txt"
while IFS= read -r line
do
	cp -r total-challenges/$line challenges/
done < "$input"
