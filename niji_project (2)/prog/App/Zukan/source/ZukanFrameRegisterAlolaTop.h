#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTERALOLATOP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTERALOLATOP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameRegisterAlolaTop.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録の際のアローラ図鑑トップ画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewRegisterAlolaTop.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameRegisterAlolaTopCell
 @brief     図鑑の登録の際のアローラ図鑑トップ画面のフレームのセル
 */
//==============================================================================
class ZukanFrameRegisterAlolaTopCell : public ZukanFrameSceneTemplateCell<ZukanViewRegisterAlolaTop> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTERALOLATOP_H_INCLUDED

