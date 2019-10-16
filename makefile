OBJ=Thread.o ThreadPool.o main.o

main: $(OBJ) 
	g++ *.o -o $@ -std=c++11 -DDEBUG -lpthread 
Thread.o: Thread.cpp 
	g++ $^ -c -std=c++11 -DDEBUG
ThreadPool.o:ThreadPool.cpp 
	g++ $^ -c -std=c++11 -DDEBUG 
main.o: main.cpp 
	g++ $< -c -std=c++11 -DDEBUG


clean:
	rm -rf $(OBJ)
