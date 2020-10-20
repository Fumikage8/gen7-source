// ============================================================================
/*
 * @file KisekaeFrameID.h
 * @brief きせかえアプリのフレームIDを定義します。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_FRAMEID_H_INCLUDE )
#define KISEKAE_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)

enum FrameID
{
  //  きせかえショップ
  KISEKAE_FRAME_ID_SHOP,
  //  きせかえ
  KISEKAE_FRAME_ID_DRESS_UP,
  //  いろそめ
  KISEKAE_FRAME_ID_COLOR_DRESS_UP,
};

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_FRAMEID_H_INCLUDE
