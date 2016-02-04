set term png
set terminal png enhanced size 1280, 1024
filename = 'S_Rend2_Rg2_Pcont_lnS_lnRg2_lnPcont_Rg.txt'

set output "ContactMatrix.png"
plot 'ContactMatrix.txt' matrix with image

set output "DistanceMatrix.png"
plot 'DistanceMatrix.txt' matrix with image

set xlabel "s"
set ylabel "REnd2"
set output "Rend2.png"
plot filename using 1:2 with linespoints

set output "Rg2.png"
set ylabel "Rg2"
plot filename using 1:3 with linespoints

set output "PCont.png"
set ylabel "PCont"
plot filename using 1:4 with linespoints

set output "lnRg.png"
set xlabel "s"
set ylabel "Rg"
set logscale xy
set xrange [1:128]
plot filename using 1:8 with linespoints, x**(0.33)*0.5, x**(0.5)*0.5

set output "lnPCont.png"
set xlabel "s"
set ylabel "PCont"
set logscale xy
plot filename using 1:4 with linespoints, 1/x, x**(-1.5)
