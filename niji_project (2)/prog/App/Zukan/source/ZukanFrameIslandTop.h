#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEISLANDTOP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEISLANDTOP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameIslandTop.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の島図鑑トップ画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewIslandTop.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameIslandTopCell
 @brief     図鑑の島図鑑トップ画面のフレームのセル
 */
//==============================================================================
class ZukanFrameIslandTopCell : public ZukanFrameSceneTemplateCell<ZukanViewIslandTop> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEISLANDTOP_H_INCLUDED

