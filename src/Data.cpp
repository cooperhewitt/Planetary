/*
 *  Data.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Thread.h"
#include "Data.h"
#include "Globals.h"
#include <boost/algorithm/string.hpp>   

using namespace ci;
using namespace ci::ipod;
using namespace std;

Data::Data()
{
}

void Data::initArtists()
{
//	mArtists	= getArtists();
	isIniting = true;
	std::thread loaderThread( &Data::backgroundInitArtists, this );	
}

void Data::backgroundInitArtists()
{
	NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
	pending = getArtists();
	std::cout << "got " << pending.size() << " artists" << std::endl;
    [autoreleasepool release];	
	isIniting = false;
}

bool Data::update()
{
	if (!isIniting && pending.size() > 0) {
		// TODO: time this, is it OK in one frame?
		mArtists.insert(mArtists.end(),pending.begin(),pending.end());
		pending.clear();
		return true;
	}
	return false;
}

void Data::filterArtistsByAlpha( char c )
{
	mFilteredArtists.clear();
	if( c != '#' ){
		char cLower = static_cast<char> ( tolower( c ) );
		char cUpper = static_cast<char> ( toupper( c ) );
		
		int index = 0;
		for( vector<ci::ipod::PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
			string name		= (*it)->getArtistName();
			string the		= name.substr( 0, 4 );
			char firstLetter;
			
			if( the == "The " || the == "the " ){
				firstLetter = name[4];
			} else {
				firstLetter = name[0];
			}
		
			if( firstLetter == cUpper || firstLetter == cLower ){
				mFilteredArtists.push_back( index );
			}
			
			index ++;
		}
	} else {
		int index = 0;
		for( vector<ci::ipod::PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
			string name		= (*it)->getArtistName();
			char firstLetter = name[0];
		
			if( isdigit( firstLetter ) ){
				std::cout << name << std::endl;
				mFilteredArtists.push_back( index );
			}
			
			index ++;
		}
	}
}