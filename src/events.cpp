
#include "../include/main.h"
#define CONFIG_FILE "configFile.json"

void distributions::readDistributionConfig(const pt::ptree &configTree)
{

        const pt::ptree &distributionTree = configTree.get_child("distribution");
        const pt::ptree &thinkTimeTree = distributionTree.get_child("thinkTime");
        const pt::ptree &timeOutTree = distributionTree.get_child("timeOut");
        const pt::ptree &serviceTimeTree = distributionTree.get_child("serviceTime");
        thinkTime.type = thinkTimeTree.get<std::string>("type");
        thinkTime.p1 = thinkTimeTree.get<double>("p1");
        thinkTime.p2 = thinkTimeTree.get<double>("p2");

        serviceTime.type = serviceTimeTree.get<std::string>("type");
        serviceTime.p1 = serviceTimeTree.get<double>("p1");
        serviceTime.p2 = serviceTimeTree.get<double>("p2");

        timeOut.type = timeOutTree.get<std::string>("type");
        timeOut.p1 = timeOutTree.get<double>("p1");
        timeOut.p2 = timeOutTree.get<double>("p2");
}
void Experiment::readExperimentConfig(const pt::ptree &configTree)
{

        const pt::ptree &experimentTree = configTree.get_child("experiment");
        runs = experimentTree.get<int>("runs");
        requestsPerRun = experimentTree.get<int>("requestPerRun");
}
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
                auto newServiceTime =0.0;
                // auto newServiceTime = S->meanServiceTime + S->contextSwitchOverhead;
                Event N{eventType::DEPARTURE, newServiceTime, nextEventObject.requestId, threadId};
                pq.push(N);
                // give a timeout event here
        }
        else if (S->Q.size() == S->queueCapacity)
        {
                M->droppedRequests += 1;
        }
        else
        {
                queueObject tempObject{nextEventObject.requestId, nextEventObject.timeStamp};
                S->Q.push_back(tempObject);
                // give a timeout event here
        }
}
void client::readClientConfig(const pt::ptree &configTree)
{
        const pt::ptree &clientTree = configTree.get_child("clientStation");
        numberOfUsers = clientTree.get<int>("numberOfUsers");
}
void server::readServerConfig(const pt::ptree &configTree)
{
        const pt::ptree &serverTree = configTree.get_child("serverStation");
        contextSwitchOverhead = serverTree.get<double>("contextSwitchOverhead");
        numberThreads = serverTree.get<int>("numberThreads");
        queueCapacity = serverTree.get<int>("queueCapacity");
}

void state::departure() {}
void state::requestTimeout() {}
void state::printState() {}
void state::updateTimeandNextEvent() {}

void state::initialize()
{
        M->requestsHandled = M->droppedRequests = M->successfulRequests = M->timedOutRequests = 0;
        currentSimulationTime = 0.0;
        for (auto i = 0; i < C->numberOfUsers; i += 1)
        {
                double thinkTime =0.0; 
                // double thinkTime = C->meanThinkTime;
        }
}

void state::printConfig()
{
        C->printClientConfig();
        S->printServerConfig();
        E->printExperimentConfig();
        D.printDistributionConfig();
}

void Experiment::printExperimentConfig()
{
        std::cout << "Experiment Config:" << std::endl;
        std::cout << "runs = " << runs << std::endl;
        std::cout << "requestsPerRun = " << requestsPerRun<< std::endl;
        std::cout << std::endl;
}
void client::printClientConfig()
{
        std::cout << "Client Config:" << std::endl;
        std::cout << "numberOfUsers = " << numberOfUsers << std::endl;
        std::cout << std::endl;
}
void server::printServerConfig()
{
        std::cout << "Server Config:" << std::endl;
        std::cout << "numberThreads = " << numberThreads << std::endl;
        std::cout << "nextThread = " << nextThread << std::endl;
        std::cout << "queueCapacity = " << queueCapacity << std::endl;
        std::cout << "contextSwitchOverhead = " << contextSwitchOverhead << std::endl;
        std::cout << "Q.size() =  " << Q.size() << std::endl;
        std::cout << "threads.size() =  " << threads.size() << std::endl;
        for (auto i = 0 ; i < threads.size(); i+=1){
                if (threads[i] == Status::IDLE){
                        std::cout<< i<<"th thread is IDLE " <<std::endl;
                }
        }
        std::cout << std::endl;
}
void distributions::printDistributionConfig() {

        std::cout << "Distribution Config:" << std::endl;

        std::cout << "ThinkTime:" << std::endl;
        std::cout << "Type:"<<thinkTime.type << std::endl;
        std::cout << "P1(mean):"<<thinkTime.p1 << std::endl;
        std::cout << "P2"<<thinkTime.p2 << std::endl;

        std::cout << "serviceTime:" << std::endl;
        std::cout << "Type:"<<serviceTime.type << std::endl;
        std::cout << "P1(mean):"<<serviceTime.p1 << std::endl;
        std::cout << "P2"<<serviceTime.p2 << std::endl;


        std::cout << "timeOut:" << std::endl;
        std::cout << "Type:"<<timeOut.type << std::endl;
        std::cout << "P1(mean):"<<timeOut.p1 << std::endl;
        std::cout << "P2"<<timeOut.p2 << std::endl;

        std::cout << std::endl;
}
void state::readConfig()
{
        pt::ptree configTree;
        pt::read_json(CONFIG_FILE, configTree);
        //reading client station
        C->readClientConfig(configTree);
        //reading server station
        S->readServerConfig(configTree);
        //reading Experiment parameters
        E->readExperimentConfig(configTree);
        // reading Distribution details
        D.readDistributionConfig(configTree);
}