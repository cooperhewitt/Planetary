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
        // TODO: later, optionally use a single thread and queue the tasks 
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
    
    // only call from main thread, no canceling from background threads yet :(
    static void cancelTask( uint64_t taskId )
    {
        if (!isTaskComplete(taskId)) {
            mCanceledIds.insert(taskId);
        }
    }
    
    // call this from the UI thread, or pain will occur
    // default ~5ms budget (but will always do at least one call)        
    static void update( const float &budgetSeconds = 0.005f )
    {
        // empty while loop *shiver*
        const float t = ci::app::getElapsedSeconds();
        while ( popTask() && ci::app::getElapsedSeconds() - t < budgetSeconds );
    }
        
private:

    // returns true if it got a lock and has more tasks, false otherwise
    static bool popTask( )
    {
        if (mFunctionsMutex.try_lock()) {
            if (mFunctions.empty()) {                
                mFunctionsMutex.unlock();
                return false;
            }
            else {
                // remember this id and increment too; 
                // (it's about to be gone from mFunctions so cancelTask won't work anyway)
                uint64_t thisTaskId = mCompletedTasks++; 
                
                 // grab the next task and remove it; there is no undo
                VoidFunc f = mFunctions.front();                
                mFunctions.pop();
                
                // let callers know if it's worth calling again
                bool hasMoreTasks = !mFunctions.empty();
                
                // unlock first so that tasks can queue other tasks
                mFunctionsMutex.unlock();
                
                // if it's not canceled, do it (otherwise just clean up the canceled marker)
                std::set<uint64_t>::iterator iter = mCanceledIds.find( thisTaskId );
                if ( iter == mCanceledIds.end() ) {
                    f();
                }
                else {
                    mCanceledIds.erase( iter );
                }
                
                return hasMoreTasks;
            }
        }
        return false;
    }
    
    static std::mutex mFunctionsMutex;
    static std::queue<VoidFunc> mFunctions;
    static uint64_t mCompletedTasks;
    static uint64_t mTotalTasks;
    static std::set<uint64_t> mCanceledIds; // FIXME: no mutex for canceling so not thread safe :(
    
};