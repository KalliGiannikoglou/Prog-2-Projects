CC= gcc
CCFLAGS= -Wall -g
OBJ = objdb.o project3.o

project3: $(OBJ)
	$(CC) $(OBJ) -o project3 

objdb.o: objdb.c objdb.h
	$(CC) $(CCFLAGS) -c $<

project3.o: project3.c objdb.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o

