//
//  TaskQueue.cpp
//  Kepler
//
//  Created by Tom Carden on 8/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "TaskQueue.h"

std::mutex UiTaskQueue::mFunctionsMutex;
std::queue<VoidFunc> UiTaskQueue::mFunctions;
uint64_t UiTaskQueue::mCompletedTasks = 0;
uint64_t UiTaskQueue::mTotalTasks = 0;
std::set<uint64_t> UiTaskQueue::mCanceledIds;
