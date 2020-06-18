#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/params/Params.h"

#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ocvOpticalFlowApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();

	void keyDown( KeyEvent event );
	void chooseFeatures( cv::Mat currentFrame );
	void trackFeatures( cv::Mat currentFrame );
	
	gl::Texture				mTexture;
	Capture					mCapture;
	cv::Mat					mPrevFrame;
	vector<cv::Point2f>		mPrevFeatures, mFeatures;
	vector<uint8_t>			mFeatureStatuses;
	bool					mDrawPoints;
	
	static const int MAX_FEATURES = 300;
};

void ocvOpticalFlowApp::setup()
{
	mDrawPoints = true;
	
	mCapture = Capture( 640, 480 );
	mCapture.start();
}

void ocvOpticalFlowApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'p' ) {
		mDrawPoints = ! mDrawPoints;
	}
	else if( event.getChar() == 'u' ) {
		chooseFeatures( mPrevFrame );
	}
}

void ocvOpticalFlowApp::chooseFeatures( cv::Mat currentFrame )
{
	cv::goodFeaturesToTrack( currentFrame, mFeatures, MAX_FEATURES, 0.005, 3.0 );
}

void ocvOpticalFlowApp::trackFeatures( cv::Mat currentFrame )
{
	vector<float> errors;
	mPrevFeatures = mFeatures;
	if( ! mFeatures.empty() )
		cv::calcOpticalFlowPyrLK( mPrevFrame, currentFrame, mPrevFeatures, mFeatures, mFeatureStatuses, errors );
}

void ocvOpticalFlowApp::update()
{
	if( mCapture.checkNewFrame() ) {
		Surface surface( mCapture.getSurface() );
		mTexture = gl::Texture( surface );
		cv::Mat currentFrame( toOcv( Channel( surface ) ) );
		if( mPrevFrame.data ) {
			if( mFeatures.empty() || getElapsedFrames() % 30 == 0 ) // pick new features once every 30 frames, or the first frame
				chooseFeatures( mPrevFrame );
			trackFeatures( currentFrame );
		}
		mPrevFrame = currentFrame;
	}
}

void ocvOpticalFlowApp::draw()
{
	if( ( ! mTexture ) || mPrevFeatures.empty() )
		return;

	gl::clear();
	gl::enableAlphaBlending();
	
	gl::setMatricesWindow( getWindowSize() );
	gl::color( 1, 1, 1 );
	gl::draw( mTexture );
	
	glDisable( GL_TEXTURE_2D );
	glColor4f( 1, 1, 0, 0.5f );
	
	if( mDrawPoints ) {
		// draw all the old points
		for( vector<cv::Point2f>::const_iterator featureIt = mPrevFeatures.begin(); featureIt != mPrevFeatures.end(); ++featureIt )
			gl::drawStrokedCircle( fromOcv( *featureIt ), 4 );

		// draw all the new points
		for( vector<cv::Point2f>::const_iterator featureIt = mFeatures.begin(); featureIt != mFeatures.end(); ++featureIt )
			gl::drawSolidCircle( fromOcv( *featureIt ), 4 );
	}
	
	// draw the lines connecting them
#if ! defined( CINDER_COCOA_TOUCH )
	glColor4f( 0, 1, 0, 0.5f );
	glBegin( GL_LINES );
	for( size_t idx = 0; idx < mFeatures.size(); ++idx ) {
		if( mFeatureStatuses[idx] ) {
			gl::vertex( fromOcv( mFeatures[idx] ) );
			gl::vertex( fromOcv( mPrevFeatures[idx] ) );
		}
	}
	glEnd();
#endif
}


CINDER_APP_NATIVE( ocvOpticalFlowApp, RendererGl )
