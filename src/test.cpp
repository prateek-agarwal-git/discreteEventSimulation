#include <queue>
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;
int main(){
  vector<double> a{};
  vector<vector<double> > b{};
  a.push_back(1);
  a.push_back(2);
  b.emplace_back(a);
  a.clear();
  a[0] =3;
  a[1] = 4;
 // a.push_back(3);
 // a.push_back(4);
//  b.push_back(a);
cout<<b[0][0]<<endl;
return 0;
}
//class abcd
//{
//public:
//        abcd(int i) : i{i} {}
//
//private:
//        int i;
//};
//struct Event
//{
//
//        int typeOfEvent;
//        float timeStamp;
//        Event(int typeOfEvent, float timeStamp) : typeOfEvent{typeOfEvent}, timeStamp{timeStamp} {}
//};
//struct eventQueue
//{
//        std::priority_queue<Event> pq;
//
//        // this will be the priority queue in which both arrival and departure times are stored.
//};
//struct compareTimestamps
//{
//        bool operator()(Event const &e1, Event const &e2)
//        {
//                return e1.timeStamp > e2.timeStamp;
//        }
//};
//
//int main()
//{
//
//        std::priority_queue<Event, std::vector<Event>, compareTimestamps> pq;
//        auto ts = 34.0;
//        auto typeOfEvent = 1;
//        for (int i = 0; i < 10; i++)
//        {
//                pq.push(Event(typeOfEvent, ts));
//                typeOfEvent = 1 - typeOfEvent;
//                ts -= 1;
//        }
//        for (int i = 0; i < 10; i += 1)
//        {
//                auto e = pq.top();
//                pq.pop();
//                std::cout << e.timeStamp << e.typeOfEvent << std::endl;
//        }
//        std::unordered_map<uint64_t, uint64_t> *tmap;
//        tmap = new std::unordered_map<uint64_t, uint64_t>();
//        (*tmap)[1] = 2;
//        (*tmap)[2] = 3;
//        (*tmap)[4] = 7;
//        (*tmap)[8] = 2;
//        for (auto it = (*tmap).begin(); it != (*tmap).end(); ++it)
//        {
//                std::cout << it->first << " : " << it->second << std::endl;
//        }
//        std::cout << (*tmap).size() << std::endl;
//        (*tmap).clear();
//
//        std::cout << (*tmap).size() << std::endl;
//        std::cout << "hello world" << std::endl;
//
//        for (auto it = (*tmap).begin(); it != (*tmap).end(); ++it)
//        {
//                std::cout << it->first << " : " << it->second << std::endl;
//        }
//        abcd A(1);
//        // std::cout<<A.i<<std::endl;
//        auto  x = 7
//        + 12;
//        std::cout<<x<<std::endl;
//
//        return 0;
//}
