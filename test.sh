#!/bin/bash

FILES=test/*.png

for f in $FILES
do
    file=`basename -s.png $f`
    ./gif $f out/${file}.gif
    diff `dirname $f`/${file}.gif out/${file}.gif
done
