#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"

#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;

class ocvWarpApp : public AppBasic {
  public:
	void setup();
	void draw();
	void resize( int width, int height );

	void updateImage();

	ci::Surface			mInputImage;
	gl::Texture			mTexture;

	params::InterfaceGl	mParams;
	Vec2f				mRotationCenter;
	float				mRotationAngle, mScale;
};

void ocvWarpApp::setup()
{		
	mInputImage = ci::Surface8u( loadImage( loadAsset( "aus.jpg" ) ) );

	mRotationCenter = mInputImage.getSize() * 0.5f;
	mRotationAngle = 31.2f;
	mScale = 0.77f;
	
	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 400 ) );
	mParams.addParam( "Rotation Center X", &mRotationCenter.x );
	mParams.addParam( "Rotation Center Y", &mRotationCenter.y );
	mParams.addParam( "Rotation Angle", &mRotationAngle );
	mParams.addParam( "Scale", &mScale, "step=0.01" );

	updateImage();
}

void ocvWarpApp::updateImage()
{
	cv::Mat input( toOcv( mInputImage ) );
	cv::Mat output;

	cv::Mat warpMatrix = cv::getRotationMatrix2D( toOcv( mRotationCenter ), mRotationAngle, mScale );
	cv::warpAffine( input, output, warpMatrix, toOcv( getWindowSize() ), cv::INTER_CUBIC );

	mTexture = gl::Texture( fromOcv( output ) );
}

void ocvWarpApp::resize( int width, int height )
{
	updateImage();
}

void ocvWarpApp::draw()
{
	gl::clear( Color( 1, 1, 1 ) );
	
	gl::color( Color( 1, 1, 1 ) );
	updateImage();
	gl::draw( mTexture );
	
	gl::color( Color( 1, 1, 0 ) );
	gl::drawSolidCircle( mRotationCenter, 4.0f );
	
	mParams.draw();
}


CINDER_APP_BASIC( ocvWarpApp, RendererGl )