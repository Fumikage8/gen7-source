#if !defined( __APP_BOX_SEARCH_CONTENTS_LIST_H_INCLUDED__ )
#define __APP_BOX_SEARCH_CONTENTS_LIST_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchContentsList.h
 * @date    2016/02/01 17:08:31
 * @author  fukushima_yuya
 * @brief   ボックス検索：検索内容リスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchListBase.h"

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
class SortData;
class PokeIcon;
class BoxSearchDrawListener;
GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Savedata)
class BOX;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   ボックス検索：検索内容リスト
//------------------------------------------------------------------------------
class ContentsList
  : public ListBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ContentsList );

public:
  enum Hierarchy
  {
    HIE_CONSONANT,    //!< 「あかさたな・・・」子音リスト
    HIE_VOWEL,        //!< 「あいうえお」母音リスト
    HIE_CONTENTS,     //!< 内容リスト
    HIE_NUM,
  };

public:
  struct CreateParam
  {
    APP_PARAM*              appParam;
    DrawerBase*             base;
    MenuCursor*             cursor;
    Savedata::BOX*          boxSave;
    gfl2::str::MsgData*     msgData_Initial;
    SearchListListener*     listener;
    BoxSearchDrawListener*  drawListener;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  ContentsList( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~ContentsList( void ) { ; }

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

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ソートデータの初期化
   */
  //------------------------------------------------------------------
  void InitializeSortData( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの初期化
   */
  //------------------------------------------------------------------
  void InitializePokeIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   技マシン情報の初期化
   */
  //------------------------------------------------------------------
  void InitializeWazaMachine( void );

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

  //------------------------------------------------------------------
  /**
   * @brief   リストの破棄
   */
  //------------------------------------------------------------------
  virtual void DeleteList( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定
   *
   * @param   prm       [out]リストパラメータ
   * @param   listMax   [in]リスト最大数
   */
  //------------------------------------------------------------------
  void SetListParam( app::tool::PaneList::SETUP_PARAM* prm, u32 listMax );

public:
  //------------------------------------------------------------------
  /**
   * @brief   次の階層へ
   */
  //------------------------------------------------------------------
  void NextHierarchy( void );

  //------------------------------------------------------------------
  /**
   * @brief   前の階層へ
   */
  //------------------------------------------------------------------
  void PrevHierarchy( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力有効フラグの生成
   *
   * @param   listMax   項目最大数
   */
  //------------------------------------------------------------------
  void CreateListBtnEnable( u32 listMax );

  //------------------------------------------------------------------
  /**
   * @brief   ボタン入力設定
   *
   * @param   listMax   項目最大数
   */
  //------------------------------------------------------------------
  void SetListBtnEnable_Consonant( u32 listMax );
  void SetListBtnEnable_Vowel( u32 listMax );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定
   *
   * @param   pane          ペインデータ
   * @param   pane_index    ペインインデックス
   * @param   pos           データインデックス
   */
  //------------------------------------------------------------------
  virtual void UpdateList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定(階層)
   */
  //------------------------------------------------------------------
  void UpdateList_Consonant( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Vowel( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Contents( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータの設定(内容リスト)
   */
  //------------------------------------------------------------------
  void UpdateList_PokeName( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Type( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_WazaName( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_WazaMachine( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Tokusei( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Seikaku( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Gender( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_Item( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_BoxMark( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_TeamEntry( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void UpdateList_TeamName( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタン有効状態のセットアップ
   */
  //------------------------------------------------------------------
  void SetupList_BtnEnable( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   ボタン有効状態のセットアップ解除
   */
  //------------------------------------------------------------------
  void UnsetupList_BtnEnable( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストの最大数の取得
   *
   * @return  リスト数
   */
  //------------------------------------------------------------------
  u32 GetListMax( void );

  //------------------------------------------------------------------
  /**
   * @brief   リストの最大数の取得
   *
   * @return  リスト数
   */
  //------------------------------------------------------------------
  u32 GetListMax_Consonant( void );
  u32 GetListMax_Vowel( void );
  u32 GetListMax_Contents( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リストの最大数の取得(内容リスト数)
   *
   * @param   index   母音インデックス
   *
   * @return  リスト数
   */
  //------------------------------------------------------------------
  u32 GetListMax_VowelNum( u32 index );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ソートデータ項目数の設定
   */
  //------------------------------------------------------------------
  void SetSortDataItemMax( void );

  //------------------------------------------------------------------
  /**
   * @brief   ソートデータの生成
   */
  //------------------------------------------------------------------
  void CreateSortDataContents( void );

  //------------------------------------------------------------------
  /**
   * @brief   ソートデータの破棄
   */
  //------------------------------------------------------------------
  void DeleteSortDataContents( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   言語ごとの頭文字リストの取得
   *
   * @return  頭文字リスト
   */
  //------------------------------------------------------------------
  const u32* GetVowelList( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   頭文字の文字列を取得
   *
   * @param   textBox   テキストボックス
   * @param   msgID     メッセージID
   */
  //------------------------------------------------------------------
  void SetInitial( gfl2::lyt::LytTextBox* textBox, u32 msgID );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークペインの取得
   *
   * @param   pane        ペインデータ
   * @param   pane_index  ペインインデックス
   * @param   pos         データインデックス
   *
   * @return  gfl2::lyt::LytPane*
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytPane* GetBoxMarkPane( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );


private:
  //------------------------------------------------------------------
  /**
   * @brief   最初の階層を取得
   *
   * @return  階層
   */
  //------------------------------------------------------------------
  u32 GetFirstHierarchy( void );

  //------------------------------------------------------------------
  /**
   * @brief   次の階層へ移動できるか？
   *
   * @return  "true  = 移動可"
   * @return  "false = 移動不可"
   */
  //------------------------------------------------------------------
  bool IsNextHierarchy( void );

  //------------------------------------------------------------------
  /**
   * @brief   階層のスキップ
   *
   * @note    日本語の英数字選択がされた時のみ
   *
   * @return  "true  = スキップあり"
   * @return  "false = スキップなし"
   */
  //------------------------------------------------------------------
  bool SkipHierarchy( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   検索条件の更新
   */
  //------------------------------------------------------------------
  void SetSearchParam( void );

  //------------------------------------------------------------------
  /**
   * @brief   検索条件の更新
   */
  //------------------------------------------------------------------
  void SetSearchParam_PokeName( u32 val );
  void SetSearchParam_Type1( u32 val );
  void SetSearchParam_Type2( u32 val );
  void SetSearchParam_WazaName( u32 val );
  void SetSearchParam_WazaMachine( u32 val );
  void SetSearchParam_Tokusei( u32 val );
  void SetSearchParam_Siekaku( u32 val );
  void SetSearchParam_Gender( u32 val );
  void SetSearchParam_Item( u32 val );
  void SetSearchParam_Mark( u32 val );
  void SetSearchParam_TeamEntry( u32 val );
  void SetSearchParam_TeamName( u32 val );

private:
  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージのセット
   *
   * @param   pos   リストインデックス
   */
  //------------------------------------------------------------------
  void SetUpperMessage( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージのセット
   *
   * @param   pos   リストインデックス
   */
  //------------------------------------------------------------------
  void SetUpperMessage_Consonant( u32 pos );
  void SetUpperMessage_Vowel( u32 pos );
  void SetUpperMessage_Contents( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージのセット
   *
   * @param   pos   リストインデックス
   */
  //------------------------------------------------------------------
  void SetUpperMessage_PokeName( u32 pos );
  void SetUpperMessage_Type1( u32 pos );
  void SetUpperMessage_Type2( u32 pos );
  void SetUpperMessage_WazaName( u32 pos );
  void SetUpperMessage_WazaMachine( u32 pos );
  void SetUpperMessage_Tokusei( u32 pos );
  void SetUpperMessage_Seikaku( u32 pos );
  void SetUpperMessage_Gender( u32 pos );
  void SetUpperMessage_Item( u32 pos );
  void SetUpperMessage_Mark( u32 pos );
  void SetUpperMessage_TeamEntry( u32 pos );
  void SetUpperMessage_TeamName( u32 pos );

public:
  //------------------------------------------------------------------
  /**
   * @brief   リスト項目IDの設定
   *
   * @param   srcID   リスト項目ID
   */
  //------------------------------------------------------------------
  void SetItemListID( u32 srcID );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   カーソル移動時のコールバック
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );

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

  Savedata::BOX*              m_pBoxSave;

  gfl2::str::MsgData*         m_pMsgDataInitial;

  SearchListListener*         m_pListener;

  BoxSearchDrawListener*      m_uiListener;

  GameSys::GameManager*       m_pGameMan;

  GameSys::GameData*          m_pGameData;

private:
  SortData*           m_pSortData;  //!< ソートデータ

  PokeIcon*           m_pPokeIcon;  //!< ポケモンアイコン

private:
  ListData  m_ListData[HIE_NUM];

  u32       m_ListID;

  u32       m_Hierarchy;

  u8        m_Language;

private:
  u32       m_HaveWazaMachineNum;

  u16*      m_pHaveWazaMachine;

private:
  bool*     m_pListBtnEnable;

private:
  gfl2::lyt::LytParts**       m_pBtnBaseParts;

  gfl2::lyt::LytTextBox**     m_pPokeNameText;

  gfl2::lyt::LytPane**        m_pPokeIconPane;
};

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

#endif // __APP_BOX_SEARCH_CONTENTS_LIST_H_INCLUDED__