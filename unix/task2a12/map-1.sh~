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
    echo "Error: not enough arguments. A file name is expected." >&2
    exit 1
fi

if [ $# -gt 1 ]
then
    echo "Error: too many arguments. A file name is expected." >&2
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
else
    thregexp='^[1-9][0-9]*$'
    if ! [[ $threads =~ $thregexp ]] ; then
        echo "Error: Invalid value for -t/--threads option." >&2   
        exit 1
    fi
fi
parcommand=""

joinpar() {
    declare -a args=("${!1}")
    joined=${args[0]}
    for j in $(seq 1 $((${#args[@]} - 1)))
    do
       joined="$joined & ${args[$j]}"         
    done             
}

lines=()
while read line
do 
    
    if [ "$input" = true ]
    then
        lines+=("$command < $line")     
    else
        lines+=("$command $line")
    fi       

        if [ "${#lines[@]}" = "$threads" ]
    then
        joinpar lines[@]
        $joined
        lines=()    
    fi
    
done < $args_file

if ! [ "${#lines[@]}" = 0 ]
then 
    joinpar lines[@]
    $joined
fi
    






