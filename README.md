## Distributed Has Table
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
## Commands
There are 5 available operations. 
PUT: 0 <key> <value>: stores a value on one of the storage nodes. They key is an integer between 1 and 1000.
GET: 1 <key>: returns the value associated with the key. If not found, -1000 returned.
ADD: 2 <rank> <id>: adds the node of the given rank to the cluster, assigning the given id. The data will be redistributed to make up for the addition.
REMOVE: 3 <id>: removed the node with the specified id from the dht. The data is trasnferred to neighboring nodes.
END: 4: terminates the program.

For the examples how the commands are called, have a look at command.c.



