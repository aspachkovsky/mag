#!/usr/bin/env dash
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

[ $# = 0 ] && { echo "Error: not enough arguments. A file name is expected." >&2 ; exit 1 ; }
[ $# -gt 1 ] && { echo "Error: too many arguments. A file name is expected." >&2 ; exit 1 ; }
[ -r "$1" ] && args_file="$1" || { echo "Error: file $1 does not exist" >&2 ; exit 1 ; }             
[ -z "$command" ] && command="echo"
[ -z "$threads" ] && threads=1

counter=0
while read line ; do 
    if [ "$threads" = "1" ] ; then
        if [ "$input" = "true" ] ; then
            echo "$line" | $command
        else
            $command $line
        fi  
    else
        counter=$((counter+1))    
        if [ "$input" = "true" ] ; then
            echo "$line" | $command &
        else
            $command $line &
        fi   
        [ $((counter%threads)) = 0 ] && wait
    fi
done < $args_file
[ "$threads" -ne "1" ] && [ $((counter%threads)) = 0 ] && wait
  






