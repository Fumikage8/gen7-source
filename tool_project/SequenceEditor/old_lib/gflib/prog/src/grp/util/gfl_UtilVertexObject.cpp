//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilVertexObject.cpp
 *	@brief  頂点オブジェクトクラス
 *	@author	Masateru Ishiguro
 *	@date		2011.2.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gfl_UtilShader.h"
#include <debug/gfl_Assert.h>
#include <grp/gfl_GraphicsType.h>
#include <grp/util/gfl_UtilVertexObject.h>

namespace gfl { namespace grp { namespace util {

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
VertexObject::VertexObject()
{

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
VertexObject::~VertexObject()
{

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void VertexObject::CreateIndexBuffer( s32 size, u16* pData )
{
	this->m_IndexNum = size / sizeof(u16);

	glGenBuffers( 1, &this->m_IndexBufferId );	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->m_IndexBufferId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW );

	if ( pData )
	{
		WriteIndexBuffer( pData, size );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void VertexObject::CreateVertexBuffer( s32 size, Vertex* pData )
{
	glGenBuffers( 1, &this->m_VertexBufferId );	
	glBindBuffer( GL_ARRAY_BUFFER, this->m_VertexBufferId );
	glBufferData( GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW );

	if ( pData )
	{
		WriteVertexBuffer( pData, size );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void VertexObject::WriteIndexBuffer( u16* pData, s32 size, s32 offset )
{
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, pData );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void VertexObject::WriteVertexBuffer( Vertex* pData, s32 size, s32 offset )
{
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, pData );
}

}}}
