#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <set>
#include <fstream>
#include <memory>
// Entry to priority_queue
// server to client during departure (ARRIVAL, TIMESTAMP, userID): Initially and at the departure or timeout.
// client to queue(TIMEOUT, TIMESTAMP,  userId) -
//  queue to server for service  (DEPARTURE, TIMESTAMP, USERID, ServerID);  - server idle
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
struct Event
{
        eventType event;
        float timeStamp;
        int requestId;
        int threadId; // this server should be made idle once the request departs. Departure
};
struct metrics
{
        metrics() {}
        metrics(int numRuns, int totalRequests)
        {
                for (int i = 0; i < totalRequests; i += 1)
                {
                        std::vector<double> temp{};
                        for (int j = 0; j < numRuns; j += 1)
                        {
                                temp.push_back(-1.0);
                                // -1.0 represent request drop
                        }
                        responseTimes.push_back(temp);
                }
        }
        int timedOutRequests;
        int successfulRequests;
        double coreUtilization;
        double requestDropRate;
        std::vector<std::vector<double>> responseTimes;
};
struct client
{

        client(double meanThinkTime, int numberOfUsers, double meanTimeout) : meanThinkTime{meanThinkTime}, numberOfUsers{numberOfUsers}, meanTimeout{meanTimeout}
        {
        }
        const double meanThinkTime;
        const int numberOfUsers;
        const double meanTimeout;
};

struct compareTimestamps
{
        bool operator()(Event const &e1, Event const &e2)
        {
                return e1.timeStamp > e2.timeStamp;
        }
};
struct queueObject
{
        int requestId;
        double arrivalTimeStamp;
};
struct server
{
        server()
        {
                std::unique_ptr<Status[]> temp{new Status[numberThreads]};
                threads = std::move(temp);
                for (auto i = 0; i < numberThreads; i += 1)
                        threads[i] = Status::IDLE;
                nextThread = 0;
        }
        server(int numberThreads, double meanServiceTime,
               double contextSwitchOverhead, int queueCapacity) : numberThreads{numberThreads},
                                                                  meanServiceTime{meanServiceTime}, queueCapacity{queueCapacity},
                                                                  contextSwitchOverhead{contextSwitchOverhead}
        {
        }

        bool allocateThread(int &threadId);

        std::deque<queueObject> Q;
        int numberThreads;
        int nextThread;
        std::unique_ptr<Status[]> threads;
        double meanServiceTime;
        double contextSwitchOverhead;
        int queueCapacity;
};

struct state
{
        state()
        {
                std::unique_ptr<server> tempS{new server};
                S = std::move(tempS);
        }
        std::priority_queue<Event, std::vector<Event>, compareTimestamps> pq;
        std::set<int> requestsAtServer;
        int numberRequestsDepartedorTimedOut;
        int numberOfDroppedRequests;
        double currentSimulationTime;
        Event nextEventObject;
        std::random_device rd;
        void arrival();
        void departure();
        void requestTimeout();
        void printState();
        void updateTimeandNextEvent();
        bool isAnyCoreIdle();
        void initialize();
        metrics M;

        std::unique_ptr<client> C;
        std::unique_ptr<server> S;
        eventType nextEventType;
};
