#include "../include/main.h"

int main()
{
    state S;
    int demo = 0;
    S.readConfig();
    S.initializeStats();
    S.printConfig();
    S.generateTimes();

    for (auto i = 0; i < S.E->runs; i += 1)
    {
        S.M->currentRun = i;
        S.initialize();
        while (S.M->requestsHandled < S.E->requestsPerRun)
        {
            if (demo)
                S.printState();
            S.updateTimeandNextEvent();
            switch (S.nextEventType)
            {
            case eventType::ARRIVAL:
                S.arrival();
                // std::cout << "ARRIVAL " << std::endl;
                //sleep(3);
                break;
            case eventType::DEPARTURE:
                S.departure();
                // std::cout << "DEPARTURE " << std::endl;
                //sleep(3);
                break;
            case eventType::TIMEOUT:
                S.requestTimeout();
                // std::cout << "TIMEOUT " << std::endl;
            //    sleep(3);
                break;
            }
            S.updateStats();
        }
            S.updateAccumulators();
    }
    S.writeStats();
    return 0;
}
