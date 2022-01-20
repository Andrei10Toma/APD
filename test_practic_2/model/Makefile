procs=6

build:
	mpicc model.c -o model

run:
	mpirun -np ${procs} --oversubscribe ./model

clean:
	rm model