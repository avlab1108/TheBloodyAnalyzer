#!/bin/bash

cd D:/1024/globule/LAMMPS

for i in {1..100..1}
do
dname="run-"$i

cd $dname
awk -f D:/convlmppos.awk dump.2500000.txt > tmpp.txt
sort -k1 --numeric tmpp.txt | awk '{
print  $2," ",$3," ",$4," ",$5," 0 0 0" ;


}' > position.txt

cd ..
done

