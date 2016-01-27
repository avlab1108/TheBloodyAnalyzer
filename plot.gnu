set term png
set terminal png enhanced size 1280, 1024

set output "ContactMatrix.png"
plot 'ContactMatrix.txt' matrix with image

set output "DistanceMatrix.png"
plot 'DistanceMatrix.txt' matrix with image


set xlabel "s"
set ylabel "REnd2"
set output "Rend2.png"
plot 'S_Rend2_Rg2_Pcont_lnS_lnRg2_lnPcont.txt' using 0:1 with linespoints

set output "Rg2.png"
set ylabel "Rg2"
plot 'S_Rend2_Rg2_Pcont_lnS_lnRg2_lnPcont.txt' using 0:2 with linespoints

set output "s_PCont.png"
set ylabel "PCont"
plot 'S_Rend2_Rg2_Pcont_lnS_lnRg2_lnPcont.txt' using 0:3 with linespoints

set output "lnRg2.png"
set xlabel "ln(s)"
set ylabel "ln(Rg2)"
plot 'S_Rend2_Rg2_Pcont_lnS_lnRg2_lnPcont.txt' using 4:5 with linespoints

