//
//  TaskQueue.h
//  Kepler
//
//  Created by Tom Carden on 8/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <queue>
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
    
    static void pushTask( VoidFunc f )
    {
        mFunctionsMutex.lock();
        mFunctions.push( f );
        mFunctionsMutex.unlock();
    }
    
    // call this from the UI thread, or pain will occur
    static void popTask( )
    {
        int tasksComplete = 0;
        float t = ci::app::getElapsedSeconds();
        do {
            // TODO: within a certain time limit, keep popping tasks
            if (mFunctionsMutex.try_lock()) {
                if (mFunctions.empty()) {                
                    mFunctionsMutex.unlock();
                    break; // quit the do/while
                }
                else {
                    VoidFunc f = mFunctions.front(); // grab the next task
                    mFunctions.pop();                // remove it; there is no undo
                    mFunctionsMutex.unlock();        // unlock first so that tasks can queue other tasks
                    f(); // go!
                }
                tasksComplete++;
            }
        } while ( ci::app::getElapsedSeconds() - t < 0.005 ); // ~5ms budget (but always do at least one call)
//        std::cout << "completed " << tasksComplete << " task" << (tasksComplete==1?"":"s") << " this frame" << std::endl;
    }
    
private:
    
    static std::mutex mFunctionsMutex;
    static std::queue<VoidFunc> mFunctions;
    
};