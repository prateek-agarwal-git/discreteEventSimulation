#include "../include/main.h"

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
                                                                  contextSwitchOverhead{contextSwitchOverhead}
        {
                threads = new Status[numberThreads];
                for (auto i = 0; i < numberThreads; i += 1)
                        threads[i] = Status::IDLE;
                nextThread = 0;
        }
        ~server()
        {
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

void state::initialize(state &state, client &Client, server &Server)
{
        state.numberRequestsDepartedorTimedOut = 0;
        state.currentSimulationTime = 0.0;

        for (auto i = 0; i < Client.numberOfUsers; i += 1)
        {
                double thinkTime = Client.meanThinkTime;
                state.pq.push();
        }
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
        int totalRequests = 1000;
        eventType nextEvent;
        state currentState;
        int demo = 0;
        client Client(meanThinkTime, numberOfUsers, meanTimeout);
        server Server(numThreads, meanServiceTime,
                      contextSwitchOverhead, queueCapacity);

        currentState.initialize(Client, Server);
        while (currentState.numberRequestsDepartedorTimedOut < totalRequests)
        {
                if (demo)
                {
                        currentState.printState();
                }
                currentState.updateTimeandNextEvent();
                switch (currentState.nextEvent)
                {
                case eventType::ARRIVAL:
                {
                        currentState.arrival();
                }
                break;
                case eventType::DEPARTURE:
                {
                        currentState.departure();
                }
                break;
                case eventType::TIMEOUT:
                {
                        currentState.requestTimeout();
                }
                break;
                }
        }
        return 0;
}
//double getMeanThinktime()
//{
//return meanThinkTime;
//}
//double getNumberOfUsers()
//{
//return numberOfUsers;
//}
//double getMeanTimeout()
//{
//return meanTimeout;
//}
//
