//
//  TaskQueue.h
//  Kepler
//
//  Created by Tom Carden on 8/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/Cinder.h"
#include "cinder/Thread.h"
#include "cinder/Function.h"

class TaskQueue {
  
public:
    
    static void pushTask( std::function<void(void)> f )
    {
        // TODO: later, use a single thread and queue the tasks 
        // e.g. http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html 
        // for now, a thread per task will help spec out the right interface:
        std::thread taskThread( &TaskQueue::doWork, f );
    }
    
private:
    
    static void doWork( std::function<void(void)> f )
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
    
    static void pushTask( std::function<void(void)> f )
    {
        mFunctionsMutex.lock();
        mFunctions.push_back( f );
        mFunctionsMutex.unlock();
    }
    
    // call this from the UI thread, or pain will occur
    static void popTask( )
    {
        // TODO: within a certain time limit, keep popping tasks
        if (mFunctionsMutex.try_lock()) {
            if (mFunctions.size() > 0) {
                std::function<void(void)> f = mFunctions[0];
                mFunctions.erase( mFunctions.begin() );
                mFunctionsMutex.unlock(); // unlock first so that tasks can queue other tasks
                f();
            }
            else {
                mFunctionsMutex.unlock();
            }
        }
    }
    
private:
    
    static std::mutex mFunctionsMutex;
    static std::vector< std::function<void(void)> > mFunctions;
    
};