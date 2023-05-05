
enum direction{North = 0, South = 1, None = 2};
enum type{Car = 0, Van = 1};

struct Node {
    struct Node *next;
    char *name;
};

struct Lane{
    int lane_number;
    int count;
    int open;
    enum direction dir;
};

struct ParameterList{
    int id;
    enum type type;
    enum direction dir;
    char *type_str;
    char *dir_str;
    int weight;
    struct Node *node_ptr;
};

struct Group{
    int vehicles;
    double n_s_percentage;
    int delay;
};