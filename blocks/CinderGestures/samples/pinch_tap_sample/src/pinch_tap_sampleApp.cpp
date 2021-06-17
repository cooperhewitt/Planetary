#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

#include "TouchRect.h"

using namespace ci;
using namespace ci::app;

class pinch_tap_sampleApp : public AppCocoaTouch {
public:
    void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();
    
    vector<TouchRect*>        mRects;
    vector<TouchEvent::Touch> mPinchTouches;
    
    gl::Texture mRectTex;
};

void pinch_tap_sampleApp::prepareSettings(Settings *settings)
{
    settings->enableMultiTouch();
}

void pinch_tap_sampleApp::setup()
{
    gl::Texture::Format format;
    format.setMinFilter(GL_NEAREST);
    format.setMagFilter(GL_NEAREST);
    mRectTex = gl::Texture(loadImage(loadResource("xy.png")), format);
    
    Rand::randomize();
    
    for(int i = 3; --i >= 0;){
        float scale = Rand::randFloat(150, 200);
        mRects.push_back(new TouchRect(this,
            Rectf(-scale, -scale, scale, scale),
            Vec3f(Rand::randFloat(100, getWindowWidth() - 100), Rand::randFloat(100, getWindowWidth() - 100), 0),
            Rand::randFloat(M_PI * 2)
        ));
    }
}

void pinch_tap_sampleApp::update()
{
}

void pinch_tap_sampleApp::draw()
{
    gl::clear(Color::black());
    
    gl::setMatricesWindowPersp(getWindowSize(), 60, 1, 2000);
    
    mRectTex.enableAndBind();
    for(vector<TouchRect*>::reverse_iterator it = mRects.rbegin(); it != mRects.rend(); ++it)
        (*it)->draw();
    mRectTex.unbind();
}


CINDER_APP_COCOA_TOUCH( pinch_tap_sampleApp, RendererGl )
