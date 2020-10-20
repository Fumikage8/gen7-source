//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_LayoutRenderPath.h
 *  @brief  レイアウト描画パス
 *  @author kawada_koji@gamefreak.co.jp
 *  @date   2015.01.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef GFLIB2_RENDERINGENGINE_RENDERER_LAYOUTRENDERPATH_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_LAYOUTRENDERPATH_H_INCLUDED
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <layout/include/gfl2_LytSys.h>
#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)
class IGLAllocator;
GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

static const u8 ORDER_FIRST  = 0;     //!< 描画順：最初
static const u8 ORDER_NORMAL = 128;   //!< 描画順：普通
static const u8 ORDER_LAST   = 255;   //!< 描画順：最後

using gfl2::renderingengine::renderer::RenderingPipeLine;

class LayoutRenderPath : public RenderingPipeLine::IRenderPath
{
  friend class lyt::LytWk;
  
public:
  LayoutRenderPath( gfl2::gfx::IGLAllocator* pGLAllocator );
  virtual ~LayoutRenderPath(void);
  
  //! @brief  初期化
  //! @note   登録できるレイアウトワーク最大数を設定
  void Init( gfl2::heap::HeapBase* heap, u32 lytWkMax );

  //! @brief  レンダーターゲットの設定
  //! @note   NULLのときはExecuteに渡されたDrawContextから取得する
  void SetRenderTarget( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );

  //! @brief  描画するLwtWkのリストをクリアする
  //! @note   毎フレーム呼んでクリアするのが普通の使い方です
  void ClearDrawableLytWk(void);
  
public:
  //! @name  friendから呼ぶ
  //@{
  
  //! @brief  描画するLytWkを追加する
  //! @note   LytWkのDraw(LyoutRenderPath* pLayoutRenderPath)から呼ぶ。
  //          pLayoutRenderPathは上画面用、下画面用、テクスチャに描き込む用、という風に用途ごとに要る。
  //          上両画面用のStereoLayoutRenderPathが必要になるかもしれない。
  //          レイアウト以外(3Dモデルやエフェクト)との描画の前後関係はRenderingPipeLineでpLayoutRenderPathをいつ呼ぶかで制御するしかない。
  void AddDrawableLytWk( lyt::LytWk* pLytWk, gfl2::lyt::DisplayType lod, u8 order = ORDER_NORMAL, Env::CameraType cameraType = Env::CAMERA_NORMAL );

  //@}

public:
  //! @name  IRenderPathのメンバ関数のオーバーライド
  //@{

  virtual void Execute(const RenderingPipeLine::DrawContext& rDrawContext);

  //@}

private:
  //! @brief  描画の前と後に呼び出す
  //@{

  void BeginDraw(const RenderingPipeLine::DrawContext& rDrawContext);
  void EndDraw(const RenderingPipeLine::DrawContext& rDrawContext);

  //@}

  //! @brief  登録位置の取得
  //@{

  u32 GetAddIndex( u8 order );

  //@}

private:
  enum
  {
    LYT_WK_NUM_MAX = 64
  };

  gfx::RasterizerStateObject*      m_pRasterizerStateObject;
  gfx::BlendStateObject*          m_pBlendStateObject;

  const gfx::Surface*    m_pRenderTarget;
  const gfx::Surface*    m_pDepthStencil;

  lyt::LytWk*       m_pLytWkList[LYT_WK_NUM_MAX];
  lyt::DisplayType  m_eDispType[LYT_WK_NUM_MAX];
  Env::CameraType   m_eCameraType[LYT_WK_NUM_MAX];
  u8                m_eOrderList[LYT_WK_NUM_MAX];
  //lyt::LytWk**      m_pLytWkList;
  //lyt::DisplayType* m_eDispType;
  //Env::CameraType*  m_eCameraType;
  u32               m_lytWkNum;
  u32               m_lytWkMaxNum;
};

GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

#endif // GFLIB2_RENDERINGENGINE_RENDERER_LAYOUTRENDERPATH_H_INCLUDED
