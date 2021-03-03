#include <queue>
#include <iostream>


struct Event{

        int typeOfEvent;
        float timeStamp;
        Event(int typeOfEvent, float timeStamp):typeOfEvent{typeOfEvent}, timeStamp{timeStamp}{}

};
struct eventQueue{
        std::priority_queue<Event> pq;

// this will be the priority queue in which both arrival and departure times are stored.
};
struct compareTimestamps{
        bool operator() (Event const & e1, Event const & e2){
                return e1.timeStamp> e2.timeStamp;
        }
};

int main(){

        std::priority_queue <Event,std::vector<Event>, compareTimestamps> pq;
        auto ts = 34.0;
        auto typeOfEvent = 1;
        for (int i = 0; i < 10; i++){
                pq.push(Event(typeOfEvent, ts));
                typeOfEvent = 1- typeOfEvent;
                ts-=1;
        }
        for (int i = 0; i < 10; i+=1){
                auto e = pq.top();
                pq.pop();
                std::cout<<e.timeStamp<<e.typeOfEvent<<std::endl;
        }
        return 0;
}