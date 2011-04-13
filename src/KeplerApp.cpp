#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/Arcball.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "Globals.h"
#include "Easing.h"
#include "World.h"
#include "HelpLayer.h"
#include "UiLayer.h"
#include "AlphaWheel.h"
#include "State.h"
#include "Data.h"
#include "PlayControls.h"
#include "Breadcrumbs.h"
#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include "PinchRecognizer.h"
#include "ParticleController.h"
#include "LoadingScreen.h"
#include "NodeArtist.h"

#include "CinderFlurry.h"
//#include "TextureLoader.h"
#include <sys/sysctl.h>
#include <vector>
#include <sstream>
#import <CoreMotion/CoreMotion.h>


using std::vector;
using namespace ci;
using namespace ci::app;
using namespace std;
using std::stringstream;
using namespace pollen::flurry;

float G_ZOOM			= 0;
bool G_ACCEL			= true;
bool G_DEBUG			= false;
bool G_HELP             = false;
bool G_DRAW_RINGS		= false;
bool G_DRAW_TEXT		= false;
bool G_IS_IPAD2			= false;
int G_NUM_PARTICLES		= 250;
int G_NUM_DUSTS			= 250;

GLfloat mat_diffuse[]	= { 1.0, 1.0, 1.0, 1.0 };

float easeInOutQuad( double t, float b, float c, double d );
Vec3f easeInOutQuad( double t, Vec3f b, Vec3f c, double d );



class KeplerApp : public AppCocoaTouch {
  public:
    
	virtual void	setup();
    void            prepareSettings(Settings *settings);
    void            remainingSetup();
	void			initLoadingTextures();
	void			initTextures();
	void			initSphereVertexArray( int segments, int *numVerts, float* &sphereVerts, float* &sphereTexCoords, float* &sphereNormals );
	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );
	virtual void	accelerated( AccelEvent event );
	virtual void	orientationChanged( OrientationEvent event );
    void            setInterfaceOrientation( const Orientation &orientation );
	virtual void	update();
	void			updateArcball();
	void			updateCamera();
	void			updatePlayhead();
	void			updateCameraPop();
	virtual void	draw();
    void            drawScene();
	void			drawInfoPanel();
	void			setParamsTex();
	void			toggleAlphaWheel( bool b, bool resetSelection );
	bool			onAlphaCharStateChanged( State *state );
	bool			onAlphaCharSelected( AlphaWheel *alphaWheel );
	bool			onWheelClosed( AlphaWheel *alphaWheel );
	bool			onBreadcrumbSelected ( BreadcrumbEvent event );
	bool			onHelpLayerButtonPressed( HelpLayer *helpLayer );
	bool			onPlayControlsButtonPressed ( PlayControls::PlayButton button );
	bool			onPlayControlsPlayheadMoved ( float amount );
	bool			onSelectedNodeChanged( Node *node );
	void			checkForNodeTouch( const Ray &ray, Matrix44f &mat, const Vec2f &pos );
	bool			onPlayerStateChanged( ipod::Player *player );
    bool			onPlayerTrackChanged( ipod::Player *player );
    bool			onPlayerLibraryChanged( ipod::Player *player );
    void            updateIsPlaying();
    Node*           getPlayingTrackNode( ipod::TrackRef playingTrack, Node* albumNode );
    Node*           getPlayingAlbumNode( ipod::TrackRef playingTrack, Node* artistNode );
    Node*           getPlayingArtistNode( ipod::TrackRef playingTrack );
	
	//NodeTrack*		mCurrentTrackNode;
	
    LoadingScreen   mLoadingScreen;
	World			mWorld;
	State			mState;
	AlphaWheel		mAlphaWheel;
	UiLayer			mUiLayer;
	HelpLayer		mHelpLayer;
	Data			mData;

// ORIENTATION
    Orientation     mInterfaceOrientation;
    Matrix44f       mOrientationMatrix;
    Matrix44f       mInverseOrientationMatrix;    
    std::map<Orientation, std::map<Orientation, int> > mRotationSteps;
    
// ACCELEROMETER
	Matrix44f		mAccelMatrix;
	Matrix44f		mNewAccelMatrix;
	
// GYRO
	CMMotionManager		*mMotionManager;
    NSOperationQueue	*mGyroQueue;
	Vec3f				mGyroData;
	Matrix44f			mGyroMat;
	
// AUDIO
	ipod::Player		mIpodPlayer;
	ipod::PlaylistRef	mCurrentAlbum;
	double				mCurrentTrackPlayheadTime;
	double				mCurrentTrackLength;
	float				mElapsedSecondsSinceTrackChange;
	
// BREADCRUMBS
	Breadcrumbs		mBreadcrumbs;	
	
// PLAY CONTROLS
	PlayControls	mPlayControls;
	float			mPlayheadPer;	// song percent complete 
	
// CAMERA PERSP
	CameraPersp		mCam;
	float			mFov, mFovDest;
	Vec3f			mEye, mCenter, mUp;
	Vec3f			mCamVel;
	Vec3f			mCenterDest, mCenterFrom;
	Vec3f			mCamNormal;
	float			mCamDist, mCamDistDest, mCamDistFrom;
	float			mCamDistPinchOffset, mCamDistPinchOffsetDest;
	float			mZoomFrom, mZoomDest;
	Arcball			mArcball;
	Matrix44f		mMatrix;
	Vec3f			mBbRight, mBbUp;
	float			mCamRingAlpha; // 1.0 = camera is viewing rings side-on
								   // 0.0 = camera is viewing rings from above or below
	
// FONTS
	Font			mFont;
	Font			mFontBig;
	Font			mFontMediSmall;
	Font			mFontMediTiny;
	
// MULTITOUCH
	Vec2f			mTouchPos;
	Vec2f			mTouchVel;
	bool			mIsDragging;
    bool            mIsTouching; // set true in touchesBegan if there's one touch and it's in-world
	bool			onPinchBegan( PinchEvent event );
    bool			onPinchMoved( PinchEvent event );
    bool			onPinchEnded( PinchEvent event );
    bool            keepTouchForPinching( TouchEvent::Touch touch );
    bool            positionTouchesWorld( Vec2f pos );
    vector<Ray>		mPinchRays;
    PinchRecognizer mPinchRecognizer;
	float			mPinchScale;
	float			mTimePinchEnded;
	float			mPinchAlphaPer;
	
// PARTICLES
    ParticleController mParticleController;
	
	// TEXTURES
