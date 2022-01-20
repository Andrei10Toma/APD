procs=12

all: build run

build:
	mpicc test2.c -o testapd2

run:
	mpirun -np ${procs} --oversubscribe ./testapd2

clean:
	rm testapd2