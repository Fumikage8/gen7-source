#if !defined( __APP_FINDER_MENU_BG_DRAW_H_INCLUDED__ )
#define __APP_FINDER_MENU_BG_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderMenuBGDraw.h
 * @date    2016/01/14 17:56:17
 * @author  fukushima_yuya
 * @brief   ポケファインダーメニュー：背景
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class DrawerBase;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   ポケファインダーメニュー：背景
//------------------------------------------------------------------------------
class FinderMenuBGDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuBGDraw );

public:
  struct CreateParam
  {
    app::util::Heap*                    heap;
    app::util::AppRenderingManager*     renderMan;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuBGDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuBGDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppLytBuff   レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Init( void* pAppLytBuff );

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
  bool IsExit( void ) { return true; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

private:
  app::util::Heap*                    m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*     m_pAppRenderingManager;

private:
  DrawerBase*       m_pDrawerBase;
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __APP_FINDER_MENU_BG_DRAW_H_INCLUDED__