// ============================================================================
/*
 * @file JoinFestaAttractionResultAppParam.h
 * @brief アトラクション結果アプリを起動する為のパラメータです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_APPPARAM_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)


struct APP_PARAM
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionData   attractionData;
};



GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULT_APPPARAM_H_INCLUDE
