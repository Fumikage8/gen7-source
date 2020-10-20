#if !defined( __RESORT_PARTS_PANE_LIST_H__ )
#define __RESORT_PARTS_PANE_LIST_H__
#pragma once
//======================================================================
/**
 * @file    ResortPartsPaneList.h
 * @date    2015/10/02 15:08:20
 * @author  fukushima_yuya
 * @brief   ポケリゾート：ペインリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/app_tool_PaneListView.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
class DrawerBase;
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
// @brief   ペインリスト
//------------------------------------------------------------------------------
class ResortPaneList
{
  GFL_FORBID_COPY_AND_ASSIGN( ResortPaneList );

public:
  struct SETUP_PARAM
  {
    u32   paneMax;
    u32   listMax;
    u32   viewCount;

    u32   listStartPane;
    f32   defPY;
    f32   paneWidth;
    f32   paneHeight;

    u32   scrollPane;

    u32   moveSE;
    u32   decideSE;
    u32   cancelSE;

    SETUP_PARAM( void )
      : paneMax( 0 )
      , listMax( 0 )
      , viewCount( 0 )
      , listStartPane( 0 )
      , defPY( 0.0f )
      , paneWidth( 0.0f )
      , paneHeight( 0.0f )
      , scrollPane( 0 )
      , moveSE(app::tool::PaneList::SE_NONE)
      , decideSE(app::tool::PaneList::SE_NONE)
      , cancelSE(app::tool::PaneList::SE_NONE)
    { ; }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ResortPaneList( DrawerBase* base, app::tool::PaneList::CallBack* callBack, app::tool::PaneListView::Listener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ResortPaneList( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの生成
   * @param   setupParam  セットアップパラメータ
   */
  //------------------------------------------------------------------
  void Create( SETUP_PARAM& setupParam, app::tool::PaneList::PANE_DATA* paneDataTbl );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの破棄
   */
  //------------------------------------------------------------------
  void Delete( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータのセット
   * @param   param       [out]リストパラメータ
   * @param   setupParam  [in]セットアップパラメータ
   */
  //------------------------------------------------------------------
  void SetParamList( app::tool::PaneList::SETUP_PARAM* param, SETUP_PARAM& setupParam );
  
public:
  //------------------------------------------------------------------
  /**
   * @brief   PaneListのリセット
   *
   * @param   setupParam  セットアップパラメータ
   */
  //------------------------------------------------------------------
  void ResetPaneList( SETUP_PARAM& setupParam );

public:
  //------------------------------------------------------------------
  /**
   * @brief   入力の有効設定
   * @param   enable    有効フラグ
   */
  //------------------------------------------------------------------
  void SetInputEnable( bool enable );

  //------------------------------------------------------------------
  /**
   * @brief   PaneListViewの取得
   * @return  PaneListViewポインタ
   */
  //------------------------------------------------------------------
  app::tool::PaneListView* GetPaneListView( void ) const;


private:
  DrawerBase*                           m_pBase;

  app::tool::PaneListView*              m_pPaneListView;

  app::tool::PaneList::CallBack*        m_pCallBack;

  app::tool::PaneList::PANE_DATA*       m_pPaneDataTbl;

private:
  bool    m_isCreated;
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_PARTS_PANE_LIST_H__
