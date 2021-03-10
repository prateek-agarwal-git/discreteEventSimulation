#include "../include/main.h"

int main()
{

        int numRuns = 1;
        int totalRequests = 1000;
        int numberOfUsers = 100;
        double meanThinkTime = 180.0;
        int queueCapacity = 20;
        int numThreads = 1;
        double meanRequestTimeout = 30.0;
        double meanServiceTime = 0.3;
        double contextSwitchOverhead = 0.1;
        metrics M(numRuns, totalRequests);

        eventType nextEventType;
        state currentState;
        int demo = 0;
        client Client(meanThinkTime, numberOfUsers, meanRequestTimeout);
        server Server(numThreads, meanServiceTime,
                      contextSwitchOverhead, queueCapacity);

        currentState.initialize();
        while (currentState.numberRequestsDepartedorTimedOut < totalRequests)
        {
                if (demo)
                {
                        currentState.printState();
                }
                currentState.updateTimeandNextEvent();
                switch (currentState.nextEventType)
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
