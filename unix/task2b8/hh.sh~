#!/bin/bash

updallfiles() {
    cat /dev/null > $ALLFILES
    for entry in * ; do
        if [ -f "$entry" ] ; then echo "$entry" >> $ALLFILES ; fi
    done
}

updalldirs() {
    cat /dev/null > $ALLDIRS
    for entry in * ; do
        if [ -d "$entry" ] ; then echo "$entry" >> $ALLDIRS ; fi
    done
}

where() {
    getmatch() { start=$1 ; len=$2 ; }
    if [ ! -e $ALLDIRS ] ; then 
        echo "Information about directories has not been collected."
        echo "Please run updalldirs command to update." ; exit 1
    fi

    while read line ; do              
        getmatch $(echo "$line" | awk 'match($0, /'$1'/) {print RSTART,RLENGTH}')
        if [ ! -z $start ] && [ ! -z $len ] ; then       
            echo "${line:0:$((start-1))}$(tput setaf 1)${line:$((start-1)):$len}$(tput sgr0)${line:$((start+len-1))}"
        else
            echo "$line"
        fi 
    done < ".alldirs"
}

wtf() {
    getmatch() { start=$1 ; len=$2 ; }
    if [ ! -e $ALLFILES ] ; then 
        echo "Information about file has not been collected."
        echo "Please run updallfiles command to update." ; exit 1
    fi
 
    while read line ; do              
        getmatch $(echo "$line" | awk 'match($0, /'$1'/) {print RSTART,RLENGTH}')
        if [ ! -z $start ] && [ ! -z $len ] ; then       
            echo "${line:0:$((start-1))}$(tput setaf 1)${line:$((start-1)):$len}$(tput sgr0)${line:$((start+len-1))}"
        else
            echo "$line"
        fi 
    done < ".allfiles"
}

export -f updallfiles
export -f updalldirs
export -f where
export -f wtf
