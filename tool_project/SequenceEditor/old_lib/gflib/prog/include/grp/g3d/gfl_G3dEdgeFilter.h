#ifndef __GFL_G3D_EDGEFILTER_H__
#define __GFL_G3D_EDGEFILTER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3deDGEfILTER.h
 *	@brief  エッジフィルター
 *	@author	Masateru Ishiguro
 *	@date		2011.04.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dPostEffectSystem.h>

#include <base/gfl_KeyContainer.h>




namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	エッジフィルタークラス
//=====================================
class G3DEdgeFilter
{
  GFL_FORBID_COPY_AND_ASSIGN(G3DEdgeFilter);
public:
  
  enum EdgeType
  {
    DEFAULT_EDGE = 0,   //! デフォルトエッジ
    VCOLOR_EDGE,        //! 頂点カラーエッジ
    ID_EDGE,            //! ＩＤエッジ
    NORMAL_EDGE,        //! 法線エッジ
#if GFL_DEBUG
    DEBUG_RGB,          //! デバッグ用
    DEBUG_A,            //! デバッグ用
    DEBUG_R,            //! デバッグ用
#endif
    INSIDE_EDGE,        //! 内側エッジ
    DOUBLE_LAYERED_EDGE,//! ２層エッジ(デフォルトエッジ+内側エッジ)
    TYPE_MAX
  };
  
  static const u32          MAX_PATH = 3;
  
  #define   TEXTURE_WIDTH   (512.0f)
  #define   TEXTURE_HEIGHT  (256.0f)

  //オフスクリーンバッファは縦長
  #define   OFFSCREAN_WIDTH   (400.0f)
  #define   OFFSCREAN_HEIGHT  (240.0f)
  #define   LOW_OFFSCREAN_WIDTH   (320.0f)
  #define   LOW_OFFSCREAN_HEIGHT  (240.0f)

  G3DEdgeFilter(void);

  ~G3DEdgeFilter();

  /**
   * @brief ワーク確保
   * @param heap_allocator      メモリ確保先
   * @param device_allocator    メモリ確保先
   */
  void CreateWork( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator );

  /**
   * @brief エッジワーク破棄
   */
  void DestroyWork(void);

  /**
   * @beirf エッジ生成に用いるテクスチャ設定
   * @param texture   テクスチャ
   */
  void SetTexture( nw::gfx::res::ResTexture texture, s32 layerNo = -1 );
  void SetColorTarget( nw::gfx::res::ResTexture target );
  void CreateDepthTarget( gfl::heap::NwAllocator* heap_allocator, const gfl::grp::GraphicsSystem* graphics_system );
  
  /**
   * @brief エッジの幅設定
   * @param scale   スケール
   */
  void ScaleEdgeWidth( f32 scale = 1.0f, gfl::grp::DisplayType type = gfl::grp::DISPLAY_UPPER );
  void ScaleEdgeWidth( f32 screanWidth, f32 screanHeight, f32 scale = 1.0f );
  
  /**
   * @brief ディスプレイタイプの設定
   * @param type   ディスプレイタイプ
   */
  void SetDisplayType( gfl::grp::DisplayType type );
  
  /**
   * @brief エッジのカラー調整
   * @param r   赤
   * @param g   緑
   * @param b   青
   * @param a   ＩＤカラー
   */
  void SetEdgeColor( f32 r, f32 g, f32 b, f32 a = 0.0f );
  
  /**
   * @brief エッジ描画用モデル（４頂点プレーン）取得
   * @return モデル
   */
  gfl::grp::g3d::Model* GetShaderModel(s32 type, s32 path = 0)
  {
    return m_pEdgeModel[type][path];
  }
  
  void SetEdgeType( EdgeType type );
  
  EdgeType GetEdgeType()
  {
    return m_Type;
  }

private:
  
  
  gfl::grp::g3d::Resource         m_Resource[TYPE_MAX];
  gfl::grp::g3d::Model            *m_pEdgeModel[TYPE_MAX][MAX_PATH];
  nw::gfx::res::ResTexture        m_ColorTarget;
  nw::gfx::res::ResImageTexture   m_DepthTarget;
  EdgeType                        m_Type;
  f32                             m_Scale;

};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DEDGESYSTEM_H__
