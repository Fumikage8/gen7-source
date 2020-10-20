#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTER_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameRegister.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewRegister.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameRegisterCell
 @brief     図鑑の登録画面のフレームのセル
 */
//==============================================================================
class ZukanFrameRegisterCell : public ZukanFrameSceneTemplateCell<ZukanViewRegister> {};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTER_H_INCLUDED

