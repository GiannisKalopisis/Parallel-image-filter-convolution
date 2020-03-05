OBJECTS = AllFunctions.o Project_Parallhla.o
EXECUTABLE = project_parallhla
CC = gcc
MPIC = mpicc
FLAGS = -g -Wall

all: project_parallhla

project_parallhla: $(OBJECTS)
	$(MPIC) $(FLAGS) AllFunctions.o Project_Parallhla.o -o project_parallhla -lm -fopenmp

AllFunction.o: AllFunctions.c
	$(CC) $(FLAGS) -c AllFunctions.c

Project_Parallhla.o: Project_Parallhla.c
	$(MPIC) $(FLAGS) -c Project_Parallhla.c -fopenmp

exec:
	mpiexec -n 4 ./project_parallhla -r 2520 -c 1920 -i 1

mexec:
	mpiexec -f machines -n 4 ./project_parallhla -r 2520 -c 1920 -i 1

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
 
