CFLAGS=-Wall -Wno-comment
EXE    = allocate
OBJ    = main.o Struct_manipulation.o Schedule_sim.o Challenge.o

allocate: main.o Struct_manipulation.o Schedule_sim.o Challenge.o
	gcc $(CFLAGS) -o allocate main.o Struct_manipulation.o Schedule_sim.o Challenge.o

main.o: main.c Struct_manipulation.h Schedule_sim.h Challenge.h
	gcc $(CFLAGS) -c main.c

Struct_manipulation.o: Struct_manipulation.c Struct_manipulation.h
	gcc $(CFLAGS) -c Struct_manipulation.c

Schedule_sim.o: Schedule_sim.c Schedule_sim.h
	gcc $(CFLAGS) -c Schedule_sim.c

Challenge.o: Challenge.c Challenge.h
		gcc $(CFLAGS) -c Challenge.c

.PHONY: clean
	rm -f $(OBJ) $(EXE)
