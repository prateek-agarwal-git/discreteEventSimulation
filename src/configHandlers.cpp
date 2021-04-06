#include "../include/main.h"
#define CONFIG_FILE "configFile.json"
void state::printConfig()
{
    C->printClientConfig();
    //sleep(10);
    S->printServerConfig();
    //sleep(10);
    E->printExperimentConfig();
    //sleep(10);
    D.printDistributionConfig();
    //sleep(10);
}

void client::readClientConfig(const pt::ptree &configTree)
{
    const pt::ptree &clientTree = configTree.get_child("clientStation");
    numberOfUsers = clientTree.get<int>("numberOfUsers");
}
void server::initializeServer()
{
    coreStatus.clear();
    threadPool.clear();
    requestQueue.clear();
    for (auto i = 0; i < numberThreads; i += 1)
        threadPool.push_back(i);
    for (auto i = 0; i < numberCores; i += 1)
        coreStatus.push_back(Status::IDLE);
    for (auto i = 0; i < numberCores; i += 1)
        readyQ[i].clear();
}
void server::readServerConfig(const pt::ptree &configTree)
{
    const pt::ptree &serverTree = configTree.get_child("serverStation");
    contextSwitchOverhead = serverTree.get<double>("contextSwitchOverhead");
    numberCores = serverTree.get<int>("numberCores");
    numberThreads = serverTree.get<int>("numberThreads");
    timeSlice = serverTree.get<double>("timeSlice");
    requestQueueCapacity = serverTree.get<int>("requestQueueCapacity");

}
void Experiment::printExperimentConfig()
{
    std::cout << "Experiment Config:" << std::endl;
    std::cout << "runs = " << runs << std::endl;
    std::cout << "requestsPerRun = " << requestsPerRun << std::endl;
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
    std::cout << "numberCores = " << numberCores << std::endl;
    std::cout << "numberThreads = " << numberThreads << std::endl;
    std::cout << "contextSwitchOverhead = " << contextSwitchOverhead << std::endl;
    std::cout << "readyQ.size() =  " << readyQ.size() << std::endl;
    std::cout << "coreStatus.size() =  " << coreStatus.size() << std::endl;
std::cout << "requestQueueCapacity = "<< requestQueueCapacity<<std::endl;
    for (auto i = 0UL; i < coreStatus.size(); i += 1)
    {
        if (coreStatus[i] == Status::IDLE)
        {
            std::cout << i << "th thread is IDLE " << std::endl;
        }
    }
    std::cout << "timeSlice =" << timeSlice << " seconds." << std::endl;
    std::cout << std::endl;
}
void distributions::printDistributionConfig()
{

    std::cout << "Distribution Config:" << std::endl;

    std::cout << "ThinkTime:" << std::endl;
    std::cout << "Type:" << thinkTime.type << std::endl;
    std::cout << "P1: " << thinkTime.p1 << std::endl;
    std::cout << "P2: " << thinkTime.p2 << std::endl;

    std::cout << "serviceTime:" << std::endl;
    std::cout << "Type:" << serviceTime.type << std::endl;
    std::cout << "P1: " << serviceTime.p1 << std::endl;
    std::cout << "P2: " << serviceTime.p2 << std::endl;

    std::cout << "timeOut:" << std::endl;
    std::cout << "Type:" << timeOut.type << std::endl;
    std::cout << "P1: " << timeOut.p1 << std::endl;
    std::cout << "P2: " << timeOut.p2 << std::endl;

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

    seed = distributionTree.get<uint32_t>("seed");
}
void Experiment::readExperimentConfig(const pt::ptree &configTree)
{
    const pt::ptree &experimentTree = configTree.get_child("experiment");
    runs = experimentTree.get<int>("runs");
    requestsPerRun = experimentTree.get<int>("requestPerRun");
}