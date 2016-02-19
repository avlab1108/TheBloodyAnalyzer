#!/bin/bash

if [[ $# != 2 ]] ; then
    echo 'Argument should be: path, number of particles'
    exit 1
fi

HOME=$(pwd)
DATA_PATH=$1
AWKCOMMAND="awk -f $HOME/convlmppos.awk dump.3000000.txt"

cd $DATA_PATH
LIMIT=100
rm -f "$HOME/filenames.txt"
COUNT=0
POSITIONFILE="position_T_04_30.txt"
TEMP=04

for (( i=1; i<=$LIMIT; i++ ))
do
    DNAME="run-$i"
    if [ -d $DNAME ]; then
        COUNT=`expr $COUNT + 1`
        cd $DNAME
        echo $(pwd)
#        $AWKCOMMAND > tmpp.txt
#        sort -k1 --numeric tmpp.txt | awk '{
#        print  $2," ",$3," ",$4," ",$5," 0 0 0" ;
#
#        }' > position.txt

       [ -f $DATA_PATH/$DNAME/$POSITIONFILE ] && echo "$DATA_PATH/$DNAME/$POSITIONFILE " >> "$HOME/filenames.txt"
        cd ..
    fi
done

sed -i "1s/^/$COUNT\n/" "$HOME/filenames.txt"

head "$HOME/filenames.txt"

cd $HOME

./analyzer.exe $2 1.2 0.2

echo "plot graph"
echo $HOME

gnuplot -e "chainLength=$2" plot.gnu

mkdir -p $DATA_PATH/$TEMP
mv *.txt $DATA_PATH/$TEMP
mv *.png $DATA_PATH/$TEMP
