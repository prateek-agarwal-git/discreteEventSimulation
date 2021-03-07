//run command is : ./a.out < input.in

#include <iostream>
#include <random>
#include <fstream>
using namespace std;

void expon(default_random_engine e, float rate)
{
  exponential_distribution<double> exp(rate);
  for (int i=0;i<10;i++)
  	cout<<exp(e)<<endl;
}

void uniform(default_random_engine e, int a, int b)
{
  uniform_int_distribution<int> uni(a,b);
  //for real numbers
  // uniform_real_distribution<double> uni(a,b)
  for (int i=0;i<10;i++)
  	cout<<uni(e)<<endl;
 
}

void normal(default_random_engine e, float mean, float sd)
{
  normal_distribution<double> norm(mean,sd );
  for (int i=0;i<10;i++)
  	cout<<norm(e)<<endl;
}

int main()
{

	/*fstream ipfile;
	ipfile.open("input.in", ios::in); //opening file in read mode
	*/
	
	char distribution;
	cin>>distribution;
	
	//use in case of exponential and normal distribution, as required.
	float mean; // float sd; for normal distribution 
	/*int S,E;  interval of uniform distribution 
	cin>>S>>E; */
	cin>>mean;
	
	unsigned int think_time, M; //M is no. of users
	float request_timeout;
	cin>>M>>think_time>>request_timeout;
	
	int threads_at_server, queue_cap;
	float  context_switch_overhead;
	cin>>threads_at_server>>queue_cap>>context_switch_overhead;
	
	int runs;
	cin>>runs;

	unsigned seed;	
	for(auto i=0;i<runs;i++)
	{	cin>>seed;
		default_random_engine eg(seed); // default generator engine
		
		switch(distribution){
		case 'E':
			expon(eg, (1/mean));
			break;
		case 'U':
			//uniform(eg, S, E);
			break;
		case 'N':
			//normal(eg, mean, sd);
			break;
		}
	}
	
	
return 0;
}

/*
Input file pattern:
Line 1: Distribution code ( 'E' for Exponetial, 'U' for uniform, 'N' for normal )
Line 2: parameters for service time distribution(for uniform distribution [S, E]  , for Exponential distribution [mean], for normal distribution [mean, sd])
Line 3: No_of_users, think_time, request_time_out
Line 4: Threads_at_server, queue_capacity, context_switch_overhead
Line 5: no. of runs(N)
Next N lines: seed values

*/
