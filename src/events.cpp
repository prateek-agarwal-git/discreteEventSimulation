
#include "../include/main.h"
bool server::allocateThread(int &threadId)
{
        for (auto count = 0; count < numberThreads; count += 1)
        {
                if (threads[nextThread] == Status::IDLE)
                {
                        threads[nextThread] == Status::BUSY;
                        threadId = nextThread;
                        nextThread+=1;
                        nextThread%=numberThreads;
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
void state::arrival()
{
        int threadId;
        currentSimulationTime = nextEventObject.timeStamp;
        if (S->Q.size() == 0 && S->allocateThread(threadId))
        {
                // request given to a thread. generate a new service time with context switchoverhead
                auto newServiceTime = S->meanServiceTime + S->contextSwitchOverhead;
                Event N{eventType::DEPARTURE, currentSimulationTime + newServiceTime, nextEventObject.requestId, threadId};
                pq.push(N);
                // give a timeout event here
        }
        else if (S->Q.size() == S->queueCapacity)
        {
                numberOfDroppedRequests += 1;
        }
        else
        {
                queueObject tempObject{nextEventObject.requestId, nextEventObject.timeStamp};
                S->Q.push_back(tempObject);
                // give a timeout event here
        }
}
void state::departure() {}
void state::requestTimeout() {}
void state::printState() {}
void state::updateTimeandNextEvent() {}

void state::initialize()
{
        numberRequestsDepartedorTimedOut = 0;
        currentSimulationTime = 0.0;
        for (auto i = 0; i < C->numberOfUsers; i += 1)
        {
                double thinkTime = C->meanThinkTime;
        }
}