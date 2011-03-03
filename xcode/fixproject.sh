#!/bin/sh

    projectfile=`find -d . -name 'project.pbxproj'`
    projectdir=`echo *.xcodeproj`
    projectfile="${projectdir}/project.pbxproj"
    tempfile="${projectdir}/project.pbxproj.out"
    savefile="${projectdir}/project.pbxproj.mergesave"

    cat $projectfile | grep -v "<<<<<<< HEAD" | grep -v "=======" | grep -v "^>>>>>>> " > $tempfile
    cp $projectfile $savefile
    mv $tempfile $projectfile
