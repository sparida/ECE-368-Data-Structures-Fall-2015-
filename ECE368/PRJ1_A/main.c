/*
Group Members:
1. Sthitapragyan Parida
2. Eric Lees
ECE 368 Project 1 (Spring 2015)
Compile With: gcc main.c -o project1-A -lm 
Run using:
1. project1-A 0.5 0.7 1 10000
2. project1-A input.txt
*/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"event.h"
#include"lpq.h"
#include"hpq.h"
#include"utility_funcs.h"
#include"sys_state.h"
#include<unistd.h>
SysState sys_state = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void runSimulationMode1(float, float, float, int);
void runSimulationMode2(char *);
void testQueues();
char decidePriority(int);
void readFile(char *);
long h_num = 0, l_num = 0;
int main(int argc, char *argv[])
{
	event_front = NULL;
	event_rear = NULL;
	hpq_front = NULL;
	hpq_rear = NULL;
	lpq_front = NULL;
	lpq_rear = NULL;
	

	float l1, l2, mu;
	int max_num;
	if(argc == 5)
	{	
		l1 = atof(argv[1]);
		l2 = atof(argv[2]);
		mu = atof(argv[3]);
		max_num = atoi(argv[4]);
		runSimulationMode1(l1, l2, mu, max_num);
	}
	else if(argc == 2)
	{
		runSimulationMode2(argv[1]);
	}
	else
	{
		printf("Wrong number of inputs\n");
		//testQueues();
	}
	
}
void runSimulationMode2(char *filename)
{
	readFile(filename);
	sys_state.global_time = 0;
	LPQ l = lpqDelete();
	lpqInsert(l);
	HPQ h = hpqDelete();
	hpqInsert(h);
	if(l.arrival_time < h.arrival_time)
	{
		LPQ l = lpqDelete();
		sys_state.global_time = l.arrival_time + l.duration;
	}
	else
	{
		HPQ l = hpqDelete();
		sys_state.global_time = h.arrival_time + h.duration;
	}

	while(getHPQCount() > 0 || getLPQCount() > 0)
	{
		long prev_global_time = sys_state.global_time;
		
		if(getHPQCount() > 0 && getLPQCount() > 0)
		{
			LPQ l = lpqDelete();
			lpqInsert(l);
			HPQ h = hpqDelete();
			hpqInsert(h);
			if(h.arrival_time <= sys_state.global_time && l.arrival_time <= sys_state.global_time)
			{

				HPQ h = hpqDelete();
				sys_state.event_arrive0_total += sys_state.global_time - h.arrival_time;
				sys_state.global_time += h.duration;
			}
			else if(h.arrival_time > sys_state.global_time && l.arrival_time <= sys_state.global_time)
			{
	
				LPQ l = lpqDelete();
				sys_state.event_arrive1_total += sys_state.global_time - l.arrival_time;
				sys_state.global_time += l.duration;
			}
			else if(h.arrival_time <= sys_state.global_time && l.arrival_time > sys_state.global_time)
			{
				HPQ h = hpqDelete();
				sys_state.event_arrive0_total += sys_state.global_time - h.arrival_time;
				sys_state.global_time += h.duration;
			}
			else
			{
				long abc = (h.arrival_time <= l.arrival_time ? h.arrival_time : l.arrival_time);
				sys_state.server_idle_total += abc;
				sys_state.global_time = abc;
			}
		}
		else if(getHPQCount() > 0 && getLPQCount() <= 0)
		{
			HPQ h = hpqDelete();
			hpqInsert(h);
			if(h.arrival_time <= sys_state.global_time)
			{
				HPQ h = hpqDelete();
				sys_state.event_arrive0_total += sys_state.global_time - h.arrival_time;
				sys_state.global_time += h.duration;
			}
			else
			{
				long abc = h.arrival_time;
				sys_state.server_idle_total += abc;
				sys_state.global_time = abc;
			}
		}
		else if(getLPQCount() > 0 && getHPQCount() <= 0)
		{
			LPQ l = lpqDelete();
			lpqInsert(l);
			if(l.arrival_time <= sys_state.global_time)
			{
				LPQ l = lpqDelete();
				sys_state.event_arrive1_total += sys_state.global_time - l.arrival_time;
				sys_state.global_time += l.duration;
			}
			else
			{
				long abc = l.arrival_time;
				sys_state.server_idle_total += abc;
				sys_state.global_time = abc;
			}
		}
		int queue_size = getLPQueueCount(sys_state.global_time) + getHPQueueCount(sys_state.global_time);
		sys_state.sum_weighted0_queue += queue_size * (sys_state.global_time - prev_global_time);
	}
	float high_queue_time = (float)sys_state.event_arrive0_total/h_num;
	float low_queue_time = (float)sys_state.event_arrive1_total/l_num;
	float avg_cpu_util = 1.0 - ((float)sys_state.server_idle_total / sys_state.global_time);
	float avg_queue_length = (float)sys_state.sum_weighted0_queue / sys_state.global_time;
	
	//printf("%f\n", high_queue_time);
	//printf("%f\n", low_queue_time);	
	//printf("%f\n", avg_queue_length);
	//printf("%f\n", avg_cpu_util);

	FILE *fp;
	fp = fopen("proj1-a_output", "w");
	if (fp == NULL) 
	{
		printf("Couldn't open file for writing.\n");
		exit(0);
	}
   
	fprintf(fp, "%f\n", high_queue_time);
	fprintf(fp, "%f\n", low_queue_time);
	fprintf(fp, "%f\n", avg_queue_length);
	fprintf(fp, "%f\n", avg_cpu_util);
	fclose(fp);


}


