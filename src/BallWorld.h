//
//  Balls.hpp
//  PaperBounce3
//
//  Created by Chaim Gingold on 8/4/16.
//
//

#ifndef Balls_hpp
#define Balls_hpp

#include <vector>
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Color.h"

#include "GameWorld.h"
#include "Contour.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Ball {
	
public:
	vec2 mLoc ;
	vec2 mLastLoc ;
	vec2 mAccel ;
	
	float mRadius ;
	ColorAf mColor ;
	
	void setLoc( vec2 l ) { mLoc=mLastLoc=l; }
	void setVel( vec2 v ) { mLastLoc = mLoc - v ; }
	vec2 getVel() const { return mLoc - mLastLoc ; }
	
	void  setMass( float m ) { mMass = m ; }
	float getMass() const { return mMass ; }
	float getInvMass() const { return 1.f / getMass() ; }
	
	void noteSquashImpact( vec2 directionAndMagnitude )
	{
		if ( length(directionAndMagnitude) > length(mSquash) ) mSquash = directionAndMagnitude ;
	}

	vec2  mSquash ; // direction and magnitude
	
private:
	float mMass = 1.f ; // let's start by doing the right thing.

};


class BallWorld : public GameWorld
{
public:
	
	void setParams( XmlTree ) override;
	void updateContours( const ContourVector &c ) override { mContours = c; }
	
	void update() override;
	void draw() override;
	
	void newRandomBall( vec2 loc );
	void clearBalls() { mBalls.clear(); }
	
	float getBallDefaultRadius() const { return mBallDefaultRadius ; }
	
	vec2 resolveCollisionWithContours	( vec2 p, float r ) const ; // returns pinned version of point
		// public so we can show it with the mouse...
	
	void mouseClick( vec2 p ) override { newRandomBall(p) ; }
	
private:

	// params
	float	mBallDefaultRadius		= 8.f *  .5f ;
	float	mBallDefaultMaxRadius	= 8.f * 4.f ;
	float	mBallMaxVel				= 8.f ;
	ColorAf mBallDefaultColor		= ColorAf::hex(0xC62D41);
	
	//
	vec2 resolveCollisionWithBalls		( vec2 p, float r, Ball* ignore=0, float correctionFraction=1.f ) const ;
		// simple pushes p out of overlapping balls.
		// but might take multiple iterations to respond to all of them
		// fraction is (0,1], how much of the collision correction to do.
	
	void resolveBallCollisions() ;

	ContourVector		mContours;
	vector<Ball>		mBalls ;
	
} ;


#endif /* Balls_hpp */