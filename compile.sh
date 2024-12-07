c_files=$(find . -name "*.c" ! -name "test.c")
gcc -o main.out -w $c_files -pthread -lrt