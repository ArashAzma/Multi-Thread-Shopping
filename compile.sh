# c_files=$(find . -name "*.c" ! -name "test.c")
# sudo gcc -o main.out -w $c_files -pthread -lrt lraylib -lGL -lm -lpthread -ldl -lX11

# gcc utils/graphic.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

c_files=$(find . -name "*.c" ! -name "test.c")
sudo gcc -o main.out -w $c_files -pthread -lraylib -lGL -lm -lpthread -ldl -lX11