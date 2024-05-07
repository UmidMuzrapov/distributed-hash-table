# Distributed Has Table
The project implements a simplified version of the distributed hash table with MPI in C. A distributed hash table is a decentralized distributed system that provides a logical key-value store where data is distributed across multiple nodes in the network.

![diagram-distributed-hash-table-800x404-1](https://github.com/UmidMuzrapov/distributed-hash-table/assets/63548446/0fae0c76-af2d-4523-ab4b-e70a1b213fe3)

_The image is taken from hazelcast.com_

## Installation 
Include dht.h in the user program. A sample user program, command.c, is given the source directory.
```#include "dht.h"```
Compile and run your program. This is an example Makefile:
```
dht:	dht.o dht-helper.o command.o
	mpicc -o dht dht.o dht-helper.o command.o

dht-helper.o:	dht-helper.c dht-helper.h
	mpicc -c -std=c99 dht-helper.c

dht.o:	dht.c dht-helper.h
	mpicc -c  -std=c99 dht.c

command.o:	command.c
	mpicc -c  -std=c99 command.c

clean:
	rm -f *.o dht 
```
## API




