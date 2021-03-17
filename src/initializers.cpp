
#include "../include/main.h"
#define CONFIG_FILE "configFile.json"

void distributions::initialize()

{
    mt_engine.seed(seed);
}

void metrics::initializeResponseTimes(int numRuns, int requestsPerRun)
{
    for (int i = 0; i < (requestsPerRun + 10); i += 1)
    {
        //10 is just a buffer to avoid index out of bounds
        std::vector<double> temp{};
        for (int j = 0; j < numRuns; j += 1)
        {
            temp.push_back(-1.0);
        }
        responseTimes.push_back(temp);
    }
}

void state::initializeStats()
{
    M->testTime = 0.0;
    M->initializeResponseTimes(E->runs, E->requestsPerRun);
    M->accumulatedDroppedRequests = 0;
    M->accumulatedTimedOutRequests = 0;
    M->accumulatedDroppedRequests = 0;
    M->accumulatedUtilization = 0.0;
    M->accumulatedAverageNumberinQueue = 0.0;
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
    for (auto i = 0; i < C->numberOfUsers; i += 1)
    {
        double thinkTime = D.getThinkTime();
        auto requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, thinkTime, requestId, -1, 0.0, thinkTime};
        pq.push(N);
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

void state::writeStats()
{
    //writing point estimates
    std::ofstream filePointStats("means.data", std::ios::out | std::ios::app);

    auto goodPut = (M->accumulatedSuccesfulRequests * 1.0) / E->runs;
    auto badPut = (M->accumulatedTimedOutRequests * 1.0) / E->runs;
    auto droppedRequests = (M->accumulatedDroppedRequests * 1.0) / E->runs;
    auto averageUtilization = M->accumulatedUtilization / E->runs;
    auto averageQueueLength = M->accumulatedAverageNumberinQueue / E->runs;

    filePointStats << "Goodput = " << std::setw(3) << goodPut << std::endl;
    filePointStats << "Badput = " << std::setw(3) << badPut << std::endl;
    filePointStats << "Dropped Requests = " << std::setw(3) << droppedRequests << std::endl;
    filePointStats << "Average Utilization = " << std::setw(3) << averageUtilization << std::endl;
    filePointStats << "Average Queue Length = " << std::setw(3) << averageQueueLength << std::endl;

    // writing response Times
    std::ofstream fileDelay("delayFile", std::ios::out | std::ios::app);
    for (auto i = 0; i < E->requestsPerRun; i += 1)
    {
        for (auto j = 0; j < E->runs; j += 1)
        {
            fileDelay << M->responseTimes[i][j] << ",";
        }
        fileDelay << "\n";
    }
}

void state::updateAccumulators()
{
    M->accumulatedDroppedRequests += M->droppedRequests.size();
    M->accumulatedTimedOutRequests += M->timedOutRequests.size();
    M->accumulatedSuccesfulRequests += M->successfulRequests.size();
    M->accumulatedAverageNumberinQueue += ((1.0 * M->areaNumInQueue) / currentSimulationTime);
    M->accumulatedUtilization += (1.0 * M->areaServerStatus) / currentSimulationTime;
    std::cout << currentSimulationTime << std::endl;
    std::cout << M->areaServerStatus << std::endl;
    std::cout << M->testTime << std::endl;
}
