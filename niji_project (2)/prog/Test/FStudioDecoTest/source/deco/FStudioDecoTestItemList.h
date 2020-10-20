#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_ITEM_LIST_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_ITEM_LIST_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestItemList.h
 * @date    2016/11/01 12:15:10
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Test/FStudioDecoTest/source/FStudioDecoTestDefine.h>

#include <Layout/include/gfl2_Layout.h>


//----------------------------------------------------------------------
// @brief   デコアイテムの管理
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class ItemList
{
  GFL_FORBID_COPY_AND_ASSIGN( ItemList );

public:
  enum {
    ITEM_MAX = 100,
  };

private:
  struct ItemParam
  {
    gfl2::lyt::LytPicture* pic;
    DecoItemKind kind;
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ItemList( gfl2::lyt::LytPane* parent );
  ~ItemList( void );

public:
  //-------------------------------------------------------
  // @brief   アイテムの追加と削除
  //-------------------------------------------------------
  bool AddItem( gfl2::lyt::LytPicture* picture, DecoItemKind kind );
  bool RemoveItem( void );

  //-------------------------------------------------------
  // @brief   アイテムの取得
  //-------------------------------------------------------
  gfl2::lyt::LytPicture* GetItem( u32 index, bool isFrame = false );
  u32                    GetItemIndex( gfl2::lyt::LytPicture* picture );

  DecoItemKind GetItemKind( u32 index );
  u32          GetItemKindIndex( DecoItemKind kind );

  //-------------------------------------------------------
  // @brief   
  //-------------------------------------------------------
  void                   SetActiveItem( gfl2::lyt::LytPicture* picture ) { m_pActiveItem = picture; }
  gfl2::lyt::LytPicture* GetActiveItem( void ) const { return m_pActiveItem; }

  //-------------------------------------------------------
  // @brief   デコの表示切替
  //-------------------------------------------------------
  void SetItemVisible( u32 index, bool visible );
  void SetAllItemVisible( bool visible );

private:
  bool _RemoveItem( u32 index );

  void DebugItemCount( void );

private:
  ItemParam                 m_pItems[ITEM_MAX];
  gfl2::lyt::LytPicture*    m_pActiveItem;
  gfl2::lyt::LytPane*       m_pItemParent;
  u32                       m_ItemCount;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_ITEM_LIST_H_INCLUDED__

#endif // #if PM_DEBUG
