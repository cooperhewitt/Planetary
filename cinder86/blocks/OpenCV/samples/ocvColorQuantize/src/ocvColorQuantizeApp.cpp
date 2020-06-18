#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"
#include "cinder/ip/Fill.h"

#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;

class ocvColorQuantizeApp : public AppNative {
  public:
	void setup();
	void draw();

	void updateImage();

	ci::Surface			mInputImage;
	gl::Texture			mTexture;
};

void ocvColorQuantizeApp::setup()
{
	// Image copyright Eric J Paparatto
	// http://www.flickr.com/photos/ejpphoto/2633923684/
	
	mInputImage = ci::Surface8u( loadImage( loadAsset( "nyc.jpg" ) ) );
	setWindowSize( mInputImage.getWidth(), mInputImage.getHeight() );

	updateImage();
}

void ocvColorQuantizeApp::updateImage()
{
	const int colorCount = 32;
	const int sampleCount = mInputImage.getHeight() * mInputImage.getWidth();
	cv::Mat colorSamples( sampleCount, 1, CV_32FC3 );

	// build our matrix of samples
	Surface::ConstIter imageIt = mInputImage.getIter();
	cv::MatIterator_<cv::Vec3f> sampleIt = colorSamples.begin<cv::Vec3f>();
	while( imageIt.line() )
		while( imageIt.pixel() )
			*sampleIt++ = cv::Vec3f( imageIt.r(), imageIt.g(), imageIt.b() );

	// call kmeans	
	cv::Mat labels, clusters;
	cv::kmeans( colorSamples, colorCount, labels, cv::TermCriteria( cv::TermCriteria::COUNT, 8, 0 ), 2, cv::KMEANS_RANDOM_CENTERS, clusters );

	Color8u clusterColors[colorCount];
	for( int i = 0; i < colorCount; ++i )
		clusterColors[i] = Color8u( clusters.at<cv::Vec3f>(i,0)[0], clusters.at<cv::Vec3f>(i,0)[1], clusters.at<cv::Vec3f>(i,0)[2] );

	Surface result( mInputImage.getWidth(), mInputImage.getHeight(), false );
	Surface::Iter resultIt = result.getIter();
	cv::MatIterator_<int> labelIt = labels.begin<int>();
	while( resultIt.line() ) {
		while( resultIt.pixel() ) {
			resultIt.r() = clusterColors[*labelIt].r;
			resultIt.g() = clusterColors[*labelIt].g;
			resultIt.b() = clusterColors[*labelIt].b;
			++labelIt;
		}
	}
	
	// draw the color palette across the bottom of the image
	const int swatchSize = 12;
	for( int i = 0; i < colorCount; ++i ) {
		ip::fill( &result, clusterColors[i], Area( i * swatchSize, result.getHeight() - swatchSize, ( i + 1 ) * swatchSize, result.getHeight() ) );
	}
	
	mTexture = gl::Texture( result );
}

void ocvColorQuantizeApp::draw()
{
	gl::clear();
	
	gl::color( Color( 1, 1, 1 ) );
	gl::draw( mTexture );	
}


CINDER_APP_NATIVE( ocvColorQuantizeApp, RendererGl )