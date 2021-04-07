#include "../include/main.h"

int main()
{
    state S;
    S.readConfig();
    S.initializeStats();
    S.printConfig();

    for (auto i = 0; i < S.E->runs; i += 1)
    {
        //S.M->currentRun = i;
        S.initialize();
        S.generateTimes();
        while (S.M->requestsHandled < S.E->requestsPerRun)
        {
            S.updateTimeandNextEvent();
            S.updateStats();
            switch (S.nextEventType)
            {
            case eventType::ARRIVAL:
                S.arrival();
                break;
            case eventType::DEPARTURE:
                S.departure();
                break;
            case eventType::TIMEOUT:
                S.requestTimeout();
                break;
            
            }
            if((S.S->numberThreads -S.S->threadPool.size()) > S.C->numberOfUsers )
                std::cout<<"free threads = "<<S.S->threadPool.size()<<std::endl;
        }

        S.updateAccumulators();
    }
    S.writeStats();
    return 0;
}