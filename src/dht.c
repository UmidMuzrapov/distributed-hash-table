/**
* @author Umid Muzrapov
 * Instructor: David Lowenthal
*
* Description: The program implements a simplified distributed hash table (dht).
 * The dht supports 5 key operations:
 * PUT 0 <key> <value>
 * GET 1 <key>
 * ADD 2 <rank> <id>
 * REMOVE 3 <id>
 * EDN 4
 *
 * Operational Requirements:
 * C99
 *  stdio
 *  stdlib
 *  dht.h
 *  dht-helper.h
 *  mpi
 *  3 or more processes.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "dht.h"
#include "dht-helper.h"

// constant whose values are determined at initialization
static int NUM_P, HEAD = 0, PERMANENT_S, COMMAND, ignore = 0;
// used to encapsulate a list_node's data
static StorageNode *node_data;

// on an END message, the head list_node is to contact all storage nodes and tell them
void head_end()
{
    int i, dummy;

    // the head list_node knows there is an END message waiting to be received because
    // we just called MPI_Probe to peek at the message.  Now we just receive it.
    MPI_Recv(&dummy, 1, MPI_INT, NUM_P - 1, END, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // tell all the storage nodes to END
    // the data sent is unimportant here, so just send a dummy value
    for (i = 1; i < NUM_P - 1; i++)
    {
        MPI_Send(&dummy, 1, MPI_INT, i, END, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    exit(0);
}

// on an END message, a storage list_node just calls MPI_Finalize and exits
void storage_end()
{
    int dummy;  // the data is unimportant for an END

    MPI_Recv(&dummy, 1, MPI_INT, 0, END, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Finalize();
    exit(0);
}

/**
 * end_node terminates the program.
 */
void end_node()
{
    if (node_data->rank == 0)
    {
        head_end();
    } else
    {
        storage_end();
    }
}

/**
 * add_node adds the list_node of the specified rank to the DHT , giving it the specified id.
 * Assumption: all ids will be positive integers.
 */
