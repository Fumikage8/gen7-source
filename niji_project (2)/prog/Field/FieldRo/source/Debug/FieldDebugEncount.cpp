//======================================================================
/**
 * @file FieldDebugEncount.cpp
 * @date 2015/09/08 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：Encount
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Debug/FieldDebugEncount.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
// Encount

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugEncount *pDebug = reinterpret_cast<DebugEncount*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_encount_enable, &DebugTypes::s_isEncountDebugEnable);
    gfl2::debug::DebugWin_AddItemValueU8( pGroup, pHeap, pMsgData, msg_dmenu_field_encount_up, &DebugTypes::s_encountProbPattern.up, 0, 255, 1);
    gfl2::debug::DebugWin_AddItemValueU8( pGroup, pHeap, pMsgData, msg_dmenu_field_encount_interval, &DebugTypes::s_encountProbPattern.interval, 0, 255, 1);
    gfl2::debug::DebugWin_AddItemValueU8( pGroup, pHeap, pMsgData, msg_dmenu_field_encount_max, &DebugTypes::s_encountProbPattern.max, 0, 255, 1);
    gfl2::debug::DebugWin_AddItemValueU8( pGroup, pHeap, pMsgData, msg_dmenu_field_encount_safety, &DebugTypes::s_encountProbPattern.safety, 0, 255, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGroup, pHeap, pMsgData, msg_dmenu_field_encount_ride_rate, &DebugTypes::s_encountRideAdjustRate, 0, 300, 1);
  }
}
/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

/**
 * @class DebugEncount
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugEncount::DebugEncount( void )
  : m_pHeap( NULL)
  , m_pMsgData( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
DebugEncount::~DebugEncount( void )
{
  Terminate();
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DebugEncount::Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
}

/**
 *  @brief  破棄
 */
void DebugEncount::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugEncount::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_encount_top, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}


#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
