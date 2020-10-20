#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestItem.cpp
 * @date    2016/10/27 20:25:13
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestItem.h"

#include <Test/FStudioDecoTest/source/util/FStudioDecoTestUIUtility.h>

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_anim_list.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/DecoItemData.cdat>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

u32 DecoItem::m_DecoItemID = PASSIVE_ITEM_ID;
DecoItemKind DecoItem::m_DecoItemKind = DECO_ITEM_KIND_NULL;
gfl2::math::VEC3 DecoItem::m_DecoItemPos = gfl2::math::VEC3::Zero();


void DecoItem::SetDecoItemData( u32 id, DecoItemKind kind, gfl2::math::VEC3 pos )
{
  m_DecoItemID = id;
  m_DecoItemKind = kind;
  m_DecoItemPos = pos;
}

void DecoItem::ResetDecoItemData( void )
{
  m_DecoItemID = PASSIVE_ITEM_ID;
  m_DecoItemKind = DECO_ITEM_KIND_NULL;
  m_DecoItemPos = gfl2::math::VEC3::Zero();
}


gfl2::lyt::LytPicture* DecoItem::CreateDecoItem_Lower( gfl2::heap::HeapBase* heap, UIUtility* util )
{
  if( m_DecoItemID == PASSIVE_ITEM_ID ) return NULL;

  gfl2::lyt::LytPicture* picture = CreateDecoItem( heap, util );
  
  // スケールを変更
  gfl2::math::VEC2 scale( LOW_CANVAS_SCALE_X, LOW_CANVAS_SCALE_Y );
  picture->SetScale( scale );

  // 位置を変更
  picture->SetTranslate( m_DecoItemPos );

  return picture;
}

gfl2::lyt::LytPicture* DecoItem::CreateDecoItem_Upper( gfl2::heap::HeapBase* heap, UIUtility* util )
{
  if( m_DecoItemID == PASSIVE_ITEM_ID ) return NULL;

  gfl2::lyt::LytPicture* picture = CreateDecoItem( heap, util );

  // スケールを変更
  gfl2::math::VEC2 scale( 1.0f, 1.0f );
  picture->SetScale( scale );

  // 位置を変更
  gfl2::math::VEC3 pos = m_DecoItemPos;
  pos.x /= LOW_CANVAS_SCALE_X;
  pos.y /= LOW_CANVAS_SCALE_Y;
  picture->SetTranslate( pos );

  return picture;
}

gfl2::lyt::LytPicture* DecoItem::CreateDecoItem( gfl2::heap::HeapBase* heap, UIUtility* util )
{
  gfl2::lyt::LytRes* res = util->GetLytRes();
  const gfl2::lyt::LytResTextureInfo* texInfo = res->GetTexture( c_DecoItemTexture[m_DecoItemID], 0 );

  gfl2::lyt::LytPicture* picture = GFL_NEW( heap ) gfl2::lyt::LytPicture( *texInfo );
  picture->SetUserAllocated();
  picture->SetVisible(false);

  return picture;
}

DecoItemKind DecoItem::GetDecoItemKind( void )
{
  return m_DecoItemKind;
}



/**
* @brief   座標変換：タッチ座標 -> レイアウト座標
*/
gfl2::math::VEC3 DecoItem::TransferLayoutCoordinate( gfl2::ui::TouchPanel* pTouchPanel )
{
  gfl2::math::VEC3 pos;

  pTouchPanel->GetXY( &pos );

  TransferLayoutCoordinate( &pos );

  return pos;
}

void DecoItem::TransferLayoutCoordinate( gfl2::math::VEC3* vec )
{
  vec->x = vec->x - 160;
  vec->y = 120 - vec->y;
}


GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
