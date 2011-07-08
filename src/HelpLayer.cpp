/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "HelpLayer.h"
#include "CinderFlurry.h"
#include "Globals.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::app;
using namespace std;

HelpLayer::HelpLayer()
{
}

HelpLayer::~HelpLayer()
{
	mApp->unregisterTouchesBegan( mCbTouchesBegan );
	mApp->unregisterTouchesMoved( mCbTouchesMoved );
	mApp->unregisterTouchesEnded( mCbTouchesEnded );
}

void HelpLayer::setInterfaceOrientation( const ci::app::Orientation &orientation )
{
	mInterfaceOrientation = orientation;
	mOrientationMtx = getOrientationMatrix44( orientation, getWindowSize() );
    
    mInterfaceSize = getWindowSize();
	
    if ( isLandscapeOrientation( orientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
    
	//float panelWidth		= mHelpPanelTex.getWidth();
	//float border			= 20.0f;
	
   // mPanelRect.x1	= border;
   // mPanelRect.x2	= border + panelWidth;
	//mCloseRect		= Rectf( mPanelRect.x2 - 40.0f, mPanelRect.y1, mPanelRect.x2, mPanelRect.y1 + 40.0f );
	
    // cancel interactions
    mIsCloseTouched = false;
	
	mHelpLocs[0] = Vec2f( 35.0f, mInterfaceSize.y - 110.0f );
	mHelpLocs[1] = Vec2f( 70.0f, mInterfaceSize.y - 85.0f );
	mHelpLocs[2] = Vec2f( mInterfaceSize.x * 0.5f - 95.0f, mInterfaceSize.y - 85.0f );
	mHelpLocs[3] = Vec2f( mInterfaceSize.x - 292.0f, mInterfaceSize.y - 164.0f );
	mHelpLocs[4] = Vec2f( mInterfaceSize.x - 257.0f, mInterfaceSize.y - 137.0f );
	mHelpLocs[5] = Vec2f( mInterfaceSize.x - 222.0f, mInterfaceSize.y - 110.0f );
	mHelpLocs[6] = Vec2f( mInterfaceSize.x - 155.0f, mInterfaceSize.y - 191.0f );
	
	if( isLandscapeOrientation( orientation ) )
	   mHelpLocs[7] = Vec2f( 35.0f, mInterfaceSize.y * 0.5f );
	else
	   mHelpLocs[7] = Vec2f( mInterfaceSize.x * 0.5f - 151.0f, mInterfaceSize.y - 191.0f );
	
	
	float buttonWidth = 100.0f;
	float border = 12.0f;
	float doubleBorder = border * 2.0f;
	float y1 = 30.0f;
	float y2 = y1 + 50.0f;
	
	mMailButton			= Rectf( mInterfaceSize.x - buttonWidth - border, y1, mInterfaceSize.x - border, y2 );
	mPlanetaryButton	= Rectf( mInterfaceSize.x - buttonWidth * 2.0f - doubleBorder, y1, mInterfaceSize.x - buttonWidth - doubleBorder, y2 );
	mBloomButton		= Rectf( mPanelRect.x1 + 60.0f, mPanelRect.y1 + 94.0f, mPanelRect.x1 + 120.0f, mPanelRect.y1 + 114.0f );
	mCinderButton		= Rectf( mPanelRect.x1 + 60.0f, mPanelRect.y1 + 140.0f, mPanelRect.x1 + 120.0f, mPanelRect.y1 + 160.0f );
}

// TODO: move this to an operator in Cinder's Matrix class?
Rectf HelpLayer::transformRect( const Rectf &rect, const Matrix44f &matrix )
{
    Vec2f topLeft = (matrix * Vec3f(rect.x1,rect.y1,0)).xy();
    Vec2f bottomRight = (matrix * Vec3f(rect.x2,rect.y2,0)).xy();
    Rectf newRect(topLeft, bottomRight);
    newRect.canonicalize();    
    return newRect;
}

void HelpLayer::setup( AppCocoaTouch *app, const Orientation &orientation )
{
	mApp = app;
	
	mCbTouchesBegan       = mApp->registerTouchesBegan( this, &HelpLayer::touchesBegan );
	mCbTouchesMoved       = mApp->registerTouchesMoved( this, &HelpLayer::touchesMoved );
	mCbTouchesEnded       = mApp->registerTouchesEnded( this, &HelpLayer::touchesEnded );

	mIsCloseTouched		= false;

	mHelpPer			= 0.0f;
	mHelpPanelTex		= loadImage( loadResource( "helpPanel.png" ) );
	
	// PANEL AND TAB
	float panelWidth	= mHelpPanelTex.getWidth();
	float panelHeight	= mHelpPanelTex.getHeight();
	float border		= 40.0f;
	
	mPanelRect			= Rectf( 19.0f, border, 19 + panelWidth, border + panelHeight );
	// REMEMBER, ACTUALLY SET IN setInterfaceOrientation() ^^ THATTA WAY
	//mCloseRect			= Rectf( mPanelRect.x2 - 35.0f, mPanelRect.y1, mPanelRect.x2, mPanelRect.y1 + 35.0f );

    // just do orientation stuff in here:
	setInterfaceOrientation( orientation );
}


void HelpLayer::initHelpTextures( const Font &font )
{
	vector<string> names;
	names.push_back( "• FLY TO CURRENT TRACK" );
	names.push_back( "• CURRENT TRACK INFO" );
	names.push_back( "SHOW/HIDE NAV •" );
	names.push_back( "• SHOW/HIDE HELP" );
	names.push_back( "• SHOW/HIDE ORBIT LINES" );
	names.push_back( "• SHOW/HIDE TEXT" );
	names.push_back( "SHOW/HIDE CONTROLS •" );
	names.push_back( "FILTER ARTISTS BY LETTER •" );
	
	int count = 0;
	for( vector<string>::iterator it = names.begin(); it != names.end(); ++it ){
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( ColorA( BRIGHT_YELLOW, 1.0f ) );
		layout.addCenteredLine( names[count] );
		mHelpTextures.push_back( gl::Texture( layout.render( true, false ) ) );
		
		count ++;
	}
}



 
bool HelpLayer::touchesBegan( TouchEvent event )
{
	if( G_HELP ){
		Vec2f touchPos = event.getTouches().begin()->getPos();
	}
	return false;
}

bool HelpLayer::touchesMoved( TouchEvent event )
{
	if( G_HELP ){
		Vec2f touchPos = event.getTouches().begin()->getPos();
	}

	return false;
}

bool HelpLayer::touchesEnded( TouchEvent event )
{
	if( G_HELP ){
		vector<TouchEvent::Touch> touches = event.getTouches();
		
		Vec2f pos = touches.begin()->getPos();
		pos = (mOrientationMtx.inverted() * Vec3f(pos,0)).xy();

		// TODO: should we use a callback for these and handle the actions in the main app?
		
		Url mailToLink( "mailto:planetary@bloom.io?subject=Planetary feedback" );
		Url planetaryWebsite( "http://planetary.bloom.io" );
		Url bloomWebsite( "http://bloom.io" );
		Url cinderWebsite( "http://libcinder.org" );
		
		if( mMailButton.contains( pos ) ){
            Flurry::getInstrumentation()->logEvent("Email Link Selected");            
			launchWebBrowser( mailToLink );
		} else if( mPlanetaryButton.contains( pos ) ){
            Flurry::getInstrumentation()->logEvent("Planetary Link Selected");            
			launchWebBrowser( planetaryWebsite );
			
		} else if( mBloomButton.contains( pos ) ){
            Flurry::getInstrumentation()->logEvent("Bloom Link Selected");            
			launchWebBrowser( bloomWebsite );
			
		} else if( mCinderButton.contains( pos ) ){
            Flurry::getInstrumentation()->logEvent("Cinder Link Selected");            
			launchWebBrowser( cinderWebsite );
		}    
	}
	
	return false;
}

void HelpLayer::update()
{
	if( G_HELP ){
		mHelpPer -= ( mHelpPer - 1.0f ) * 0.2f;
	} else {
		mHelpPer -= mHelpPer * 0.2f;
	}
}

void HelpLayer::draw( const gl::Texture &tex, float y )
{		
	if( mHelpPer > 0.001f ){
		float dragAlphaPer = pow( ( mInterfaceSize.y - y ) / 65.0f, 2.0f );
		float alpha = mHelpPer * dragAlphaPer;
		ColorA base( 1.0f, 1.0f, 1.0f, alpha );
		
		glPushMatrix();
		glMultMatrixf( mOrientationMtx );
		
		gl::color( base );
		gl::enableAdditiveBlending();
		gl::draw( mHelpPanelTex, mPanelRect );
		gl::enableAlphaBlending();
        
        float texw = tex.getWidth();
        float texh = tex.getHeight();
        gl::draw( tex, Area(0.75f * texw, 0.25f * texh, 1.0f * texw, 0.5f * texh), mMailButton );
        gl::draw( tex, Area(0.75f * texw, 0.25f * texh, 1.0f * texw, 0.5f * texh), mPlanetaryButton );
		
		/*
		int count = 0;
		for( vector<gl::Texture>::iterator it = mHelpTextures.begin(); it != mHelpTextures.end(); ++it ){
			Vec2f offset( -3.0f, -15.0f );
			gl::draw( *it, mHelpLocs[count] + offset );
			count ++;
		}
		
		glDisable( GL_TEXTURE_2D );
		gl::color( ColorA( BRIGHT_YELLOW, 0.25f * alpha ) );
		for( int i=0; i<G_TOTAL_HELP_CALLOUTS; i++ ){
			Vec2f offset = Vec2f( 0.0f, 0.0f );
			
			// this is pretty squirrely
			// if i = 2 or 6, the text is right aligned so have to compensate for the placement
			// of the connecting lines. Also, if in landscape, the lines are all misaligned
			// on the x axis by 1. 
			if( isLandscapeOrientation( mInterfaceOrientation ) ){
				if( i == 2 ){
					offset.x = 95.0f;
				} else if( i == 6 ){
					offset.x = 131.0f;
				} else {
					offset.x -= 1.0f;
				}
			} else {
				if( i == 2 ){
					offset.x = 96.0f;
				} else if( i == 6 ){
					offset.x = 132.0f;
				}
			}
			
			if( i < 7 ){ // if we arent drawing the alpha-wheel-help callout...
				gl::drawLine( mHelpLocs[i] + offset, Vec2f( mHelpLocs[i].x, mInterfaceSize.y ) + offset );	
			} else {
				if( isLandscapeOrientation( mInterfaceOrientation ) ){
					Vec2f from = Vec2f( 195.0f, mInterfaceSize.y * 0.5f - 6.0f );
					Vec2f to = from + Vec2f( 36.0f, 0.0f );
					gl::drawLine( from, to );
				} else {
					Vec2f from( mInterfaceSize.x * 0.5f + 1.0f, mInterfaceSize.y - 204.0f ); 
					Vec2f to = from + Vec2f( 0.0f, -28.0f );
					gl::drawLine( from, to );
				}
			}
			
			if( i == 7 ){
				gl::color( base );
				
				gl::enableAdditiveBlending();
				if( isLandscapeOrientation( mInterfaceOrientation ) ){
					float yMid	= mInterfaceSize.y * 0.5f;
					float w		= 100.0f;
					Rectf arcRect( -w, -15.0f, w, 15.0f );
					glPushMatrix();
					gl::translate( Vec2f( 239.0f, yMid - 6.0f ) );
					gl::rotate( 90.0f );
                    float tw = tex.getWidth();
                    float th = tex.getHeight();
                    gl::draw( tex, Area(0.5f*tw, 0.75f*th, 1.0f*tw, 0.9f*th), arcRect );
					glPopMatrix();
				} else {
					float xMid	= mInterfaceSize.x * 0.5f;
					float w		= 100.0f;
					Rectf arcRect( xMid - w, mInterfaceSize.y - 255.0f, xMid + w, mInterfaceSize.y - 225.0f );
                    float tw = tex.getWidth();
                    float th = tex.getHeight();
                    gl::draw( tex, Area(0.5f*tw, 0.75f*th, 1.0f*tw, 0.9f*th), arcRect );                    
				}
				gl::enableAlphaBlending();
			}
		}
		 
		 */
		glPopMatrix(); 
	}
}
