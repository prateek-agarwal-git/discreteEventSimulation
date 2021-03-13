#include "../include/main.h"

int main()
{
        state S;
        int demo = 0;
        S.readConfig();
        S.initialize();
        S.printConfig();
        S.generateTimes();
        exit(0);
        for (auto i = 0; i < S.E->runs; i += 1)
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
                                break;
                        case eventType::DEPARTURE:
                                S.departure();
                                break;
                        case eventType::TIMEOUT:
                                S.requestTimeout();
                                break;
                        }
                }
        }
        return 0;
}
