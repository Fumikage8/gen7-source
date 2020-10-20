#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestItemList.cpp
 * @date    2016/11/01 12:15:25
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestItemList.h"

// インクルード
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/DecoItemData.cdat>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

//----------------------------------------------------------------------
// @brief   コンストラクタ・デストラクタ
//----------------------------------------------------------------------
ItemList::ItemList( gfl2::lyt::LytPane* parent )
  : m_pActiveItem( NULL )
  , m_pItemParent( parent )
  , m_ItemCount(0)
{
  for( u32 i = 0; i < ITEM_MAX; ++i )
  {
    m_pItems[i].pic = NULL;
    m_pItems[i].kind = DECO_ITEM_KIND_NULL;
  }
}

ItemList::~ItemList( void )
{
  m_pActiveItem = NULL;
  m_ItemCount = 0;

  for( u32 i = 0; i < ITEM_MAX; ++i )
  {
    if( m_pItems[i].pic )
    {
      m_pItemParent->RemoveChild( m_pItems[i].pic );
      GFL_SAFE_DELETE( m_pItems[i].pic );
    }
    m_pItems[i].kind = DECO_ITEM_KIND_NULL;
  }
}



//----------------------------------------------------------------------
// @brief   アイテムの追加と削除
//----------------------------------------------------------------------
bool ItemList::AddItem( gfl2::lyt::LytPicture* picture, DecoItemKind kind )
{
  if( m_ItemCount < ITEM_MAX )
  {
    // フレームの場合、１つだけの登録になるためその前のフレームを削除する
    if( kind == DECO_ITEM_KIND_FRAME )
    {
      u32 itemIdx = GetItemKindIndex( DECO_ITEM_KIND_FRAME );
      if( itemIdx != ITEM_MAX )
      {
        _RemoveItem( itemIdx );
      }
    }

    // アイテムを追加
    m_pItems[m_ItemCount].pic = picture;
    m_pItems[m_ItemCount].kind = kind;
    m_ItemCount++;

    // 子ペインの追加
    m_pItemParent->AppendChild( picture );

    // アクティブの設定
    SetActiveItem( picture );

    DebugItemCount();

    return true;
  }

  // 追加に失敗
  return false;
}

bool ItemList::RemoveItem( void )
{
  // 削除
  u32 index = GetItemIndex( m_pActiveItem );

  return _RemoveItem( index );
}

bool ItemList::_RemoveItem( u32 index )
{
  if( index < ITEM_MAX )
  {
    GFL_PRINT( "Remove Deco Item [%d]\n", index );

    m_pItemParent->RemoveChild( m_pItems[index].pic );
    GFL_SAFE_DELETE( m_pItems[index].pic );
    m_pItems[index].kind = DECO_ITEM_KIND_NULL;
  }
  else {
    GFL_PRINT( "Not Remove Deco Item\n" );
    return false;
  }

  // アイテム数を減らす
  --m_ItemCount;

  // 整理
  for( u32 i = index; i < ( ITEM_MAX - 1 ); ++i )
  {
    m_pItems[i] = m_pItems[i + 1];
  }
  m_pItems[ITEM_MAX - 1].pic = NULL;
  m_pItems[ITEM_MAX - 1].kind = DECO_ITEM_KIND_NULL;

  DebugItemCount();

  return true;
}



gfl2::lyt::LytPicture* ItemList::GetItem( u32 index, bool isFrame )
{
  if( index < ITEM_MAX && index < m_ItemCount )
  {
    if( !isFrame )
    {
      return m_pItems[index].pic;
    }
    // デコがフレームの場合、除外する。
    else if( m_pItems[index].kind != DECO_ITEM_KIND_FRAME )
    {
      return m_pItems[index].pic;
    }
  }

  return NULL;
}

u32 ItemList::GetItemIndex( gfl2::lyt::LytPicture* picture )
{
  for( u32 i = 0; i < ITEM_MAX; ++i )
  {
    if( m_pItems[i].pic == picture )
    {
      return i;
    }
  }

  return ITEM_MAX;
}

DecoItemKind ItemList::GetItemKind( u32 index )
{
  if( index < ITEM_MAX && index < m_ItemCount )
  {
    return static_cast<DecoItemKind>( m_pItems[index].kind );
  }

  return DECO_ITEM_KIND_NULL;
}

u32 ItemList::GetItemKindIndex( DecoItemKind kind )
{
  for( u32 i = 0; i < ITEM_MAX; ++i )
  {
    if( m_pItems[i].kind == kind )
    {
      return i;
    }
  }

  return ITEM_MAX;
}




//----------------------------------------------------------------------
// @brief   デコの表示切替
//----------------------------------------------------------------------
void ItemList::SetItemVisible( u32 index, bool visible )
{
  if( m_pItems[index].pic )
  {
    m_pItems[index].pic->SetVisible( visible );
  }
}

void ItemList::SetAllItemVisible( bool visible )
{
  for( u32 i = 0; i < ITEM_MAX; ++i )
  {
    SetItemVisible( i, visible );
  }
}





void ItemList::DebugItemCount( void )
{
  u32 count = 0;

  for( u32 i = 0; i < ITEM_MAX; ++i )
  {
    if( m_pItems[i].pic )
    {
      count++;
    }
  }

  GFL_PRINT( "Item Count [%d]\n", count );
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