//    TextureLoader   mTextureLoader;
	gl::Texture		mParamsTex;
	gl::Texture		mStarTex, mStarGlowTex, mEclipseGlowTex;
	gl::Texture		mSkyDome;//, mMilkyWayDome;
	gl::Texture		mDottedTex;
	gl::Texture		mPlayheadProgressTex;
    gl::Texture     mRingsTex;
	gl::Texture		mUiButtonsTex;
	gl::Texture		mCurrentTrackTex;
    gl::Texture		mAtmosphereTex;
	vector<gl::Texture> mPlanetsTex;
	vector<gl::Texture> mCloudsTex;
	
	float			mTime;
	
	bool			mDataIsLoaded;
    bool            mRemainingSetupCalled; // setup() is short and fast, remainingSetup() is slow
};

void KeplerApp::prepareSettings(Settings *settings)
{
    Flurry::getInstrumentation()->init("DZ7HPD6FE1GGADVNJ3EX");
}

void KeplerApp::setup()
{
    std::cout << "setupStart: " << getElapsedSeconds() << std::endl;
    
    // http://stackoverflow.com/questions/448162/determine-device-iphone-ipod-touch-with-iphone-sdk/1561920#1561920
    // http://www.clintharris.net/2009/iphone-model-via-sysctlbyname/
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);  
    char *machine = new char[size];
    sysctlbyname("hw.machine", machine, &size, NULL, 0);    
    G_IS_IPAD2 = (strcmp("iPad1,1",machine) != 0);
    cout << "G_IS_IPAD2: " << G_IS_IPAD2 << endl;
    delete[] machine;
	
	/*
	if( G_IS_IPAD2 ){
		G_NUM_PARTICLES = 1000;
		G_NUM_DUSTS = 4000;
	}
    */
	
	
    setInterfaceOrientation( getInterfaceOrientation() );
    
    mRemainingSetupCalled = false;

    std::map<Orientation,int> pSteps;
    pSteps[LANDSCAPE_LEFT_ORIENTATION] = 1;
    pSteps[LANDSCAPE_RIGHT_ORIENTATION] = -1;
    pSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = 2;
    std::map<Orientation,int> llSteps;
    llSteps[PORTRAIT_ORIENTATION] = -1;
    llSteps[LANDSCAPE_RIGHT_ORIENTATION] = 2;
    llSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = 1;
    std::map<Orientation,int> lrSteps;
    lrSteps[PORTRAIT_ORIENTATION] = 1;
    lrSteps[LANDSCAPE_LEFT_ORIENTATION] = 2;
    lrSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = -1;
    std::map<Orientation,int> upSteps;
    upSteps[PORTRAIT_ORIENTATION] = 2;
    upSteps[LANDSCAPE_LEFT_ORIENTATION] = -1;
    upSteps[LANDSCAPE_RIGHT_ORIENTATION] = 1;
    mRotationSteps[PORTRAIT_ORIENTATION] = pSteps;
    mRotationSteps[LANDSCAPE_LEFT_ORIENTATION] = llSteps;
    mRotationSteps[LANDSCAPE_RIGHT_ORIENTATION] = lrSteps;
    mRotationSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = upSteps;
    
    mLoadingScreen.setup( this );

    initLoadingTextures();
}

void KeplerApp::remainingSetup()
{
    if (mRemainingSetupCalled) return;
    
    mRemainingSetupCalled = true;

    mDataIsLoaded	= false;
	G_DRAW_RINGS	= true;
	G_DRAW_TEXT		= true;
	//Rand::randomize();
    
	mElapsedSecondsSinceTrackChange = 0.0f;
	
	
    // TEXTURES
    initTextures();
	    
	// INIT ACCELEROMETER
	enableAccelerometer();
	mAccelMatrix		= Matrix44f();
	
	
	/*
	Vec3f v1, v2, v3;
	// INIT GYRO
	mGyroData			= Vec3f::zero();
	// TODO: Release on shutdown
    mGyroQueue			= [[NSOperationQueue alloc] init];
    // TODO: Release on shutdown
    mMotionManager		= [[CMMotionManager alloc] init];
	CMDeviceMotion *deviceMotion	= mMotionManager.deviceMotion;      
	CMAttitude *attitude			= deviceMotion.attitude;
	CMRotationMatrix rm				= attitude.rotationMatrix;
	
	v1 = Vec3f( rm.m11, rm.m12, rm.m13 );
	v2 = Vec3f( rm.m21, rm.m22, rm.m23 );
	v3 = Vec3f( rm.m31, rm.m32, rm.m33 );
	
    [mMotionManager startGyroUpdatesToQueue:mGyroQueue withHandler:^(CMGyroData *gyroData, NSError *error) {
		*attitude			= deviceMotion.attitude;
		mGyroMat	= Matrix44f( v1, v2, v3 );
		console() << v1 << std::endl;
	 }];
	
	*/
	
	
	// ARCBALL
	mMatrix	= Quatf();
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( G_DEFAULT_ARCBALL_RADIUS );
	
	// CAMERA PERSP
	mCamDist			= G_INIT_CAM_DIST;
	mCamDistDest		= mCamDist;
	mCamDistPinchOffset	= 1.0f;
	mCamDistPinchOffsetDest = 1.0f;
	mCamDistFrom		= mCamDist;
	mEye				= Vec3f( 0.0f, 0.0f, mCamDist );
	mCenter				= Vec3f::zero();
	mCamNormal			= Vec3f::zero();
	mCenterDest			= mCenter;
	mCenterFrom			= mCenter;
    // FIXME: let's put this setup stuff back in setup()
    // this was overriding the (correct) value which is now always set by setInterfaceOrientation
//	mUp					= Vec3f::yAxis();
	mFov				= G_DEFAULT_FOV;
	mFovDest			= G_DEFAULT_FOV;
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.0001f, 1200.0f );
	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
	
	// FONTS
	mFont				= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 14 );
	mFontBig			= Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 256 );
	mFontMediSmall		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 13 );
	mFontMediTiny		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 12 );
	
	// TOUCH VARS
	mTouchPos			= Vec2f::zero();
	mTouchVel			= Vec2f::zero();
	mIsDragging			= false;
    mIsTouching         = false;
	mTime				= getElapsedSeconds();
	mTimePinchEnded		= 0.0f;
	mPinchRecognizer.init(this);
    mPinchRecognizer.registerBegan( this, &KeplerApp::onPinchBegan );
    mPinchRecognizer.registerMoved( this, &KeplerApp::onPinchMoved );
    mPinchRecognizer.registerEnded( this, &KeplerApp::onPinchEnded );
    mPinchRecognizer.setKeepTouchCallback( this, &KeplerApp::keepTouchForPinching );
	mPinchAlphaPer		= 1.0f;
    
    // PARTICLES
    mParticleController.addParticles( G_NUM_PARTICLES );
	mParticleController.addDusts( G_NUM_DUSTS );
	
    // NB:- order of UI init is important to register callbacks in correct order
    
	// BREADCRUMBS
	mBreadcrumbs.setup( this, mFontMediSmall );
	mBreadcrumbs.registerBreadcrumbSelected( this, &KeplerApp::onBreadcrumbSelected );
	mBreadcrumbs.setHierarchy(mState.getHierarchy());

	// PLAY CONTROLS
	mPlayControls.setup( this, mIpodPlayer.getPlayState() == ipod::Player::StatePlaying );
	mPlayControls.registerButtonPressed( this, &KeplerApp::onPlayControlsButtonPressed );
	mPlayControls.registerPlayheadMoved( this, &KeplerApp::onPlayControlsPlayheadMoved );

	// UILAYER
	mUiLayer.setup( this );

	// HELP LAYER
	mHelpLayer.setup( this );
	mHelpLayer.registerHelpButtonPressed( this, &KeplerApp::onHelpLayerButtonPressed );
	
    // ALPHA WHEEL
	mAlphaWheel.setup( this );
	mAlphaWheel.registerAlphaCharSelected( this, &KeplerApp::onAlphaCharSelected );
	mAlphaWheel.registerWheelClosed( this, &KeplerApp::onWheelClosed );
	mAlphaWheel.initAlphaTextures( mFontBig );	
	
	// STATE
	mState.registerAlphaCharStateChanged( this, &KeplerApp::onAlphaCharStateChanged );
	mState.registerNodeSelected( this, &KeplerApp::onSelectedNodeChanged );
		
	// PLAYER
	mIpodPlayer.registerStateChanged( this, &KeplerApp::onPlayerStateChanged );
    mIpodPlayer.registerTrackChanged( this, &KeplerApp::onPlayerTrackChanged );
    mIpodPlayer.registerLibraryChanged( this, &KeplerApp::onPlayerLibraryChanged );
	
    // DATA
    mData.setup(); // NB:- is asynchronous, see update() for what happens when it's done

    // WORLD
    mWorld.setup( &mData );  
	
    std::cout << "setupEnd: " << getElapsedSeconds() << std::endl;
}

