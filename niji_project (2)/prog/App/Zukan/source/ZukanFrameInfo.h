#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEINFO_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEINFO_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameInfo.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の情報画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewInfo.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameInfoCell
 @brief     図鑑の情報画面のフレームのセル
 */
//==============================================================================
class ZukanFrameInfoCell : public ZukanFrameSceneTemplateCell<ZukanViewInfo> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEINFO_H_INCLUDED

