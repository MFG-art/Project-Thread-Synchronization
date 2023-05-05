#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Duh! what do we need this for? ;)
#include <time.h> // Used to set random seed
#include<unistd.h> // contains linux sleep() method
#include <stddef.h>
#include <string.h> // string lib for string stuff
#include "structures.h"

// This is how to initialize locks and condition variables
pthread_mutex_t Lane1Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Lane2Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t WeightLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lane1list_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lane2list_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t north_queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t south_queue_lock = PTHREAD_MUTEX_INITIALIZER;


// Are the lanes open, what direction are they in? 
pthread_cond_t Lane1open = PTHREAD_COND_INITIALIZER;
pthread_cond_t Lane2open = PTHREAD_COND_INITIALIZER;
pthread_cond_t Lane1North = PTHREAD_COND_INITIALIZER;
pthread_cond_t Lane1South = PTHREAD_COND_INITIALIZER;
pthread_cond_t Lane2North = PTHREAD_COND_INITIALIZER;
pthread_cond_t Lane2South = PTHREAD_COND_INITIALIZER;
pthread_cond_t weightUnderLimit = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;


int weight = 0;
// Queues that contain references to parameters. Used to display what state a vehicle is in.
struct Node lane1_list, lane2_list, north_queue, south_queue;
struct Lane Lane1, Lane2;
/* This function follows a single vehicle on a selected lane. It uses the lock and condition variables for that specific lane*/
int choose_lane (struct Lane *lane, struct ParameterList *params){
    // the lane is in the direction of travel and open
    lane->dir = params->dir;
    lane->open = 1;
    
    // selecting locks based on Lane chosen
    pthread_mutex_t lock = lane->lane_number == 1 ? Lane1Lock : Lane2Lock;
    pthread_cond_t north_cond = lane->lane_number == 1 ? Lane1North : Lane2North;
    pthread_cond_t south_cond = lane->lane_number == 1 ? Lane1South : Lane2South;

    // ARRIVAL
    printf("%s #%d (%s) has arrived on Lane %d\n",params->type_str,params->id, params->dir_str, lane->lane_number);

    pthread_mutex_lock(&lock);


    // If Lane is not available, go to sleep
    while (weight + params->weight > 1200){
            pthread_cond_wait(&weightUnderLimit, &lock);
    }

    // try to see if the weight lock is free.
    pthread_mutex_lock(&WeightLock);
    weight += params->weight;
    pthread_mutex_unlock(&WeightLock);

    lane->count++;
    // Call a waiting lane before going to sleep
    if (params->dir == North){
        pthread_cond_signal(&north_cond);
    } else {
        pthread_cond_signal(&south_cond);
    }
    pthread_mutex_unlock(&lock);



    printf("%s #%d (%s) is crossing the bridge\n",params->type_str,params->id, params->dir_str);
    sleep(3);

    pthread_mutex_lock(&lock);

    weight -= params->weight;
    pthread_cond_signal(&weightUnderLimit);

    lane->count--;
    if (lane->count == 0){
        lane->dir = None;
        printf("*** Lane %d is now empty!\n", lane->lane_number);
        pthread_cond_signal(&empty);

    }

    pthread_cond_signal(&north_cond);
    pthread_cond_signal(&south_cond);
    pthread_mutex_unlock(&lock);

    printf("%s #%d (%s) has exited Lane %d\n",params->type_str,params->id, params->dir_str, lane->lane_number);

}


