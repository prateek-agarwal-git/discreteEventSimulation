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
	cout<<"Disrtibution is "<<distribution<<endl;
	
	//use in case of exponential and normal distribution, as required.
	// float mean; float sd; 
	int S,E; // interval of uniform distribution
	cin>>S>>E;
	cout<<"next inputs are"<<S<<" "<<E<<" "<<endl;
	
	unsigned int think_time, M, runs; //M is no. of users
	cin>>M>>think_time;
	cout<<"No of users:"<<M<<" think time="<<think_time<<endl;
	cin>>runs;
	cout<<"No. of runs is:"<<runs<<endl;
	
	unsigned seed;
	cin>>seed;
	cout<<"Seed value is: "<<seed<<endl;
	
	
	default_random_engine eg(seed); // default generator engine
	
	switch(distribution){
	case 'E':
		//expon(eg, (1/mean));
		break;
	case 'U':
		uniform(eg, S, E);
		break;
	case 'N':
		//normal(eg, mean, sd);
		break;
	}
	
	
return 0;
}

/*
Input file pattern:
Line 1: Distribution code ( 'E' for Exponetial, 'U' for uniform, 'N' for normal )
Line 2: parameters for service time distribution(for uniform distribution [S, E]  , for Exponential distribution [mean], for normal distribution [mean, sd])
Line 3: No_of_users, think_time
Line 4: no. of runs(N)
Next N lines: seed values

*/
