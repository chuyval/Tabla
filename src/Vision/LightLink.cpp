//
//  LightLink.cpp
//  PaperBounce3
//
//  Created by Chaim Gingold on 8/5/16.
//
//

#include "LightLink.h"
#include "xml.h"

static string vecToString( vec2 v )
{
	return toString(v.x) + " " + toString(v.y);
};

static string vecsToString( const vec2* a, int n )
{
	string s;
	
	for( int i=0; i<n; ++i )
	{
		s += vecToString(a[i]) + "\n";
	}
	
	return s;
};


static vector<float> stringToFloatVec( string value )
{
	// clobber [], chars
	for( int i=0; i<value.size(); ++i )
	{
		if (value[i]==','||value[i]=='['||value[i]==']'||value[i]==';')
		{
			value[i]=' ';
		}
	}		
	
	// break into float vector
	vector<float> f;
	
	std::istringstream ss(value);
	while ( !ss.fail() )
	{
		float x;
		ss >> x;
		if (!ss.fail()) f.push_back(x);
	}
	
	return f;
}

LightLink::CaptureProfile::CaptureProfile( string name, string deviceName, vec2 size )
	: mName(name)
	, mDeviceName(deviceName)
	, mCaptureSize(size)
{
	mCaptureCoords[0] = vec2(0,1) * size;
	mCaptureCoords[1] = vec2(1,1) * size;
	mCaptureCoords[2] = vec2(1,0) * size;
	mCaptureCoords[3] = vec2(0,0) * size;
	
	for( int i=0; i<4; ++i )
	{
		mCaptureWorldSpaceCoords[i] = mCaptureCoords[i] / 10.f ; // 10px per cm as a default...
	}
}

void LightLink::CaptureProfile::setParams( XmlTree xml )
{
	getXml(xml, "Name",mName);
	getXml(xml, "DeviceName",mDeviceName);
	getXml(xml, "CaptureSize",mCaptureSize);
	getXml(xml, "CaptureCoords", mCaptureCoords, 4 );
	getXml(xml, "CaptureWorldSpaceCoords", mCaptureWorldSpaceCoords, 4 );

	if ( xml.hasChild("DistCoeffs") )
	{
		vector<float> f = stringToFloatVec( xml.getChild("DistCoeffs").getValue() );
		
		if (f.size() > 0)
		{
			mDistCoeffs = cv::Mat(1,f.size(),CV_32F);
			for( int i=0; i<f.size(); ++i ) mDistCoeffs.at<float>(i) = f[i];
			cout << "DistCoeffs: " << mDistCoeffs << endl;
		}
	}

	if ( xml.hasChild("CameraMatrix") )
	{
		vector<float> f = stringToFloatVec( xml.getChild("CameraMatrix").getValue() );
		
		if (f.size() == 9)
		{
			mCameraMatrix = cv::Mat(3,3,CV_32F);
			for( int i=0; i<f.size(); ++i ) mCameraMatrix.at<float>(i) = f[i];
			cout << "CameraMatrix: " << mCameraMatrix << endl;
		}
	}
}

XmlTree LightLink::CaptureProfile::getParams() const
{
	XmlTree t("Profile","");
	
	t.push_back( XmlTree( "Name", mName) );
	t.push_back( XmlTree( "DeviceName", mDeviceName) );
	t.push_back( XmlTree( "CaptureSize", vecToString(mCaptureSize) ) );
	t.push_back( XmlTree( "CaptureCoords", vecsToString(mCaptureCoords,4) ));
	t.push_back( XmlTree( "CaptureWorldSpaceCoords", vecsToString(mCaptureWorldSpaceCoords,4) ));

	if ( mDistCoeffs.rows==1 )
	{
		std::ostringstream ss;
		ss << mDistCoeffs;
		t.push_back( XmlTree( "DistCoeffs", ss.str() ) );
	}

	if ( !mCameraMatrix.empty() )
	{
		std::ostringstream ss;
		ss << mCameraMatrix;
		t.push_back( XmlTree( "CameraMatrix", ss.str() ) );
	}
	
	return t;
}

LightLink::ProjectorProfile::ProjectorProfile( string name, vec2 size, const vec2 captureWorldSpaceCoords[4] )
	: mName(name)
	, mProjectorSize(size)
{
	mProjectorCoords[0] = vec2(0,1) * size;
	mProjectorCoords[1] = vec2(1,1) * size;
	mProjectorCoords[2] = vec2(1,0) * size;
	mProjectorCoords[3] = vec2(0,0) * size;
	
	if (captureWorldSpaceCoords)
	{
		for( int i=0; i<4; ++i ) {
			mProjectorWorldSpaceCoords[i] = captureWorldSpaceCoords[i];
		}
	}
	else
	{
		for( int i=0; i<4; ++i ) {
			mProjectorWorldSpaceCoords[i] = mProjectorCoords[i] / 10.f ; // 10px per cm as a default...
		}
	}
}

