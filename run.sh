export OMP_NUM_THREADS=4
echo Number of Threads: $OMP_NUM_THREADS
gcc main.c -fopenmp -lcrypt -finput-charset=UTF-8 -fextended-identifiers -o main.exe 
./main.exe