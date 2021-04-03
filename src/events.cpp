#include "../include/main.h"

void state::updateStats()
{
    double timeSinceLastEvent = currentSimulationTime - timeOfLastEvent;
    M->testTime += timeSinceLastEvent;
    int serverQueueLength = 0;
    for (auto i = 0; i < S->numberCores; i += 1)
    {
        serverQueueLength += S->Q[i].size();
    }
    int busyThreads = S->countBusyCores();
    M->areaNumInQueue += timeSinceLastEvent * serverQueueLength;
    M->areaServerStatus += timeSinceLastEvent * ((busyThreads * 1.0) / S->numberCores);
}
void state::updateTimeandNextEvent()
{
    timeOfLastEvent = currentSimulationTime;
    nextEventObject = pq.top();
    pq.pop();
    nextEventType = nextEventObject.event;
    currentSimulationTime = nextEventObject.timeStamp;
}
void state::arrival()
{
    int coreId;
    auto requestId = nextEventObject.requestId;
    auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
    auto timeOut = D.getTimeOut();

    auto newTimeStamp = currentSimulationTime + timeOut;
    Event T{eventType::TIMEOUT, newTimeStamp, requestId,
            -1,-1, 0.0, arrivalTimeStamp};
    pq.push(T);
    if (S->availableThreads.size() > 0)
    {
        auto threadId = S->availableThreads.front();
        coreId = threadId % S->numberCores;
        if (S->coreStatus[coreId] == Status::IDLE)
        {
            S->coreStatus[coreId] == Status::BUSY;
            auto serviceTime = D.getServiceTime();
            S->availableThreads.pop_front();
            auto remainingTime = std::max((serviceTime - S->timeSlice), 0.0);
            double nextTimeStamp = currentSimulationTime + std::min(serviceTime, S->timeSlice) + S->contextSwitchOverhead;
            Event N{eventType::DEPARTURE, nextTimeStamp, requestId, coreId,
                    threadId, remainingTime, arrivalTimeStamp};
            requestsAtServer.insert(nextEventObject.requestId);
            pq.push(N);
        }
        else
        {
            auto serviceTime = D.getServiceTime();
            threadObject tempObject{requestId, threadId, arrivalTimeStamp, serviceTime};
            S->Q[coreId].push_back(tempObject);
            requestsAtServer.insert(nextEventObject.requestId);
        }
    }
    else
    {
        M->droppedRequests.insert(requestId);
    }
}

void state::departure()
{
    auto threadId = nextEventObject.threadId;
    int coreId = nextEventObject.coreId;
    auto requestId = nextEventObject.requestId;
    auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
    auto remainingTime = nextEventObject.remainingTime;
    if (std::abs(remainingTime) <= 0.00001)
    {
        auto it = M->timedOutRequests.find(requestId);
        if (it == M->timedOutRequests.end())
            M->successfulRequests.insert(requestId);

        double responseTime = currentSimulationTime - arrivalTimeStamp;
        M->currentResponseVector.push_back(responseTime);

        auto it2 = requestsAtServer.find(requestId);
        if (it2 != requestsAtServer.end())
            requestsAtServer.erase(it2);

        S->coreStatus[coreId] = Status::IDLE;
        auto newThinkTime = D.getThinkTime();
        auto newArrivalTime = currentSimulationTime + newThinkTime;
        int requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, requestId, -1, -1, 0.0, newArrivalTime};
        pq.push(N);
    }
    else
    {
        threadObject currentRequest{requestId, threadId, arrivalTimeStamp, remainingTime};
        S->Q[coreId].push_back(currentRequest);
    }
    if (S->Q[coreId].empty() == false)
    {
        auto nextRequest = S->Q[coreId].front();
        S->Q[coreId].pop_front();
        auto requestId = nextRequest.requestId;
        auto nextThreadId = nextRequest.threadId;
        S->coreStatus[coreId] = Status::BUSY;
        auto currentServiceQuantum = std::min(nextRequest.remainingTime, S->timeSlice);
        auto remainingServiceTime = std::max(0.0, nextRequest.remainingTime - S->timeSlice);
        auto nextTimeStamp = currentSimulationTime + currentServiceQuantum + S->contextSwitchOverhead;
        if (S->Q[coreId].size() == 0)
        {
            nextTimeStamp -= S->contextSwitchOverhead;
        }
        Event N{eventType::DEPARTURE, nextTimeStamp,
                requestId, coreId, nextThreadId, remainingServiceTime, nextRequest.arrivalTimeStamp};
        pq.push(N);
    }
}
void state::requestTimeout()
{
    int requestId = nextEventObject.requestId;
    auto its = M->successfulRequests.find(requestId);
    auto itd = M->droppedRequests.find(requestId);
    auto it = requestsAtServer.find(requestId);
    if (it != requestsAtServer.end())
        requestsAtServer.erase(it);
    if ((its == M->successfulRequests.end()) && (itd == M->droppedRequests.end()))
        M->timedOutRequests.insert(requestId);
    if (itd != M->droppedRequests.end())
    {
        M->requestsDepartedorDropped += 1;
        double newThinkTime = D.getThinkTime();
        double newArrivalTime = currentSimulationTime + newThinkTime;
        int requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, requestId,-1, -1, 0.0, newArrivalTime};
        pq.push(N);
    }
}

void server::printServerState()
{
    for (auto i = 0; i < numberCores; i += 1)
    {
        std::cout << i << "th thread: " << std::endl;
        if (coreStatus[i] == Status::IDLE)
            std::cout << "IDLE ";
        else
            std::cout << "BUSY ";
    }
    std::cout << std::endl;
    std::cout << "Queue At Server:" << std::endl;
    std::cout << "Queue Size " << Q.size() << std::endl;
    for (auto i = 0; i < numberCores; i += 1)
    {
        for (auto it = Q[i].begin(); it != Q[i].end(); ++it)
            std::cout << "Request Id: " << it->requestId << "Arrival TimeStamp" << it->arrivalTimeStamp << std::endl;
    }
}
void metrics::printMetrics()
{
    return;
}
void state::printState()
{
    M->printMetrics();
}

int server::countBusyCores()
{
    int count = 0;
    for (auto i = 0; i < numberCores; i++)
    {
        if (coreStatus[i] == Status::BUSY)
            count++;
    }
    return count;
}

double distributions::getTime(preComputedTimes &P)
{
    auto temp = P.times[P.currentIndex];
    P.currentIndex += 1;
    P.currentIndex %= P.times.size();
    return temp;
}
double distributions::getThinkTime()
{
    return getTime(thinkTimeValues);
}

double distributions::getServiceTime()
{
    return getTime(serviceTimeValues);
}
double distributions::getTimeOut()
{
    return getTime(timeOutValues);
}

bool server::allocateCore(int &coreId)
{
    for (auto count = 0; count < numberCores; count += 1)
    {
        if (coreStatus[nextCore] == Status::IDLE)
        {
            coreStatus[nextCore] = Status::BUSY;
            coreId = nextCore;
            nextCore += 1;
            nextCore %= numberCores;
            return true;
        }
        nextCore += 1;
        if (nextCore == numberCores)
        {
            nextCore = 0;
        }
    }
    coreId = -1;
    return false;
}