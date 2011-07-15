/*
 *  NodeTrack.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeTrack.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"
#include "cinder/ip/Resize.h"
#include "OrbitRing.h"
#include "BloomGl.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeTrack::NodeTrack( Node *parent, int index, const Font &font, const Font &smallFont, const Surface &hiResSurfaces, const Surface &loResSurfaces, const Surface &noAlbumArt )
	: Node( parent, index, font, smallFont, hiResSurfaces, loResSurfaces, noAlbumArt )
{	
	mGen				= G_TRACK_LEVEL;
	mPos				= mParentNode->mPos;
	mIsHighlighted		= true;
    mIsPlaying			= false;
	mHasClouds			= false;
	mIsMostPlayed		= false;
	mHasAlbumArt		= false;
	mHasCreatedAlbumArt = false;
	
	mTotalOrbitVertices		= 0;
	mPrevTotalOrbitVertices = -1;
    mOrbitVerts				= NULL;
	mOrbitTexCoords			= NULL;
	mOrbitColors			= NULL;
	
	mShadowVerts		= NULL;
	mShadowTexCoords	= NULL;
	
	mMyTime				= Rand::randFloat( 250.0 );
}

void NodeTrack::setData( TrackRef track, PlaylistRef album, const Surface &albumArt )
{
	mAlbumArtSurface = albumArt;
	mAlbum			= album;
// TRACK INFORMATION
	mTrack			= track;
	mTrackLength	= track->getLength();
	mPlayCount		= track->getPlayCount() + 1.0f; // TODO: fix in calculations that use playcount so that mPlayCount can remain accurate
	mStarRating		= track->getStarRating();
    mId             = track->getItemId();
    
	string name		= getName();
	char c1			= ' ';
	if( name.length() >= 3 ){
		c1 = name[1];
	}
	
	int c1Int = constrain( int(c1), 32, 127 );
	
	mAsciiPer = ( c1Int - 32 )/( 127.0f - 32 );
	
	
	
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	if( playCountDelta == 1.0f )
		mNormPlayCount		= 1.0f;
	else
		mNormPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
	
	
	
	if( mStarRating == 0 ){ // no star rating
		mPlanetTexIndex			= constrain( (int)( mNormPlayCount * 4 ), 0, 4 );
		mCloudTexIndex			= c1Int%G_NUM_CLOUD_TYPES + G_NUM_CLOUD_TYPES;
	} else {
		mPlanetTexIndex			= mStarRating - 1;
		mCloudTexIndex			= mStarRating - 1 + G_NUM_CLOUD_TYPES;
	}
	
	if( album->size() > 1 ){
		if( mPlayCount >= mParentNode->mHighestPlayCount ){
			mIsMostPlayed = true;
		}
	}
	

	mOrbitPath.clear();
	
	mHue				= mAsciiPer;
	mSat				= ( 1.0f - sin( mHue * M_PI ) ) * 0.1f + 0.15f;
	mColor				= Color( CM_HSV, mHue, mSat * 0.5f, 1.0f );
	mGlowColor			= mParentNode->mGlowColor;
	mEclipseColor		= mColor;
	
	mRadiusDest 		= math<float>::max( (mParentNode->mRadiusInit * 0.1f) * pow( mNormPlayCount + 0.5f, 2.0f ), 0.0025f );
	mRadiusInit			= mRadiusDest;
	mRadius				= 0.0f;
	mSphere				= Sphere( mPos, mRadiusDest );
	mIdealCameraDist	= 0.15f;//math<float>::max( mRadiusDest * 5.0f, 0.5f );
	mCloudLayerRadius	= mRadiusDest * 0.025f;
	
	mOrbitPeriod		= mTrackLength;

	setStartAngle();
	
	mInitAngle			= mOrbitStartAngle;
	mAxialTilt			= Rand::randFloat( -5.0f, 20.0f );
    mAxialVel			= Rand::randFloat( 15.0f, 20.0f );
	mAxialRot			= Vec3f( 0.0f, Rand::randFloat( 150.0f ), mAxialTilt );

	// TEXTURE CREATION MOVED TO UPDATE
}


void NodeTrack::setStartAngle()
{
	mPercentPlayed		= 0.0f;
	float timeOffset	= (float)mMyTime/mOrbitPeriod;
	mOrbitStartAngle	= timeOffset * TWO_PI;
	
    if (G_DEBUG) {
        float angle	= atan2( mPos.z - mParentNode->mPos.z, mPos.x - mParentNode->mPos.x );
        std::cout << "Start Angle set in NodeTrack: " << mOrbitStartAngle << std::endl;
        std::cout << "mPercentPlayed: " << mPercentPlayed << std::endl;
        std::cout << "timeOffset: " << timeOffset << std::endl;
        std::cout << "mMyTime: " << mMyTime << std::endl;
        std::cout << "angle: " << angle << std::endl;
        std::cout << " ================= " << std::endl;
    }
}


void NodeTrack::updateAudioData( double currentPlayheadTime )
{
	if( mIsPlaying ){
		//std::cout << "NodeTrack::updateAudioData()" << std::endl;
		mPercentPlayed		= currentPlayheadTime/mTrackLength;
		mOrbitAngle			= mPercentPlayed * TWO_PI + mOrbitStartAngle;
		
//		std::cout << "CurrentPlayheadTime = " << currentPlayheadTime << std::endl;
		
		// TODO: Find a better way to do this without clearing mOrbitPath every frame.
		mOrbitPath.clear();
		
		// Add start position
		mOrbitPath.push_back( Vec3f( cos( mOrbitStartAngle ), 0.0f, sin( mOrbitStartAngle ) ) );
		
		// Add middle positions
		int maxNumVecs		= 400;
		int currentNumVecs	= mPercentPlayed * maxNumVecs;
		float invMaxNumVecs	= 1.0f/(float)maxNumVecs;
		for( int i=0; i<currentNumVecs; i++ ){
			float per = (float)i*invMaxNumVecs;
			float angle = mOrbitStartAngle + per * TWO_PI;
			Vec2f pos = Vec2f( cos( angle ), sin( angle ) );
			
			mOrbitPath.push_back( Vec3f( pos.x, 0.0f, pos.y ) );
		}
		
		// Add end position
		mOrbitPath.push_back( Vec3f( cos( mOrbitAngle ), 0.0f, sin( mOrbitAngle ) ) );
		
		buildPlayheadProgressVertexArray();
	}
}



void NodeTrack::buildPlayheadProgressVertexArray()
{
	int orbitPathSize	= mOrbitPath.size();
	mTotalOrbitVertices	= orbitPathSize * 2;
	
	if( mTotalOrbitVertices != mPrevTotalOrbitVertices ){
		if (mOrbitVerts != NULL)		delete[] mOrbitVerts;
		if( mOrbitTexCoords != NULL)	delete[] mOrbitTexCoords;
		
		mOrbitVerts		= new float[mTotalOrbitVertices*3];
		mOrbitTexCoords	= new float[mTotalOrbitVertices*2];
		
		mPrevTotalOrbitVertices = mTotalOrbitVertices;
	}
	
	int vIndex		= 0;
	int tIndex		= 0;
	int index		= 0;
	float radius	= mRadius * 1.1f;
//	float alpha		= constrain( G_ZOOM - G_ARTIST_LEVEL, 0.0f, 1.0f ) * 0.3f;
	
	for( vector<Vec3f>::iterator it = mOrbitPath.begin(); it != mOrbitPath.end(); ++it )
	{
		float per				= (float)index/(float)orbitPathSize;
		Vec3f pos1				= *it * ( mOrbitRadius + radius );
		Vec3f pos2				= *it * ( mOrbitRadius - radius );
		
		mOrbitVerts[vIndex++]	= pos1.x;
		mOrbitVerts[vIndex++]	= pos1.y;
		mOrbitVerts[vIndex++]	= pos1.z;
		
		mOrbitVerts[vIndex++]	= pos2.x;
		mOrbitVerts[vIndex++]	= pos2.y;
		mOrbitVerts[vIndex++]	= pos2.z;
		
		mOrbitTexCoords[tIndex++]	= per;//mMyTime;
		mOrbitTexCoords[tIndex++]	= 0.0f;
		
		mOrbitTexCoords[tIndex++]	= per;//mMyTime;
		mOrbitTexCoords[tIndex++]	= 1.0f;
		
		index ++;
	}
}


void NodeTrack::update( float param1, float param2 )
{	
	//////////////////////
	// CREATE MOON TEXTURE
	if( !mHasCreatedAlbumArt && mAge>mIndex * 2 ){
        int albumArtWidth   = mAlbumArtSurface.getWidth();
		if( albumArtWidth > 256 ) albumArtWidth = 256; // FIXME: This is here because the album art is coming back at 320x320 for a 256x256 image request
		int totalWidth		= albumArtWidth/2; // TODO: rename these?
		int halfWidth		= totalWidth/2;
		if( mAlbumArtSurface ){
            
			// using 'totalwidth' here because the album art surface that is
			// being provided by albumNode is 256x256 so the bit that I pull
			// should be from a 256x256 texture, despite what our eventual
			// texture size will be.
			int x			= (int)( totalWidth - mNormPlayCount*totalWidth);
			int y			= (int)( halfWidth + halfWidth*mAsciiPer );
			
			int w			= (int)( mNormPlayCount*totalWidth*2 );
			int h			= (int)( mNormPlayCount*totalWidth );
            
			// grab a section of the album art
			Area a			= Area( x, y, x+w, y+h );
            //			std::cout << "area = " << a << std::endl;
			Surface crop	= Surface( totalWidth, totalWidth, false );
			Surface crop2	= Surface( totalWidth, totalWidth, false );
			ci::ip::resize( mAlbumArtSurface, a, &crop, Area( 0, 0, halfWidth, totalWidth ), FilterCubic() );
            
			// iterate through it to make it a mirror image
			Surface::Iter iter = crop2.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					int xi, yi;
					if( iter.x() >= halfWidth ){
						xi = iter.x() - halfWidth;
						yi = iter.y();
					} else {
						xi = (halfWidth-1) - iter.x();
						yi = iter.y();	
					}
					ColorA c = crop.getPixel( Vec2i( xi, yi ) );
					iter.r() = c.r * 255.0f;
					iter.g() = c.g * 255.0f;
					iter.b() = c.b * 255.0f;
				}
			}
			
			
            
			// fix the polar pinching
			Surface::Iter iter2 = crop.getIter();
			while( iter2.line() ) {
				float cosTheta = cos( M_PI * ( iter2.y() - (float)( totalWidth - 1 )/2.0f ) / (float)( totalWidth - 1 ) );
				
				while( iter2.pixel() ) {
					float phi	= TWO_PI * ( iter2.x() - halfWidth ) / (double)totalWidth;
					float phi2	= phi * cosTheta;
					int i2 = phi2 * totalWidth/TWO_PI + halfWidth;
					
					if( i2 < 0 || i2 > totalWidth-1 ){
						iter2.r() = 255.0f;
						iter2.g() = 0.0f;
						iter2.b() = 0.0f;
					} else {
						ColorA c = crop2.getPixel( Vec2i( i2, iter2.y() ) );
						iter2.r() = c.r * 255.0f;
						iter2.g() = c.g * 255.0f;
						iter2.b() = c.b * 255.0f;
					}
				}
			}
			
			
			// add the planet texture
			// and add the shadow from the cloud layer
            int lowResWidth = mLowResSurfaces.getWidth();
			Area planetArea			= Area( 0, lowResWidth * mPlanetTexIndex, lowResWidth, lowResWidth * ( mPlanetTexIndex + 1 ) );
			Surface planetSurface	= mLowResSurfaces.clone( planetArea );
			
			iter = planetSurface.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					Vec2i v( iter.x(), iter.y() );
					ColorA albumColor	= crop.getPixel( v );
					ColorA surfaceColor	= planetSurface.getPixel( v );
					float planetVal		= surfaceColor.r;
					float cloudShadow	= surfaceColor.g * 0.5f + 0.5f;
					
					ColorA final		= albumColor * planetVal;
					final *= cloudShadow;
					
					iter.r() = final.r * 255.0f;// + 25.0f;
					iter.g() = final.g * 255.0f;// + 25.0f;
					iter.b() = final.b * 255.0f;// + 25.0f;
				}
			}
            
            gl::Texture::Format fmt;
            fmt.enableMipmapping( true );
            fmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
			
			
			mAlbumArtTex		= gl::Texture( planetSurface, fmt );
			mHasAlbumArt		= true;
			
			mHasCreatedAlbumArt = true;
		}
		
	}
	// END CREATE MOON TEXTURE	
	//////////////////////////
	
	
	mRadiusDest		= mRadiusInit * param1;
	mRadius			-= ( mRadius - mRadiusDest ) * 0.2f;
	mSphere			= Sphere( mPos, mRadius );
	
	mPrevTime		= mCurrentTime;
	mCurrentTime	= (float)app::getElapsedSeconds();
	
	if( !mIsPlaying ){
		mMyTime			+= mCurrentTime - mPrevTime;
		mMyTime += param2 * 50.0f;
	}
	
	float timeOffset	= mMyTime/mOrbitPeriod;
	if( !mIsPlaying ){
		//mOrbitAngle	+= param2;
		mAxialRot.y -= mAxialVel * ( param2 * 15.0f );
	} else {
		//mOrbitAngle = ( mPercentPlayed + timeOffset ) * TWO_PI;// + mOrbitStartAngle;
		mAxialRot.y -= mAxialVel * 0.1f;
	}
	mOrbitAngle			= ( mPercentPlayed + timeOffset ) * TWO_PI;
	
	float orbitDelta	= mOrbitAngle - mParentNode->mOrbitAngle;
    
	if( cos( orbitDelta ) > 0 )
		mShadowPer		= max( pow( abs( sin( orbitDelta ) ), 0.5f ) * ( 1.0f + mParentNode->mRadius * 12.0f ) - mParentNode->mRadius * 12.0f, 0.0f );
	else
		mShadowPer		= 1.0f;
	
	Vec3f prevPos = mPos;
    
	mRelPos				= Vec3f( cos( mOrbitAngle ), 0.0f, sin( mOrbitAngle ) ) * mOrbitRadius;
	mPos				= mParentNode->mPos + mRelPos;
	
	if( mIsPlaying ){
		mStartRelPos	= Vec3f( cos( mOrbitStartAngle ), 0.0f, sin( mOrbitStartAngle ) ) * mOrbitRadius;
		mTransStartPos	= mParentNode->mPos + mStartRelPos;
	}
    
	
    /////////////////////////
    // CALCULATE ECLIPSE VARS
	if( mParentNode->mParentNode->mDistFromCamZAxisPer > 0.001f && mDistFromCamZAxisPer > 0.001f )
	{
		Vec2f p		= mScreenPos;
		float r		= mSphereScreenRadius;
		float rsqrd = r * r;
		
		Vec2f P		= mParentNode->mParentNode->mScreenPos;
		float R		= mParentNode->mParentNode->mSphereScreenRadius * 0.85f;
		float Rsqrd	= R * R;
		float A		= M_PI * Rsqrd;
		
		float c		= p.distance( P );
		mEclipseDirBasedAlpha = 1.0f - constrain( c, 0.0f, 2750.0f )/2750.0f;
		if( mEclipseDirBasedAlpha > 0.9f )
			mEclipseDirBasedAlpha = 0.9f - ( mEclipseDirBasedAlpha - 0.9f ) * 9.0f;
		
		if( c < r + R )
		{
			float csqrd = c * c;
			float cos1	= ( Rsqrd + csqrd - rsqrd )/( 2.0f * R * c );
			float CBA	= acos( constrain( cos1, -1.0f, 1.0f ) );
			float CBD	= CBA * 2.0f;
			
			float cos2	= ( rsqrd + csqrd - Rsqrd )/( 2.0f * r * c );
			float CAB	= acos( constrain( cos2, -1.0f, 1.0f ) );
			float CAD	= CAB * 2.0f;
			float intersectingArea = CBA * Rsqrd - 0.5f * Rsqrd * sin( CBD ) + 0.5f * CAD * rsqrd - 0.5f * rsqrd * sin( CAD );
			mEclipseStrength = pow( 1.0f - ( A - intersectingArea ) / A, 2.0f );
			mParentNode->mParentNode->mEclipseStrength += mEclipseStrength;
			//if( mEclipseStrength > mParentNode->mEclipseStrength )
			//	mParentNode->mParentNode->mEclipseStrength = mEclipseStrength;
		}
        
		mEclipseAngle = atan2( P.y - p.y, P.x - p.x );
	}
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * ( 1.0f - mEclipseStrength * 0.5f );
    // END CALCULATE ECLIPSE VARS
    /////////////////////////////
	
	//mClosenessFadeAlpha = constrain( ( mDistFromCamZAxis - mRadius ) * 80.0f, 0.0f, 1.0f );
	
	Node::update( param1, param2 );
    
	mVel = mPos - prevPos;	
}

void NodeTrack::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
		if( mEclipseStrength > mParentNode->mParentNode->mEclipseStrength )
			mParentNode->mParentNode->mEclipseStrength = mEclipseStrength;
	}
}

void NodeTrack::drawPlanet( const gl::Texture &tex )
{	
	if( mSphereScreenRadius > 0.5f && mClosenessFadeAlpha > 0.0f )
	{
        // ROBERT: this was crashing so I put a check for texture existence first
        if (mHasCreatedAlbumArt) {
            mAlbumArtTex.enableAndBind();
        }

//        if (G_DEBUG) {
//            Vec2f center = app::getWindowCenter();
//            Vec2f dir		= mScreenPos - center;
//            float dirLength = dir.length()/500.0f;
//            float angle		= dirLength > 0.999f ? atan2( dir.y, dir.x ) : 0.0f;
//            float stretch	= 1.0f + dirLength * 0.1f;
//            gl::color( Color::white() );
//            Vec2f size = Vec2f( mRadius * stretch, mRadius ) * 2.45f;
//            gl::drawBillboard( mPos, size, -toDegrees( angle ), mBbRight, mBbUp );        
//        }
//        else {
            glPushMatrix();
            gl::translate( mPos );
            const float radius = mRadius * mDeathPer;
            gl::scale( Vec3f( radius, radius, radius ) );
            gl::rotate( mAxialRot );
            
			
		if( mIsHighlighted ){
			const float grey = mShadowPer + 0.2f;
            gl::color( ColorA( grey, grey, grey, mClosenessFadeAlpha ) );
			gl::enableAlphaBlending();
		} else {
            gl::color( ColorA( BLUE, mClosenessFadeAlpha ) );
			gl::enableAdditiveBlending();
		}
            if( mSphereScreenRadius > 60.0f ){
                mHiSphere->draw();
            } else if( mSphereScreenRadius > 30.0f  ){
                mMdSphere->draw();
            } else if( mSphereScreenRadius > 15.0f  ){
                mLoSphere->draw();
            } else {
                mTySphere->draw();
            }
//        }
        
        if (mHasCreatedAlbumArt) {
            mAlbumArtTex.disable();
        }

		glPopMatrix();
	}
	
}

void NodeTrack::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mSphereScreenRadius > 2.0f && mDistFromCamZAxisPer > 0.0f ){
		if( mIsMostPlayed ){

			glPushMatrix();
			gl::translate( mPos );
			clouds[mCloudTexIndex].enableAndBind();

			const float radius = mRadius * mDeathPer + mCloudLayerRadius;
			gl::scale( Vec3f( radius, radius, radius ) );
			
			gl::rotate( mAxialRot );
			const float alpha = max( 1.0f - mDistFromCamZAxisPer, 0.0f );
			

			if( mIsHighlighted ){
				const float grey = mShadowPer + 0.2f;
				gl::color( ColorA( grey, grey, grey, alpha * mClosenessFadeAlpha ) );
			} else {
				gl::color( ColorA( BLUE, alpha * mClosenessFadeAlpha ) );
			}
			
			gl::enableAdditiveBlending();
            if( mSphereScreenRadius > 60.0f ){
                mHiSphere->draw();
            } else if( mSphereScreenRadius > 30.0f  ){
                mMdSphere->draw();
            } else if( mSphereScreenRadius > 15.0f  ){
                mLoSphere->draw();
            } else {
                mTySphere->draw();
            }

            clouds[mCloudTexIndex].disable();
            
            glPopMatrix();
		}
	}
}

void NodeTrack::drawAtmosphere( const Vec3f &camEye, const Vec2f &center, const gl::Texture &tex, const gl::Texture &directionalTex, float pinchAlphaPer )
{
	if( mClosenessFadeAlpha > 0.0f ){

//		Vec2f dir		= mScreenPos - center;
//		float dirLength = dir.length()/500.0f;
		float alpha = mNormPlayCount * mDeathPer * mClosenessFadeAlpha;
		
//		float alpha = 0.3f * ( 1.0f - dirLength );
//		if( G_ZOOM <= G_ALBUM_LEVEL )
//			alpha = pinchAlphaPer;

		Vec2f radius( mRadius, mRadius );
		radius *= ( 2.435f + max( ( mSphereScreenRadius - 175.0f ) * 0.001f, 0.0f ) );
		
		
//		float grey = mShadowPer + 0.2f;
		gl::color( ColorA( BRIGHT_BLUE, alpha ) );
		tex.enableAndBind();
		bloom::gl::drawSphericalBillboard( camEye, mPos, radius, 0.0f );
		tex.disable();
		
		if( mIsHighlighted ){ // ONLY DRAW HIGHLIGHTED ATMOSPHERE IF NOT A GHOST MOON
			gl::color( ColorA( mShadowPer, mShadowPer, mShadowPer, alpha * mEclipseDirBasedAlpha * mDeathPer ) );
			directionalTex.enableAndBind();
			//bloom::gl::drawBillboard( mPos, radius, -mEclipseAngle, mBbRight, mBbUp );
			bloom::gl::drawSphericalRotatedBillboard( mPos, camEye, mParentNode->mParentNode->mPos, radius );        
			directionalTex.disable();
		}
	}
}



void NodeTrack::drawOrbitRing( float pinchAlphaPer, float camAlpha, const OrbitRing &orbitRing )
{	
	float newPinchAlphaPer = pinchAlphaPer;
	if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
		newPinchAlphaPer = pinchAlphaPer;
	} else {
		newPinchAlphaPer = 1.0f;
	}
	
	if( mIsHighlighted ){
		gl::color( ColorA( BLUE, camAlpha * mDeathPer ) );		
	} else {
		gl::color( ColorA( BLUE, camAlpha * mDeathPer * 0.3f ) );
	}
	
	glPushMatrix();
	gl::translate( mParentNode->mPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( Vec3f( 90.0f, 0.0f, toDegrees( mOrbitAngle ) ) );
    orbitRing.drawLowRes();
	glPopMatrix();
}

void NodeTrack::drawPlayheadProgress( float pinchAlphaPer, float camAlpha, float pauseAlpha, const gl::Texture &tex, const gl::Texture &originTex )
{
	if( mIsPlaying ){
		float newPinchAlphaPer = pinchAlphaPer;
		if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
			newPinchAlphaPer = pinchAlphaPer;
		} else {
			newPinchAlphaPer = 1.0f;
		}
		
		
		float alpha = pow( camAlpha, 0.25f ) * newPinchAlphaPer * pauseAlpha;
		
		tex.enableAndBind();
		glPushMatrix();
		gl::translate( mParentNode->mPos );
		gl::color( ColorA( mParentNode->mParentNode->mGlowColor, alpha ) );
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mOrbitVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mOrbitTexCoords );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, mTotalOrbitVertices );		
		tex.disable();
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glPopMatrix();
		
		Vec3f pos = Vec3f( cos( mOrbitStartAngle ), 0.0f, sin( mOrbitStartAngle ) );

		gl::enableAlphaBlending();

		originTex.enableAndBind();
		gl::drawBillboard( mParentNode->mPos + pos * mOrbitRadius, Vec2f( mRadius, mRadius ) * 2.15f, 0.0f, Vec3f::xAxis(), Vec3f::zAxis() );
		originTex.disable();
		
	//	gl::drawLine( pos * ( mOrbitRadius + mRadius * 1.2f ), pos * ( mOrbitRadius - mRadius * 1.2f ) );
		
				
	}
}

void NodeTrack::findShadows( float camAlpha )
{	
	if( mIsHighlighted ){
		Vec3f P0, P1, P2, P4;
		Vec3f P3a, P3b;
		Vec3f P5a, P5b, P6a, P6b;
		Vec3f outerTanADir, outerTanBDir, innerTanADir, innerTanBDir;
		
		float r0, r1, r0Inner, rTotal;
		float d, dMid, dMidSqrd;
		
		// Positions	
		P0		= mParentNode->mParentNode->mPos;
		P1		= mPos;
		P4		= ( P0 + P1 )*0.5f;
		
		// Radii
		r0				= mParentNode->mParentNode->mRadius * 0.175f;
		r1				= mRadius * 1.25f;
		rTotal			= r0 + r1;
		r0Inner			= abs( r0 - r1 );
		
		d				= P0.distance( P1 );
		dMid			= d * 0.5f;
		dMidSqrd		= dMid * dMid;
		
		float newRTotal		= r0Inner + dMid;
		float newRDelta		= abs( dMid - r0Inner );
		
		if( dMid > newRTotal ){
			// std::cout << "not intersecting" << std::endl;
		} else if( dMid < newRDelta ){
			// std::cout << "contained" << std::endl;
		} else if( dMid == 0 ){
			// std::cout << "concentric" << std::endl;
		} else {
			float a = ( dMidSqrd - r0Inner * r0Inner + dMidSqrd ) / d;
			P2 = P4 + a * ( ( P0 - P4 ) / dMid );
			
			float h = sqrt( dMidSqrd - a * a ) * 0.5f;
			
			Vec3f p = ( P1 - P0 )/dMid;
			
			P3a = P2 + h * Vec3f( -p.z, p.y, p.x );
			P3b = P2 - h * Vec3f( -p.z, p.y, p.x );
			
			
			Vec3f P3aDirNorm = P3a - P0;
			P3aDirNorm.normalize();
			
			Vec3f P3bDirNorm = P3b - P0;
			P3bDirNorm.normalize();
			
			P5a = P3a + P3aDirNorm * r1;
			P5b = P3b + P3bDirNorm * r1;
			P6a = P1 + P3aDirNorm * r1; 
			P6b = P1 + P3bDirNorm * r1;
			
			float amt = r0 * 3.0f;
			outerTanADir = ( P6a - P5a ) * amt;
			outerTanBDir = ( P6b - P5b ) * amt;
			innerTanADir = ( P6a - P5b ) * amt;
			innerTanBDir = ( P6b - P5a ) * amt;

			Vec3f P7a = P6a + outerTanBDir;
			Vec3f P7b = P6b + outerTanADir;
			
			float distOfShadow = ( 1.75f - r0 ) * 0.05f;
			P7a = P6a + ( P7a - P6a ).normalized() * distOfShadow;
			P7b = P6b + ( P7b - P6b ).normalized() * distOfShadow;
			
			glEnable( GL_TEXTURE_2D );
			buildShadowVertexArray( P6a, P6b, P7a, P7b );
			
			float alpha = camAlpha * mDeathPer * mShadowPer;//( 1.0f - dist*0.2f ) * camAlpha;
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f * alpha ) );
			
			glVertexPointer( 3, GL_FLOAT, 0, mShadowVerts );
			glTexCoordPointer( 2, GL_FLOAT, 0, mShadowTexCoords );
			
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );        
			glDrawArrays( GL_TRIANGLES, 0, 12 ); // dont forget to change the vert count in buildShadowVertexArray VVV
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			
		}
		
		/*
		if( G_DEBUG ){
			glDisable( GL_TEXTURE_2D );
			
			gl::enableAlphaBlending();
			gl::color( ColorA( mGlowColor, 0.4f ) );
			gl::drawLine( P0, P1 );
			
			glPushMatrix();
			gl::translate( P0 );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), r0, 50 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P0 );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), r0Inner, 50 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P1 );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), r1, 25 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P2 );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
			glPopMatrix();
			
			
			
			glPushMatrix();
			gl::translate( P3a );
			//gl::rotate( mMatrix );
			//gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P3b );
			//gl::rotate( mMatrix );
			//gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), 0.001f, 16 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P5a );
			gl::drawStrokedCircle( Vec2f::zero(), 0.001f, 16 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P5b );
			gl::drawStrokedCircle( Vec2f::zero(), 0.001f, 16 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P6a );
			gl::drawStrokedCircle( Vec2f::zero(), 0.001f, 16 );
			glPopMatrix();
			
			glPushMatrix();
			gl::translate( P6b );
			gl::drawStrokedCircle( Vec2f::zero(), 0.001f, 16 );
			glPopMatrix();
			
			
			gl::drawLine( P6a, ( P6a + mMatrix * outerTanBDir ) );
			gl::drawLine( P6b, ( P6b + mMatrix * outerTanBDir ) );
			gl::drawLine( P6a, ( P6a + mMatrix * innerTanBDir ) );
			gl::drawLine( P6b, ( P6b + mMatrix * innerTanBDir ) );
			
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.4f ) );	
			glPushMatrix();
			gl::translate( P4 );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
			gl::drawStrokedCircle( Vec2f::zero(), dMid, 50 );
			glPopMatrix();
			
			glEnable( GL_TEXTURE_2D );
		}
		*/
	}
}

void NodeTrack::buildShadowVertexArray( Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4 )
{
	if( mShadowVerts != NULL )		delete[] mShadowVerts;
	if( mShadowTexCoords != NULL )  delete[] mShadowTexCoords;
    
	int numVerts		= 12;			// dont forget to change the vert count in findShadows ^^^
	mShadowVerts		= new float[ numVerts * 3 ]; // x, y
	mShadowTexCoords	= new float[ numVerts * 2 ]; // u, v
	int i = 0;
	int t = 0;
	
	Vec3f v1 = ( p1 + p2 ) * 0.5f;	// midpoint between base vertices
	Vec3f v2 = ( p3 + p4 ) * 0.5f;	// midpoint between end vertices
	
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	mShadowVerts[i++]	= p3.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p3.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p3.z;
	
	// umbra 
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	// umbra 
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= p4.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p4.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p4.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;	
}


int NodeTrack::getTrackNumber()
{
	return ( mIndex + 1 );
}

string NodeTrack::getName()
{
	string name = mTrack->getTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeTrack::getId()
{
    return mId;
}
