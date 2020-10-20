#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>

//using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfBinaryMdlDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryMotData::GfBinaryMotData() :
  IResourceData(),
  m_pMotionDataTop(NULL)
{
}

GfBinaryMotData::~GfBinaryMotData()
{
}

bool GfBinaryMotData::SetMotionData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }

  m_pMotionDataTop = pData;
  return true;
}

//! @brief 全データの数の取得
s32 GfBinaryMotData::GetSize( void )
{
	return 1;
}

//! @brief データの取得
const void* GfBinaryMotData::GetData( u32 index )
{
	return m_pMotionDataTop;
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryMotData::GetDataSize( u32 index )
{
	return 1;
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryMotData::GetType( u32 index )
{
  static const IResourceData::Type motType("animres");
  return motType;
}

}}}}
