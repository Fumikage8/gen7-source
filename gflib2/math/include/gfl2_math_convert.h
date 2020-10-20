#ifndef GFLIB2_MATH_CONVERT_H_INCLUDED
#define GFLIB2_MATH_CONVERT_H_INCLUDED
//===================================================================
/**
 * @file    GameData.h
 * @brief   ゲーム進行データ管理
 * @author  k.ohno
 */
//===================================================================
#pragma once

#include <nw/math.h>

#include <math/include/gfl2_Vector2.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Matrix44.h>

#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(math)

//-----------------------------------------------------------------------------
/**
 *					VECTOR
 */
//-----------------------------------------------------------------------------
// gfl2::math::Vector2 -> nw::math::VEC2
static inline nw::math::VEC2 ConvertGfVec2ToNwVec2( const gfl2::math::Vector2& vec )
{
	return nw::math::VEC2( vec.x, vec.y );
}

// nw::math::VEC2 -> gfl2::math::Vector2
static inline gfl2::math::Vector2 ConvertNwVec2ToGfVec2( const nw::math::VEC2& vec )
{
	return gfl2::math::Vector2( vec.x, vec.y );
}


// gfl2::math::Vector3 -> nw::math::VEC3
static inline nw::math::VEC3 ConvertGfVec3ToNwVec3( const gfl2::math::Vector3& vec )
{
	return nw::math::VEC3( vec.x, vec.y, vec.z );
}

// nw::math::VEC3 -> gfl2::math::Vector3
static inline gfl2::math::Vector3 ConvertNwVec3ToGfVec3( const nw::math::VEC3& vec )
{
	return gfl2::math::Vector3( vec.x, vec.y, vec.z );
}


// gfl2::math::Vector4 -> nw::math::VEC4
static inline nw::math::VEC4 ConvertGfVec4ToNwVec4( const gfl2::math::Vector4& vec )
{
	return nw::math::VEC4( vec.x, vec.y, vec.z, vec.w );
}

// nw::math::VEC4 -> gfl2::math::Vector4
static inline gfl2::math::Vector4 ConvertNwVec4ToGfVec4( const nw::math::VEC4& vec )
{
	return gfl2::math::Vector4( vec.x, vec.y, vec.z, vec.w );
}


//-----------------------------------------------------------------------------
/**
 *					MATRIX
 */
//-----------------------------------------------------------------------------
// gfl2::math::Matrix34 -> nw::lyt::MTX34
static inline nw::math::MTX34 ConvertGfMtx34ToNwMtx34( const gfl2::math::Matrix34& gf_mtx )
{
	nw::math::MTX34 nw_mtx;

	for( int i=0; i < nw::math::MTX34::ROW_COUNT; ++i )
	{
		for( int j=0; j < nw::math::MTX34::COLUMN_COUNT; ++j )
		{
			nw_mtx.m[i][j] = gf_mtx.GetElem(i, j);
		}
	}


	return nw_mtx;
}

// nw::lyt::MTX34 -> gfl2::math::Matrix34
static inline gfl2::math::Matrix34 ConvertNwMtx34ToGfMtx34( const nw::math::MTX34& nw_mtx )
{
	gfl2::math::Matrix34 gf_mtx;

	for( int i=0; i < nw::math::MTX34::ROW_COUNT; ++i )
	{
		for( int j=0; j < nw::math::MTX34::COLUMN_COUNT; ++j )
		{
			gf_mtx.SetElem( i, j, nw_mtx.m[i][j] );
		}
	}

	return gf_mtx;
}


// gfl2::math::Matrix44 -> nw::lyt::MTX44
static inline nw::math::MTX44 ConvertGfMtx44ToNwMtx44( const gfl2::math::Matrix44& gf_mtx )
{
	nw::math::MTX44 nw_mtx;
	
	for( int i=0; i < nw::math::MTX44::ROW_COUNT; ++i )
	{
		for( int j=0; j < nw::math::MTX44::COLUMN_COUNT; ++j )
		{
			nw_mtx.m[i][j] = gf_mtx.GetElem(i, j);
		}
	}

	return nw_mtx;
}

