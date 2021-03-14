#include "../include/main.h"

int main()
{
        state S;
        int demo = 0;
        S.readConfig();
        S.initializeResponseTimeVector();
        S.printConfig();
        S.generateTimes();
        //exit(0);
        //for (auto i = 0; i < S.E->runs; i += 1)
        for (auto i = 0; i < 1; i += 1)
        {
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
                                std::cout<<"ARRIVAL "<<std::endl;
                                sleep(3);
                                break;
                        case eventType::DEPARTURE:
                                S.departure();
                                std::cout<<"DEPARTURE "<<std::endl;
                                sleep(3);
                                break;
                        case eventType::TIMEOUT:
                                S.requestTimeout();
                                std::cout<<"DEPARTURE "<<std::endl;
                                sleep(3);
                                break;
                        }
                        S.updateStats();
                        std::cout<<"requestsHandled are:  "<<S.M->requestsHandled<<std::endl;
                        std::cout<<"\nQueue size is " <<S.S->Q.size()<<std::endl;
                }

                S.M->responseTimes.push_back(S.responseTimesPerRun);
                //other Calculations:
                //1) avg responseTime, avg serverUtilization, avgNumInQueue
                //2) writing response times in a file

        }
        return 0;
}
