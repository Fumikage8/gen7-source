#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryLocData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>

//using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfBinaryLocData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfBinaryLocData::Impl : public gfx::GLMemory
{

public:
	Impl():gfx::GLMemory(),
		m_pLocDataTop(NULL),
		m_DataNum(0)
	{}

	virtual ~Impl();

	bool setLocData( c8* pData );

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );

	c8											*m_pLocDataTop;
	u32											m_Version;
	u32											m_DataNum;
	u32											m_DataSize;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfBinaryLocData::Impl::~Impl()
{
	
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfBinaryLocData::Impl::setLocData( c8* pData )
{
	u32*					pSeek = reinterpret_cast<u32*>(pData);

	m_Version = *pSeek;
	++pSeek; 

	m_DataNum = *pSeek;
	++pSeek; 
	++pSeek; 

	m_DataSize = *pSeek;

	// Header Size is 24 Bytes:
	m_pLocDataTop = reinterpret_cast<c8*>(pData+24);	


	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfBinaryLocData::Impl::GetSize( void )
{
	return m_DataNum;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfBinaryLocData::Impl::GetData( u32 index )
{
	return m_pLocDataTop;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfBinaryLocData::Impl::GetDataSize( u32 index )
{
	return m_DataSize;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfBinaryLocData::Impl::GetType( u32 index )
{
	return IResourceData::Type("locator");	
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfBinaryLocDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryLocData::GfBinaryLocData() : IResourceData()
{
	m_pImpl = new Impl();
}

bool GfBinaryLocData::SetLocData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }
	return m_pImpl->setLocData( pData );
}


GfBinaryLocData::~GfBinaryLocData()
{
	delete m_pImpl;
}

//! @brief 全データの数の取得
s32 GfBinaryLocData::GetSize( void )
{
	return m_pImpl->GetSize();
}

//! @brief データの取得
const void* GfBinaryLocData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryLocData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryLocData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}

//! @brief 
u32 GfBinaryLocData::GetVersionId( void )
{
	return m_pImpl->m_Version;
}

}}}}