void KeplerApp::initLoadingTextures()
{
    float t = getElapsedSeconds();
    std::cout << "initLoadingTextures, begin: " << t << std::endl;
    // only add textures here if they are *required* for LoadingScreen
    // otherwise add them to initTextures
	
	mStarGlowTex	= loadImage( loadResource( "starGlow.png" ) );
	
//    mTextureLoader.requestTexture( "star.png",     mStarTex );
//    mTextureLoader.requestTexture( "starGlow.png", mStarGlowTex );
    std::cout << "initLoadingTextures, duration: " << getElapsedSeconds() - t << std::endl;
}

void KeplerApp::initTextures()
{
	float t = getElapsedSeconds();
	cout << "initTextures start time = " << t << endl;
    mStarTex			= loadImage( loadResource( "star.png" ) );
	mEclipseGlowTex		= loadImage( loadResource( "eclipseGlow.png" ) );
	mSkyDome			= loadImage( loadResource( "skydome.jpg" ) );
	//mMilkyWayDome		= loadImage( loadResource( "skydome.png" ) );
	mDottedTex			= loadImage( loadResource( "dotted.png" ) );
	mDottedTex.setWrap( GL_REPEAT, GL_REPEAT );
	mRingsTex           = loadImage( loadResource( "rings.png" ) );
    mPlayheadProgressTex = loadImage( loadResource( "playheadProgress.png" ) );
	mPlayheadProgressTex.setWrap( GL_REPEAT, GL_REPEAT );
	mParamsTex			= gl::Texture( 768, 75 );    
	mUiButtonsTex		= loadImage( loadResource( "uiButtons.png" ) );
    mAtmosphereTex		= loadImage( loadResource( "atmosphere.png" ) );
    mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "11.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "12.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "13.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "21.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "22.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "23.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "31.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "32.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "33.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "41.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "42.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "43.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "51.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "52.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "53.jpg" ) ) ) );
    
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds1.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds2.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds3.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds4.png" ) ) ) );
	//mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds5.png" ) ) ) );
    
	cout << "initTextures duration = " << (getElapsedSeconds()-t) << endl;
}

void KeplerApp::touchesBegan( TouchEvent event )
{	
    if (!mRemainingSetupCalled) return;
	mIsDragging = false;
	const vector<TouchEvent::Touch> touches = getActiveTouches();
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f && keepTouchForPinching(*touches.begin()) ) {
        mIsTouching = true;
        mTouchPos		= touches.begin()->getPos();
        mTouchVel		= Vec2f::zero();
        Vec3f worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
        mArcball.mouseDown( Vec2i(worldTouchPos.x, worldTouchPos.y) );
	}
    else {
        mIsTouching = false;
    }
}

void KeplerApp::touchesMoved( TouchEvent event )
{
    if (!mRemainingSetupCalled) return;

    if ( mIsTouching ) {
        float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;	
        const vector<TouchEvent::Touch> touches = getActiveTouches();
        if( touches.size() == 1 && timeSincePinchEnded > 0.2f ){
            Vec2f currentPos = touches.begin()->getPos();
            Vec2f prevPos = touches.begin()->getPrevPos();        
            if (positionTouchesWorld(currentPos) && positionTouchesWorld(prevPos)) {
                mIsDragging = true;
                mTouchVel		= currentPos - prevPos;
                mTouchPos		= currentPos;
                Vec3f worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
                mArcball.mouseDrag( Vec2i( worldTouchPos.x, worldTouchPos.y ) );
            }
        }
    }
}

void KeplerApp::touchesEnded( TouchEvent event )
{
    if (!mRemainingSetupCalled) return;    
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;	
	const vector<TouchEvent::Touch> touches = event.getTouches();
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f ){        
        Vec2f currentPos = touches.begin()->getPos();
        Vec2f prevPos = touches.begin()->getPrevPos();   
        if (positionTouchesWorld(currentPos) && positionTouchesWorld(prevPos)) {
            mTouchPos = currentPos;
            // if the nav wheel isnt showing and you havent been dragging
			// and your touch is above the uiLayer panel and the Help panel isnt showing
            if( ! mAlphaWheel.getShowWheel() && ! mIsDragging && !G_HELP ){
                float u			= mTouchPos.x / (float) getWindowWidth();
                float v			= mTouchPos.y / (float) getWindowHeight();
                Ray touchRay	= mCam.generateRay( u, 1.0f - v, mCam.getAspectRatio() );
                checkForNodeTouch( touchRay, mMatrix, mTouchPos );
            } else {
                Flurry::getInstrumentation()->logEvent("Camera Moved");
            }
        }
	}
	if (getActiveTouches().size() != 1) {
		mIsDragging = false;
        mIsTouching = false;
	} else {
        // TODO: camera moved log is a bit suspect, better to log at mArcball.mouseDrag if mIsDragging, to be sure?
        Flurry::getInstrumentation()->logEvent("Camera Moved");
    }
}

