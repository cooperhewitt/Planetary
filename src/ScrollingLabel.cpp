//
//  ScrollingLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ScrollingLabel.h"

void ScrollingLabel::setText(string text)
{ 
    if (mText != text) { 
        mText = text; 
        updateTexture(); 
    }
}

void ScrollingLabel::updateTexture()
{
    if (mText == "") {
        mTexture.reset();
    }
    else {
		TextLayout layout;
		layout.setFont( mFont );
		layout.setColor( mColor );			
		layout.addLine( mText );
		bool PREMULT = false;
		mTexture = gl::Texture( layout.render( true, PREMULT ) );        
    }
}

void ScrollingLabel::draw()
{
    // TODO: little fady bits, yer actual scrolling text
    
    // !!! currentTrackRect below is now mRect...
    
//    currentTrackTex.enableAndBind();
//    float ctSpaceWidth = ctw;
//    float ctTexWidth = currentTrackTex.getWidth();
//    float ctu1 = 0.0f;
//    float ctu2;
//    float ctPer = ctSpaceWidth/ctTexWidth;
//    
//    if( ctTexWidth < ctSpaceWidth ){ // if the texture width is less than the rect to fit it in...
//        ctu2 = ctPer;
//        drawButton( currentTrackRect, ctu1, 0.0f, ctu2, 1.0f );
//        
//    } else {				// if the texture is too wide, animate the u coords
//        
//        float x1;
//        if( app::getElapsedSeconds() - secsSinceTrackChange > 5.0f ){	// but wait 5 seconds first
//            x1 = fmodf( ( app::getElapsedSeconds() - ( secsSinceTrackChange + 5.0f ) )*20.0f, ctTexWidth + 10.0f ) - ctTexWidth-10.0f;
//        } else {
//            x1 = -ctTexWidth-10.0f;
//        }
//        
//        Area a1 = Area( x1, 0.0f, x1 + ctSpaceWidth, currentTrackTex.getHeight() );
//        gl::draw( currentTrackTex, a1, currentTrackRect );
//        
//        Area a2 = Area( x1 + ctTexWidth + 10.0f, 0.0f, x1 + ctTexWidth + 10.0f + ctSpaceWidth, currentTrackTex.getHeight() );
//        gl::draw( currentTrackTex, a2, currentTrackRect );			
//        
//        Area aLeft		 = Area( 200.0f, 140.0f, 214.0f, 150.0f ); 
//        Rectf coverLeft  = Rectf( currentTrackRect.x1, currentTrackRect.y1, currentTrackRect.x1 + 10.0f, currentTrackRect.y2 );
//        Rectf coverRight = Rectf( currentTrackRect.x2 + 1.0f, currentTrackRect.y1, currentTrackRect.x2 - 9.0f, currentTrackRect.y2 );
//        
//        gl::draw( uiButtonsTex, aLeft, coverLeft );
//        gl::draw( uiButtonsTex, aLeft, coverRight );
//    }    
    
    if (mTexture) {
        gl::draw(mTexture, mRect.getUpperLeft());    
    }
}
