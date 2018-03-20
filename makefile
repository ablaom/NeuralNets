main: main.o neuralnet.o instant_input.o consoleinput.o 
	g++ main.o neuralnet.o instant_input.o consoleinput.o -o nn

main.o: main.cpp neuralnet.h instant_input.h consoleinput.h
	g++ -c main.cpp

neuralnet.o: neuralnet.h neuralnet.cpp
	g++ -c neuralnet.cpp

instant_input.o: instant_input.h instant_input.cpp
	g++ -c instant_input.cpp

consoleinput.o: consoleinput.h consoleinput.cpp
	g++ -c consoleinput.cpp

clean:
	rm *.o


