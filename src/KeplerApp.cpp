#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/Arcball.h"
#include "cinder/ImageIo.h"
#include "Globals.h"
#include "Easing.h"
#include "World.h"
#include "UiLayer.h"
#include "State.h"
#include "Data.h"
#include "Breadcrumbs.h"
#include "BreadcrumbEvent.h"
#include <vector>

using std::vector;
using namespace ci;
using namespace ci::app;
using namespace std;

int G_CURRENT_LEVEL	= 0;

float easeInOutQuad( double t, float b, float c, double d );
Vec3f easeInOutQuad( double t, Vec3f b, Vec3f c, double d );

class KeplerApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );
	void			initFonts();
	virtual void	update();
	void			updateArcball();
	void			updateCamera();
	virtual void	draw();
	bool			onAlphaCharStateChanged( State *state );
	bool			onAlphaCharSelected( UiLayer *uiLayer );
	bool			onBreadcrumbSelected ( BreadcrumbEvent event );
	bool			onNodeSelected( Node *node );
	void			checkForNodeTouch( const Ray &ray, Matrix44f &mat );
	World			mWorld;
	State			mState;
	UiLayer			mUiLayer;
	Data			mData;
	
	CallbackMgr<bool(Node*)> mCallbacksNodeSelected;
	
	template<typename T>
	CallbackId registerNodeSelected( T *obj, bool (T::*callback)(Node*) ){
		return mCallbacksNodeSelected.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}
	
	
	// BREADCRUMBS
	Breadcrumbs		mBreadcrumbs;
	
	
	// CAMERA PERSP
	CameraPersp		mCam;
	float			mFov, mFovDest;
	Vec3f			mEye, mCenter, mUp;
	Vec3f			mCamVel;
	Vec3f			mCenterDest, mCenterFrom;
	float			mCamDist, mCamDistDest, mCamDistFrom, mCamDistDestMulti;
	float			mZoomFrom, mZoomDest;
	Arcball			mArcball;
	Matrix44f		mMatrix;
	Vec3f			mBbRight, mBbUp;
	
	
	// FONTS
	vector<Font>	mFonts;
	
	
	// MULTITOUCH
	Vec2f			mTouchPos;
	Vec2f			mTouchThrowVel;
	Vec2f			mTouchVel;
	bool			mIsDragging;
	
	
	// NODES
	Node			*mSelectedNode;
	Node			*mPrevSelectedNode;
	std::map<u_int8_t, Node*> mMapOfNodes;
	
	
	// TEXTURES
	gl::Texture		mStarTex;
	gl::Texture		mStarGlowTex;
	
	float			mTime;
};

void KeplerApp::setup()
{
	// ARCBALL
	mMatrix	= Quatf();
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 300 );
	
	
	// CAMERA PERSP
	mCamDist			= 180.0f;
	mCamDistDest		= mCamDist;
	mCamDistFrom		= mCamDist;
	mCamDistDestMulti	= 1.0f;
	mEye				= Vec3f( 0.0f, 0.0f, mCamDist );
	mCenter				= Vec3f::zero();
	mCenterDest			= mCenter;
	mCenterFrom			= mCenter;
	mUp					= Vec3f::yAxis();
	mFov				= 90.0f;
	mFovDest			= 90.0f;
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 4000.0f );
	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
	
	
	// FONTS
	initFonts();
	
	
	// TOUCH VARS
	mTouchPos			= Vec2f::zero();
	mTouchThrowVel		= Vec2f::zero();
	mTouchVel			= Vec2f::zero();
	mIsDragging			= false;
	mTime				= getElapsedSeconds();
	
	
	// TEXTURES
	mStarTex			= gl::Texture( loadImage( loadResource( "star.png" ) ) );
	mStarGlowTex		= gl::Texture( loadImage( loadResource( "starGlow.png" ) ) );
	
	
	// BREADCRUMBS
	mBreadcrumbs.setup( this, mFonts[4] );
	mBreadcrumbs.registerBreadcrumbSelected( this, &KeplerApp::onBreadcrumbSelected );
	vector<string> testHierarchy;
	testHierarchy.push_back("Home");
	mBreadcrumbs.setHierarchy(testHierarchy);
	
	registerNodeSelected( this, &KeplerApp::onNodeSelected );
	
	
	// STATE
	mState.registerAlphaCharStateChanged( this, &KeplerApp::onAlphaCharStateChanged );
	
	
	// UILAYER
	mUiLayer.setup( this );
	mUiLayer.registerAlphaCharSelected( this, &KeplerApp::onAlphaCharSelected );
	mUiLayer.initAlphaTextures( mFonts[0] );
	
	
	// DATA
	mData.initArtists();
	
	
	// WORLD
	mWorld.setData( &mData );
	mWorld.initNodes( mFonts[3] );
}