void add_node()
{
    int *input = malloc(sizeof(int) * 2);
    int new_id, new_rank;
    check_malloc(input);

    MPI_Recv(input, 2, MPI_INT, node_data->parent, ADD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    new_rank = input[0];
    new_id = input[1];

    if (new_id > node_data->id) // refer to the next
    {
        MPI_Send(input, 2, MPI_INT, node_data->child, ADD, MPI_COMM_WORLD);
    } else // found its child
    {
        // transfer key value pairs
        int count = get_less_count(node_data->data, new_id + 1);
        // number of ints needs = 2* number of key-value pairs
        int blockSize = count * 2;
        int *keyValues = malloc(sizeof(int) * blockSize);
        get_less(node_data->data, new_id + 1, keyValues);
        MPI_Send(keyValues, blockSize, MPI_INT, new_rank, DATA_TRANSFER, MPI_COMM_WORLD);
        MPI_Recv(&ignore, 1, MPI_INT, new_rank, CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        delete_range(&(node_data->data), keyValues, count);

        // activate a new list_node
        int *output = malloc(sizeof(int) * 2);
        output[0] = node_data->rank;
        output[1] = new_id;

        MPI_Send(output, 2, MPI_INT, new_rank, ACTIVATION, MPI_COMM_WORLD);
        MPI_Recv(&ignore, 1, MPI_INT, new_rank, CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // update the child pointer of the parent list_node
        MPI_Send(&new_rank, 1, MPI_INT, node_data->parent, UPDATE_CHILD, MPI_COMM_WORLD);
        MPI_Recv(&ignore, 1, MPI_INT, node_data->parent, CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // send around the ring that the request is complete
        MPI_Send(&ignore, 1, MPI_INT, node_data->child, ACK, MPI_COMM_WORLD);
    }
}

/**
 * remove_node removes the list_node with the specified id from the DHT
 */
void remove_node()
{
    int target = 0;
    MPI_Recv(&target, 1, MPI_INT, node_data->parent, REMOVE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (target == node_data->id)
    {
        int count = size(node_data->data);
        int block_size = count * 2;
        int *key_values = malloc(sizeof(int) * block_size);
        get_data(node_data->data, key_values);

        // transfer keys to the child
        MPI_Send(key_values, block_size, MPI_INT, node_data->child, DATA_TRANSFER, MPI_COMM_WORLD);
        MPI_Recv(&ignore, 1, MPI_INT, node_data->child, CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        delete_range(&(node_data->data), key_values, count);

        // update the parent child pointer to the current list_node's child
        MPI_Send(&(node_data->child), 1, MPI_INT, node_data->parent, UPDATE_CHILD, MPI_COMM_WORLD);
        MPI_Recv(&ignore, 1, MPI_INT, node_data->parent, CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // send a confirmation that the request has been successful
        MPI_Send(&ignore, 1, MPI_INT, node_data->child, ACK, MPI_COMM_WORLD);
    } else
    {
        MPI_Send(&target, 1, MPI_INT, node_data->child, REMOVE, MPI_COMM_WORLD);
    }

}

/**
 * put_key_value stores a value on one of the storage nodes. The key will always be between 1 and 1000, inclusive.
 */
void put_key_value()
{
    int *key_value = malloc(sizeof(int) * 2);
    MPI_Recv(key_value, 2, MPI_INT, node_data->parent, PUT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int key = key_value[0];

    if (node_data->id >= key)
    {
        // add the key-value pair to the list_node's list
        add(&(node_data->data), key_value);
        // send confirmation that the request is complete
        MPI_Send(&ignore, 1, MPI_INT, node_data->child, ACK, MPI_COMM_WORLD);

    } else // if id < key, send to the next list_node
    {
        MPI_Send(key_value, 2, MPI_INT, node_data->child, PUT, MPI_COMM_WORLD);
    }
}

/**
 * get_key_val retrieves the value from the correct storage list_node and sends the value and the storage of the list_node to the head list_node.
 */
void get_key_val()
{
    int *argsAdd;
    int key, value;

    // receive the GET message
    // note that at this point, we've only called MPI_Probe, which only peeks at the message
    // we are receiving the key from whoever sent us the message
    MPI_Recv(&key, 1, MPI_INT, node_data->parent, GET, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (key <= node_data->id)
    {  // I own this key/value pair
        // allocate two integers: the first will be the value, the second will be this storage id
        argsAdd = (int *) malloc(2 * sizeof(int));

        value = get_value(node_data->data, key);

        argsAdd[0] = value;
        argsAdd[1] = node_data->id;
        // send this value around the ring
        MPI_Send(argsAdd, 2, MPI_INT, node_data->child, RETVAL, MPI_COMM_WORLD);
    } else
    {  // I do NOT own this key/value pair; just forward request to next hop
        MPI_Send(&key, 1, MPI_INT, node_data->child, GET, MPI_COMM_WORLD);
    }
}

/**
 * retval notifies that the get_value command has been successful.
 * It does so in the ring pattern.
 */
void retval()
{
    int *result = malloc(sizeof(int) * 2);
    MPI_Recv(result, 2, MPI_INT, node_data->parent, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (node_data->rank == HEAD) // if head, send the message of the request completion to the command list_node
    {
        MPI_Send(result, 2, MPI_INT, COMMAND, RETVAL, MPI_COMM_WORLD);
    } else MPI_Send(result, 2, MPI_INT, node_data->child, RETVAL, MPI_COMM_WORLD);
}

/**
 * ack notifies that the put, add, and remove commands have been successful.
 * The communication flows from one to another until it is sent to the command list_node.
 */
void ack()
{
    MPI_Recv(&ignore, 1, MPI_INT, node_data->parent, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (node_data->rank == HEAD) // if head, send the message of the request completion to the command list_node
    {
        MPI_Send(&ignore, 1, MPI_INT, COMMAND, ACK, MPI_COMM_WORLD);
    } else MPI_Send(&ignore, 1, MPI_INT, node_data->child, ACK, MPI_COMM_WORLD);
}

/**
 * The function activates a storage list_node.
 */
void activate()
{
    int *input = malloc(sizeof(int) * 2);
    // wait for the activation signal
    MPI_Recv(input, 2, MPI_INT, node_data->parent, ACTIVATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // set my child pointer and id
    node_data->child = input[0];
    node_data->id = input[1];
    // send the confirmation of the successful completion of the request to the source list_node
    MPI_Send(&ignore, 1, MPI_INT, node_data->parent, CONTINUE, MPI_COMM_WORLD);
}

/**
 * The function helps to transfer data from one list_node to another.
 * @param block_size
 */
void transfer_data(int block_size)
{
    int *key_value_pairs = malloc(sizeof(int) * block_size);
    // transfer key value pairs to this list_node from the source
    // number of key/value pairs = block_size/2
    MPI_Recv(key_value_pairs, block_size, MPI_INT, node_data->parent, DATA_TRANSFER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < block_size / 2; i++)
    {
        int *keyValue = malloc(sizeof(int) * 2);
        keyValue[0] = key_value_pairs[2 * i];
        keyValue[1] = key_value_pairs[2 * i + 1];
        add(&(node_data->data), keyValue);
    }

    // send the confirmation of the successful completion of the request to the source list_node
    MPI_Send(&ignore, 1, MPI_INT, node_data->parent, CONTINUE, MPI_COMM_WORLD);
}

/**
 * This function allows the list_node to update its child list_node pointer as nodes are added or removed.
 */
void update_child()
{
    MPI_Recv(&(node_data->child), 1, MPI_INT, node_data->parent, UPDATE_CHILD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&ignore, 1, MPI_INT, node_data->parent, CONTINUE, MPI_COMM_WORLD);
}

// handle_messages repeatedly gets messages and performs the appropriate action
void handle_messages()
{
    MPI_Status status;
    int count, tag;

    while (1)
    {

        // peek at the message
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // skeleton doesn't use MPI_Get_count, but you will need to---
        // this call just gets how many integers are in the message
        MPI_Get_count(&status, MPI_INT, &count);
        // get_value the source and the tag---which MPI rank sent the message, and
        // what the tag of that message was (the tag is the command)
        node_data->parent = status.MPI_SOURCE;
        tag = status.MPI_TAG;


        // now take the appropriate action
        switch (tag)
        {
            case END:
                end_node();
                break;
            case ADD:
                add_node();
                break;
            case REMOVE:
                remove_node();
                break;
            case PUT:
                put_key_value();
                break;
            case GET:
                get_key_val();
                break;
            case ACK:
                ack();
                break;
            case RETVAL:
                retval();
                break;
            case ACTIVATION:
                activate();
                break;
            case DATA_TRANSFER:
                transfer_data(count);
                break;
            case UPDATE_CHILD:
                update_child();
                break;
            default:
                printf("ERROR, my id is %d, source is %d, tag is %d, count is %d\n", node_data->rank, node_data->parent, tag,
                       count);
                exit(1);
        }
    }
}

/**
 * This function initializes the distributed hash table.
 * Initially, only three nodes are active -- head, command and permanent storage nodes.
 * @param argc
 * @param argv
 */
void initialize(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
    node_data = malloc(sizeof(StorageNode));
    node_data->data = NULL;
    check_malloc(node_data);

    // get_value my rank and the total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &(node_data->rank));
    MPI_Comm_size(MPI_COMM_WORLD, &NUM_P);
    PERMANENT_S = NUM_P - 2;
    COMMAND = NUM_P - 1;


    // set up the head list_node and the last storage list_node
    if (node_data->rank == HEAD)
    {
        node_data->id = 0;
        node_data->child = PERMANENT_S;
    } else if (node_data->rank == PERMANENT_S)
    {
        node_data->id = MAX;
        node_data->child = HEAD;
    }
}

/**
 * This is the entry point of each process.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    initialize(&argc, &argv);

    // the command list_node is handled separately
    if (node_data->rank < COMMAND)
        handle_messages();
    else command_node();

    return 0;
}

