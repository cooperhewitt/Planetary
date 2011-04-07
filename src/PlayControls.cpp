/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayControls.h"
#include "Globals.h"

void PlayControls::setup( AppCocoaTouch *app, bool initialPlayState )
{
    mApp			= app;
    // TODO: unregister these in destructor!
    cbTouchesBegan	= mApp->registerTouchesBegan( this, &PlayControls::touchesBegan );
    cbOrientationChanged = mApp->registerOrientationChanged( this, &PlayControls::orientationChanged );    
    cbTouchesEnded	= 0;
    cbTouchesMoved	= 0;		
    lastTouchedType = NO_BUTTON;
    mIsPlaying		= initialPlayState;
    mMinutes		= 0;
    mSeconds		= 60;
    mPrevSeconds	= 0;
    mIsDraggingPlayhead = false;
    mIsDrawingRings = true;
    mIsDrawingStars = true;
    mIsDrawingPlanets = true;    
    mDeviceOrientation = PORTRAIT_ORIENTATION; // TODO: init from mApp (add getter to AppCocoaTouch)
    mInterfaceSize = mApp->getWindowSize();
}

void PlayControls::update()
{
    // TODO: update anything time based here, e.g. elapsed time of track playing
    // or e.g. button animation
    
    // clean up listeners here, because if we remove in touchesEnded then things get crazy
    if (mApp->getActiveTouches().size() == 0 && cbTouchesEnded != 0) {
        mApp->unregisterTouchesEnded( cbTouchesEnded );
        mApp->unregisterTouchesMoved( cbTouchesMoved );
        cbTouchesEnded = 0;
        cbTouchesMoved = 0;
    }		
}

bool PlayControls::touchesBegan( TouchEvent event )
{
    Rectf transformedBounds = transformRect( lastDrawnBoundsRect, mOrientationMtx );
    vector<TouchEvent::Touch> touches = event.getTouches();
    if (touches.size() > 0 && transformedBounds.contains(touches[0].getPos())) {
        if (cbTouchesEnded == 0) {
            cbTouchesEnded = mApp->registerTouchesEnded( this, &PlayControls::touchesEnded );
            cbTouchesMoved = mApp->registerTouchesMoved( this, &PlayControls::touchesMoved );			
        }

        lastTouchedType = findButtonUnderTouches(touches);
        
        if( lastTouchedType == SLIDER ){
            mIsDraggingPlayhead = true;
        }
        
        return true;
    }
    else {
        lastTouchedType = NO_BUTTON;
    }
    return false;
}

bool PlayControls::touchesMoved( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();

    lastTouchedType = findButtonUnderTouches(touches);

    if( mIsDraggingPlayhead ){
        if( touches.size() == 1 ){
            
            Vec2f pos = touches.begin()->getPos();
            pos = (mOrientationMtx.inverted() * Vec3f(pos,0)).xy();
            
            float border = ( mInterfaceSize.x - 628 ) * 0.5f;
            float playheadPer = ( pos.x - border ) / 628;
            
            // TODO: Make this callback take a double (between 0 and 1?)
            mCallbacksPlayheadMoved.call(playheadPer);
        }
    }
    return false;
}	

bool PlayControls::touchesEnded( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();
    if (lastTouchedType != NO_BUTTON && lastTouchedType == findButtonUnderTouches(touches)) {
        mCallbacksButtonPressed.call(lastTouchedType);
    }
    lastTouchedType = NO_BUTTON;
    mIsDraggingPlayhead = false;
    return false;
}

bool PlayControls::orientationChanged( OrientationEvent event )
{
    if ( event.isValidInterfaceOrientation() ) {
        mDeviceOrientation = event.getOrientation();
    }
    else {
        return false;
    }
    
    Vec2f deviceSize = app::getWindowSize();
    
    mOrientationMtx = event.getOrientationMatrix();
    
    mInterfaceSize = deviceSize;
    
    if ( event.isLandscape() ) {
        mInterfaceSize = mInterfaceSize.yx();
    }
    
	return false;
}

