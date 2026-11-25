Main: ./src/main.c perceptron.o dataset.o
	gcc -g main.c perceptron.o dataset.o -o ./output/main -Wall -Wextra


dataset.o: ./src/dataset.c ./Include/dataset.h
	gcc -g -Wall -Wextra ./src/dataset.c -c -o ./output/dataset.o


testdataset : ./src/dataset.c ./Include/dataset.h
	gcc -g -Wall -Wextra ./src/dataset.c -o ./output/dataset
	chmod a+rwx ./output/dataset

perceptron.o: ./src/perceptron.c ./Include/perceptron.h
	gcc -g -Wall -Wextra ./src/perceptron.c -c -o ./output/perceptron.o


clean:
	rm -f ./output/*