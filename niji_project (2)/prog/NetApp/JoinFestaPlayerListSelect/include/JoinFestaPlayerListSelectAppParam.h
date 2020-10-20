// ============================================================================
/*
 * @file JoinFestaPlayerListSelectAppParam.h
 * @brief プレイヤー単体選択アプリを起動する為のパラメータです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_APPPARAM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)


//! アプリの起動パラメータ
struct APP_PARAM
{
  u32 dummy;
};


//! アプリの結果
struct APP_RESULT
{
  NetAppLib::JoinFesta::JoinFestaPersonalData*    pSelectJoinFestaPersonalData;       //! 選択したパーソナルデータ
  bool                                            bIsSelectedJoinFestaPersonalData;   //! 選択フラグ[ true : 選択した, false : 選択しないで戻った ]
};


GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECT_APPPARAM_H_INCLUDE