void PlayControls::draw( const vector<gl::Texture> &texs, const Font &font, float y, float currentTime, float totalTime, bool isDrawingRings, bool isDrawingText )
{    
    gl::pushModelView();
    gl::multModelView( mOrientationMtx );    
    
    lastDrawnBoundsRect = Rectf(0, y, mInterfaceSize.x, mInterfaceSize.y );
    
    gl::color( Color( 0.0f, 0.0f, 0.0f ) );
    gl::drawSolidRect( lastDrawnBoundsRect ); // TODO: make height settable in setup()?
    
    touchRects.clear();
    touchTypes.clear(); // technically touch types never changes, but whatever
    
    float bWidth		= 55.0f;
    float bHeight		= 55.0f;
	float topBorder		= 7.0f;
	float sideBorder	= 10.0f;
	float buttonGap		= 1.0f;
	float sliderWidth	= mInterfaceSize.x * 0.3f;
    float sliderHeight	= 22.0f;
    float sliderInset	= bWidth * 2.0f;
	float x1, x2, y1, y2;
	
	
	y1 = y + topBorder;
	y2 = y1 + bHeight;
	
	
	x1 = sideBorder;
	x2 = x1 + bWidth;
	Rectf currentTrackButton( x1, y1, x2, y2 );
	
	x1 = bWidth * 3.0f + sliderWidth;
	x2 = x1 + bWidth;
	Rectf showWheelButton( x1, y1, x2, y2 );
	
	x1 = mInterfaceSize.x - sideBorder - bWidth;
	x2 = x1 + bWidth;
	Rectf nextButton( x1, y1, x2, y2 );
	
	x1 -= bWidth + buttonGap;
	x2 = x1 + bWidth;
	Rectf playButton( x1, y1, x2, y2 );
	
	x1 -= bWidth + buttonGap;
	x2 = x1 + bWidth;
	Rectf prevButton( x1, y1, x2, y2 );


	/*
	Rectf helpButton( x1 + 5.0f, y1 + 6, x1 + 35.0f, y1 + 36 );
    Rectf drawRingsButton( x1 + 40.0f, y1 + 6, x1 + 70.0f, y1 + 36 );
    Rectf drawTextButton( x1 + 75.0f, y1 + 6, x1 + 105.0f, y1 + 36 );
	*/
	//    Rectf currentTrackButton( x1 + 110.0f, y1 + 6, x1 + 140.0f, y1 + 36 );
    
    
    float playheadPer	= 0.0f;
    if( totalTime > 0.0f ){
        playheadPer = currentTime/totalTime;
    }
    float bgx1			= sliderInset;
    float bgx2			= bgx1 + sliderWidth;
    float bgy1			= y + 34.0f;
    float bgy2			= bgy1 + sliderHeight;
    float fgx1			= sliderInset;
    float fgx2			= fgx1 + ( sliderWidth * playheadPer );
    float fgy1			= bgy1;
    float fgy2			= bgy2;
    
    Rectf sliderBg( bgx1, bgy1, bgx2, bgy2 );
    Rectf sliderFg( fgx1, fgy1, fgx2, fgy2 );
    Rectf sliderButton( fgx2 - 16.0f, ( fgy1 + fgy2 ) * 0.5f - 16.0f, fgx2 + 16.0f, ( fgy1 + fgy2 ) * 0.5f + 16.0f );
    
	touchRects.push_back( currentTrackButton );
	touchTypes.push_back( CURRENT_TRACK );
	touchRects.push_back( showWheelButton );
	touchTypes.push_back( SHOW_WHEEL );
    touchRects.push_back( prevButton );
    touchTypes.push_back( PREVIOUS_TRACK );
    touchRects.push_back( playButton );
    touchTypes.push_back( PLAY_PAUSE );
    touchRects.push_back( nextButton );
    touchTypes.push_back( NEXT_TRACK );
    touchRects.push_back( sliderButton );
    touchTypes.push_back( SLIDER );
	/*
    touchRects.push_back( helpButton );
    touchTypes.push_back( HELP );
    touchRects.push_back( drawRingsButton );
    touchTypes.push_back( DRAW_RINGS );
    touchRects.push_back( drawTextButton );
    touchTypes.push_back( DRAW_TEXT );
	 */
	
	//	touchRects.push_back( currentTrackButton );
	//    touchTypes.push_back( CURRENT_TRACK );
	
	
	gl::color( Color::white() );
    texs[ TEX_BUTTONS ].enableAndBind();
	
	
	
	float u1, u2, v1, v2, v3;
	v1 = 0.0f; v2 = 0.25f; v3 = 0.5f;
	float uw = 1.0f/6.0f;
// CURRENT TRACK
	u1 = uw * 0.0f;
	u2 = u1 + uw;
    if( lastTouchedType == CURRENT_TRACK )
		drawButton( currentTrackButton, u1, u2, v2, v3 );
    else
		drawButton( currentTrackButton, u1, u2, v1, v2 );

	
// SHOW WHEEL
	u1 = uw * 1.0f;
	u2 = u1 + uw;
    if( lastTouchedType == SHOW_WHEEL )
		drawButton( showWheelButton, u1, u2, v2, v3 );
    else
		drawButton( showWheelButton, u1, u2, v1, v2 );	
	
	
// PREV
	u1 = uw * 2.0f;
	u2 = u1 + uw;
    if( lastTouchedType == PREVIOUS_TRACK )
		drawButton( prevButton, u1, u2, v2, v3 );
    else
		drawButton( prevButton, u1, u2, v1, v2 );
    
    
// PLAY/PAUSE	
	if( ! mIsPlaying )
		u1 = uw * 3.0f;
	else
		u1 = uw * 4.0f;
	u2 = u1 + uw;
	if( lastTouchedType == PLAY_PAUSE )
		drawButton( playButton, u1, u2, v2, v3 );
	else
		drawButton( playButton, u1, u2, v1, v2 );

    
// NEXT
	u1 = uw * 5.0f;
	u2 = u1 + uw;
    if( lastTouchedType == NEXT_TRACK )
		drawButton( nextButton, u1, u2, v2, v3 );
    else
		drawButton( nextButton, u1, u2, v1, v2 );
	
	
// SLIDER BG
	u1 = 0.0f; u2 = 1.0f;
	v1 = 0.5f; v2 = 0.6f;
    drawButton( sliderBg, u1, u2, v1, v2 );
	
// SLIDER FG
	u1 = 0.0f; u2 = 1.0f;
	v1 = 0.6f; v2 = 0.7f;
    drawButton( sliderFg, u1, u2, v1, v2 );
	
    /*
    // HELP
    if( G_HELP ) gl::color( Color( 1.0f, 1.0f, 1.0f ) );
    else		  gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
    texs[ TEX_HELP ].enableAndBind();
    gl::drawSolidRect( helpButton );
    
    // DRAW RINGS
    if( isDrawingRings ) gl::color( Color( 1.0f, 1.0f, 1.0f ) );
    else				 gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
    texs[ TEX_DRAW_RINGS ].enableAndBind();
    gl::drawSolidRect( drawRingsButton );
    
    // DRAW TEXT	
    if( isDrawingText )		gl::color( Color( 1.0f, 1.0f, 1.0f ) );
    else					gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
    texs[ TEX_DRAW_TEXT ].enableAndBind();
    gl::drawSolidRect( drawTextButton );
    */
	
	// CURRENT TRACK		
//    if( lastTouchedType == CURRENT_TRACK ) texs[ TEX_CURRENT_TRACK_ON ].enableAndBind();
//    else texs[ TEX_CURRENT_TRACK ].enableAndBind();
//	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
//    gl::drawSolidRect( currentTrackButton );
    
	
    
    // CURRENT TIME
    mMinutes		= floor( currentTime/60 );
    mPrevSeconds	= mSeconds;
    mSeconds		= (int)currentTime%60;
    
    mMinutesTotal	= floor( totalTime/60 );
    mSecondsTotal	= (int)totalTime%60;
    
    double timeLeft = totalTime - currentTime;
    mMinutesLeft	= floor( timeLeft/60 );
    mSecondsLeft	= (int)timeLeft%60;
    
    if( mSeconds != mPrevSeconds ){
        string minsStr = to_string( mMinutes );
        string secsStr = to_string( mSeconds );
        if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
        if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
        stringstream ss;
        ss << minsStr << ":" << secsStr;
        TextLayout layout;
        layout.setFont( font );
        layout.setColor( Color::white() );
        layout.addLine( ss.str() );
        mCurrentTimeTex = layout.render( true, false );
        
        
        
        minsStr = to_string( mMinutesLeft );
        secsStr = to_string( mSecondsLeft );
        if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
        if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
        
        ss.str("");
        ss << "-" << minsStr << ":" << secsStr;
        TextLayout layout2;
        layout2.setFont( font );
        layout2.setColor( Color::white() );
        layout2.addLine( ss.str() );
        mRemainingTimeTex = layout2.render( true, false );
    }
    gl::draw( mCurrentTimeTex,   Vec2f( bgx1 - 40.0f, bgy1 + 1 ) );
    gl::draw( mRemainingTimeTex, Vec2f( bgx2 + 8.0f, bgy1 + 1 ) );
    
    

//
    gl::color( Color::white() );
	gl::enableAlphaBlending();
    texs[TEX_SLIDER_BUTTON].enableAndBind();
    gl::drawSolidRect( sliderButton );
    texs[TEX_SLIDER_BUTTON].disable();
    
	 
	 
    gl::popModelView();
	
	//    gl::color( Color( 1.0f, 0, 0 ) );
	//    gl::drawSolidRect( transformRect( lastDrawnBoundsRect, mOrientationMtx ) );
}



