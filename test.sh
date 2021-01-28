gcc DLA_seriale.c -o dla_seriale
gcc DLA_openmp.c -o dla_openmp -fopenmp
gcc DLA_pthread.c -o dla_pthread -lpthread
for parti in $(($1 * 10)) $(($1 * 20)) $(($1 * 30)) $(($1 * 40)) $(($1 * 50));
do echo "#######################################################################"
for mosse in $1 $(($1 * 5)) $(($1 * 10));
do echo "-------------------------------------- X=$1   P=$parti   M=$mosse"
echo "seriale"
time ./dla_seriale $1 $parti $mosse
echo "open"
time ./dla_openmp $1 $parti $mosse 4
echo "pthread"
time ./dla_pthread $1 $parti $mosse 4
done;
done