/* This function takes a look at both lanes and selects one for the incoming vehicle*/
void* vehicle_routine(void* args){
    struct ParameterList *params = (struct ParameterList*) args;

    // Key idea? How do you decide which lane a vehicle will go to?
    // First, check if either lane is unused
    // Check if both lanes are open and have matching direction of travel, choose lane with fewest vehicles.
    // If neither lane matches the direction of travel, close the lane with the fewest vehicles and change its direction of travel

    // FOR ARRIVAL: Car #6 (northbound) has arrived. // include space after and no newline?

    // Bridge status, waiting queue
    // Van #8 is now crossing the bridge.
    // Car #6 exited the bridge.
    



    if ((Lane1.dir == None) && Lane1.open == 1){    // is the first lane empty and open
        choose_lane(&Lane1, params);
    } else if ((Lane2.dir == None) && Lane2.open == 1){ // is the second lane empty and open
        choose_lane(&Lane2, params);
    } else if (Lane1.dir == params->dir &&  Lane1.open == 1 && Lane2.dir == params->dir && Lane2.open == 1){ // are both lanes in direction of travel?
        Lane1.count > Lane2.count ? choose_lane(&Lane2, params) : choose_lane(&Lane1, params);
    } else if (Lane1.dir == params->dir && Lane1.open == 1){ // we know at least lane 1 is the direction of travel
        choose_lane(&Lane1, params);
    } else if (Lane2.dir == params->dir && Lane2.open == 1){ // we know at least lane 2 is in the direction of travel
        choose_lane(&Lane2, params);
    }
    else if (((Lane1.dir != params->dir || Lane1.dir != None) && Lane1.open == 1) && ((Lane2.dir != params->dir || Lane2.dir != None) && Lane2.open == 1)){  // neither lane is in dir. of travel
        if (params->dir == North){ // direction is north, add to queue and wait.

            pthread_cond_wait(&Lane1North, &Lane1Lock);
        } else {                        // direction is south, add to queue and wait.

            pthread_cond_wait(&Lane1South, &Lane1Lock);
        }
        if (Lane1.count > Lane2.count) {
            Lane1.open == 0;
            printf("*** %s %d (%s) has closed off Lane 1\n", params->type_str, params->id, params->dir_str);
            pthread_mutex_lock(&Lane1Lock);
            while (Lane1.count > 0){
                pthread_cond_wait(&empty, &Lane1Lock);
            }
            choose_lane(&Lane1, params);
        } else {
            Lane2.open = 0;
            printf("*** %s %d (%s) has closed off Lane 2\n", params->type_str, params->id, params->dir_str);
            pthread_mutex_lock(&Lane2Lock);
            while (Lane2.count > 0){
                pthread_cond_wait(&empty, &Lane2Lock);
            }
            choose_lane(&Lane2, params);

        }

    } else if (Lane1.open == 0){ // if lane 1 is closed, it will be used for 
        printf("*** %s %d (%s) is waiting on Lane 1 to open\n", params->type_str, params->id, params->dir_str);
        pthread_mutex_lock(&Lane1Lock); 
        while (Lane1.open == 0){
            if (Lane1.count == 0){
                pthread_cond_signal(&empty);
            }
            if (params->dir == North){ // direction is north, add to queue and wait.
                pthread_cond_wait(&Lane1North, &Lane1Lock);
            } else {                        // direction is south, add to queue and wait.
                pthread_cond_wait(&Lane1South, &Lane1Lock);
            }
        }
        choose_lane(&Lane2, params);
    } else if (Lane2.open == 0) {       // lane two is closed, prepare to enter it once it opens
        printf("*** %s %d (%s) is waiting on Lane 2 to open\n", params->type_str, params->id, params->dir_str);
        pthread_mutex_lock(&Lane2Lock); 
        while (Lane2.open == 0){
            if (Lane2.count == 0){
                    pthread_cond_signal(&empty);
            }
            if (params->dir == North){ // direction is north, add to queue and wait.
                pthread_cond_wait(&Lane2North, &Lane2Lock);
            } else {                        // direction is south, add to queue and wait.
                pthread_cond_wait(&Lane2South, &Lane2Lock);
            }
        }
        choose_lane(&Lane2, params);

    }

    return 0;
}

