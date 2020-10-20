#if !defined( __APP_BOX_SEARCH_BG_DRAW_H_INCLUDED__ )
#define __APP_BOX_SEARCH_BG_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchBGDraw.h
 * @date    2016/02/01 13:08:00
 * @author  fukushima_yuya
 * @brief   ボックス検索：背景
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   ボックス検索：背景
//------------------------------------------------------------------------------
class BoxSearchBGDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( BoxSearchBGDraw );

public:
  enum DispMode
  {
    DISP_MODE_UPPER,
    DISP_MODE_LOWER,
    DISP_MODE_NUM,
  };

public:
  // 初期化パラメータ
  struct InitParam
  {
    void*     lytBuff;
    DispMode  dispMode;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxSearchBGDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxSearchBGDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Init( InitParam& param );

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

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   BGアニメの開始
   *
   * @param   dispMode    ディスプレイ
   */
  //------------------------------------------------------------------
  void StartBGAnim( DispMode dispMode );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;
};

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

#endif // __APP_BOX_SEARCH_BG_DRAW_H_INCLUDED__