#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Niepoprawna ilość argumentów"
    exit 1
fi

if ! [ -x $1 ]; then
    echo "To nie jest plik wykonywalny"
    exit 1
fi

if ! [ -d $2 ]; then
    echo "Nie ma takiego katlogu"
    exit 1
fi

#szukam pliku START

start_file="brak"

for curr_file in "$2"*; do
    line=$(head -n 1 "$curr_file")
    if [ "$line" = "START" ]; then
        start_file="$curr_file"
    fi
done

#teraz iteruję przez linie pliku

target="$start_file"
input_file="$(mktemp)"
output_file="$(mktemp)"
next_file="brak"
end="false"

while [ $end == "false" ]; do

    while read curr_line; do
        if [[ "$curr_line" == "FILE "* ]]; then
            next_file=${curr_line#FILE *}
        elif [[ "$curr_line" == "STOP" ]]; then
            end="true"
        elif [[ "$curr_line" != "START" ]]; then
            echo "$curr_line" >> "$input_file"
        fi
    done < "$target"
    
    if [[ $end == "false" ]]; then
        cat "$input_file" | ./$1 > "$output_file"
    elif [[ $end == "true" ]]; then
        cat "$input_file" | ./$1
    fi
    
    temp="$input_file"
    input_file="$output_file"
    output_file="$temp"
    > "$output_file"
    
    target="$2"$next_file
done

rm -f $output_file
rm -f $input_file

exit 0