bool KeplerApp::onPinchBegan( PinchEvent event )
{
	mPinchScale = 1.0f;
    mPinchRays = event.getTouchRays( mCam );
	
	mTouchVel	= Vec2f::zero();
	vector<PinchEvent::Touch> touches = event.getTouches();
	Vec2f averageTouchPos;
	for( vector<PinchEvent::Touch>::iterator it = touches.begin(); it != touches.end(); ++it ){
		averageTouchPos += it->mPos;
	}
	averageTouchPos /= touches.size();
	mTouchPos = averageTouchPos;
	
    return false;
}

bool KeplerApp::onPinchMoved( PinchEvent event )
{	
    mPinchRays = event.getTouchRays( mCam );
	
	Node *selectedNode = mState.getSelectedNode();
	int currentLevel = 0;
	if( selectedNode ){
		currentLevel = selectedNode->mGen;
	}
	
	if( currentLevel <= G_ALPHA_LEVEL ){
		mFovDest += ( 1.0f - event.getScaleDelta() ) * 150.0f;
		
	} else {
		mCamDistPinchOffsetDest *= ( event.getScaleDelta() - 1.0f ) * -3.5f + 1.0f;
		mCamDistPinchOffsetDest = constrain( mCamDistPinchOffsetDest, 0.35f, 4.5f );
		
		if( mCamDistPinchOffsetDest > 4.1f ){
			mFovDest = 115.0f;//( 1.0f - event.getScaleDelta() ) * 20.0f;
		} else {
			mFovDest = G_DEFAULT_FOV;
		}
	}
	
	vector<PinchEvent::Touch> touches = event.getTouches();
	Vec2f averageTouchPos;
	for( vector<PinchEvent::Touch>::iterator it = touches.begin(); it != touches.end(); ++it ){
		averageTouchPos += it->mPos;
	}
	averageTouchPos /= touches.size();
	
	//mTouchThrowVel	= ( averageTouchPos - mTouchPos );
	//mTouchVel		= mTouchThrowVel;
	mTouchPos		= averageTouchPos;
	
	mPinchScale		= event.getScale();
	//cout << "pinch scale: " << mPinchScale << endl;
    return false;
}

bool KeplerApp::onPinchEnded( PinchEvent event )
{
    Flurry::getInstrumentation()->logEvent("Pinch");
	//std::cout << "mCamDistPinchOffset = " << mCamDistPinchOffset << std::endl;

	if( mCamDistPinchOffsetDest > 4.1f ){
		Node *selected = mState.getSelectedNode();
		if( selected ){
            console() << "backing out using pinch!" << std::endl;
			mState.setSelectedNode( selected->mParentNode );
			mFovDest = G_DEFAULT_FOV;
		}
		mCamDistPinchOffsetDest = 1.0f;
	}
	mTimePinchEnded = getElapsedSeconds();
	mAlphaWheel.setTimePinchEnded( mTimePinchEnded );	
    mPinchRays.clear();
    return false;
}

bool KeplerApp::keepTouchForPinching( TouchEvent::Touch touch )
{
    return positionTouchesWorld(touch.getPos());
}

bool KeplerApp::positionTouchesWorld( Vec2f screenPos )
{
    Vec2f worldPos = (mInverseOrientationMatrix * Vec3f(screenPos,0)).xy();
    bool aboveUI = worldPos.y < mUiLayer.getPanelYPos();
    bool belowBreadcrumbs = worldPos.y > mBreadcrumbs.getHeight();
    bool notTab = !mUiLayer.getPanelTabRect().contains(worldPos);
    bool valid = aboveUI && belowBreadcrumbs && notTab;
//    cout << "worldPos: " << worldPos << " aboveUI: " << aboveUI  
//                                     << " belowBreadcrumbs: " << belowBreadcrumbs
//                                     << " notTab: " << notTab << endl;
//    cout << "screenPos: " << screenPos << " valid: " << valid << endl;
    return valid;
}

void KeplerApp::accelerated( AccelEvent event )
{
	mNewAccelMatrix = event.getMatrix();
	mNewAccelMatrix.invert();
}

void KeplerApp::orientationChanged( OrientationEvent event )
{
    //console() << event << std::endl;
    
    setInterfaceOrientation(event.getInterfaceOrientation());

    // Look over there!
    // heinous trickery follows...
    Orientation prevOrientation = event.getPrevInterfaceOrientation();
    if (mInterfaceOrientation != prevOrientation) {
        if( mTouchVel.length() > 2.0f && !mIsDragging ){        
            int steps = mRotationSteps[prevOrientation][mInterfaceOrientation];
            mTouchVel.rotate( (float)steps * M_PI/2.0f );
        }
    }
    // ... end heinous trickery
}

void KeplerApp::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44<float>( mInterfaceOrientation );
    mInverseOrientationMatrix = mOrientationMatrix.inverted();
    mUp = getUpVectorForOrientation<float>( mInterfaceOrientation );    
}

bool KeplerApp::onWheelClosed( AlphaWheel *alphaWheel )
{
//	std::cout << "wheel closed" << std::endl;
	mFovDest = G_DEFAULT_FOV;
	return false;
}

bool KeplerApp::onAlphaCharSelected( AlphaWheel *alphaWheel )
{
	mState.setAlphaChar( alphaWheel->getAlphaChar() );
	return false;
}

bool KeplerApp::onAlphaCharStateChanged( State *state )
{
	mData.filterArtistsByAlpha( mState.getAlphaChar() );
	
	std::cout << "Letter " << mState.getAlphaChar() << " has " << mData.mFilteredArtists.size() << " artists" << std::endl;
	
	mWorld.filterNodes();
	mBreadcrumbs.setHierarchy( mState.getHierarchy() );	
    
    mState.setSelectedNode( NULL );
    
	return false;
}

bool KeplerApp::onSelectedNodeChanged( Node *node )
{
	mTime			= getElapsedSeconds();
	mCenterFrom		= mCenter;
	mCamDistFrom	= mCamDist;	
	mZoomFrom		= G_ZOOM;			
	mBreadcrumbs.setHierarchy( mState.getHierarchy() );
    	
	if( node && node->mGen > G_ZOOM ){
		node->wasTapped();
	}

	if( node != NULL ) {
        if (node->mGen == G_TRACK_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Track Selected");
            // FIXME: is this a bad OOP thing or is there a cleaner/safer C++ way to handle it?
            NodeTrack* trackNode = (NodeTrack*)node;
            if ( mIpodPlayer.hasPlayingTrack() ){
                ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();
                if ( trackNode->getId() != playingTrack->getItemId() ) {
                    mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );
                }
            }
            else {
                mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );			
            }
        } 
        else if (node->mGen == G_HOME_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Home Selected");
        } 
        else if (node->mGen == G_ALPHA_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Alpha Selected");
        } 
        else if (node->mGen == G_ARTIST_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Artist Selected");
        } 
        else if (node->mGen == G_ALBUM_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Album Selected");
        }
	}

    updateIsPlaying();

	return false;
}

