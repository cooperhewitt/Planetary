#!/bin/sh

# See also:
# http://linux.die.net/man/1/enscript

PYTHON=`which python`
ENSCRIPT=`which enscript`
PSTOPDF=`which pstopdf`

WHOAMI=`${PYTHON} -c 'import os, sys; print os.path.realpath(sys.argv[1])' $0`
BIN=`dirname ${WHOAMI}`
ROOT=`dirname ${BIN}`

PDF=$1

if [ -z $PDF ]
then
    PDF="${ROOT}/planetary.pdf"
fi

if [ -f $PDF ]
then
    echo "${PDF} already exists, removing in 5 seconds..."
    sleep 5
    rm ${PDF}
fi

echo "writing source code to ${PDF}"

# Note that we are excluding blocks/CinderFlurry/lib/FlurryLib/libFlurry.a

${ENSCRIPT} -v --toc --margins=30:30:50:50 --non-printable-format=octal -p - --line-numbers --header='6814.1.2013|$N|$%/$=' --file-align=2 --word-wrap -O -F 'Courier6' -f 'Courier8' ${ROOT}/src/* ${ROOT}/blocks/BloomScene/include/* ${ROOT}/blocks/BloomScene/src/* ${ROOT}/blocks/BloomTasks/src/* ${ROOT}/blocks/CinderFlurry/include/* ${ROOT}/blocks/CinderFlurry/lib/FlurryLib/*.h ${ROOT}/blocks/CinderFlurry/src/* ${ROOT}/blocks/CinderGestures/include/* ${ROOT}/blocks/CinderIPod/include/* ${ROOT}/blocks/CinderIPod/src/* ${ROOT}/blocks/CinderOrientation/include/* ${ROOT}/blocks/CinderOrientation/src/*  | ${PSTOPDF} -i -o ${PDF}

exit
