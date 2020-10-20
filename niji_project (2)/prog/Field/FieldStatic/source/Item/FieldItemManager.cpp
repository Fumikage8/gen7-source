//======================================================================
/**
 * @file FieldItemManager.cpp
 * @date 2015/09/04 14:44:33
 * @author ikawa_hiroki
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "Field/FieldStatic/include/Item/FieldItemManager.h"

#include "GameSys/include/GameManager.h"


#include "Field/FieldRo/include/PlacementData/accessor/FieldItemAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldStaticModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/StaticModel/StaticModelCollisionDataAccessor.h"

#include "niji_conv_header/field/static_model/static_model_id.h"

#include <niji_conv_header/field/placement_data/FieldPlacementItemTypes.h>
#include <niji_conv_header/field/FocusIdDef.h> // @fix GFNMCat[958]

GFL_NAMESPACE_BEGIN( Field )

  /**
   *  @brief  コンストラクタ
   */
FieldItemManager::FieldItemManager()
{
//  GFL_PRINT("m_DropItemList: %d %d", m_DropItemList.MaxSize(), m_DropItemList.Size());
}

/**
  *  @brief  デストラクタ
  */
FieldItemManager::~FieldItemManager()
{
}

/**
  *  @brief  初期化関数
  */
void FieldItemManager::Initialize( void )
{
  for( u32 i = 0; i < MAX_DROP_ITEM; i++ ) 
  {
    m_DropItemList.colData[ i ].col_num = 1;
    m_DropItemList.colData[ i ].col_type = FieldCollisionAccessor::CYLINDER;
  }
  m_DropItemList.num = MAX_DROP_ITEM; // @fix GFNMCat[958]  ドロップアイテムを４つ生成し、生成順に取得すると、２つのアイテムが消えない。
  m_DropModelList.num = MAX_DROP_ITEM; // @fix GFNMCat[958]  ドロップアイテムを４つ生成し、生成順に取得すると、２つのアイテムが消えない。
  m_DropItemNum = 0;  // @fix GFNMCat[958]  ドロップアイテムを４つ生成し、生成順に取得すると、２つのアイテムが消えない。
}


/**
  *  @brief  終了関数
  */
void FieldItemManager::Terminate()
{
}

/**
  *  @brief  リストからFieldtemAccessor用データを生成
  *  @return  生成場所のインデックス
  */
