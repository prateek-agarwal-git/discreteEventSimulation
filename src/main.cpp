#include "../include/main.h"

int main()
{
    state S;
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
            S.updateTimeandNextEvent();
            // S.printState();

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
            S.updateStats();
        }
        S.updateAccumulators();
    }
    S.writeStats();
    return 0;
}
// S.printState();
//if (S.pq.size()== 0){
//    // std::cout<<"this should not happen"<<std::endl;
//    S.updateStats();
//    // exit(0);
//    break;
//}
// std
//if (S.pq.size() == 0)
//            {
//                std::cout << "this should not happen" << std::endl;
//                std::cout << "currentRun = " << i << std::endl;
//                std::cout << "server queue size = " << S.S->Q.size() << std::endl;
//                S.updateStats();
//                exit(0);
//                break;
//            }

// std::cout << "ARRIVAL " << std::endl;

// std::cout << "DEPARTURE " << std::endl;

// std::cout << "TIMEOUT " << std::endl;