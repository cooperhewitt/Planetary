/*
 *  State.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#include <deque>
#include "State.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

void State::setup()
{
    mFilterMode		= FilterModeAlphaChar;
	mAlphaChar		= 'A';
	mSelectedNode	= NULL;
	mDistBetweenPrevAndCurrentNode = 50.0f;
}

void State::setAlphaChar( char c )
{
    mAlphaChar = c;
    mCallbacksAlphaCharStateChanged.call( mAlphaChar );
}

void State::setAlphaChar( const string &name )
{
	char firstLetter = name[0];	
	if (name.size() > 4) {
		string the = name.substr( 0, 4 );
		if( the == "The " || the == "the " ){
			firstLetter = name[4];
		}	
	}
	if (!isalpha(firstLetter)) {
		firstLetter = '#';
	}
	setAlphaChar(firstLetter);
}


void State::setPlaylist( ci::ipod::PlaylistRef playlist )
{
	mCurrentPlaylist = playlist;
	mCallbacksPlaylistStateChanged.call( playlist );
}


void State::setSelectedNode( Node* node )
{
//	if (node == mSelectedNode) {
//		return;
//	}
	
	if (node == NULL) {
		// clear currently selected node and all parents
		Node *selection = mSelectedNode;
		while( selection != NULL ) {
			selection->deselect();
			selection = selection->mParentNode;
		}
		mSelectedNode = NULL;
		mDistBetweenPrevAndCurrentNode = 10.0f;
	}
	else {
		
		// deque so we can push_front instead of messing with reverse()
		deque<Node*> currentChain;
		deque<Node*> nextChain;
		
		Node* current = mSelectedNode;	// track, album, artist
		while (current != NULL) {
			currentChain.push_front(current);
			current = current->mParentNode;
		}

		Node* next = node;				// new track, album, artist
		while (next != NULL) {
			nextChain.push_front(next);
			next = next->mParentNode;
		}

		vector<bool> sharedAncestor;
		for (int i = 0; i < currentChain.size(); i++) {
			if (nextChain.size() - 1 < i) {
				sharedAncestor.push_back(false);
			}
			else {
				sharedAncestor.push_back(currentChain[i] == nextChain[i]);
			}
		}
		
		for (int i = currentChain.size() - 1; i >= 0; i--) { 
			if (!sharedAncestor[i]) {
				currentChain[i]->deselect();
			}
		}
		
		Node* prevSelectedNode = mSelectedNode;
		mSelectedNode = node;
		
		if( prevSelectedNode && mSelectedNode && (prevSelectedNode != mSelectedNode) ) {
			mDistBetweenPrevAndCurrentNode = prevSelectedNode->mPos.distance( mSelectedNode->mPos );
		}
				
		// ensure everything in the next chain is selected
        for (int i = 0; i < nextChain.size(); i++) {
            nextChain[i]->select();
        }
	}
    
	// and then spread the good word
	mCallbacksNodeSelected.call(mSelectedNode);
}

bool State::setFilterMode(FilterMode filterMode)
{ 
    mFilterMode = filterMode; 
    mCallbacksFilterModeStateChanged.call(filterMode);
    return false; // for use in callbacks
}