void KeplerApp::touchesBegan( TouchEvent event )
{	
	mIsDragging = false;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) 
	{
		mTouchPos		= touchIt->getPos();
		mTouchThrowVel	= Vec2f::zero();
		mIsDragging		= false;
		if( event.getTouches().size() == 1 )
			mArcball.mouseDown( Vec2f( mTouchPos.x, mTouchPos.y ) );
	}
}

void KeplerApp::touchesMoved( TouchEvent event )
{
	mIsDragging = true;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	{
		mTouchThrowVel	= touchIt->getPos() - mTouchPos;
		mTouchVel		= mTouchThrowVel;
		mTouchPos		= touchIt->getPos();
		if( event.getTouches().size() == 1 )
			mArcball.mouseDrag( Vec2f( mTouchPos.x, mTouchPos.y ) );
	}
}

void KeplerApp::touchesEnded( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	{
		mTouchPos = touchIt->getPos();
		if( ! mUiLayer.getShowWheel() && ! mIsDragging ){
			float u			= mTouchPos.x / (float) getWindowWidth();
			float v			= mTouchPos.y / (float) getWindowHeight();
			Ray touchRay	= mCam.generateRay( u, 1.0f - v, mCam.getAspectRatio() );
			checkForNodeTouch( touchRay, mMatrix );
		}
		mIsDragging = false;
	}
}

void KeplerApp::initFonts()
{
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 256 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 64 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 48 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 32 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 24 ) );
}

bool KeplerApp::onAlphaCharSelected( UiLayer *uiLayer )
{
	mState.setAlphaChar( uiLayer->getAlphaChar() );
	return false;
}

bool KeplerApp::onAlphaCharStateChanged( State *state )
{
	char filterChar = state->getAlphaChar();
	mData.filterArtistsByAlpha( filterChar );
	mUiLayer.setAlphaChar( filterChar );
	mWorld.filterNodes();
	
	string s;
	s += filterChar;
	
	vector<string> hierarchy;
	hierarchy.push_back( s );
	hierarchy.push_back( "Home" );
	reverse( hierarchy.begin(), hierarchy.end() );

	mBreadcrumbs.setHierarchy( hierarchy );
	
	return false;
}

bool KeplerApp::onNodeSelected( Node *node )
{
	mTime			= getElapsedSeconds();
	mCenterFrom		= mCenter;
	mCamDistFrom	= mCamDist;
				
	int gen = node->mGen;

	if( node->mIsSelected ){						// If the touched node is already selected...
		mSelectedNode = node->mParentNode;
		node->deselect();								// deselect it
		
	} else {										// If the touched node is not already selected...
		//node->mIsSelected = true;						// select it and make it create some children.
		node->select();
	}
	
	if( mMapOfNodes[gen] ){							// If there was already a touched node at this level...
		mMapOfNodes[gen]->deselect();					// deselect it
		for( int i=gen; i<G_NUM_LEVELS; i++ )
			mMapOfNodes.erase(i);						// and erase it from the map.
	}
	
	if( node->mIsSelected ){						// If the touched node is now selected...
		mMapOfNodes[gen]	= node;						// add the touched node to the map.
		mSelectedNode		= node;
	}
	
	if( mSelectedNode )
		G_CURRENT_LEVEL		= mSelectedNode->mGen;
	else
		G_CURRENT_LEVEL		= 0;
		
		
			
	vector<string> hierarchy;
	if( node != NULL ){
		Node *parent = node;
		while( parent != NULL ){
			hierarchy.push_back( parent->mName );
			parent = parent->mParentNode;
		}
		
		string s;
		s += mState.getAlphaChar();
		hierarchy.push_back( s );
		hierarchy.push_back("Home");
		reverse( hierarchy.begin(), hierarchy.end() );
	}
	
	mBreadcrumbs.setHierarchy( hierarchy );
	
	return false;
}

