/**
 *  GAME FREAK inc.
 *
 *  @file   FieldDebugTraffic.cpp
 *  @brief  往来デバッグメニュー
 *  @author miyachi_soichi
 *  @date   2015/06/29
 */

#if PM_DEBUG

// niji
#include "GameSys/include/GameManager.h"

//Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Debug/FieldDebugTraffic.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

// NPC作成
static void UpdateCreateActorFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( !pButton->IsTrigger( gfl2::ui::BUTTON_A ) ){ return; }

  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  TrafficManager *pMgr = pDebug->GetMgr();
  pMgr->Debug_CreateActor();
}
// 座標再作成
static void UpdateCreateVectorList(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( !pButton->IsTrigger( gfl2::ui::BUTTON_A ) ){ return; }

  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  TrafficManager *pMgr = pDebug->GetMgr();
  gfl2::math::Vector3 vec_list[10];
  for( u32 i = 0; i < 10; ++ i )
  {
    vec_list[ i ] = pDebug->GetPosition( i );
  }
  pMgr->Debug_VectorPos( pDebug->GetVectorNum(), vec_list, pDebug->IsCurve() );
}
// 座標数変更
static void UpdateChangeVectorNumFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  u32 num = pDebug->GetVectorNum();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && num > 1 )
  {
    -- num;
    pDebug->SetVectorNum( num );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && num < 10 )
  {
    ++ num;
    pDebug->SetVectorNum( num );
  }
}
static wchar_t* DrawChangeVectorNumFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetVectorNum() );
  return workStr;
}
// 直線or曲線
static void UpdateChangeStraightOrCurveFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    pDebug->SetCurve( true );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pDebug->SetCurve( false );
  }
}
static wchar_t* DrawChangeStraightOrCurveFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( pDebug )
  {
    gfl2::str::StrBuf*  workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    if( !pDebug->IsCurve() )
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_traffic_line_straight, *workStrBuf );
    }
    else
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_traffic_line_curve, *workStrBuf );
    }
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls:%ls", item->GetName(), workStrBuf->GetPtr() );
  }
  return workStr;
}
// 座標変更
static void UpdateChangeVectorPosFunc( DebugTraffic *pDebug , u32 idx )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    Fieldmap *pFieldmap = pGameManager->GetFieldmap();
    Field::MoveModel::FieldMoveModel *pChara = pFieldmap->GetPlayerCharacter();
    gfl2::math::Vector3 vPos = pChara->GetCharaDrawInstance()->GetPosition();

    pDebug->SetPosition( idx, vPos );
  }
}
static void UpdateChangeVectorPos0Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 0 );
}
static void UpdateChangeVectorPos1Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 1 );
}
static void UpdateChangeVectorPos2Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 2 );
}
static void UpdateChangeVectorPos3Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 3 );
}
static void UpdateChangeVectorPos4Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 4 );
}
static void UpdateChangeVectorPos5Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 5 );
}
static void UpdateChangeVectorPos6Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 6 );
}
static void UpdateChangeVectorPos7Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 7 );
}
static void UpdateChangeVectorPos8Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 8 );
}
static void UpdateChangeVectorPos9Func(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  UpdateChangeVectorPosFunc( pDebug, 9 );
}
static wchar_t* DrawChangeVectorPosFunc( DebugTraffic *pDebug, u32 idx, gfl2::debug::DebugWinItem* item )
{
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  gfl2::math::Vector3 pos = pDebug->GetPosition( idx );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls%d[%0.2f, %0.2f, %0.2f]", item->GetName(), idx+1, pos.x, pos.y, pos.z );
  return workStr;
}
static wchar_t* DrawChangeVectorPos0Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 0, item );
}
static wchar_t* DrawChangeVectorPos1Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 1, item );
}
static wchar_t* DrawChangeVectorPos2Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 2, item );
}
static wchar_t* DrawChangeVectorPos3Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 3, item );
}
static wchar_t* DrawChangeVectorPos4Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 4, item );
}
static wchar_t* DrawChangeVectorPos5Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 5, item );
}
static wchar_t* DrawChangeVectorPos6Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 6, item );
}
static wchar_t* DrawChangeVectorPos7Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 7, item );
}
static wchar_t* DrawChangeVectorPos8Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 8, item );
}
static wchar_t* DrawChangeVectorPos9Func( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  return DrawChangeVectorPosFunc( pDebug, 9, item );
}

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugTraffic *pDebug = reinterpret_cast<DebugTraffic*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    // リロード
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_reload, pDebug, UpdateCreateVectorList, NULL );
    // NPC作成
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_create, pDebug, UpdateCreateActorFunc, NULL );
    // 線
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_line, pDebug, UpdateChangeStraightOrCurveFunc, DrawChangeStraightOrCurveFunc );
    // 座標変更
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_num, pDebug, UpdateChangeVectorNumFunc, DrawChangeVectorNumFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos0Func, DrawChangeVectorPos0Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos1Func, DrawChangeVectorPos1Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos2Func, DrawChangeVectorPos2Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos3Func, DrawChangeVectorPos3Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos4Func, DrawChangeVectorPos4Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos5Func, DrawChangeVectorPos5Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos6Func, DrawChangeVectorPos6Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos7Func, DrawChangeVectorPos7Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos8Func, DrawChangeVectorPos8Func );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_traffic_pos, pDebug, UpdateChangeVectorPos9Func, DrawChangeVectorPos9Func );
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
 *  @brief  コンストラクタ
 */
DebugTraffic::DebugTraffic( void )
: m_pTrafficManager( NULL )
, m_nVectorNum( 3 )
, m_bCurve( true )
{
  m_vPos[ 0 ] = gfl2::math::Vector3( 37700.0f, -999.0f, 27300.0f );
  m_vPos[ 1 ] = gfl2::math::Vector3( 38000.0f, -999.0f, 27400.0f );
  m_vPos[ 2 ] = gfl2::math::Vector3( 38300.0f, -999.0f, 27300.0f );
}

/**
 *  @brief  デストラクタ
 */
DebugTraffic::~DebugTraffic( void )
{
  Terminate();
}

/**
 *  @brief  初期化
 */
void DebugTraffic::Initialize( gfl2::heap::HeapBase *pHeap, TrafficManager *pTrafficManager, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pTrafficManager = pTrafficManager;
  m_pMsgData = pMsgData;
}

/**
 *  @brief  破棄
 */
void DebugTraffic::Terminate( void )
{
}

void DebugTraffic::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_traffic_top, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
