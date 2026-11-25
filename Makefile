Main: ./src/main.c perceptron.o dataset.o
	gcc -g ./src/main.c ./output/perceptron.o ./output/dataset.o -o ./output/main -Wall -Wextra -lm


dataset.o: ./src/dataset.c ./Include/dataset.h
	gcc -g -Wall -Wextra ./src/dataset.c -c -o ./output/dataset.o

perceptron.o: ./src/perceptron.c ./Include/perceptron.h
	gcc -g -Wall -Wextra ./src/perceptron.c -c -o ./output/perceptron.o


clean:
	rm -f ./output/*