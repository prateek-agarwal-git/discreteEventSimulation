#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <set>

enum class eventType
{
        ARRIVAL,
        DEPARTURE,
        TIMEOUT
};

enum class Status
{
        BUSY,
        IDLE
};

struct queueObject{
        int userId;
        double arrivalTimeStamp;
};

struct metrics
{
        int timedOutRequests;
        int successfulRequests;
        double coreUtilization;
        double requestDropRate;
};
struct server
{
        server(int numberThreads, double meanServiceTime,
         double contextSwitchOverhead, int queueCapacity) : numberThreads{numberThreads}, 
         meanServiceTime{meanServiceTime}, queueCapacity{queueCapacity},
         contextSwitchOverhead{contextSwitchOverhead} {
                 threads = new Status[numberThreads];
                 for (auto i = 0 ; i <numberThreads; i+=1)
                         threads[i] = Status::IDLE;
                nextThread = 0;       
         }
         ~server(){
                 delete threads;
         }

private:
        int numberThreads;
        int nextThread;
        std::deque<queueObject> Q;
        int numRequestsDropped;
        Status *threads;
        double meanServiceTime;
        double contextSwitchOverhead;
        const int queueCapacity;
};
struct client
{

        client(double meanThinkTime, int numberOfUsers, double meanTimeout) : meanThinkTime{meanThinkTime}, numberOfUsers{numberOfUsers}, meanTimeout{meanTimeout} {}
        double getMeanThinktime()
        {
                return meanThinkTime;
        }
        double getNumberOfUsers()
        {
                return numberOfUsers;
        }
        double getMeanTimeout()
        {
                return meanTimeout;
        }

private:
        const double meanThinkTime;
        const int numberOfUsers;
        int numRequestsSent;
        const double meanTimeout;
};
struct Event
{
        int typeOfEvent;
        float timeStamp;
        int userId;
};
struct eventQueue
{
        std::priority_queue<Event> pq;
        // this will be the priority queue in which both arrival and departure times are stored.
};
struct compareTimestamps
{
        bool operator()(Event const &e1, Event const &e2)
        {
                return e1.timeStamp > e2.timeStamp;
        }
};

void arrive(){

}
void departure(){

}
void requestTimeout(){

}
// time is in seconds
int main()
{
        int numRuns = 1;
        int numberOfUsers = 100;
        double meanThinkTime = 180.0;
        int queueCapacity = 20;
        int numThreads = 1;
        double meanTimeout = 30.0;
        double meanServiceTime = 0.3;
        double contextSwitchOverhead = 0.1;
        client C(meanThinkTime, numberOfUsers, meanTimeout);
        std::cout << C.getMeanThinktime() << std::endl;
        std::cout << C.getMeanTimeout() << std::endl;
        std::cout << C.getNumberOfUsers() << std::endl;
        server Server( int numberThreads, double meanServiceTime,
         double contextSwitchOverhead, int queueCapacity);
        return 0;
}