// nw::lyt::MTX44 -> gfl2::math::Matrix44
static inline gfl2::math::Matrix44 ConvertNwMtx44ToGfMtx44( const nw::math::MTX44& nw_mtx )
{
	gfl2::math::Matrix44 gf_mtx;

	for( int i=0; i < nw::math::MTX44::ROW_COUNT; ++i )
	{
		for( int j=0; j < nw::math::MTX44::COLUMN_COUNT; ++j )
		{
			gf_mtx.SetElem( i, j, nw_mtx.m[i][j] );
		}
	}

	return gf_mtx;
}




//*****************************************************************************
/**
 *		マトリックスのコンバート
 */
//*****************************************************************************
//-----------------------------------------------------------------------------
/**
 *	gfl2::math::Matrix34 -> nw::math::MTX34
 */
//-----------------------------------------------------------------------------
class ConvNWMatrix34
{
private:
	nw::math::MTX34 m_Mtx;

public:
	nw::math::MTX34* getMatrix( const gfl2::math::Matrix34* inMtx )
	{
		for( int i=0; i<nw::math::MTX34::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX34::COLUMN_COUNT; ++j )
			{
				m_Mtx.m[i][j] = inMtx->GetElem(i, j);
			}
		}
		
		return &m_Mtx;
	}

	void convMtx( const nw::math::MTX34* inMtx, gfl2::math::Matrix34* outMtx )
	{
		for( int i=0; i<nw::math::MTX34::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX34::COLUMN_COUNT; ++j )
			{
				outMtx->SetElem( i, j, inMtx->m[i][j] );
			}
		}
	}
};

//-----------------------------------------------------------------------------
/**
 *	nw::math::MTX34 -> gfl2::math::Matrix34
 */
//-----------------------------------------------------------------------------
class ConvGFMatrix34
{
private:
	gfl2::math::Matrix34 m_Mtx;

public:
	gfl2::math::Matrix34* getMatrix( const nw::math::MTX34* inMtx )
	{
		for( int i=0; i<nw::math::MTX34::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX34::COLUMN_COUNT; ++j )
			{
				m_Mtx.SetElem( i, j, inMtx->m[i][j] );
			}
		}
		
		return &m_Mtx;
	}

	void convMtx( const gfl2::math::Matrix34* inMtx, nw::math::MTX34* outMtx )
	{
		for( int i=0; i<nw::math::MTX34::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX34::COLUMN_COUNT; ++j )
			{
				outMtx->m[i][j] = inMtx->GetElem( i, j );
			}
		}
	}
};

//-----------------------------------------------------------------------------
/**
 *	gfl2::math::Matrix44 -> nw::math::MTX44
 */
//-----------------------------------------------------------------------------
class ConvNWMatrix44
{
private:
	nw::math::MTX44 m_Mtx;

public:
	nw::math::MTX44* getMatrix( const gfl2::math::Matrix44* inMtx )
	{
		for( int i=0; i<nw::math::MTX44::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX44::COLUMN_COUNT; ++j )
			{
				m_Mtx.m[i][j] = inMtx->GetElem(i, j);
			}
		}

		return &m_Mtx;
	}

	void convMtx( const nw::math::MTX44* inMtx, gfl2::math::Matrix44* outMtx )
	{
		for( int i=0; i<nw::math::MTX44::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX44::COLUMN_COUNT; ++j )
			{
				outMtx->SetElem( i, j, inMtx->m[i][j] );
			}
		}
	}
};

//-----------------------------------------------------------------------------
/**
 *	nw::math::MTX44 -> gfl2::math::Matrix44
 */
//-----------------------------------------------------------------------------
class ConvGFMatrix44
{
private:
	gfl2::math::Matrix44 m_Mtx;

public:
	gfl2::math::Matrix44* getMatrix( const nw::math::MTX44* inMtx )
	{
		for( int i=0; i<nw::math::MTX44::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX44::COLUMN_COUNT; ++j )
			{
				m_Mtx.SetElem( i, j, inMtx->m[i][j] );
			}
		}
		
		return &m_Mtx;
	}

	void convMtx( const gfl2::math::Matrix44* inMtx, nw::math::MTX44* outMtx )
	{
		for( int i=0; i<nw::math::MTX44::ROW_COUNT; ++i )
		{
			for( int j=0; j<nw::math::MTX44::COLUMN_COUNT; ++j )
			{
				outMtx->m[i][j] = inMtx->GetElem( i, j );
			}
		}
	}
};

GFL_NAMESPACE_END(math)
GFL_NAMESPACE_END(gfl2)

#endif
