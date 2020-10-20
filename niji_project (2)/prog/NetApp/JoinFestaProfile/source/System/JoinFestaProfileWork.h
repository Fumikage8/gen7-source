// ============================================================================
/*
 * @file JoinFestaProfileWork.h
 * @brief プロフィールアプリのワーククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_WORK_H_INCLUDE )
#define JOINFESTAPROFILE_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileDefine.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileTypes.h"
#include <niji_conv_header/message/msg_jf_myprofile.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaProfileWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaProfile::JOINFESTAPROFILE_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaProfile::JOINFESTAPROFILE_APP_DEVICE_HEAP_SIZE;

public:

  typedef struct
  {
    u32       pos;
    f32       scroll;
  } PaneListCursorInfo;

  enum DescriptionType
  {
    DESCRIPTION_TYPE_FIRST_BOOT_DESCRIPTION,
    DESCRIPTION_TYPE_MENU_DESCRIPTION
  };

public:

  JoinFestaProfileWork() :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
    m_EditMenuList( GetDeviceHeap(),  NetApp::JoinFestaProfile::EDIT_MENU_ID_MAX ),
    m_pSelectEditMenu( NULL ),
    m_LikePokemonName( 128, GetDeviceHeap() ),
    m_HonorString( 128, GetDeviceHeap() ),
    m_JoinFestaName( 128, GetDeviceHeap() ),
    m_bFadeInRequest( false ),
    m_eBootProcID( NetApp::JoinFestaProfile::BOOT_PROC_ID_NONE ),
    m_PaneListCursorInfo(),
    m_eDescriptionType( DESCRIPTION_TYPE_FIRST_BOOT_DESCRIPTION ),
    m_bDescriptionOpenAnimeFlag( true )
  {
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_MEETING_MESSAGE,   msg_jf_myprofile_sel_01, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 出会いの挨拶
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_GOODBY_MESSAGE,    msg_jf_myprofile_sel_02, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 別れの挨拶
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_THROB,             msg_jf_myprofile_sel_03, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 感動した時
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_REGRETFUL,         msg_jf_myprofile_sel_04, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 残念な時
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_MY_SECRET,         msg_jf_myprofile_sel_05, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 私の秘密
  #if defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_LIKE_POKEMON,      msg_jf_myprofile_sel_06, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 好きなポケモン
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_HONOR,             msg_jf_myprofile_sel_07, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 肩書き
  #endif // defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )

    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

    if( JoinFestaScript::REWARD_STATE_FINISH == pJoinFestaDataSave->GetRewardState( JoinFestaScript::REWARD_NAMING ) )
    {
      m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_NAME,              msg_jf_myprofile_sel_08, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_01, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // ジョインフェスタの名前
    }

    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_BATTLE,            msg_jf_myprofile_sel_14, msg_jf_myprofile_win_10, msg_jf_myprofile_sel_20, msg_jf_myprofile_sel_21, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_11, msg_jf_myprofile_win_12, msg_jf_myprofile_exp_03, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 対戦の申し込み
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_TRADE,             msg_jf_myprofile_sel_15, msg_jf_myprofile_win_13, msg_jf_myprofile_sel_20, msg_jf_myprofile_sel_21, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_14, msg_jf_myprofile_win_15, msg_jf_myprofile_exp_04, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // 交換の申し込み

    if( JoinFestaScript::REWARD_STATE_FINISH == pJoinFestaDataSave->GetRewardState( JoinFestaScript::REWARD_ATTRACTION ) )
    {
      m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_ATTRACTION,        msg_jf_myprofile_sel_16, msg_jf_myprofile_win_16, msg_jf_myprofile_sel_22, msg_jf_myprofile_sel_21, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_17, msg_jf_myprofile_win_18, msg_jf_myprofile_exp_05, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 1 ) ); // アトラクションへの意気込み
    }

    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_FRIEND,            msg_jf_myprofile_sel_19, msg_jf_myprofile_win_01, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_08, msg_jf_myprofile_win_09, msg_jf_myprofile_exp_01, msg_jf_myprofile_exp_02,                      NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 2 ) ); // フレンドとの通信
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_VIP,               msg_jf_myprofile_sel_18, msg_jf_myprofile_win_03, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_04, msg_jf_myprofile_win_05, msg_jf_myprofile_exp_06, msg_jf_myprofile_exp_07,                      NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 2 ) ); // VIPとの通信
    m_EditMenuList.PushBack( NetApp::JoinFestaProfile::System::EditMenuInfo( EDIT_MENU_ID_GUEST,             msg_jf_myprofile_sel_17, msg_jf_myprofile_win_02, msg_jf_myprofile_sel_11, msg_jf_myprofile_sel_12, msg_jf_myprofile_sel_13, msg_jf_myprofile_win_06, msg_jf_myprofile_win_07, msg_jf_myprofile_exp_09, msg_jf_myprofile_exp_10,                      NetApp::JoinFestaProfile::INVALID_MESSAGE_ID, 2 ) ); // ゲストとの通信

    gfl2::std::MemClear( &m_PaneListCursorInfo, sizeof( PaneListCursorInfo ) );
  }

  virtual~JoinFestaProfileWork()
  {
  }

  gfl2::util::List<NetApp::JoinFestaProfile::System::EditMenuInfo>& GetEditMenuList() { return m_EditMenuList; }

  NetApp::JoinFestaProfile::System::EditMenuInfo* GetSelectEditMenu() { return m_pSelectEditMenu; }
  void SetSelectEditMenu( NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu ) { m_pSelectEditMenu = pSelectEditMenu; }

  gfl2::str::StrBuf& GetLikePokemonName() { return m_LikePokemonName; }
  gfl2::str::StrBuf& GetHonorString() { return m_HonorString; }
  gfl2::str::StrBuf& GetJoinFestaName() { return m_JoinFestaName; }

  void FadeInRequest() { m_bFadeInRequest = true; }
  void ResetFadeInRequest() { m_bFadeInRequest = false; }
  bool IsFadeInRequest() { return m_bFadeInRequest; }

  NetApp::JoinFestaProfile::BootProcID GetBootProcID(){ return m_eBootProcID; }
  void SetBootProcID( NetApp::JoinFestaProfile::BootProcID eBootProcID ){ m_eBootProcID = eBootProcID; }

  PaneListCursorInfo& GetPaneListCursorInfo() { return m_PaneListCursorInfo; }

  void SetDescriptionType( DescriptionType eDescriptionType ) { m_eDescriptionType = eDescriptionType; }
  DescriptionType GetDescriptionType() { return m_eDescriptionType; }

  void SetDescriptionOpenAnimeFlag( bool bFlag ) { m_bDescriptionOpenAnimeFlag = bFlag; }
  bool GetDescriptionOpenAnimeFlag() { return m_bDescriptionOpenAnimeFlag; }


private:

  gfl2::util::List<NetApp::JoinFestaProfile::System::EditMenuInfo>    m_EditMenuList;

  NetApp::JoinFestaProfile::System::EditMenuInfo*                     m_pSelectEditMenu;

  gfl2::str::StrBuf                                                   m_LikePokemonName;
  gfl2::str::StrBuf                                                   m_HonorString;
  gfl2::str::StrBuf                                                   m_JoinFestaName;

  bool                                                                m_bFadeInRequest;

  NetApp::JoinFestaProfile::BootProcID                                m_eBootProcID;

  PaneListCursorInfo                                                  m_PaneListCursorInfo;

  DescriptionType                                                     m_eDescriptionType;
  bool                                                                m_bDescriptionOpenAnimeFlag;


};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILEWORK_H_INCLUDE
