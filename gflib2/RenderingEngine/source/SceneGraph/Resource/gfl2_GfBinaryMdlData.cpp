#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfBinaryMdlData::Impl::~Impl()
{
	
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfBinaryMdlData::Impl::setModelData( c8* pData )
{
	u32*					pSeek = reinterpret_cast<u32*>(pData);

	m_Version = *pSeek;
	++pSeek;

	m_DataNum = *pSeek;
	++pSeek;

	++pSeek;
	++pSeek;

	m_pModelDataTop = reinterpret_cast<c8*>(pSeek);

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfBinaryMdlData::Impl::GetSize( void )
{
	return m_DataNum;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfBinaryMdlData::Impl::GetData( u32 index )
{
	if ( index >= m_DataNum )
		return NULL;

	c8*							pSeek = m_pModelDataTop;
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
u32 GfBinaryMdlData::Impl::GetDataSize( u32 index )
{
	if ( index >= m_DataNum )
		return 0;

	c8*							pSeek = m_pModelDataTop;
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
IResourceData::Type GfBinaryMdlData::Impl::GetType( u32 index )
{
	if ( index >= m_DataNum )
		return IResourceData::Type("none");

	c8*							pSeek = m_pModelDataTop;
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
//! @brief  GfBinaryMdlDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryMdlData::GfBinaryMdlData() : IResourceData()
{
	
}

bool GfBinaryMdlData::SetModelData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }
	return m_Impl.setModelData( pData );
}


GfBinaryMdlData::~GfBinaryMdlData()
{
	
}

//! @brief 全データの数の取得
s32 GfBinaryMdlData::GetSize( void )
{
	return m_Impl.GetSize();
}

//! @brief データの取得
const void* GfBinaryMdlData::GetData( u32 index )
{
	return m_Impl.GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryMdlData::GetDataSize( u32 index )
{
	return m_Impl.GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryMdlData::GetType( u32 index )
{
	return m_Impl.GetType(index);
}

//! @brief 
u32 GfBinaryMdlData::GetVersionId( void )
{
	return m_Impl.m_Version;
}

}}}}
