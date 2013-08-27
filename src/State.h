/*
 *  State.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
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
    
    enum FilterMode {
        FilterModeAlphaChar,
        FilterModePlaylist
    };    
    
	State() {};
    
    void setup();
	
	// Alpha char for filtering artist name
	char getAlphaChar(){ return mAlphaChar; }
	void setAlphaChar( char c );
	void setAlphaChar( const string &artistName );
	template<typename T>
	CallbackId registerAlphaCharStateChanged( T *obj, bool ( T::*callback )( char ) ){
		return mCallbacksAlphaCharStateChanged.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	
	// Playlist filtering
	ci::ipod::PlaylistRef getPlaylist(){ return mCurrentPlaylist; }
	void setPlaylist( ci::ipod::PlaylistRef playlist );
	template<typename T>
	CallbackId registerPlaylistStateChanged( T *obj, bool ( T::*callback )( ci::ipod::PlaylistRef ) ){
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
	Node* getSelectedTrackNode() { return getNodeAtLevel(G_TRACK_LEVEL); }	
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

    bool setFilterMode(FilterMode filterMode);
    FilterMode getFilterMode() { return mFilterMode; }
	template<typename T>
	CallbackId registerFilterModeStateChanged( T *obj, bool ( T::*callback )( FilterMode ) ){
		return mCallbacksFilterModeStateChanged.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:
	CallbackMgr<bool(ci::ipod::PlaylistRef)> mCallbacksPlaylistStateChanged;
	CallbackMgr<bool(char)> mCallbacksAlphaCharStateChanged;	
	CallbackMgr<bool(FilterMode)> mCallbacksFilterModeStateChanged;
	CallbackMgr<bool(Node*)> mCallbacksNodeSelected;
	CallbackMgr<bool(NodeTrack*)> mCallbacksNodePlaying;

	Node *mSelectedNode;	
	float mDistBetweenPrevAndCurrentNode;  // used to control duration of the tween
    
    FilterMode mFilterMode;
	char mAlphaChar;
	ci::ipod::PlaylistRef mCurrentPlaylist;
};

