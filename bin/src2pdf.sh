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

echo "writing source code to ${PDF}"

${ENSCRIPT} --toc -p - --line-numbers -B --file-align=2 --word-wrap -O -f 'Courier8' ${ROOT}/src/* | ${PSTOPDF} -i -o ${PDF}

exit
