main.o: main.c config.h
	gcc -c main.c
parse.o: parse.c config.h
	gcc -c parse.c
buildin.o: buildin.c config.h
	gcc -c buildin.c
exec.o: exec.o config.h
	gcc -c exec.c
shell: main.o parse.o ui.o buildin.o exec.o 
	gcc ui.o main.o parse.o buildin.o exec.o -o shell -lreadline	

.PHONY : clean
clean:
	rm shell 
	rm *.o 

