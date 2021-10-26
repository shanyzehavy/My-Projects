#!/bin/bash

# Deleting from rules file comments and empty lines:
parse_rules=$(grep -o "^[^#].*" $1 | sed 's/#.*/ /' | sed '/^$/d' | tr -d "\t");

# Inserting stdin file to a variable
packets=$(cat | tr -d "\t");

while read line;do

# running the exe file for each semi rule:
correct_packets="$correct_packets$(echo "${packets}" \
				| ./firewall.exe "$(echo "$line" | cut -d ',' -f 1)" \
				| ./firewall.exe "$(echo "$line" | cut -d ',' -f 2)" \
				| ./firewall.exe "$(echo "$line" | cut -d ',' -f 3)" \
				| ./firewall.exe "$(echo "$line" | cut -d ',' -f 4)")\n";
								   	
done <<< "$parse_rules" # Stdin is parsed_rules

echo -e "$correct_packets" | sort | sed '/^$/d' | sed 's/ //g' 
