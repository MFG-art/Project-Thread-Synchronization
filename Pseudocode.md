# Pseudocode for Project Thread Synchronization

Ideas: Modify global variables via a lock 
(weight variable is a critical section, so gatekeep behind a lock)

Bridge rules: No more than 1200 units of weight on Bridge. If there is traffic in both directions, there must be a lane allocated for northbound and southbound travel. If travel is in one direction, use both lanes for that one direction of traffic.

How can I implement this: Implement weight variable

On Arrival, each vehicle waits for enough weight available on bridge and adds its weight to the total once on the bridge.
A vehicle also needs access to a lane of travel in its direction. If there isn't a lane of travel in it's direction, the lane with the fewest vehicles will stop accepting vehicles and clear out, so that it can handle traffic in the new direction.

Lane structures are used to represent the physical lanes. They contain the number of cars on that lane as well as the current direction of travel. Lanes cannot be edited without access to a key

There is also a global integer variable that contains the total weight on the bridge. It cannot be modified unless the key for it is lead.

We can use enums to represent values such as direction and vehicle type. Because these values are calculated based off pseudorandom numbers, having an enum can make these resulting values easier to understand in the code. Enums are also easier to compare to than strings in C.

However, for console logging purposes, it might be useful to have a string representation of these values saved.

Example: let's say car is a value of 0 in an Enum.
car enum = 0, car string = "Car"
printing enum = 0 #1
printing string = Car #1 - makes more sense!

```c
// This is how to initialize locks and condition variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int weight; // this is a global integer value that contains the total weight on the bridge, camnot exceed 1200

def struct Lane(
    int vehicle_count;
    enum direction{North, South, None}
)
```
DON'T DECLARE AND INITIALIZE MULTIPLE LOCKS IN ONE LINE. LESSON LEARNED!

## Lane Conditions:
There are four important lane conditions that I need to implement.
 
 1. At least one lane is empty. The vehicle should always take an empty lane if it is available.
 2. There is one lane of the vehicle's direction of travel. The vehicle attempst to use this lane.
 2. Both lanes are in the vehicle's direction of travel. The vehicle uses the lane with the fewest vehicles.
 4. Both names have travel opposite to the vehicle. The lane with the fewest vechicles is cleared out, and assigned as travel in the vehicle's direction.

 What condition variables are needed? - correction, are these condition variables or global variables.

 Condition variables are something we can have threads sleep on. Some of these conditions are just being checked in order to make a decision outside of sleeping.

GLOBAL VARIABLES / STATE VARIABLES

lane1open , lane2open - Are the lanes accepting new vehicles. Only used in case of oncoming traffic.
lane1dir, lane2dir - This is the current direction of travel. (Northbound, Southbound, none)

In case of oncoming traffic, a vehicle must initiate the request to clear out a full lane. If other vehicles travelling in the same direction see a closed lane with traffic opposite to them, they should assume that the lane will change directions once cleared out and should wait on that lane to do so.
Idea: each lane has a value that determines if it is open or closed. All cars attempting to enter a lane MUST verify that the lane is open. This makes for a quick way to clear out a lane. The lane can be opened by the vehicle that closed it, reversing the traffic direction.

Can we divide choosing a lane versus following a vehicle on a chosen lane?
vehicle_routine can look at lane conditions. Vehicle selects either lane 1 or 2, and whether or not to wait based on state.
choose_lane - lane is passed as argument, all locks/conditions used relate to that lane, except for global variables

What state variables are needed?

lane1vehicle_count, lane2vehicle_count = Used to tell whether lanes are empty; which lane has fewest vehicles
bridge_weight = used to prevent too much weight on bridge

The best way to represent these queues are by using linked lists. -> linked lists are flexible in size, as opposed to arrays. 
Can we use a node structure to represent linked list nodes?

# Other ideas /  housekeeping:

DRY - any repeated code becomes a function. Inspiration for making choose_lane (don't repeat code for two lanes, just pass lane arg)

Comments! Try to comment every method and important part of code. Short description of what each method does.

use makefile. 

Implement quick way to test cases. Inspiration for entering imputs as args. Really, this is what I should have done first.

How can we keep track of vehicles outside of arrival, crossing, and leaving. It is beneficial to see when lanes clear out, and the clearing out of lanes by oncoming traffic.

