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
while read line
do
    if [ $counter -ge $threads ]
        echo "$parcommand"
        counter=0
        parcommand=""        
    fi

    if [ "$input" = true ]
        then
            parcommand="$parcommand $command < $line "     
        else
            parcommand="$parcommand $command $line "
        fi   

        if [ $counter -lt $(($threads-1)) ]
        then
            parcommand="$parcommand & "
        fi

        counter=$(($counter+1))
    fi
done < $args_file


    






