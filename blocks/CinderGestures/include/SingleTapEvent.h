/*
 *  SingleTapEvent.h
 *  CinderGestures
 *
 *  Created by Tom Carden on 2/9/11.
 *  Copyright 2011 Bloom Studio Inc. All rights reserved.
 */

#pragma once

#include <vector>
#include "cinder/app/TouchEvent.h"

namespace cinder {

class SingleTapEvent : public Event {

private:

	Vec2f mPos;
	double mTime;

public:

	SingleTapEvent(const Vec2f &pos, const double &t): mPos(pos), mTime(t) { }

	const double& getTime() {
		return mTime;
	}

	const Vec2f& getPos() {
		return mPos;
	}

};

} // namespace cinder
