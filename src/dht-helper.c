/**
 * @author Umid Muzrapov
 * This file contains a list functions for the dht.
 */

#include "dht-helper.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * The function checks that the memory has been allocated properly.
 * @param p
 */
void check_malloc(void *p)
{
    if (p == NULL)
    {
        printf("melloc failure. Exiting.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * get_less_count gets the number of elements less than the given key.
 * @param list dht list_node's key-value list
 * @param key key
 * @return number of elements less than the given key
 */
int get_less_count(Node *list, int key)
{
    Node *cur = list;
    int count = 0;

    while (cur != NULL)
    {
        if (cur->data->key < key) count++;
        cur = cur->next;
    }

    return count;
}

/**
 * get_less returns key-value pairs less than the given key as a contiguous block of memory.
 * @param list dht list_node's key-value list
 * @param key key
 * @param key_value a contiguous block of memory that contains key-value pairs
 */
void get_less(Node *list, int key, int *key_value)
{
    Node *cur = list;
    int i = 0;
    while (cur != NULL)
    {
        if (cur->data->key < key)
        {
            key_value[i] = cur->data->key;
            key_value[i + 1] = cur->data->value;
            i += 2; // Move to the next pair
        }

        cur = cur->next;
    }
}


/**
 * add adds a key-value pair to the list_node's key-value list.
 * @param list_p pointer to dht list_node's key-value list
 * @param key_value a contiguous block of memory that contains key-value pair
 */
void add(Node **list_p, const int *key_value)
{
    int key = key_value[0];
    int value = key_value[1];
    Node *newNode = malloc(sizeof(Node));
    check_malloc(newNode);
    newNode->data = malloc(sizeof(KeyValue));
    newNode->data->key = key;
    newNode->data->value = value;
    newNode->next = *list_p;
    *list_p = newNode;
}

/**
 * Delete the given key-values from the list
 * @param list_p pointer to the list
 * @param keyValuePairs key-value pairs to be removed
 * @param count number of key-value pair
 */
void delete_range(Node **list_p, int *keyValuePairs, int count)
{
    for (int i = 0; i < count; i++)
    {
        delete(list_p, keyValuePairs[i * 2]); // Passing the key to delete function
    }
}

/**
 * deletes a key from the the list_node's key-value list
 * @param list_p pointer to dht list_node's key-value list
 * @param key key
 */
void delete(Node **list_p, int key)
{
    Node *cur = *list_p;
    Node *prev = NULL;

    while (cur != NULL && cur->data->key != key)
    {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) // Key not found
    {
        return;
    }

    if (prev == NULL) // If it's the first list_node
    {
        *list_p = cur->next;
    } else
    {
        prev->next = cur->next;
    }

    free(cur->data); // Free memory occupied by data
    free(cur);       // Free memory occupied by the list_node
}


/**
 * Returns the number of key value pairs in a list
 * @param list dht list_node's key-value list
 * @return size of the list
 */
int size(Node *list)
{
    Node *cur = list;
    int count = 0;
    while (cur != NULL)
    {
        count++;
        cur = cur->next;
    }

    return count;
}

/**
 * Returns the key-value pairs of the list as a block of memory.
 * @param list list_node's key-value list
 * @param key_value_pairs a block of memory containing key-value pairs
 */
void get_data(Node *list, int *key_value_pairs)
{
    int i = 0;
    Node *cur = list;

    while (cur != NULL)
    {
        key_value_pairs[i] = cur->data->key;
        key_value_pairs[i + 1] = cur->data->value;
        cur = cur->next;
        i += 2; // move to the next pair
    }
}

/**
 * The function gets the value associated with the key.
 * @param list list_node's key-value list
 * @param key key
 * @return value mapped to the key
 */
int get_value(Node *list, int key)
{
    Node *temp = list;

    while (temp != NULL)
    {
        if (temp->data->key == key)
        {
            return temp->data->value;
        }
        temp = temp->next;
    }

    // not found. Should not happen based on the assignment conditions
    return -1000;
}