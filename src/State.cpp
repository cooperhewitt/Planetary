/*
 *  State.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "State.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

State::State()
{
	mAlphaChar = ' ';
	mSelectedNode = NULL;
}

void State::setAlphaChar( char c )
{
	std::cout << "State::setAlphaChar " << c << std::endl;
	mAlphaChar = c;
	mCallbacksAlphaCharStateChanged.call( this );
}

void State::setSelectedNode( Node* node )
{
	if (node == NULL) {
		for( int i=0; i<G_NUM_LEVELS; i++ ){
			if( mMapOfNodes[i] ){
				mMapOfNodes[i]->deselect();
				mMapOfNodes.erase(i);
			}
		}		
		mSelectedNode = NULL;
	}
	else {
	
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
	}
	
	if( mSelectedNode )
		G_CURRENT_LEVEL		= mSelectedNode->mGen;
	else
		G_CURRENT_LEVEL		= 0;
	
	mCallbacksNodeSelected.call(mSelectedNode);
}

vector<string> State::getHierarchy()
{
	vector<string> hierarchy;
	if( mSelectedNode != NULL ){
		Node *parent = mSelectedNode;
		while( parent != NULL ){
			hierarchy.push_back( parent->mName );
			parent = parent->mParentNode;
		}		
	}

	if(mAlphaChar != ' ') {
		string s;
		s += mAlphaChar;
		hierarchy.push_back( s );
	}
	
	hierarchy.push_back("Home");
	
	if (hierarchy.size() > 1) {
		reverse( hierarchy.begin(), hierarchy.end() );
	}
	
	return hierarchy;
}
