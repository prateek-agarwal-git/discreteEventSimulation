#include <cstdio>
#include <vector>
#include <queue>
#include <random>
#include <set>
#include <iostream>
#include <fstream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <fstream>
namespace pt = boost::property_tree;
enum class eventType
{
    ARRIVAL,
    DEPARTURE,
    TIMEOUT
};

enum class Status
{
    BUSY,
    IDLE
};
struct Distribution
{
    std::string type;
    double p1;
    double p2;
};
struct Event
{
    eventType event;
    double timeStamp;
    int requestId;
    int threadId;
    double remainingTime;
    double arrivalTimeStamp; // this server should be made idle once the request departs. Departure
};
struct metrics
{
    metrics()
    {
    }
    void printMetrics();
    double areaNumInQueue;
    double areaServerStatus;
    double testTime;
    int requestsHandled;
    int requestsDepartedorDropped;
    std::set<int> timedOutRequests;
    std::set<int> successfulRequests;
    std::set<int> droppedRequests;
    double accumulatedUtilization;
    double accumulatedAverageNumberinQueue;
    int accumulatedTimedOutRequests;
    int accumulatedSuccesfulRequests;
    int accumulatedDroppedRequests;
    double throughPut;
    double goodPut;
    double badPut;
    double dropRate;
    int currentRun;
    double coreUtilization;
    std::vector<std::vector<double>> responseTimes;
    std::vector<double> currentResponseVector;
};
struct client
{
    client() {}
    void printClientConfig();
    void readClientConfig(const pt::ptree &configTree);
    int numberOfUsers;
};

struct Experiment
{

    void printExperimentConfig();
    void readExperimentConfig(const pt::ptree &configTree);
    int runs;
    int requestsPerRun;
};
struct compareTimestamps
{
    bool operator()(Event const &e1, Event const &e2)
    {
        return e1.timeStamp > e2.timeStamp;
    }
};
struct threadObject
{
    int requestId;
    double arrivalTimeStamp;
    double remainingTime;
};
struct server
{
    server() {}

    void initializeServer();
    void printServerState();
    void readServerConfig(const pt::ptree &configTree);
    bool allocateCore(int &threadId);
    void printServerConfig();
    int countBusyCores();
    std::deque<threadObject> Q;
    int numberCores;
    int nextCore;
    std::vector<Status> cores;
    double contextSwitchOverhead;
    uint64_t numberThreads;
    double timeSlice;
};
struct preComputedTimes
{
    std::vector<double> times{};
    int currentIndex;
};

struct distributions
{
    distributions()
    {
    }
    double getThinkTime();
    double getServiceTime();
    double getTimeOut();
    uint32_t seed;
    void readDistributionConfig(const pt::ptree &configTree);

    double getTime(preComputedTimes &P);
    void generateParticular(preComputedTimes &P, Distribution &D, int num);
    void generateTimeHelper(int);
    void initialize();
    void printDistributionConfig();
    preComputedTimes thinkTimeValues;
    preComputedTimes serviceTimeValues;
    preComputedTimes timeOutValues;
    std::mt19937_64 mt_engine;
    Distribution thinkTime;
    Distribution serviceTime;
    Distribution timeOut;
};
struct state
{
    state()
    {
        std::unique_ptr<server> tempS{new server};
        S = std::move(tempS);
        std::unique_ptr<client> tempC{new client};
        C = std::move(tempC);
        std::unique_ptr<metrics> tempM{new metrics};
        M = std::move(tempM);
        std::unique_ptr<Experiment> tempE{new Experiment};
        E = std::move(tempE);
    }
    std::priority_queue<Event, std::vector<Event>, compareTimestamps> pq;
    std::set<int> requestsAtServer;
    double currentSimulationTime;
    double timeOfLastEvent;

    void updateAccumulators();
    void writeStats();
    Event nextEventObject;
    std::random_device rd;

    void initializeStats();
    void arrival();
    void readConfig();
    void printConfig();
    void departure();
    void requestTimeout();
    void generateTimes();
    void printState();
    void updateTimeandNextEvent();
    // bool isAnyCoreIdle();
    void initialize();

    void updateStats();
    std::unique_ptr<metrics> M;
    std::unique_ptr<client> C;
    std::unique_ptr<server> S;
    distributions D;
    std::unique_ptr<Experiment> E;
    eventType nextEventType;
};
