
#include "../include/main.h"
#define CONFIG_FILE "configFile.json"
bool server::allocateThread(int &threadId)
{
        for (auto count = 0; count < numberThreads; count += 1)
        {
                if (threads[nextThread] == Status::IDLE)
                {
                        threads[nextThread] == Status::BUSY;
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
void state::readClientConfig(const pt::ptree &configTree)
{
        const pt::ptree &clientTree = configTree.get_child("clientStation");
        C->numberOfUsers = clientTree.get<int>("numberOfUsers");
        C->meanThinkTime = clientTree.get<double>("meanThinkTime");
        C->meanRequestTimeout = clientTree.get<double>("meanRequestTimeout");
}
void state::readServerConfig(const pt::ptree &configTree)
{
        const pt::ptree &serverTree = configTree.get_child("serverStation");
        S->contextSwitchOverhead = serverTree.get<double>("contextSwitchOverhead");
        S->meanServiceTime = serverTree.get<double>("meanServiceTime");
        S->numberThreads = serverTree.get<int>("numberThreads");
        S->queueCapacity = serverTree.get<int>("queueCapacity");
}

void state::readExperimentConfig(const pt::ptree &configTree){
        const pt::ptree &experimentTree = configTree.get_child("experiment");
        E->runs = experimentTree.get<int>("runs");
        E->requestsPerRun = experimentTree.get<int>("requestPerRun");
}

void state::readDistributionConfig(const pt::ptree &configTree){
        const pt::ptree &clientTree = configTree.get_child("distributions");

}
void state::readConfig()
{
        pt::ptree configTree;
        pt::read_json(CONFIG_FILE, configTree);
        //reading client station
        readClientConfig(configTree);
        //reading server station
        readServerConfig(configTree);
        //reading Experiment parameters
        readExperimentConfig(configTree);
        // reading Distribution details
        readDistributionConfig(configTree);
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