#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_ITEM_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_ITEM_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestItem.h
 * @date    2016/10/27 20:25:03
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Test/FStudioDecoTest/source/FStudioDecoTestDefine.h>

#include <Layout/include/gfl2_Layout.h>
#include <ui/include/gfl2_UI.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class UIUtility;

class DecoItem
{
public:
  static void SetDecoItemData( u32 id, DecoItemKind kind, gfl2::math::VEC3 pos );
  static void ResetDecoItemData( void );

  static gfl2::lyt::LytPicture* CreateDecoItem_Lower( gfl2::heap::HeapBase* heap, UIUtility* util );
  static gfl2::lyt::LytPicture* CreateDecoItem_Upper( gfl2::heap::HeapBase* heap, UIUtility* util );
  static DecoItemKind           GetDecoItemKind( void );
  
  // 座標変換：タッチ座標 -> レイアウト座標
  static gfl2::math::VEC3 TransferLayoutCoordinate( gfl2::ui::TouchPanel* pTouchPanel );
  static void             TransferLayoutCoordinate( gfl2::math::VEC3* vec );

private:
  static gfl2::lyt::LytPicture* CreateDecoItem( gfl2::heap::HeapBase* heap, UIUtility* util );

public:
  static u32                m_DecoItemID;
  static DecoItemKind       m_DecoItemKind;
  static gfl2::math::VEC3   m_DecoItemPos;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_ITEM_H_INCLUDED__

#endif // #if PM_DEBUG
