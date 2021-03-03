#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>

struct thread{
        int status;
};
struct request
{


};
enum class eventType {ARRIVAL, DEPARTURE, REQUESTTIMEOUT};
struct serverQueue{
  int capacity;
  int currentLength;
} ;
struct server
{
        int numberThreads;
        // vector <int> 
        int currentThreadForRoundRobin;
        serverQueue S;
        int numRequestsDropped;

}Server;
struct client{
        double meanThinkTime;
        int numberOfUsers;
        int numRequestsSent;
}Client;
struct Event{
        int typeOfEvent;
        float timeStamp;

};
struct eventQueue{
        std::priority_queue<Event> pq;
// this will be the priority queue in which both arrival and departure times are stored.
};
struct compareTimestamps{
        bool operator() (Event const & e1, Event const & e2){
                return e1.timeStamp> e2.timeStamp;
        }
};

int main(){


        return 0;
}