u32 FieldItemManager::CreateDropItemData( gfl2::math::Vector3 position, u32 itemID, Type type, u32* p_eventID)
{
  // これ以上追加できない場合、追加失敗で終了
  if( m_DropItemNum >= MAX_DROP_ITEM ) // @fix GFNMCat[958]
  {
    return MAX_DROP_ITEM;
  }
  u32 index = 0;

  // 空きエリアに追加
  for( u32 i=0; i<MAX_DROP_ITEM; i++, index++ )
  {
    if( m_DropItemList.itemData[ i ].eventID == 0 )
    {
      // アイテムのアクター
      FieldItemAccessor::FieldItemData* pItemData = &m_DropItemList.itemData[ i ];
      pItemData->type = FIELD_ACTOR_TYPE_ITEM;
      pItemData->position[ 0 ] = position.GetX();
      pItemData->position[ 1 ] = position.GetY();
      pItemData->position[ 2 ] = position.GetZ();
      pItemData->quaternion[ 0 ] = 0;
      pItemData->quaternion[ 1 ] = 0;
      pItemData->quaternion[ 2 ] = 0;
      pItemData->quaternion[ 3 ] = 1;
      pItemData->romVersion = PM_VERSION;
      pItemData->flagwork = 0;
      pItemData->flagwork_num = 0;
      pItemData->eventID = DROP_ITEM_EVENT_ID_OFFSET + i;
      pItemData->itemID = itemID;
      pItemData->item_num = 1;
      pItemData->shapeType = ITEM_TYPE_SPECIAL;

      // @fix GFNMCat[612] ドロップアイテムのフォーカスIDが出ない。
      static const u32 sc_AttentionID[] = 
      {
        FOCUS_ITEMSPECIAL_02,
        FOCUS_ITEMSPECIAL_03,
        FOCUS_ITEMSPECIAL_04,
      };
      GFL_ASSERT( GFL_NELEMS(sc_AttentionID) == TYPE_MAX );

      if( type < TYPE_MAX )
      {
        pItemData->attentionID = sc_AttentionID[type];
      }
      else
      {
        GFL_ASSERT( type < TYPE_MAX );
        pItemData->attentionID = sc_AttentionID[0];
      }

      pItemData->collisionOffset = 
        sizeof( u32 )   // FieldItemDataの個数
          + sizeof( FieldItemAccessor::FieldItemData ) * MAX_DROP_ITEM
          + sizeof( DropCollisionData ) * i;

      // アイテムのコリジョン
      FieldCollisionAccessor::Cylinder* pColData = &m_DropItemList.colData[i].colData;
      {
        pColData->center[ 0 ] = pItemData->position[ 0 ];
        pColData->center[ 1 ] = pItemData->position[ 1 ];
        pColData->center[ 2 ] = pItemData->position[ 2 ];
        pColData->radius = DROP_ITEM_TALK_EV_RADIUS;
        pColData->height = DROP_ITEM_TALK_EV_HEIGT;
      }

      static const u32 sc_StaticModelID[] = 
      {
        STATIC_MODEL_ID_COM_BM_ITEMSPECIAL02, //TYPE_NORMAL,
        STATIC_MODEL_ID_COM_BM_ITEMSPECIAL06, //TYPE_SEA,
        STATIC_MODEL_ID_COM_BM_ITEMSPECIAL07, //TYPE_GRASS,
      };
      GFL_ASSERT( GFL_NELEMS(sc_StaticModelID) == TYPE_MAX );
      
 
      // アイテムのモデル
      FieldStaticModelAccessor::StaticModelData* pModelData = &m_DropModelList.modelData[ i ];
      pModelData->type = FIELD_ACTOR_TYPE_STATICMODEL;
      pModelData->position[0] =  position.GetX();
      pModelData->position[1] =  position.GetY();
      pModelData->position[2] =  position.GetZ();
      pModelData->quaternion[ 0 ] = 0;
      pModelData->quaternion[ 1 ] = 0;
      pModelData->quaternion[ 2 ] = 0;
      pModelData->quaternion[ 3 ] = 1;
      pModelData->romVersion = PM_VERSION;
      pModelData->flagwork = 0;
      pModelData->flagwork_num = 0;
      pModelData->eventID = pItemData->eventID;
      pModelData->collisionOffset = 0;  // ドロップアイテムは衝突判定なし

      if( type < TYPE_MAX )
      {
        pModelData->staticModelID = sc_StaticModelID[type];
      }
      else
      {
        GFL_ASSERT( type < TYPE_MAX );
        pModelData->staticModelID = sc_StaticModelID[0];
      }

      m_DropItemNum ++; // @fix GFNMCat[958]
      *p_eventID = pItemData->eventID;
      break;
    }
  }

  return index;
}

/**
  *  @brief  ドロップアイテム用FieldtemAccessor用データを破棄
  *  @param  eventID
  *  @return 破棄の成否
  */
