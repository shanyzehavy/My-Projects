#!/bin/bash

printf "Total Players: " >> Results.csv

wget https://www.premierleague.com/players 

grep -Po "/players/[0-9]+/[a-zA-Z-]+/overview" players > urls.txt 

cat urls.txt | wc -l >> Results.csv

sed -i 's/\/players/https:\/\/www\.premierleague\.com\/players/' urls.txt

for PLAYERS in $(cat urls.txt); do

# save each players' adress line into an idividual text file named adress:
wget $PLAYERS | echo "$PLAYERS" > adress.txt; 

# look for the word "midfielder" ( with or without capslk) and pushes each 
#appearance into the file "temp":
grep -ioE "midfielder" overview > temp.txt;

# counter for the word appearance from the "temp" file:
cat temp.txt | wc -w > counter.txt;

# print the adress and the counter into the Results file according to demands:
if [[ $(cat counter.txt) != 0 ]]; then 
echo "$(cat adress.txt), Midfielder, $(cat counter.txt)" >> Results.csv
fi
	
# removing all temporary files:
rm overview;
rm temp.txt;
rm adress.txt;
rm counter.txt;
done

