#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMELAUNCH_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMELAUNCH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameLaunch.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の起動画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewLaunch.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameLaunchCell
 @brief     図鑑の起動画面のフレームのセル
 */
//==============================================================================
class ZukanFrameLaunchCell : public ZukanFrameSceneTemplateCell<ZukanViewLaunch> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMELAUNCH_H_INCLUDED

