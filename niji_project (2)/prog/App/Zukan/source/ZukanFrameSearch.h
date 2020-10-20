#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESEARCH_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESEARCH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameSearch.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の検索画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewSearch.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameSearchCell
 @brief     図鑑の検索画面のフレームのセル
 */
//==============================================================================
class ZukanFrameSearchCell : public ZukanFrameSceneTemplateCell<ZukanViewSearch> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESEARCH_H_INCLUDED

