// ============================================================================
/*
 * @file NBRAppParam.h
 * @brief 通信対戦受付アプリを起動する為のパラメータです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_APPPARAM_H_INCLUDE )
#define NBR_APPPARAM_H_INCLUDE
#pragma once

#include <pml/include/pmlib.h>
#include <macro/include/gfl2_macros.h>
#include "Battle/include/battle_SetupParam.h"
#include "Battle/Regulation/include/Regulation.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetDefine.h"

// 前方宣言
namespace NetAppLib{
  namespace UI{
    class RegulationDrawInfo;
  }
}
class Regulation;

namespace NetApp{
  namespace NBR{
    class NBRNet;
  }
}

namespace NetEvent{
  namespace NetworkBattle{
    struct PersonalData;
  }
}

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)

//-------------------------------------
//@enum NBR開始モード  
//=====================================
enum StartupMode{
  STARTUP_FIRST,                //! 最初の初期起動
  STARTUP_RULE_SELECT_DECIDE,   //! ルール選択アプリでレギュレーション選択時の起動
  STARTUP_RULE_SELECT_CANCEL,   //! ルール選択アプリでキャンセルした時の起動
  STARTUP_TEAM_SELECT_AGAIN,    //! バトルチーム選択アプリ後の起動
  STARTUP_BATTLE_AGAIN,         //! バトル後の起動
  STARTUP_DISCONNECT_OCCUR,     //! バトル後の起動（ロイヤルで切断発生していた場合）
};


//-------------------------------------
//@enum NBR終了モード
//=====================================
enum ReturnMode{
  RET_END,            //! 終了
  RET_RULE_SELECT,    //! ルール選択アプリへ
  RET_TEAM_SELECT,    //! バトルチーム選択アプリへ
  RET_BATTLE,         //! バトルへ
};

//-------------------------------------
//@enum バトルチーム選択結果
//=====================================
enum TeamSelectReturnMode{
  TEAM_SELECT_RETURN_DECIDE,        //! チームを選んだ
  TEAM_SELECT_RETURN_CANCEL,        //! キャンセル選んだ
};

//! 相手の人数MAX
static const u32 TARGET_MAX = 3;

//-------------------------------------
//@struct 外部設定パラメータ
//=====================================
struct APP_PARAM
{
  StartupMode             startupMode;              //! [in]起動モード
  int                     ruleSelectorNetId;        //! [in]現在のルール選択者のNetID
  u8                      priority[ NBRNetDefine::NET_NUM_MAX ];//! [in]NetIdが要素で優先度が格納されている
  int                     hostNetID;                //! [in]主催者のNetID
  Regulation::BATTLE_RULE battleRule;               //! [in]バトルルール
  TeamSelectReturnMode    teamSelectReturnMode;     //! [in]バトルチーム選択アプリの結果
  u32                     teamSelectIndex;          //! [in]バトルチーム選択アプリで選択したチームインデックス
  NBRNet*                 pNet;                     //! [in]通信クラス
  ReturnMode              returnMode;               //! [out]終了モード
  BTL_CLIENT_ID           standingPos;              //! [out]戦う立ち位置
  u32                     bgm;                      //! [out]選択されたBGM
  s8                      selectBgmIndex;           //! [in/out]選択しているBGMインデックス
  NetEvent::NetworkBattle::PersonalData*    pPersonalData[ BTL_CLIENT_NUM ];  //! [out]参加者データ
  NetAppLib::UI::RegulationDrawInfo*        pSelectRegulationDrawInfo;        //! [out]レギュレーション選択アプリで選択したレギュレーション
};


GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_APPPARAM_H_INCLUDE