bool KeplerApp::onPlayControlsPlayheadMoved( float dragPer )
{
	if ( mIpodPlayer.hasPlayingTrack() ) {
        ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();
        double trackLength = playingTrack->getLength();
        if( getElapsedFrames() % 3 == 0 ){
            // FIXME: make the playhead update to finger position when touched
            // rather than making the playhead TIME update to finger position and then lagging to update the playhead position
            // subtle but important :)
            mIpodPlayer.setPlayheadTime( trackLength * dragPer );
        }
    }
    return false;
}

bool KeplerApp::onPlayControlsButtonPressed( PlayControls::PlayButton button )
{
    Flurry::getInstrumentation()->logEvent("Player controlls Selected");
    
    switch( button ) {
        
        case PlayControls::PREVIOUS_TRACK:
            mIpodPlayer.skipPrev();
            break;
        
        case PlayControls::PLAY_PAUSE:
            if (mIpodPlayer.getPlayState() == ipod::Player::StatePlaying) {
                mIpodPlayer.pause();
            }
            else {
                mIpodPlayer.play();
            }
            break;
        
        case PlayControls::NEXT_TRACK:
            mIpodPlayer.skipNext();	
            break;
        
        case PlayControls::HELP:
            G_HELP = !G_HELP;
            break;
        
        case PlayControls::DRAW_RINGS:
            G_DRAW_RINGS = !G_DRAW_RINGS;
            break;
        
        case PlayControls::DRAW_TEXT:
            G_DRAW_TEXT = !G_DRAW_TEXT;
            break;
        
        case PlayControls::CURRENT_TRACK:
            // pretend the track just got changed again, this will select it:
            onPlayerTrackChanged( &mIpodPlayer );
            break;

        case PlayControls::SHOW_WHEEL:
            toggleAlphaWheel( !mAlphaWheel.getShowWheel(), false ); // just toggle, dont reset selections
            break;
            
        default:
            console() << "unknown button pressed!" << std::endl;
            break;
            
	} // switch

	return false;
}

bool KeplerApp::onHelpLayerButtonPressed( HelpLayer *helpLayer )
{
	G_HELP = !G_HELP;
	
	return false;
}


void KeplerApp::toggleAlphaWheel( bool b, bool resetSelection )
{
	mAlphaWheel.setShowWheel( b );

	if( mAlphaWheel.getShowWheel() ) mFovDest = G_MAX_FOV;
	else							 mFovDest = G_DEFAULT_FOV;
	
	mCamDistPinchOffsetDest = 1.0f;
		
	if( resetSelection ){
		mWorld.deselectAllNodes();
		mState.setSelectedNode( NULL );
		mState.setAlphaChar( ' ' );
	}
}


bool KeplerApp::onBreadcrumbSelected( BreadcrumbEvent event )
{
    Flurry::getInstrumentation()->logEvent("Breadcrumb Selected");
	int level = event.getLevel();
	if( level == G_HOME_LEVEL ){				// BACK TO HOME
		toggleAlphaWheel( true, true );
	}
	else if( level == G_ALPHA_LEVEL ){			// BACK TO ALPHA FILTER
		mWorld.deselectAllNodes();
		mData.filterArtistsByAlpha( mState.getAlphaChar() );
		mWorld.filterNodes();
		mState.setSelectedNode(NULL);
		mCamDistPinchOffsetDest = 1.0f;
	}
	else if( level >= G_ARTIST_LEVEL ){			// BACK TO ARTIST/ALBUM/TRACK
		// get Artist, Album or Track from selectedNode
		Node *current = mState.getSelectedNode();
		while (current != NULL && current->mGen > level) {
			current = current->mParentNode;
		}
		mState.setSelectedNode(current);
	}
	return false;
}

void KeplerApp::checkForNodeTouch( const Ray &ray, Matrix44f &mat, const Vec2f &pos )
{
	vector<Node*> nodes;
	//mWorld.checkForSphereIntersect( nodes, ray, mat );
	mWorld.checkForNameTouch( nodes, pos );

	if( nodes.size() > 0 ){
		Node *nodeWithHighestGen = *nodes.begin();
		int highestGen = nodeWithHighestGen->mGen;
		
		vector<Node*>::iterator it;
		for( it = nodes.begin(); it != nodes.end(); ++it ){
			if( (*it)->mGen > highestGen ){
				highestGen = (*it)->mGen;
				nodeWithHighestGen = *it;
			}
		}
		
		
		if( nodeWithHighestGen ){
			if( highestGen == G_ARTIST_LEVEL ){
				if( ! mState.getSelectedArtistNode() ) {
                    console() << "setting artist node selection" << std::endl;                                        
					mState.setSelectedNode( nodeWithHighestGen );
                }
                else {
                    console() << "ignoring artist node selection" << std::endl;                                        
                }
			} 
            else if ( highestGen == G_TRACK_LEVEL ){
				if( nodeWithHighestGen != mState.getSelectedNode() ) {
                    mState.setSelectedNode( nodeWithHighestGen );
                }
                else {
                    // if this is already the selected node, just toggle the play state
                    if (mIpodPlayer.getPlayState() == ipod::Player::StatePlaying) {
                        mIpodPlayer.pause();
                    }
                    else if (mIpodPlayer.getPlayState() == ipod::Player::StatePaused) {
                        mIpodPlayer.play();
                    }                    
                }
                
            }
            else {
                console() << "setting node selection" << std::endl;
                mState.setSelectedNode( nodeWithHighestGen );
			}

		}
        else {
            // TODO: shouldn't be possible... confirm!
        }        
	}
    else {
        console() << "no nodes hit by touches" << std::endl;
    }
}