void readFile(char * filename)
{
	//printf("%s\n", filename);
	FILE *fin = fopen(filename, "r");
	char line[1024];
	while (fgets(line, 1024, fin))
	{
		int arrival, priority, duration;
		arrival = 0;
		priority = 0;
		duration = 0;
		int result = sscanf(line, "%d %d %d", &arrival, &priority, &duration);
		if(result == 3)
		{
			if(priority == 0)
			{
				HPQ hpq = {arrival, duration};
				hpqInsert(hpq);
				h_num++;
			}
			else if(priority == 1)
			{
				LPQ lpq = {arrival, duration};
				lpqInsert(lpq);
				l_num++;
			}

		}
	}
	fclose(fin);
	/*
	LPQ l = lpqDelete();
	lpqInsert(l);
	printf("%d\n", l.arrival_time);

	while (getHPQCount() > 0)
	{
		HPQ h = hpqDelete();
		printf ("%d 0 %d\n", h.arrival_time, h.duration);			
	}
	printf("_____________________\n");
	while (getLPQCount() > 0)
	{
		LPQ l = lpqDelete();
		printf ("%d 1 %d\n",l.arrival_time, l.duration);			
	}
	*/

}
void runSimulationMode1(float l1, float l2, float mu, int max_num)
{
	srand(time(NULL));
	char p;
	int a = (rand() % 2);
	int b = (a == 0 ? 1 :0);

	int arrival_time;
	arrival_time = 0;
	sys_state.high_priority_total_num++;
	Event initial_event0 = {'A', 0, a};
	eventInsert(initial_event0);

	arrival_time = 0;
	sys_state.low_priority_total_num++;
	Event initial_event1 = {'A', 0, b};
	eventInsert(initial_event1);

	Event event;
	while(getEventCount() > 0)
	{
		event = eventDelete();
		long prev_global_time = sys_state.global_time;	
		sys_state.global_time = event.trigger_time;
		//printf("%d %c %d \n", event.trigger_time, event.event_type, event.priority);
		if(event.event_type == 'A')
		{
			if(sys_state.server_busy == 0)
			{
				sys_state.server_busy = 1;
				int duration = genRandInterval(mu);
				Event departure = {'D', sys_state.global_time + duration, event.priority};
				eventInsert(departure);
				if(event.priority == 0)
				{
					sys_state.event_depart0_total += sys_state.global_time + duration;
					sys_state.event_service0_total += duration; 
				}
				else if(event.priority == 1)
				{
					sys_state.event_depart1_total += sys_state.global_time + duration;
					sys_state.event_service1_total += duration;
				}

			}
			else if(sys_state.server_busy == 1)
			{
				
				if (event.priority == 0)
				{
					sys_state.high_priority_current_num++;
				}
				else if(event.priority == 1)
				{
					sys_state.low_priority_current_num++;
				}

			}	
			char p = decidePriority(max_num);
			int new_arrival_time;
			if (p == 'h')
			{
				new_arrival_time = genRandInterval(l1);
				sys_state.high_priority_total_num++;
			}
			else if(p == 'l')
			{
				new_arrival_time = genRandInterval(l2);
				sys_state.low_priority_total_num++;
			}
			if(p != 's')
			{
				int p2;
				if (p=='h')
				{
					p2 = 0;
				}
				else if(p == 'l')
				{
					p2 = 1;
				}
				Event new_event = {'A', sys_state.global_time + new_arrival_time, p2};
				eventInsert(new_event);
				if(p2 == 0)
				{
					sys_state.event_arrive0_total += sys_state.global_time + new_arrival_time;
				} 
				else if(p2 == 1)
				{
					sys_state.event_arrive1_total += sys_state.global_time + new_arrival_time;
				} 
			}
		}
		else if(event.event_type == 'D') 
		{
			if(sys_state.high_priority_current_num > 0)
			{
				sys_state.high_priority_current_num--;
				int duration = genRandInterval(mu);
				Event departure = {'D', sys_state.global_time + duration, 0};
				eventInsert(departure);
				sys_state.event_depart0_total += sys_state.global_time + duration;
				sys_state.event_service0_total += duration;
				 

			}
			else if (sys_state.high_priority_current_num == 0 && sys_state.low_priority_current_num > 0)
			{
				sys_state.low_priority_current_num--;
				int duration = genRandInterval(mu);
				Event departure = {'D', sys_state.global_time + duration, 1};
				eventInsert(departure);
				sys_state.event_depart1_total += sys_state.global_time + duration; 
				sys_state.event_service1_total += duration;
				
			}
			else
			{
				sys_state.server_busy = 0;
			}

		}
		sys_state.sum_weighted0_queue += (sys_state.global_time - prev_global_time) * sys_state.high_priority_current_num;
		sys_state.sum_weighted1_queue += (sys_state.global_time - prev_global_time) * sys_state.low_priority_current_num;
	}
	sys_state.server_busy_total = sys_state.event_service0_total + sys_state.event_service1_total; 
			
	//printf("Total High Priority Arrival Time:%f\n", (float)sys_state.event_arrive0_total);
	//printf("Total High Priority Departure Time:%f\n", (float)sys_state.event_depart0_total);
	//printf("Average High Priority System Waiting Time:%f\n", (float)(sys_state.event_depart0_total - sys_state.event_arrive0_total)/max_num);
	//printf("Average Low Priority System Waiting Time:%f\n", (float)(sys_state.event_depart1_total - sys_state.event_arrive1_total)/max_num);
	//printf("Average High Priority Queue Waiting Time:%f\n", (float)(sys_state.event_depart0_total - sys_state.event_arrive0_total - sys_state.event_service0_total)/max_num);
	//printf("Average Low Priority Queue Waiting Time:%f\n", (float)(sys_state.event_depart1_total - sys_state.event_arrive1_total - sys_state.event_service1_total)/max_num);
	//printf("Average High Priority Queue Size:%f\n", (float)(sys_state.sum_weighted0_queue)/sys_state.global_time);
	//printf("Average Low Priority Queue Size:%f\n", (float)(sys_state.sum_weighted1_queue)/sys_state.global_time);
	//printf("Average Utlilization of CPU:%f\n", (float)(sys_state.server_busy_total)/sys_state.global_time);
	float high_queue_time = (float)(sys_state.event_depart0_total - sys_state.event_arrive0_total - sys_state.event_service0_total)/max_num;
	float low_queue_time = (float)(sys_state.event_depart1_total - sys_state.event_arrive1_total - sys_state.event_service1_total)/max_num;
	float avg_queue_length = ( ((float)(sys_state.sum_weighted0_queue)/sys_state.global_time) + ((float)(sys_state.sum_weighted1_queue)/sys_state.global_time) ) / 2;
	float avg_cpu_util = (float)(sys_state.server_busy_total)/sys_state.global_time;
	FILE *fp;
	fp = fopen("proj1-a_output", "w");
	if (fp == NULL) 
	{
		printf("Couldn't open file for writing.\n");
		exit(0);
	}
   
	fprintf(fp, "%f\n", high_queue_time);
	fprintf(fp, "%f\n", low_queue_time);
	fprintf(fp, "%f\n", avg_queue_length);
	fprintf(fp, "%f\n", avg_cpu_util);
	fclose(fp);



}

