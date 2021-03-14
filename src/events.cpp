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
    //Calculate time since last event and area
    timeOfLastEvent = currentSimulationTime;
    nextEventObject = pq.top();
    pq.pop();
    nextEventType = nextEventObject.event;
    currentSimulationTime = nextEventObject.timeStamp;
}
void state::arrival()
{
    int threadId;
    if (S->Q.size() == 0 && S->allocateThread(threadId))
    {
        auto serviceTime = D.getServiceTime();
        auto remainingTime = serviceTime > S->timeSlice ? (serviceTime - S->timeSlice) : 0.0;

        // std::cout << "Service time for this arrival is " << serviceTime << std::endl;

        //get time at which this request will depart from server.
        double newTimeStamp = currentSimulationTime + std::min(remainingTime, S->timeSlice) + S->contextSwitchOverhead;
        Event N{eventType::DEPARTURE, newTimeStamp, nextEventObject.requestId, threadId, remainingTime, nextEventObject.arrivalTimeStamp};
        requestsAtServer.insert(nextEventObject.requestId);
        pq.push(N);

        // give a timeout event here
        auto timeOut = currentSimulationTime + D.getTimeOut();
        Event T{eventType::TIMEOUT, timeOut, nextEventObject.requestId, -1, 0.0, nextEventObject.arrivalTimeStamp};
        pq.push(T);
    }
    else if (S->Q.size() == S->queueCapacity)
    {
        M->droppedRequests.insert(nextEventObject.requestId);
        //schedule TIMEOUT for this request.
        auto timeOut = D.getTimeOut();
        auto newTimeStamp = currentSimulationTime + timeOut;
        Event N{eventType::TIMEOUT, newTimeStamp, nextEventObject.requestId, -1, 0.0, nextEventObject.arrivalTimeStamp};
        pq.push(N);
        std::cout << "Server Queue is full. Request is dropped. Timeout  event is added." << std::endl;
    }
    else
    {
        //add request in server Queue
        auto ServiceTime = D.getServiceTime();
        queueObject tempObject{nextEventObject.requestId, nextEventObject.arrivalTimeStamp, ServiceTime};
        S->Q.push_back(tempObject);
        // give a timeout event here
        auto timeOut = D.getTimeOut();
        Event N{eventType::TIMEOUT, timeOut, nextEventObject.requestId, -1, ServiceTime, nextEventObject.arrivalTimeStamp};
        requestsAtServer.insert(nextEventObject.requestId);
        pq.push(N);
        std::cout << "Added this request in the server queue, added TIMEOUT " << std::endl;
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
        //calculate response time.
        double responseTime = currentSimulationTime - nextEventObject.arrivalTimeStamp;
        responseTimesPerRun.push_back(responseTime);
        //delete from requestsAtServer set
        auto it2 = requestsAtServer.find(nextEventObject.requestId);
        if (it2 != requestsAtServer.end())
            requestsAtServer.erase(nextEventObject.requestId);
        //schedule next Arrival from current time.
        double newThinkTime = D.getThinkTime();
        double newArrivalTime = currentSimulationTime + newThinkTime;
        int requestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, requestId, -1, 0.0, 0.0};
        pq.push(N);
    }

    else if (S->Q.size() == S->queueCapacity)
    {
        //queue is full
        //add request id in dropped requests, timeout event is already there in pq
        M->droppedRequests.insert(nextEventObject.requestId);
        if (requestsAtServer.find(nextEventObject.requestId) != requestsAtServer.end())
            requestsAtServer.erase(nextEventObject.requestId);
    }
    else
    {
        //this case itself will handle S->Q.size()==0 condition.
        //add this request in the Q
        queueObject currentRequest{nextEventObject.requestId, currentSimulationTime, nextEventObject.remainingTime};
        S->Q.push_back(currentRequest);

        // take one request from Q and schedule for that request departure.
        auto newRequest = S->Q.front();
        int requestId = nextEventObject.requestId;
        int threadId = nextEventObject.threadId;

        //double timeStamp = nextEventObject.timeStamp;
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
    //check if this request is present in dropped or successful request sets
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