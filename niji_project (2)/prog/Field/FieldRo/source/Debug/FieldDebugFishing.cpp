//======================================================================
/**
 * @file FieldDebugFishing.cpp
 * @date 2016/01/21
 * @author Masanori Kanamaru
 * @brief 釣りデバッグ
 * @par Copyright
 */
//======================================================================

#include "Field/FieldRo/include/Debug/FieldDebugFishing.h"
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldStatic/include/Event/EventFishingAction.h"
#include "Savedata/include/FishingSpot.h"

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugFishing* pDebug = reinterpret_cast<DebugFishing*>( userWork );
  gfl2::debug::DebugWinGroup* pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase* pHeap    = pDebug->GetHeap();

    gfl2::debug::DebugWin_AddItemValueBool(pGroup, pHeap, L"Always Spawn Rare", &(Savedata::FishingSpot::debugForceSpawnRare));
    gfl2::debug::DebugWin_AddItemValueInt(pGroup, pHeap, L"Stabilize Target", &(EventFishingAction::stabilizingTargetFlag), EventFishingAction::Disable, EventFishingAction::ForceItem);
  }
}


/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup* pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

/**
  *  @brief  初期化
  */
void DebugFishing::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::str::MsgData* pMsgData )
{
  this->pHeap = pHeap;
}

/**
  *  @brief  メニューの作成
  */
void DebugFishing::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( this->pHeap, L"Fishing", root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
#endif //PM_DEBUG