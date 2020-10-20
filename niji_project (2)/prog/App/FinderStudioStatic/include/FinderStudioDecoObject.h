#if !defined (__FINDER_STUDIO_DECO_OBJECT_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_OBJECT_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecoObject.h
 * @date    2017/02/06 14:36:46
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：デコオブジェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDefine.h"

#include <math/include/gfl2_Math.h>
#include <Layout/include/gfl2_Layout.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコオブジェクト
///
//////////////////////////////////////////////////////////////////////////
class DecoObject
{
public:
  //-------------------------------------------------------
  // @brief   デコオブジェクトの生成：上画面
  //-------------------------------------------------------
  static gfl2::lyt::LytPicture* CreateDecoObject_Upper( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID, gfl2::math::VEC3 pos );
  static gfl2::lyt::LytPicture* CreateDecoObject_Upper( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo, gfl2::math::VEC3 pos );
  static gfl2::lyt::LytPicture* CreateDecoObject_Upper( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, Static::StampData stampData );

  static gfl2::lyt::LytPicture* CreateDecoObject_UpperFrame( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID );
  static gfl2::lyt::LytPicture* CreateDecoObject_UpperFrame( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo );
  static gfl2::lyt::LytPicture* CreateDecoObject_UpperFrame( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, Static::DecorationData data );

  //-------------------------------------------------------
  // @brief   デコオブジェクトの生成：下画面
  //-------------------------------------------------------
  static gfl2::lyt::LytPicture* CreateDecoObject_Lower( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID, gfl2::math::VEC3 pos );
  static gfl2::lyt::LytPicture* CreateDecoObject_Lower( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo, gfl2::math::VEC3 pos );

  static gfl2::lyt::LytPicture* CreateDecoObject_LowerFrame( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID );
  static gfl2::lyt::LytPicture* CreateDecoObject_LowerFrame( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo );

  //-------------------------------------------------------
  // @brief   デコオブジェクトの破棄
  //-------------------------------------------------------
  static void DeleteDecoObject( gfl2::lyt::LytPicture* picture );

  //-------------------------------------------------------
  // @brief   座標変換
  //-------------------------------------------------------
  static gfl2::math::VEC3 ConvertDecoPositionToUpper( gfl2::math::VEC3 lowPos );
  static gfl2::math::VEC3 ConvertDecoPositionToLower( gfl2::math::VEC3 uppPos );

private:
  //-------------------------------------------------------
  // @brief   デコオブジェクトの生成：非公開
  //-------------------------------------------------------
  static gfl2::lyt::LytPicture* CreateDecoObject( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo );

  //-------------------------------------------------------
  // @brief   コンバート：非公開
  //-------------------------------------------------------
  static gfl2::math::VEC3 _ConvertVectorScreenToLayout( gfl2::math::VEC3 pos );
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECO_OBJECT_H_INCLUDED__
