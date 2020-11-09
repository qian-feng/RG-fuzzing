input="./cblist.txt"
while IFS= read -r line
do
  echo "$(./getcb.sh $line)"
  echo $line
  # mv disable-64-challenges/$line challenges/
done < "$input"

