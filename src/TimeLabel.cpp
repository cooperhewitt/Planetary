//
//  TimeLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TimeLabel.h"

void TimeLabel::setTime(float time)
{ 
    if (mTime != time) { 
        mTime = time; 
        updateTexture(); 
    }
}

void TimeLabel::updateTexture()
{
//    
//    // CURRENT TIME
//    mMinutes	= floor( abs(currentTime)/60.0f );
//    mPrevSeconds = mSeconds;
//    mSeconds	= (int)abs(currentTime)%60;
//    
//    mMinutesTotal	= floor( totalTime/60.0f );
//    mSecondsTotal	= (int)totalTime%60;
//    
//    double timeLeft = min(totalTime, totalTime - currentTime);
//    mMinutesLeft	= floor( timeLeft/60.0f );
//    mSecondsLeft	= (int)timeLeft%60;
//    
//    if( mSeconds != mPrevSeconds ){
//        string minsStr = ci::toString( abs(mMinutes) );
//        string secsStr = ci::toString( abs(mSeconds) );
//        if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
//        if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
//        
//        stringstream ss;
//        ss << minsStr << ":" << secsStr << endl;
//        
//        TextLayout layout;
//        layout.setFont( font );
//        layout.setColor( COLOR_BRIGHT_BLUE );
//        layout.addLine( ss.str() );
//        mCurrentTimeTex = layout.render( true, false );
//        
//        
//        
//        minsStr = ci::toString( mMinutesLeft );
//        secsStr = ci::toString( mSecondsLeft );
//        if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
//        if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
//        
//        ss.str("");
//        ss << "-" << minsStr << ":" << secsStr;
//        TextLayout layout2;
//        layout2.setFont( font );
//        layout2.setColor( COLOR_BRIGHT_BLUE );
//        layout2.addLine( ss.str() );
//        mRemainingTimeTex = layout2.render( true, false );
//    }
//    if (currentTime < 0) {
//        TextLayout layout3;
//        layout3.setFont( font );
//        layout3.setColor( COLOR_BRIGHT_BLUE );
//        layout3.addLine( "-" );
//        gl::Texture hyphenTex = layout3.render( true, false );        
//        gl::draw( hyphenTex,   Vec2f( bgx1 - 40.0f - hyphenTex.getWidth(), bgy1 + 2 ) );
//    }    
//    
    
    TextLayout layout;
    layout.setFont( mFont );
    layout.setColor( mColor );			
    layout.addLine( "-00:00" ); // FIXME
    bool PREMULT = false;
    mTexture = gl::Texture( layout.render( true, PREMULT ) );        
}

void TimeLabel::draw()
{
    if (mTexture) {
        gl::draw(mTexture, mRect.getUpperLeft());
    }
}
