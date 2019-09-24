#!/bin/bash
# run this script from the root directory
if [ ! -f $PWD/tools/check_license/LICENSE_HEADER ] ; then
    echo "you have to run the script ./tools/check_license/check_license.sh from the root of the sources"
    exit -1
fi

execludelist=(\
    )

for file in `find . -regextype posix-egrep -regex ".*\.(h|hpp|cpp|cu|c|cpp.in)$" -not -path "./build*" -not -path "./install*" -not -path "./tools/*" -not -path "./libs/*"`; do

    if [[ ! " ${execludelist[@]} " =~ " ${file} " ]]; then # if not in exclude list
        if ! grep -q "Copyright (c) " $file ; then
            echo $file
            if [ "$1" == "fix" ]; then
                echo "$(cat tools/check_license/LICENSE_HEADER)
$(cat $file)" > $file
            fi
            else # if the license matches the string above
            if [ "$1" == "update" ]; then
                sed -i '0,/\*\//d' $file
                echo "$(cat tools/check_license/LICENSE_HEADER)
$(cat $file)" > $file
            fi
        fi
    fi
done
