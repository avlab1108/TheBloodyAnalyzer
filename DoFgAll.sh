#!/bin/bash

if [[ $# -eq 0 ]] ; then
    echo 'Argument with folder required'
    exit 1
fi

PWD=$(pwd)
AWKCOMMAND="awk -f $PWD/convlmppos.awk unwrappedDump-2500000.txt"

cd $1

for i in {1..1..1}
do
dname="run-"$i

cd $dname
echo $(pwd)
$AWKCOMMAND > tmpp.txt
sort -k1 --numeric tmpp.txt | awk '{
print  $2," ",$3," ",$4," ",$5," 0 0 0" ;

}' > position.txt

cd ..
done

cd $PWD

# python positionFinder.py $2 $1 1.2 1