bool FieldItemManager::TerminateDropItemData( u32 eventID )
{
  // これ以上破棄できない場合、失敗で終了
  if( m_DropItemNum == 0 )  // @fix GFNMCat[958]
  {
    return false;
  }

  for( u32 i=0; i<MAX_DROP_ITEM; i++ )
  {
    if( m_DropItemList.itemData[ i ].eventID == eventID )
    {
      FieldItemAccessor::FieldItemData* pItemData = &m_DropItemList.itemData[ i ];
      pItemData->type = 0;
      pItemData->position[ 0 ] = 0;
      pItemData->position[ 1 ] = 0;
      pItemData->position[ 2 ] = 0;
      pItemData->quaternion[ 0 ] = 0;
      pItemData->quaternion[ 1 ] = 0;
      pItemData->quaternion[ 2 ] = 0;
      pItemData->quaternion[ 3 ] = 1;
      pItemData->romVersion = PM_VERSION;
      pItemData->eventID = 0;
      pItemData->itemID = 0;
      pItemData->attentionID = 0;
      pItemData->collisionOffset = 0;

      FieldCollisionAccessor::Cylinder* pColData = &m_DropItemList.colData[ i ].colData;
      {
        pColData->center[ 0 ] = 0;
        pColData->center[ 1 ] = 0;
        pColData->center[ 2 ] = 0;
        pColData->radius = 0;
        pColData->height = 0;
      }

      // アイテムのモデル
      FieldStaticModelAccessor::StaticModelData* pModelData = &m_DropModelList.modelData[ i ];
      pModelData->type = 0;
      pModelData->position[0] = 0;
      pModelData->position[1] = 0;
      pModelData->position[2] = 0;
      pModelData->quaternion[ 0 ] = 0;
      pModelData->quaternion[ 1 ] = 0;
      pModelData->quaternion[ 2 ] = 0;
      pModelData->quaternion[ 3 ] = 1;
      pModelData->romVersion = 0;
      pModelData->flagwork = 0;
      pModelData->flagwork_num = 0;
      pModelData->eventID = 0;
      pModelData->staticModelID = 0;
      pModelData->collisionOffset = 0;  // ドロップアイテムは衝突判定なし

     m_DropItemNum --;// @fix GFNMCat[958]
      return true;
    }
  }
  // 指定IDがなかった
  return false;
}

/**
  *  @brief  ドロップアイテム用FieldtemAccessor用データを全破棄
  *  @return 破棄の成否
  */
bool FieldItemManager::TerminateDropItemData()
{
  for( u32 i=0; i<MAX_DROP_ITEM; i++ )
  {
    FieldItemAccessor::FieldItemData* pItemData = &m_DropItemList.itemData[ i ];
    pItemData->type = 0;
    pItemData->position[ 0 ] = 0;
    pItemData->position[ 1 ] = 0;
    pItemData->position[ 2 ] = 0;
    pItemData->quaternion[ 0 ] = 0;
    pItemData->quaternion[ 1 ] = 0;
    pItemData->quaternion[ 2 ] = 0;
    pItemData->quaternion[ 3 ] = 1;
    pItemData->romVersion = PM_VERSION;
    pItemData->eventID = 0;
    pItemData->itemID = 0;
    pItemData->attentionID = 0;   
    pItemData->collisionOffset = 0;

    FieldCollisionAccessor::Cylinder* pColData = &m_DropItemList.colData[ i ].colData;
    {
      pColData->center[ 0 ] = 0;
      pColData->center[ 1 ] = 0;
      pColData->center[ 2 ] = 0;
      pColData->radius = 0;
      pColData->height = 0;
    }
      // アイテムのモデル
      FieldStaticModelAccessor::StaticModelData* pModelData = &m_DropModelList.modelData[ i ];
      pModelData->type = 0;
      pModelData->position[0] = 0;
      pModelData->position[1] = 0;
      pModelData->position[2] = 0;
      pModelData->quaternion[ 0 ] = 0;
      pModelData->quaternion[ 1 ] = 0;
      pModelData->quaternion[ 2 ] = 0;
      pModelData->quaternion[ 3 ] = 1;
      pModelData->romVersion = PM_VERSION;
      pModelData->flagwork = 0;
      pModelData->flagwork_num = 0;
      pModelData->eventID = 0;
      pModelData->staticModelID = 0;
      pModelData->collisionOffset = 0;  // ドロップアイテムは衝突判定なし
  }
  m_DropItemNum = 0; // @fix GFNMCat[958]

  return true;
}

/**
  *  @brief  ドロップアイテム用FieldtemActorを登録解除
  *  @param  eventID
  *  @return 生成の成否
  */
void FieldItemManager::UnregistDropItemModelActor( u32 index )
{
  FieldStaticModelActor* pActor = GetDropItemModelActor( index );
  if( pActor )
  {
    GetDropItemModelActor( index )->UnregistActor();
  }
}

GFL_NAMESPACE_END( Field )
