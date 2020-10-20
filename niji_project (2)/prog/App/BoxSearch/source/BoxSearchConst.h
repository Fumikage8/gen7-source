#ifndef NIJI_PROJECT_APP_BOX_SEARCH_CONST_H_INCLUDED
#define NIJI_PROJECT_APP_BOX_SEARCH_CONST_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file BoxSearchConst.h
 * @date 2015/07/27 12:58:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_LytSys.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//! @brief  画面モード
enum DispMode
{
  DISP_MODE_TOP,    //!< 検索トップ画面
  DISP_MODE_COND,   //!< 検索条件設定画面
  DISP_MODE_NUM,
};

//! @brief  上画面メッセージID
enum UppMsgID
{
  UMSG_POKENAME_INITIALS,       //!< ポケモン名：頭文字選択
  UMSG_POKENAME_NAMESELECT,     //!< ポケモン名：名前選択
  UMSG_TYPE_1,                  //!< タイプ１
  UMSG_TYPE_2,                  //!< タイプ２
  UMSG_WAZA_INITIALS,           //!< わざ名：頭文字選択
  UMSG_WAZA_NAMESELECT,         //!< わざ名：名前選択
  UMSG_WAZAMACHINE,             //!< わざマシン
  UMSG_TOKUSEI_INITIALS,        //!< とくせい：頭文字選択
  UMSG_TOKUSEI_NAMESELECT,      //!< とくせい：名前選択
  UMSG_SEIKAKU,                 //!< せいかく
  UMSG_GENDER,                  //!< せいべつ
  UMSG_ITEM,                    //!< もちもの
  UMSG_MARK,                    //!< ボックスマーク
//  UMSG_TEAM_ENTRY,              //!< チームに登録しているポケモン  //!< 仕様削除20160222
  UMSG_SELECT_TEAM_ENTRY,       //!< 選択したチームに登録しているポケモン
  UMSG_NUM,
  UMSG_NULL,
};

//! @brief  検索項目ID
enum {
  SRCID_POKE_NAME,     //!< ポケモン名
  SRCID_TYPE_1,        //!< タイプ１
  SRCID_TYPE_2,        //!< タイプ２
  SRCID_WAZA_NAME,     //!< 技名
  SRCID_WAZAMACHINE,   //!< 技マシン
  SRCID_TOKUSEI,       //!< 特性
  SRCID_SEIKAKU,       //!< 性格
  SRCID_GENDER,        //!< 性別
  SRCID_ITEM,          //!< 持ち物
  SRCID_MARK,          //!< マーク
  //SRCID_ENTRY,         //!< 登録  //!< 仕様削除20160222
  SRCID_TEAM,          //!< チーム
  SRCID_NUM,
};

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_BOX_SEARCH_CONST_H_INCLUDED