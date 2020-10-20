// ============================================================================
/*
 * @file JoinFestaOpenListAppParam.h
 * @brief 開催選択アプリを起動する為のパラメータです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_APPPARAM_H_INCLUDE )
#define JOINFESTAOPENLIST_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


struct APP_PARAM
{
  u32 dummy;
};


//! アプリの結果
struct APP_RESULT
{
  bool  bIsSelectedJoinFestaAttractionData; //! 選択フラグ[ true : 選択した, false : 選択しないで戻った ]
};


GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLIST_APPPARAM_H_INCLUDE
