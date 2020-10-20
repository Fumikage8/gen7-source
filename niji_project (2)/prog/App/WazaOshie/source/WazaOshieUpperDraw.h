#ifndef NIJI_PROJECT_APP_WAZAOSHIE_UPPER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_UPPER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieUpperDraw.h
 * @date 2015/09/04 21:08:36
 * @author fukushima_yuya
 * @brief 上画面の描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/WazaOshie/source/WazaOshieConst.h>

#include <AppLib/include/Ui/UIView.h>


//-----------------------------------------------------------------------------
// @brief   前方宣言
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )
class DrawerBase;
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   技教え・技忘れ上画面描画クラス
//------------------------------------------------------------------------------
class WazaOshieUpperDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaOshieUpperDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  WazaOshieUpperDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~WazaOshieUpperDraw( void ) { ; }
  
public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   fadeMan       フェードマネージャ
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::Fade::FadeManager* fadeMan );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のオープン
   */
  //------------------------------------------------------------------
  void WazaOshieOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のクローズ
   */
  //------------------------------------------------------------------
  void WazaOshieClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

private:
  app::util::Heap*      m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*    m_pFadeManager;

  u8    m_Seq;

  DrawerBase*   m_pDrawerBase;
};

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_UPPER_DRAW_H_INCLUDED