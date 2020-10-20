#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEALOLATOP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEALOLATOP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameAlolaTop.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑のアローラ図鑑トップ画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewAlolaTop.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameAlolaTopCell
 @brief     図鑑のアローラ図鑑トップ画面のフレームのセル
 */
//==============================================================================
class ZukanFrameAlolaTopCell : public ZukanFrameSceneTemplateCell<ZukanViewAlolaTop> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEALOLATOP_H_INCLUDED

