#if !defined(__APP_FINDER_ROTOM_FADE_DRAW_H_INCLUDED__)
#define __APP_FINDER_ROTOM_FADE_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderRotomFadeDraw.h
 * @date    2016/01/12 17:29:57
 * @author  fukushima_yuya
 * @brief   ポケファインダー：ロトムフェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <AppLib/include/ui/UIView.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )
struct APP_PARAM;
class DrawerBase;
GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   ポケファインダー：ロトムフェード
//------------------------------------------------------------------------------
class FinderRotomFade
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderRotomFade );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderRotomFade( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderRotomFade( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff );

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
   *
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void ) { return true; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

public:
  //------------------------------------------------------------------
  /**
   * @brief   アニメの開始
   */
  //------------------------------------------------------------------
  void StartRotomFade( void );

  //------------------------------------------------------------------
  /**
   * @brief   アニメの終了
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsEndRotomFade( void );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  APP_PARAM*                        m_pAppParam;

  u8                                m_Seq;

private:
  enum {
    DRAW_UPP,
    DRAW_LOW,
    DRAW_NUM,
  };
  DrawerBase*     m_pDrawerBase[DRAW_NUM];
};

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

#endif // __APP_FINDER_ROTOM_FADE_DRAW_H_INCLUDED__