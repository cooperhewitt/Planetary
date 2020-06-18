#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Camera.h"
#include "cinder/Matrix.h"

#include "PinchRecognizer.h"

using namespace ci;
using namespace ci::app;

class pinch_camera_sampleApp : public AppCocoaTouch {
public:
    void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();
    
    bool onPinchBegan(PinchEvent event);
    bool onPinchMoved(PinchEvent event);
    bool onPinchEnded(PinchEvent event);
    
    PinchRecognizer mPinchRecognizer;
    
    CameraPersp mCamera;
    Matrix44f   mMatrix;

    vector<Ray> mPinchRays;
};


void pinch_camera_sampleApp::prepareSettings(Settings *settings)
{
    settings->enableMultiTouch();
}

void pinch_camera_sampleApp::setup()
{
	mPinchRecognizer.init(this);
    mPinchRecognizer.registerBegan(this, &pinch_camera_sampleApp::onPinchBegan);
    mPinchRecognizer.registerMoved(this, &pinch_camera_sampleApp::onPinchMoved);
    mPinchRecognizer.registerEnded(this, &pinch_camera_sampleApp::onPinchEnded);

    mCamera.lookAt(Vec3f(2.5,2.5,5.0), Vec3f::zero(), Vec3f::yAxis());
    mCamera.setAspectRatio(getWindowAspectRatio());
    
    gl::enableDepthRead();
}

void pinch_camera_sampleApp::update()
{
}

void pinch_camera_sampleApp::draw()
{
    gl::clear(Color::black());
    
    gl::setMatrices(mCamera);
    
    gl::color(Color(1,0,0));
    for(vector<Ray>::iterator it = mPinchRays.begin(); it != mPinchRays.end(); ++it){
        gl::drawVector(it->getOrigin(), it->getOrigin() + it->getDirection(), 0.1f, 0.02f);
    }
    
    gl::multModelView(mMatrix);
    
    gl::drawCoordinateFrame();
    
    gl::color(Color(1,1,1));
    gl::drawStrokedCube(Vec3f(0,0,0), Vec3f(1,1,1));
}


bool pinch_camera_sampleApp::onPinchBegan(PinchEvent event)
{
    mPinchRays = event.getTouchRays(mCamera);
    return false;
}

bool pinch_camera_sampleApp::onPinchMoved(PinchEvent event)
{
    mPinchRays = event.getTouchRays(mCamera);
    mMatrix = event.getTransformDelta(mCamera, mCamera.getEyePoint().distance(Vec3f::zero())) * mMatrix;
    return false;
}

bool pinch_camera_sampleApp::onPinchEnded(PinchEvent event)
{
    mPinchRays.clear();
    return false;
}


CINDER_APP_COCOA_TOUCH( pinch_camera_sampleApp, RendererGl )
