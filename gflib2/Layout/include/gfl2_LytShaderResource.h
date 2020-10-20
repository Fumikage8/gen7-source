//----------------------------------------------------------------------------
/**
 *  @file    LytShaderResource.h
 *  @brief   RectDrawerShaderのリソース管理のクラス
 *  @author  k.ohno
 *  @date    2015.6.24
 */
//----------------------------------------------------------------------------
#if !defined( __GFL2_LYTSHADERRESOURCE_H__ )
#define __GFL2_LYTSHADERRESOURCE_H__
#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nn.h>
#include <nw/lyt2.h>
#elif defined(GF_PLATFORM_WIN32)
#include <nw/lyt.h>
#endif

#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_Font.h>

#include <layout/include/gfl2_GraphicsType.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)


//-----------------------------------------------------------------------------
/**
*/
//-----------------------------------------------------------------------------
class LytShaderResource
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytShaderResource);


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LytShaderResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~LytShaderResource();


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   シェーダーの登録
   *
   *
   * @li  レイアウトに設定されたメモリを使用する
   */
  //-----------------------------------------------------------------------------
  void SetData( void* pData, u32 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   GraphicsResourceの引き出し
   * @return  グラフィックリソース
   */
  //-----------------------------------------------------------------------------
  nw::lyt::GraphicsResource* GetGraphicsResource( void );

  void Setup(void );
  void Release(void );

  
private:

  void* mpData;
  u32 mSize;

  nw::lyt::GraphicsResource m_ShaderResource;   ///< RectDrawerShaderのリソース管理のクラス

};


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

#endif  // __GFL2_LYTSHADERRESOURCE_H__
