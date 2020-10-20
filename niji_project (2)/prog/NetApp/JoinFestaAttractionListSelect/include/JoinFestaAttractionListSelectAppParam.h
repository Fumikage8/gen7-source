// ============================================================================
/*
 * @file JoinFestaAttractionListSelectAppParam.h
 * @brief アトラクション開催参加アプリを起動する為のパラメータです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_APPPARAM_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


struct APP_PARAM
{
  u32 dummy;
};


//! アプリの結果
struct APP_RESULT
{
  // アトラクションデータの定義が無いのでコメントアウト
//  NetAppLib::JoinFesta::JoinFestaAttractionData*  pJoinFestaAttractionData;           //! 選択したアトラクションデータ
  bool                                            bIsSelectedJoinFestaAttractionData; //! 選択フラグ[ true : 選択した, false : 選択しないで戻った ]
};


GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECT_APPPARAM_H_INCLUDE