void LightLink::ProjectorProfile::setParams( XmlTree xml )
{
	getXml(xml, "Name",mName);
	getXml(xml, "ProjectorWorldSpaceCoords", mProjectorWorldSpaceCoords, 4 );
	getXml(xml, "ProjectorSize", mProjectorSize);
	getXml(xml, "ProjectorCoords", mProjectorCoords, 4 );
}

XmlTree LightLink::ProjectorProfile::getParams() const
{
	XmlTree t("Profile","");
	
	t.push_back( XmlTree( "Name", mName) );
	t.push_back( XmlTree( "ProjectorWorldSpaceCoords", vecsToString(mProjectorWorldSpaceCoords,4) ));
	t.push_back( XmlTree( "ProjectorSize", vecToString(mProjectorSize)) );
	t.push_back( XmlTree( "ProjectorCoords", vecsToString(mProjectorCoords,4) ));
	
	return t;
}

void LightLink::setParams( XmlTree xml )
{
	getXml(xml,"CaptureProfile",mActiveCaptureProfileName);
	getXml(xml,"ProjectorProfile",mActiveProjectorProfileName);
	
	mCaptureProfiles.clear();
	for( auto i = xml.begin( "Capture/Profile" ); i != xml.end(); ++i )
	{
		CaptureProfile p;
		p.setParams(*i);
		mCaptureProfiles[p.mName] = p;
	}
	
	mProjectorProfiles.clear();
	for( auto i = xml.begin( "Projector/Profile" ); i != xml.end(); ++i )
	{
		ProjectorProfile p;
		p.setParams(*i);
		mProjectorProfiles[p.mName] = p;
	}
	
	ensureActiveProfilesAreValid();
}

XmlTree LightLink::getParams() const
{
	XmlTree t("LightLink","");
	
	t.push_back( XmlTree("CaptureProfile", mActiveCaptureProfileName) );
	t.push_back( XmlTree("ProjectorProfile", mActiveProjectorProfileName) );
	
	XmlTree capture("Capture","");
	for( const auto& p : mCaptureProfiles ) {
		capture.push_back( p.second.getParams() );
	}
	t.push_back(capture);

	XmlTree projector("Projector","");
	for( const auto& p : mProjectorProfiles ) {
		projector.push_back( p.second.getParams() );
	}
	t.push_back(projector);
	
	return t;
}

LightLink::CaptureProfile&
LightLink::getCaptureProfile()
{
	auto i = mCaptureProfiles.find(mActiveCaptureProfileName);
	assert( i != mCaptureProfiles.end() );
	return i->second;
}

LightLink::ProjectorProfile&
LightLink::getProjectorProfile()
{
	auto i = mProjectorProfiles.find(mActiveProjectorProfileName);
	assert( i != mProjectorProfiles.end() );
	return i->second;
}

const LightLink::CaptureProfile&
LightLink::getCaptureProfile() const
{
	auto i = mCaptureProfiles.find(mActiveCaptureProfileName);
	assert( i != mCaptureProfiles.end() );
	return i->second;
}

const LightLink::ProjectorProfile&
LightLink::getProjectorProfile() const
{
	auto i = mProjectorProfiles.find(mActiveProjectorProfileName);
	assert( i != mProjectorProfiles.end() );
	return i->second;
}

vector<const LightLink::CaptureProfile*>
LightLink::getCaptureProfilesForDevice( string deviceName ) const
{
	vector<const CaptureProfile*> result;
	
	for( const auto &p : mCaptureProfiles )
	{
		if (p.second.mDeviceName==deviceName)
		{
			result.push_back( &(p.second) );
		}
	}
	
	return result;
}

void LightLink::ensureActiveProfilesAreValid()
{
//	assert( !mProjectorProfiles.empty() );
//	assert( !mCaptureProfiles.empty() );
//  don't do this anymore

	if ( !mCaptureProfiles.empty() && mCaptureProfiles.find(mActiveCaptureProfileName) == mCaptureProfiles.end() )
	{
		mActiveCaptureProfileName = mCaptureProfiles.begin()->second.mName;
	}

	if ( !mProjectorProfiles.empty() && mProjectorProfiles.find(mActiveProjectorProfileName) == mProjectorProfiles.end() )
	{
		mActiveProjectorProfileName = mProjectorProfiles.begin()->second.mName;
	}
}

void LightLink::setCaptureProfile  ( string name ) {
	mActiveCaptureProfileName=name;
	assert(mCaptureProfiles.find(name)!=mCaptureProfiles.end());
}

void LightLink::setProjectorProfile( string name ) {
	mActiveProjectorProfileName=name;
	assert(mProjectorProfiles.find(name)!=mProjectorProfiles.end());
}