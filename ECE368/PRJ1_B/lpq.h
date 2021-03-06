#include<stdio.h>
/*
Group Members:
1. Sthitapragyan Parida
2. Eric Lees
ECE 368 Project 1B (Spring 2015)
*/
#include <stdlib.h>
#include<math.h>
#ifndef LPQ_H
#define LPQ_H
typedef struct LPQ
{
	long arrival_time;
	long duration; 
	long ns;
	long srv_times[32]; 
	long srv;
		
} LPQ;

typedef struct LPQNode
{
	LPQ lpq;
	struct LPQNode *link;
} LPQNode;

LPQNode *lpq_front,*lpq_rear;   /* Global Declarations */

void lpqInsert(LPQ new_lpq)
{
	LPQNode *temp, *iter, *iter_front; 
	temp = (LPQNode *)malloc(sizeof(LPQNode));

	//iter = (LPQNode *)malloc(sizeof(LPQNode));
	//iter_front = (LPQNode *)malloc(sizeof(LPQNode));

	if( temp == NULL)
	{
		printf(" Out of Memory !! Overflow !!!");
	}
	else
	{
		temp->lpq.arrival_time = new_lpq.arrival_time;
		temp->lpq.duration = new_lpq.duration;
		temp->lpq.ns = new_lpq.ns;
		temp->lpq.srv = new_lpq.srv;
		long i = 0;
		for(i = 0; i < 32 ; i++)
		{
			if(i < temp->lpq.ns)	temp->lpq.srv_times[i] = new_lpq.srv_times[i];
			if(i >= temp->lpq.ns)	temp->lpq.srv_times[i] = -1;
		}
		temp->link = NULL;
		if(lpq_front == NULL)
		{
			lpq_front = lpq_rear = temp;
		}
		
		else if(lpq_front == lpq_rear)
		{
			if(temp->lpq.arrival_time > lpq_front->lpq.arrival_time)
			{
				lpq_rear->link=temp;
				lpq_rear = temp;
			}
			else
			{
				temp->link = lpq_front;
				lpq_front = temp;
			}
		}
		
		else
		{
			
			iter = lpq_front;
			iter_front = iter;
			if(temp->lpq.arrival_time <= lpq_front->lpq.arrival_time)
			{
				temp->link = lpq_front;
				lpq_front = temp;
			}
			else if(temp->lpq.arrival_time >= lpq_rear->lpq.arrival_time)
			{
				lpq_rear->link = temp;
				lpq_rear = temp;
			}
			else
			{
			while(iter != lpq_rear)
			{
	
					if(iter->lpq.arrival_time >= temp->lpq.arrival_time)
					{
						iter_front->link = temp;
						temp->link = iter;
						break;
					} 
					iter_front = iter;
					iter = iter->link;
				}
			}
			if (iter == lpq_rear)
			{
				iter_front->link =temp;
				temp->link = lpq_rear;
			}		
				
			//lpq_rear->link=temp;
			//lpq_rear = temp;
			
		}
	}
}
 
LPQ lpqDelete()
{
	LPQ lpq;
	LPQNode *temp;
	if(lpq_front ==  NULL) 
	{
		printf(" Underflow!!!\n");
		return;
	}
	else
	{
		temp=lpq_front;
		lpq.arrival_time = lpq_front->lpq.arrival_time;
		lpq.duration = lpq_front->lpq.duration;
		lpq.ns = lpq_front->lpq.ns;
		lpq.srv = lpq_front->lpq.srv;
		long i = 0;
		for(i = 0; i < 32 ; i++)
		{
			if(i < lpq.ns)	lpq.srv_times[i] = lpq_front->lpq.srv_times[i];
			if(i >= lpq.ns)	lpq.srv_times[i] = -1;
		}

		if(lpq_front == lpq_rear) lpq_rear=NULL;
		lpq_front=lpq_front->link;
		temp->link=NULL;
		free(temp);
		return(lpq);
	}
}
 
long getLPQCount()
{
	LPQNode *temp;
	if( lpq_front == NULL) return 0;
	else
	{
		temp=lpq_front;
		long count = 0;
		while(temp)
		{
			if (temp->lpq.srv != 1) count = count + 1;
			temp=temp->link;
		}
	return count;
	}
}

LPQ getLPQIndexElement(long i)
{
	LPQ lpq;
	LPQNode *temp;
	temp = lpq_front;

	if (i >= getLPQCount() || i < 0)
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
		lpq.arrival_time = temp->lpq.arrival_time;
		lpq.duration = temp->lpq.duration;
		lpq.ns = temp->lpq.ns;
		lpq.srv = temp->lpq.srv;
		long i = 0;
		for(i = 0; i < 32 ; i++)
		{
			if(i < lpq.ns)	lpq.srv_times[i] = temp->lpq.srv_times[i];
			if(i >= lpq.ns)	lpq.srv_times[i] = -1;
		}
		return lpq;
	}

	
}
LPQ deleteLPQIndexElement(long i)
{
	LPQ *lpq_array = (LPQ *)malloc(sizeof(LPQ) * (i+1));
	LPQNode *temp;
	temp = lpq_front;

	if (i >= getLPQCount() || i < 0)
	{
		printf("Invalid Access\n");
		exit(0);
	}
	else if(getLPQCount() == 1)
	{
		free(temp);
		lpq_front = NULL;
		lpq_rear = NULL;
	}
	else
	{
		long j = 0;
		for(j = 0; j <= i ; j++)
		{
			lpq_array[j] = lpqDelete();
		}
		for(j = 0; j <= i-1 ; j++)
		{
			lpqInsert(lpq_array[j]);
		}
		LPQ lpq;
		lpq.arrival_time = lpq_array[i].arrival_time;
		lpq.duration = lpq_array[i].duration;
		lpq.ns = lpq_array[i].ns;
		lpq.srv = lpq_array[i].srv;
		long k = 0;
		for(k = 0; k < 32 ; k++)
		{
			if(k < lpq.ns)	lpq.srv_times[k] = lpq_array[i].srv_times[k];
			if(k >= lpq.ns)	lpq.srv_times[k] = -1;
		}
		free(lpq_array);
		return lpq;
	}
	
}

long getLPQTimeCount(long global_time)
{
	LPQNode *temp;
	if( lpq_front == NULL) return 0;
	else
	{
		temp=lpq_front;
		long count = 0;
		while(temp && (temp->lpq.arrival_time <= global_time))
		{
			if (temp->lpq.srv != 1) count = count + 1;
			temp=temp->link;
		}
	return count;
	}
}


#endif
