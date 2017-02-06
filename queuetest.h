#ifndef _QUEUETEST_H
#define _QUEUETEST_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>


#define MAXQUEUESIZE  10

class MutexLock
{
public:
	MutexLock();
	~MutexLock();

	void lock();
	void unlock();
	pthread_mutex_t* get_mutexlock();
	
private:
	void mutexinit();
	void mutexDestory();


private:
	pthread_mutex_t m_Mutex;
};

class Condition
{
public:
	Condition(MutexLock& lock);
	~Condition();
	void wait();
	void notify();
	void notifyAll();
private:
	pthread_cond_t _cond;
	MutexLock &m_Mutex;
	
};

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



#endif
