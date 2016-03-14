#!/bin/bash

if [[ $# != 2 ]] ; then
    echo 'Argument should be: path, number of particles'
    exit 1
fi

HOME=$(pwd)
DATA_PATH=$1

cd $DATA_PATH
LIMIT=200
rm -f "$HOME/filenames.txt"
COUNT=0
POSITIONFILE="position.txt"
TEMP="stats-1.4"

for (( i=1; i<=$LIMIT; i++ ))
do
    DNAME="run-$i"
    if [ -d $DNAME ]; then
        cd $DNAME
        echo $(pwd)

        if [ ! -f $DATA_PATH/$DNAME/$POSITIONFILE ]; then
            LAST_DUMP=$(ls -la dump*.txt | sort -k7 | tail -1 | awk '{print $9;}' )
            awk -f $HOME/convlmppos.awk $LAST_DUMP > tmpp.txt
            sort -k1 --numeric tmpp.txt | awk '{
            print  $2," ",$3," ",$4," ",$5," 0 0 0" ;
            }' > position.txt
        fi

        if [ -f $DATA_PATH/$DNAME/$POSITIONFILE ]; then
           COUNT=`expr $COUNT + 1`
           echo "$DATA_PATH/$DNAME/$POSITIONFILE " >> "$HOME/filenames.txt"
        fi
        cd ..
    fi
done

sed -i "1s/^/$COUNT\n/" "$HOME/filenames.txt"

head "$HOME/filenames.txt"

cd $HOME

./analyzer.exe $2 1.4 10.0

echo "plot graph"
echo $HOME

gnuplot -e "chainLength=$2" plot.gnu

mkdir -p $DATA_PATH/$TEMP
mv *.txt $DATA_PATH/$TEMP
mv *.png $DATA_PATH/$TEMP
