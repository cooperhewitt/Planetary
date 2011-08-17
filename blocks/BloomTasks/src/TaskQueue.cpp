//
//  TaskQueue.cpp
//  Kepler
//
//  Created by Tom Carden on 8/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "TaskQueue.h"

std::mutex UiTaskQueue::mFunctionsMutex;
std::vector< std::function<void(void)> > UiTaskQueue::mFunctions;

