#if !defined( __APP_BOX_SEARCH_LIST_BASE_H_INCLUDED__ )
#define __APP_BOX_SEARCH_LIST_BASE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchListBase.h
 * @date    2016/02/01 17:21:20
 * @author  fukushima_yuya
 * @brief   ボックス検索：リストのベースクラス
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
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   ボックス検索：検索リストのリスナー
//------------------------------------------------------------------------------
class SearchListListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   検索項目リストの表示
   */
  //------------------------------------------------------------------
  virtual void OpenItemList( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   検索内容リストの表示
   *
   * @param   srcID   検索項目ID
   */
  //------------------------------------------------------------------
  virtual void OpenContentsList( u32 srcID ) { ; }
};

//------------------------------------------------------------------------------
// @brief   ボックス検索：リストのベースクラス
//------------------------------------------------------------------------------
class ListBase
  : public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( ListBase );

protected:
  //! @brief  ボタンアニメ
  enum {
    BTN_ANIM_TOUCH = 0,
    BTN_ANIM_RELEASE,
    BTN_ANIM_CANCEL,
    BTN_ANIM_PASSIVE,
    BTN_ANIM_ACTIVE,
    BTN_ANIM_TOUCH_RELEASE,
    BTN_ANIM_NUM,
  };

protected:
  struct ListData
  {
    u32   index;    //!< 選択した項目インデックス
    u32   max;      //!< リストの最大数
    u32   curPos;   //!< カーソルの初期位置
    f32   scroll;   //!< スクロール数

    ListData( void )
      : index( 0 )
      , max( 0 )
      , curPos( 0 )
      , scroll( 0.f )
    { ; }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  ListBase( DrawerBase* base, MenuCursor* cursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~ListBase( void ) { ; }

protected:
  //------------------------------------------------------------------
  /**
   * @brief   リストの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializeList( void ) = 0;

public:
  //------------------------------------------------------------------
  /**
   * @brief   リストの生成
   */
  //------------------------------------------------------------------
  virtual void CreateList( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   リストの破棄
   */
  //------------------------------------------------------------------
  virtual void DeleteList( void );

protected:
  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定
   *
   * @param   pane
   */
  //------------------------------------------------------------------
  virtual void UpdateList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos ) = 0;

public:
  //------------------------------------------------------------------
  /**
   * @brief   スクロール値の計算
   *
   * @param   list_setup    リストのセットアップパラメータ
   *
   * @return  スクロール値
   */
  //------------------------------------------------------------------
  u32 GetScrollValue( app::tool::PaneList::SETUP_PARAM& list_setup );

  //------------------------------------------------------------------
  /**
   * @brief   スクロールバーのセットアップパラメータの取得(デフォルト)
   *
   * @return  app::util::ScrollBar::SETUP_PARAM
   */
  //------------------------------------------------------------------
  app::util::ScrollBar::SETUP_PARAM& GetDefaultScrollBarParam( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの入力有効フラグの設定
   *
   * @param   enable    入力有効フラグ
   */
  //------------------------------------------------------------------
  void SetInputEnable( bool enable );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの更新リザルトの取得
   *
   * @return  更新リザルト
   */
  //------------------------------------------------------------------
  app::tool::PaneList::Result GetPaneListUpdateResult( void ) const;

protected:
  DrawerBase*                       m_pBase;

  MenuCursor*                       m_pCursor;

  app::tool::PaneListView*          m_pPaneListView;

  app::tool::PaneList::PANE_DATA*   m_pPaneData;

  app::util::ScrollBar::SETUP_PARAM m_ScrollBarSetup;
};

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

#endif // __APP_BOX_SEARCH_LIST_BASE_H_INCLUDED__
