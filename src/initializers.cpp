
#include "../include/main.h"
#define CONFIG_FILE "configFile.json"

void distributions::initialize()

{
    mt_engine.seed(seed);
}

void metrics::initializeResponseTimes(int numRuns, int requestsPerRun)
{
    for (int i = 0; i < requestsPerRun; i += 1)
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

void state::initializeResponseTimeVector()
{
    M->initializeResponseTimes(E->runs, E->requestsPerRun);
}
void state::initialize()
{
    M->requestsHandled = 0;
    currentSimulationTime = 0.0;
    timeOfLastEvent = 0.0;
    M->areaNumInQueue = 0.0;
    M->areaServerStatus = 0.;
    M->timedOutRequests.clear();
    M->droppedRequests.clear();
    M->successfulRequests.clear();
    S->initializeServer();
    D.initialize();
    generateTimes();
    responseTimesPerRun.clear();
    for (auto i = 0; i < C->numberOfUsers; i += 1)
    {
        double thinkTime = D.getThinkTime();
        auto requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, thinkTime, requestId, -1, 0.0, 0.0};
        pq.push(N);
        // double thinkTime = C->meanThinkTime;
    }
    std::cout << "\nFinished Initialization\n"
              << std::endl;
}
void distributions::generateParticular(preComputedTimes &P, Distribution &D, int num)
{
    P.times.clear();
    P.currentIndex = 0;
    mt_engine.seed(seed);
    if (D.type == "normal")
    {
        std::normal_distribution<double> nd{D.p1, D.p2};
        for (auto i = 0; i < num; i += 1)
        {
            auto temp = nd(mt_engine);
            while (temp < 0)
                temp = nd(mt_engine);
            P.times.push_back(temp);
        }
    }
    else if (D.type == "exponential")
    {
        // p2 is the minimum value,p1 is the parameter.
        std::exponential_distribution<double> ed{(1.0 / D.p1)};
        for (auto i = 0; i < num; i += 1)
        {
            auto temp = D.p2 + ed(mt_engine);
            P.times.push_back(temp);
        }
    }
    else if (D.type == "uniform")
    {
        std::uniform_real_distribution<double> rd{D.p1, D.p2};
        for (auto i = 0; i < num; i += 1)
        {
            auto temp = rd(mt_engine);
            P.times.push_back(temp);
        }
    }
    else
    {
        std::cout << "Distribution " << D.type << "not supported." << std::endl;
    }
}
void distributions::generateTimeHelper(int num)
{
    generateParticular(thinkTimeValues, thinkTime, num);
    seed += 1;
    generateParticular(serviceTimeValues, serviceTime, num);
    seed += 1;
    generateParticular(timeOutValues, timeOut, num);
    seed += 1;
}
void state::generateTimes()
{
    D.generateTimeHelper(E->requestsPerRun);
}