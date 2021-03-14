
#include "../include/main.h"
#define CONFIG_FILE "configFile.json"
void state::generateTimes()
{
        D.generateTimeHelper(E->requestsPerRun);
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
                        P.times.push_back(temp);
                }
        }
        else if (D.type == "exponential")
        {
                // p2 is the minimum value,p1 is the parameter.
                std::exponential_distribution<double> ed{D.p1};
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
void state::updateTimeandNextEvent() {
        //Calculate time since last event and area
        double timeSinceLastEvent = currentSimulationTime - timeOfLastEvent;
        timeOfLastEvent = currentSimulationTime;
        int numberInServerQueue = S->Q.size();
        int busyThreads  = S->countBusyThreads();
        areaNumInQueue = timeSinceLastEvent * numberInServerQueue ; 
        areaServerStatus = timeSinceLastEvent * (busyThreads/ S->numberThreads);

        // next event
        nextEventObject = pq.top();
        pq.pop();
        nextEventType = nextEventObject.event;
}
void state::arrival()
{
        int threadId;
        currentSimulationTime = nextEventObject.timeStamp;
        if (S->Q.size() == 0 && S->allocateThread(threadId))
        {
                // request given to a thread. calculate remaining time with context switchoverhead
                auto serviceTime = D.getServiceTime();
                auto remainingTime = serviceTime - S->timeSlice;
                //get time at which this request will depart from server.
                auto newTimeStamp = currentSimulationTime + S->timeSlice +  S->contextSwitchOverhead;
                Event N{eventType::DEPARTURE, newTimeStamp, nextEventObject.requestId, threadId, remainingTime, newTimeStamp};
                requestsAtServer.insert(nextEventObject.requestId);
                pq.push(N);
                
                // give a timeout event here
                auto timeOut = currentSimulationTime + D.getTimeOut();
                Event T{eventType::TIMEOUT,timeOut, nextEventObject.requestId,-1,0.0,nextEventObject.arrivalTimeStamp};
                pq.push(T);
        }
        else if (S->Q.size() == S->queueCapacity)
        {
                M->droppedRequests.insert(nextEventObject.requestId);
                //schedule TIMEOUT for this request.
                auto timeOut = D.getTimeOut();
                auto newTimeStamp = currentSimulationTime + timeOut;
                Event N{eventType::TIMEOUT, newTimeStamp,nextEventObject.requestId, -1,0.0, nextEventObject.arrivalTimeStamp};
                pq.push(N);

        }
        else
        {
                //adding in server Queue
                auto ServiceTime = D.getServiceTime();
                queueObject tempObject{nextEventObject.requestId, nextEventObject.arrivalTimeStamp,ServiceTime};
                S->Q.push_back(tempObject);
                // give a timeout event here
                auto timeOut = D.getTimeOut();
                Event N{eventType::TIMEOUT, timeOut, nextEventObject.requestId, -1, ServiceTime, nextEventObject.arrivalTimeStamp};
                requestsAtServer.insert(nextEventObject.requestId);
                pq.push(N);
        }
        
}

void state::departure() {
        currentSimulationTime = nextEventObject.timeStamp;
        S->threads[nextEventObject.threadId] = Status::IDLE;
        if(nextEventObject.remainingTime == 0.0)
        {       
                M->successfulRequests.insert(nextEventObject.requestId);
                //calculate response time.

                //delete from requestsAtServer set

                //schedule next Arrival from current time.
                double newThinkTime = D.getThinkTime();
                double newArrivalTime = currentSimulationTime + newThinkTime;
                int requestId = M->requestsHandled;
                M->requestsHandled+=1;
                Event N{eventType::ARRIVAL,newArrivalTime,requestId,-1,0.0,0.0};
                pq.push(N);
        }
        else if(S->Q.size()==0){
                //Queue is empty
        }
        else if(S->Q.size() == S->queueCapacity){
                //queue is full
                //add request id in dropped requests, timeout event is already there in pq
                M->droppedRequests.insert(nextEventObject.requestId);
                
                
        }
        else
        {
                // add this request in the Q
                queueObject currentRequest{nextEventObject.requestId, nextEventObject.timeStamp, nextEventObject.remainingTime};
                S->Q.push_back(currentRequest);

                // take one request from Q and schedule for that request departure.
                auto newRequest = S->Q.front();
                int requestId = nextEventObject.requestId;
                int threadId = nextEventObject.threadId;
                //double timeStamp = nextEventObject.timeStamp;
                double serviceTime = nextEventObject.remainingTime > S->timeSlice ? (nextEventObject.remainingTime - S->timeSlice) : nextEventObject.remainingTime;
                double remainingTime = serviceTime > 0.0 ? serviceTime : 0.0;
                double departureTime = serviceTime + currentSimulationTime;
                Event N {eventType::DEPARTURE, departureTime, requestId, threadId, remainingTime,newRequest.arrivalTimeStamp };
                pq.push(N);
                S->Q.pop_front();
                
                
        }
}
void state::requestTimeout() {

}
void state::printState() {}


void state::initialize()
{
        M->requestsHandled = 0;
        currentSimulationTime = 0.0;
        timeOfLastEvent = 0.0;
        areaNumInQueue = 0.0;
        areaServerStatus = 0.;
        M->timedOutRequests.empty();
        M->droppedRequests.empty();
        M->successfulRequests.empty();
        S->initializeServer();
        D.initialize();
        generateTimes();
        for (auto i = 0; i < C->numberOfUsers; i += 1)
        {
                double thinkTime = D.getThinkTime();
                auto requestId = M->requestsHandled;
                M->requestsHandled+=1;
                Event N{eventType::ARRIVAL, thinkTime,requestId,-1,0.0,0.0 };
                pq.push(N);
                // double thinkTime = C->meanThinkTime;
        }
        std::cout<<"\nFinished Initialization\n"<<std::endl;
        /*
        for (auto i = 0; i < C->numberOfUsers; i += 1)
        {
                auto topElement = pq.top();
                std::cout<<"request id: "<<topElement.requestId<<"  ,Think time: "<<topElement.timeStamp<<std::endl;
                pq.pop();
                // double thinkTime = C->meanThinkTime;
        }
        */
}

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

        for (auto i = 0; i < numberThreads; i += 1)
                threads.push_back(Status::IDLE);
        nextThread = 0;
}
void server::readServerConfig(const pt::ptree &configTree)
{
        const pt::ptree &serverTree = configTree.get_child("serverStation");
        contextSwitchOverhead = serverTree.get<double>("contextSwitchOverhead");
        numberThreads = serverTree.get<int>("numberThreads");
        queueCapacity = serverTree.get<int>("queueCapacity");
        timeSlice = serverTree.get<double>("timeSlice");
}
int server::countBusyThreads()
{
        int count=0;
        for (auto i=0; i < numberThreads; i++)
        {
                if( threads[i] == Status::BUSY)
                        count++;
        }
        return count;
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
        std::cout << "numberThreads = " << numberThreads << std::endl;
        std::cout << "nextThread = " << nextThread << std::endl;
        std::cout << "queueCapacity = " << queueCapacity << std::endl;
        std::cout << "contextSwitchOverhead = " << contextSwitchOverhead << std::endl;
        std::cout << "Q.size() =  " << Q.size() << std::endl;
        std::cout << "threads.size() =  " << threads.size() << std::endl;
        for (auto i = 0; i < threads.size(); i += 1)
        {
                if (threads[i] == Status::IDLE)
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
        std::cout << "P1(mean):" << thinkTime.p1 << std::endl;
        std::cout << "P2" << thinkTime.p2 << std::endl;

        std::cout << "serviceTime:" << std::endl;
        std::cout << "Type:" << serviceTime.type << std::endl;
        std::cout << "P1(mean):" << serviceTime.p1 << std::endl;
        std::cout << "P2" << serviceTime.p2 << std::endl;

        std::cout << "timeOut:" << std::endl;
        std::cout << "Type:" << timeOut.type << std::endl;
        std::cout << "P1(mean):" << timeOut.p1 << std::endl;
        std::cout << "P2" << timeOut.p2 << std::endl;

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
double distributions::getTime(preComputedTimes &P)
{
        auto temp = P.times[P.currentIndex];
        P.currentIndex += 1;
        P.currentIndex %= P.times.size();
        return temp>0 ? temp : -1*temp;
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
void distributions::initialize()
{
        mt_engine.seed(seed);
        pt::ptree configTree;
        pt::read_json(CONFIG_FILE, configTree);
        readDistributionConfig(configTree);
        //printDistributionConfig();
}