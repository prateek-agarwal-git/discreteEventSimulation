#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <set>
#include <fstream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
namespace pt = boost::property_tree;
enum class eventType
{
        ARRIVAL,
        DEPARTURE,
        TIMEOUT
};

enum class Distribution
{
        NORMAL,
        EXPONENTIAL,
        UNIFORM,
        ERLANG
};

enum class Status
{
        BUSY,
        IDLE
};
struct Event
{
        eventType event;
        float timeStamp;
        int requestId;
        int threadId; // this server should be made idle once the request departs. Departure
};
struct metrics
{
        metrics() {}
        metrics(int numRuns, int totalRequests)
        {
                for (int i = 0; i < totalRequests; i += 1)
                {
                        std::vector<double> temp{};
                        for (int j = 0; j < numRuns; j += 1)
                        {
                                temp.push_back(-1.0);
                                // -1.0 represent request drop
                        }
                        responseTimes.push_back(temp);
                }
        }
        int timedOutRequests;
        int successfulRequests;
        double coreUtilization;
        double requestDropRate;
        std::vector<std::vector<double>> responseTimes;
};
struct client
{
        client() {}

        client(double meanThinkTime, int numberOfUsers, double meanRequestTimeout) : meanThinkTime{meanThinkTime}, numberOfUsers{numberOfUsers}, meanRequestTimeout{meanRequestTimeout}
        {
        }
        double meanThinkTime;
        int numberOfUsers;
        double meanRequestTimeout;
};

struct Experiment
{
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
struct queueObject
{
        int requestId;
        double arrivalTimeStamp;
};
struct server
{
        server()
        {
                std::unique_ptr<Status[]> temp{new Status[numberThreads]};
                threads = std::move(temp);
                for (auto i = 0; i < numberThreads; i += 1)
                        threads[i] = Status::IDLE;
                nextThread = 0;
        }
        server(int numberThreads, double meanServiceTime,
               double contextSwitchOverhead, int queueCapacity) : numberThreads{numberThreads},
                                                                  meanServiceTime{meanServiceTime}, queueCapacity{queueCapacity},
                                                                  contextSwitchOverhead{contextSwitchOverhead}
        {
        }

        bool allocateThread(int &threadId);

        std::deque<queueObject> Q;
        int numberThreads;
        int nextThread;
        std::unique_ptr<Status[]> threads;
        double meanServiceTime;
        double contextSwitchOverhead;
        int queueCapacity;
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
        int numberRequestsDepartedorTimedOut;
        int numberOfDroppedRequests;
        double currentSimulationTime;
        Event nextEventObject;
        std::random_device rd;
        void arrival();
        void readConfig();
        void readClientConfig(const pt::ptree &configTree);
        void readServerConfig(const pt::ptree &configTree);
        void readExperimentConfig(const pt::ptree &configTree);
        void readDistributionConfig(const pt::ptree &configTree);
        void departure();
        void requestTimeout();
        void printState();
        void updateTimeandNextEvent();
        bool isAnyCoreIdle();
        void initialize();
        std::unique_ptr<metrics> M;

        std::unique_ptr<client> C;
        std::unique_ptr<server> S;
        distributions D;
        std::unique_ptr<Experiment> E;
        eventType nextEventType;
};
struct distributions
{
        Distribution thinkTime;
        Distribution serviceTime;
        Distribution timeout;
};