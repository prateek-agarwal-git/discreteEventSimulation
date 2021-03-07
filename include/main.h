#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <set>
#include <fstream>

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
        int typeOfEvent;
        float timeStamp;
        int userId;
};

struct compareTimestamps
{
        bool operator()(Event const &e1, Event const &e2)
        {
                return e1.timeStamp > e2.timeStamp;
        }
};
struct state
{
        std::priority_queue<Event, std::vector<Event>, compareTimestamps> pq;
        std::set<int> requestsAtServer;
        int numberRequestsDepartedorTimedOut;
        double currentSimulationTime;
        std::random_device rd;
        void arrival();
        void departure();
        void requestTimeout();
        void printState();
        void updateTimeandNextEvent();
        void initialize(client &Client, server &Server);
        eventType nextEvent;
};
struct client
{

        client(double meanThinkTime, int numberOfUsers, double meanTimeout) : meanThinkTime{meanThinkTime}, numberOfUsers{numberOfUsers}, meanTimeout{meanTimeout}
        {
        }

        // private:
        const double meanThinkTime;
        const int numberOfUsers;
        const double meanTimeout;
};
struct queueObject
{
        int userId;
        double arrivalTimeStamp;
};
