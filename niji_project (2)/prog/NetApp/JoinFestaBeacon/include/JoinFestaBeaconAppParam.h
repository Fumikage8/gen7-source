// ============================================================================
/*
 * @file JoinFestaBeaconAppParam.h
 * @brief ビーコンアプリを起動する為のパラメータです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_APPPARAM_H_INCLUDE )
#define JOINFESTABEACON_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

enum
{
  MATCHING_MAX = 3,
};

//どれに進むか
enum
{
  TYPE_TRADE = 0, //交換
  TYPE_BATTLE_S,  //シングル
  TYPE_BATTLE_D,  //ダブル
  TYPE_BATTLE_M,  //マルチ
  TYPE_BATTLE_R   //ロイヤル
};

//招待タイプ
enum
{
  TYPE_INVITE = 0,//申し込み
  TYPE_INVITED,   //申し込まれた
  TYPE_RECRUIT    //募集
};

//!定数
enum
{
  SINGLE_CONNECT_NUM = 1,//シングル接続人数
  MULTI_CONNECT_NUM = 3,//マルチ接続人数
};

struct APP_PARAM
{
  bool isCancel;
  bool isConnectSuccess;
  u32 type;
  u32 connectNum;//1 or 3
  u32 inviteType;
  NetAppLib::JoinFesta::JoinFestaPersonalData*         m_pSelectedPersonalDataArray[MATCHING_MAX];     //!  リストで選択したパーソナルデータ
};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACON_APPPARAM_H_INCLUDE
