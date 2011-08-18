//
//  TaskQueue.h
//  Kepler
//
//  Created by Tom Carden on 8/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <queue>
#include <set>
#include "cinder/app/AppBasic.h"
#include "cinder/Cinder.h"
#include "cinder/Thread.h"
#include "cinder/Function.h"

typedef std::function<void(void)> VoidFunc;

class TaskQueue {
  
public:
    
    static void pushTask( VoidFunc f )
    {
        // TODO: later, use a single thread and queue the tasks 
        // e.g. http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html 
        // for now, a thread per task will help spec out the right interface:
        std::thread taskThread( &TaskQueue::doWork, f );
    }
    
private:
    
    static void doWork( VoidFunc f )
    {
        // Cinder's ThreadSetup class ensures that the OS know's what's up
        // (creates and drains an autorelease pool, etc)
        ci::ThreadSetup threadSetup;
        
        // otherwise we'd need:
        // NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
        
        // do the work:
        f();
        
        // cleanup (now handled by ThreadSetup):
        // http://stackoverflow.com/questions/797419/whats-the-difference-between-sending-release-or-drain-to-an-autorelease-pool
        //    [autoreleasepool release];	
    }
    
};

class UiTaskQueue {
    
public:
    
    static int pushTask( VoidFunc f )
    {
        mFunctionsMutex.lock();
        mFunctions.push( f );
        mFunctionsMutex.unlock();
        return mTotalTasks++;
    }
    
    static bool isTaskComplete( uint64_t taskId )
    {
        return mCompletedTasks > taskId;
    }
    
    // only call from main thread, no canceling from background threads mmmkay?
    static void cancelTask( uint64_t taskId )
    {
        if (!isTaskComplete(taskId)) {
            mCanceledIds.insert(taskId);
        }
    }
    
    // call this from the UI thread, or pain will occur
    static void popTask( )
    {
        float t = ci::app::getElapsedSeconds();
        do {
            // TODO: within a certain time limit, keep popping tasks
            if (mFunctionsMutex.try_lock()) {
                if (mFunctions.empty()) {                
                    mFunctionsMutex.unlock();
                    break; // quit the do/while
                }
                else {
                    uint64_t thisTaskId = mCompletedTasks++; // remember this id and increment too; 
                    // (it's about to be gone from mFunctions so cancelTask won't work anyway)
                    VoidFunc f = mFunctions.front(); // grab the next task
                    mFunctions.pop();                // remove it; there is no undo
                    mFunctionsMutex.unlock();        // unlock first so that tasks can queue other tasks
                    std::set<uint64_t>::iterator iter = mCanceledIds.find( thisTaskId );
                    if ( iter == mCanceledIds.end() ) {
                        // if it's not canceled, do it
                        std::cout << "performing task " << thisTaskId << std::endl;
                        f();
                    }
                    else {
                        std::cout << "skipping task " << thisTaskId << std::endl;
                        // otherwise remove it
                        mCanceledIds.erase( iter );
                    }
                }
            }
        } while ( ci::app::getElapsedSeconds() - t < 0.005 ); // ~5ms budget (but always do at least one call)
    }
    
private:
    
    static std::mutex mFunctionsMutex;
    static std::queue<VoidFunc> mFunctions;
    static uint64_t mCompletedTasks;
    static uint64_t mTotalTasks;
    static std::set<uint64_t> mCanceledIds;
    
};