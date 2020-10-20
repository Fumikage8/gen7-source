#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
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
GfBinaryCollisionMdlData::Impl::~Impl()
{
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfBinaryCollisionMdlData::Impl::setModelData( c8* pData , s32 size )
{
  c8* pSeek = pData;
  s32 binarySize = size;

  // @attention コリジョンモデルのキャッシュバージョンが更新されたらここも変更しないといけない
  static const u32 sc_cacheVersion = 0x14120500;

  u32 cacheVersion = 0;
  std::memcpy(&cacheVersion, pSeek, sizeof(cacheVersion));

  pSeek += sizeof(cacheVersion);
  binarySize = size - sizeof(cacheVersion);

  // キャッシュバージョンが合わなければ失敗
  if (cacheVersion != sc_cacheVersion)
  {
    return false;
  }

  // バイナリデータ保持
  // 4バイト ポリゴン数
  // メッシュデータ
  m_pBinaryData = pSeek;
  m_BinaryDataSize = binarySize;

  // データ数
  m_DataNum = 1;

  // バージョン
  m_Version = 0.0f;

	return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfBinaryCollisionMdlData::Impl::GetSize( void )
{
	return m_DataNum;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfBinaryCollisionMdlData::Impl::GetData( u32 index )
{
  return m_pBinaryData;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfBinaryCollisionMdlData::Impl::GetDataSize( u32 index )
{
  return m_BinaryDataSize;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfBinaryCollisionMdlData::Impl::GetType( u32 index )
{
	return IResourceData::Type("collmesh");
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfBinaryCollisionMdlDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryCollisionMdlData::GfBinaryCollisionMdlData() : IResourceData()
{
	
}

bool GfBinaryCollisionMdlData::SetModelData( c8* pData , s32 size )
{
  if (pData == NULL)
  {
    return false;
  }
	return m_Impl.setModelData( pData , size);
}


GfBinaryCollisionMdlData::~GfBinaryCollisionMdlData()
{
	
}

//! @brief 全データの数の取得
s32 GfBinaryCollisionMdlData::GetSize( void )
{
	return m_Impl.GetSize();
}

//! @brief データの取得
const void* GfBinaryCollisionMdlData::GetData( u32 index )
{
	return m_Impl.GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryCollisionMdlData::GetDataSize( u32 index )
{
	return m_Impl.GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryCollisionMdlData::GetType( u32 index )
{
	return m_Impl.GetType(index);
}

}}}}