char decidePriority(int max_num)
{
	if(sys_state.high_priority_total_num < max_num && sys_state.low_priority_total_num < max_num)
	{
		return ((rand() % 2) ? 'h' :'l');
		
	}
	else if(sys_state.high_priority_total_num < max_num && sys_state.low_priority_total_num >= max_num)
	{
		return 'h';
	}
	else if(sys_state.high_priority_total_num >= max_num && sys_state.low_priority_total_num < max_num)
	{
		return 'l';
	}
	else
	{
		return 's';
	}

}

void testQueues()
{
		printf("Testing queues...\n");
		Event e1 = {'a', 20};
		Event e2 = {'d', 10};
		Event e3 = {'a', 5};
		Event e4 = {'d', 40};
		Event e5 = {'d', 35};
		HPQ l1 = {12, 10};
		HPQ l2 = {10, 5};
		HPQ l3 = {5, 6};
		HPQ l4 = {17, 8};
		HPQ l5 = {20, 20};
	
		eventInsert(e1);
		eventInsert(e2);
		eventInsert(e3);
		eventInsert(e4);
		eventInsert(e5);
		hpqInsert(l1);
		hpqInsert(l2);
		hpqInsert(l3);
		hpqInsert(l4);
		hpqInsert(l5);
		
		while (getEventCount() > 0)
		{
			Event t = eventDelete();
			printf ("%c %d\n", t.event_type, t.trigger_time);			
		}
		while (getHPQCount() > 0)
		{
			HPQ h = hpqDelete();
			printf ("%d %d\n", h.arrival_time, h.duration);			
		}

	
}
