#ifndef NIJI_PROJECT_APP_HERO_SELECT_LOWER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_HERO_SELECT_LOWER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectLowerDraw.h
 * @date 2015/08/26 12:26:11
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )
class DrawerBase;
GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   容姿・性別選択下画面描画
//------------------------------------------------------------------------------
class HeroSelectLowerDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( HeroSelectLowerDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  HeroSelectLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~HeroSelectLowerDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   msgData         メッセージデータ
   * @param   wordSet         ワードセット
   */
  //------------------------------------------------------------------
  void Init( void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

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
   * @param   pAppLytBuff     レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*          m_pFadeManager;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

private:
  DrawerBase*   m_pDrawerBase;

  u8            m_Seq;
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_HERO_SELECT_LOWER_DRAW_H_INCLUDED