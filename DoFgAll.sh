#!/bin/bash

if [[ $# != 2 ]] ; then
    echo 'Argument should be: path, number of particles'
    exit 1
fi

HOME=$(pwd)
path=$1
AWKCOMMAND="awk -f $HOME/convlmppos.awk unwrappedDump-2500000.txt"

cd $path
LIMIT=100
rm -f "$HOME/filenames.txt"
echo "$LIMIT" >> "$HOME/filenames.txt"

for (( i=1; i<=$LIMIT; i++ ))
do
    dname="run-$i"

    cd $dname
    echo $(pwd)
    $AWKCOMMAND > tmpp.txt
    sort -k1 --numeric tmpp.txt | awk '{
    print  $2," ",$3," ",$4," ",$5," 0 0 0" ;

    }' > position.txt

    echo "$path/$dname/position.txt" >> "$HOME/filenames.txt"

    cd ..
done

cd $HOME

./analyzer.exe $2 1.2 0.2
gnuplot plot.gnu
mv *.txt $path
mv *.png $path