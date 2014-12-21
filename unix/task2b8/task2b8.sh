#!/usr/bin/env dash

updallfiles() {
    find . -maxdepth 1 -not -name ".*" -type f -print | colrm 1 2 > ".allfiles"  
}

updalldirs() {
    find . -maxdepth 1 -not -name ".*" -type d -print | colrm 1 2 > ".alldirs"  
}

where() {
    if [ ! -r ".alldirs" ] ; then 
        echo "Information about directories has not been collected or cannot be accessed."
        echo "Please run updalldirs command to update." 
    else
        grep --color=auto "$1" .alldirs
    fi
}

wtf() {
    if [ ! -r ".allfiles" ] ; then 
        echo "Information about file has not been collected or cannot be accessed."
        echo "Please run updallfiles command to update."
    else 
        grep --color=auto "$1" .allfiles
    fi
}
