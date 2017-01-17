/***********************************************************
//
************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>

using namespace std;

#define MAXQUEUESIZE  10

pthread_mutex_t 	g_mutex_queue_cs;		 
pthread_cond_t		queue_cv;
pthread_mutexattr_t g_MutexAttr;


typedef struct 
{
	int Id;
	char desInfo[100];
}QueueNode;

class QueueInfo
{
public:
	QueueInfo()
	{
		QueueNode Queuelist[MAXQUEUESIZE] = {0};
		queueHead = 0;
		queueTail = 0;
	}
	
	void PushDataIntoQueue();
	void GetDataFromQueue();

private:
	
	int EnQueue(QueueNode node);
	int DeQueue(QueueNode *node);

private:
	QueueNode Queuelist[MAXQUEUESIZE];
	int queueHead;
	int queueTail;

};

void Sleep(int iCountMs)
{
    struct timeval t_timeout = {0};


    if (iCountMs < 1000)
    {
        t_timeout.tv_sec  = 0;
        t_timeout.tv_usec = iCountMs * 1000;
    }
    else
    {
        t_timeout.tv_sec  = iCountMs / 1000;
        t_timeout.tv_usec = (iCountMs % 1000) * 1000;
    }
    select(0, NULL, NULL, NULL, &t_timeout);    // \u8c03\u7528select\u51fd\u6570\u963b\u585e\u7a0b\u5e8f
}
int QueueInfo::EnQueue(QueueNode  node)
{
	int iretval = 0;
	int iNextPos = 0;
	pthread_mutex_lock(&g_mutex_queue_cs);
	
	iNextPos = queueTail + 1;

	if(iNextPos >= MAXQUEUESIZE)
	{
		iNextPos = 0;
	}
	if(iNextPos == queueHead)
	{
		iretval = -1;
	}
	else
	{
		//insert queue
		memset(&Queuelist[queueTail],0X00,sizeof(QueueNode));
		memcpy(&Queuelist[queueTail],&node,sizeof(QueueNode));
		queueTail = iNextPos;
	}

    pthread_cond_signal(&queue_cv);
	pthread_mutex_unlock(&g_mutex_queue_cs);

	return iretval;
}

void QueueInfo::PushDataIntoQueue()
{
	static int iCountNum = 0;
	QueueNode Qdata;

	Qdata.Id = iCountNum;
	snprintf(Qdata.desInfo,sizeof(Qdata.desInfo)-1,"yuecheng%d",iCountNum);

    iCountNum++;
	if(iCountNum > MAXQUEUESIZE - 1)
	{
		iCountNum = 0;
	}

	while(EnQueue(Qdata) == -1)
	{
		Sleep(1000);
	}

	cout<<"putdata into Queue : Id = "<<Qdata.Id<<"Info: "<<Qdata.desInfo<<endl;
	
}

int QueueInfo::DeQueue(QueueNode *node)
{

	QueueNode tQueueData = {0};

	if(node == NULL)
	{
		cout<<"node is null"<<endl;
		return -1;
	}

	pthread_mutex_lock(&g_mutex_queue_cs);
	
	if(queueHead == queueTail)
	{
		cout<<"wait queue insert data"<<endl;
		pthread_cond_wait(&queue_cv,&g_mutex_queue_cs);
	}

	
	memset(&tQueueData,0X00,sizeof(QueueNode));
	memcpy(&tQueueData,&Queuelist[queueHead],sizeof(QueueNode));
	queueHead++;

	
	if(queueHead>=MAXQUEUESIZE)
	{
		queueHead = 0;
	}

	pthread_mutex_unlock(&g_mutex_queue_cs);

	memcpy(node,&tQueueData,sizeof(QueueNode));

	return 0;
}


void QueueInfo::GetDataFromQueue()
{
	QueueNode Qdata = {0};

	if(DeQueue(&Qdata) == -1)
	{
		cout<<"get queue data error"<<endl;

		return ;
	}

	cout<<"getdata from Queue : Id = "<<Qdata.Id<<"Info: "<<Qdata.desInfo<<endl;
	
}


int main(int argc,char* argv[])
{
	cout<<"*********now operate program************"<<endl;

	pthread_mutex_init(&g_mutex_queue_cs, &g_MutexAttr);
    pthread_cond_init(&queue_cv, NULL);

	QueueInfo qInfo;
	
	while(1)
	{
		
		qInfo.PushDataIntoQueue();
		Sleep(2*1000);
		
		qInfo.GetDataFromQueue();

		Sleep(2*1000);
	}
	
	return 0;
}
