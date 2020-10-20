#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>

//using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfBinaryTexData::Impl::~Impl()
{
	
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfBinaryTexData::Impl::setTextureData( c8* pData )
{
	u32*					pSeek = reinterpret_cast<u32*>(pData);

	m_Version = *pSeek;
	++pSeek;

	m_DataNum = *pSeek;
	++pSeek;

	m_pTextureDataTop = reinterpret_cast<c8*>(pSeek);

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfBinaryTexData::Impl::GetSize( void )
{
	return m_DataNum;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfBinaryTexData::Impl::GetData( u32 index )
{
	if ( index >= m_DataNum )
		return NULL;

	c8*							pSeek = m_pTextureDataTop;
	ChunkHeader			*pHeader;

	for( u32 i = 0; i < index; ++i )
	{
		pHeader = reinterpret_cast<ChunkHeader*>( pSeek );
		pSeek += sizeof(ChunkHeader);
		pSeek += pHeader->m_Size;
	}

	pSeek += sizeof(ChunkHeader);

	return pSeek;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfBinaryTexData::Impl::GetDataSize( u32 index )
{
	if ( index >= m_DataNum )
		return 0;

	c8*							pSeek = m_pTextureDataTop;
	ChunkHeader			*pHeader;

	for( u32 i = 0; i < index; ++i )
	{
		pHeader = reinterpret_cast<ChunkHeader*>( pSeek );
		pSeek += sizeof(ChunkHeader);
		pSeek += pHeader->m_Size;
	}

	pHeader = reinterpret_cast<ChunkHeader*>( pSeek );

	return pHeader->m_Size;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfBinaryTexData::Impl::GetType( u32 index )
{
	if ( index >= m_DataNum )
		return IResourceData::Type("none");

	c8*							pSeek = m_pTextureDataTop;
	ChunkHeader			*pHeader;

	for( u32 i = 0; i < index; ++i )
	{
		pHeader = reinterpret_cast<ChunkHeader*>( pSeek );
		pSeek += sizeof(ChunkHeader);
		pSeek += pHeader->m_Size;
	}

	pHeader = reinterpret_cast<ChunkHeader*>( pSeek );

	IResourceData::Type			type;

	type.m_ID = pHeader->m_ID;

	return type;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfBinaryTexDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryTexData::GfBinaryTexData() : IResourceData()
{
	
}

bool GfBinaryTexData::SetTextureData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }
	return m_Impl.setTextureData( pData );
}


GfBinaryTexData::~GfBinaryTexData()
{
	
}

//! @brief 全データの数の取得
s32 GfBinaryTexData::GetSize( void )
{
	return m_Impl.GetSize();
}

//! @brief データの取得
const void* GfBinaryTexData::GetData( u32 index )
{
	return m_Impl.GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryTexData::GetDataSize( u32 index )
{
	return m_Impl.GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryTexData::GetType( u32 index )
{
	return m_Impl.GetType(index);
}

}}}}
