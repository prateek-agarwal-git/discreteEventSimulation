#include "../include/main.h"

void state::updateStats()
{
    double timeSinceLastEvent = currentSimulationTime - timeOfLastEvent;
    auto serverQueueLength = 0;
    for(auto i=0; i<S->numberCores; i++ )
        serverQueueLength += S->readyQ[i].size();
    int busyCores = S->countBusyCores();
    M->areaNumInQueue += timeSinceLastEvent * serverQueueLength;
    M->areaServerStatus += timeSinceLastEvent * ((busyCores * 1.0) / S->numberCores);
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
            auto remainingTime = std::max(0.0, serviceTime - S->timeSlice);
            auto nextTimeStamp = currentSimulationTime + std::min(serviceTime, S->timeSlice) + S->contextSwitchOverhead;

            Event E{eventType::DEPARTURE, nextTimeStamp, requestId, coreId, threadId, remainingTime, arrivalTimeStamp};
            pq.push(E);
        }
        else
        {
            threadObject T{requestId, threadId, arrivalTimeStamp, serviceTime};
            S->readyQ[coreId].push_back(T);
        }
    }
    else
    {
        if (S->requestQueue.size() <= S->requestQueueCapacity)
        {
            requestQueueObject R{requestId, arrivalTimeStamp};
            S->requestQueue.push_back(R);
        }
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
            auto R = S->requestQueue.front();
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
        int newRequestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, newRequestId, -1, -1, 0.0, newArrivalTime};
        pq.push(N);
    }
    else
    {
        threadObject currentThreadObject{requestId, threadId, arrivalTimeStamp, remainingTime};
        S->readyQ[coreId].push_back(currentThreadObject);
    }
    if(S->readyQ[coreId].size() > 0)
    {
        auto T = S->readyQ[coreId].front();
        S->readyQ[coreId].pop_front();
        S->coreStatus[coreId] = Status::BUSY;

        auto newRequestId = T.requestId;
        auto threadId = T.threadId;
        auto arrivalTimeStamp = T.arrivalTimeStamp;

        auto remainingTime = std::max(0.0,T.remainingTime - S->timeSlice);
        auto currentServiceQuantum = std::min(remainingTime, S->timeSlice);
        auto timeStamp = currentSimulationTime + currentServiceQuantum  + S->contextSwitchOverhead;

        if(S->readyQ[coreId].size() == 0 && requestId == newRequestId)
            remainingTime -= S->contextSwitchOverhead;

        Event N{eventType::DEPARTURE, timeStamp, newRequestId, coreId, threadId, arrivalTimeStamp };
        pq.push(N);
    }
}

