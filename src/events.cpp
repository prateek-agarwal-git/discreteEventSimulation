#include "../include/main.h"

void state::updateStats()
{
    double timeSinceLastEvent = currentSimulationTime - timeOfLastEvent;
    int serverQueueLength = S->Q.size();
    int busyThreads = S->countBusyThreads();
    M->areaNumInQueue = timeSinceLastEvent * serverQueueLength;
    M->areaServerStatus = timeSinceLastEvent * ((busyThreads * 1.0) / S->numberThreads);
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
    int threadId;
    auto requestId = nextEventObject.requestId;
    auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
    auto timeOut = currentSimulationTime + D.getTimeOut();
    auto newTimeStamp = currentSimulationTime + timeOut;
    Event T{eventType::TIMEOUT, newTimeStamp, requestId,
            -1, 0.0, arrivalTimeStamp};
    pq.push(T);
    if (S->Q.size() == 0 && S->allocateThread(threadId))
    {
        auto serviceTime = D.getServiceTime();
        auto remainingTime = std::max((serviceTime - S->timeSlice), 0.0);
        double nextTimeStamp = currentSimulationTime + std::min(remainingTime, S->timeSlice) + S->contextSwitchOverhead;
        Event N{eventType::DEPARTURE, nextTimeStamp, requestId,
                threadId, remainingTime, arrivalTimeStamp};
        requestsAtServer.insert(nextEventObject.requestId);
        pq.push(N);
    }
    else if (S->Q.size() == S->queueCapacity)
    {
        M->droppedRequests.insert(requestId);
    }
    else
    {
        auto ServiceTime = D.getServiceTime();
        queueObject tempObject{requestId, arrivalTimeStamp, ServiceTime};
        S->Q.push_back(tempObject);
        requestsAtServer.insert(nextEventObject.requestId);
    }
}

void state::departure()
{
    S->threads[nextEventObject.threadId] = Status::IDLE;
    if (std::abs(nextEventObject.remainingTime) <= 0.0001)
    {
        auto it = M->timedOutRequests.find(nextEventObject.requestId);
        if (it == M->timedOutRequests.end())
            M->successfulRequests.insert(nextEventObject.requestId);

        double responseTime = currentSimulationTime - nextEventObject.arrivalTimeStamp;
        M->responseTimes[M->requestsHandled][M->currentRun] = responseTime;

        auto it2 = requestsAtServer.find(nextEventObject.requestId);
        if (it2 != requestsAtServer.end())
            requestsAtServer.erase(nextEventObject.requestId);

        double newThinkTime = D.getThinkTime();
        double newArrivalTime = currentSimulationTime + newThinkTime;
        int requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, requestId, -1, 0.0, newArrivalTime};
        pq.push(N);
    }

    else if (S->Q.size() == S->queueCapacity)
    {
        M->droppedRequests.insert(nextEventObject.requestId);
        if (requestsAtServer.find(nextEventObject.requestId) != requestsAtServer.end())
            requestsAtServer.erase(nextEventObject.requestId);
    }
    else
    {
        queueObject currentRequest{nextEventObject.requestId, currentSimulationTime, nextEventObject.remainingTime};
        S->Q.push_back(currentRequest);

        auto newRequest = S->Q.front();
        int requestId = nextEventObject.requestId;
        int threadId = nextEventObject.threadId;

        double serviceTime = nextEventObject.remainingTime > S->timeSlice ? (nextEventObject.remainingTime - S->timeSlice) : nextEventObject.remainingTime;
        double remainingTime = serviceTime > 0.0 ? serviceTime : 0.0;
        double departureTime = serviceTime + currentSimulationTime;
        Event N{eventType::DEPARTURE, departureTime, requestId, threadId, remainingTime, newRequest.arrivalTimeStamp};
        pq.push(N);
        S->Q.pop_front();
    }
}
void state::requestTimeout()
{
    int requestId = nextEventObject.requestId;
    auto its = M->successfulRequests.find(requestId);
    auto itd = M->droppedRequests.find(requestId);
    if ((its == M->successfulRequests.end()) && (itd == M->droppedRequests.end()))
    {
        M->timedOutRequests.insert(requestId);
        if (requestsAtServer.find(requestId) != requestsAtServer.end())
            requestsAtServer.erase(requestId);
    }
}
void server::printServerState()
{
    for (auto i = 0; i < numberThreads; i += 1)
    {
        std::cout << i << "th thread: " << std::endl;
        if (threads[i] == Status::IDLE)
            std::cout << "IDLE ";
        else
            std::cout << "BUSY ";
    }
    std::cout << std::endl;
    std::cout << "Queue At Server:" << std::endl;
    std::cout << "Queue Size " << Q.size() << std::endl;
    for (auto it = Q.begin(); it != Q.end(); ++it)
        std::cout << "Request Id: " << it->requestId << "Arrival TimeStamp" << it->arrivalTimeStamp << std::endl;
}
void metrics::printMetrics()
{
    return;
}
void state::printState()
{
    S->printServerState();
    M->printMetrics();
}

int server::countBusyThreads()
{
    int count = 0;
    for (auto i = 0; i < numberThreads; i++)
    {
        if (threads[i] == Status::BUSY)
            count++;
    }
    return count;
}

double distributions::getTime(preComputedTimes &P)
{
    auto temp = P.times[P.currentIndex];
    P.currentIndex += 1;
    P.currentIndex %= P.times.size();
    return temp > 0 ? temp : -1 * temp;
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

bool server::allocateThread(int &threadId)
{
    for (auto count = 0; count < numberThreads; count += 1)
    {
        if (threads[nextThread] == Status::IDLE)
        {
            threads[nextThread] = Status::BUSY;
            threadId = nextThread;
            nextThread += 1;
            nextThread %= numberThreads;
            return true;
        }
        nextThread += 1;
        if (nextThread == numberThreads)
        {
            nextThread = 0;
        }
    }
    threadId = -1;
    return false;
}