// This value returns a value that can be assigned to an enum. Percentage determines probability of enum = 0. Othersi
int random_value(float percentage){
    int random =  rand() % 100; // Returns a random value between 0 and 100
    int percent = percentage * 100;
    int output;
    if (random < percent){
        output = 0;
    } else {
        output = 1;
    }
    return output;
}
/* Main function. Set up lane structs and local variables.  */
int main(int argc, char *argv[]){
    srand(time(NULL));      // randomize the random seed
    // setting up the Lane structures. Lanes have 0 vehicles, no assigned direction and are open
    Lane1.count = 0;
    Lane1.dir = None;
    Lane2.count = 0;
    Lane2.dir = None;
    Lane1.open = 1;
    Lane2.open = 1;
    Lane1.lane_number = 1;
    Lane2.lane_number = 2;

    int vehicle_id = 1;
    int total_threads = 0;

    /*
    Do we need a while loop? YES
    Number of groups:
    for each group: get number of vehicles, north/south probability, delay

    There is always a 50/50 probability of a vehicle being a car or a van.

    For each car called, create a thread using vehicle_routine
    */
    if (argc < 2){
        char *buffer;
        size_t size = 10;
        buffer = malloc(size * sizeof(char));
        printf("Please enter how many groups of vehicles are there? (Enter a number): ");
        getline(&buffer, &size, stdin);
        int groups = atoi(buffer);

        struct Group group_list[groups];

        // Terminal Loop. Get vehicle group info
        for (int i = 0; i < groups; i++){
            printf("How many vehicles are there in group %d? (Enter a number): ", i+1);
            getline(&buffer, &size, stdin);
            group_list[i].vehicles = atoi(buffer);
            total_threads += group_list[i].vehicles; // add a thread for each vehicle
            printf("Enter the north/southbound percentage for group %d (Enter as a decimal): ", i+1);
            getline(&buffer, &size, stdin);
            group_list[i].n_s_percentage = atof(buffer);
            printf("Enter the delay, in seconds. For no delay, type 0: ");
            getline(&buffer, &size, stdin);
            group_list[i].delay = atoi(buffer);
        }
        // array containing all threads
        pthread_t threads[total_threads];
        struct ParameterList* new_vehicles[groups]; 

    // Take group info and generate vehicles
 
        for (int i = 0; i < groups; i++){
            new_vehicles[i] = malloc(group_list[i].vehicles * sizeof(struct ParameterList));
            for (int j = 0; j < group_list[i].vehicles; j++){
                // create an array of parameter lists. One goes out per thread
                
                new_vehicles[i][j].id = vehicle_id++;
                new_vehicles[i][j].type = random_value(0.5);
                new_vehicles[i][j].dir = random_value(group_list[i].n_s_percentage);
                new_vehicles[i][j].weight = new_vehicles[i][j].type == Car ? 200 : 300; // Assign weight if car or van
                // we can initialize the node item used in the linked lists here.
                // We will only ever change the prev, next pointers from now on.

                // Used for printing purposes. Better to store this info here than to repeatedly run this logic
                if (new_vehicles[i][j].dir == North){
                    new_vehicles[i][j].dir_str = "North";
                } else {
                    new_vehicles[i][j].dir_str = "South";
                }

                if (new_vehicles[i][j].type == Car) {
                    new_vehicles[i][j].type_str = "Car";
                } else {
                    new_vehicles[i][j].type_str = "Van";
                }

                // Initialize the thread here.
                pthread_create(&threads[vehicle_id-2], NULL, vehicle_routine, (void *)&new_vehicles[i][j]);

            }
            // Here, we can print the bridge status and waiting queue
            // printf("Sleeping for %d seconds.\n",group_list[i].delay);

            sleep(group_list[i].delay);
        }
        for (int i = 0; i < total_threads; i++){
            pthread_join(threads[i], NULL);
        }

    } else { // first argument is groups
        int groups = atoi(argv[1]);
        int group_size;
        double group_dir_percent;
        int group_delay;
        int total_threads = 0;
        for (int i = 2; i < argc; i = i + 3){
            total_threads += atoi(argv[i]);
        }
        pthread_t threads[total_threads];

    
        struct ParameterList* new_vehicles[groups]; 

        for (int i = 0; i < groups; i++){
           
            group_size = atoi(argv[(3*i) + 2]);
            double group_dir_percent = atof(argv[(3*i) + 3]);
            int group_delay = atoi(argv[(3*i) + 4]);

            new_vehicles[i] = malloc(group_size * sizeof(struct ParameterList));

            for (int j = 0; j < group_size; j++){
                // create an array of parameter lists. One goes out per thread
                
                new_vehicles[i][j].id = vehicle_id++;
                new_vehicles[i][j].type = random_value(0.5);
                new_vehicles[i][j].dir = random_value(group_dir_percent);
                new_vehicles[i][j].weight = new_vehicles[i][j].type == Car ? 200 : 300; // Assign weight if car or van
                // we can initialize the node item used in the linked lists here.
                // We will only ever change the prev, next pointers from now on.

                // Used for printing purposes. Better to store this info here than to repeatedly run this logic
                if (new_vehicles[i][j].dir == North){
                    new_vehicles[i][j].dir_str = "North";
                } else {
                    new_vehicles[i][j].dir_str = "South";
                }

                if (new_vehicles[i][j].type == Car) {
                    new_vehicles[i][j].type_str = "Car";
                } else {
                    new_vehicles[i][j].type_str = "Van";
                }

                // Initialize the thread here.
                pthread_create(&threads[vehicle_id-2], NULL, vehicle_routine, (void *)&new_vehicles[i][j]);

            }
            // Here, we can print the bridge status and waiting queue
            // printf("Sleeping for %d seconds.\n",group_list[i].delay);
            sleep(group_delay);
   
        }
        for (int i = 0; i < total_threads; i++){
            pthread_join(threads[i], NULL);
        }

    }



    return 0;
}