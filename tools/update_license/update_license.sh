#!/bin/bash
# run this script from the root directory
if [ ! -f $PWD/LICENSE ] ; then
    echo "you have to run the script from the cmakels or lscpp root directory"
    exit -1
fi

execludelist=(\
    )

for file in `find . -regextype posix-egrep -regex ".*\.(h|hpp|cpp|cu|c|cpp.in)$" -not -path "*/build*" -not -path "./external/*" -not -path "./.cmakels/*"`; do

    if [[ ! " ${execludelist[@]} " =~ " ${file} " ]]; then # if not in exclude list
        if ! grep -q "Copyright " $file ; then
            echo $file
            if [ "$1" == "fix" ]; then
                echo "$(cat $PWD/../tools/update_license/HEADER)
$(cat $file)" > $file
            fi
            else # if the license matches the string above
            if [ "$1" == "update" ]; then
                sed -i '0,/\*\//d' $file
                echo "$(cat $PWD/../tools/update_license/HEADER)
$(cat $file)" > $file
            fi
        fi
    fi
done
