//
//  PinballWorld.hpp
//  PaperBounce3
//
//  Created by Chaim Gingold on 12/5/16.
//
//

#ifndef PinballWorld_hpp
#define PinballWorld_hpp

#include "BallWorld.h"
#include "PureDataNode.h"

class PinballWorld : public BallWorld
{
public:

	PinballWorld();
	~PinballWorld();

	void setParams( XmlTree ) override;
	
	string getSystemName() const override { return "PinballWorld"; }

	void gameWillLoad() override;
	void update() override;
	void draw( DrawType ) override;
	
	void worldBoundsPolyDidChange() override;

	void keyDown( KeyEvent ) override;

protected:
	virtual void onBallBallCollide   ( const Ball&, const Ball& ) override;
	virtual void onBallContourCollide( const Ball&, const Contour& ) override;
	virtual void onBallWorldBoundaryCollide	( const Ball& ) override;

private:
	
	// keymap
	map<char,string> mKeyToInput; // maps keystrokes to input names
	map<string,function<void()>> mInputToFunction; // maps input names to code handlers

	// synthesis
	cipd::PureDataNodeRef	mPureDataNode;	// synth engine
	cipd::PatchRef			mPatch;			// pong patch
	
	void setupSynthesis();

};

class PinballWorldCartridge : public GameCartridge
{
public:
	virtual string getSystemName() const override { return "PinballWorld"; }

	virtual std::shared_ptr<GameWorld> load() const override
	{
		return std::make_shared<PinballWorld>();
	}
};

#endif /* PinballWorld_hpp */