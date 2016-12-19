//
//  ocv.h
//  PaperBounce3
//
//  Created by Chaim Gingold on 8/10/16.
//
//

#ifndef ocv_h
#define ocv_h

namespace cinder {
	
	inline void vec2toOCV_4( vec2 in[4], cv::Point2f o[4] )
	{
		for ( int i=0; i<4; ++i ) o[i] = toOcv( in[i] );
	}
	
	inline vector<vec2> fromOcv( vector<cv::Point> pts )
	{
		vector<vec2> v;
		for( auto p : pts ) v.push_back(vec2(p.x,p.y));
		return v;
	}

	inline vector<vec2> fromOcv( vector<cv::Point2f> pts )
	{
		vector<vec2> v;
		for( auto p : pts ) v.push_back(vec2(p.x,p.y));
		return v;
	}

	// stuff below doesn't really belong in cinder namespace, but whatever.
	inline glm::mat3x3 fromOcvMat3x3( const cv::Mat& m )
	{
		glm::mat3x3 r;
		
		for( int i=0; i<3; ++i )
		{
			for( int j=0; j<3; ++j )
			{
				r[j][i] = m.at<double>(i,j);
				// opencv: (row,column)
				// glm:    (column,row)
			}
		}
		
		return r;
	}

	inline mat4 mat3to4 ( mat3 i )
	{
		mat4 o;
		
		// copy upper left 2x2
		for( int x=0; x<2; ++x )
		for( int y=0; y<2; ++y )
		{
			o[x][y] = i[x][y];
		}
		
		o[0][3] = i[0][2];
		o[1][3] = i[1][2];

		o[3][0] = i[2][0];
		o[3][1] = i[2][1];
		
		o[3][3] = i[2][2];
		
		return o;
	}

	inline mat3 mat4to3 ( mat4 i )
	{
		mat3 o;

		// copy upper left 2x2
		for( int x=0; x<2; ++x )
		for( int y=0; y<2; ++y )
		{
			o[x][y] = i[x][y];
		}

		o[0][2] = i[0][3];
		o[1][2] = i[1][3];

		o[2][0] = i[3][0];
		o[2][1] = i[3][1];

		o[2][2] = i[3][3];

		return o;
	}


	inline mat4 getOcvPerspectiveTransform( const vec2 from[4], const vec2 to[4] )
	{
		cv::Point2f srcpt[4], dstpt[4];
		
		for( int i=0; i<4; ++i )
		{
			srcpt[i] = toOcv( from[i] );
			dstpt[i] = toOcv( to[i] );
		}

		cv::Mat xform = cv::getPerspectiveTransform( srcpt, dstpt ) ;
		
		return mat3to4( fromOcvMat3x3(xform) );
	}

	template <class T>
	bool isMatEqual( cv::Mat a, cv::Mat b )
	{
		// http://stackoverflow.com/questions/9905093/how-to-check-whether-two-matrixes-are-identical-in-opencv

		if ( a.empty() != b.empty() ) return false;
		if ( a.cols != b.cols ) return false;
		if ( a.rows != b.rows ) return false;

		for( int x=0; x<b.cols; ++x )
		for( int y=0; y<b.cols; ++y )
		{
			if ( a.at<T>(y,x) != b.at<T>(y,x) ) return false;
		}

		return true;
	}
	
}


#endif /* ocv_h */