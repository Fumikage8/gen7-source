#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>
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
GfBinaryMcnfData::GfBinaryMcnfData() :
  IResourceData(),
  m_pMcnfDataTop(NULL)
{
}

bool GfBinaryMcnfData::SetMcnfData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }

  m_pMcnfDataTop = pData;
  return true;
}


GfBinaryMcnfData::~GfBinaryMcnfData()
{
}

//! @brief 全データの数の取得
s32 GfBinaryMcnfData::GetSize( void )
{
  return 1;
}

//! @brief データの取得
const void* GfBinaryMcnfData::GetData( u32 index )
{
  return m_pMcnfDataTop;
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryMcnfData::GetDataSize( u32 index )
{
  return 1;
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryMcnfData::GetType( u32 index )
{
  static IResourceData::Type s_type("mcnf");
	return s_type;
}

}}}}
