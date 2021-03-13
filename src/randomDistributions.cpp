#include <iostream>
#include <random>
#include <iomanip>
int main()
{
        std::mt19937_64 mt_engine{1};
        mt_engine.seed(1);
        std::uniform_int_distribution<int> d{1, 10};
        for (auto i = 0; i < 30; i += 1)
        {
                std::cout << std::setw(2) << d(mt_engine) << " ";
        }
        std::cout << std::endl;

        mt_engine.seed(12);
        for (auto i = 0; i < 30; i += 1)
        {
                std::cout << std::setw(2) << d(mt_engine) << " ";
        }
        std::cout << std::endl;
        std::normal_distribution<double> nd{40, 5};
        std::exponential_distribution<double> ed{0.01};
        int a[80];
        for (int i = 0; i < 80; i += 1)
        {
                a[i] = 0;
        }
        auto s = 0.0;
        for (int i = 0; i < 1000000; i += 1)
        {
                auto x = ed(mt_engine);
                s += x;
                auto t = static_cast<int>(x);
                if (t >= 0 && t < 80)
                {
                        a[t] += 1;
                }
        }
        for (int i = 0; i < 80; i += 1)
        {
                // std::cout<<a[i]<<std::endl;
                for (auto j = 0; j < a[i]; j += 1000)
                {
                        std::cout << "*";
                }
                std::cout << std::endl;
        }
        std::cout << (s / 1000000) << std::endl;
        return 0;
}