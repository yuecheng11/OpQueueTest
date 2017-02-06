/***********************************************************
//
************************************************************/
#include "queuetest.h"

using namespace std;

MutexLock:: MutexLock()
{
	mutexinit();
}
MutexLock::~MutexLock()
{	
	mutexDestory();
}

void MutexLock::mutexinit()
{
	pthread_mutex_init(&m_Mutex,NULL);
}

void MutexLock::mutexDestory()
{
	pthread_mutex_destroy(&m_Mutex);
}

void MutexLock::lock()
{
	pthread_mutex_lock(&m_Mutex);
}

void MutexLock::unlock()
{
	pthread_mutex_unlock(&m_Mutex);
}

pthread_mutex_t* MutexLock::get_mutexlock()
{

	return &m_Mutex;

}

Condition::Condition(MutexLock& lock):m_Mutex(lock)
{
	pthread_cond_init(&_cond,NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&_cond);
}

void Condition::wait()
{
	pthread_cond_wait(&_cond,m_Mutex.get_mutexlock());
}

void Condition::notify()
{
	pthread_cond_signal(&_cond);
}

void Condition::notifyAll()
{
	pthread_cond_broadcast(&_cond);
}


MutexLock Mlock;
Condition cond(Mlock);


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

	Mlock.lock();

	//pthread_mutex_lock(&g_mutex_queue_cs);
	
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

	cond.notify();
	Mlock.unlock();
	
	//pthread_mutex_unlock(&g_mutex_queue_cs);

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

	Mlock.lock();
	//pthread_mutex_lock(&g_mutex_queue_cs);
	
	if(queueHead == queueTail)
	{
		cout<<"wait queue insert data"<<endl;
		cond.wait();
		//pthread_cond_wait(&queue_cv,&g_mutex_queue_cs);
	}

	
	memset(&tQueueData,0X00,sizeof(QueueNode));
	memcpy(&tQueueData,&Queuelist[queueHead],sizeof(QueueNode));
	queueHead++;

	
	if(queueHead>=MAXQUEUESIZE)
	{
		queueHead = 0;
	}

	Mlock.unlock();
	//pthread_mutex_unlock(&g_mutex_queue_cs);

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