void KeplerApp::update()
{
	if( mData.update() ){
		mWorld.initNodes( &mIpodPlayer, mFont );
		mDataIsLoaded = true;
        // clear all the breadcrumbs etc.
        onSelectedNodeChanged( NULL );
        // and then make sure we know about the current track if there is one
        if ( mIpodPlayer.getPlayState() == ipod::Player::StatePlaying ) {
            onPlayerTrackChanged( &mIpodPlayer );
        }
	}
    //mTextureLoader.update();
    
    if ( mRemainingSetupCalled )
	{
        mAccelMatrix			= lerp( mAccelMatrix, mNewAccelMatrix, 0.35f );
		
		updateArcball();
		
        mWorld.update( mMatrix );
        mParticleController.update();
        
        updateCamera();
        mWorld.updateGraphics( mCam, mBbRight, mBbUp );

		Matrix44f inverseMatrix = mMatrix.inverted();
        Vec3f invBbRight		= inverseMatrix * mBbRight;
        Vec3f invBbUp			= inverseMatrix * mBbUp;        
        
        if( mDataIsLoaded ){
            mWorld.buildStarsVertexArray( invBbRight, invBbUp );
            mWorld.buildStarGlowsVertexArray( invBbRight, invBbUp );
        }
		mParticleController.update();
		mParticleController.buildParticleVertexArray( invBbRight, invBbUp );
		if( mState.getSelectedArtistNode() ){
			mParticleController.buildDustVertexArray( mState.getSelectedArtistNode(), mPinchAlphaPer, mCamRingAlpha );
		}
        
        mUiLayer.update();
		if( G_HELP ) mHelpLayer.update();
		mAlphaWheel.update( mFov );
        mBreadcrumbs.update();
        mPlayControls.update();
        
        updatePlayhead();
    }
    else {
        // make sure we've drawn the loading screen first
        if (getElapsedFrames() > 1) {
            remainingSetup();
        }        
    }
}

void KeplerApp::updateArcball()
{	
	if( mTouchVel.length() > 2.0f && !mIsDragging ){
        Vec3f downPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
        mArcball.mouseDown( Vec2i(downPos.x, downPos.y) );        
        Vec3f dragPos = mInverseOrientationMatrix * Vec3f(mTouchPos + mTouchVel,0);
        mArcball.mouseDrag( Vec2i(dragPos.x, dragPos.y) );        
	}
	
	//if( G_ACCEL ){
	//	mArcball.setQuat( mArcball.getQuat() + mGyroMat );
		//mMatrix = mAccelMatrix * mArcball.getQuat();
	//	mMatrix = mArcball.getQuat();
	//} else {
		mMatrix = mArcball.getQuat();
	//}
}


void KeplerApp::updateCamera()
{
	double p		= constrain( getElapsedSeconds()-mTime, 0.0, G_DURATION );
	
	mCamDistPinchOffset -= ( mCamDistPinchOffset - mCamDistPinchOffsetDest ) * 0.4f;
	
	updateCameraPop();
	
	int currentLevel = 0;
	Node* selectedNode = mState.getSelectedNode();
	if( selectedNode ){
		currentLevel	= selectedNode->mGen;
		mCamDistDest	= selectedNode->mIdealCameraDist * mCamDistPinchOffset;
		mCenterDest		= selectedNode->mTransPos; //mMatrix.transformPointAffine( selectedNode->mPos );
		mZoomDest		= selectedNode->mGen;
		
		mCenterFrom		+= selectedNode->mTransVel;
		
	} else {
		mCamDistDest	= G_INIT_CAM_DIST * mCamDistPinchOffset;
		mCenterDest		= mMatrix.transformPointAffine( Vec3f::zero() );

		mZoomDest		= G_HOME_LEVEL;
		if( mState.getAlphaChar() != ' ' ){
			mZoomDest	= G_ALPHA_LEVEL;
		}
	}
	
	// UPDATE FOV
	mFovDest = constrain( mFovDest, G_MIN_FOV, G_MAX_FOV );
	mFov -= ( mFov - mFovDest ) * 0.15f;
	
	
	

	if( mFovDest >= G_MAX_FOV - 10 && ! mAlphaWheel.getShowWheel() && currentLevel <= G_ALPHA_LEVEL ){
		toggleAlphaWheel( true, false );

	} else if( mFovDest < G_MAX_FOV - 10 && mAlphaWheel.getShowWheel() ){
		toggleAlphaWheel( false, false );
	}

	mCenter			= easeInOutQuad( p, mCenterFrom, mCenterDest - mCenterFrom, G_DURATION );
	mCamDist		= easeInOutQuad( p, mCamDistFrom, mCamDistDest - mCamDistFrom, G_DURATION );
	mCamDist		= min( mCamDist, G_INIT_CAM_DIST );
	G_ZOOM			= easeInOutQuad( p, mZoomFrom, mZoomDest - mZoomFrom, G_DURATION );
	
	Vec3f prevEye	= mEye;
	mEye			= Vec3f( mCenter.x, mCenter.y, mCenter.z - mCamDist );
	mCamVel			= mEye - prevEye;

	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 2000.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	mCam.getBillboardVectors( &mBbRight, &mBbUp );
	mCamNormal = mEye - mCenter;
	mCamNormal.normalize();
	
	
	
	Quatf cameraViewDirection = mCam.getOrientation();
	Vec3f quatAxis = mMatrix.inverted() * cameraViewDirection.getAxis();
	float quatZ		= abs( quatAxis.z );
	mCamRingAlpha	= pow( quatZ, 3.0f ) * 0.025f;
}

void KeplerApp::updatePlayhead()
{
    // TODO: only call this once a second?
	//if( mIpodPlayer.getPlayState() == ipod::Player::StatePlaying ){
		mCurrentTrackPlayheadTime	= mIpodPlayer.getPlayheadTime();
        // TODO: cache this when playing track changes
		mCurrentTrackLength			= mIpodPlayer.getPlayingTrack()->getLength();
	//}
}

void KeplerApp::updateCameraPop()
{
	if( mCamDistPinchOffsetDest > 4.1f ){
		mPinchAlphaPer -= ( mPinchAlphaPer ) * 0.2f;
	} else {
		mPinchAlphaPer -= ( mPinchAlphaPer - 1.0f ) * 0.2f;
	}
}

void KeplerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	if( !mDataIsLoaded ){
		mLoadingScreen.draw( mStarGlowTex );
	} else {
		drawScene();
	}
}

