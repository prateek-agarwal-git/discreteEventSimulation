#!/bin/env python3
import heapq
import random
from pathlib import Path 
NUM_SERVERS=4
Q_LIMIT=2000 
BUSY=1
IDLE=0
MAXRUNS=50
MAXDELAYS=5000
# Event types 1 for arrival , 2 for departure
nextEventType=0
timedoutRequests=0 
numRequests=5000
requestsHandled=0 
#num_delays_required=0
numberOfEvents=2
numbersInQueue=0
serverStatus=IDLE
numRepetitions=0
areaNumbersInQueue=0
areaServerStatus=0
meanInterArrival= 0.0
meanServiceTime=0.0
simulationTime=0
arrivalTimes=[0 for i in range(Q_LIMIT + 1)]
time_last_event=0
time_next_event=0
total_of_delays=0
alldelays=[[0 for in range(MAXDELAYS)] for j in range(MAXRUNS)];
infile=0
outfile=0
delayfile=0
demo=0
run=0
def initialize():
    sim_time = 0.0;
    
    server_status = IDLE;
    num_in_q = 0;
    time_last_event = 0.0;

    num_custs_delayed = 0;
    total_of_delays = 0.0;
    area_num_in_q = 0.0;
    area_server_status = 0.0;

    time_next_event[1] = sim_time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;

def timing():
    i=0;
    min_time_next_event = 1.0e+29;
    next_event_type = 0;
    for i in range(1,num_events):
        if time_next_event[i] < min_time_next_event:
            min_time_next_event = time_next_event[i];
            next_event_type = i;

    if next_event_type == 0:
        outfile.fprintf("\nEvent list empty at time %f", sim_time);
        exit(1);
    sim_time = min_time_next_event;
def arrive():
    delay=0.0;
    
    time_next_event[1] = sim_time + expon(mean_interarrival);

    if server_status == BUSY:
        ++num_in_q;
        if num_in_q > Q_LIMIT:
            outfile.fprintf("\nOverflow of the array time_arrival at")
            outfile.fprintf(" time %f", sim_time);
            exit(2)
        time_arrival[num_in_q] = sim_time;
    else:
        delay = 0.0;
        total_of_delays += delay;
        alldelays[run][num_custs_delayed] = delay;

        ++num_custs_delayed;
        server_status = BUSY;
        time_next_event[2] = sim_time + expon(mean_service);
def depart():
    i=0;
    delay=0.0;
    if num_in_q == 0:
        server_status = IDLE;
        time_next_event[2] = 1.0e+30;
    else:
        --num_in_q;
        delay = sim_time - time_arrival[1];
        total_of_delays += delay;
        alldelays[run][num_custs_delayed] = delay;
        ++num_custs_delayed;
        time_next_event[2] = sim_time + expon(mean_service);
        for i in range(num_in_q):
            time_arrival[i] = time_arrival[i + 1];

def report():
    outfile.fprintf("\n\nAverage delay in queue%11.3f minutes\n\n", total_of_delays / num_custs_delayed);
    outfile.fprintf("Average number in queue%10.3f\n\n", area_num_in_q / sim_time);
    outfile.fprintf("Server utilization%15.3f\n\n", area_server_status / sim_time);
    outfile.fprintf("Time simulation ended%12.3f minutes", sim_time);
def update_time_avg_stats():
    time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;
    area_num_in_q += num_in_q * time_since_last_event;
    area_server_status += server_status * time_since_last_event;
def printstate():
    print("Sim Time = %.2f | "+ str(sim_time),end ="");
    if server_status == IDLE:
        print("Server Idle  | ",end="");
    else:
        print("Server Busy    | ", end="");
    if num_in_q:
        print("%d in queue. buffer  (Arrival Timestamps): "+ str(num_in_q),end="");
        for i in range(1, num_in_q+1):
            print(round(time_arrival[i],5),end="")
    else:
        print("Queue Empty ")

    printf(" |  Next Arrival: %.5f  | Next Departure: ", time_next_event[1]);
    if (time_next_event[2] >= 1.0e+29)
        print("inf",end="");
    else:
        print(round(time_next_event[2],2),end="");
    print()
    print()
def expon(mean):
	pass
j=0
i=0
infile = fopen("mm1.in", "r");
outfile = fopen("mm1.out", "w");
delayfile = fopen("delayfile", "w");
demo = int(input().rstrip())
[meanInterarrival,meanServiceTime,numRequests]= list(map(float, input().rstrip().split()))
outfile = Path('mm1.out').open(mode='w')
infile = Path('mm1.in').open(mode='r')
delayfile= Path('delayfile').open(mode='w')
outfile.write("Single-server queueing system\n\n")
outfile.write("Mean interarrival time" + mean_interarrival+" minutes\n\n")
outfile.fprintf("Mean service time%16.3f minutes\n\n", mean_service)
outfile.fprintf("Number of customers%14d\n\n", num_delays_required)
num_of_repetitions = int(input().rstrip())
for run in range(num_of_repetitions):
	print("Run "+run)
	[seed, stream]= list(map(int , input().rstrip().split()))
        lcgrandst(seed, stream);
        initialize();

        while num_custs_delayed < num_delays_required:
            if (demo)
                printstate()
            timing()
            update_time_avg_stats();
            if next_event_type == 1:
    		    arrive()
            elif next_event_type == 2:
                depart() 
        # /*      fprintf(delayfile, "Run %d done \n", i+1); */
        # /* Invoke the report generator and end the simulation. */
        report()
for j in range(num_delays_required):
    for i in range(num_of_repetitions):
        delayfile.fprintf("%.3f ", alldelays[i][j])
    delayfile.fprintf("\n");
infile.close()
outfile.close()

float expon(float mean) /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */

    return -mean * log(lcgrand(stream));
}

void printstate()
{
    int i;
    printf("Sim Time = %.2f | ", sim_time);
    if (server_status == IDLE)
        printf("Server Idle  | ");
    else
        printf("Server Busy    | ");
    if (num_in_q)
    {
        printf("%d in queue. buffer  (Arrival Timestamps): ", num_in_q);
        for (i = 1; i <= num_in_q; i++)
            printf("%.5f ", time_arrival[i]);
    }
    else
        printf("Queue Empty ");

    printf(" |  Next Arrival: %.5f  | Next Departure: ", time_next_event[1]);
    if (time_next_event[2] >= 1.0e+29)
        printf("inf");
    else
        printf("%.2f", time_next_event[2]);
    printf("\n\n");
}
