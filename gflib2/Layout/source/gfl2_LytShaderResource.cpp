//----------------------------------------------------------------------------
/**
 *  @file   LytShaderResource.cpp
 *  @brief  RectDrawerShaderのリソース管理のクラス
 *  @author k.ohno
 *  @date   2015.6.24
 */
//----------------------------------------------------------------------------
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "layout/include/gfl2_LytShaderResource.h"

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LytShaderResource::LytShaderResource(void)
{
 // m_ShaderResource.Initialize();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LytShaderResource::~LytShaderResource()
{
  GflHeapFreeMemory(mpData);
}


//-----------------------------------------------------------------------------
/**
 * @brief   シェーダーの登録
 * @li  レイアウトに設定されたメモリを使用する
 */
//-----------------------------------------------------------------------------
void LytShaderResource::SetData( void* pData, u32 size )
{
  mpData = pData;
  mSize = size;
}


//-----------------------------------------------------------------------------
/**
 * @brief   シェーダーの登録
 * @li  レイアウトに設定されたメモリを使用する
 */
//-----------------------------------------------------------------------------
void LytShaderResource::Setup(void )
{
  GFL_ASSERT(!m_ShaderResource.Initialized());
#if defined(GF_PLATFORM_CTR)
  m_ShaderResource.Setup(mpData,mSize);
#else
  m_ShaderResource.Setup(1000);
  //  m_ShaderResource.Setup(mSize, mpData);
#endif

}

//-----------------------------------------------------------------------------
/**
 * @brief   シェーダーの登録
 * @li  レイアウトに設定されたメモリを使用する
 */
//-----------------------------------------------------------------------------
void LytShaderResource::Release(void )
{
  m_ShaderResource.Finalize();
}


//-----------------------------------------------------------------------------
/**
 * @brief   GraphicsResourceの引き出し
 * @return  グラフィックリソース
 */
//-----------------------------------------------------------------------------
nw::lyt::GraphicsResource* LytShaderResource::GetGraphicsResource( void )
{
  return &m_ShaderResource;
}


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

