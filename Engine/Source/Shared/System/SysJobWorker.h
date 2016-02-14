/**************************************************************************
*
* File:		SysJobWorker.h
* Author:	Neil Richardson 
* Ver/Date:	6/07/11
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __SysJobWorker_H__
#define __SysJobWorker_H__

#include "Base/BcTypes.h"
#include "System/SysJob.h"
#include "System/SysFence.h"
#include "System/SysJobQueue.h"

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

//////////////////////////////////////////////////////////////////////////
// Forward Declarations
class SysJobQueue;

//////////////////////////////////////////////////////////////////////////
// SysJobWorker
class SysJobWorker
{
public:
	SysJobWorker( class SysKernel* Parent, SysFence& StartFence, const char* DebugName );
	virtual ~SysJobWorker();
	
	/**
	 * Start worker.
	 */
	void					start();
	
	/**
	 * Stop worker.
	 */
	void					stop();

	/**
	 * Add job queue.
	 */
	void					addJobQueue( SysJobQueue* JobQueue );

	/**
	 * Notify of schedule.
	 */
	void					notifySchedule();

	/**
	 * Log debug information.
	 */
	void					debugLog();

private:
	virtual void			execute();
	
private:
	class SysKernel* Parent_;
	std::string DebugName_;
	SysFence& StartFence_;
	BcBool Active_;
	std::atomic< size_t > PendingJobQueue_;
	std::atomic< size_t > PendingJobSchedule_;
	std::condition_variable WorkScheduled_;
	std::mutex WorkScheduledMutex_;
	std::thread ExecutionThread_;
	std::mutex JobQueuesLock_;
	SysJobQueueList NextJobQueues_;
	SysJobQueueList CurrJobQueues_;
	size_t JobQueueIndex_;
};

#endif
