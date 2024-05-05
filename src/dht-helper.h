
struct key_value{
    int key;
    int value;
};

struct list_node{
    struct list_node *next;
    struct key_value *data;
};

struct storage_node_data {
    struct list_node *data;
    int child;
    int id;
    int rank;
    int parent;
};

typedef struct list_node Node;
typedef struct storage_node_data StorageNode;
typedef struct key_value KeyValue;

void add(Node **list_p, const int* key_value);
void delete(Node **list_p, int key);
int get_value(Node *list, int key);
void check_malloc(void* p);
int get_less_count(Node *list, int key);
void get_less(Node *list, int key, int* key_value);
int size(Node *list);
void get_data(Node *list, int* key_value_pairs);
void delete_range(Node **list_p, int*  key_value_pairs, int count);

