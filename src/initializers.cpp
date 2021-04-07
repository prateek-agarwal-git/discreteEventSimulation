
#include "../include/main.h"
#define CONFIG_FILE "configFile.json"

void distributions::initialize()

{
    mt_engine.seed(seed);
}

void state::initializeStats()
{
    for(auto i=0; i<S->numberCores; i++)
        S->readyQ.push_back(std::deque<threadObject>());
    M->responseTimes.clear();
    M->accumulatedDroppedRequests = 0;
    M->accumulatedTimedOutRequests = 0;
    M->accumulatedDroppedRequests = 0;
    M->accumulatedUtilization = 0.0;
    M->accumulatedAverageNumberinQueue = 0.0;
    M->dropRate = 0.0;
    M->badPut = 0.0;
    M->goodPut = 0.0;
    M->throughPut = 0.0;
    
}

void state::initialize()
{
    M->requestsHandled = 0;
    currentSimulationTime = 0.0;
    timeOfLastEvent = 0.0;
    M->areaNumInQueue = 0.0;
    M->areaServerStatus = 0.0;
    M->timedOutRequests.clear();
    M->droppedRequests.clear();
    M->successfulRequests.clear();
    M->currentResponseVector.clear();
    
    S->initializeServer();
    D.initialize();
    generateTimes();
    while (pq.empty() == false)
    {
        pq.pop();
    }

    for (auto i = 0; i < C->numberOfUsers; i += 1)
    {
        double thinkTime = D.getThinkTime() + 2*i;
        auto requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, thinkTime, requestId, -1,-1, 0.0, thinkTime};
        pq.push(N);
    }
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
            while (temp <= 0)
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
    std::ofstream filePointStats("output/means.data", std::ios::out | std::ios::app);

    auto averageSuccessfulRequests = (M->accumulatedSuccesfulRequests * 1.0) / E->runs;
    auto averageTimeoutRequests = (M->accumulatedTimedOutRequests * 1.0) / E->runs;
    auto droppedRequests = (M->accumulatedDroppedRequests * 1.0) / E->runs;
    auto averageUtilization = (1.0 * M->accumulatedUtilization) / E->runs;
    auto averageQueueLength = M->accumulatedAverageNumberinQueue / E->runs;
    auto dropRate = M->dropRate / E->runs;
    auto goodPut = M->goodPut / E->runs;
    auto badPut = M->badPut / E->runs;
    auto throughPut = M->throughPut / E->runs;

    filePointStats << "NumberOfUsers = " << C->numberOfUsers << std::endl;
    filePointStats << "Average Successful Requests = " << std::setw(3) << averageSuccessfulRequests << std::endl;
    filePointStats << "Average TimeOutRequests = " << std::setw(3) << averageTimeoutRequests << std::endl;
    filePointStats << "Dropped Requests = " << std::setw(3) << droppedRequests << std::endl;
    filePointStats << "Average Utilization = " << std::setw(3) << averageUtilization << std::endl;
    filePointStats << "Average Queue Length = " << std::setw(3) << averageQueueLength << std::endl;
    filePointStats << "GoodPut = " << goodPut << std::endl;
    filePointStats << "badPut = " << badPut << std::endl;
    filePointStats << "throughPut = " << throughPut << std::endl;
    filePointStats << "dropRate = " << dropRate << std::endl;
    double s = 0.0;
    int n = 0;
    // writing response Times
    /*
    std::string delayFile = "output/delayFile";
    delayFile += std::to_string(C->numberOfUsers);
    delayFile += ".csv";
    std::ofstream fileDelay(delayFile, std::ios::out);
    */
    for (auto i = 0UL; i < M->responseTimes.size(); i += 1)
    {
        for (auto j = 0UL; j < M->responseTimes[i].size(); j += 1)
        {
            //fileDelay << M->responseTimes[i][j] << " ";
            s += M->responseTimes[i][j];
            n += 1;
        }
        //fileDelay << "\n";
    }

    auto avgResponseTime = s / n;
    filePointStats << "Average Response time= " << std::setw(3) << avgResponseTime << std::endl;
    filePointStats << "=========================================== "<< std::endl;
}

void state::updateAccumulators()
{
    int  numDroppedRequests = M->droppedRequests.size();
    int numTimedOutRequests = M->timedOutRequests.size();
    int numSuccessfulRequests = M->successfulRequests.size();
    M->accumulatedDroppedRequests += numDroppedRequests;
    M->accumulatedTimedOutRequests += numTimedOutRequests;
    M->accumulatedSuccesfulRequests += numSuccessfulRequests;
    M->accumulatedAverageNumberinQueue += ((1.0 * M->areaNumInQueue) / currentSimulationTime);
    M->accumulatedUtilization += ((1.0 * M->areaServerStatus) / currentSimulationTime);
    M->responseTimes.push_back(M->currentResponseVector);
    M->dropRate += numDroppedRequests / currentSimulationTime;
    M->badPut += numTimedOutRequests / currentSimulationTime;
    M->goodPut += numSuccessfulRequests / currentSimulationTime;
    M->throughPut += (numTimedOutRequests+numSuccessfulRequests) / currentSimulationTime;
}