bool KeplerApp::onBreadcrumbSelected( BreadcrumbEvent event )
{
	vector<string> hierarchy;
	int level = event.getLevel();
	if( level == 0 ){					// BACK TO HOME
		mWorld.deselectAllNodes();
		
		for( int i=0; i<G_NUM_LEVELS; i++ ){
			if( mMapOfNodes[i] ){
				mMapOfNodes.erase(i);
			}
			
		}
		hierarchy.push_back("Home");
		
	} else if( level == 1 ){			// BACK TO ALPHA FILTER
		for( int i=G_NUM_LEVELS; i>=0; i-- ){
			if( mMapOfNodes[i] ){
				mMapOfNodes[i]->deselect();
			}
		}
		
		string s;
		s += mState.getAlphaChar();
		hierarchy.push_back( s );
		hierarchy.push_back("Home");
		reverse( hierarchy.begin(), hierarchy.end() );
	} else {
	
	}
	mBreadcrumbs.setHierarchy( hierarchy );
	
	return false;
}

void KeplerApp::checkForNodeTouch( const Ray &ray, Matrix44f &mat )
{
	Node *touchedNode = NULL;
	mWorld.checkForSphereIntersect( touchedNode, ray, mat );
	
	if( touchedNode ) mCallbacksNodeSelected.call( touchedNode );
}

void KeplerApp::update()
{
	updateArcball();
	updateCamera();
	mWorld.update( mMatrix, mBbRight, mBbUp );
	mBreadcrumbs.update();
}

void KeplerApp::updateArcball()
{
	if( mTouchThrowVel.length() > 10.0f && !mIsDragging ){
		if( mTouchVel.length() > 1.0f ){
			mTouchVel *= 0.99f;
			mArcball.mouseDown( mTouchPos );
			mArcball.mouseDrag( mTouchPos + mTouchVel );
		}
	}
	
	mMatrix = mArcball.getQuat();
}


void KeplerApp::updateCamera()
{
	if( mSelectedNode ){
		float radiusMulti = 15.0f;
		
		mCamDistDest	= ( mSelectedNode->mRadius * radiusMulti  );
		mCenterDest		= mMatrix.transformPointAffine( mSelectedNode->mPos );
		mZoomDest		= mSelectedNode->mGen;
		
		if( mSelectedNode->mParentNode )
			mCenterFrom		+= mSelectedNode->mParentNode->mVel;

	}
	
	
	
	// UPDATE FOV
	if( mUiLayer.getShowWheel() ){
		mFovDest = 130.0f;
	} else {
		mFovDest = 90.0f;
	}
	mFov -= ( mFov - mFovDest ) * 0.2f;
	
	double p	= constrain( getElapsedSeconds()-mTime, 0.0, G_DURATION );
	mCenter		= easeInOutQuad( p, mCenterFrom, mCenterDest - mCenterFrom, G_DURATION );
	mCamDist	= easeInOutQuad( p, mCamDistFrom, mCamDistDest*mCamDistDestMulti - mCamDistFrom, G_DURATION );
	
	Vec3f prevEye		= mEye;
	mEye				= Vec3f( mCenter.x, mCenter.y, mCenter.z - mCamDist );
	mCamVel				= mEye - prevEye;
	
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.01f, 4000.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	mCam.getBillboardVectors( &mBbRight, &mBbUp );
}

void KeplerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	gl::enableDepthWrite();
	gl::setMatrices( mCam );
	
	gl::enableAdditiveBlending();
	
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlows();
	mStarGlowTex.disable();
	
	mStarTex.enableAndBind();
	mWorld.drawStars();
	mStarTex.disable();
	
	mWorld.drawNames();
	glDisable( GL_TEXTURE_2D );
	
	gl::disableAlphaBlending();
	
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	gl::setMatricesWindow( getWindowSize() );
	mUiLayer.draw();
	mBreadcrumbs.draw();
}


CINDER_APP_COCOA_TOUCH( KeplerApp, RendererGl )
