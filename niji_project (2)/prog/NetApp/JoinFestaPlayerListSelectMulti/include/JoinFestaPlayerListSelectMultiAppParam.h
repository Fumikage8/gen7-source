// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiAppParam.h
 * @brief プレイヤー複数選択アプリを起動する為のパラメータです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_APPPARAM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)


//! アプリの起動パラメータ
struct APP_PARAM
{
  u32 dummy;
};


//! アプリの結果
struct APP_RESULT
{
  NetAppLib::JoinFesta::JoinFestaPersonalData*    pSelectJoinFestaPersonalData1;      //! 選択したパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData*    pSelectJoinFestaPersonalData2;      //! 選択したパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData*    pSelectJoinFestaPersonalData3;      //! 選択したパーソナルデータ
  bool                                            bIsSelectedJoinFestaPersonalData;   //! 選択フラグ[ true : 選択した, false : 選択しないで戻った ]
};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTI_APPPARAM_H_INCLUDE
