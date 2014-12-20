#!/bin/ash
# Unix. A. Pachkovsky. Task 2, A12, map script

usage() {
    echo "Usage: [-h|--help] [-c|--cmd COMMAND] [-i|--input] [-t|--threads THREDNUM] FILENAME"
    echo -e "Maps a specified command and its arguments from FILENAME file.\n"
    echo "Positional arguments:"
    echo -e "  FILENAME                    A file name of the file that contains arguments.\n"
    echo "Optional arguments:"
    echo "  -c COMMAND,                 A command that will be applied to arguments."
    echo "      --cmd COMMAND           echo command is used by default."    
    echo "  -i, --input                 An arguments from FILENAME file will be passed"
    echo "                              to the input stream of the specified command."
    echo "  -h, --help                  Prints this help message."
    echo "  -t THREADSNUM,              Executes command in parralell by THREADNUM commands" 
    echo "      --threads THREADSNUM    at once. A non-zero integer number is expected, 1 by default."     
}

while : 
do
    case "$1" in
        -c | --cmd)
            command="$2"
            shift 2
            ;;

        -i | --input)
            input=true
            shift
            ;;

        -t | --threads)
            threads="$2"
            shift 2
            ;;

        -h | --help)
            usage
            exit 0
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

if [ $# = 0 ] ; then echo "Error: not enough arguments. A file name is expected." >&2 ; exit 1 ; fi
if [ $# -gt 1 ] ; then echo "Error: too many arguments. A file name is expected." >&2 ; exit 1 ; fi
if [ -e "$1" ] ; then args_file="$1" ; else echo "Error: file $1 does not exist" >&2 ; exit 1 ; fi             
if [ -z "$command" ] ; then command="echo" ; fi
if [ -z "$threads" ] ; then threads=1 ; fi

counter=0
while read line ; do 
    counter=$((counter+1))
    if [ $((conter%threads)) = 0 ] ; then 
        if [ "$input" = true ] ; then
            $command < $line &
        else
            $command $line &
        fi    
    else 
        wait
    fi
done < $args_file
wait    






