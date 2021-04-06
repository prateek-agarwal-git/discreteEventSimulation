#include "../include/main.h"

void state::updateStats()
{
    double timeSinceLastEvent = currentSimulationTime - timeOfLastEvent;
    auto serverQueueLength = 0;
    for (auto i = 0; i < S->numberCores; i += 1)
        serverQueueLength += S->readyQ[i].size();
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
    auto requestId = nextEventObject.requestId;
    auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
    auto timeOut = D.getTimeOut();

    auto newTimeStamp = currentSimulationTime + timeOut;
    Event T{eventType::TIMEOUT, newTimeStamp, requestId,
            -1, -1, 0.0, arrivalTimeStamp};
    pq.push(T);
    if (S->threadPool.size() > 0)
    {
        auto threadId = S->threadPool.front();
        S->threadPool.pop_front();
        auto coreId = threadId % S->numberCores;
        auto serviceTime = D.getServiceTime();
        if (S->coreStatus[coreId] == Status::IDLE)
        {
            S->coreStatus[coreId] = Status::BUSY;
            auto remainingTime = std::max((serviceTime - S->timeSlice), 0.0);
            double nextTimeStamp = currentSimulationTime + std::min(serviceTime, S->timeSlice) + S->contextSwitchOverhead;
            Event N{eventType::DEPARTURE, nextTimeStamp, requestId, coreId,
                    threadId, remainingTime, arrivalTimeStamp};
            pq.push(N);
        }
        else
        {
            threadObject tempObject{requestId, threadId, arrivalTimeStamp, serviceTime};
            S->readyQ[coreId].push_back(tempObject);
        }
    }
    else
    {
        if (S->requestQueue.size() < S->requestQueueCapacity)
            S->requestQueue.push_back({requestId, arrivalTimeStamp});
        else
            M->droppedRequests.insert(requestId);
    }
}

void state::departure()
{
    auto threadId = nextEventObject.threadId;
    auto coreId = nextEventObject.coreId;
    auto requestId = nextEventObject.requestId;
    auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
    auto remainingTime = nextEventObject.remainingTime;
    if (std::abs(remainingTime) <= 0.00001)
    {
        auto it = M->timedOutRequests.find(requestId);
        if (it == M->timedOutRequests.end())
            M->successfulRequests.insert(requestId);

        S->coreStatus[coreId] = Status::IDLE;
        if (S->requestQueue.size() > 0)
        {
            requestQueueObject R = S->requestQueue.front();
            S->requestQueue.pop_front();
            auto wRequestId = R.requestId;
            auto wArrivalTimeStamp = R.arrivalTimeStamp;
            auto wServiceTime = D.getServiceTime();
            threadObject T{wRequestId, threadId, wArrivalTimeStamp, wServiceTime};
            S->readyQ[coreId].push_back(T);
        }
        else
            S->threadPool.push_back(threadId);
        double responseTime = currentSimulationTime - arrivalTimeStamp;
        M->currentResponseVector.push_back(responseTime);

        auto newThinkTime = D.getThinkTime();
        auto newArrivalTime = currentSimulationTime + newThinkTime;
        int newArrivalRequestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, newArrivalRequestId, -1, -1, 0.0, newArrivalTime};
        pq.push(N);
    }
    else
    {
        threadObject currentThreadObject{requestId, threadId, arrivalTimeStamp, remainingTime};
        S->readyQ[coreId].push_back(currentThreadObject);
    }
    if (S->readyQ[coreId].size() > 0)
    {
        auto T = S->readyQ[coreId].front();
        S->readyQ[coreId].pop_front();
        S->coreStatus[coreId] = Status::BUSY;

        auto newRequestId = T.requestId;
        auto threadId = T.threadId;
        auto currentServiceQuantum = std::min(T.remainingTime, S->timeSlice);
        auto remainingServiceTime = std::max(0.0, T.remainingTime - S->timeSlice);
        auto timeStamp = currentSimulationTime + currentServiceQuantum + S->contextSwitchOverhead;
        auto arrivalTimeStamp = T.arrivalTimeStamp;
        if ((S->readyQ[coreId].size() == 0) && (newRequestId == requestId))
            timeStamp -= S->contextSwitchOverhead;
        Event N{
            eventType::DEPARTURE,
            timeStamp,
            newRequestId,
            coreId,
            threadId,
            remainingServiceTime,
            arrivalTimeStamp
        };
        pq.push(N);
    }
}
void state::requestTimeout()
{
    int requestId = nextEventObject.requestId;
    auto its = M->successfulRequests.find(requestId);
    auto itd = M->droppedRequests.find(requestId);
    if ((its == M->successfulRequests.end()) && (itd == M->droppedRequests.end()))
        M->timedOutRequests.insert(requestId);
        
    if (itd != M->droppedRequests.end())
    {
        double newThinkTime = D.getThinkTime();
        double newArrivalTime = currentSimulationTime + newThinkTime;
        int requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, requestId, -1, -1, 0.0, newArrivalTime};
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
    for (auto i = 0; i < numberCores; i += 1)
    {

        std::cout << "Queue" << i << "Size = " << readyQ[i].size() << std::endl;
        for (auto it = readyQ[i].begin(); it != readyQ[i].end(); ++it)
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