/*
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

        double responseTime = currentSimulationTime - arrivalTimeStamp;
        M->currentResponseVector.push_back(responseTime);

        S->coreStatus[coreId] = Status::IDLE;

        auto newThinkTime = D.getThinkTime();
        auto newArrivalTime = currentSimulationTime + newThinkTime;
        int newRequestId = M->requestsHandled;
        M->requestsHandled += 1;
        Event N{eventType::ARRIVAL, newArrivalTime, newRequestId, -1, -1, 0.0, newArrivalTime};
        pq.push(N);

        if (S->readyQ[coreId].size() > 0)
        {
           
            auto next = S->readyQ[coreId].front();
            S->readyQ[coreId].pop_front();

            S->coreStatus[coreId] = Status::BUSY;

            auto remainingTime = std::max(0.0, next.remainingTime - S->timeSlice);
            auto timeStamp = currentSimulationTime + std::min(next.remainingTime, S->timeSlice) + S->contextSwitchOverhead;

            Event E{eventType::DEPARTURE, timeStamp, next.requestId, coreId, next.threadId, remainingTime, next.arrivalTimeStamp};
            pq.push(E);

            // take the request from request queue if present and add in the ready queue
            if (S->requestQueue.size() > 0)
            {
                auto request = S->requestQueue.front();
                S->requestQueue.pop_front();
                auto serviceTime = D.getServiceTime();
                threadObject T{request.requestId, threadId, request.arrivalTimeStamp, serviceTime};
                S->readyQ[coreId].push_back(T);
            }
            else
            {
                S->threadPool.push_back(threadId);
            }
        }
        else if (S->requestQueue.size())
        {
            // core is idle, there is no queue in the ready queue
            auto request = S->requestQueue.front();
            S->requestQueue.pop_front();
            auto serviceTime = D.getServiceTime();
            S->coreStatus[coreId] = Status::BUSY;

            auto remainingTime = std::max(0.0, serviceTime - S->timeSlice);
            auto timeStamp = currentSimulationTime + std::min(serviceTime, S->timeSlice) + S->contextSwitchOverhead;

            Event E{eventType::DEPARTURE, timeStamp, request.requestId, coreId, threadId, remainingTime, request.arrivalTimeStamp};
            pq.push(E);
        }
        else
        {
            S->threadPool.push_back(threadId);
        }
    }
    else
    {
        int coreId = nextEventObject.coreId;
        int requestId = nextEventObject.requestId;
        int threadId = nextEventObject.threadId;
        auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
        auto remainingTime = nextEventObject.remainingTime;
        //insert this thread in the ready queue
        threadObject T{requestId, threadId, arrivalTimeStamp, remainingTime};
        S->readyQ[coreId].push_back(T);
    
    
        //take one thread from ready queue and schedule at this core
        auto next = S->readyQ[coreId].front();
        S->readyQ[coreId].pop_front();
        auto timeStamp = currentSimulationTime + std::min(remainingTime, S->timeSlice) + S->contextSwitchOverhead;
        remainingTime = std::max(0.0, remainingTime - S->timeSlice);
        if (S->readyQ[coreId].size() == 0)
        {
            timeStamp -= S->contextSwitchOverhead;
        }
        //schedule departure
        Event e{eventType::DEPARTURE, timeStamp, next.requestId, coreId, next.threadId, remainingTime, next.arrivalTimeStamp};
        pq.push(e);
    }
}
*/
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
    //std::cout << "Queue Size " << Q.size() << std::endl;
    //for (auto it = Q.begin(); it != Q.end(); ++it)
    //    std::cout << "Request Id: " << it->requestId << "Arrival TimeStamp" << it->arrivalTimeStamp << std::endl;
}
void metrics::printMetrics()
{
    return;
}
void state::printState()
{
    M->printMetrics();
    // S->printServerState();
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
/*
void server::allocateCore(int &threadId, int &coreId)
{
    if(numberCores==1)
    {
        
        coreId = 0;
    }   //if core status idle: return true
        // add this thread in the readyQ of core
    else{
         coreId = threadId%numberCores;
        //coreId = coreStatus[id] == Status::IDLE ? id :-1;
        // if core status idle: return true
        //else add in core's ready queue : readyQ[coreId].push_back(threadObject)
    }
    
    //return false;
}
*/
/*
bool server::allocateThread(int &threadId)
{
    if(threadPool.size())
    {
        threadId = threadPool.front();
        threadPool.pop_front();
        return true;
    }
    threadId = -1;
    return false;
}
*/
/*
void state::context_switch()
{
    int coreId = nextEventObject.coreId;
    int requestId = nextEventObject.requestId;
    int threadId = nextEventObject.threadId;
    auto arrivalTimeStamp = nextEventObject.arrivalTimeStamp;
    auto remainingTime = nextEventObject.remainingTime;
    threadObject T{requestId, threadId, arrivalTimeStamp, remainingTime };
    S->readyQ[coreId].push_back(T);

    //take one thread from ready queue and schedule at this core
    auto next = S->readyQ[coreId].front();
    S->readyQ[coreId].pop_front();
    auto timeStamp = currentSimulationTime + std::min(remainingTime,S->timeSlice) + S->contextSwitchOverhead;
    remainingTime = std::max(0.0,remainingTime-S->timeSlice);
    if(S->readyQ[coreId].size()==0)
    {
        timeStamp-=S->contextSwitchOverhead;
    }
    if(remainingTime <= 0.0001)
    {
        //schedule departure
        Event e{eventType::DEPARTURE, timeStamp, next.requestId, coreId, next.threadId, remainingTime, next.arrivalTimeStamp};
        pq.push(e);
    }
    else{
        //schedule contextSwitch
        Event e{eventType::CONTEXT_SWITCH, timeStamp, next.requestId, coreId, next.threadId, remainingTime, next.arrivalTimeStamp};
        pq.push(e);
    } 
    
}
*/