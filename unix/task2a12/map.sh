#!/bin/bash
# Unix. A. Pachkovsky. Task 2, A12, map script

while :
do
    case "$1" in
        -c | --cmd)
            command="$2"
            shift 2
            ;;
        -i | --input)
            input=true
            shift 1
            ;;
        -t | --threads)
            threads="$2"
            shift 2
            ;;
        -* | --*)
            echo "Error: unknown option: $1" >&2
            exit 1
            ;;
        *)
            # No more optional arguments
            break
            ;;
    esac
done

if [ $# = 0 ]
then
    echo "Error: not enough arguments. A file anme is expected." >&2
    exit 1
fi

if [ $# -gt 1 ]
then
    echo "Error: too many arguments. A file anme is expected." >&2
    exit 1
fi

if [ -e "$1" ]
then     
    args_file="$1"
else
    echo "Error: file $1 does not exist" >&2
    exit 1
fi             

if [ -z "$command" ]
then
    command="echo"
fi

if [ -z "$threads" ]
then
    threads=1
fi

counter=0
parcommand=""

joinpar() {
    echo "++++"
    joined="$1"
    if [ $# -gt 0 ]
    then
        shift 1
        args_count=$(($# - 1))
        for j in $(seq 1 $args_count)
        do
            joined="$joined & $1"         
            shift 1  
        done    
        joined="$joined & $1"         
    fi
}

lines=()
echo "${#lines[*]}"
while read line
do 
#    echo "${#lines[*]}"
    if [ "$input" = true ]
    then
        lines=("${lines[@]}" "$command<$line")     
    else
        lines=("${lines[@]}" "$command $line")
    fi   
 #   echo "${#lines[*]}"

    if [ "${#lines[*]}" -gt "$threads" ]
    then
        joinpar ${lines[*]}
        parcommand=$joined 
        unset lines     
    fi

    echo $parcommand
done < $args_file

echo "${#lines[*]}"
    