void PlayControls::drawButton( const Rectf &rect, float u1, float u2, float v1, float v2 )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	GLfloat verts[8];
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8];
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	int vi = 0;
	int ti = 0;
	verts[vi++] = rect.getX2(); texCoords[ti++] = u2;
	verts[vi++] = rect.getY1(); texCoords[ti++] = v1;
	verts[vi++] = rect.getX1(); texCoords[ti++] = u1;
	verts[vi++] = rect.getY1(); texCoords[ti++] = v1;
	verts[vi++] = rect.getX2(); texCoords[ti++] = u2;
	verts[vi++] = rect.getY2(); texCoords[ti++] = v2;
	verts[vi++] = rect.getX1(); texCoords[ti++] = u1;
	verts[vi++] = rect.getY2(); texCoords[ti++] = v2;
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}

PlayControls::PlayButton PlayControls::findButtonUnderTouches(vector<TouchEvent::Touch> touches) {
    Rectf transformedBounds = transformRect( lastDrawnBoundsRect, mOrientationMtx );
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        Vec2f pos = touch.getPos();
        if ( transformedBounds.contains( pos ) ) {
            for (int i = 0; i < touchRects.size(); i++) {
                Rectf rect = transformRect( touchRects[i], mOrientationMtx );
//                if (touchTypes[i] == SLIDER) {
//                    std::cout << "testing slider rect: " << touchRects[i] << std::endl;
//                    std::cout << "      transformRect: " << rect << std::endl;
//                    std::cout << "                pos: " << pos << std::endl;
//                }
                if (rect.contains(pos)) {
//                    if (touchTypes[i] == SLIDER) {
//                        std::cout << "HIT slider rect!" << std::endl;
//                    }
                    return touchTypes[i];
                }
            }		
        }
    }		
    return NO_BUTTON;
}

// TODO: move this to an operator in Cinder's Matrix class?
Rectf PlayControls::transformRect( const Rectf &rect, const Matrix44f &matrix )
{
    Vec2f topLeft = (matrix * Vec3f(rect.x1,rect.y1,0)).xy();
    Vec2f bottomRight = (matrix * Vec3f(rect.x2,rect.y2,0)).xy();
    Rectf newRect(topLeft, bottomRight);
    newRect.canonicalize();    
    return newRect;
}
