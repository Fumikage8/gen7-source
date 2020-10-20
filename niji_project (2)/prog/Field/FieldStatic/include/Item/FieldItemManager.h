//======================================================================
/**
 * @file FieldItemManager.h
 * @date 2015/09/04 13:20:27
 * @author ikawa_hiroki
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDITEMMANAGER_H_INCLUDED__
#define __FIELDITEMMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Quaternion.h>
#include <util/include/gfl2_List.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldItemAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldStaticModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActor.h"
#include "Field/FieldRo/include/PlacementData/EventIDOffset.h"

GFL_NAMESPACE_BEGIN( Field )

class FieldItemManager
{
    //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( FieldItemManager );

public:
  enum
  {
    MAX_DROP_ITEM = 4,        //@caution StaticModelのDynamicワーク数にあわせる。
    DROP_ITEM_EVENT_ID_OFFSET = DROPITEM_ACTOR_START,

    DROP_ITEM_TALK_EV_RADIUS = 60,
    DROP_ITEM_TALK_EV_HEIGT = 35,
  };

  enum Type
  {
    TYPE_NORMAL,
    TYPE_SEA,
    TYPE_GRASS,

    TYPE_MAX,
  };

  struct DropItemData
  {
    FieldItemAccessor::FieldItemData itemData;
    u32 col_num;
    u32 col_type;
    FieldCollisionAccessor::Cylinder colData;
  };

  struct DropCollisionData
  {
    u32 col_num;
    u32 col_type;
    FieldCollisionAccessor::Cylinder colData;
  };

  struct DropItemList
  {
    u32 num;
    FieldItemAccessor::FieldItemData  itemData[ MAX_DROP_ITEM ];
    FieldItemManager::DropCollisionData colData[ MAX_DROP_ITEM ];
  };

  struct DropItemModelList
  {
    u32 num;
    FieldStaticModelAccessor::StaticModelData modelData[ MAX_DROP_ITEM ];
  };

  /**
   *  @brief  コンストラクタ
   */
  FieldItemManager();

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldItemManager();

  /**
   *  @brief  初期化関数
   */
  void Initialize(void);

  /**
   *  @brief  終了関数
   */
  void Terminate();

  /**
   *  @brief  ドロップアイテム用FieldtemAccessor用データを生成
   *  @param(in)  position  座標
   *  @param(in)  itemID    アイテムID
   *  @param(in)  type      見た目のタイプ
   *  @param(out) p_eventID   追加されたアイテムのイベントID
   *  @return 生成の成否
   */
 u32 CreateDropItemData( gfl2::math::Vector3 position, u32 itemID, Type type, u32* p_eventID );
  /**
   *  @brief  ドロップアイテム用FieldtemAccessor用データを破棄
   *  @param  eventID
   *  @return 生成の成否
   */
  bool TerminateDropItemData( u32 eventID );
  bool TerminateDropItemData();
  

  /**
   *  @brief  ドロップアイテム用FieldtemAccessor用データを登録解除
   *  @param  eventID
   *  @return 生成の成否
   */
  void UnregistDropItemModelActor( u32 index );


  /**
   *  @brief  ドロップアイテム用FieldItemAccessor用データを取得
   */
  inline void*  GetDropItemData() { return reinterpret_cast<void *>( &m_DropItemList ); }

  /**
   *  @brief  ドロップアイテム用FieldStaticModelAccessor用データを取得
   */
  inline void*  GetDropItemModelData() { return reinterpret_cast<void *>( &m_DropModelList ); }

  /**
   *  @brief  ドロップアイテム用FieldStaticModelAccessor用データを取得
   */
  inline FieldStaticModelActor*  GetDropItemModelActor( u32 index ) { return m_pStaticModelActor[ index ]; }

  /**
   *  @brief  ドロップアイテム用FieldStaticModelAccessor用データをセット
   */
  inline void SetDropItemModelActor( FieldStaticModelActor* pActor, u32 index ) { m_pStaticModelActor[ index ] = pActor; }

private:
  u32                     m_DropItemNum;  ///< 保持しているドロップアイテムの数 @fix GFNMCat[958]
  DropItemList            m_DropItemList;
  DropItemModelList       m_DropModelList;
  FieldStaticModelActor*  m_pStaticModelActor[ MAX_DROP_ITEM ];        ///< ドロップアイテム配置モデル用

};

GFL_NAMESPACE_END( Field )

#endif // __FIELDITEMMANAGER_H_INCLUDED__
