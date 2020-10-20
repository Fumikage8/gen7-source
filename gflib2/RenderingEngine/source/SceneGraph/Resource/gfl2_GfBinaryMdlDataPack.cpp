#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

//using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfBinaryMdlDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryMdlDataPack::GfBinaryMdlDataPack() :
  IResourceData(),
  m_pModelDataTop(NULL)
{
}

bool GfBinaryMdlDataPack::SetModelData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }

  m_pModelDataTop = pData;
  return true;
}


GfBinaryMdlDataPack::~GfBinaryMdlDataPack()
{
}

//! @brief 全データの数の取得
s32 GfBinaryMdlDataPack::GetSize( void )
{
  return 1;
}

//! @brief データの取得
const void* GfBinaryMdlDataPack::GetData( u32 index )
{
  return m_pModelDataTop;
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryMdlDataPack::GetDataSize( u32 index )
{
  return 1;
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryMdlDataPack::GetType( u32 index )
{
  static IResourceData::Type type("mdlp");
	return type;
}

//! @brief 
u32 GfBinaryMdlDataPack::GetVersionId( void )
{
  return 0;
}

}}}}
