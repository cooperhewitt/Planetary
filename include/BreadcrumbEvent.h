/*
 *  BreadcrumbEvent.h
 *  CinderFizz
 *
 *  Created by Tom Carden on 2/15/11.
 *  Copyright 2011 Bloom Studio Inc. All rights reserved.
 *
 */

#pragma once

#include <string>
#include "cinder/app/Event.h"

class BreadcrumbEvent : public ci::app::Event {
private:
	std::string mLabel;
	int mLevel;
public:	
	BreadcrumbEvent( int level, std::string label ): ci::app::Event(), mLevel(level), mLabel(label) {
	}
	~BreadcrumbEvent() { }
	int getLevel() {
		return mLevel;
	}
	std::string getLabel() {
		return mLabel;
	}
};
