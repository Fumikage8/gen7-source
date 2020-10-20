//======================================================================
/**
 * @file FieldEffectLayout.h
 * @date 2015/07/14 16:40:58
 * @author miyachi_soichi
 * @brief フィールド用UIエフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_LAYOUT_H_INCLUDED__
#define __FIELD_EFFECT_LAYOUT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>

// appLib
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Util/app_util_2d.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

  /**
   * @class EffectLayout
   * @brief 汎用UIエフェクト
   */
  class EffectLayout : public app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN(EffectLayout);

  public:
    /**
     *  初期化構造体
     */
    struct SetupParam
    {
      const app::util::G2DUtil::LytResData *lytResTbl;
      const app::util::G2DUtil::LytwkData *lytDataTbl;
      u32 layoutNum;
      u32 resTblNum;
      u32 dataTblNum;
    };

  public:
    /**
     *  @brief  コンストラクタ
     *  @param  pHeap           ヒープ
     *  @param  pRenderManager  UI描画管理
     */
    EffectLayout( app::util::Heap *pHeap, app::util::AppRenderingManager *pRenderManager );

    /**
     *  @brief  デストラクタ
     */
    virtual ~EffectLayout( void );

    /**
     *  @brief  レイアウト作成
     *  @param  param   初期化用データ
     */
    void Initialize( const SetupParam &param, gfl2::str::MsgData *pMsgData = NULL, print::WordSet *pWordSet = NULL );

    /**
     *  @brief  レイアウト破棄
     */
    void Finalize( void );

    /**
     *  @brief  更新
     *  UiView側の更新処理：呼び出し元UiView::UpdateTree()
     */
    virtual void Update( void );

    /**
     *  @brief  描画
     *  UiView側の描画処理：呼び出し元UiView::DrawTree()
     */
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    /**
     *  @brief  アニメーション再生
     *  @param  anm           アニメ番号
     *  @param  auto_anm      オートアニメーションするかどうか
     *  @param  auto_unbind   終了時に自動でアンバインドするか
     */
    void StartAnimation( u32 anm, bool auto_anm, bool auto_unbind );

    /**
     *  @brief  アニメーション設定
     *  @param  anm   アニメ番号
     *  @param  frame フレーム番号
     */
    void SetAnimationFrame( u32 anm, f32 frame );


    /**
     *  @brief  アニメーションをとめる。
     *  @param  anm           アニメ番号
     */
    void StopAnimation( u32 anm );


    /**
     *  @brief  座標設定
     *  @param  idx   PaneIndex
     *  @param  pos   座標
     */
    void SetPosition( u32 idx, const gfl2::math::Vector3 *pos );

    /**
     *  @brief  描画切り替え
     *  @param  idx   PaneIndex
     *  @param  clr   色
     */
    void SetColor( u32 idx, const gfl2::math::Vector4 &clr );

    /**
     *  @brief  描画設定切り替え
     *  @param  flag  true:描画する/false:描画しない
     */
    virtual inline void SetVisible( bool flag ){ m_bVisible = flag; }
    /**
     *  @brief  描画優先度切り替え
     */
    inline void SetPriority( u32 prio ){ m_nPriority = prio; }

  private:
    app::util::AppRenderingManager *m_pAppRenderingManager;
    app::util::Heap *m_pAppHeap;

    u32   m_nPriority;
    bool  m_bVisible;
  };

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_LAYOUT_H_INCLUDED__