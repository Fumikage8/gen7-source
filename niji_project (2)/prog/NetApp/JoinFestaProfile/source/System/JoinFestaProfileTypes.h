// ============================================================================
/*
 * @file JoinFestaProfileTypes.h
 * @brief プロフィールアプリで使用する型の定義ファイルです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILETYPES_H_INCLUDE )
#define JOINFESTAPROFILETYPES_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


struct EditMenuInfo
{
  u32   editMenuID;              // 自身のID
  u32   editMenuMessageID;       // 編集項目のメッセージID

  u32   selectMessageID;         // 編集項目を選択した時のメッセージID
  u32   selectMenuID0;           // 3択メニューのメッセージID
  u32   selectMenuID1;           // 3択メニューのメッセージID
  u32   selectMenuID2;           // 3択メニューのメッセージID

  u32   selectMenuMessageID0;    // 3択メニューを選択した時のメッセージID
  u32   selectMenuMessageID1;    // 3択メニューを選択した時のメッセージID

  u32   descriptionMessageID0;   // 説明文のメッセージID
  u32   descriptionMessageID1;   // 説明文のメッセージID
  u32   descriptionMessageID2;   // 説明文のメッセージID

  u32   descriptionPageMax;      // 説明文のページ数

  EditMenuInfo() :
    editMenuID( NetApp::JoinFestaProfile::EDIT_MENU_ID_MAX ),
    editMenuMessageID( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),

    selectMessageID( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),
    selectMenuID0( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),
    selectMenuID1( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),
    selectMenuID2( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),

    selectMenuMessageID0( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),
    selectMenuMessageID1( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),

    descriptionMessageID0( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),
    descriptionMessageID1( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),
    descriptionMessageID2( NetApp::JoinFestaProfile::INVALID_MESSAGE_ID ),

    descriptionPageMax(0)
  {
  }

  EditMenuInfo(
    u32 _editMenuID,
    u32 _editMenuMessageID,
    
    u32 _selectMessageID,
    u32 _selectMenuID0,
    u32 _selectMenuID1,
    u32 _selectMenuID2,

    u32 _selectMenuMessageID0,
    u32 _selectMenuMessageID1,

    u32 _descriptionMessageID0,
    u32 _descriptionMessageID1,
    u32 _descriptionMessageID2,

    u32 _descriptionPageMax ) :

    editMenuID( _editMenuID ),
    editMenuMessageID( _editMenuMessageID ),

    selectMessageID( _selectMessageID ),
    selectMenuID0( _selectMenuID0 ),
    selectMenuID1( _selectMenuID1 ),
    selectMenuID2( _selectMenuID2 ),

    selectMenuMessageID0( _selectMenuMessageID0 ),
    selectMenuMessageID1( _selectMenuMessageID1 ),

    descriptionMessageID0( _descriptionMessageID0 ),
    descriptionMessageID1( _descriptionMessageID1 ),
    descriptionMessageID2( _descriptionMessageID2 ),

    descriptionPageMax( _descriptionPageMax )
  {
  }

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILETYPES_H_INCLUDE
