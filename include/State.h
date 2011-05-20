/*
 *  State.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/Font.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "Globals.h"
#include "Node.h"
#include "NodeTrack.h"

using namespace ci;

class State {
 public:
	State();
    
    void setup();
	void draw( const ci::Font &font);
	
	// Alpha char for filtering artist name
	char getAlphaChar(){ return mAlphaChar; }
	void setAlphaChar( char c );
	void setAlphaChar( const string &artistName );
	template<typename T>
	CallbackId registerAlphaCharStateChanged( T *obj, bool ( T::*callback )( State* ) ){
		return mCallbacksAlphaCharStateChanged.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	
	// Playlist filtering
	ci::ipod::PlaylistRef getPlaylist(){ return mCurrentPlaylist; }
	void setPlaylist( ci::ipod::PlaylistRef playlist );
	string getPlaylistName(){ return mCurrentPlaylistName; }
	template<typename T>
	CallbackId registerPlaylistStateChanged( T *obj, bool ( T::*callback )( State* ) ){
		return mCallbacksPlaylistStateChanged.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	

	Node* getSelectedNode() { return mSelectedNode; }
	void setSelectedNode( Node* selectedNode );
	float getDistBetweenNodes() { return mDistBetweenPrevAndCurrentNode; }
	
	template<typename T>
	CallbackId registerNodeSelected( T *obj, bool (T::*callback)(Node*) ){
		return mCallbacksNodeSelected.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}
	
	// TODO: should these use casts to get NodeArtist/NodeAlbum out?
	Node* getSelectedArtistNode() { return getNodeAtLevel(G_ARTIST_LEVEL); }
	Node* getSelectedAlbumNode() { return getNodeAtLevel(G_ALBUM_LEVEL); }
	
	Node* getNodeAtLevel(int level) {
		if ( mSelectedNode != NULL && mSelectedNode->mGen >= level ) {
			Node *retNode = mSelectedNode;
			while ( retNode->mGen != level) {
				retNode = retNode->mParentNode;
			}
			return retNode;
		}
		return NULL;
	}
	
	std::vector<std::string> getHierarchy();

private:
	CallbackMgr<bool(State*)> mCallbacksPlaylistStateChanged;
	CallbackMgr<bool(State*)> mCallbacksAlphaCharStateChanged;	
	CallbackMgr<bool(Node*)> mCallbacksNodeSelected;
	CallbackMgr<bool(NodeTrack*)> mCallbacksNodePlaying;

	Node *mSelectedNode;
	Node *mPrevSelectedNode;
	float mDistBetweenPrevAndCurrentNode;  // used to control duration of the tween
	char mAlphaChar;
	ci::ipod::PlaylistRef mCurrentPlaylist;
	string mCurrentPlaylistName;
};

