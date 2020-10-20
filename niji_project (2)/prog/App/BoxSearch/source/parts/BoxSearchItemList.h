#if !defined( __APP_BOX_SEARCH_ITEM_LIST_H_INCLUDED__ )
#define __APP_BOX_SEARCH_ITEM_LIST_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchItemList.h
 * @date    2016/02/01 17:08:01
 * @author  fukushima_yuya
 * @brief   ボックス検索：検索項目リスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchListBase.h"

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)
struct APP_PARAM;
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Savedata)
class BOX;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   ボックス検索：検索項目リスト
//------------------------------------------------------------------------------
class ItemList
  : public ListBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ItemList );

public:
  struct CreateParam
  {
    APP_PARAM*            appParam;
    DrawerBase*           base;
    MenuCursor*           cursor;
    Savedata::BOX*        boxSave;
    SearchListListener*   listener;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  ItemList( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~ItemList( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  void Terminate( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializeList( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   リストの生成
   */
  //------------------------------------------------------------------
  virtual void CreateList( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定
   *
   * @param   prm   [out]リストパラメータ
   */
  //------------------------------------------------------------------
  void SetListParam( app::tool::PaneList::SETUP_PARAM* prm );

  //------------------------------------------------------------------
  /**
   * @brief   タッチエリアの設定
   *
   * @param   prm   [in]リストパラメータ
   */
  //------------------------------------------------------------------
  void SetTouchArea( const app::tool::PaneList::SETUP_PARAM* prm );

  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定
   *
   * @param   pane
   */
  //------------------------------------------------------------------
  virtual void UpdateList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定
   *
   * @param   pane_index
   */
  //------------------------------------------------------------------
  void UpdateList_PokeName( u32 pane_index );       //!< ポケモン名
  void UpdateList_Type1( u32 pane_index );          //!< タイプ１
  void UpdateList_Type2( u32 pane_index );          //!< タイプ２
  void UpdateList_WazaName( u32 pane_index );       //!< 技名
  void UpdateList_WazaMachine( u32 pane_index );    //!< 技マシン
  void UpdateList_Tokusei( u32 pane_index );        //!< 特性
  void UpdateList_Seikaku( u32 pane_index );        //!< 性格
  void UpdateList_Gender( u32 pane_index );         //!< 性別
  void UpdateList_Item( u32 pane_index );           //!< もちもの
  void UpdateList_BoxMark( u32 pane_index );        //!< ボックスマーク
  void UpdateList_TeamEntry( u32 pane_index );      //!< チーム登録状態
  void UpdateList_TeamName( u32 pane_index );       //!< チーム名


public:
  //------------------------------------------------------------------
  /**
   * @brief   検索条件のリセット
   */
  //------------------------------------------------------------------
  void ResetSearchParam( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   パラメータのリセット
   */
  //------------------------------------------------------------------
  void ResetParameter( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストのリセット
   */
  //------------------------------------------------------------------
  void ResetPaneList( void );

  //------------------------------------------------------------------
  /**
   * @brief   補正
   */
  //------------------------------------------------------------------
  void AdjustValue( app::tool::PaneList::SETUP_PARAM* param, u32 cursorPos, u32 scrollVal );


public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   カーソル移動時のコールバック
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   カーソルを非表示にするコールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   リストの項目を描画するコールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   入れ替え用コールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ペインリスト決定時に呼ばれるリスナー
   */
  //------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos );


private:
  APP_PARAM*                  m_pAppParam;

  ListData                    m_ListData;

  Savedata::BOX*              m_pBoxSave;

  SearchListListener*         m_pListener;

private:
  gfl2::lyt::LytTextBox**     m_pContentsText;

  gfl2::lyt::LytPane**        m_pBoxMarkPane;
};

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

#endif // __APP_BOX_SEARCH_ITEM_LIST_H_INCLUDED__