void KeplerApp::drawScene()
{		
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
	
//	float uiAlpha = constrain( 1.0f - mAlphaWheel.getWheelScale(), 0.0f, 1.0f );
        
// SKYDOME
    gl::pushModelView();
    gl::rotate( mMatrix );
    gl::color( Color::white() );
    mSkyDome.enableAndBind();
    gl::drawSphere( Vec3f::zero(), G_SKYDOME_RADIUS, 24 );
	/*
	gl::enableAlphaBlending();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, uiAlpha ) );
	mMilkyWayDome.enableAndBind();
    gl::drawSphere( Vec3f::zero(), G_SKYDOME_RADIUS - 50.0f, 24 );
	*/
    gl::popModelView();
    
    gl::enableAdditiveBlending();
    
    
// STARS
	mStarTex.enableAndBind();
	mWorld.drawStarsVertexArray( mMatrix );
	mStarTex.disable();
    

// ECLIPSEGLOWS OVERLAY
	mEclipseGlowTex.enableAndBind();
	mWorld.drawEclipseGlows();
	mEclipseGlowTex.disable();
	
// STARGLOWS occluded
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlowsVertexArray( mMatrix );
	mStarGlowTex.disable();
 	
	
	Node *artistNode = mState.getSelectedArtistNode();
	if( artistNode ){
		gl::enableDepthRead();
		gl::disableAlphaBlending();
		glEnable( GL_LIGHTING );
		//glEnable( GL_COLOR_MATERIAL );
		glShadeModel( GL_SMOOTH );
		
		glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );				
		
		// LIGHT FROM ARTIST
		glEnable( GL_LIGHT0 );
		Vec3f lightPos          = artistNode->mTransPos;
		GLfloat artistLight[]	= { lightPos.x, lightPos.y, lightPos.z, 1.0f };
		glLightfv( GL_LIGHT0, GL_POSITION, artistLight );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, ColorA( ( artistNode->mGlowColor + Color::white() ) * 0.5f, 1.0f ) );

		
// PLANETS
		mWorld.drawPlanets( mPlanetsTex );
		
// CLOUDS
		mWorld.drawClouds( mPlanetsTex, mCloudsTex );
	}
	
	glDisable( GL_LIGHTING );
	gl::enableAdditiveBlending();  	
	gl::disableDepthRead();
	
// STARGLOWS bloom
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlowsVertexArray( mMatrix );
	mWorld.drawTouchHighlights();
	mStarGlowTex.disable();

    
    
    gl::disableDepthWrite();
    gl::enableAdditiveBlending();
	gl::enableDepthRead();

	
// ORBITS
	if( G_DRAW_RINGS ){
        mWorld.drawOrbitRings( mPinchAlphaPer );
	}
	
// PARTICLES
	if( mState.getSelectedArtistNode() ){
		mStarGlowTex.enableAndBind();
		mParticleController.drawParticleVertexArray( mState.getSelectedArtistNode(), mMatrix );
		mStarGlowTex.disable();
	}
	
// STAR CENTER
	NodeArtist* selectedArtist = (NodeArtist*)artistNode;
	if( selectedArtist ){
		selectedArtist->drawStarCenter( mStarTex );
	}
	
// RINGS
	mWorld.drawRings( mRingsTex, mCamRingAlpha );
	
// DUSTS
	if( mState.getSelectedAlbumNode() ){
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		mParticleController.drawDustVertexArray( mState.getSelectedAlbumNode(), mMatrix );
	}
	

		
// CURRENT TRACK ORBIT PATH
	if( mWorld.mPlayingTrackNode && G_ZOOM > G_ARTIST_LEVEL ){
		gl::enableAdditiveBlending();
		mWorld.mPlayingTrackNode->updateAudioData( mCurrentTrackPlayheadTime );
		if( G_DRAW_RINGS )
			mWorld.mPlayingTrackNode->drawPlayheadProgress( mPinchAlphaPer, mPlayheadProgressTex );
	}
	
	
// CONSTELLATION
	if( mData.mFilteredArtists.size() > 1 ){
		gl::enableAdditiveBlending();
		mDottedTex.enableAndBind();
		mWorld.drawConstellation( mMatrix );
		mDottedTex.disable();
	}
	
	gl::disableDepthRead();
	// ATMOSPHERE
	Node *albumNode = mState.getSelectedAlbumNode();
	if( albumNode ){
		albumNode->drawAtmosphere( mCamNormal, mAtmosphereTex );
	}
	

	
	gl::disableDepthWrite();
	glEnable( GL_TEXTURE_2D );
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAdditiveBlending();
	
	
// NAMES
	if( G_DRAW_TEXT ){
        
        // TODO: write a function for getAngleForOrientation<float>( mInterfaceOrientation );
        float nameAngle = 0;
        if (mInterfaceOrientation == UPSIDE_DOWN_PORTRAIT_ORIENTATION) {
            nameAngle = M_PI;
        }
        else if (mInterfaceOrientation == LANDSCAPE_LEFT_ORIENTATION) {
            nameAngle = M_PI/2.0;
        }
        else if (mInterfaceOrientation == LANDSCAPE_RIGHT_ORIENTATION) {
            nameAngle = -M_PI/2;
        }        
        
		mWorld.drawNames( mCam, mPinchAlphaPer, nameAngle );
	}
    
    glDisable( GL_TEXTURE_2D );

//    for (int i = 0; i < mWorld.mNodes.size(); i++) {
//        Node* artistNode = mWorld.mNodes[i];
//        if (artistNode->mIsHighlighted) {
//            gl::color(ColorA(0.0f,0.0f,1.0f,0.25f));
//            gl::drawSolidRect(artistNode->mHitArea);
//            gl::drawSolidRect(artistNode->mSphereHitArea);            
//            for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
//                Node* albumNode = artistNode->mChildNodes[j];
//                if (albumNode->mIsHighlighted) {
//                    gl::color(ColorA(0.0f,1.0f,0.0f,0.25f));
//                    gl::drawSolidRect(albumNode->mHitArea);
//                    gl::drawSolidRect(albumNode->mSphereHitArea);            
//                    for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
//                        Node *trackNode = albumNode->mChildNodes[k];
//                        if (trackNode->mIsHighlighted) {
//                            gl::color(ColorA(1.0f,0.0f,0.0f,0.25f));
//                            gl::drawSolidRect(trackNode->mHitArea);
//                            gl::drawSolidRect(trackNode->mSphereHitArea);
//                        }
//                    }            
//                }
//            }
//        }
//    }
    
    gl::disableAlphaBlending();
    gl::enableAlphaBlending();
	
// EVERYTHING ELSE
	mAlphaWheel.draw( mData.mWheelDataVerts, mData.mWheelDataTexCoords, mData.mWheelDataColors );
    mUiLayer.draw( mUiButtonsTex );
    mBreadcrumbs.draw( mUiButtonsTex );
    mPlayControls.draw( mUiButtonsTex, mCurrentTrackTex, &mAlphaWheel, mFontMediTiny, mUiLayer.getPanelYPos(), mCurrentTrackPlayheadTime, mCurrentTrackLength, mElapsedSecondsSinceTrackChange );
	
	if( G_HELP ) mHelpLayer.draw( mUiButtonsTex );
	
	gl::disableAlphaBlending();
//    if( G_DEBUG ) drawInfoPanel();
}

