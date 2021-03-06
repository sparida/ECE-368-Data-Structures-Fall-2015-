/*
Group Members:
1. Sthitapragyan Parida
2. Eric Lees
ECE 368 Project 1B (Spring 2015)
*/
#include<stdio.h>
#include <stdlib.h>
#include<math.h>
#ifndef HPQ_H
#define HPQ_H
typedef struct HPQ
{
	long arrival_time;
	long duration; 
	long ns;
	long srv_times[32];
	long srv;
		
} HPQ;

typedef struct HPQNode
{
	HPQ hpq;
	struct HPQNode *link;
} HPQNode;

HPQNode *hpq_front,*hpq_rear;   /* Global Declarations */

void hpqInsert(HPQ new_hpq)
{
	HPQNode *temp, *iter, *iter_front; 
	temp = (HPQNode *)malloc(sizeof(HPQNode));

	//iter = (HPQNode *)malloc(sizeof(HPQNode));
	//iter_front = (HPQNode *)malloc(sizeof(HPQNode));

	if( temp == NULL)
	{
		printf(" Out of Memory !! Overflow !!!");
	}
	else
	{
		temp->hpq.arrival_time = new_hpq.arrival_time;
		temp->hpq.duration = new_hpq.duration;
		temp->hpq.ns = new_hpq.ns;
		temp->hpq.srv = new_hpq.srv;
		long i = 0;
		for(i = 0; i < 32 ; i++)
		{
			if(i < temp->hpq.ns)	temp->hpq.srv_times[i] = new_hpq.srv_times[i];
			if(i >= temp->hpq.ns)	temp->hpq.srv_times[i] = -1;
		}

		//temp->hpq.duration = 0;

		temp->link = NULL;
		if(hpq_front == NULL)
		{
			hpq_front = hpq_rear = temp;
		}
		
		else if(hpq_front == hpq_rear)
		{
			if(temp->hpq.arrival_time > hpq_front->hpq.arrival_time)
			{
				hpq_rear->link=temp;
				hpq_rear = temp;
			}
			else
			{
				temp->link = hpq_front;
				hpq_front = temp;
			}
		}
		
		else
		{
			
			iter = hpq_front;
			iter_front = iter;
			if(temp->hpq.arrival_time <= hpq_front->hpq.arrival_time)
			{
				temp->link = hpq_front;
				hpq_front = temp;
			}
			else if(temp->hpq.arrival_time >= hpq_rear->hpq.arrival_time)
			{
				hpq_rear->link = temp;
				hpq_rear = temp;
			}
			else
			{
			while(iter != hpq_rear)
			{
	
					if(iter->hpq.arrival_time >= temp->hpq.arrival_time)
					{
						iter_front->link = temp;
						temp->link = iter;
						break;
					} 
					iter_front = iter;
					iter = iter->link;
				}
			}
			if (iter == hpq_rear)
			{
				iter_front->link =temp;
				temp->link = hpq_rear;
			}		
				
			//hpq_rear->link=temp;
			//hpq_rear = temp;
			
		}
	}
}
 
HPQ hpqDelete()
{
	//printf("Hello\n");
	HPQ hpq;
	HPQNode *temp;
	if(hpq_front ==  NULL) 
	{
		printf("DUMBnderflow!!!\n");
		return;
	}
	else
	{
		temp=hpq_front;
		hpq.arrival_time = hpq_front->hpq.arrival_time;
		hpq.duration = hpq_front->hpq.duration;
		hpq.ns = hpq_front->hpq.ns;
		hpq.srv = hpq_front->hpq.srv;
		long i = 0;
		for(i = 0; i < 32 ; i++)
		{
			if(i < hpq.ns)	hpq.srv_times[i] = hpq_front->hpq.srv_times[i];
			if(i >= hpq.ns)	hpq.srv_times[i] = -1;
		}

		if(hpq_front == hpq_rear) hpq_rear=NULL;
		hpq_front=hpq_front->link;
		temp->link=NULL;
		free(temp);
		return(hpq);
	}
}
 
long getHPQCount()
{
	HPQNode *temp;
	if( hpq_front == NULL) return 0;
	else
	{
		temp=hpq_front;
		long count = 0;
		while(temp)
		{
			if (temp->hpq.srv != 1) count = count + 1;
			temp=temp->link;
		}
		return count;
	}
}

HPQ deleteHPQIndexElement(long i)
{
	HPQ *hpq_array = malloc(sizeof(HPQ) * (i+1));
	HPQNode *temp;
	temp = hpq_front;

	if (i >= getHPQCount() || i < 0)
	{
		printf("Invalid Access\n");
		exit(0);
	}
	else if(getHPQCount() == 1)
	{
		free(temp);
		hpq_front = NULL;
		hpq_rear = NULL;
	}
	else
	{
		long j = 0;
		for(j = 0; j <= i ; j++)
		{
			hpq_array[j] = hpqDelete();
		}
		for(j = 0; j <= i-1 ; j++)
		{
			hpqInsert(hpq_array[j]);
		}
		HPQ hpq;
		hpq.arrival_time = hpq_array[i].arrival_time;
		hpq.duration = hpq_array[i].duration;
		hpq.ns = hpq_array[i].ns;
		hpq.srv = hpq_array[i].srv;
		long k = 0;
		for(k = 0; k < 32 ; k++)
		{
			if(k < hpq.ns)
			{
				hpq.srv_times[k] = hpq_array[i].srv_times[k];
				//prlong
			}
			if(k >= hpq.ns)	hpq.srv_times[k] = -1;
		}
		free(hpq_array);
		return hpq;
	}
	
}

HPQ getHPQIndexElement(long i)
{
	HPQ hpq;
	HPQNode *temp;
	temp = hpq_front;

	if (i >= getHPQCount() || i < 0)
	{
		printf("Invalid Access\n");
		exit(0);
	}
	else
	{
		while (i-- > 0)
		{
		temp = temp->link;
		}
		hpq.arrival_time = temp->hpq.arrival_time;
		hpq.duration = temp->hpq.duration;
		hpq.ns = temp->hpq.ns;
		hpq.srv = temp->hpq.srv;
		long i = 0;
		for(i = 0; i < 32 ; i++)
		{
			if(i < hpq.ns)	hpq.srv_times[i] = temp->hpq.srv_times[i];
			if(i >= hpq.ns)	hpq.srv_times[i] = -1;
		}
		return hpq;
	}

	
}
long getHPQTimeCount(long global_time)
{
	HPQNode *temp;
	if( hpq_front == NULL) return 0;
	else
	{
		temp=hpq_front;
		long count = 0;
		while(temp && (temp->hpq.arrival_time <= global_time))
		{
			if (temp->hpq.srv != 1) count = count + 1;
			temp=temp->link;
		}
	return count;
	}
}

#endif
