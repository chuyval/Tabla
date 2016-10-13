//
//  MusicWorld.hpp
//  PaperBounce3
//
//  Created by Chaim Gingold on 9/20/16.
//
//

#ifndef MusicWorld_hpp
#define MusicWorld_hpp

#include "GameWorld.h"
#include "PureDataNode.h"

class MusicWorld : public GameWorld
{
public:

	MusicWorld();
	~MusicWorld();
	
	string getSystemName() const override { return "MusicWorld"; }

	void update() override;
	void updateContours( const ContourVector &c ) override { mContours=c; }
	void draw( bool highQuality ) override;

private:
	
	float mStartTime;
	vec2  mTimeVec; // in world space, which way does time flow forward?
	
	// scores
	ContourVector mContours;
	
	void getQuadOrderedSides( const PolyLine2& p, vec2 out[4] );
	
	// synthesis
	cipd::PureDataNodeRef	mPureDataNode;	// synth engine
	cipd::PatchRef			mPatch;			// pong patch
	
	void setupSynthesis();
};

class MusicWorldCartridge : public GameCartridge
{
public:
	virtual std::shared_ptr<GameWorld> load() const override
	{
		return std::make_shared<MusicWorld>();
	}
};

#endif /* MusicWorld_hpp */