void KeplerApp::drawInfoPanel()
{
	gl::setMatricesWindow( getWindowSize() );
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
	if( getElapsedFrames() % 30 == 0 ){
		setParamsTex();
	}
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	gl::draw( mParamsTex, Vec2f( 23.0f, 25.0f ) );
    gl::popModelView();
}


void KeplerApp::setParamsTex()
{
    stringstream s;
	TextLayout layout;	
	layout.setFont( mFontMediSmall );
	layout.setColor( Color( 1.0f, 0.0f, 0.0f ) );

	s.str("");
	s << " FPS: " << getAverageFps();
	layout.addLine( s.str() );
	
	int currentLevel = G_HOME_LEVEL;
	if (mState.getSelectedNode()) {
		currentLevel = mState.getSelectedNode()->mGen;
	}
	else if (mState.getAlphaChar() != ' ') {
		currentLevel = G_ALPHA_LEVEL;
	}
	
	mParamsTex = gl::Texture( layout.render( true, false ) );
}

bool KeplerApp::onPlayerLibraryChanged( ipod::Player *player )
{	
	console() << "/////////////////////" << std::endl;
	console() << "onPlayerLibraryChanged!" << std::endl;

    Flurry::getInstrumentation()->logEvent("Player Library Changed");

    mDataIsLoaded = false;    

    mState.setup();    
    mData.setup();
    
    return false;
}

bool KeplerApp::onPlayerTrackChanged( ipod::Player *player )
{	
    // TODO: does Flurry care about this?
    
    mElapsedSecondsSinceTrackChange = getElapsedSeconds();
    
	console() << "==================================================================" << std::endl;
	console() << "onPlayerTrackChanged!" << std::endl;

	if (mIpodPlayer.hasPlayingTrack()) {
        
		ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();

	// Create current track text texture
		TextLayout layout;
		layout.setFont( mFontMediSmall );
		layout.setColor( Color::white() );			
		layout.addLine( " " + playingTrack->getArtist() + " • " + playingTrack->getAlbumTitle() + " • " + playingTrack->getTitle() + " " );
		bool PREMULT = false;
		mCurrentTrackTex = gl::Texture( layout.render( true, PREMULT ) );
		 
        
        Node *selectedNode = mState.getSelectedNode();
        if (!(selectedNode != NULL && selectedNode->getId() == playingTrack->getItemId())) {
        
            Node* artistNode = getPlayingArtistNode( playingTrack );
            if (artistNode != NULL) {

                // ensure that breadcrumbs are consistent
                mState.setAlphaChar( artistNode->getName() );

                artistNode->select();

                Node* albumNode = getPlayingAlbumNode( playingTrack, artistNode );
                if (albumNode != NULL) {

                    albumNode->select();

                    // TODO: only select the track automatically if we're already at track level
//                    if ( selectedNode && selectedNode->mGen == G_TRACK_LEVEL ) {
                        // TODO: let's not do this if the current playing album and artist don't match
                        //       the transition is too jarring/annoying
                        //       better to use this opportunity to update info about the currently playing track
                        Node* trackNode = getPlayingTrackNode( playingTrack, albumNode );
                        if (trackNode != NULL) {

                            if (!trackNode->mIsSelected) {
                                console() << "    selecting track node" << std::endl;
                                // this one gets selected in World
                                mState.setSelectedNode(trackNode);
                            }
                            else {
                                console() << "    track node already selected" << std::endl;                            
                            }
                        }
                        else {
                            // TODO: log this in Flurry, with track details and current state details
                            console() << "  track changed to a track we don't have a track node for - this should never happen" << std::endl;
                        }
                    
//                    }
//                    else {
//                        console() << "    not selecting track node because it's too whooshy" << std::endl;                                                    
//                        // make sure updateIsPlaying is correct though...
//                        updateIsPlaying();
//                    }
                    
                }
                else {
                    // TODO: log this in Flurry, with track details and current state details
                    console() << "  track changed to a track we don't have an album node for - this should never happen" << std::endl;
                }                
            }
            else {
                // TODO: log this in Flurry, with track details and current state details
                console() << "  track changed to a track we don't have an artist node for - this should never happen" << std::endl;
            }
        }
        else {
            console() << "    track changed but we've already selected the node for that track" << std::endl;
        }
        
	}
	else {
		console() << "    trackchanged but nothing's playing" << endl;
	}

    updatePlayhead();
    
    console() << "onPlayerTrackChanged done in " << (getElapsedSeconds() - mElapsedSecondsSinceTrackChange) << " seconds" << std::endl;
	console() << "==================================================================" << std::endl;

    return false;
}

bool KeplerApp::onPlayerStateChanged( ipod::Player *player )
{	
    // TODO: does Flurry care about this?
    if ( mState.getSelectedNode() == NULL ) {
        onPlayerTrackChanged( player );
    }
    mPlayControls.setPlaying(player->getPlayState() == ipod::Player::StatePlaying);
    updateIsPlaying();    
    return false;
}

void KeplerApp::updateIsPlaying()
{
    // update mIsPlaying state for all nodes...
    if ( mIpodPlayer.hasPlayingTrack() ){
        ipod::TrackRef track = mIpodPlayer.getPlayingTrack();
        mWorld.setIsPlaying( track->getArtistId(), track->getAlbumId(), track->getItemId() );
    }
    else {
        // this should be OK to do since the above will happen if something is queued and paused
        mWorld.setIsPlaying( 0, 0, 0 );
    }
}


Node* KeplerApp::getPlayingTrackNode( ipod::TrackRef playingTrack, Node* albumNode )
{
    if (albumNode != NULL) {
        for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
            // FIXME: what's the proper C++ way to do this cast?
            NodeTrack *trackNode = (NodeTrack*)(albumNode->mChildNodes[k]);
            if (trackNode->getId() == playingTrack->getItemId()) {
                return trackNode;
            }
        }
    }
    return NULL;
}

Node* KeplerApp::getPlayingAlbumNode( ipod::TrackRef playingTrack, Node* artistNode )
{
    if (artistNode != NULL) {
        uint64_t albumId = playingTrack->getAlbumId();
        for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
            Node* albumNode = artistNode->mChildNodes[j];
            if (albumNode->getId() == albumId) {
                return albumNode;
            }
        }
    }
    return NULL;
}

Node* KeplerApp::getPlayingArtistNode( ipod::TrackRef playingTrack )
{
    uint64_t artistId = playingTrack->getArtistId();    
    for (int i = 0; i < mWorld.mNodes.size(); i++) {
        Node* artistNode = mWorld.mNodes[i];
        if (artistNode->getId() == artistId) {
            return artistNode;
        }
    }    
    return NULL;
}

CINDER_APP_COCOA_TOUCH( KeplerApp, RendererGl )
