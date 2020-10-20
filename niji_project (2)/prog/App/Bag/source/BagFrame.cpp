//=============================================================================
/**
 * @file    BagFrame.cpp
 * @brief   バッグ画面フレームワーク
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================

// module
#include "App/Bag/include/BagFrame.h"
#include "App/AppEvent/include/Bag/BagGamedata.h"
#include "BagDraw.h"
#include "BagPokemon.h"
#include "App/FieldBag/include/BagInfoField.h"
#include "App/FieldBag/include/BagLvupWindow.h"
#include "BagWazaSelect.h"
#include "App/FieldBag/include/BagSaleWindow.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <thread/include/gfl2_ThreadStatic.h>
// niji
#include "System/include/DeviceTurnWatcher.h"
#include "GameSys/include/GameData.h"
#include "Print/include/ItemMessage.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Ui/UIViewManager.h"
#include "PokeTool/include/ItemRecover.h"
#include "PokeTool/include/PokeToolForm.h"
#include "PokeTool/include/EvolutionUtil.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/CommonBag.gaix"
#include "arc_index/FieldBag.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/app/common_bag/CommonBag_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//! メインシーケンス
enum MainSequenceID
{
  SEQ_START = 0,
  SEQ_MAIN,

  SEQ_MSG_WAIT,
  SEQ_MSG_WAIT_POKE_SELECT,

  SEQ_MSG_WAIT_ITEM_SUB,
  SEQ_MSG_WAIT_ITEM_SUB_POKE_SELECT,

  SEQ_POKE_SET,

  SEQ_HP_RECOVER,
  SEQ_ALL_DEATH_RECOVER,
  SEQ_LEVEL_UP,
  SEQ_WAZA_SELECT,

  SEQ_USE_TOKUSEIKAPUSERU,
  SEQ_USE_FORM_CHANGE,
  SEQ_USE_WAZA_MACHINE,
  SEQ_USE_BEADS,
  SEQ_USE_ZCUBE_WAZA,

  SEQ_WAZA_RESULT,

  SEQ_POKE_ITEM_MOVE,

  SEQ_LIST_ITEM_MOVE,

  SEQ_BATTLE_RECOVER_UPDATE,
  SEQ_BATTLE_ITEM_MOVE,

  SEQ_SALE,

  SEQ_DEMO_MODE,

  SEQ_WAIT_END,
  SEQ_END
};

//! フィールドポケットID
const u32 BagFrame::FieldPocketID[PlayData::POCKET_DATA_MAX] =
{
  BAG_POCKET_ROTOPON,
  BAG_POCKET_DRUG,
  BAG_POCKET_NORMAL,
  BAG_POCKET_WAZA,
  BAG_POCKET_NUTS,
  BAG_POCKET_EVENT,
  BAG_POCKET_FREE,
  BAG_POCKET_BEADS,
};


typedef int (BagFrame::*pItemUseFunc)( u16 item );  //!< アイテム使用型

//! アイテム使用データ
typedef struct
{
  u32 itemID;         //!< アイテム番号
  pItemUseFunc func;  //!< 使用関数
}ITEM_USE_DATA;

//! アイテム使用データ
static const ITEM_USE_DATA UseCheckTable[] =
{
  { ITEM_ANANUKENOHIMO,   &BagFrame::ItemUse_ReturnField },     // あなぬけのひも
  { ITEM_AMAIMITU,        &BagFrame::ItemUse_ReturnField },     // あまいみつ
  { ITEM_GAKUSYUUSOUTI,   &BagFrame::ItemUse_Gakusyuusouti },   // がくしゅうそうち
  { ITEM_MUSIYOKESUPUREE, &BagFrame::ItemUse_Spray },           // むしよけスプレー
  { ITEM_SIRUBAASUPUREE,  &BagFrame::ItemUse_Spray },           // シルバースプレー
  { ITEM_GOORUDOSUPUREE,  &BagFrame::ItemUse_Spray },           // ゴールドスプレー
  { ITEM_TAMAGOHUKAPON,   &BagFrame::ItemUse_RotomPower },      // タマゴふかポン
  { ITEM_YASUURIPON,      &BagFrame::ItemUse_RotomPower },      // やすうりポン
  { ITEM_OKODUKAIPON,     &BagFrame::ItemUse_RotomPower },      // おこづかいポン
  { ITEM_KEIKENTIPON,     &BagFrame::ItemUse_RotomPower },      // けいけんちポン
  { ITEM_NATUKIPON,       &BagFrame::ItemUse_RotomPower },      // なつきポン
  { ITEM_SOUGUUPON,       &BagFrame::ItemUse_RotomPower },      // そうぐうポン
  { ITEM_ONMITUPON,       &BagFrame::ItemUse_RotomPower },      // おんみつポン
};



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
BagFrame::BagFrame(void)
  : m_pItemManager( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_gameManager( NULL )
  , m_gameData( NULL )
  , m_myItem( NULL )
  , m_unionPokemon( NULL )
  , m_misc( NULL )
  , m_zukan( NULL )
  , m_eventWork( NULL )
  , m_playData( NULL )
  , m_placeLabelID( 0 )
  , m_pPokeIcon( NULL )
  , m_pItemIcon( NULL )
  , m_pocket( 0 )
  , m_pDraw( NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
  , m_rcvPokeSelectPos( 0 )
  , m_pBagPokemon( NULL )
  , m_pInfoField( NULL )
  , m_pFieldLytResBuff( NULL )
  , m_pFieldItemInfoBuff( NULL )
  , m_pLvupWindow( NULL )
  , m_pWazaSelect( NULL )
  , m_pSaleWindow( NULL )
  , m_pMenuWindow( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_pPaneListView( NULL )
  , m_pCommonIcon( NULL )
  , m_pokeSelectMode( POKESEL_MODE_NONE )
  , m_yesnoResult( MENU_ID_MAX )
  , m_newTokusei( TOKUSEI_NULL )
  , m_unionPos( 0 )
  , m_formChangeID( FORM_CHG_ID_ITEM_USE )
  , m_isRcvDead( false )
  , m_rcvHpCount( 0 )
  , m_rcvWazaPos( 0 )
  , m_rcvPos( -1 )
  , m_lvupSameLevelIndex( 0 )
  , m_lvupNewWaza( WAZANO_NULL )
  , m_battleStopFlag( false )
  , m_battleRecoverMode( RCV_USE )
  , m_pTypeSortTable( NULL )
  , m_pSortItem( NULL )
  , m_pSortWaza( NULL )
  , m_upperItem( ITEM_DUMMY_DATA )
  , m_upperPutCount( START_ITEM_NEW_RESET_COUNT )
  , m_listUpdateLock( false )
  , m_cpyPocketItem( NULL )
  , m_cpyFreeSpaceItem( NULL )
  , m_isExScroll( false )
  , m_exScrollVec( 0 )
  , m_catchMovePocket( 0 )
  , m_catchMovePocketCount( 0 )
  , m_listCatchPocket( 0 )
  , m_listCatchPos( 0 )
  , m_listCatchScroll( 0.0f )
  , m_isUpdateCatch( false )
  , m_pWordSetLoader( NULL )
  , m_isDemoMode( false )
  , m_demoReq( REQ_SELECT_BALL_POCKET )
  , m_useItem( ITEM_DUMMY_DATA )
  , m_updatePlateLock( false )
  , m_zCubeSelectWazaNo( WAZANO_NULL )
  , m_pDevTurnWatcher( NULL )
  , m_isSasiosaeErr( false )
  , m_pItemEffect( NULL )
  , m_isCatchItemPut( false )
  , m_touchPanel( NULL )
  , m_isUIViewInputExclusiveKey( false )
  , m_isFrameDrawEnable( false )
  , m_pokeItemCatchPos( pml::PokeParty::MAX_MEMBERS )
  , m_pocketOnBit( 0 )
  , m_rotomPower( NULL )
{
  gfl2::std::MemClear( &m_formChangeWork, sizeof(FORM_CHG_WORK) );
  for( u32 i=0; i<ZCUBE_WAZA_MAX; i++ )
  {
    m_zCubeWazaTable[i] = WAZANO_NULL;
  }
  GetInputExculusiveKeyMode();
  SetInputExculusiveKeyMode( true );

//  InitDemoMode();   // 捕獲デモテスト
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
BagFrame::~BagFrame()
{
  SetInputExculusiveKeyMode( m_isUIViewInputExclusiveKey );
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   外部設定パラメータをセット
 * @date    2015.03.04
 *
 * @param   param             外部設定パラメータ
 * @parma   heap              ヒープ
 * @param   rendering_manager 描画マネージャー
 */
//-----------------------------------------------------------------------------
void BagFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager )
{
  m_appParam         = param;
  m_heap             = heap;
  m_renderingManager = rendering_manager;

  m_useItem = m_appParam->select_item;

  if( m_appParam->call_mode == CALL_SHOP && m_appParam->pokemon != NULL )
  {
    GFL_ASSERT( 0 );
    m_appParam->pokemon = NULL;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if PM_DEBUG
#if 1
void BagFrame::Degug_CreateItemData(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    if( m_appParam->pocket[0].table == NULL )
    {
      for( u32 i=0; i<m_appParam->pocket_max; i++ )
      {
        u32 max;
        m_appParam->pocket[i].table = m_myItem->GetPocketItem( i, &max );
        m_appParam->pocket[i].max = m_myItem->GetItemCount( i );
      }
    }
  }
}
#endif
#endif


//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.03.04
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BagFrame::InitFunc(void)
{
  do {

    switch( m_mainSeq )
    {
    case 0:
      InitFadeIn();
      GetGameData();
      m_mainSeq++;
      /* FALL THROUGH */
    case 1:
      if( CreateItemManager() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 2:
      if( OpenMsgFile() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 3:
      if( CreateWordSetLoader() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 4:
      RecoverEvolutionItem();
      CreateItemTable();
      CreatePaneList();
      m_mainSeq++;
      /* FALL THROUGH */
    case 5:
      if( LoadLayoutResource() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 6:
      if( LoadMessage() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 7:
      if( LoadFieldLayoutResource() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 8:
      if( CreateMenuCursor() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 9:
      if( CreateItemIcon() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 10:
      if( CreatePokeIcon() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 11:
      if( CreateCommonIcon() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 12:
      if( CreateSortData() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 13:
      if( CreateEffect() == false )
      {
        break;
      }
      m_mainSeq++;
      /* FALL THROUGH */
    case 14:
      if( CreateBagDraw() == false )
      {
        break;
      }
      CreateFieldInfo();
      CreateSaleWindow();
      CreateLvupWindow();
      CreateWazaSelect();
      CreateMenuWindow();
      m_mainSeq++;
      /* FALL THROUGH */
    case 15:
      if( CreatePokemonPlate() == false )
      {
        break;
      }
      CreateDeviceTurnWatcher();
      InitPaneList();
      InitDisplay();
      m_mainSeq = 0;
      return applib::frame::RES_FINISH;

    default:
      GFL_ASSERT( 0 );
      break;
    }

    if( m_appParam->call_mode != CALL_BATTLE_MENU )
    {
      gfl2::thread::ThreadStatic::CurrentSleep( 1 );  // Sleep
    }

  }while( m_appParam->call_mode != CALL_BATTLE_MENU );

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.03.04
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BagFrame::EndFunc(void)
{
  switch( m_mainSeq )
  {
  case 0:
    if( DeleteWazaSelect() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    DeleteDeviceTurnWatcher();
    DeleteLvupWindow();
    DeleteSaleWindow();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( DeleteMenuWindow() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeleteFieldInfo() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeletePokemonPlate() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeleteBagDraw() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeletePokeIcon() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeleteItemIcon() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeleteMenuCursor() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    if( DeleteEffect() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    DeleteCommonIcon();
    UnloadFieldLayoutResource();
    UnloadMessage();
    UnloadLayoutResource();
    DeleteSortData();
    DeletePaneList();
    DeleteItemTable();
    DeleteWordSetLoader();
    m_mainSeq++;
    /* FALL THROUGH */
  case 3:
    if( CloseMsgFile() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 4:
    if( DeleteItemManager() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    m_isFrameDrawEnable = false;
    m_mainSeq = 0;
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.03.04
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BagFrame::UpdateFunc(void)
{
/*
  if( IsStopUpdate() != false )
  {
    static u32 cnt = 0;
    cnt = ( cnt + 1 ) & 0x1f;
    if( cnt == 0 )
    {
      m_appParam->select_item = ITEM_PIIPIIEIDO;
      RecoverUpdate( static_cast<RecoverMode>(RCV_USE) );
    }
  }
*/

/*
#if PM_DEBUG
  DebugUpdateDemoMode();  // 捕獲デモテスト
#endif
*/

//  InitPokeIcon();

  m_pDraw->UpdateTree();
  UpdateItemIcon();
  UpdatePocketNewIcon();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    Update_SeqStart();
    {
      GFL_PRINT( "■SEQ_START:\n" );
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
    break;
  case SEQ_MAIN:
    break;
  case SEQ_MSG_WAIT:
    Update_SeqMessageWait();
    break;
  case SEQ_MSG_WAIT_POKE_SELECT:
    Update_SeqMessageWaitPokeSelect();
    break;

  case SEQ_MSG_WAIT_ITEM_SUB:
    Update_SeqMessageWaitItemSub();
    break;
  case SEQ_MSG_WAIT_ITEM_SUB_POKE_SELECT:
    Update_SeqMessageWaitItemSubPokeSelect();
    break;

  case SEQ_POKE_SET:
    Update_SeqItemPokeSet();
    break;

  case SEQ_HP_RECOVER:
    Update_SeqHpRecover();
    break;

  case SEQ_ALL_DEATH_RECOVER:
    Update_SeqAllDeathRecover();
    break;

  case SEQ_LEVEL_UP:
    Update_SeqLevelUp();
    break;

  case SEQ_WAZA_SELECT:
    Update_SeqWazaSelect();
    break;

  case SEQ_USE_TOKUSEIKAPUSERU:
    Update_SeqItemUseTokuseikapuseru();
    break;

  case SEQ_USE_FORM_CHANGE:
    Update_SeqItemUseFormChange();
    break;

  case SEQ_USE_WAZA_MACHINE:
    Update_SeqItemUseWazaMachine();
    break;

  case SEQ_USE_BEADS:
    Update_SeqItemUseBeads();
    break;

  case SEQ_USE_ZCUBE_WAZA:
    Update_SeqItemUseZCubeWaza();
    break;

  case SEQ_WAZA_RESULT:
    Update_SeqWazaResult();
    break;

  case SEQ_POKE_ITEM_MOVE:
    Update_SeqPokeItemMove();
    break;

  case SEQ_LIST_ITEM_MOVE:
    Update_SeqListItemMove();
    break;

  case SEQ_BATTLE_RECOVER_UPDATE:
    Update_SeqBattleRecoverMode();
    break;

  case SEQ_BATTLE_ITEM_MOVE:
    Update_SeqBattleItemMove();
    break;


  case SEQ_SALE:
    Update_SeqSale();
    break;

  case SEQ_DEMO_MODE:
    Update_SeqDemoMode();
    break;

  case SEQ_WAIT_END:
    Update_SeqWaitEnd();
    break;
  case SEQ_END:
    return applib::frame::RES_FINISH;
  }
  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.03.04
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void BagFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsDrawEnable
 * @brief   描画できる状態になった事を返すための関数
 * @date    2016.04.20
 *
 * @retval  true  = 描画可
 * @retval  false = 描画不可
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsDrawEnable(void)
{
  return m_isFrameDrawEnable;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力リスナー
 * @date    2015.03.04
 *
 * @param   id  入力ID
 */
//-----------------------------------------------------------------------------
void BagFrame::OnAction( u32 id )
{
  switch( id )
  {
  case BagDraw::ACTION_B_BUTTON:
    SelectCancel();
    break;

  case BagDraw::ACTION_POCKET_00:
  case BagDraw::ACTION_POCKET_01:
  case BagDraw::ACTION_POCKET_02:
  case BagDraw::ACTION_POCKET_03:
  case BagDraw::ACTION_POCKET_04:
  case BagDraw::ACTION_POCKET_05:
  case BagDraw::ACTION_POCKET_06:
  case BagDraw::ACTION_POCKET_07:
    ChangePocket( id-BagDraw::ACTION_POCKET_00, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );
    break;
  case BagDraw::ACTION_POCKET_LEFT:
    MovePocket( -1 );
    break;
  case BagDraw::ACTION_POCKET_RIGHT:
    MovePocket( 1 );
    break;
  case BagDraw::ACTION_TOUCH_ON_PLATE:
    SetVisibleMenuCursor( false );
    break;
  case BagDraw::ACTION_TOUCH_OFF_PLATE:
    SetVisibleMenuCursor( true );
    break;
  case BagDraw::ACTION_SELECT_PLATE:
    SelectPokemon( m_pBagPokemon->GetSelectPos() );
    break;
  case BagDraw::ACTION_GET_POKE_ITEM:
    GetPokemonItem();
    break;
  case BagDraw::ACTION_PUT_POKE_ITEM:
//    PutPokemonItem();
    m_isCatchItemPut = true;
    break;
  case BagDraw::ACTION_PUT_LIST_ITEM:
//    PutListItem();
    m_isCatchItemPut = true;
    break;
  case BagDraw::ACTION_MENU_00:
  case BagDraw::ACTION_MENU_01:
  case BagDraw::ACTION_MENU_02:
  case BagDraw::ACTION_MENU_03:
  case BagDraw::ACTION_MENU_04:
  case BagDraw::ACTION_MENU_05:
  case BagDraw::ACTION_MENU_06:
  case BagDraw::ACTION_MENU_07:
    SelectMenu( id-BagDraw::ACTION_MENU_00 );
    break;
  case BagDraw::ACTION_SORT:
    // 入れ替え時は無効
    if( m_pDraw->IsItemChangeMode() == false )
    {
      SetSortMenu();
    }
    {
      GFL_PRINT( "■BagDraw::ACTION_SORT\n" );
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
    break;
  case BagDraw::ACTION_IREKAE:
    if( m_pDraw->IsItemChangeMode() == false )
    {
      StartItemChange();
    }
    else
    {
      EndItemChange( true, true );
    }
    break;

/*
  case BagDraw::ACTION_EX_LIST_UP:
    ScrollItemListEx( -1 );
    break;

  case BagDraw::ACTION_EX_LIST_DOWN:
    ScrollItemListEx( 1 );
    break;
*/
  }
}

//--------------------------------------------------------------------------
/**
 * @func    UpdateCursorItem
 * @brief   カーソルの指すアイテムが変更されたときのリスナー
 * @date    2015.03.04
 *
 * @param   item  道具データ
 */
//--------------------------------------------------------------------------
void BagFrame::UpdateCursorItem( const Savedata::MyItem::ITEM_ST * item )
{
  if( m_listUpdateLock != false )
  {
    return;
  }

  if( m_pInfoField != NULL )
  {
    m_pInfoField->UpdateInfo( item );
  }

  if( item == NULL )
  {
    if( m_updatePlateLock == false )
    {
      m_pBagPokemon->InitPlate( m_pItemManager, ITEM_DUMMY_DATA, m_pBagPokemon->IsPutParty() );
    }
    else
    {
      m_updatePlateLock = false;
    }
  }
  else
  {
    if( m_updatePlateLock == false )
    {
      m_pBagPokemon->InitPlate( m_pItemManager, item->id, m_pBagPokemon->IsPutParty() );
    }
    else
    {
      m_updatePlateLock = false;
    }
    ChangeItemIconRequest( item->id, ITEMICON_ID_INFO );
  }

  if( m_appParam->call_mode == CALL_SHOP )
  {
    m_pDraw->PutPrice( GetPrice(item) );
  }
}

//--------------------------------------------------------------------------
/**
 * @func    SelectItem
 * @brief   アイテムが選択されたときのリスナー
 * @date    2015.03.04
 *
 * @param   item  道具データ
 */
//--------------------------------------------------------------------------
void BagFrame::SelectItem( Savedata::MyItem::ITEM_ST * item )
{
  if( m_pDraw->IsItemChangeMode() != false )
  {
    EndItemChange( true, false );
    return;
  }

//  m_pDraw->ChangePaneListItemColor( true );

  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
//    SetEnd( END_SELECT, item->id );
    StopUpdate( item->id );
  }
  else
  {
    ChangeItemIconRequest( item->id, ITEMICON_ID_INFO );
    m_pInfoField->UpdateInfo( item );
    m_pBagPokemon->InitPlate( m_pItemManager, item->id, m_pBagPokemon->IsPutParty() );
    ResetNewItemFlag( item );

    const pml::pokepara::PokemonParam * pp = NULL;
    app::bag::BagDraw::MsgID msg_id;
    u32 menu_max;
    if( m_appParam->call_mode == CALL_FIELD_MENU )
    {
      menu_max = MakeMenuTable( item->id );
      pp = NULL;
      msg_id = BagDraw::MSG_ID_ITEM_MENU;
    }
    else if( m_appParam->call_mode == CALL_FIELD_POKELIST )
    {
      m_pBagPokemon->SetSelectPos();
      menu_max = MakeSingleMenuTable( item->id );
      pp = m_appParam->pokemon->GetPokeConst( m_appParam->pokemon_index );
      msg_id = BagDraw::MSG_ID_ITEM_CATCH_MENU;
    }
    else if( m_appParam->call_mode == CALL_BOX )
    {
      m_pDraw->SetDispActive( false );
      ChangeModeMessage();
      // 持たせられる
      if( IsPokeSetItem( item->id ) != false )
      {
        m_pBagPokemon->SetSelectPos();
        m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ITEM, item->id );
        m_subSeq = 1;
        SetSubSeq( SEQ_POKE_SET );
      }
      // ビーズ
      else if( item::ITEM_CheckBeads( item->id ) != false )
      {
        m_pBagPokemon->SetSelectPos();
        ItemUseBeads( item->id, m_appParam->pokemon->GetPoke(m_appParam->pokemon_index), false );
      }
      else
      {
        m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ERROR_ITEM, item->id );
        SetSubSeq( SEQ_MSG_WAIT );
      }
      return;
    }
    else if( m_appParam->call_mode == CALL_SHOP )
    {
      m_pDraw->PutPrice( GetPrice(item) );
      SelectSaleItem( item );
      return;
    }
    else
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( menu_max == 1 )
    {
      m_pDraw->SetDispActive( false );
      m_pDraw->PrintMessage( BagDraw::MSG_ID_COLLECT_ITEM );
      ChangeModeMessage();
      SetSubSeq( SEQ_MSG_WAIT );
    }
    else
    {
      m_pDraw->PutMessage( msg_id, item, pp );
      StartMenuWindow( menu_max );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @func    CatchItem
 * @brief   ペインリストの項目キャッチ時のリスナー
 * @date    2015.05.11
 *
 * @param   item  道具データ
 */
//--------------------------------------------------------------------------
void BagFrame::CatchItem( Savedata::MyItem::ITEM_ST * item )
{
  // カーソル位置記憶
  m_listCatchPocket = m_pocket;
  m_pPaneListView->GetPaneList()->GetCursorData( &m_listCatchPos, &m_listCatchScroll );

  // Newリセット
  ResetNewItemFlag( GetCatchListItem() );

  // 項目色変更
  m_pDraw->SetCatchItemChangeMode( true );

  { // アイコン移動
    u16 x, y;
    m_touchPanel->GetXY( &x, &y );
    m_pDraw->MoveItemIcon( x, y );
    m_pDraw->ChangeScrollPutData();
  }

  ChangeItemIconRequest( item->id, ITEMICON_ID_CATCH );
  if( m_pInfoField != NULL )
  {
    ChangeItemIconRequest( item->id, ITEMICON_ID_INFO );
    m_pInfoField->UpdateInfo( item->id, item->num );
  }

  // プレート変更
  m_pBagPokemon->InitPlate( m_pItemManager, item->id, m_pBagPokemon->IsPutParty() );
  m_updatePlateLock = false;

  { // ポケットのボタンをパッシブにする
    u32 bit = 0xffffffff;
    if( m_appParam->call_mode == CALL_BATTLE_MENU )
    {
      bit ^= ( 1 << m_pocket );
    }
    else
    {
      bit ^= ( 1 << PlayData::FREE_SPACE_POCKET );
      bit ^= ( 1 << GetFieldPocketNum(m_pItemManager->GetParam(item->id,item::ITEM_DATA::PRM_ID_F_POCKET)) );
    }
    m_pDraw->SetPocketButtonActive( bit, false );
  }
  ChangeModeListItemCatch();
  m_listUpdateLock = true;
  m_subSeq = 0;
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    SetSubSeq( SEQ_BATTLE_ITEM_MOVE );
  }
  else
  {
    SetSubSeq( SEQ_LIST_ITEM_MOVE );
  }

  m_pDraw->SetInputEnabled( false );    // @fix niji NMCat[98]: 入力できないようにする
  m_pDraw->SetPaneListEnable( false );  // @fix niji NMCat[98]: 入力できないようにする
}

//--------------------------------------------------------------------------
/**
 * @func    StopUpdate
 * @brief   内部処理を一時停止（バトル用）
 * @date    2015.04.23
 *
 * @param   item  道具データ
 */
//--------------------------------------------------------------------------
void BagFrame::StopUpdate( u32 item )
{
  m_pBagPokemon->SetInputEnabled( false );
  m_pDraw->SetInputEnabled( false );
  m_pDraw->SetPaneListEnable( false );

  m_appParam->select_item        = item;
  m_appParam->select_skill       = WAZANO_NULL;
  m_appParam->select_pokemon_pos = 0;
  m_appParam->select_skill_pos   = 0;
  m_appParam->pocket_no          = m_pocket;

  m_battleStopFlag = true;  // 一時停止

  SetInputExculusiveKeyMode( m_isUIViewInputExclusiveKey );
}

//--------------------------------------------------------------------------
/**
 * @func    IsStopUpdate
 * @brief   内部処理が一時停止しているか（バトル用）
 * @date    2015.04.23
 *
 * @retval  true  = 停止中
 * @retval  false = それ以外
 */
//--------------------------------------------------------------------------
bool BagFrame::IsStopUpdate(void)
{
  return m_battleStopFlag;
}

//--------------------------------------------------------------------------
/**
 * @func    RecoverUpdate
 * @brief   外部から内部処理を復帰させる（バトル用）
 * @date    2015.04.23
 *
 * @param   mode  復帰モード
 */
//--------------------------------------------------------------------------
void BagFrame::RecoverUpdate( RecoverMode mode )
{
  if( m_battleStopFlag == false )
  {
    GFL_ASSERT( 0 );
  }
  m_battleStopFlag = false;
  m_battleRecoverMode = mode;
  SetSubSeq( SEQ_BATTLE_RECOVER_UPDATE );

  SetInputExculusiveKeyMode( true );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    GetInputExculusiveKeyMode
 * @brief   UIViewのボタンアニメ中の処理モードを取得
 * @date    2016.02.18
 */
//-----------------------------------------------------------------------------
void BagFrame::GetInputExculusiveKeyMode(void)
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  m_isUIViewInputExclusiveKey = man->GetInputExclusiveKeyByButtonAnimation();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputExculusiveKeyMode
 * @brief   UIViewのボタンアニメ中の処理モードを設定
 * @date    2016.02.18
 *
 * @param   flg   true = 排他制御ON
 */
//-----------------------------------------------------------------------------
void BagFrame::SetInputExculusiveKeyMode( bool flg )
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  man->SetInputExclusiveKeyByButtonAnimation( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ取得
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagFrame::GetGameData(void)
{
  m_gameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  m_gameData     = m_gameManager->GetGameData();
  m_myItem       = m_gameData->GetMyItem();
  m_unionPokemon = m_gameData->GetUnionPokemon();
  m_misc         = m_gameData->GetMisc();
  m_zukan        = m_gameData->GetZukanData();
  m_eventWork    = m_gameData->GetEventWork();
  m_playData     = m_gameData->GetBagPlayData();
  m_touchPanel   = m_gameManager->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  m_rotomPower   = m_gameManager->GetRotomPowerManager();

  // @note デバッグバトルがZoneIDを入れてないみたいなので対処（バトルでは必要ないパラメータ）
  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    Field::ZoneDataAccessor accessor( m_gameData->GetFieldZoneDataLoader()->GetZoneData(m_gameData->GetNowZoneID()) );
    m_placeLabelID = accessor.GetPlaceLabelID();
  }

/*
#if PM_DEBUG
  {
    Savedata::MyItem::ITEM_ST * st = m_myItem->GetItem( ITEM_IIKIZUGUSURI );
    st->new_flag = true;
  }
#endif
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFadeIn
 * @brief   フェードイン初期設定
 * @date    2015.06.09
 */
//-----------------------------------------------------------------------------
void BagFrame::InitFadeIn(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );

  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
  }
  else if( m_appParam->pocket_max != BATTLE_BAG_POCKET_MAX )
  {
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_LOWER, &color );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2015.06.09
 */
//-----------------------------------------------------------------------------
void BagFrame::SetFadeIn(void)
{
  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
  }
  else if( m_appParam->pocket_max != BATTLE_BAG_POCKET_MAX )
  {
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2015.06.12
 */
//-----------------------------------------------------------------------------
void BagFrame::SetFadeOut(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    if( m_appParam->pocket_max != BATTLE_BAG_POCKET_MAX )
    {
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestDefaultOut( gfl2::Fade::DISP_LOWER );
    }
  }
  else
  {
    static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2015.06.12
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsFadeEnd(void)
{
  if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_UPPER) == false ||
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_LOWER) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEnd
 * @brief   終了設定
 * @date    2015.04.10
 *
 * @param   mode  終了モード
 * @param   item  選択されたアイテム
 * @param   waza  選択された技
 * @param   poke  選択されたポケモン位置
 * @param   pos   選択された技位置
 */
//-----------------------------------------------------------------------------
void BagFrame::SetEnd( EndMode mode, u32 item, u32 waza, u32 poke, u32 pos )
{
  // 入力停止
  m_pDraw->SetInputEnabled( false );
  m_pDraw->SetPaneListEnable( false );
  m_pBagPokemon->SetInputEnabled( false );

  m_appParam->end_mode           = mode;
  m_appParam->select_item        = item;
  m_appParam->select_skill       = waza;
  m_appParam->select_pokemon_pos = poke;
  m_appParam->select_skill_pos   = pos;
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    m_appParam->pocket_no = m_pocket;
  }
  else
  {
    m_appParam->pocket_no = GetFieldPocketID( m_pocket );
    m_playData->SetPocketPos( m_appParam->pocket_no );
  }
  SetPocketData( m_pocket );
  m_pDraw->StartOutAnime();
  if( m_pInfoField != NULL )
  {
    m_pInfoField->StartOutAnime();
  }
  SetSubSeq( SEQ_WAIT_END );

  {
    GFL_PRINT( "■SetEnd:\n" );
    GFL_PRINT(
      "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
      m_heap->GetSystemHeap()->GetTotalFreeSize(),
      m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
    GFL_PRINT(
      "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
      m_heap->GetDeviceHeap()->GetTotalFreeSize(),
      m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemManager
 * @brief   アイテムマネージャ生成
 * @date    2015.06.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateItemManager(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->item_manager != NULL )
    {
      m_pItemManager = m_appParam->item_manager;
      return true;
    }
    m_pItemManager = GFL_NEW(m_heap->GetDeviceHeap()) itemman::ITEM_MANAGER( m_heap->GetSystemHeap(), Savedata::MyItem::TOTAL_ITEM_MAX, false );
    m_pItemManager->FileOpen( m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pItemManager->IsFileOpen() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteItemManager
 * @brief   アイテムマネージャ削除
 * @date    2015.06.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteItemManager(void)
{
  if( m_pItemManager == NULL )
  {
    return true;
  }

  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->item_manager != NULL )
    {
      m_pItemManager = NULL;
      return true;
    }
    m_pItemManager->FileClose( m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pItemManager->IsFileClose() == false )
    {
      break;
    }
    GFL_SAFE_DELETE( m_pItemManager );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemTable
 * @brief   ポケットごとの道具テーブルを生成
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateItemTable(void)
{
#if PM_DEBUG
#if 1
  Degug_CreateItemData();
#endif
#endif

  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    u32 i;
    for( i=0; i<m_appParam->pocket_max; i++ )
    {
			m_itemTable[i] = GFL_NEW_ARRAY(m_heap->GetDeviceHeap()) Savedata::MyItem::ITEM_ST*[m_appParam->pocket[i].max];
      m_itemTableMax[i] = m_appParam->pocket[i].max;

      Savedata::MyItem::ITEM_ST ** table = m_itemTable[i];
      for( u32 j=0; j<m_appParam->pocket[i].max; j++ )
      {
        table[j] = &m_appParam->pocket[i].table[j];
      }

      m_pocketNewMax[i] = 0;

      m_pocketOnBit |= (1<<i);
    }
    for( ; i<PlayData::POCKET_DATA_MAX; i++ )
    {
      m_itemTable[i] = NULL;
      m_pocketNewMax[i] = 0;
    }

    m_pocket = m_appParam->pocket_no;
    if( IsRotoponPocket() == false || m_appParam->pocket_max != BATTLE_BAG_POCKET_MAX )
    {
      m_pocketOnBit ^= 1;
      if( m_pocket == 0 )
      {
        m_pocket = 1;
      }
//      m_appParam->pocket_max -= 1;
    }
  }
  else
  {
    for( u32 i=0; i<GFL_NELEMS(FieldPocketID); i++ )
    {
      u32 id = GetFieldPocketID( i );
      // フリースペース
      if( id == BAG_POCKET_FREE )
      {
        m_itemTable[i] = GFL_NEW_ARRAY(m_heap->GetDeviceHeap()) Savedata::MyItem::ITEM_ST*[Savedata::MyItem::TOTAL_ITEM_MAX];
        m_itemTableMax[i] = m_myItem->MakeFreeSpaceItemTable( m_itemTable[PlayData::FREE_SPACE_POCKET], Savedata::MyItem::TOTAL_ITEM_MAX );
      }
      else
      {
        u32 max;
        m_myItem->GetPocketItem( id, &max );
        m_itemTable[i] = GFL_NEW_ARRAY(m_heap->GetDeviceHeap()) Savedata::MyItem::ITEM_ST*[max];
        m_itemTableMax[i] = m_myItem->MakePocketItemTable( id, m_itemTable[i], max );
      }
    }
    m_appParam->pocket_max = PlayData::POCKET_DATA_MAX;
    m_pocketOnBit = 0xff;
    u32 pocket = m_appParam->pocket_no;
    if( IsBeadsPocket() == false )
    {
      m_pocketOnBit ^= ( 1 << GetFieldPocketNum(BAG_POCKET_BEADS) );
//      m_appParam->pocket_max -= 1;
      if( m_appParam->pocket_no == BAG_POCKET_BEADS )
      {
        pocket = BAG_POCKET_DRUG;
      }
    }
    if( IsRotoponPocket() == false )
    {
      m_pocketOnBit ^= ( 1 << GetFieldPocketNum(BAG_POCKET_ROTOPON) );
//      m_appParam->pocket_max -= 1;
      if( m_appParam->pocket_no == BAG_POCKET_ROTOPON )
      {
        pocket = BAG_POCKET_DRUG;
      }
    }
    m_pocket = GetFieldPocketNum( pocket );

    for( u32 i=0; i<m_appParam->pocket_max; i++ )
    {
      SetCountNew( i );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteItemTable
 * @brief   ポケットごとの道具テーブルを削除
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteItemTable(void)
{
  for( u32 i=0; i<PlayData::POCKET_DATA_MAX; i++ )
  {
    GFL_SAFE_DELETE_ARRAY( m_itemTable[i] );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_COMMON_BAG, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_COMMON_BAG ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_COMMON_BAG,
      GARC_CommonBag_CommonBag_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_COMMON_BAG, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_COMMON_BAG ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayoutResource
 * @brief   レイアウトリソース削除
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.06.17
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_bag_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージ削除
 * @date    2015.06.17
 */
//-----------------------------------------------------------------------------
void BagFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreatePokeIcon
 * @brief   ポケモンアイコン生成
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreatePokeIcon(void)
{

  switch( m_subSeq )
  {
  case 0:
		m_pPokeIcon = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::PokeIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), pml::PokeParty::MAX_MEMBERS );
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      m_pPokeIcon->AllocTextureBuffer( i );
    }
    m_pPokeIcon->FileOpen( m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsFileOpen() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePokeIcon
 * @brief   ポケモンアイコン削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeletePokeIcon(void)
{
  if( m_pPokeIcon == NULL )
  {
    return true;
  }
  if( m_pPokeIcon->IsModuleFree() == false )
  {
    return false;
  }

  switch( m_subSeq )
  {
  case 0:
    m_pPokeIcon->FileClose(m_heap->GetSystemHeap());
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsFileClose() == false )
    {
      break;
    }
    GFL_SAFE_DELETE( m_pPokeIcon );
    m_subSeq = 0;
    return true;
  }

  return false;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemIcon
 * @brief   アイテムアイコン生成
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateItemIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pItemIcon = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::ItemIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), ITEMICON_ID_MAX );
    for( u32 i=0; i<ITEMICON_ID_MAX; i++ )
    {
      m_pItemIcon->AllocTextureBuffer( i );
    }
    m_pItemIcon->FileOpen( m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pItemIcon->IsFileOpen() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteItemIcon
 * @brief   アイテムアイコン削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteItemIcon(void)
{
  if( m_pItemIcon == NULL )
  {
    return true;
  }
  if( m_pItemIcon->IsModuleFree() == false )
  {
    return false;
  }

  switch( m_subSeq )
  {
  case 0:
    m_pItemIcon->FileClose(m_heap->GetSystemHeap());
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pItemIcon->IsFileClose() == false )
    {
      break;
    }
    GFL_SAFE_DELETE( m_pItemIcon );
    m_subSeq = 0;
    return true;
  }

  return false;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateCommonIcon
 * @brief   状態異常/タイプ/分類アイコン生成
 * @date    2015.07.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateCommonIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pCommonIcon = GFL_NEW(m_heap->GetSystemHeap()) app::tool::AppCommonGrpIconData();
    m_pCommonIcon->Initialize( m_heap );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pCommonIcon->IsReady() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteCommonIcon
 * @brief   状態異常/タイプ/分類アイコン削除
 * @date    2015.07.02
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteCommonIcon(void)
{
  if( m_pCommonIcon != NULL )
  {
    m_pCommonIcon->Terminate();
    GFL_SAFE_DELETE( m_pCommonIcon );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   メニューカーソル生成
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateMenuCursor(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorBuffer );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuCursor->LoadResource() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   メニューカーソル削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteMenuCursor(void)
{
  if( m_pMenuCursor == NULL )
  {
    return true;
  }
  if( m_pMenuCursor->IsModuleFree() == false )
  {
    return false;
  }
  m_pMenuCursor->DeleteResource();
  GFL_SAFE_DELETE( m_pMenuCursor );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleMenuCursor
 * @brief   メニューカーソル表示切り替え
 * @date    2015.04.24
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagFrame::SetVisibleMenuCursor( bool flg )
{
  if( flg != false && m_itemTableMax[m_pocket] == 0 )
  {
    flg = false;
  }
  m_pMenuCursor->SetVisible( flg );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    LoadFieldLayoutResource
 * @brief   フィールド関連のリソース読み込み
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::LoadFieldLayoutResource(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    return true;
  }

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_FIELD_BAG, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( ARCID_FIELD_BAG ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_FIELD_BAG,
      GARC_FieldBag_FieldBag_applyt_COMP,
      &m_pFieldLytResBuff,
			m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pFieldLytResBuff ) == false )
    {
      break;
    }
		app::util::FileAccessor::FileClose( ARCID_FIELD_BAG, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( ARCID_FIELD_BAG ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      print::GetMessageArcId(),
      print::ItemMessage::GetArcDatID(),
      &m_pFieldItemInfoBuff,
			m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:
    if( app::util::FileAccessor::IsFileLoad( &m_pFieldItemInfoBuff ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadFieldLayoutResource
 * @brief   フィールド関連のリソース削除
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::UnloadFieldLayoutResource(void)
{
  GflHeapSafeFreeMemory( m_pFieldItemInfoBuff );
  GflHeapSafeFreeMemory( m_pFieldLytResBuff );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    OpenMsgFile
 * @brief   メッセージファイルオープン
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::OpenMsgFile(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( print::GetMessageArcId(), m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( print::GetMessageArcId() ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseMsgFile
 * @brief   メッセージファイルクローズ
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CloseMsgFile(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileClose( print::GetMessageArcId(), m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileClose( print::GetMessageArcId() ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateBagDraw
 * @brief   描画クラス生成
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateBagDraw(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pDraw = GFL_NEW(m_heap->GetDeviceHeap()) BagDraw( m_heap, m_renderingManager, m_pCommonIcon, &m_isDemoMode );
    m_pDraw->CreateNumFont();
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->SetNumFont() == false )
    {
      break;
    }
    m_pDraw->Init( m_appParam, m_pLytResBuff, m_pMsgBuff, m_pWordSetLoader, m_pMenuCursor, &m_pocketOnBit );
    m_pDraw->SetItemEffect( m_pItemEffect );
    m_pDraw->SetDrawListener( this );
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteBagDraw
 * @brief   描画クラス削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteBagDraw(void)
{
  if( m_pDraw == NULL )
  {
    return true;
  }
  if( m_pDraw->End() == false )
  {
    return false;
  }
  m_pDraw->UnsetItemEffect();
  m_pDraw->DeleteNumFont();
  m_pDraw->EndPaneList();
  GFL_SAFE_DELETE( m_pDraw );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreatePokemonPlate
 * @brief   ポケモン関連クラス生成
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreatePokemonPlate(void)
{
  switch( m_subSeq )
  {
  case 0:
    {
      bool is_catch = true;
      u32 single_pos = pml::PokeParty::MAX_MEMBERS;
      if( m_appParam->call_mode == CALL_BATTLE_MENU )
      {
        is_catch = false;
        if( m_appParam->pocket_max != BATTLE_BAG_POCKET_MAX )
        {
          single_pos = m_appParam->pokemon_index;
        }
      }
      else if( m_appParam->call_mode == CALL_FIELD_POKELIST || m_appParam->call_mode == CALL_BOX )
      {
        single_pos = m_appParam->pokemon_index;
      }
      m_pBagPokemon = GFL_NEW(m_heap->GetDeviceHeap()) BagPokemon( m_appParam->pokemon, m_pPokeIcon, m_pCommonIcon, m_heap, m_pDraw->GetG2DUtil(), is_catch, single_pos, &m_isDemoMode );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pBagPokemon->Init() == false )
    {
      break;
    }
    m_pBagPokemon->SetListener( m_pDraw );
    m_pDraw->AddSubView( m_pBagPokemon );
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePokemonPlate
 * @brief   ポケモン関連クラス削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeletePokemonPlate(void)
{
  if( m_pBagPokemon != NULL )
  {
    m_pBagPokemon->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pBagPokemon );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateFieldInfo
 * @brief   フィールド関連クラス生成
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateFieldInfo(void)
{
  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    app::util::G2DUtil * g2d = m_pDraw->GetG2DUtil();
    m_pInfoField = GFL_NEW(m_heap->GetDeviceHeap()) BagInfoField( m_heap, m_pItemManager, m_renderingManager, m_pCommonIcon );
    m_pInfoField->Setup( m_pFieldLytResBuff, m_pFieldItemInfoBuff, g2d->GetMsgData(), g2d->GetWordSet(), m_pWordSetLoader );
    m_pInfoField->UpdatePocketName( GetFieldPocketID(m_pocket) );
    m_pInfoField->ChangePocketBg( m_pocket );
    m_pInfoField->UpdateMyGold( m_misc->GetGold() );
    if( m_appParam->call_mode == CALL_SHOP )
    {
      m_pInfoField->VanishYButton();
    }
    m_pDraw->AddSubView( m_pInfoField );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFieldInfo
 * @brief   フィールド関連クラス削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteFieldInfo(void)
{
  if( m_pInfoField != NULL )
  {
    if( m_pInfoField->IsEnd() == false )
    {
      return false;
    }
    m_pInfoField->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pInfoField );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateSaleWindow
 * @brief   売却ウィンドウ生成
 * @date    2015.07.17
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateSaleWindow(void)
{
  if( m_appParam->call_mode == CALL_SHOP )
  {
    m_pSaleWindow = GFL_NEW(m_heap->GetDeviceHeap()) BagSaleWindow( m_heap, m_pDraw->GetG2DUtil() );
    m_pDraw->AddSubView( m_pSaleWindow );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteSaleWindow
 * @brief   売却ウィンドウ削除
 * @date    2015.07.17
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteSaleWindow(void)
{
  if( m_pSaleWindow != NULL )
  {
    m_pSaleWindow->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pSaleWindow );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateLvupWindow
 * @brief   レベルアップウィンドウ生成
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateLvupWindow(void)
{
  if( m_appParam->call_mode != CALL_BATTLE_MENU && m_appParam->call_mode != CALL_SHOP )
  {
    m_pLvupWindow = GFL_NEW(m_heap->GetDeviceHeap()) BagLvupWindow( m_heap, m_pDraw->GetG2DUtil(), &m_isDemoMode );
    m_pDraw->AddSubView( m_pLvupWindow );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteLvupWindow
 * @brief   レベルアップウィンドウ削除
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteLvupWindow(void)
{
  if( m_pLvupWindow != NULL )
  {
    m_pLvupWindow->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pLvupWindow );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateWazaSelect
 * @brief   技選択ウィンドウ生成
 * @date    2015.04.22
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateWazaSelect(void)
{
  app::util::G2DUtil * g2d = m_pDraw->GetG2DUtil();
	m_pWazaSelect = GFL_NEW(m_heap->GetDeviceHeap()) BagWazaSelect( m_appParam, m_heap, m_renderingManager, m_pCommonIcon, &m_isDemoMode );
  m_pWazaSelect->Setup( m_pLytResBuff, g2d->GetMsgData(), g2d->GetWordSet(), &m_pMenuCursorBuffer );
  m_pDraw->AddSubView( m_pWazaSelect );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteWazaSelect
 * @brief   技選択ウィンドウ削除
 * @date    2015.04.22
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteWazaSelect(void)
{
  if( m_pWazaSelect != NULL )
  {
    if( m_pWazaSelect->IsModuleEnd() == false )
    {
      return false;
    }
    m_pWazaSelect->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pWazaSelect );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuWindow
 * @brief   メニューウィンドウ生成
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateMenuWindow(void)
{
	m_pMenuWindow = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuWindow( m_heap, m_renderingManager, m_pMenuCursorBuffer );
  m_pMenuWindow->SetListener( m_pDraw );
  m_pDraw->AddSubView( m_pMenuWindow );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuWindow
 * @brief   メニューウィンドウ削除
 * @date    2015.04.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteMenuWindow(void)
{
  if( m_pMenuWindow == NULL )
  {
    return true;
  }
  if( m_pMenuWindow->IsModuleFree() == false )
  {
    return false;
  }
  m_pMenuWindow->RemoveFromSuperView();
  GFL_SAFE_DELETE( m_pMenuWindow );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneList
 * @brief   ペインリスト生成
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::CreatePaneList(void)
{
  m_pPaneListView = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::PaneListView( m_heap, true );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePaneList
 * @brief   ペインリスト削除
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::DeletePaneList(void)
{
  GFL_SAFE_DELETE( m_pPaneListView );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateSortData
 * @brief   ソートデータ生成
 * @date    2015.04.28
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateSortData(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->call_mode != CALL_BATTLE_MENU )
    {
      m_pTypeSortTable = GFL_NEW_ARRAY(m_heap->GetDeviceHeap()) u32[Savedata::MyItem::TOTAL_ITEM_MAX];
      m_pSortItem = GFL_NEW(m_heap->GetDeviceHeap()) app::sort::StringItem();
      m_pSortWaza = GFL_NEW(m_heap->GetDeviceHeap()) app::sort::StringWaza();
    }
    else
    {
      return true;
    }
    m_pSortItem->LoadSetup( m_heap->GetDeviceHeap(), m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pSortItem->LoadMain() == false )
    {
      break;
    }
    m_pSortWaza->LoadSetup( m_heap->GetDeviceHeap(), m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pSortWaza->LoadMain() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteSortData
 * @brief   ソートデータ削除
 * @date    2015.04.28
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteSortData(void)
{
  GFL_SAFE_DELETE( m_pSortWaza );
  GFL_SAFE_DELETE( m_pSortItem );
  GFL_SAFE_DELETE_ARRAY( m_pTypeSortTable );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateWordSetLoader
 * @brief   ワードセットローダー生成
 * @date    2015.06.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateWordSetLoader(void)
{
  switch( m_subSeq )
  {
  case 0:
    {
      u32 id = 0;
      if( m_appParam->call_mode != CALL_BATTLE_MENU )
      {
        id = PRELOAD_ITEMNAME | PRELOAD_WAZANAME | PRELOAD_POCKETNAME;
      }
      else
      {
        id = PRELOAD_ITEMNAME | PRELOAD_WAZANAME;
      }
      m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), id );
      m_pWordSetLoader->StartLoad( m_heap->GetDeviceHeap() );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pWordSetLoader->WaitLoad() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteWordSetLoader
 * @brief   ワードセットローダー削除
 * @date    2015.06.25
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteWordSetLoader(void)
{
  GFL_SAFE_DELETE( m_pWordSetLoader );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateDeviceTurnWatcher
 * @brief   本体逆さチェッククラス生成
 * @date    2015.12.15
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateDeviceTurnWatcher(void)
{
  m_pDevTurnWatcher = GFL_NEW(m_heap->GetSystemHeap()) System::DeviceTurnWatcher( m_heap->GetSystemHeap(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDeviceTurnWatcher
 * @brief   本体逆さチェッククラス削除
 * @date    2015.12.15
 */
//-----------------------------------------------------------------------------
void BagFrame::DeleteDeviceTurnWatcher(void)
{
  GFL_SAFE_DELETE( m_pDevTurnWatcher );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModeMenu
 * @brief   モード切り替え：メニューへ
 * @date    2015.04.29
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModeMenu(void)
{
  m_pBagPokemon->SetInputEnabled( false );
  m_pDraw->SetInputEnabled( false );
  m_pDraw->SetPaneListEnable( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModePokemonSelect
 * @brief   モード切り替え：ポケモン選択へ
 * @date    2015.04.29
 *
 * @param   mode  選択モード
 * @param   pos   初期位置
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModePokemonSelect( PokeSelectMode mode, u32 pos )
{
  ChangePokeSelectMode( true, mode, pos );
  m_pBagPokemon->SetInputEnabled( true );
  m_pDraw->ChangeNormalMode( false );       // 道具選択無効
  m_pDraw->ChangePokeSelectMode( true );    // ポケモン選択モード有効
  m_pDraw->SetInputEnabled( true );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModeItemSelect
 * @brief   モード切り替え：道具選択へ
 * @date    2015.04.29
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModeItemSelect(void)
{
  ChangePokeSelectMode( false, POKESEL_MODE_NONE );
  m_pBagPokemon->SetInputEnabled( true );
  m_pDraw->ChangePokeSelectMode( false );   // ポケモン選択モード無効
  m_pDraw->ChangeNormalMode( true );        // 道具選択有効
  m_pDraw->SetPocketButtonActive( true );   // ポケット選択有効（入れ替え用）
  m_pDraw->SetInputEnabled( true );
  m_pDraw->ChangePaneListItemColor( false );
//  m_pDraw->SetPaneListEnable( true );     // m_pDraw->ChangeNormalMode(true)で実行される
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModeMessage
 * @brief   モード切り替え：メッセージ処理へ
 * @date    2015.04.29
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModeMessage(void)
{
  m_pBagPokemon->SetInputEnabled( false );
  m_pDraw->SetInputEnabled( false );
  m_pDraw->SetPaneListEnable( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModeItemChange
 * @brief   モード切り替え：道具入れ替えへ
 * @date    2015.04.30
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModeItemChange(void)
{
  m_pDraw->SetPocketButtonActive( false );
  m_pDraw->SetInputEnabled( true );
  m_pBagPokemon->SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModeItemCatchMove
 * @brief   モード切り替え：ポケモンの持ち物のキャッチムーブへ
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModeItemCatchMove(void)
{
  m_pBagPokemon->SetInputEnabled( false );
  m_pDraw->SetInputEnabled( false );
  m_pDraw->SetPaneListEnable( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeModeListItemCatch
 * @brief   モード切り替え：道具リストのキャッチムーブへ
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeModeListItemCatch(void)
{
//  m_pDraw->ChangeListItemCatchMode();
//  m_pBagPokemon->SetInputEnabled( false );
  m_pBagPokemon->ChangeListItemCatchMode();
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2015.04.13
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqStart(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_subSeq++;
    if( m_appParam->rcv_mode == RCV_WAZAOSHIE ||
        ( m_appParam->rcv_mode == RCV_EVOLUTION && m_appParam->select_item == ITEM_HUSIGINAAME && m_myItem->Check(ITEM_HUSIGINAAME,1) != false ) )
    {
      m_pBagPokemon->InitMode( m_appParam->select_pokemon_pos );
      break;
    }
    else
    {
      m_pBagPokemon->InitMode( pml::PokeParty::MAX_MEMBERS );
    }
    if( m_appParam->call_mode == CALL_BATTLE_MENU && m_appParam->pocket_max == BATTLE_BAG_POCKET_MAX )
    {
      break;
    }
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsInitPocketAnimeEnd() == false )
    {
      break;
    }
    m_pDraw->StartBattleMode();
    m_pDraw->StartShopMode();
    SetFadeIn();
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( IsFadeEnd() == false )
    {
      break;
    }
    m_isFrameDrawEnable = true;
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( m_pDraw->Start() == false )
    {
      break;
    }
    m_pBagPokemon->SetInputEnabled( true );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:
    m_subSeq = 0;
    if( m_appParam->rcv_mode == RCV_WAZAOSHIE )
    {
      ChangeModeMessage();
      SetSubSeq( SEQ_WAZA_RESULT );
    }
    else
    {
      /*
        @fix niji GFNMCat[1231]
        [1048]の修正でBagDraw::Start()でペインリストが有効になるため、止める
      */
      if( m_appParam->rcv_mode == RCV_EVOLUTION && m_pBagPokemon->IsPutParty() != false && m_appParam->select_item == ITEM_HUSIGINAAME && m_myItem->Check(ITEM_HUSIGINAAME,1) != false )
      {
        m_pDraw->SetPaneListEnable( false );
      }
      else
      {
        // 通常時はカーソル表示
        SetVisibleMenuCursor( true );
      }
      SetSubSeq( SEQ_MAIN );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMessageWait
 * @brief   アップデート：メッセージ待ち
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqMessageWait(void)
{
  if( m_pDraw->UpdatePrintMessage() == false )
  {
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    SetSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMessageWaitPokeSelect
 * @brief   アップデート：メッセージ待ち => ポケモン選択
 * @date    2015.04.17
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqMessageWaitPokeSelect(void)
{
  if( m_pDraw->UpdatePrintMessage() == false )
  {
    m_pDraw->SetDispActive( true );
    ChangeModePokemonSelect( m_pokeSelectMode, m_rcvPokeSelectPos );
    SetSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMessageWaitItemSub
 * @brief   アップデート：メッセージ待ち => 道具を減らす
 * @date    2016.03.07
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqMessageWaitItemSub(void)
{
  if( m_pDraw->UpdatePrintMessage() == false )
  {
    ItemUseSub( m_useItem );
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    SetSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMessageWaitItemSubPokeSelect
 * @brief   アップデート：メッセージ待ち => 道具を減らす => ポケモン選択
 * @date    2016.03.07
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqMessageWaitItemSubPokeSelect(void)
{
  if( m_pDraw->UpdatePrintMessage() == false )
  {
    ItemUseSub( m_useItem );
    m_pDraw->SetDispActive( true );
    ChangeModePokemonSelect( m_pokeSelectMode, m_rcvPokeSelectPos );
    SetSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqItemPokeSet
 * @brief   アップデート：ポケモンに道具を持たせる
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqItemPokeSet(void)
{
  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  switch( m_subSeq )
  {
  case 0:   // 終了へ
    break;

  case 1:   // アイテム更新
    SetPokemonItem( GetListPosItemID() );
    { // フォルムチェンジ
      u32 pos = m_pBagPokemon->GetSelectPos();
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );
      u32 item_id = pp->GetItem();
      if( CheckSetItemFormChange( pos, pp, item_id ) != false )
      {
        m_pBagPokemon->ChangePokeSelectMode( true, pos );
        SetFormChangeWork( pos, item_id );
        m_subSeq = 0;
        m_formChangeID = FORM_CHG_ID_ITEM_SET;
        SetSubSeq( SEQ_USE_FORM_CHANGE );
        return;
      }
    }
    break;

  case 2:   // 交換の"はい/いいえ"をセット
    StartMenuWindow( MakeYesNoMenuTable(), true );
    m_yesnoResult = MENU_ID_MAX;
    m_subSeq = 3;
    return;

  case 3:   // 交換の"はい/いいえ"分岐
    if( m_yesnoResult == MENU_ID_YES )
    {
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( m_pBagPokemon->GetSelectPos() );
      u32 item = pp->GetItem();
      if( item::ITEM_CheckPiece(item) != false || m_myItem->AddCheck(item,1) != false )
      {
        m_pDraw->PrintMessage( BagDraw::MSG_ID_CHANGE_ITEM, item, GetListPosItemID() );
        m_subSeq = 1;
      }
      else
      {
        m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ERROR_MAX );
        m_subSeq = 0;
      }
    }
    else if( m_yesnoResult == MENU_ID_NO )
    {
      break;
    }
    return;
  }

  // ボックスの場合は終了
  if( m_appParam->call_mode == CALL_BOX )
  {
    m_subSeq = 0;
    SetEnd( END_CANCEL );   // 終了モードはなんでもいいはず。
    SetFadeOut();
  }
  else
  {
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqHpRecover
 * @brief   アップデート：HP回復
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqHpRecover(void)
{
  u32 sel_pos = m_pBagPokemon->GetSelectPos();

  if( m_pBagPokemon->IsPlayHpRecoverAnime( sel_pos ) == false )
  {
    const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( sel_pos );
    if( m_isRcvDead == false )
    {
      s32 cnt = m_rcvHpCount;
      m_pDraw->PrintItemUseMessage( item::ITEM_TYPE_HP_RCV, pp, &cnt );
    }
    else
    {
      m_pDraw->PrintItemUseMessage( item::ITEM_TYPE_DEATH_RCV, pp );
    }
    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    if( m_isUpdateCatch == false )
    {
      // まだ消費してないので２個あれば続けられる
      if( m_myItem->Check( m_useItem, 2 ) == false )
      {
        SetSubSeq_MsgWaitItemSub( m_useItem );
      }
      else
      {
        SetSubSeq_MsgWaitItemSubPokeSelect( m_useItem, sel_pos );
      }
    }
    else
    {
      SetSubSeq_MsgWaitItemSub( m_useItem );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqAllDeathRecover
 * @brief   アップデート：全体瀕死回復
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqAllDeathRecover(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pBagPokemon->ChangePlateParty( true );
    m_subSeq = 1;
    /* FALL THROUGH */
  case 1:
    m_rcvPos = GetAllDeathRecoverUsePos( ITEM_SEINARUHAI );
    if( m_rcvPos == -1 )
    {
      Savedata::IncRecord( Savedata::Record::RECID_HEAL_KIDOU );  // レコード
      ItemUseSub( ITEM_SEINARUHAI );
      m_pBagPokemon->ChangePlateParty( false );
      ChangeModeItemSelect();
      SetSubSeq( SEQ_MAIN );
    }
    else
    {
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_rcvPos );
      u32 old_hp = pp->GetHp();
      PokeTool::ITEM_RCV_Recover( pp, ITEM_SEINARUHAI, 0, m_placeLabelID, m_pItemManager );
      m_pBagPokemon->StartHpRecoverAnime( m_rcvPos, pp, old_hp );
      m_pBagPokemon->ChangePokeSelectMode( true, m_rcvPos );
      m_pBagPokemon->VanishStatusIcon( m_rcvPos );
      Sound::PlaySE( SEQ_SE_RECOVERY );
      m_subSeq = 2;
    }
    break;

  case 2:
    if( m_pBagPokemon->IsPlayHpRecoverAnime(m_rcvPos) == false )
    {
      m_pDraw->PrintItemUseMessage( item::ITEM_TYPE_DEATH_RCV, m_appParam->pokemon->GetPoke(m_rcvPos) );
      m_pDraw->SetDispActive( false );
      m_subSeq = 3;
    }
    break;

  case 3:
    if( m_pDraw->UpdatePrintMessage() == false )
    {
      m_pDraw->SetDispActive( true );
      m_subSeq = 4;
    }
    break;

  case 4:
  default:
    // SEが被らないように空ループさせる
    m_subSeq++;
    if( m_subSeq == 8 )
    {
      m_subSeq = 1;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqLevelUp
 * @brief   アップデート：レベルアップ
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqLevelUp(void)
{
  m_pDevTurnWatcher->Update();

  switch( m_subSeq )
  {
  case 0:
    if( m_pDraw->UpdatePrintMessage() == false )
    {
      m_pLvupWindow->Start();
      m_subSeq = 1;
    }
    break;

  case 1:
    if( m_pLvupWindow->IsEnd() == false )
    {
      break;
    }
    m_subSeq = 2;
    /* FALL THROUGH */
  case 2:
    {
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke(m_pBagPokemon->GetSelectPos());
      for ever
      {
        switch( pp->LearnNewWazaOnCurrentLevel( &m_appParam->lvupWazaSetIndex, &m_lvupNewWaza ) )
        {
        case pml::pokepara::WAZA_LEARNING_SUCCEEDED:    // 成功：新しい技を覚えた
          m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_COMP, pp, m_lvupNewWaza );
          m_subSeq = 3;
          return;

        case pml::pokepara::WAZA_LEARNING_FAILED_FULL:  // 失敗：手持ち技がいっぱいで覚えられなかった
          m_pDraw->PrintLvupWazaOboeMessage( pp, m_lvupNewWaza );
          m_subSeq = 4;
          return;

        case pml::pokepara::WAZA_LEARNING_FAILED_SAME:  // 失敗：すでに覚えている
          break;

        case pml::pokepara::WAZA_LEARNING_FAILED_NOT_EXIST: // 失敗：覚える技が無かった
          ItemUseSub( m_useItem );

          {	// 進化チェック
            PokeTool::SetupEvolveSituation( &m_appParam->evoSituation, m_gameManager, m_pDevTurnWatcher->GetResult(), m_appParam->weather );
            pml::PokeParty * party = m_appParam->pokemon->GetPokeParty();
            MonsNo mons;
            u32 evo_root;
            if( pp->CanEvolve( &m_appParam->evoSituation, party, &mons, &evo_root ) == true )
            {
              m_subSeq = 0;
              SetEnd( END_CALL_EVOLUTION, ITEM_HUSIGINAAME, WAZANO_NULL, m_pBagPokemon->GetSelectPos() ); 
              SetFadeOut();
              return;
            }
          }

          m_pDraw->SetDispActive( true );
          if( m_myItem->Check( ITEM_HUSIGINAAME, 1 ) == false || m_pBagPokemon->IsPutParty() == false )
          {
            ChangeModeItemSelect();
          }
          else
          {
            m_pDraw->PutMessage( BagDraw::MSG_ID_USE_SELECT );
            ChangeModePokemonSelect( POKESEL_MODE_MENU_USE, m_pBagPokemon->GetSelectPos() );
          }
          m_appParam->lvupWazaSetIndex = 0;   // カウンタ初期化
          m_subSeq = 0;
          SetSubSeq( SEQ_MAIN );
          return;

  			default:
          GFL_ASSERT( 0 );
			  }
      }
    }

  case 3:   // 覚えた
    if( m_pDraw->UpdatePrintMessage() == false )
    {
      m_subSeq = 2;
    }
    break;

  case 4:   // 技忘れへ
    if( m_pDraw->UpdatePrintMessage() == false )
    {
      StartMenuWindow( MakeYesNoMenuTable(), true );
      m_yesnoResult = MENU_ID_MAX;
      m_subSeq = 5;
    }
    break;

  case 5:
    if( m_yesnoResult == MENU_ID_YES )
    {
      SetEnd( END_CALL_WAZA_WASURE, ITEM_HUSIGINAAME, m_lvupNewWaza, m_pBagPokemon->GetSelectPos() );
      SetFadeOut();
      m_subSeq = 0;
    }
    else if( m_yesnoResult == MENU_ID_NO )
    {
      m_subSeq = 2;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqWazaSelect
 * @brief   アップデート：技選択
 * @date    2015.04.22
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqWazaSelect(void)
{
  if( m_pWazaSelect->IsEnd() != false )
  {
    u32 result = m_pWazaSelect->GetResult();
    if( result < pml::MAX_WAZA_NUM )
    {
      u32 poke_pos = m_pBagPokemon->GetSelectPos();
      if( m_appParam->call_mode == CALL_BATTLE_MENU )
      {
        SetEnd( END_SELECT, m_appParam->select_item, WAZANO_NULL, poke_pos, result );
      }
      else
      {
        ItemUseHealing(
          GetListPosItemID(),
          m_appParam->pokemon->GetPoke(poke_pos),
          poke_pos,
          result,
          m_isUpdateCatch );
      }
    }
    else
    {
      if( m_isUpdateCatch == false )
      {
        ChangeModePokemonSelect( m_pokeSelectMode, m_pBagPokemon->GetSelectPos() );
      }
      else
      {
        ChangeModeItemSelect();
      }
      m_pDraw->SetDispActive( true );
      SetSubSeq( SEQ_MAIN );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqItemUseTokuseikapuseru
 * @brief   アップデート：特性カプセル使用
 * @date    2015.04.17
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqItemUseTokuseikapuseru(void)
{
  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  switch( m_subSeq )
  {
  case 0:   // 変更の"はい/いいえ"をセット
    StartMenuWindow( MakeYesNoMenuTable(), true );
    m_yesnoResult = MENU_ID_MAX;
    m_subSeq = 1;
    return;

  case 1:   // 変更の"はい/いいえ"分岐
    if( m_yesnoResult == MENU_ID_YES )
    {
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pBagPokemon->GetSelectPos() );
			pp->FlipTokuseiIndex();
      m_pDraw->PrintTokuseikapuseruMessage( BagDraw::MSG_ID_TOKUSEI_CHANGE, pp, m_newTokusei );
      m_subSeq = 2;
    }
    else if( m_yesnoResult == MENU_ID_NO )
    {
      break;
    }
    return;

  case 2:   // 使用
    ItemUseSub( GetListPosItemID() );
  }

  m_pDraw->SetDispActive( true );
  ChangeModeItemSelect();
  m_subSeq = 0;
  SetSubSeq( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqItemUseFormChange
 * @brief   アップデート：フォルムチェンジ
 * @date    2015.04.17
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqItemUseFormChange(void)
{
  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  // サブシーケンス
  enum
  {
    SUB_SEQ_CREATE = 0,         // エフェクト生成
    SUB_SEQ_CREATE_WAIT,        // エフェクト生成待ち
    SUB_SEQ_CHANGE,             // フォルムチェンジ
    SUB_SEQ_EFF_END_WAIT,       // エフェクト終了待ち
    SUB_SEQ_VOICE_WAIT,         // 鳴き声終了待ち

    SUB_SEQ_CHECK_NEXT_POKE,    // エフェクト生成（２匹目）
    SUB_SEQ_CREATE_WAIT_2,      // エフェクト生成待ち（２匹目）
    SUB_SEQ_CHANGE_2,           // フォルムチェンジ（２匹目）
    SUB_SEQ_EFF_END_WAIT_2,     // エフェクト終了待ち（２匹目）
    SUB_SEQ_VOICE_WAIT_2,       // 鳴き声終了待ち（２匹目）

    SUB_SEQ_CHANGE_WAZA,        // 技変更
    SEQ_SEQ_CALL_WAZAWASURE,    // 技忘れへ

    SUB_SEQ_FINISH,             // 終了
  };


  switch( m_subSeq )
  {
  case SUB_SEQ_CREATE:          // エフェクト生成
    CreateFormChangeEffect( m_formChangeWork.poke[0].pp, m_formChangeWork.poke[0].item, m_formChangeWork.poke[0].pos, 0 );
    if( m_formChangeWork.poke[0].item == ITEM_IDENSINOKUSABI ||
        IsNekurozumaCombineItem(m_formChangeWork.poke[0].item) != false )
    {
      u32 pos = m_pBagPokemon->GetSelectPos();
      CreateFormChangeEffect(
        m_appParam->pokemon->GetPoke( pos ),
        m_formChangeWork.poke[0].item,
        pos, 1 );
    }
    m_pDraw->SetDispActive( true );
    m_subSeq++;
    break;

  case SUB_SEQ_CREATE_WAIT:     // エフェクト生成待ち
    if( m_pItemEffect->IsCreate( 0 ) == false )
    {
      break;
    }
    if( m_formChangeWork.poke[0].item == ITEM_IDENSINOKUSABI ||
        IsNekurozumaCombineItem(m_formChangeWork.poke[0].item) != false )
    {
      if( m_pItemEffect->IsCreate( 1 ) == false )
      {
        break;
      }
    }
    m_pItemEffect->Start();
    m_subSeq++;
    break;

  case SUB_SEQ_CHANGE:          // フォルムチェンジ
    if( m_pItemEffect->IsUpdateTiming( 0 ) != false )
    {
      FormChangeCore( &m_formChangeWork.poke[0] );
      m_subSeq++;
    }
    break;

  case SUB_SEQ_EFF_END_WAIT:    // エフェクト終了待ち
    if( m_pItemEffect->IsEnd( 0 ) != false )
    {
      if( m_formChangeWork.poke[0].item == ITEM_IDENSINOKUSABI ||
          IsNekurozumaCombineItem(m_formChangeWork.poke[0].item) != false )
      {
        if( m_pItemEffect->IsEnd( 1 ) == false )
        {
          break;
        }
      }

      m_pItemEffect->End();
      m_pItemEffect->Delete( 0 );

      if( m_formChangeWork.poke[0].item == ITEM_IDENSINOKUSABI ||
          IsNekurozumaCombineItem(m_formChangeWork.poke[0].item) != false )
      {
        m_pItemEffect->Delete( 1 );

        m_pBagPokemon->ResetPane();
        m_pBagPokemon->InitPlate( m_pItemManager, m_formChangeWork.poke[0].item );
        m_pBagPokemon->ChangePokeSelectMode( true, m_unionPos );
      }
      Sound::PlayVoice( 0, m_formChangeWork.poke[0].pp );
      m_subSeq++;
    }
    break;

  case SUB_SEQ_VOICE_WAIT:      // 鳴き声終了待ち
    if( Sound::IsVoiceFinished( 0 ) != false )
    {
      m_pDraw->SetDispActive( false );
      m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_FORM_CHANGE, m_formChangeWork.poke[0].pp );
      m_subSeq++;
    }
    break;

  case SUB_SEQ_CHECK_NEXT_POKE: // エフェクト生成（２匹目）
    if( m_formChangeWork.poke[1].pp != NULL )
    {
      m_pBagPokemon->ChangePokeSelectMode( false, m_formChangeWork.poke[0].pos );
      m_pBagPokemon->ChangePokeSelectMode( true, m_formChangeWork.poke[1].pos );
      CreateFormChangeEffect( m_formChangeWork.poke[1].pp, m_formChangeWork.poke[1].item, m_formChangeWork.poke[1].pos, 0 );
      m_pDraw->SetDispActive( true );
      m_subSeq++;
    }
    else
    {
//      m_subSeq = SUB_SEQ_FINISH;
      m_subSeq = SUB_SEQ_CHANGE_WAZA;
    }
    break;

  case SUB_SEQ_CREATE_WAIT_2:   // エフェクト生成待ち（２匹目）
    if( m_pItemEffect->IsCreate( 0 ) == false )
    {
      break;
    }
    m_pItemEffect->Start();
    m_subSeq++;
    break;

  case SUB_SEQ_CHANGE_2:        // フォルムチェンジ（２匹目）
    if( m_pItemEffect->IsUpdateTiming( 0 ) != false )
    {
      FormChangeCore( &m_formChangeWork.poke[1] );
      m_subSeq++;
    }
    break;

  case SUB_SEQ_EFF_END_WAIT_2:  // エフェクト終了待ち（２匹目）
    if( m_pItemEffect->IsEnd( 0 ) != false )
    {
      m_pItemEffect->End();
      m_pItemEffect->Delete( 0 );
      Sound::PlayVoice( 0, m_formChangeWork.poke[1].pp );
      m_subSeq++;
    }
    break;

  case SUB_SEQ_VOICE_WAIT_2:    // 鳴き声終了待ち（２匹目）
    if( Sound::IsVoiceFinished( 0 ) != false )
    {
      m_pDraw->SetDispActive( false );
      m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_FORM_CHANGE, m_formChangeWork.poke[0].pp );
      m_subSeq++;
    }
    break;

  case SUB_SEQ_CHANGE_WAZA:     // 技変更　※ネクロズマ用 momiji追加
    if( IsNekurozumaCombineItem(m_formChangeWork.poke[0].item) != false )
    {
      if( AddUnionWaza( m_formChangeWork.poke[0].pp, m_formChangeWork.poke[0].item ) != false )
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_COMP, m_formChangeWork.poke[0].pp, GetUnionWaza(m_formChangeWork.poke[0].item) );
        m_subSeq = SUB_SEQ_FINISH;
      }
      else
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_SELECT, NULL, GetUnionWaza(m_formChangeWork.poke[0].item) );
        m_subSeq = SEQ_SEQ_CALL_WAZAWASURE;
      }
    }
    else if( IsNekurozumaSeparateItem(m_formChangeWork.poke[0].item) != false )
    {
      // @fix MMCat[441]: ねんりきをセットした場合は別メッセージを表示
      SubUnionWazaResult result = SubUnionWaza( m_formChangeWork.poke[0].pp, m_formChangeWork.poke[0].item );
      if( result != SUB_UNION_WAZA_NONE )
      {
        bool is_nenriki = false;
        if( result == SUB_UNION_WAZA_NENRIKI )
        {
          is_nenriki = true;
        }
        m_pDraw->PrintUnionWazaSubMessage( m_formChangeWork.poke[0].pp, GetUnionWaza(m_formChangeWork.poke[0].item), is_nenriki );
      }
      m_subSeq = SUB_SEQ_FINISH;
    }
    else
    {
      m_subSeq = SUB_SEQ_FINISH;
    }
    break;

  case SEQ_SEQ_CALL_WAZAWASURE:   // 技忘れへ
    SetEnd(
      END_CALL_WAZA_WASURE,
      GetListPosItemID(),   // 道具が置き換わっているため
      GetUnionWaza( m_formChangeWork.poke[0].item ),
      m_unionPos );
    SetFadeOut();
    m_subSeq = 0;
    return;

  case SUB_SEQ_FINISH:          // 終了
    if( m_formChangeWork.poke[0].item == ITEM_IDENSINOKUSABI ||
        m_formChangeWork.poke[0].item == ITEM_IDENSINOKUSABI_1 ||
        m_formChangeWork.poke[0].item == ITEM_SORUPURASU ||
        m_formChangeWork.poke[0].item == ITEM_SORUPURASU_1 ||
        m_formChangeWork.poke[0].item == ITEM_RUNAPURASU ||
        m_formChangeWork.poke[0].item == ITEM_RUNAPURASU_1 )
    {
      m_pBagPokemon->ChangePlateParty( false );
    }

    // ミツスイ用のアイテムの場合は消費
    if( GetMitsusuiForm(m_formChangeWork.poke[0].item) != FORMNO_MITUSUI_MAX )
    {
      SubItem( m_formChangeWork.poke[0].item, 1 );
    }

    {
      const Savedata::MyItem::ITEM_ST * st = GetListPosItem();
      u32 pos_id = ITEM_DUMMY_DATA;
      if( st != NULL )
      {
        pos_id = st->id;
      }
      m_pBagPokemon->InitPlate( m_pItemManager, pos_id, m_pBagPokemon->IsPutParty() );
    }

    // ボックスの場合は終了
    if( m_appParam->call_mode == CALL_BOX )
    {
      m_subSeq = 0;
      SetEnd( END_CANCEL );   // モードはなんでもいいはず。
      SetFadeOut();
    }
    // キャッチムーブ => リストへ
    else if( m_formChangeID == FORM_CHG_ID_ITEM_CATCH_LIST )
    {
      m_pDraw->SetDispActive( true );
      m_subSeq = 3;
      // @note NMCat[3917]関連メモ：キャッチムーブからの処理なので位置を再設定する必要はない
      SetSubSeq( SEQ_POKE_ITEM_MOVE );
    }
    // キャッチムーブ => 別のポケモンへ
    else if( m_formChangeID == FORM_CHG_ID_ITEM_CATCH_POKE )
    {
      m_pDraw->SetDispActive( true );
      m_subSeq = 3;
      // @note NMCat[3917]関連メモ：キャッチムーブからの処理なので位置を再設定する必要はない
      SetSubSeq( SEQ_POKE_ITEM_MOVE );
    }
    // それ以外
    else
    {
      m_pDraw->SetDispActive( true );
      ChangeModeItemSelect();
      m_subSeq = 0;
      SetSubSeq( SEQ_MAIN );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqItemUseBeads
 * @brief   アップデート：ビーズ使用
 * @date    2015.11.05
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqItemUseBeads(void)
{
  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  switch( m_subSeq )
  {
  case 0:
    // はい・いいえセット
    StartMenuWindow( MakeYesNoMenuTable(), true );
    m_yesnoResult = MENU_ID_MAX;
    m_subSeq++;
    break;

  case 1:
    if( m_yesnoResult == MENU_ID_YES )
    {
      u32 item  = GetListPosItemID();
      u32 piece = m_pBagPokemon->GetPiece( m_pItemManager, item );
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pBagPokemon->GetSelectPos() );
      m_pDraw->PrintBeadsMessage( BagDraw::MSG_ID_BEADS_START, pp, item, piece );
      m_subSeq = 2;
    }
    else if( m_yesnoResult == MENU_ID_NO )
    {
      m_subSeq = 8;
    }
    break;

  case 2:
    {
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pBagPokemon->GetSelectPos() );
      u32 item = pp->GetItem();
      if( item != ITEM_DUMMY_DATA )
      {
        m_pDraw->PrintMessage( BagDraw::MSG_ID_CHANGE_CHECK, item, ITEM_DUMMY_DATA, pp );
        m_subSeq = 3;
      }
      else
      {
        u32 piece = m_pBagPokemon->GetPiece( m_pItemManager, GetListPosItemID() );
        CreateItemEffect(
          m_pBagPokemon->GetSelectPos(),
          0,
          m_pItemEffect->GetZPowerItemEffectType(piece) );
        m_subSeq = 5;
      }
    }
    break;

  case 3:
    // はい・いいえセット
    StartMenuWindow( MakeYesNoMenuTable(), true );
    m_yesnoResult = MENU_ID_MAX;
    m_subSeq++;
    break;

  case 4:
    if( m_yesnoResult == MENU_ID_YES )
    {
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( m_pBagPokemon->GetSelectPos() );
      u32 item = pp->GetItem();
      if( item::ITEM_CheckPiece(item) != false || m_myItem->AddCheck(item,1) != false )
      {
        u32 piece = m_pBagPokemon->GetPiece( m_pItemManager, GetListPosItemID() );
        CreateItemEffect(
          m_pBagPokemon->GetSelectPos(),
          0,
          m_pItemEffect->GetZPowerItemEffectType(piece) );
        m_subSeq = 5;
      }
      else
      {
        m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ERROR_MAX );
        m_subSeq = 8;
      }
    }
    else if( m_yesnoResult == MENU_ID_NO )
    {
      m_subSeq = 8;
    }
    break;

  case 5:
    if( m_pItemEffect->IsCreate( 0 ) == false )
    {
      break;
    }
    m_pDraw->SetDispActive( true );
    m_pItemEffect->Start();
    m_subSeq = 6;
    break;

  case 6:
    if( m_pItemEffect->IsEnd( 0 ) != false )
    {
      m_pItemEffect->End();
      m_pItemEffect->Delete( 0 );

      m_pDraw->SetDispActive( false );

      u32 poke_pos = m_pBagPokemon->GetSelectPos();
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( poke_pos );
      u16 set_item = GetListPosItemID();
      u16 get_item = pp->GetItem();
      u32 piece = m_pBagPokemon->GetPiece( m_pItemManager, set_item );
      pp->SetItem( piece );
      m_pBagPokemon->PutItemIcon( poke_pos, true );
      m_pBagPokemon->InitPlate(
        poke_pos,
        pp,
        m_appParam->pokemon->IsSickMoudoku( poke_pos ),
        m_pItemManager,
        set_item );
      m_pBagPokemon->ChangePokeSelectMode( true, poke_pos );
      if( get_item == ITEM_DUMMY_DATA )
      {
        m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ITEM, piece );
      }
      else
      {
        m_myItem->Add( get_item, 1 );
        m_updatePlateLock = true;   // プレートが非選択になってしまうのでロック
        ResetPocketItemTable( get_item ); // リスト更新
        ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );
        SetVisibleMenuCursor( false );  // カーソル非表示
        m_pDraw->PrintMessage( BagDraw::MSG_ID_CHANGE_ITEM, get_item, piece );
      }
      m_subSeq = 7;
    }
    break;

  case 7:
    {
      u32 poke_pos = m_pBagPokemon->GetSelectPos();
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( poke_pos );
      u32 item_id = pp->GetItem();
      // フォルムチェンジ
      if( CheckSetItemFormChange( poke_pos, pp, item_id ) != false )
      {
        m_pBagPokemon->ChangePokeSelectMode( true, poke_pos );
        SetFormChangeWork( poke_pos, item_id );
        m_subSeq = 0;
        m_formChangeID = FORM_CHG_ID_ITEM_SET;
        SetSubSeq( SEQ_USE_FORM_CHANGE );
      }
      else
      {
        m_subSeq = 8;
      }
    }
    break;

  case 8:
    m_pDraw->SetDispActive( true );
    m_subSeq = 0;

    if( m_appParam->call_mode == CALL_BOX )
    {
      // @note ボックスの場合、道具の入れ替えはないので持っているなら持たせたことになる
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pBagPokemon->GetSelectPos() );
      if( pp->GetItem() != ITEM_DUMMY_DATA )
      {
        SetEnd( END_CANCEL );   // 終了モードはなんでもいいはず。
        SetFadeOut();
      }
      else
      {
        ChangeModeItemSelect();
        SetSubSeq( SEQ_MAIN );
      }
      break;
    }

    if( m_isUpdateCatch == false && m_pBagPokemon->IsPutParty() != false )
    {
      m_pDraw->PutMessage( BagDraw::MSG_ID_USE_SELECT );
      ChangeModePokemonSelect( POKESEL_MODE_MENU_USE, m_pBagPokemon->GetSelectPos() );
    }
    else
    {
      ChangeModeItemSelect();
    }
    SetSubSeq( SEQ_MAIN );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqItemUseWazaMachine
 * @brief   アップデート：技マシン使用
 * @date    2015.04.20
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqItemUseWazaMachine(void)
{
  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  switch( m_subSeq )
  {
  case 0:
    {
      WazaNo waza = static_cast<WazaNo>( item::ITEM_GetWazaNo(GetListPosItemID()) );
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pBagPokemon->GetSelectPos() );
      if( pp->AddWazaIfEmptyExist(waza) == pml::pokepara::WAZA_LEARNING_SUCCEEDED )
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_COMP, pp, waza );
        Savedata::IncRecord( Savedata::Record::RECID_LERAN_WAZA );      // レコード
        PokeDiary::Set( PDID_COUNT_WAZAMASHIN, pp->GetMonsNo(), waza ); // ダイアリー
        m_subSeq = 1;
      }
      else
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_SELECT, NULL, waza );
        m_subSeq = 2;
      }
    }
    return;

  case 1:
    {
      u32 sel_pos = m_pBagPokemon->GetSelectPos();
      m_pBagPokemon->InitPlate(
        sel_pos,
        m_appParam->pokemon->GetPokeConst( sel_pos ),
        m_appParam->pokemon->IsSickMoudoku( sel_pos ),
        m_pItemManager,
        GetListPosItemID(),
        m_pBagPokemon->IsPutParty() );
    }
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
    return;

  case 2:
    {
      u32 item = GetListPosItemID();
      SetEnd( END_CALL_WAZA_WASURE, item, item::ITEM_GetWazaNo(item), m_pBagPokemon->GetSelectPos() );
      SetFadeOut();
    }
    m_subSeq = 0;
    return;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqItemUseZCubeWaza
 * @brief   アップデート：ジガルデキューブ使用
 * @date    2015.11.30
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqItemUseZCubeWaza(void)
{
  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  switch( m_subSeq )
  {
  case 0:
    {
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pBagPokemon->GetSelectPos() );
      // 覚えている
      if( pp->HaveWaza(m_zCubeSelectWazaNo) != false )
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_ERR_SAME, pp, m_zCubeSelectWazaNo );
        m_subSeq = 1;
      }
      else if( pp->AddWazaIfEmptyExist(m_zCubeSelectWazaNo) == pml::pokepara::WAZA_LEARNING_SUCCEEDED )
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_COMP, pp, m_zCubeSelectWazaNo );
        m_subSeq = 1;
      }
      else
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_SELECT, pp, m_zCubeSelectWazaNo );
        m_subSeq = 2;
      }
    }
    return;

  case 1:
/*
    {
      u32 sel_pos = m_pBagPokemon->GetSelectPos();
      m_pBagPokemon->InitPlate(
        sel_pos,
        m_appParam->pokemon->GetPokeConst( sel_pos ),
        m_appParam->pokemon->IsSickMoudoku( sel_pos ),
        m_pItemManager,
        GetListPosItemID(),
        m_pBagPokemon->IsPutParty() );
    }
*/
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
    return;

  case 2:   // 技忘れへ
    SetEnd( END_CALL_WAZA_WASURE, GetListPosItemID(), m_zCubeSelectWazaNo, m_pBagPokemon->GetSelectPos() );
    SetFadeOut();
    m_subSeq = 0;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqWazaResult
 * @brief   アップデート：技忘れ画面から復帰
 * @date    2015.04.20
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqWazaResult(void)
{
  // レベルアップ処理に戻るときのみ
  if( m_subSeq == 3 )
  {
    m_pDevTurnWatcher->Update();
  }

  // メッセージ終了待ち
  if( m_pDraw->UpdatePrintMessage() != false )
  {
    return;
  }

  switch( m_subSeq )
  {
  case 0:
    m_pDraw->SetDispActive( false );
    {
      WazaNo add_waza = static_cast<WazaNo>( m_appParam->select_skill );
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_appParam->select_pokemon_pos );
      if( m_appParam->select_skill_pos >= pml::MAX_WAZA_NUM )
      {
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_COMP_CANCEL, pp, add_waza );
        if( item::ITEM_CheckWazaMachine(m_appParam->select_item) != false ||
            CheckItemUseZCube(m_appParam->select_item) != false ||
            IsNekurozumaSeparateItem(m_appParam->select_item) != false )
        {
          m_subSeq = 2;
        }
        else
        {
          // @note この関数内でさかさチェックを行わないとうまくいかないので、ここで初期化
          m_pDevTurnWatcher->Initialize( System::DeviceTurnWatcher::COUNT_RATE_30F, System::DeviceTurnWatcher::COUNT_MODE_SHORT );
          m_subSeq = 3;
        }
      }
      else
      {
        WazaNo sub_waza = pp->GetWazaNo( m_appParam->select_skill_pos );
        m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_COMP_ERASE, pp, sub_waza, add_waza );
        pp->SetWaza( m_appParam->select_skill_pos, add_waza );
        if( item::ITEM_CheckWazaMachine(m_appParam->select_item) != false )
        {
          Savedata::IncRecord( Savedata::Record::RECID_LERAN_WAZA );          // レコード
          PokeDiary::Set( PDID_COUNT_WAZAMASHIN, pp->GetMonsNo(), add_waza ); // ダイアリー
          m_subSeq = 1;
        }
        else if( CheckItemUseZCube(m_appParam->select_item) != false ||
                 IsNekurozumaSeparateItem(m_appParam->select_item) != false )
        {
          m_subSeq = 1;
        }
        else
        {
          // @note この関数内でさかさチェックを行わないとうまくいかないので、ここで初期化
          m_pDevTurnWatcher->Initialize( System::DeviceTurnWatcher::COUNT_RATE_30F, System::DeviceTurnWatcher::COUNT_MODE_SHORT );
          m_subSeq = 3;
        }
      }
    }
    return;

  case 1:
    m_pBagPokemon->InitPlate(
      m_appParam->select_pokemon_pos,
      m_appParam->pokemon->GetPokeConst( m_appParam->select_pokemon_pos ),
      m_appParam->pokemon->IsSickMoudoku( m_appParam->select_pokemon_pos ),
      m_pItemManager,
      m_appParam->select_item,
      m_pBagPokemon->IsPutParty() );
    m_subSeq = 2;
    /* FALL THROUGH */
  case 2:
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
    return;

  case 3:
    m_subSeq = 2;   // 技覚えへ
    SetSubSeq( SEQ_LEVEL_UP );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqPokeItemMove
 * @brief   アップデート：ポケモンの持ち物移動
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqPokeItemMove(void)
{
  PokeItemMoveSub();

  switch( m_subSeq )
  {
  case 0:
    m_pDraw->StartItemCatch();
    m_pBagPokemon->PutItemIcon( m_pokeItemCatchPos, false );  // @fix NMCat[3917]: キャッチムーブ開始時の位置
    m_pDraw->SetInputEnabled( false );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_touchPanel->IsTouch() == false )
    {
      if( m_isCatchItemPut == false )
      {
        m_isCatchItemPut = true;
      }
      m_pDraw->SetPaneListEnable( false );  // @fix niji GFNMCat[803]: リリースしたらリストの入力を停止
    }
    if( m_isCatchItemPut != false )
    {
      if( m_isExScroll == false )
      {
        PutPokemonItem();
        m_isCatchItemPut = false;
      }
    }
    m_pDraw->MoveItemIcon();
    break;

  case 2:
    if( m_isExScroll != false )
    {
      break;
    }
    if( m_pDraw->ReleaseItemIcon() == false )
    {
      switch( m_pDraw->GetItemPutPosID() )
      {
      case BagDraw::PUT_ID_CANCEL:
        {
          m_pBagPokemon->PutItemIcon( m_pokeItemCatchPos, true );  // @fix NMCat[3917]: キャッチムーブ開始時の位置
          m_subSeq = 3;
        }
        break;
      case BagDraw::PUT_ID_PLATE:
        {
          u16 get_poke = m_pokeItemCatchPos;  // @fix NMCat[3917]: キャッチムーブ開始時の位置
          u16 put_poke = m_pDraw->GetItemPutIndex();
          pml::pokepara::PokemonParam * put_pp = m_appParam->pokemon->GetPoke( put_poke );
          if( get_poke == put_poke || put_pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
          {
            m_pBagPokemon->PutItemIcon( get_poke, true );
            m_subSeq = 3;
          }
          else
          {
            pml::pokepara::PokemonParam * get_pp = m_appParam->pokemon->GetPoke( get_poke );
            u16 get_item = get_pp->GetItem();
            u16 put_item = put_pp->GetItem();
            get_pp->SetItem( put_item );
            put_pp->SetItem( get_item );
            m_pBagPokemon->PutItemIcon( put_poke, true );
            m_pBagPokemon->InitPlate(
              put_poke,
              put_pp,
              m_appParam->pokemon->IsSickMoudoku( put_poke ),
              m_pItemManager,
              GetListPosItemID() );
            if( put_item == ITEM_DUMMY_DATA )
            {
              m_pBagPokemon->InitPlate(
                get_poke,
                get_pp,
                m_appParam->pokemon->IsSickMoudoku( get_poke ),
                m_pItemManager,
                GetListPosItemID() );
              m_subSeq = 6;
            }
            else
            {
              ChangeItemIconRequest( put_item, ITEMICON_ID_CATCH );
              m_pDraw->StartItemCatch();
              m_subSeq = 4;
            }
          }
        }
        break;
      case BagDraw::PUT_ID_LIST:
        {
          u32 get_pos = m_pokeItemCatchPos;  // @fix NMCat[3917]: キャッチムーブ開始時の位置
          pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( get_pos ); 
          u32 item_id = pp->GetItem();
          // バッグに戻せない
          if( m_myItem->Add(item_id,1) == false )
          {
            m_pBagPokemon->PutItemIcon( get_pos, true );
            m_pDraw->SetDispActive( false );
            m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ERROR_MAX );
            m_subSeq = 7;
            break;
          }
          else
          {
            u32 pocket_id = m_pItemManager->GetParam( item_id, item::ITEM_DATA::PRM_ID_F_POCKET );
            u32 pocket_num;
            // フリースペースに登録されている
            if( m_myItem->IsRegisterFreeSpace(item_id) != false )
            {
              pocket_num = PlayData::FREE_SPACE_POCKET;
            }
            else
            {
              pocket_num = GetFieldPocketNum( pocket_id );
            }
            // 表示中のポケットへ入れる
            if( m_pocket == static_cast<s32>(pocket_num) )
            {
              u32 list_pos;
              f32 list_scroll;
              m_pPaneListView->GetPaneList()->GetCursorData( &list_pos, &list_scroll );
              s32 in_pos = m_pDraw->GetItemPutIndex() + static_cast<int>(list_scroll);    // 挿入位置
              // フリースペース
              if( m_pocket == PlayData::FREE_SPACE_POCKET )
              {
                if( m_myItem->Check(item_id,2) == false )
                {
                  ResetPocketItemTable( item_id );
                }
                Savedata::MyItem::ITEM_ST * item = m_myItem->GetItem( item_id );
                if( (static_cast<s32>(item->free_space)-1) < in_pos )
                {
                  in_pos -= 1;
                }
                if( in_pos >= static_cast<s32>(m_itemTableMax[m_pocket]) )
                {
                  in_pos = m_itemTableMax[m_pocket] - 1;
                }
                if( (static_cast<s32>(item->free_space)-1) != in_pos )
                {
                  m_myItem->InsertFreeSpace( m_itemTable[PlayData::FREE_SPACE_POCKET], item, &m_itemTableMax[PlayData::FREE_SPACE_POCKET], in_pos );
                }
              }
              // 通常
              else
              {
                ResetPocketItemTable( item_id );    // リスト更新
                u32 now = m_myItem->GetItemPocketPos( pocket_id, item_id, false );
                if( static_cast<s32>(now) < in_pos )
                {
                  in_pos -= 1;
                }
                if( in_pos >= static_cast<s32>(m_itemTableMax[m_pocket]) )
                {
                  in_pos = m_itemTableMax[m_pocket] - 1;
                }
                if( static_cast<s32>(now) != in_pos )
                {
                  m_myItem->Insert( pocket_id, now, in_pos );
                  ResetPocketItemTable( item_id );  // リスト再更新
                }
              }
              ChangePocket( m_pocket, item_id, in_pos-static_cast<u32>(list_scroll) );             // 表示更新
              m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
            }
            else
            {
              if( m_myItem->Check(item_id,2) == false )
              {
                ResetPocketItemTable( item_id );  // リスト更新
              }
            }
            pp->SetItem( ITEM_DUMMY_DATA );
            // フォルムチェンジ
            if( CheckSetItemFormChange( get_pos, pp, ITEM_DUMMY_DATA ) != false )
            {
              m_pBagPokemon->ChangePokeSelectMode( true, get_pos );
              SetFormChangeWork( get_pos, ITEM_DUMMY_DATA );
              m_subSeq = 0;
              m_formChangeID = FORM_CHG_ID_ITEM_CATCH_LIST;
              SetSubSeq( SEQ_USE_FORM_CHANGE );
              return;
            }
            else
            {
              m_subSeq = 3;
            }
          }
        }
        break;
      case BagDraw::PUT_ID_POCKET:
        {
          u32 get_pos = m_pokeItemCatchPos;  // @fix NMCat[3917]: キャッチムーブ開始時の位置
          pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( get_pos ); 
          u32 item_id = pp->GetItem();
          // バッグに戻せない
          if( m_myItem->Add(item_id,1) == false )
          {
            m_pBagPokemon->PutItemIcon( get_pos, true );
            m_pDraw->SetDispActive( false );
            m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ERROR_MAX );
            m_subSeq = 7;
            break;
          }
          // フリースペースに登録されている
          else if( m_myItem->IsRegisterFreeSpace(item_id) != false )
          {
            if( m_myItem->Check(item_id,2) == false )
            {
              ResetPocketItemTable( item_id );   // リスト更新
              if( m_pocket == PlayData::FREE_SPACE_POCKET )
              {
                ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );             // 表示更新
                m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
              }
            }
          }
          // 通常のポケット
          else
          {
            if( m_myItem->Check(item_id,2) == false )
            {
              ResetPocketItemTable( item_id );   // リスト更新
              if( static_cast<int>(GetFieldPocketID(m_pocket)) == m_pItemManager->GetParam(item_id,item::ITEM_DATA::PRM_ID_F_POCKET) )
              {
                ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );             // 表示更新
                m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
              }
            }
          }
          pp->SetItem( ITEM_DUMMY_DATA );
          // フォルムチェンジ
          if( CheckSetItemFormChange( get_pos, pp, ITEM_DUMMY_DATA ) != false )
          {
            m_pBagPokemon->ChangePokeSelectMode( true, get_pos );
            SetFormChangeWork( get_pos, ITEM_DUMMY_DATA );
            m_subSeq = 0;
            m_formChangeID = FORM_CHG_ID_ITEM_CATCH_LIST;
            SetSubSeq( SEQ_USE_FORM_CHANGE );
            return;
          }
          else
          {
            m_subSeq = 3;
          }
        }
        break;

      case BagDraw::PUT_ID_DELETE:
        {
          u32 poke_pos = m_pokeItemCatchPos;  // @fix NMCat[3917]: キャッチムーブ開始時の位置
          pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( poke_pos );
          pp->SetItem( ITEM_DUMMY_DATA );
          CheckSetItemFormChange( poke_pos, pp, ITEM_DUMMY_DATA );  // ピースを捨てた場合なので、エフェクトは発生しない
          m_pBagPokemon->InitPlate(
            poke_pos,
            pp,
            m_appParam->pokemon->IsSickMoudoku( poke_pos ),
            m_pItemManager,
            GetListPosItemID() );
        }
        m_subSeq = 3;
        break;
      }
    }
    break;

  case 3:
    {
      const Savedata::MyItem::ITEM_ST * item = GetListPosItem();
      if( item != NULL )
      {
        ChangeItemIconRequest( item->id, ITEMICON_ID_INFO );
        m_pBagPokemon->InitPlate( m_pItemManager, item->id, m_pBagPokemon->IsPutParty() );
      }
      else
      {
        m_pBagPokemon->InitPlate( m_pItemManager, ITEM_DUMMY_DATA, m_pBagPokemon->IsPutParty() );
      }
      if( m_pInfoField != NULL )
      {
        m_pInfoField->UpdateInfo( item );
        m_pInfoField->UpdatePocketName( GetFieldPocketID(m_pocket) );
        m_pInfoField->ChangePocketBg( m_pocket );
      }
    }
    m_pBagPokemon->ChangePokeSelectMode( false, m_pokeItemCatchPos );  // @fix NMCat[3917]: キャッチムーブ開始時の位置
    m_pDraw->SetPocketButtonActive( 0xffffffff, true );
    ChangeModeItemSelect();
    m_listUpdateLock = false;
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
    break;

  case 4:
    if( m_pDraw->IsAnimeEnd( LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_ITEM_CATCH ) != false )
    {
      gfl2::math::VEC3 pos = m_pBagPokemon->GetItemIconPos( m_pokeItemCatchPos );  // @fix NMCat[3917]: キャッチムーブ開始時の位置
      m_pDraw->EndItemCatch( &pos );
      m_subSeq = 5;
    }
    break;

  case 5:
    if( m_pDraw->ReleaseItemIcon() == false )
    {
      u32 poke_pos = m_pokeItemCatchPos;  // @fix NMCat[3917]: キャッチムーブ開始時の位置
      m_pBagPokemon->PutItemIcon( poke_pos, true );
      m_pBagPokemon->InitPlate(
        poke_pos,
        m_appParam->pokemon->GetPoke( poke_pos ),
        m_appParam->pokemon->IsSickMoudoku( poke_pos ),
        m_pItemManager,
        GetListPosItemID() );
      m_subSeq = 6;
    }
    break;

  case 6:
    {
      u16 get_poke = m_pokeItemCatchPos;  // @fix NMCat[3917]: キャッチムーブ開始時の位置
      u16 put_poke = m_pDraw->GetItemPutIndex();
      pml::pokepara::PokemonParam * get_pp = m_appParam->pokemon->GetPoke( get_poke );
      pml::pokepara::PokemonParam * put_pp = m_appParam->pokemon->GetPoke( put_poke );
      u16 get_item = get_pp->GetItem();
      u16 put_item = put_pp->GetItem();

      bool is_get_change = CheckSetItemFormChange( get_poke, get_pp, get_item );
      bool is_put_change = CheckSetItemFormChange( put_poke, put_pp, put_item );

      // フォルムチェンジ
      if( is_get_change != false )
      {
        m_pBagPokemon->ChangePokeSelectMode( true, get_poke );
        if( is_put_change != false )
        {
          SetFormChangeWork( get_poke, get_item, put_poke, put_item );
        }
        else
        {
          SetFormChangeWork( get_poke, get_item );
        }
        m_subSeq = 0;
        m_formChangeID = FORM_CHG_ID_ITEM_CATCH_POKE;
        SetSubSeq( SEQ_USE_FORM_CHANGE );
        return;
      }
      else if( is_put_change != false )
      {
        m_pBagPokemon->ChangePokeSelectMode( true, put_poke );
        SetFormChangeWork( put_poke, put_item );
        m_subSeq = 0;
        m_formChangeID = FORM_CHG_ID_ITEM_CATCH_POKE;
        SetSubSeq( SEQ_USE_FORM_CHANGE );
        return;
      }
      else
      {
        m_subSeq = 3;
      }
    }
    break;

  case 7:
    if( m_pDraw->UpdatePrintMessage() == false )
    {
      m_pDraw->SetDispActive( true );
      m_subSeq = 3;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqListItemMove
 * @brief   アップデート：リストの道具移動
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqListItemMove(void)
{
  PokeItemMoveSub();

  switch( m_subSeq )
  {
  case 0:
    m_pDraw->StartItemCatch();
    m_pDraw->SetInputEnabled( false );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_touchPanel->IsTouch() == false )
    {
      if( m_isCatchItemPut == false )
      {
        m_isCatchItemPut = true;
      }
      m_pDraw->SetPaneListEnable( false );  // @fix niji GFNMCat[803]: リリースしたらリストの入力を停止
    }
    if( m_isCatchItemPut != false )
    {
      if( m_isExScroll == false )
      {
        m_isCatchItemPut = false;
        if( PutListItem() != false )
        {
          return;
        }
      }
    }
    m_pDraw->MoveItemIcon();
    break;

  case 2:
    if( m_isExScroll != false )
    {
      break;
    }
    if( m_pDraw->ReleaseItemIcon() == false )
    {
      switch( m_pDraw->GetItemPutPosID() )
      {
      case BagDraw::PUT_ID_CANCEL:
        m_subSeq = 3;
        break;
      case BagDraw::PUT_ID_PLATE:
        {
          pml::pokepara::PokemonParam * put_pp = m_appParam->pokemon->GetPoke( m_pDraw->GetItemPutIndex() );
          // タマゴ
          if( put_pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
          {
            m_subSeq = 3;
          }
          //「やめる」のみ
          else if( m_menuTable[0] == MENU_ID_CANCEL )
          {
            m_subSeq = 3;
          }
          //「つかう」
          else if( m_menuTable[0] == MENU_ID_CATCH_USE )
          {
            if( m_menuTable[1] == MENU_ID_CATCH_SET )
            {
              GFL_ASSERT( 0 );
              m_subSeq = 3;
            }
            else
            {
              m_pDraw->ResetCatchItemChangeModeFlag();  // フラグだけ下げる
              m_pDraw->SetPocketButtonActive( 0xffffffff, true );
              m_listUpdateLock = false;
              m_subSeq = 0;

              Savedata::MyItem::ITEM_ST * item = GetCatchListItem();

              // 全員瀕死回復
              AllDeathRcvID rcv_id = CheckAllDeathRecoverItemUse( item->id );
              if( rcv_id == ALL_DEATH_RECOVER_USE_TRUE )
              {
                m_pDraw->SetDispActive( true );
                SetSubSeq( SEQ_ALL_DEATH_RECOVER );
              }
              else if( rcv_id == ALL_DEATH_RECOVER_USE_FALSE )
              {
                m_pDraw->SetDispActive( false );
                m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
                SetSubSeq( SEQ_MSG_WAIT );
              }
              // 道具使用
              else
              {
                PokeItemUse( m_pDraw->GetItemPutIndex(), item, true );
              }
              return;
            }
          }
          //「もたせる」
          else
          {
            // 持たせた
            Savedata::MyItem::ITEM_ST * item = GetCatchListItem();

            m_pDraw->SetDispActive( false );
            ChangeModeMessage();
            m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ITEM, item->id );
            m_pBagPokemon->PutItemIcon( m_pDraw->GetItemPutIndex(), true );
            m_subSeq = 4;
          }
        }
        break;
      case BagDraw::PUT_ID_LIST:
        {
          Savedata::MyItem::ITEM_ST * item = GetCatchListItem();
          u32 item_id = item->id;
          u32 list_pos;
          f32 list_scroll;
          m_pPaneListView->GetPaneList()->GetCursorData( &list_pos, &list_scroll );
          s32 in_pos = m_pDraw->GetItemPutIndex() + static_cast<int>(list_scroll);    // 挿入位置
          // フリースペース
          if( m_pocket == PlayData::FREE_SPACE_POCKET )
          {
            if( m_myItem->IsRegisterFreeSpace(item) == false )
            {
              m_myItem->AddFreeSpace( m_itemTable[PlayData::FREE_SPACE_POCKET], item, &m_itemTableMax[PlayData::FREE_SPACE_POCKET] );
              ResetPocketItemTable( item_id );
              m_pDraw->PutPocketNewIcon( PlayData::FREE_SPACE_POCKET, m_pocketNewMax[PlayData::FREE_SPACE_POCKET] );
            }
            if( (static_cast<s32>(item->free_space)-1) < in_pos )
            {
              in_pos -= 1;
            }
            if( in_pos >= static_cast<s32>(m_itemTableMax[m_pocket]) )
            {
              in_pos = m_itemTableMax[m_pocket] - 1;
            }
            if( (static_cast<s32>(item->free_space)-1) != in_pos )
            {
              m_myItem->InsertFreeSpace( m_itemTable[PlayData::FREE_SPACE_POCKET], item, &m_itemTableMax[PlayData::FREE_SPACE_POCKET], in_pos );
            }
          }
          // 通常
          else
          {
            if( m_myItem->IsRegisterFreeSpace(item) != false )
            {
              m_myItem->SubFreeSpace( item );
            }
            u32 pocket_id = GetFieldPocketID(m_pocket);
            u32 now = m_myItem->GetItemPocketPos( pocket_id, item_id, false );
            if( static_cast<s32>(now) < in_pos )
            {
              in_pos -= 1;
            }
            if( in_pos >= static_cast<s32>(m_itemTableMax[m_pocket]) )
            {
              in_pos = m_itemTableMax[m_pocket] - 1;
            }
            if( static_cast<s32>(now) != in_pos )
            {
              m_myItem->Insert( pocket_id, now, in_pos );
            }
            ResetPocketItemTable( item_id );
          }
          m_pDraw->SetCatchItemChangeMode( false );
          ChangePocket( m_pocket, item_id, in_pos-static_cast<u32>(list_scroll) );    // 表示更新
          m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
          m_subSeq = 3;
        }
        break;
      case BagDraw::PUT_ID_POCKET:
        {
          u32 put = m_pDraw->GetItemPutIndex();
          if( put != m_listCatchPocket && m_pDraw->IsPocketButtonActive( put ) != false )
          {
            Savedata::MyItem::ITEM_ST * item = GetCatchListItem();
            u32 item_id = item->id;
            if( put == PlayData::FREE_SPACE_POCKET )
            {
              m_myItem->AddFreeSpace( m_itemTable[PlayData::FREE_SPACE_POCKET], item, &m_itemTableMax[PlayData::FREE_SPACE_POCKET] );
            }
            else
            {
              m_myItem->SubFreeSpace( item );
            }
            ResetPocketItemTable( item_id );
            m_pDraw->PutPocketNewIcon( PlayData::FREE_SPACE_POCKET, m_pocketNewMax[PlayData::FREE_SPACE_POCKET] );
            m_pDraw->SetCatchItemChangeMode( false );
            ChangePocket( put, item_id, BagDraw::RESET_LIST_POS_NORMAL );         // 表示更新
            m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
          }
          m_subSeq = 3;
        }
        break;
      }
    }
    break;

  case 3:
//    m_pBagPokemon->ChangePokeSelectMode( false, m_pBagPokemon->GetSelectPos() );
    m_pDraw->SetCatchItemChangeMode( false );
    m_pDraw->SetPocketButtonActive( 0xffffffff, true );
    ChangeModeItemSelect();
    m_listUpdateLock = false;
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
    break;

  case 4:
    // メッセージ終了待ち
    if( m_pDraw->UpdatePrintMessage() == false )
    {
      Savedata::MyItem::ITEM_ST * item = GetCatchListItem();
      SetPokemonItem( item->id );

      u32 put_pos = m_pDraw->GetItemPutIndex();
      pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( put_pos );
      u32 item_id = pp->GetItem();
      if( CheckSetItemFormChange( put_pos, pp, item_id ) != false )
      {
        m_pDraw->SetCatchItemChangeMode( false );
        m_pDraw->SetPocketButtonActive( 0xffffffff, true );
        m_listUpdateLock = false;

        m_pBagPokemon->ChangePokeSelectMode( true, put_pos );
        SetFormChangeWork( put_pos, item_id );
        m_subSeq = 0;
        m_formChangeID = FORM_CHG_ID_ITEM_SET;
        SetSubSeq( SEQ_USE_FORM_CHANGE );
      }
      else
      {
        // ボックスの場合は終了へ
        if( m_appParam->call_mode == CALL_BOX )
        {
          m_subSeq = 0;
          SetEnd( END_CANCEL );   // モードはなんでもいいはず
          SetFadeOut();
        }
        else
        {
          m_pDraw->SetDispActive( true );
          m_subSeq = 3;
        }
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqBattleRecoverMode
 * @brief   アップデート：バトル用復帰処理
 * @date    2015.04.24
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqBattleRecoverMode(void)
{
  switch( m_battleRecoverMode )
  {
  case RCV_USE:     // つかう
    m_pDraw->ChangeListPos( m_appParam->select_item, GetListPosItemID() );
    // ポケモン選択済みモード
    if( m_pBagPokemon->IsPutParty() == false )
    {
      item::ITEM_DATA item_data;
      m_pItemManager->GetData( m_appParam->select_item, item_data );
      // 技選択へ
      if( ITEM_UTIL_IsNeedSelectSkill( &item_data ) != false )
      {
        m_pWazaSelect->Start( m_appParam->pokemon->GetPokeConst(m_appParam->pokemon_index), msg_bag_04_07 );
        ChangeModeMenu();
        m_isUpdateCatch = true;   // 技選択後に道具選択へ戻るためにキャッチ扱いにする
        SetSubSeq( SEQ_WAZA_SELECT );
      }
      else
      {
        SetEnd( END_SELECT, m_appParam->select_item, WAZANO_NULL, m_appParam->pokemon_index, 0 );
      }
    }
    // 通常モード
    else
    {
      ChangeModePokemonSelect( POKESEL_MODE_BATTLE_USE );
      m_pDraw->PutMessage( BagDraw::MSG_ID_USE_SELECT );
      SetSubSeq( SEQ_MAIN );
    }
    break;
  case RCV_CANCEL:  // キャンセル
    m_pDraw->ChangeListPos( m_appParam->select_item, GetListPosItemID() );
    ChangeModeItemSelect();
    SetSubSeq( SEQ_MAIN );
    break;
  case RCV_END:     // 終了へ
    SetEnd( END_SELECT, m_appParam->select_item, m_appParam->select_skill, m_appParam->select_pokemon_pos, m_appParam->select_skill_pos );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqBattleItemMove
 * @brief   アップデート：バトル用リスト道具移動
 * @date    2015.05.13
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqBattleItemMove(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pDraw->StartItemCatch();
    m_pDraw->SetInputEnabled( false );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_touchPanel->IsTouch() == false )
    {
      if( m_isCatchItemPut == false )
      {
        m_isCatchItemPut = true;
      }
    }
    if( m_isCatchItemPut != false )
    {
      if( m_isExScroll == false )
      {
        PutListItem();
        m_isCatchItemPut = false;
      }
    }
    m_pDraw->MoveItemIcon();
    break;

  case 2:
    if( m_pDraw->ReleaseItemIcon() != false )
    {
      break;
    }

    switch( m_pDraw->GetItemPutPosID() )
    {
    case BagDraw::PUT_ID_CANCEL:
      if( m_isSasiosaeErr != false )
      {
        m_isSasiosaeErr = false;  // フラグを下げる
        m_pDraw->SetCatchItemChangeMode( false );
        m_pDraw->SetPocketButtonActive( 0xffffffff, true );
        m_listUpdateLock = false;
        m_pDraw->SetDispActive( false );
        m_pDraw->PrintSasiosaeMessage( m_appParam->pokemon->GetPokeConst(m_pDraw->GetItemPutIndex()) );
        m_subSeq = 0;
        SetSubSeq( SEQ_MSG_WAIT );
      }
      else
      {
        m_subSeq = 3;
      }
      break;

    case BagDraw::PUT_ID_LIST:
    case BagDraw::PUT_ID_POCKET:
    case BagDraw::PUT_ID_SCROLL:
      m_subSeq = 3;
      break;

    case BagDraw::PUT_ID_PLATE:
      {
        Savedata::MyItem::ITEM_ST * item = GetCatchListItem();
        item::ITEM_DATA item_data;
        m_pItemManager->GetData( item->id, item_data );
        // 技選択へ
        if( ITEM_UTIL_IsNeedSelectSkill( &item_data ) != false )
        {
          m_appParam->select_item = item->id;
          m_pDraw->SetCatchItemChangeMode( false );
          m_pDraw->SetPocketButtonActive( 0xffffffff, true );
          m_listUpdateLock = false;

          m_pWazaSelect->Start( m_appParam->pokemon->GetPoke(m_pDraw->GetItemPutIndex()), msg_bag_04_07 );
          ChangeModeMenu();
          m_isUpdateCatch = true;
          m_subSeq = 0;
          SetSubSeq( SEQ_WAZA_SELECT );
        }
        else
        {
          m_subSeq = 0;
          SetEnd( END_SELECT, item->id, 0, m_pDraw->GetItemPutIndex(), 0 );
        }
      }
      break;
    }

    break;

  case 3:
//    m_pBagPokemon->ChangePokeSelectMode( false, m_pBagPokemon->GetSelectPos() );
    m_pDraw->SetCatchItemChangeMode( false );
    m_pDraw->SetPocketButtonActive( 0xffffffff, true );
    ChangeModeItemSelect();
    m_listUpdateLock = false;
    m_subSeq = 0;
    SetSubSeq( SEQ_MAIN );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqSale
 * @brief   アップデート：道具売却
 * @date    2015.07.17
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqSale(void)
{

  switch( m_subSeq )
  {
  case 0:
    if( m_pDraw->UpdatePrintMessage() != false )
    {
      break;
    }
    { // ウィンドウ表示
      const Savedata::MyItem::ITEM_ST * item = GetListPosItem();
      m_pSaleWindow->Start( item, GetPrice(item) );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    // ウィンドウ表示待ち
    if( m_pSaleWindow->IsStart() == false )
    {
      break;
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    { // 個数選択
      BagSaleWindow::Result result = m_pSaleWindow->GetResult();
      if( result == BagSaleWindow::RESULT_CANCEL )
      {
        m_pDraw->SetDispActive( true );
        m_pSaleWindow->End();
        m_subSeq++;
      }
      else if( result == BagSaleWindow::RESULT_DECIDE )
      {
        m_pSaleWindow->End();
        m_subSeq = 4;
      }
    }
    break;

  case 3:  // 終了
    if( m_pSaleWindow->IsEnd() == false )
    {
      break;
    }
    ChangeModeItemSelect();
    SetSubSeq( SEQ_MAIN );
    m_subSeq = 0;
    break;

  case 4:
    // ウィンドウ非表示待ち
    if( m_pSaleWindow->IsEnd() == false )
    {
      break;
    }
    // メッセージセット
    m_pDraw->PrintSaleMessage( BagDraw::MSG_ID_SALE_YESNO, GetListPosItemID(), m_pSaleWindow->GetGold()*m_pSaleWindow->GetNum() );
    m_subSeq++;
    /* FALL THROUGH */
  case 5:
    // メッセージ終了待ち
    if( m_pDraw->UpdatePrintMessage() != false )
    {
      break;
    }
    // はい・いいえセット
    StartMenuWindow( MakeYesNoMenuTable(), true );
    m_yesnoResult = MENU_ID_MAX;
    m_subSeq++;
    break;

  case 6:
    if( m_yesnoResult == MENU_ID_YES )
    {
      u32 num  = m_pSaleWindow->GetNum();
      u32 gold = m_pSaleWindow->GetGold() * num;
      u32 item_id = GetListPosItemID();
      if( num == 1 )
      {
        m_pDraw->PrintSaleMessage( BagDraw::MSG_ID_SALE_COMP, item_id, gold );
      }
      else
      {
        m_pDraw->PrintSaleMessage( BagDraw::MSG_ID_SALE_COMP_2, item_id, gold );
      }
      SubItem( item_id, num );
      m_misc->AddGold( gold );
      m_pInfoField->UpdateMyGold( m_misc->GetGold() );
      Sound::PlaySE( SEQ_SE_SYS_006 );
      m_subSeq++;
    }
    else if( m_yesnoResult == MENU_ID_NO )
    {
      m_pDraw->SetDispActive( true );
      m_subSeq = 3;
    }
    break;

  case 7:
    // メッセージ終了待ち
    if( m_pDraw->UpdatePrintMessage() != false )
    {
      break;
    }
    m_pDraw->SetDispActive( true );
    m_subSeq = 3;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqWaitEnd
 * @brief   アップデート：終了待ち
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqWaitEnd(void)
{
  if( IsFadeEnd() == false )
  {
    return;
  }

  if( m_pDraw->IsOutAnime() == false )
  {
    return;
  }
  if( m_pInfoField != NULL )
  {
    if( m_pInfoField->IsOutAnime() == false )
    {
      return;
    }
  }
  SetSubSeq( SEQ_END );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    InitDisplay
 * @brief   画面表示初期化
 * @date    2015.04.13
 */
//-----------------------------------------------------------------------------
void BagFrame::InitDisplay(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    // ポケットのNewマーク
    for( u32 i=0; i<=FIELD_BAG_POCKET_MAX; i++ )
    {
      m_pDraw->PutPocketNewIcon( i, 0 );  // バトルでは表示しない
    }
    m_pDraw->InitDisplay( m_pocket, 1, false, false );
    return;
  }
  else
  {
    // ポケットのNewマーク
    for( u32 i=0; i<=FIELD_BAG_POCKET_MAX; i++ )
    {
      m_pDraw->PutPocketNewIcon( i, m_pocketNewMax[i] );
    }

    if( m_appParam->rcv_mode == RCV_WAZAOSHIE )
    {
      m_pDraw->InitDisplay( m_pocket, 0, false, true );
      return;
    }
    else if( m_appParam->rcv_mode == RCV_EVOLUTION )
    {
      if( m_pBagPokemon->IsPutParty() != false && m_appParam->select_item == ITEM_HUSIGINAAME && m_myItem->Check(ITEM_HUSIGINAAME,1) != false )
      {
        m_pDraw->InitDisplay( m_pocket, 0, true, false );
        m_pDraw->PutMessage( BagDraw::MSG_ID_USE_SELECT );
        SetVisibleMenuCursor( false );
        /*
          @fix niji GFNMCat[1231] 関連バグ[1048]
          ChangeModePokemonSelect(...)だと入力が有効になってしまうため、
          中身の入力が有効になる部分以外を処理する
        */
        ChangePokeSelectMode( true, POKESEL_MODE_MENU_USE, 0 );
        m_pDraw->ChangeNormalMode( false );     // 道具選択無効
        m_pDraw->ChangePokeSelectMode( true );  // ポケモン選択モード有効
        return;
      }
    }
  }
  m_pDraw->InitDisplay( m_pocket, 0, false, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPaneList
 * @brief   ペインリスト初期化
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::InitPaneList(void)
{
  m_pDraw->InitPaneList( m_pPaneListView, m_pocket, m_itemTable[m_pocket], m_itemTableMax[m_pocket] );
  SetVisibleMenuCursor( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePocket
 * @brief   ポケット切り替え
 * @date    2015.04.10
 *
 * @param   new_page  新しいポケットのページ
 * @param   item      初期位置の道具
 * @param   list_pos  リスト位置
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangePocket( u32 new_pocket, u32 item, u32 list_pos )
{
  SetPocketData( m_pocket );
  m_pDraw->ChangePocket( m_pocket, new_pocket, m_itemTable[new_pocket], m_itemTableMax[new_pocket], item, list_pos );
  if( m_pInfoField != NULL )
  {
    m_pInfoField->UpdatePocketName( GetFieldPocketID(new_pocket) );
    m_pInfoField->ChangePocketBg( new_pocket );
  }
  m_pocket = new_pocket;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePocketCatch
 * @brief   ポケット切り替え（キャッチムーブ時）
 * @date    2015.05.08
 *
 * @param   new_pocket  新しいポケット
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangePocketCatch( u32 new_pocket )
{
  SetPocketData( m_pocket );
  m_pDraw->ChangePocket( m_pocket, new_pocket, m_itemTable[new_pocket], m_itemTableMax[new_pocket] );
  SetVisibleMenuCursor( false );  // メニューカーソルが表示されるので非表示にする
  m_pocket = new_pocket;
}

//-----------------------------------------------------------------------------
/**
 * @func    MovePocket
 * @brief   ポケット切り替え
 * @date    2015.04.10
 *
 * @param   mv  切り替え方向
 */
//-----------------------------------------------------------------------------
void BagFrame::MovePocket( s32 mv )
{
  // 入れ替え時は無効
  if( m_pDraw->IsItemChangeMode() != false )
  {
    return;
  }

/*
  s32 new_pocket = m_pocket + mv;
  if( new_pocket < 0 )
  {
    new_pocket = m_appParam->pocket_max - 1;
  }
  else if( new_pocket >= m_appParam->pocket_max )
  {
    new_pocket = 0;
  }
*/
  s32 new_pocket = m_pocket;
  s32 shift = m_pocket;
  for( u32 i=0; i<8; i++ )
  {
    shift += mv;
    if( shift < 0 )
    {
      shift = 7;
    }
    else if( shift >= 8 )
    {
      shift = 0;
    }
    if( ( m_pocketOnBit & (1<<shift) ) != 0 )
    {
      new_pocket = shift;
      break;
    }
  }
  ChangePocket( new_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );
  Sound::PlaySE( SEQ_SE_PAGE2 );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateItemIcon
 * @brief   アイテムアイコン更新（全体）
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::UpdateItemIcon(void)
{
  if( m_pItemIcon->IsFileOpen() == false )
  {
    return;
  }
  for( u32 i=0; i<ITEMICON_ID_MAX; i++ )
  {
    UpdateItemIcon( static_cast<ItemIconID>(i) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateItemIcon
 * @brief   アイテムアイコン更新（個別）
 * @date    2015.04.10
 *
 * @param   id  アイコンID
 */
//-----------------------------------------------------------------------------
void BagFrame::UpdateItemIcon( ItemIconID id )
{
  if( m_itemIconReq[id] == ITEM_DUMMY_DATA )
  {
    return;
  }

  switch( m_itemIconSeq[id] )
  {
  case 0:
    m_pItemIcon->LoadRequest( id, m_itemIconReq[id] );
    m_itemIconLoad[id] = m_itemIconReq[id];
    m_itemIconSeq[id] = 1;
    /* FALL THROUGH */
  case 1:
    if( m_pItemIcon->IsLoadFinished(id) == false )
    {
      break;
    }
    // 新しいリクエストが発行されていないか
    if( m_itemIconLoad[id] == m_itemIconReq[id] )
    {
      // キャッチ用
      if( id == ITEMICON_ID_CATCH )
      {
        m_pItemIcon->ReplaceReadTexture( id, m_pDraw->GetItemIconPicture() );
        m_pDraw->VisibleItemIcon( true );
      }
      // 上画面
      else
      {
        if( m_pInfoField != NULL )
        {
          m_pItemIcon->ReplaceReadTexture( id, m_pInfoField->GetItemIcon() );
          m_pInfoField->VisibleItemIcon( true );
        }
      }
      m_itemIconReq[id] = ITEM_DUMMY_DATA;
    }
    m_itemIconSeq[id] = 0;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeItemIconRequest
 * @brief   アイテムアイコン切り替えリクエスト
 * @date    2015.04.10
 *
 * @param   item  アイテム番号
 * @param   icon  アイコンID
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangeItemIconRequest( u32 item, ItemIconID icon )
{
  if( m_pItemIcon != NULL )
  {
    if( icon == ITEMICON_ID_INFO && m_pInfoField == NULL )
    {
      return;
    }

    bool flg = false;
    if( m_itemIconLoad[icon] == item )
    {
      flg = true;
    }
    else
    {
      m_itemIconReq[icon] = item;
    }
    if( icon == ITEMICON_ID_INFO )
    {
      m_pInfoField->VisibleItemIcon( flg );
    }
    else
    {
      m_pDraw->VisibleItemIcon( flg );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartMenuWindow
 * @brief   メニュー開始
 * @date    2015.04.10
 *
 * @param   cnt       項目数
 * @param   is_yesno  true = "はい・いいえ"メニュー
 */
//-----------------------------------------------------------------------------
void BagFrame::StartMenuWindow( u32 cnt, bool is_yesno )
{
  static const u32 tbl[] =
  {
    msg_bag_03_01,  // つかう
    msg_bag_03_08,  // オン（がくしゅうそうち）
    msg_bag_03_09,  // オフ（がくしゅうそうち）
    msg_bag_03_02,  // もたせる
    msg_bag_03_04,  // フリースペースへ
    msg_bag_03_05,  // もとのばしょへ（フリースペース）
    msg_bag_03_01,  // つかう（対象選択済み）
    msg_bag_03_02,  // もたせる（対象選択済み）
    msg_bag_03_01,  // つかう（キャッチムーブ用）
    msg_bag_03_02,  // もたせる（キャッチムーブ用）
    msg_bag_03_20,  // しゅるいじゅん
    msg_bag_03_22,  // ばんごうじゅん
    msg_bag_03_21,  // なまえじゅん
    msg_bag_03_23,  // New
    msg_bag_03_11,  // しらべる
    msg_bag_03_13,  // すがたを　かえる
    msg_bag_03_12,  // わざを　おぼえさせる
    msg_bag_03_10,  // やめる
    msg_bag_05_01,  // はい
    msg_bag_05_02,  // いいえ
  };

  for( u32 i=0; i<cnt; i++ )
  {
    gfl2::str::StrBuf * str = m_pDraw->GetString( tbl[m_menuTable[i]] );
    if( m_menuTable[i] == MENU_ID_CANCEL )
    {
      m_pMenuWindow->AddItem( *str, true, true );
    }
    else if( m_menuTable[i] == MENU_ID_NO )
    {
      m_pMenuWindow->AddItem( *str, true, false );
    }
    else
    {
      m_pMenuWindow->AddItem( *str, false, false );
    }
  }

  if( is_yesno == false )
  {
    m_pMenuWindow->StartMenu( 0 );
  }
  else
  {
    m_pMenuWindow->StartYesNoMenu( 0 );
  }

  m_pDraw->SetDispActive( false );
  ChangeModeMenu();
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeMenuTable
 * @brief   道具メニューテーブル生成
 * @date    2015.04.10
 *
 * @param   item  アイテム番号
 *
 * @return  項目数
 */
//-----------------------------------------------------------------------------
u32 BagFrame::MakeMenuTable( u32 item )
{
  u32 cnt = 0;

  // つかう
  if( m_pItemManager->GetParam( item, item::ITEM_DATA::PRM_ID_F_FUNC ) != 0 )
  {
    // がくしゅうそうち
    if( item == ITEM_GAKUSYUUSOUTI )
    {
      if( m_misc->GetGakusyuusoutiFlag() != false )
      {
        m_menuTable[cnt] = MENU_ID_OFF_GAKUSYUUSOUTI;
        cnt++;
      }
      else
      {
        m_menuTable[cnt] = MENU_ID_ON_GAKUSYUUSOUTI;
        cnt++;
      }
    }
    // その他
    else
    {
      m_menuTable[cnt] = MENU_ID_USE;
      cnt++;
    }
  }

  // もたせる
  if( IsPokeSetItem( item ) != false ){
    m_menuTable[cnt] = MENU_ID_SET_POKEMON;
    cnt++;
  }

  // ジガルデキューブ
  if( CheckItemUseZCube(item) != false )
  {
    m_menuTable[cnt] = MENU_ID_ZCUDE_CHECK;
    cnt++;
  }

  // フリースペースへ
  if( GetFieldPocketID(m_pocket) != BAG_POCKET_FREE )
  {
    m_menuTable[cnt] = MENU_ID_USE_IN_FREESPACE;
  }
  // もとのばしょへ
  else
  {
    m_menuTable[cnt] = MENU_ID_USE_OUT_FREESPACE;
  }
  cnt++;

  // やめる
  m_menuTable[cnt] = MENU_ID_CANCEL;
  cnt++;

  return cnt;
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeCatchMoveMenuTable
 * @brief   道具メニューテーブル生成（キャッチ用）
 * @date    2015.05.12
 *
 * @param   item  アイテム番号
 *
 * @return  項目数
 */
//-----------------------------------------------------------------------------
u32 BagFrame::MakeCatchMoveMenuTable( u32 item )
{
  bool is_zcube = CheckItemUseZCube( item );
  u32 cnt = 0;

  // つかう
  if( m_appParam->call_mode != CALL_BOX )
  {
    if( m_pItemManager->GetParam( item, item::ITEM_DATA::PRM_ID_F_FUNC ) != 0 )
    {
      if( m_pItemManager->GetParam( item, item::ITEM_DATA::PRM_ID_F_POCKET ) != BAG_POCKET_EVENT )
      {
        m_menuTable[cnt] = MENU_ID_CATCH_USE;
        cnt++;
      }
      else
      {
        if( item == ITEM_GURASIDEANOHANA ||   // グラシデアのはな
            item == ITEM_UTUSIKAGAMI ||       // うつしかがみ
            item == ITEM_IDENSINOKUSABI ||    // いでんしのくさび（合体用）
            item == ITEM_IDENSINOKUSABI_1 ||  // いでんしのくさび（解除用）
            item == ITEM_IMASIMENOTUBO ||     // いましめのツボ
            item == ITEM_SORUPURASU ||        // ソルプラス（合体用）
            item == ITEM_SORUPURASU_1 ||      // ソルプラス（解除用）
            item == ITEM_RUNAPURASU ||        // ルナプラス（合体用）
            item == ITEM_RUNAPURASU_1 ||      // ルナプラス（解除用）
            is_zcube != false )               // ジガルデキューブ
        {
          m_menuTable[cnt] = MENU_ID_CATCH_USE;
          cnt++;
        }
      }
    }

    // ジガルデキューブ
    if( is_zcube != false )
    {
      m_menuTable[cnt] = MENU_ID_ZCUDE_CHECK;
      cnt++;
    }
  }
  else
  {
    if( item::ITEM_CheckBeads(item) != false )
    {
      m_menuTable[cnt] = MENU_ID_CATCH_USE;
      cnt++;
    }
  }

  // もたせる
  if( IsPokeSetItem( item ) != false )
  {
    m_menuTable[cnt] = MENU_ID_CATCH_SET;
    cnt++;
  }

  // やめる
  m_menuTable[cnt] = MENU_ID_CANCEL;
  cnt++;

  return cnt;
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeSingleMenuTable
 * @brief   道具メニューテーブル生成（単体モード用）
 * @date    2015.05.15
 *
 * @param   item  アイテム番号
 *
 * @return  項目数
 */
//-----------------------------------------------------------------------------
u32 BagFrame::MakeSingleMenuTable( u32 item )
{
  u32 cnt = MakeCatchMoveMenuTable( item );
  for( u32 i=0; i<cnt; i++ )
  {
    if( m_menuTable[i] == MENU_ID_CATCH_USE )
    {
      m_menuTable[i] = MENU_ID_SINGLE_USE;
    }
    else if( m_menuTable[i] == MENU_ID_CATCH_SET )
    {
      m_menuTable[i] = MENU_ID_SINGLE_SET;
    }
  }
  return cnt;
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeSortMenuTable
 * @brief   ソートメニューテーブル生成
 * @date    2015.04.10
 *
 * @return  項目数
 */
//-----------------------------------------------------------------------------
u32 BagFrame::MakeSortMenuTable(void)
{
  u32 pocket_id = GetFieldPocketID( m_pocket );
  u32 cnt = 0;

  if( pocket_id == BAG_POCKET_FREE )
  {
    // しゅるいじゅん
    m_menuTable[cnt] = MENU_ID_SORT_TYPE;
    cnt++;
  }
  else
  {
    if( pocket_id == BAG_POCKET_WAZA )
    {
      // ばんごうじゅん
      m_menuTable[cnt] = MENU_ID_SORT_NUMBER;
    }
    else
    {
      // しゅるいじゅん
      m_menuTable[cnt] = MENU_ID_SORT_TYPE;
    }
    cnt++;
    // なまえじゅん
    m_menuTable[cnt] = MENU_ID_SORT_NAME;
    cnt++;
    // New
    m_menuTable[cnt] = MENU_ID_SORT_NEW;
    cnt++;
  }
  // やめる
  m_menuTable[cnt] = MENU_ID_CANCEL;
  cnt++;

  return cnt;
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeYesNoMenuTable
 * @brief   "はい・いいえ"生成
 * @date    2015.04.23
 *
 * @return  項目数
 */
//-----------------------------------------------------------------------------
u32 BagFrame::MakeYesNoMenuTable(void)
{
  m_menuTable[0] = MENU_ID_YES;
  m_menuTable[1] = MENU_ID_NO;
  return 2;
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeZCubeMenuTable
 * @brief   ジガルデキューブメニューテーブル生成
 * @date    2015.11.30
 *
 * @param   item  アイテム番号
 *
 * @return  項目数
 */
//-----------------------------------------------------------------------------
u32 BagFrame::MakeZCubeMenuTable(void)
{
  u32 cnt = 0;

  m_menuTable[cnt] = MENU_ID_ZCUBE_FORM;
  cnt++;

  for( u32 i=0; i<ZCUBE_WAZA_MAX; i++ )
  {
    if( CheckZigarudeCore( i ) != false )
    {
      m_menuTable[cnt] = MENU_ID_ZCUBE_WAZA;
      cnt++;
      break;
    }
  }

  m_menuTable[cnt] = MENU_ID_CANCEL;
  cnt++;

  return cnt;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   メニュー決定処理
 * @date    2015.04.10
 *
 * @param   id  選択された項目
 */
//-----------------------------------------------------------------------------
void BagFrame::SelectMenu( u32 id )
{
  m_pMenuWindow->EndMenu();

  switch( m_menuTable[id] )
  {
  case MENU_ID_USE:               // つかう
  case MENU_ID_ON_GAKUSYUUSOUTI:  // オン（がくしゅうそうち）
  case MENU_ID_OFF_GAKUSYUUSOUTI: // オフ（がくしゅうそうち）
    MenuUse();
    break;

  case MENU_ID_SET_POKEMON:       // もたせる
    MenuPokeSet();
    break;

  case MENU_ID_USE_IN_FREESPACE:  // フリースペースへ
    MenuAddFreeSpace();
    break;

  case MENU_ID_USE_OUT_FREESPACE: // もとのばしょへ（フリースペース）
    MenuSubFreeSpace();
    break;

  case MENU_ID_SINGLE_USE:        // つかう（対象選択済み）
    MenuSingleUse();
    break;

  case MENU_ID_SINGLE_SET:        // もたせる（対象選択済み）
    MenuSingleSet();
    break;

  case MENU_ID_CATCH_USE:         // つかう（キャッチムーブ用）
    MenuCatchUse();
    break;

  case MENU_ID_CATCH_SET:         // もたせる（キャッチムーブ用）
    MenuCatchSet();
    break;

  case MENU_ID_SORT_TYPE:         // しゅるいじゅん
  case MENU_ID_SORT_NUMBER:       // ばんごうじゅん
    MenuSortType();
    break;

  case MENU_ID_SORT_NAME:         // なまえじゅん
    MenuSortName();
    break;

  case MENU_ID_SORT_NEW:          // New
    MenuSortNew();
    break;

  case MENU_ID_ZCUDE_CHECK:       // しらべる（ジガルデキューブ）
    MenuZCubeCheck();
    break;

  case MENU_ID_ZCUBE_FORM:        // すがたをかえる（ジガルデキューブ）
    MenuZCubeForm();
    break;

  case MENU_ID_ZCUBE_WAZA:        // わざをおぼえる（ジガルデキューブ）
    MenuZCubeWaza();
    break;

  case MENU_ID_CANCEL:            // やめる
    m_pDraw->SetDispActive( true );
    ChangeModeItemSelect();
    break;

  case MENU_ID_YES:
  case MENU_ID_NO:
    m_yesnoResult = m_menuTable[id];
    break;

  case MENU_ID_ZCUBE_WAZA_1:
  case MENU_ID_ZCUBE_WAZA_2:
  case MENU_ID_ZCUBE_WAZA_3:
  case MENU_ID_ZCUBE_WAZA_4:
  case MENU_ID_ZCUBE_WAZA_5:
  case MENU_ID_ZCUBE_WAZA_6:
  case MENU_ID_ZCUBE_WAZA_7:
    MenuZCubeWazaSelect( id );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPokeSetItem
 * @brief   持たせられるアイテムか
 * @date    2015.04.03
 *
 * @param   item  アイテム番号
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsPokeSetItem( u32 item )
{
  // 大事なもの
  if( m_pItemManager->GetParam(item,item::ITEM_DATA::PRM_ID_IMP) != 0 )
  {
    return false;
  }

  // ビーズ
  if( item::ITEM_CheckBeads(item) != false )
  {
    return false;
  }

  // 持たせられるアイテムか
  if( item::ITEM_CheckPokeAdd(item) == false )
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetFieldPocketID
 * @brief   バッグ内のポケットの並びからセーブデータのポケット番号を取得
 * @date    2015.04.10
 *
 * @param   pos   ポケットの並び位置
 *
 * @return  セーブデータのポケット番号
 */
//-----------------------------------------------------------------------------
u32 BagFrame::GetFieldPocketID( u32 pos )
{
  return FieldPocketID[pos];
}

//-----------------------------------------------------------------------------
/**
 * @func    GetFieldPocketNum
 * @brief   セーブデータのポケット番号からバッグ内のポケットの並びを取得
 * @date    2015.04.13
 *
 * @param   pos   セーブデータのポケット番号
 *
 * @return  ポケットの並び位置
 */
//-----------------------------------------------------------------------------
u32 BagFrame::GetFieldPocketNum( u32 id )
{
  for( u32 i=0; i<PlayData::POCKET_DATA_MAX; i++ )
  {
    if( FieldPocketID[i] == id )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSortMenu
 * @brief   ソートメニューをセット
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagFrame::SetSortMenu(void)
{
  ResetNewItemFlag( GetListPosItem() );
  m_pDraw->PutMessage( BagDraw::MSG_ID_SORT );
  StartMenuWindow( MakeSortMenuTable() );
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePokeSelectMode
 * @brief   ポケモン選択モード切り替え
 * @date    2015.04.13
 *
 * @param   flg   true = 選択中へ, false = 非選択へ
 * @param   mode  選択モード
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagFrame::ChangePokeSelectMode( bool flg, PokeSelectMode mode, u32 pos )
{
  m_pBagPokemon->ChangePokeSelectMode( flg, pos, !m_pBagPokemon->IsPutParty() );
  m_pokeSelectMode = mode;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectPokemon
 * @brief   ポケモン選択
 * @date    2015.04.13
 *
 * @param   pos   選択位置
 */
//-----------------------------------------------------------------------------
void BagFrame::SelectPokemon( u32 pos )
{
  switch( m_pokeSelectMode )
  {
  case POKESEL_MODE_NONE:         //!< ポケモン選択モードじゃない
    GFL_ASSERT( 0 );
    break;

  case POKESEL_MODE_BATTLE_USE:   //!< バトル時の対象選択
    PokeItemUseBattle( pos );
    break;

  case POKESEL_MODE_MENU_USE:     //!< 道具を使用する対象を選択
    PokeItemUse( pos, GetListPosItem(), false );
    break;

  case POKESEL_MODE_MENU_SET:     //!< 道具を持たせる対象を選択
    PokeItemSet( pos, false );
    break;

  case POKESEL_MODE_UNION:        //!< 合体対象を選択
    PokeItemUnion( pos, GetListPosItem() );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPocketData
 * @brief   ポケットデータをセット
 * @date    2015.04.14
 *
 * @param   pocket  ポケット位置
 */
//-----------------------------------------------------------------------------
void BagFrame::SetPocketData( u32 pocket )
{
  app::tool::PaneList * list = m_pPaneListView->GetPaneList();
  u32 pos;
  f32 scroll;
  list->GetCursorData( &pos, &scroll );
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
//    if( pocket < m_appParam->pocket_max )
    if( ( m_pocketOnBit & (1<<pocket) ) != 0 )
    {
      m_appParam->pocket[pocket].cursor_pos  = pos;
      m_appParam->pocket[pocket].list_scroll = static_cast<u16>(scroll);
    }
  }
  else
  {
    m_playData->SetPocketData( pocket, pos, static_cast<u32>(scroll) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PokeItemUseBattle
 * @brief   道具使用（バトル用）
 * @date    2015.04.24
 *
 * @param   pos   ポケモン位置
 */
//-----------------------------------------------------------------------------
void BagFrame::PokeItemUseBattle( u32 pos )
{
  pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );

  // タマゴ
  if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    SetSubSeq_MsgWaitPokeSelect( pos );
  }
  // さしおさえ　
  // @fix MMCat[338]: ダブルバトルがあるのでビット管理に変更
  else if( ( m_appParam->sasiosae_index & (1<<pos) ) != 0 )
  {
    m_pDraw->PrintSasiosaeMessage( pp );
    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    SetSubSeq_MsgWaitPokeSelect( pos );
  }
  else
  {
    item::ITEM_DATA item_data;
    m_pItemManager->GetData( m_appParam->select_item, item_data );
    // 技選択へ
    if( ITEM_UTIL_IsNeedSelectSkill( &item_data ) != false )
    {
      m_pWazaSelect->Start( pp, msg_bag_04_07 );
      ChangeModeMenu();
      m_subSeq = 0;
      SetSubSeq( SEQ_WAZA_SELECT );
    }
    else
    {
      SetEnd( END_SELECT, m_appParam->select_item, WAZANO_NULL, pos, 0 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuUse
 * @brief   メニュー：つかう
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuUse(void)
{
  u16 item = GetListPosItemID();

  {
    u32 use = GetUseCheckItemIndex( item );
    if( use != ITEM_USE_DATA_MAX )
    {
      (this->*UseCheckTable[use].func)( item );
      return;
    }
  }

  // ポケモンがいない
  if( m_appParam->pokemon->GetMemberCount() == 0 )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_NO_POKEMON );
    SetSubSeq( SEQ_MSG_WAIT );
    return;
  }

  { // 全員瀕死回復
    AllDeathRcvID rcv_id = CheckAllDeathRecoverItemUse( item );
    if( rcv_id == ALL_DEATH_RECOVER_USE_TRUE )
    {
      m_pDraw->SetDispActive( true );
      m_subSeq = 0;
      SetSubSeq( SEQ_ALL_DEATH_RECOVER );
      return;
    }
    else if( rcv_id == ALL_DEATH_RECOVER_USE_FALSE )
    {
      m_pDraw->SetDispActive( false );
      m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
      SetSubSeq( SEQ_MSG_WAIT );
      return;
    }
  }

  // ポケモン選択へ
  m_pDraw->SetDispActive( true );
  if( item::ITEM_CheckWazaMachine(item) == false )
  {
    m_pDraw->PutMessage( BagDraw::MSG_ID_USE_SELECT );
  }
  else
  {
    m_pDraw->PutMessage( BagDraw::MSG_ID_USE_WAZAMACHINE_SELECT );
  }
  ChangeModePokemonSelect( POKESEL_MODE_MENU_USE );
}

//-----------------------------------------------------------------------------
/**
 * @func    PokeItemUse
 * @brief   ポケモンに道具を使う
 * @date    2015.04.17
 *
 * @param   pos       ポケモンの位置
 * @param   item      使用するアイテムデータ
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::PokeItemUse( u32 pos, const Savedata::MyItem::ITEM_ST * item, bool is_catch )
{
  pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );
  if( pp == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  // タマゴ
  if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    if( is_catch == false )
    {
      SetSubSeq_MsgWaitPokeSelect( pos );
    }
    else
    {
      SetSubSeq( SEQ_MSG_WAIT );
    }
    return;
  }

  // 技マシン
  if( item::ITEM_CheckWazaMachine(item->id) != false )
  {
    ItemUseWazaMachine( item->id, pp, pos, is_catch );
  }
  // 進化アイテム
  else if( CheckItemUseEvolution(item->id,pp) != false  )
  {
    SetEnd( END_CALL_EVOLUTION, item->id, WAZANO_NULL, pos ); 
    SetFadeOut();
  }
  // 特性カプセル
  else if( CheckItemUseTokusei(item->id) != false )
  {
    ItemUseTokusei( pp );
  }
  // フォルムチェンジアイテム
  else if( CheckItemUseFormChange(item->id,pp) != false )
  {
    ItemUseFormChange( item->id, pp, pos, is_catch );
  }
  // ビーズ
  else if( item::ITEM_CheckBeads(item->id) != false )
  {
    ItemUseBeads( item->id, pp, is_catch );
  }
  // ジガルデキューブ
  else if( CheckItemUseZCube(item->id) != false )
  {
    ItemUseZCube( item, pp, is_catch );
  }
  // その他
  else
  {
    CheckItemUseHealing( item->id, pp, pos, is_catch );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckItemUseHealing
 * @brief   回復アイテム使用チェック
 * @date    2015.04.17
 *
 * @param   item      アイテム番号
 * @param   pp        ポケモンパラメータ
 * @param   poke_pos  ポケモン位置
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::CheckItemUseHealing( u32 item, pml::pokepara::PokemonParam * pp, u32 poke_pos, bool is_catch )
{
  item::ITEM_DATA item_data;
  m_pItemManager->GetData( item, item_data );

  // 技選択へ
  if( ITEM_UTIL_IsNeedSelectSkill( &item_data ) != false )
  {
    if( item::ITEM_UTIL_GetHealingItemType(&item_data) == item::ITEM_TYPE_PP_RCV )
    {
      m_pWazaSelect->Start( pp, msg_bag_04_07 );
    }
    else
    {
      m_pWazaSelect->Start( pp, msg_bag_04_08 );
    }
//    if( is_catch != false )
//    {
    m_pDraw->SetDispActive( true );
//    }
    m_pDraw->PutMessageWindowSmall( false );

    ChangeModeMenu();
    m_subSeq = 0;
    m_isUpdateCatch = is_catch;
    SetSubSeq( SEQ_WAZA_SELECT );
    return;
	}

/*
	swk->itemUseWork.wazaPos = pml::MAX_WAZA_NUM;

	//「あおいビードロ」は専用処理
	if( swk->selItem.id == ITEM_AOIBIIDORO ){
		return UseBlueVidro();
	}
*/

  ItemUseHealing( item, pp, poke_pos, pml::MAX_WAZA_NUM, is_catch );
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseHealing
 * @brief   回復アイテム使用
 * @date    2015.04.17
 *
 * @param   item      アイテム番号
 * @param   pp        ポケモンパラメータ
 * @param   poke_pos  ポケモン位置
 * @param   waza_pos  技位置
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseHealing( u32 item, pml::pokepara::PokemonParam * pp, u32 poke_pos, u32 waza_pos, bool is_catch )
{
  // 使用できるか
  if( PokeTool::ITEM_RCV_RecoverCheck(pp,item,waza_pos,m_pItemManager) != false )
  {
    item::ITEM_DATA	item_data;
    m_pItemManager->GetData( item, item_data );
    u32	id = item::ITEM_UTIL_GetHealingItemType( &item_data );
    switch( id )
    {
    case item::ITEM_TYPE_LV_UP:   // レベルアップ
      m_pLvupWindow->SetPokemon( pp );
      // 瀕死の場合はアイコンを消す
      if( pp->GetHp() == 0 )
      {
        m_pBagPokemon->VanishStatusIcon( poke_pos );
      }
      PokeTool::ITEM_RCV_Recover( pp, item, 0, m_placeLabelID, m_pItemManager );
      m_pBagPokemon->PutLevel( poke_pos, pp );
      m_pBagPokemon->PutHp( poke_pos, pp );
//      ItemUseSub( item );
      m_useItem = item;   // 使用した道具番号
      m_pDraw->SetDispActive( false );
      {
        s32 lv = pp->GetLevel();
        m_pDraw->PrintItemUseMessage( id, pp, &lv );
      }
      ChangeModeMessage();
      m_appParam->lvupWazaSetIndex = 0;   // 技覚えのカウンタ初期化
      m_subSeq = 0;
      m_pDevTurnWatcher->Initialize( System::DeviceTurnWatcher::COUNT_RATE_30F, System::DeviceTurnWatcher::COUNT_MODE_SHORT );
      SetSubSeq( SEQ_LEVEL_UP );
      return;

    case item::ITEM_TYPE_HP_RCV:    // HP回復
    case item::ITEM_TYPE_DEATH_RCV: // 瀕死回復
      Sound::PlaySE( SEQ_SE_RECOVERY );
      {
        u32 old_hp = pp->GetHp();
        PokeTool::ITEM_RCV_Recover( pp, item, waza_pos, m_placeLabelID, m_pItemManager );
        m_pBagPokemon->StartHpRecoverAnime( poke_pos, pp, old_hp );
        m_rcvHpCount = pp->GetHp() - old_hp;
      }
      if( pp->GetSick() == pml::pokepara::SICK_NULL )
      {
        m_pBagPokemon->VanishStatusIcon( poke_pos );
      }
//      ItemUseSub( item );
      m_useItem = item;   // 使用した道具番号
//      m_pDraw->SetDispActive( true );
      m_pDraw->PutMessageWindowSmall( false );
      if( id == item::ITEM_TYPE_DEATH_RCV )
      {
        m_isRcvDead = true;
      }
      else
      {
        m_isRcvDead = false;
      }
      ChangeModeMessage();  // 入力停止（回復処理後にメッセージが表示されるため、メッセージモードでok）
      m_isUpdateCatch = is_catch;
      if( is_catch != false )
      {
        m_pDraw->SetDispActive( true );
      }
      SetSubSeq( SEQ_HP_RECOVER );
      Savedata::IncRecord( Savedata::Record::RECID_HEAL_KIDOU );  // レコード
      return;

    case item::ITEM_TYPE_PP_RCV:    // pp回復系
      Sound::PlaySE( SEQ_SE_RECOVERY );
      PokeTool::ITEM_RCV_Recover( pp, item, waza_pos, m_placeLabelID, m_pItemManager );
      if( waza_pos == pml::MAX_WAZA_NUM )
      {
        m_pDraw->PrintItemUseMessage( id, pp );
      }
      else
      {
        s32	waza = pp->GetWazaNo( waza_pos );
        m_pDraw->PrintItemUseMessage( id, pp, &waza );
      }
      Savedata::IncRecord( Savedata::Record::RECID_HEAL_KIDOU );  // レコード
      break;

    case item::ITEM_TYPE_PP_UP:     // ppUp系
    case item::ITEM_TYPE_PP_3UP:    // pp3Up系
      Sound::PlaySE( SEQ_SE_RECOVERY );
      PokeTool::ITEM_RCV_Recover( pp, item, waza_pos, m_placeLabelID, m_pItemManager );
      if( waza_pos == pml::MAX_WAZA_NUM )
      {
        m_pDraw->PrintItemUseMessage( id, pp );
      }
      else
      {
        s32	waza = pp->GetWazaNo( waza_pos );
        m_pDraw->PrintItemUseMessage( id, pp, &waza );
      }
      Savedata::IncRecord( Savedata::Record::RECID_POINTUP_USE ); // レコード
      break;

    case item::ITEM_TYPE_HP_UP:   // HP努力値UP
    case item::ITEM_TYPE_ATC_UP:  // 攻撃努力値UP
    case item::ITEM_TYPE_DEF_UP:  // 防御努力値UP
    case item::ITEM_TYPE_SPA_UP:  // 特攻努力値UP
    case item::ITEM_TYPE_SPD_UP:  // 特防努力値UP
    case item::ITEM_TYPE_AGI_UP:  // 素早さ努力値UP
      Sound::PlaySE( SEQ_SE_RECOVERY );
      PokeTool::ITEM_RCV_Recover( pp, item, waza_pos, m_placeLabelID, m_pItemManager );
      m_pBagPokemon->PutHp( poke_pos, pp );
      m_pDraw->PrintItemUseMessage( id, pp );
      break;

    case item::ITEM_TYPE_HP_DOWN:   // HP努力値DOWN
    case item::ITEM_TYPE_ATC_DOWN:  // 攻撃努力値DOWN
    case item::ITEM_TYPE_DEF_DOWN:  // 防御努力値DOWN
    case item::ITEM_TYPE_SPA_DOWN:  // 特攻努力値DOWN
    case item::ITEM_TYPE_SPD_DOWN:  // 特防努力値DOWN
    case item::ITEM_TYPE_AGI_DOWN:  // 素早さ努力値DOWN
      {
        static const pml::pokepara::PowerID eff[] =
        {
          pml::pokepara::POWER_HP,
          pml::pokepara::POWER_ATK,
          pml::pokepara::POWER_DEF,
          pml::pokepara::POWER_SPATK,
          pml::pokepara::POWER_SPDEF,
          pml::pokepara::POWER_AGI,
        };
        s32	prm[2];
        prm[0] = pp->GetFamiliarity();                                  // なつき度
        prm[1] = pp->GetEffortPower( eff[id-item::ITEM_TYPE_HP_DOWN] ); // 努力値
        PokeTool::ITEM_RCV_Recover( pp, item, waza_pos, m_placeLabelID, m_pItemManager );
        prm[0] = pp->GetFamiliarity() - prm[0];
        prm[1] = pp->GetEffortPower( eff[id-item::ITEM_TYPE_HP_DOWN] ) - prm[1];
        m_pBagPokemon->PutHp( poke_pos, pp );
        m_pDraw->PrintItemUseMessage( id, pp, prm );
      }
      break;

		case item::ITEM_TYPE_NEMURI_RCV:		// 眠り回復
		case item::ITEM_TYPE_DOKU_RCV:			// 毒回復
		case item::ITEM_TYPE_YAKEDO_RCV:		// 火傷回復
		case item::ITEM_TYPE_KOORI_RCV:			// 氷回復
		case item::ITEM_TYPE_MAHI_RCV:			// 麻痺回復
		case item::ITEM_TYPE_KONRAN_RCV:		// 混乱回復
		case item::ITEM_TYPE_ALL_ST_RCV:		// 全快
		case item::ITEM_TYPE_MEROMERO_RCV:	// メロメロ回復
      Sound::PlaySE( SEQ_SE_RECOVERY );
      PokeTool::ITEM_RCV_Recover( pp, item, waza_pos, m_placeLabelID, m_pItemManager );
      m_pBagPokemon->VanishStatusIcon( poke_pos );
      m_pDraw->PrintItemUseMessage( id, pp );
      Savedata::IncRecord( Savedata::Record::RECID_HEAL_KIDOU );  // レコード
      break;
		}

//    ItemUseSub( item );

    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    if( is_catch == false )
    {
      // まだ消費してないので２個あれば続ける
      if( m_myItem->Check( item, 2 ) == false )
      {
        SetSubSeq_MsgWaitItemSub( item );
      }
      else
      {
        SetSubSeq_MsgWaitItemSubPokeSelect( item, poke_pos );
      }
    }
    else
    {
      SetSubSeq_MsgWaitItemSub( item );
    }
	}
  else
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );

    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    if( is_catch == false )
    {
      if( m_myItem->Check( item, 1 ) == false )
      {
        SetSubSeq( SEQ_MSG_WAIT );
      }
      else
      {
        SetSubSeq_MsgWaitPokeSelect( poke_pos );
      }
    }
    else
    {
      SetSubSeq( SEQ_MSG_WAIT );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseWazaMachine
 * @brief   技マシン使用
 * @date    2015.04.20
 *
 * @param   item      アイテム番号
 * @param   pp        ポケモンパラメータ
 * @param   poke_pos  ポケモン位置
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseWazaMachine( u32 item, pml::pokepara::PokemonParam * pp, u32 poke_pos, bool is_catch )
{
  WazaNo waza = static_cast<WazaNo>( item::ITEM_GetWazaNo(item) );

  m_pDraw->SetDispActive( false );
  ChangeModeMessage();

  // 覚えている
  if( pp->HaveWaza(waza) != false )
  {
    m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_ERR_SAME, pp, waza );
    if( is_catch == false )
    {
      SetSubSeq_MsgWaitPokeSelect( poke_pos );
    }
    else
    {
      SetSubSeq( SEQ_MSG_WAIT );
    }
  }
  // 覚えられない
  else if( pp->CheckWazaMachine( item::ITEM_GetWazaMashineNo(item) ) == false )
  {
    m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_ERR_BAD, pp, waza );
    if( is_catch == false )
    {
      SetSubSeq_MsgWaitPokeSelect( poke_pos );
    }
    else
    {
      SetSubSeq( SEQ_MSG_WAIT );
    }
  }
  // 起動
  else
  {
    m_pDraw->PrintWazaMachineMessage( BagDraw::MSG_ID_WM_START );
    SetSubSeq( SEQ_USE_WAZA_MACHINE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckAllDeathRecoverItemUse
 * @brief   全員瀕死回復アイテム使用チェック
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 *
 * @retval  ALL_DEATH_ITEM_USE_TRUE  = 使用可
 * @retval  ALL_DEATH_ITEM_USE_FALSE = 使用不可
 * @retval  ALL_DEATH_ITEM_NOT       = 全員瀕死回復アイテムではない
 */
//-----------------------------------------------------------------------------
BagFrame::AllDeathRcvID BagFrame::CheckAllDeathRecoverItemUse( u16 item )
{
  item::ITEM_DATA	data;
  m_pItemManager->GetData( item, data );
  if( item::ITEM_UTIL_IsDeathRecoverAllItem( &data ) == false )
  {
    return ALL_DEATH_RECOVER_NOT;
  }
  if( GetAllDeathRecoverUsePos( item ) != -1 )
  {
    return ALL_DEATH_RECOVER_USE_TRUE;
  }
  return ALL_DEATH_RECOVER_USE_FALSE;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetAllDeathRecoverUsePos
 * @brief   全ポケモンに対しての瀕死回復アイテム使用チェック
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 *
 * @retval  -1 = 瀕死のポケモンなし
 * @retval  -1 != 瀕死のポケモン位置
 */
//-----------------------------------------------------------------------------
int BagFrame::GetAllDeathRecoverUsePos( u16 item )
{
  for( u32 i=0; i<m_appParam->pokemon->GetMemberCount(); i++ )
  {
    const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( i );
    if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
    {
      continue;
    }
    if( PokeTool::ITEM_RCV_RecoverCheck( pp, item, 0, m_pItemManager ) != false )
    {
      return i;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetUseCheckItemIndex
 * @brief   道具使用テーブルのインデックスを取得
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 *
 * @retval  ITEM_USE_DATA_MAX = テーブルにデータなし
 * @retval  ITEM_USE_DATA_MAX != 対応したデータのインデックス
 */
//-----------------------------------------------------------------------------
u32 BagFrame::GetUseCheckItemIndex( u16 item )
{
  if( GFL_NELEMS(UseCheckTable) != ITEM_USE_DATA_MAX )
  {
    GFL_ASSERT( 0 );
    return ITEM_USE_DATA_MAX;
  }

  for( u32 i=0; i<ITEM_USE_DATA_MAX; i++ )
  {
    if( item == UseCheckTable[i].itemID )
    {
      return i;
    }
  }
  return ITEM_USE_DATA_MAX;
}

//--------------------------------------------------------------------------
/**
 * @func    ItemUse_ReturnField
 * @brief   道具使用処理：フィールド復帰アイテム
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 *
 * @return  0
 */
//--------------------------------------------------------------------------
int BagFrame::ItemUse_ReturnField( u16 item )
{
  if( Field::FieldItemUse::ItemUseCheck( m_appParam->itemuse_work, item ) == Field::FieldItemUse::FLDITEM_RET_USE_OK )
  {
    // 特定のアイテムのみ、アイテムを減らす
//    if( item == ITEM_AMAIMITU || item == ITEM_ANANUKENOHIMO )
    {
      m_myItem->Sub( item, 1 );
      if( m_pocket == PlayData::FREE_SPACE_POCKET && m_myItem->Check(item,1) == false )
      {
        m_itemTableMax[PlayData::FREE_SPACE_POCKET] = m_myItem->MakeFreeSpaceItemTable( m_itemTable[PlayData::FREE_SPACE_POCKET], Savedata::MyItem::TOTAL_ITEM_MAX );
      }
    }
    m_gameData->SetFieldMenuOpen( false );  // @note メニューを閉じた状態でフィールドに復帰させる
    m_pDraw->SetDispActive( true );
    SetEnd( END_SELECT, item );
    SetFadeOut();
	}
  else
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR );
    SetSubSeq( SEQ_MSG_WAIT );
  }
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @func    ItemUse_Gakusyuusouti
 * @brief   道具使用処理：がくしゅうそうち
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 *
 * @return  0
 */
//--------------------------------------------------------------------------
int BagFrame::ItemUse_Gakusyuusouti( u16 item )
{
  if( m_misc->GetGakusyuusoutiFlag() != false )
  {
    m_misc->SetGakusyuusoutiFlag( false );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_GAKUSYUUSOUTI_OFF );
  }
  else
  {
    m_misc->SetGakusyuusoutiFlag( true );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_GAKUSYUUSOUTI_ON );
  }
  m_pDraw->SetDispActive( false );
  SetSubSeq( SEQ_MSG_WAIT );
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @func    ItemUse_Spray
 * @brief   道具使用処理：スプレー
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 *
 * @return  0
 */
//--------------------------------------------------------------------------
int BagFrame::ItemUse_Spray( u16 item )
{
  if( m_eventWork->CanUseMushiyoke() != false )
  {
//    ItemUseSub( item );

    // @fix GFNMCat[2535]: アイテムデータは１バイトなので、10倍する
    m_eventWork->SetMushiyokeCount( item, m_pItemManager->GetParam(item,item::ITEM_DATA::PRM_ID_ATTACK)*10 );

    Sound::PlaySE( SEQ_SE_SYS_018 );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ITEM, item );
    SetSubSeq_MsgWaitItemSub( item );
	}
  else
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_ERR_SPRAY );
    SetSubSeq( SEQ_MSG_WAIT );
	}
  m_pDraw->SetDispActive( false );
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @func    ItemUse_RotomPower
 * @brief   道具使用処理：ロトムパワー関連
 * @date    2017.03.23
 *
 * @param   item  アイテム番号
 *
 * @return  0
 */
//--------------------------------------------------------------------------
int BagFrame::ItemUse_RotomPower( u16 item )
{
  if( Field::FieldItemUse::ItemUseCheck( m_appParam->itemuse_work, item ) == Field::FieldItemUse::FLDITEM_RET_USE_OK )
  {
    m_myItem->Sub( item, 1 );
    if( m_pocket == PlayData::FREE_SPACE_POCKET && m_myItem->Check(item,1) == false )
    {
      m_itemTableMax[PlayData::FREE_SPACE_POCKET] = m_myItem->MakeFreeSpaceItemTable( m_itemTable[PlayData::FREE_SPACE_POCKET], Savedata::MyItem::TOTAL_ITEM_MAX );
    }
    m_gameData->SetFieldMenuOpen( false );  // @note メニューを閉じた状態でフィールドに復帰させる
    m_pDraw->SetDispActive( true );
    SetEnd( END_SELECT, item );
    SetFadeOut();
	}
  else
  {
    BagDraw::MsgID msg_id = BagDraw::MSG_ID_USE_ERROR;

    if( m_rotomPower->IsRotomPower() != false )
    {
      struct ROTOM_POWER_DATA
      {
        u32 item;
        Field::RotomPower::ROTOM_POWER_ID id;
      };
      static const ROTOM_POWER_DATA tbl[] =
      {
        { ITEM_TAMAGOHUKAPON, Field::RotomPower::ROTOM_POWER_ID_EGG_INC    },   // タマゴふかポン
        { ITEM_YASUURIPON,    Field::RotomPower::ROTOM_POWER_ID_SALE       },   // やすうりポン
        { ITEM_OKODUKAIPON,   Field::RotomPower::ROTOM_POWER_ID_MONEY      },   // おこづかいポン
        { ITEM_KEIKENTIPON,   Field::RotomPower::ROTOM_POWER_ID_EXP_INC    },   // けいけんちポン
        { ITEM_NATUKIPON,     Field::RotomPower::ROTOM_POWER_ID_NATSUKI    },   // なつきポン
        { ITEM_SOUGUUPON,     Field::RotomPower::ROTOM_POWER_ID_SOUGUU_INC },   // そうぐうポン
        { ITEM_ONMITUPON,     Field::RotomPower::ROTOM_POWER_ID_SOUGUU_DEC },   // おんみつポン
      };

      for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
      {
        if( tbl[i].item == item )
        {
          if( m_rotomPower->GetRotomPowerID() == tbl[i].id )
          {
            msg_id = BagDraw::MSG_ID_USE_ERROR_ROTOM_POWER;
          }
          break;
        }
      }
    }

    m_pDraw->SetDispActive( false );
    m_pDraw->PrintMessage( msg_id );
    SetSubSeq( SEQ_MSG_WAIT );
  }

  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseSub
 * @brief   道具使用後の消費処理
 * @date    2015.04.15
 *
 * @param   item  アイテム番号
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseSub( u16 item )
{
  m_useItem = item;
	if( m_pItemManager->GetParam( item, item::ITEM_DATA::PRM_ID_USE_SPEND ) != 0 )
  {
    return;
	}
  if( m_myItem->Check(item,1) != false )
  {
    m_updatePlateLock = true;
  }
  SubItem( item, 1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckItemUseEvolution
 * @brief   進化アイテム使用チェック
 * @date    2015.04.17
 *
 * @param   item  アイテム番号
 * @param   pp    ポケモンパラメータ
 *
 * @retval  true  = 使用可
 * @retval  false = 使用不可
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckItemUseEvolution( u32 item, const pml::pokepara::PokemonParam * pp )
{
  MonsNo	mons;
  u32 evo_root;
  // アローラ以外の場所では通常ポケモンに進化したい要望
  pml::pokepara::EvolveSituation evoSituation;
  // 進化設定 場所の情報だけ欲しいので天気と逆さ状況は適当
  PokeTool::SetupEvolveSituation( &evoSituation, GFL_SINGLETON_INSTANCE(GameSys::GameManager), false, Field::weather::SUNNY );
  return pp->CanEvolveByItem( &evoSituation, item, &mons, &evo_root );
}

//-----------------------------------------------------------------------------
/**
 * @func    RecoverEvolutionItem
 * @brief   進化画面からの復帰処理
 * @date    2015.04.21
 */
//-----------------------------------------------------------------------------
void BagFrame::RecoverEvolutionItem(void)
{
  if( m_appParam->rcv_mode == RCV_EVOLUTION )
  {
    if( m_appParam->select_item != ITEM_HUSIGINAAME )
    {
      m_myItem->Sub( m_appParam->select_item, 1 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckItemUseTokusei
 * @brief   特性変更アイテム使用チェック
 * @date    2015.04.17
 *
 * @param   item  アイテム番号
 *
 * @retval  true  = 使用可
 * @retval  false = 使用不可
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckItemUseTokusei( u32 item )
{
  if( item == ITEM_TOKUSEIKAPUSERU )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseTokusei
 * @brief   特性変更アイテム使用
 * @date    2015.04.17
 *
 * @param   pp  ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseTokusei( pml::pokepara::PokemonParam * pp )
{
  m_pDraw->SetDispActive( false );
  ChangeModeMessage();

  // 特性２がない
  pml::personal::LoadPersonalData( pp->GetMonsNo(), pp->GetFormNo() );
  if( pml::personal::GetTokuseiPattern() != pml::personal::TOKUSEI_PATTERN_ABC )
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    SetSubSeq( SEQ_MSG_WAIT );
    return;
  }

  // 特性３が設定されている
  u32	index = pp->GetTokuseiIndex();
  if( index == 2 )
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    SetSubSeq( SEQ_MSG_WAIT );
    return;
  }

  if( index == 0 )
  {
    m_newTokusei = static_cast<TokuseiNo>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_tokusei2) );
  }
  else
  {
    m_newTokusei = static_cast<TokuseiNo>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_tokusei1) );
  }
  m_pDraw->PrintTokuseikapuseruMessage( BagDraw::MSG_ID_TOKUSEI_CHANGE_CHECK, pp, m_newTokusei );
  SetSubSeq( SEQ_USE_TOKUSEIKAPUSERU );
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseBeads
 * @brief   ビーズ使用
 * @date    2015.11.05
 *
 * @param   item      アイテム番号
 * @param   pp        ポケモンパラメータ
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseBeads( u32 item, pml::pokepara::PokemonParam * pp, bool is_catch )
{
  m_isUpdateCatch = is_catch;

  m_pDraw->SetDispActive( false );
  ChangeModeMessage();

  u32 piece = m_pBagPokemon->GetPiece( m_pItemManager, item );

  // こうかなし
  if( pml::waza::ZenryokuWazaSystem::IsZenryokuWazaEnable( pp, piece ) == false )
  {
    m_pDraw->PrintBeadsMessage( BagDraw::MSG_ID_BEADS_WAZA_NONE );
    m_subSeq = 0;
    SetSubSeq( SEQ_USE_BEADS );
  }
  else
  {
    m_pDraw->PrintBeadsMessage( BagDraw::MSG_ID_BEADS_START, pp, item, piece );
    m_subSeq = 2;
    SetSubSeq( SEQ_USE_BEADS );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckItemUseZCube
 * @brief   ジガルデキューブ使用チェック
 * @date    2015.11.30
 *
 * @param   item  アイテム番号
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckItemUseZCube( u32 item )
{
  if( item == ITEM_ZIGARUDEKYUUBU )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseZCube
 * @brief   ジガルデキューブ使用
 * @date    2015.11.30
 *
 * @param   item      アイテムデータ
 * @param   pp        ポケモンパラメータ
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseZCube( const Savedata::MyItem::ITEM_ST * item, pml::pokepara::PokemonParam * pp, bool is_catch )
{
  m_isUpdateCatch = is_catch;

  // ジガルデ以外
  if( pp->GetMonsNo() != MONSNO_ZIGARUDE )
  {
    m_pDraw->SetDispActive( false );
    ChangeModeMessage();
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    SetSubSeq( SEQ_MSG_WAIT );
    return;
  }

  m_pDraw->PutMessage( BagDraw::MSG_ID_ZCUBE_MENU, item, pp );
  m_pDraw->ChangePokeSelectMode( false );     // ポケモン選択モード無効
  StartMenuWindow( MakeZCubeMenuTable() );
  SetSubSeq( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuZCubeCheck
 * @brief   ジガルデキューブ用メニュー"しらべる"
 * @date    2015.11.30
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuZCubeCheck(void)
{
  m_pDraw->SetDispActive( false );
  m_pDraw->PrintZCubeMessage(
    BagDraw::MSG_ID_ZCUBE_CHECK,
    m_eventWork->GetEventWork(WK_SYS_ZIGARUDE_COLLECT),
    m_eventWork->GetZigarudeCell() );
  SetSubSeq( SEQ_MSG_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuZCubeForm
 * @brief   ジガルデキューブ用メニュー"すがたをかえる"
 * @date    2015.11.30
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuZCubeForm(void)
{
  u32 pos = m_pBagPokemon->GetSelectPos();
  pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );

  FormNo form = pp->GetFormNo();

  if( form == FORMNO_ZIGARUDE_SW50PER || form == FORMNO_ZIGARUDE_SW10PER )
  {
    ChangeModeMessage();  // 入力停止（デモ後にメッセージが表示されるため、メッセージモードでok）
    SetFormChangeWork( pos, ITEM_ZIGARUDEKYUUBU );  // 固定でいいはず
    m_formChangeID = FORM_CHG_ID_ITEM_USE;
    SetSubSeq( SEQ_USE_FORM_CHANGE );
  }
  else
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintZCubeMessage( BagDraw::MSG_ID_ZCUBE_FORM_ERR );
    SetSubSeq( SEQ_MSG_WAIT );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuZCubeWaza
 * @brief   ジガルデキューブ用メニュー"わざをおぼえる"
 * @date    2015.11.30
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuZCubeWaza(void)
{
  static const WazaNo tbl[] =
  {
    WAZANO_KOAPANISSYAA,    // コアパニッシャー
    WAZANO_SAUZANAROO,      // サウザンアロー
    WAZANO_SAUZANWHEEBU,    // サウザンウェーブ
    WAZANO_SINSOKU,         // しんそく
    WAZANO_RYUUNOMAI,       // りゅうのまい
  };

  u32 cnt = 0;

  for( u32 i=0; i<ZCUBE_WAZA_MAX; i++ )
  {
    if( CheckZigarudeCore( i ) != false )
    {
      gfl2::str::StrBuf * str = m_pDraw->GetWazaName( tbl[i] );
      m_pMenuWindow->AddItem( *str, false, false );
      m_menuTable[cnt] = static_cast<MenuID>( MENU_ID_ZCUBE_WAZA_1 + cnt );
      m_zCubeWazaTable[cnt] = tbl[i];
      cnt++;
    }
  }

  { // やめる
    gfl2::str::StrBuf * str = m_pDraw->GetString( msg_bag_03_10 );
    m_pMenuWindow->AddItem( *str, true, true );
    m_menuTable[cnt] = MENU_ID_CANCEL;
    cnt++;
  }

  m_pMenuWindow->StartMenu( 0 );
  m_pDraw->SetDispActive( false );
  m_pDraw->SetVisibleMessageWindow( false );
  ChangeModeMenu();
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuZCubeWazaSelect
 * @brief   ジガルデキューブ用技選択
 * @date    2015.11.30
 *
 * @param   index   選択位置
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuZCubeWazaSelect( u32 index )
{
  m_zCubeSelectWazaNo = m_zCubeWazaTable[index];
  m_subSeq = 0;
  SetSubSeq( SEQ_USE_ZCUBE_WAZA );
//    m_pDraw->SetDispActive( true );
//    ChangeModeItemSelect();
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckItemUseFormChange
 * @brief   道具使用時のフォルムチェンジチェック
 * @date    2015.04.17
 *
 * @param   item  アイテム番号
 * @param   pp    ポケモンパラメータ
 *
 * @retval  true  = フォルムチェンジ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckItemUseFormChange( u32 item, const pml::pokepara::PokemonParam * pp )
{
  // ありえないけど、タマゴは強制的に失敗させる
  if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true )
  {
    return false;
  }

  MonsNo mons = pp->GetMonsNo();
  FormNo form = pp->GetFormNo();

  // グラシデアのはな
  if( item == ITEM_GURASIDEANOHANA )
  {
    if( mons == MONSNO_SHEIMI && form == FORMNO_SHEIMI_LAND )
    {
      // 配布ポケモンかどうか
      if( pp->GetEventPokeFlag() == false )
      {
        return false;
      }
      // 状態異常「氷」かどうか
      if( pp->GetSick() == pml::pokepara::SICK_KOORI )
      {
        return false;
      }
      // 時間帯
      if( Field::EvTime::GetAlolaTimeZone(m_gameData) >= GameSys::TimeZone::NIGHT )
      {
        return false;
      }
      return true;
    }
  }
  // うつしかがみ
  else if( item == ITEM_UTUSIKAGAMI )
  {
    if( mons == MONSNO_TORUNEROSU || mons == MONSNO_BORUTOROSU || mons == MONSNO_RANDOROSU )
    {
      return true;
    }
  }
  // いでんしのくさび（合体用）
  else if( item == ITEM_IDENSINOKUSABI )
  {
    if( mons == MONSNO_KYUREMU && form == FORMNO_KYUREMU_A )
    {
      return true;
    }
  }
  // いでんしのくさび（解除用）
  else if( item == ITEM_IDENSINOKUSABI_1 )
  {
    if( mons == MONSNO_KYUREMU && ( form == FORMNO_KYUREMU_WHITE || form == FORMNO_KYUREMU_BLACK ) )
    {
      return true;
    }
  }
  // いましめのツボ
  else if( item == ITEM_IMASIMENOTUBO )
  {
    if( mons == MONSNO_HUUPA && form == FORMNO_HUUPA_NORMAL )
    {
      return true;
    }
  }
  // ソルプラス/ルナプラス（合体用）
  else if( item == ITEM_SORUPURASU || item == ITEM_RUNAPURASU )
  {
    if( mons == MONSNO_PURIZUMU && form == FORMNO_PURIZUMU_NORMAL )
    {
      return true;
    }
  }
  // ソルプラス（解除用）
  else if( item == ITEM_SORUPURASU_1 )
  {
    if( mons == MONSNO_PURIZUMU && form == FORMNO_PURIZUMU_FORM_S )
    {
      return true;
    }
  }
  // ルナプラス（解除用）
  else if( item == ITEM_RUNAPURASU_1 )
  {
    if( mons == MONSNO_PURIZUMU && form == FORMNO_PURIZUMU_FORM_L )
    {
      return true;
    }
  }
  // ミツスイ用
  else if( mons == MONSNO_MITUSUI )
  {
    FormNo chg_form = GetMitsusuiForm( item );
    if( chg_form != FORMNO_MITUSUI_MAX && chg_form != form )
    {
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMitsusuiForm
 * @brief   道具に対応したミツスイのフォルムを取得
 * @date    2015.12.25
 *
 * @param   item  アイテム番号
 *
 * @return  フォルム番号
 */
//-----------------------------------------------------------------------------
FormNo BagFrame::GetMitsusuiForm( u32 item )
{
  static const u16 tbl[][2] = 
  { // item, form
    { ITEM_KURENAINOMITU,  FORMNO_MITUSUI_FLAMENCO },
    { ITEM_YAMABUKINOMITU, FORMNO_MITUSUI_CHEER },
    { ITEM_USUMOMONOMITU,  FORMNO_MITUSUI_HULA },
    { ITEM_MURASAKINOMITU, FORMNO_MITUSUI_NICHIBU },
  };
  for( u32 i=0; i<FORMNO_MITUSUI_MAX; i++ )
  {
    if( item == tbl[i][0] )
    {
      return tbl[i][1];
    }
  }
  return FORMNO_MITUSUI_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseFormChange
 * @brief   道具使用時のフォルムチェンジ
 * @date    2015.04.17
 *
 * @param   item      アイテム番号
 * @param   pp        ポケモンパラメータ
 * @param   poke_pos  ポケモン位置
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseFormChange( u32 item, const pml::pokepara::PokemonParam * pp, u32 poke_pos, bool is_catch )
{
  MonsNo mons = pp->GetMonsNo();
  FormNo form = pp->GetFormNo();

  // グラシデアのはな, うつしかがみ, いましめのツボ
  if( item == ITEM_GURASIDEANOHANA || item == ITEM_UTUSIKAGAMI || item == ITEM_IMASIMENOTUBO )
  {
    ChangeModeMessage();  // 入力停止（デモ後にメッセージが表示されるため、メッセージモードでok）
    SetFormChangeWork( poke_pos, item );
    m_formChangeID = FORM_CHG_ID_ITEM_USE;
    SetSubSeq( SEQ_USE_FORM_CHANGE );
    return;
  }
	// いでんしのくさび（合体用）
  else if( item == ITEM_IDENSINOKUSABI )
  {
    if( mons == MONSNO_KYUREMU && form == FORMNO_KYUREMU_A )
    {
      ItemUseFormChange_UnionCombine( item, pp, poke_pos, is_catch, mons, form );
      return;
    }
  }
  // いでんしのくさび（解除用）
  else if( item == ITEM_IDENSINOKUSABI_1 )
  {
    if( mons == MONSNO_KYUREMU && ( form == FORMNO_KYUREMU_WHITE || form == FORMNO_KYUREMU_BLACK ) )
    {
      ItemUseFormChange_UnionSeparate( item, poke_pos );
      return;
    }
  }
	// ソルプラス/ルナプラス（合体用）
  else if( item == ITEM_SORUPURASU || item == ITEM_RUNAPURASU )
  {
    if( mons == MONSNO_PURIZUMU && form == FORMNO_PURIZUMU_NORMAL )
    {
      ItemUseFormChange_UnionCombine( item, pp, poke_pos, is_catch, mons, form );
      return;
    }
  }
  // ソルプラス（解除用）
  else if( item == ITEM_SORUPURASU_1 )
  {
    if( mons == MONSNO_PURIZUMU && form == FORMNO_PURIZUMU_FORM_S )
    {
      ItemUseFormChange_UnionSeparate( item, poke_pos );
      return;
    }
  }
  // ルナプラス（解除用）
  else if( item == ITEM_RUNAPURASU_1 )
  {
    if( mons == MONSNO_PURIZUMU && form == FORMNO_PURIZUMU_FORM_L )
    {
      ItemUseFormChange_UnionSeparate( item, poke_pos );
      return;
    }
  }
  // ミツスイ用アイテム
  else if( mons == MONSNO_MITUSUI )
  {
    FormNo chg_form = GetMitsusuiForm( item );
    if( chg_form == FORMNO_MITUSUI_MAX && chg_form == form )
    {
      GFL_ASSERT( 0 );
    }
    ChangeModeMessage();  // 入力停止（デモ後にメッセージが表示されるため、メッセージモードでok）
    SetFormChangeWork( poke_pos, item );
    m_formChangeID = FORM_CHG_ID_ITEM_USE;
    SetSubSeq( SEQ_USE_FORM_CHANGE );
    return;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseFormChange_UnionCombine
 * @brief   道具使用時の合体
 * @date    2016.12.22
 *
 * @param   item      アイテム番号
 * @param   pp        ポケモンパラメータ
 * @param   poke_pos  ポケモン位置
 * @param   is_catch  true = キャッチ操作
 * @param   mons      ポケモン番号
 * @param   form      フォルム番号
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseFormChange_UnionCombine( u32 item, const pml::pokepara::PokemonParam * pp, u32 poke_pos, bool is_catch, MonsNo mons, FormNo form )
{
  if( pp->GetHp() == 0 )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR_DEATH1 );
    ChangeModeMessage();
    if( is_catch == false )
    {
      SetSubSeq_MsgWaitPokeSelect( m_pBagPokemon->GetSelectPos() );
    }
    else
    {
      SetSubSeq( SEQ_MSG_WAIT );
    }
  }
  else
  {
    if( m_appParam->call_mode != CALL_FIELD_MENU )
    {
      m_pBagPokemon->ChangePlateParty( true );  // 全ポケモンを表示
    }

    if( is_catch != false )
    {
      m_pDraw->SetDispActive( true );
    }
    m_unionPos = poke_pos;
    ChangeModePokemonSelect( POKESEL_MODE_UNION, m_unionPos );
    SetFormChangeWork( m_unionPos, item );
    m_pDraw->PutMessage( BagDraw::MSG_ID_UNION_SELECT );
    SetSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemUseFormChange_UnionSeparate
 * @brief   道具使用時の合体解除
 * @date    2016.12.22
 *
 * @param   item      アイテム番号
 * @param   poke_pos  ポケモン位置
 */
//-----------------------------------------------------------------------------
void BagFrame::ItemUseFormChange_UnionSeparate( u32 item, u32 poke_pos )
{
  ChangeModeMessage();
  if( m_appParam->pokemon->GetMemberCount() >= pml::PokeParty::MAX_MEMBERS )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR_OFF );
    SetSubSeq( SEQ_MSG_WAIT );
  }
  else
  {
    if( m_appParam->call_mode != CALL_FIELD_MENU )
    {
      m_pBagPokemon->ChangePlateParty( true );  // 全ポケモンを表示
      m_pBagPokemon->ChangePokeSelectMode( true, poke_pos );  // @note ポケモン選択モードが変化するが「つかう」なので問題ないはず
    }

    SetFormChangeWork( m_pBagPokemon->GetSelectPos(), item );
    m_formChangeID = FORM_CHG_ID_ITEM_USE;
    SetSubSeq( SEQ_USE_FORM_CHANGE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFormChangeWork
 * @brief   フォルムチェンジワーク設定
 * @date    2015.04.17
 *
 * @param   pos1    １匹目の位置
 * @param   item1   １匹目に影響する道具
 * @param   pos2    ２匹目の位置
 * @param   item2   ２匹目に影響する道具
 */
//-----------------------------------------------------------------------------
void BagFrame::SetFormChangeWork( u32 pos1, u32 item1, u32 pos2, u32 item2 )
{
  FORM_CHG_WORK * fwk = &m_formChangeWork;

  fwk->poke[0].pp         = m_appParam->pokemon->GetPoke( pos1 );
  fwk->poke[0].item       = item1;
  fwk->poke[0].pos        = pos1;
  fwk->poke[0].is_moudoku = m_appParam->pokemon->IsSickMoudoku( pos1 );
  if( pos2 < pml::PokeParty::MAX_MEMBERS )
  {
    fwk->poke[1].pp         = m_appParam->pokemon->GetPoke( pos2 );
    fwk->poke[1].item       = item2;
    fwk->poke[1].pos        = pos2;
    fwk->poke[1].is_moudoku = m_appParam->pokemon->IsSickMoudoku( pos2 );
  }
  else
  {
    fwk->poke[1].pp         = NULL;
    fwk->poke[1].item       = ITEM_DUMMY_DATA;
    fwk->poke[1].pos        = pml::PokeParty::MAX_MEMBERS;
    fwk->poke[1].is_moudoku = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    FormChangeCore
 * @brief   フォルムチェンジ処理
 * @date    2015.04.17
 *
 * @param   poke  フォルムチェンジポケモン
 */
//-----------------------------------------------------------------------------
void BagFrame::FormChangeCore( const FORM_CHG_POKEMON * poke )
{
  // 単純なフォルムの反転
  if( poke->item == ITEM_GURASIDEANOHANA || poke->item == ITEM_UTUSIKAGAMI )
  {
    FormNo form = poke->pp->GetFormNo();
    poke->pp->ChangeFormNo( form^1 );
    m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
    m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID(), false );
    m_pBagPokemon->PutHp( poke->pos, poke->pp );
  }
  // ジガルデキューブ
  else if( poke->item == ITEM_ZIGARUDEKYUUBU )
  {
    FormNo form = poke->pp->GetFormNo();
    if( form == FORMNO_ZIGARUDE_SW50PER )
    {
      form = FORMNO_ZIGARUDE_SW10PER;
    }
    else if( form == FORMNO_ZIGARUDE_SW10PER )
    {
      form = FORMNO_ZIGARUDE_SW50PER;
    }
    else
    {
      GFL_ASSERT( 0 );  // @note 抜けても元のフォルムになるだけ
    }
    poke->pp->ChangeFormNo( form );
    m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
    m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID(), false );
    m_pBagPokemon->PutHp( poke->pos, poke->pp );
  }
  // いましめのつぼ（フーパ)
  else if( poke->item == ITEM_IMASIMENOTUBO )
  {
		FormNo form = poke->pp->GetFormNo();
		PokeTool::form::SetHuupaForm( poke->pp, form^1 );
    m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
    m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID(), false );
    m_pBagPokemon->PutHp( poke->pos, poke->pp );
  }
  // いでんしのくさび（合体用）
  else if( poke->item == ITEM_IDENSINOKUSABI )
  {
    u32 pos = m_pBagPokemon->GetSelectPos();
    pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );
    // フォルムチェンジ
    if( pp->GetMonsNo() == MONSNO_ZEKUROMU )
    {
      FormChangeCore_Combine( Savedata::UnionPokemon::POKEID_KYUREMU, poke, FORMNO_KYUREMU_BLACK, pp, pos );
    }
    else
    {
      FormChangeCore_Combine( Savedata::UnionPokemon::POKEID_KYUREMU, poke, FORMNO_KYUREMU_WHITE, pp, pos );
    }
  }
  // いでんしのくさび（解除用）
  else if( poke->item == ITEM_IDENSINOKUSABI_1 )
  {
    FormChangeCore_Separate( Savedata::UnionPokemon::POKEID_KYUREMU, poke, FORMNO_KYUREMU_A );
  }
  // ソルプラス（合体用）
  else if( poke->item == ITEM_SORUPURASU )
  {
    u32 pos = m_pBagPokemon->GetSelectPos();
    FormChangeCore_Combine(
      Savedata::UnionPokemon::POKEID_NEKUROZUMA_S, poke, FORMNO_PURIZUMU_FORM_S, m_appParam->pokemon->GetPoke(pos), pos );
  }
  // ソルプラス（解除用）
  else if( poke->item == ITEM_SORUPURASU_1 )
  {
    FormChangeCore_Separate( Savedata::UnionPokemon::POKEID_NEKUROZUMA_S, poke, FORMNO_PURIZUMU_NORMAL );
  }
  // ルナプラス（合体用）
  else if( poke->item == ITEM_RUNAPURASU )
  {
    u32 pos = m_pBagPokemon->GetSelectPos();
    FormChangeCore_Combine(
      Savedata::UnionPokemon::POKEID_NEKUROZUMA_L, poke, FORMNO_PURIZUMU_FORM_L, m_appParam->pokemon->GetPoke(pos), pos );
  }
  // ルナプラス（解除用）
  else if( poke->item == ITEM_RUNAPURASU_1 )
  {
    FormChangeCore_Separate( Savedata::UnionPokemon::POKEID_NEKUROZUMA_L, poke, FORMNO_PURIZUMU_NORMAL );
  }
  // その他
  else
  {
    // ミツスイ用アイテム
    FormNo chg_form = GetMitsusuiForm( poke->item );
    if( chg_form != FORMNO_MITUSUI_MAX )
    {
      poke->pp->ChangeFormNo( chg_form );
      m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
      m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID(), false );
      m_pBagPokemon->PutHp( poke->pos, poke->pp );
    }
    else
    {
      FormNo form = poke->pp->GetNextFormNoFromHoldItem( poke->item );
      poke->pp->ChangeFormNo( form );
      m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
      m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID(), false );
      m_pBagPokemon->PutHp( poke->pos, poke->pp );
    }
  }

  m_zukan->SetPokeSee( *poke->pp );	// 図鑑みたフラグセット
}

//-----------------------------------------------------------------------------
/**
 * @func    FormChangeCore_Combine
 * @brief   合体処理
 * @date    2016.12.22
 *
 * @param   id        データID
 * @param   poke      合体元のポケモン
 * @param   new_form  合体後後のフォルム
 * @param   pp2       合体対象のポケモン
 * @param   poke2     合体対象のポケモン位置
 */
//-----------------------------------------------------------------------------
void BagFrame::FormChangeCore_Combine( Savedata::UnionPokemon::PokemonID id, const FORM_CHG_POKEMON * poke, FormNo new_form, pml::pokepara::PokemonParam * pp2, u32 pos2 )
{
  SwapMyItem( poke->item );
  poke->pp->ChangeFormNo( new_form );
  // 合体元の位置のアイコンを書き換える
  m_pBagPokemon->ChangePokeSelectMode( true, poke->pos );
  m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
  m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID(), false );
  m_pBagPokemon->InitPaneCore( poke->pos );
  // 合体対象の位置を非表示
  m_pBagPokemon->VisiblePlate( pos2, false );
  // 合体対象を記憶
  m_unionPokemon->SetPokemon( id, pp2 );
  pml::PokeParty * party = m_appParam->pokemon->GetPokeParty();
  party->RemoveMember( pos2 );
  if( pos2 < m_unionPos )
  {
    m_unionPos -= 1;
    if( m_pBagPokemon->IsPutParty() == false )
    {
      m_appParam->pokemon_index -= 1;
      m_pBagPokemon->SetSingleModePos( m_appParam->pokemon_index );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    FormChangeCore_Separate
 * @brief   合体解除処理
 * @date    2016.12.22
 *
 * @param   id        データID
 * @param   poke      分離元のポケモン
 * @param   new_form  分離後のフォルム
 */
//-----------------------------------------------------------------------------
void BagFrame::FormChangeCore_Separate( Savedata::UnionPokemon::PokemonID id, const FORM_CHG_POKEMON * poke, FormNo new_form )
{
  SwapMyItem( poke->item );
  // ポケモン復帰
  pml::pokepara::PokemonParam * pp = GFL_NEW_LOW(m_heap->GetSystemHeap()) pml::pokepara::PokemonParam( m_heap->GetSystemHeap() );
  m_unionPokemon->GetPokemon( id, pp );
  pml::PokeParty * party = m_appParam->pokemon->GetPokeParty();
  u32 add_pos = m_appParam->pokemon->GetMemberCount();
  party->AddMember( *pp );
  m_unionPokemon->InitPokemonData( id, m_heap->GetSystemHeap() ); // 退避データをクリア
  // フォルムチェンジ
  poke->pp->ChangeFormNo( new_form );
  // かきかえ
  // 分離するポケモン
  m_pBagPokemon->ReplaceTexture( poke->pos, poke->pp );
  m_pBagPokemon->InitPlate( poke->pos, poke->pp, poke->is_moudoku, m_pItemManager, GetListPosItemID() );
  m_pBagPokemon->InitPaneCore( poke->pos );
  // 分離したポケモン
  m_pBagPokemon->ReplaceTexture( add_pos, pp );
  m_pBagPokemon->InitPlate( add_pos, pp, m_appParam->pokemon->IsSickMoudoku(add_pos), m_pItemManager, GetListPosItemID() );
  m_pBagPokemon->InitPaneCore( add_pos );
  m_pBagPokemon->VisiblePlate( add_pos, true );
  GFL_DELETE pp;

  m_pBagPokemon->ChangePokeSelectMode( true, poke->pos ); // @note ポケモン選択モードが変化するが「つかう」なので問題ないはず
}

//-----------------------------------------------------------------------------
/**
 * @func    PokeItemUnion
 * @brief   ポケモン合体処理
 * @date    2015.04.17
 *
 * @param   pos   合体させるポケモンの位置
 * @param   item  使用するアイテムデータ
 */
//-----------------------------------------------------------------------------
void BagFrame::PokeItemUnion( u32 pos, const Savedata::MyItem::ITEM_ST * item )
{
  pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );
  if( pp == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  ChangeModeMessage();

  // タマゴ
  if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR_EGG );
    SetSubSeq_MsgWaitPokeSelect( pos );
  }
  else
  {
    if( item->id == ITEM_IDENSINOKUSABI )
    {
      if( IsPokeItemUnionDataNull( Savedata::UnionPokemon::POKEID_KYUREMU, pos ) == false )
      {
        return;
      }
      if( IsPokeItemUnionMonsNo( pp, MONSNO_ZEKUROMU, pos ) != false )
      {
        return;
      }
      if( IsPokeItemUnionMonsNo( pp, MONSNO_RESIRAMU, pos ) != false )
      {
        return;
      }
    }
    else if( item->id == ITEM_SORUPURASU )
    {
      if( IsPokeItemUnionDataNull( Savedata::UnionPokemon::POKEID_NEKUROZUMA_S, pos ) == false )
      {
        return;
      }
      if( IsPokeItemUnionMonsNo( pp, MONSNO_NIKKOU, pos ) != false )
      {
        return;
      }
    }
    else if( item->id == ITEM_RUNAPURASU )
    {
      if( IsPokeItemUnionDataNull( Savedata::UnionPokemon::POKEID_NEKUROZUMA_L, pos ) == false )
      {
        return;
      }
      if( IsPokeItemUnionMonsNo( pp, MONSNO_GEKKOU, pos ) != false )
      {
        return;
      }
    }
    else
    {
      GFL_ASSERT( 0 );
    }

    m_pDraw->SetDispActive( false );
    m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR );
    SetSubSeq_MsgWaitPokeSelect( pos );

/*
    MonsNo mons;
    {
      pml::pokepara::PokemonParam * tmp_pp = GFL_NEW_LOW(m_heap->GetSystemHeap()) pml::pokepara::PokemonParam( m_heap->GetSystemHeap() );
      m_unionPokemon->GetPokemon( Savedata::UnionPokemon::POKEID_KYUREMU, tmp_pp );
      mons = tmp_pp->GetMonsNo();
      GFL_DELETE tmp_pp;
    }
    // すでに合体済み
    if( mons != MONSNO_NULL )
    {
      m_pDraw->SetDispActive( false );
      m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR );
      SetSubSeq_MsgWaitPokeSelect( pos );
    }
    else
    {
      mons = pp->GetMonsNo();
      if( mons == MONSNO_ZEKUROMU || mons == MONSNO_RESIRAMU )
      {
        // 瀕死チェック
        if( pp->GetHp() != 0 )
        {
          m_formChangeID = FORM_CHG_ID_ITEM_USE;
          SetSubSeq( SEQ_USE_FORM_CHANGE );
        }
        else
        {
          m_pDraw->SetDispActive( false );
          m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR_DEATH2 );
          SetSubSeq_MsgWaitPokeSelect( pos );
        }
      }
      else
      {
        m_pDraw->SetDispActive( false );
        m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR );
        SetSubSeq_MsgWaitPokeSelect( pos );
      }
    }
*/
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPokeItemUnionDataNull
 * @brief   ポケモン合体処理：退避先データのNULLチェック
 * @date    2016.12.22
 *
 * @param   id    データID
 * @param   pos   合体させるポケモンの位置
 *
 * @retval  true  = NULLデータ
 * @retval  false = それ以外
 *
 * @note  falseの場合はメッセージがセットされる
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsPokeItemUnionDataNull( Savedata::UnionPokemon::PokemonID id, u32 pos )
{
  pml::pokepara::PokemonParam * pp = GFL_NEW_LOW(m_heap->GetSystemHeap()) pml::pokepara::PokemonParam( m_heap->GetSystemHeap() );
  m_unionPokemon->GetPokemon( id, pp );
  MonsNo mons = pp->GetMonsNo();
  GFL_DELETE pp;

  // すでに合体済み
  if( mons != MONSNO_NULL )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR );
    SetSubSeq_MsgWaitPokeSelect( pos );
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPokeItemUnionMonsNo
 * @brief   ポケモン合体処理：合体対象のポケモン番号チェック
 * @date    2016.12.22
 *
 * @param   pp    合体対象のポケモン
 * @param   mons  合体可能なポケモン番号
 * @param   pos   合体させるポケモンの位置
 *
 * @retval  true  = 合体可能なポケモン
 * @retval  false = それ以外
 *
 * @note  trueの場合はメッセージがセットされる
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsPokeItemUnionMonsNo( pml::pokepara::PokemonParam * pp, MonsNo mons, u32 pos )
{
  if( pp->GetMonsNo() != mons )
  {
    return false;
  }

  // 瀕死チェック
  if( pp->GetHp() != 0 )
  {
    m_formChangeID = FORM_CHG_ID_ITEM_USE;
    SetSubSeq( SEQ_USE_FORM_CHANGE );
  }
  else
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintFormChangeMessage( BagDraw::MSG_ID_UNION_ERR_DEATH2 );
    SetSubSeq_MsgWaitPokeSelect( pos );
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    SwapMyItem
 * @brief   アイテムを入れ替える（アイテム番号を変更する）
 * @date    2015.04.17
 *
 * @param   item  アイテム番号
 *
 * @li  "いでんしのくさび(合体用)"と"いでんしのくさび(分離用)"を変更する場合に使用
 */
//-----------------------------------------------------------------------------
void BagFrame::SwapMyItem( u32 item )
{
  m_myItem->Swap( item );
  m_pInfoField->UpdateInfo( GetListPosItem() );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuPokeSet
 * @brief   メニュー：もたせる
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuPokeSet(void)
{
  // ポケモンがいない
  if( m_appParam->pokemon->GetMemberCount() == 0 )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_NO_POKEMON );
    SetSubSeq( SEQ_MSG_WAIT );
  }
  else
  {
    m_pDraw->SetDispActive( true );
    m_pDraw->PutMessage( BagDraw::MSG_ID_SET_SELECT );
    ChangeModePokemonSelect( POKESEL_MODE_MENU_SET );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PokeItemSet
 * @brief   ポケモンに道具を持たせる
 * @date    2015.04.15
 *
 * @param   pos       ポケモンの位置
 * @param   is_catch  true = キャッチ操作
 */
//-----------------------------------------------------------------------------
void BagFrame::PokeItemSet( u32 pos, bool is_catch )
{
  const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( pos );

  m_pDraw->SetDispActive( false );
  ChangeModeMessage();

  // タマゴ
  if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ERROR_EGG );
    if( is_catch == false )
    {
      SetSubSeq_MsgWaitPokeSelect( pos );
    }
    else
    {
      SetSubSeq( SEQ_MSG_WAIT );
    }
    return;
  }

  u32 item = pp->GetItem();
  // 道具を持っている
  if( item != ITEM_DUMMY_DATA )
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_CHANGE_CHECK, item, ITEM_DUMMY_DATA, pp );
    m_subSeq = 2;
  }
  else
  {
    m_pDraw->PrintMessage( BagDraw::MSG_ID_SET_ITEM, GetListPosItemID() );
    m_subSeq = 1;
  }
  SetSubSeq( SEQ_POKE_SET );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListPosItem
 * @brief   カーソル位置のアイテムデータを取得
 * @date    2015.04.15
 *
 * @return  アイテムデータ
 */
//-----------------------------------------------------------------------------
Savedata::MyItem::ITEM_ST * BagFrame::GetListPosItem(void)
{
  app::tool::PaneList * list = m_pPaneListView->GetPaneList();
  u32 pos;
  f32 scroll;
  list->GetCursorData( &pos, &scroll );

  pos = static_cast<u32>(scroll) + pos;

  if( pos >= m_itemTableMax[m_pocket] )
  {
    return NULL;
  }

  Savedata::MyItem::ITEM_ST ** table = m_itemTable[m_pocket];
  return table[pos];
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListPosItemID
 * @brief   カーソル位置のアイテム番号を取得
 * @date    2015.12.26
 *
 * @return  アイテム番号
 */
//-----------------------------------------------------------------------------
u32 BagFrame::GetListPosItemID(void)
{
  Savedata::MyItem::ITEM_ST * item = GetListPosItem();
  if( item != NULL )
  {
    return item->id;
  }
  return ITEM_DUMMY_DATA;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectCancel
 * @brief   戻るボタンを選択
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagFrame::SelectCancel(void)
{
  switch( m_pokeSelectMode )
  {
  case POKESEL_MODE_NONE:         //!< ポケモン選択モードじゃない
    if( m_pDraw->IsItemChangeMode() == false )
    {
      SetEnd( END_CANCEL );
      SetFadeOut();
    }
    else
    {
      EndItemChange( false, false );
    }
    break;
  case POKESEL_MODE_BATTLE_USE:   //!< バトル時の対象選択
  case POKESEL_MODE_MENU_USE:     //!< 道具を使用する対象を選択
  case POKESEL_MODE_MENU_SET:     //!< 道具を持たせる対象を選択
    ChangeModeItemSelect();
    break;
  case POKESEL_MODE_UNION:        //!< 合体対象を選択
    m_pBagPokemon->ChangePlateParty( false );
    ChangeModeItemSelect();
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPokemonItem
 * @brief   ポケモンに道具をセット
 * @date    2015.04.15
 *
 * @param   item_id   アイテム番号
 */
//-----------------------------------------------------------------------------
void BagFrame::SetPokemonItem( u32 item_id )
{
  // プレート更新
  u32 pos = m_pBagPokemon->GetSelectPos();
  pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( pos );
  u32 poke_item = pp->GetItem();
  pp->SetItem( item_id );
  m_pBagPokemon->PutItemIcon( pos, pp );
  // アイテム更新
  bool free_space = false;
  if( poke_item != ITEM_DUMMY_DATA )
  {
    m_myItem->Add( poke_item, 1 );
    ResetPocketItemTable( poke_item );  // リスト更新
  }
  SubItem( item_id, 1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckSetItemFormChange
 * @brief   フォルムチェンジが発生するか
 * @date    2015.05.08
 *
 * @param   pos       ポケモン位置
 * @param   pp        ポケモンパラメータ
 * @param   item_id   アイテム番号
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckSetItemFormChange( u32 pos, pml::pokepara::PokemonParam * pp, u32 item_id )
{
  FormNo form = pp->GetNextFormNoFromHoldItem( item_id );

  if( pp->GetFormNo() != form )
  {
    // アルセウル, ゲノセクト, グリプス２は演出いらないのでこの場でチェンジ
    MonsNo mons = pp->GetMonsNo();
    if( mons == MONSNO_ARUSEUSU || mons == MONSNO_GENOSEKUTO || mons == MONSNO_GURIPUSU2 )
    {
      pp->ChangeFormNo( form );
      m_pBagPokemon->PutHp( pos, pp );
      m_zukan->SetPokeSee( *pp );   // 図鑑みたフラグセット
      return false;   // フォルムチェンジのシーケンスにいかないようにする
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SubItem
 * @brief   道具を減らす
 * @date    2015.04.17
 *
 * @param   item_id   アイテム番号
 * @param   sub       減らす個数
 */
//-----------------------------------------------------------------------------
void BagFrame::SubItem( u16 item_id, u16 sub )
{
  u16 item_num = m_myItem->GetItemNum( item_id );

  m_myItem->Sub( item_id, sub );

  ResetPocketItemTable( item_id );   // リスト更新
  // リスト表示更新
  ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );
  SetVisibleMenuCursor( false );  // カーソル非表示

  {
    const Savedata::MyItem::ITEM_ST * st = GetListPosItem();
    u16 pos_id  = ITEM_DUMMY_DATA;
    u16 pos_num = 0;
    if( st != NULL )
    {
      pos_id  = st->id;
      pos_num = st->num;
    }
    if( m_pInfoField != NULL )
    {
      ChangeItemIconRequest( pos_id, ITEMICON_ID_INFO );
      m_pInfoField->UpdateInfo( pos_id, pos_num );
    }
    m_pBagPokemon->InitPlate( m_pItemManager, pos_id, m_pBagPokemon->IsPutParty() );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuAddFreeSpace
 * @brief   メニュー"フリースペースへ"
 * @date    2015.04.24
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuAddFreeSpace(void)
{
  Savedata::MyItem::ITEM_ST * item = GetListPosItem();

  m_pDraw->SetDispActive( false );
  m_pDraw->PrintFreeSpaceMessage( BagDraw::MSG_ID_ADD_FREESPACE, item->id );

  m_myItem->AddFreeSpace( m_itemTable[PlayData::FREE_SPACE_POCKET], item, &m_itemTableMax[PlayData::FREE_SPACE_POCKET] );
  SetCountNew( PlayData::FREE_SPACE_POCKET );
  m_pDraw->PutPocketNewIcon( PlayData::FREE_SPACE_POCKET, m_pocketNewMax[PlayData::FREE_SPACE_POCKET] );
  ResetPocketItemTable( item->id );  // リスト更新
  ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );         // 表示更新
  m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める

  SetSubSeq( SEQ_MSG_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSubFreeSpace
 * @brief   メニュー"もとのばしょへ（フリースペース）"
 * @date    2015.04.24
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSubFreeSpace(void)
{
  Savedata::MyItem::ITEM_ST * item = GetListPosItem();
  u32 pocket_id = m_pItemManager->GetParam( item->id, item::ITEM_DATA::PRM_ID_F_POCKET );

  m_pDraw->SetDispActive( false );
  m_pDraw->PrintFreeSpaceMessage( BagDraw::MSG_ID_SUB_FREESPACE, item->id, pocket_id );

  m_myItem->SubFreeSpace( item );
  ResetPocketItemTable( item->id );  // リスト更新
  ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );  // 表示更新
  m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める

  SetSubSeq( SEQ_MSG_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSort
 * @brief   ソートメニュー処理（全体）
 * @date    2015.04.27
 *
 * @param   msg_id  メッセージID
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSort( u32 msg_id )
{
  ChangePocket( m_pocket, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL ); // 表示更新
  m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
  m_pDraw->SetDispActive( false );
  m_pDraw->PrintSortMessage( static_cast<BagDraw::MsgID>(msg_id) );
  Sound::PlaySE( SEQ_SE_COMPLETE1 );
  SetSubSeq( SEQ_MSG_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSortType
 * @brief   ソートメニュー処理（種類順）
 * @date    2015.04.27
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSortType(void)
{
  SortType();
  if( GetFieldPocketID( m_pocket ) == BAG_POCKET_WAZA )
  {
    MenuSort( BagDraw::MSG_ID_SORT_NUMBER );
  }
  else
  {
    MenuSort( BagDraw::MSG_ID_SORT_TYPE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSortName
 * @brief   ソートメニュー処理（名前順）
 * @date    2015.04.27
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSortName(void)
{
  SortName();
  MenuSort( BagDraw::MSG_ID_SORT_NAME );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSortNew
 * @brief   ソートメニュー処理（New）
 * @date    2015.04.27
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSortNew(void)
{
  SortNew();
  MenuSort( BagDraw::MSG_ID_SORT_NEW );
}

//-----------------------------------------------------------------------------
/**
 * @func    SortType
 * @brief   ソート処理（種類順）
 * @date    2015.04.27
 */
//-----------------------------------------------------------------------------
void BagFrame::SortType(void)
{
  Savedata::MyItem::ITEM_ST ** table = m_itemTable[m_pocket];
  int max = m_itemTableMax[m_pocket];

  // 現在の並びのデータ作成
  for( int i=0; i<max; i++ )
  {
    if( table[i]->id == ITEM_DUMMY_DATA )
    {
      GFL_ASSERT( 0 );
      break;
    }
    // カテゴリ << 28 + ソート番号 << 16 + アイテム番号
    m_pTypeSortTable[i] = ( m_pItemManager->GetParam(table[i]->id,item::ITEM_DATA::PRM_ID_ITEM_TYPE) << 28 ) +
                          ( m_pItemManager->GetParam(table[i]->id,item::ITEM_DATA::PRM_ID_SORT) << 16 ) +
                          table[i]->id;
  }

  // ソート
  u32 pocket_id = GetFieldPocketID( m_pocket );
  for( int i=0; i<max-1; i++ )
  {
    if( table[i]->id == ITEM_DUMMY_DATA )
    {
      GFL_ASSERT( 0 );
      break;
    }
    for( int j=i; j<max; j++ )
    {
      if( table[j]->id == ITEM_DUMMY_DATA )
      {
        GFL_ASSERT( 0 );
        break;
      }
      if( m_pTypeSortTable[i] > m_pTypeSortTable[j] )
      {
        if( pocket_id != BAG_POCKET_FREE )
        {
          m_myItem->Change( table[i], table[j] );
        }
        u32	tmp = m_pTypeSortTable[i];
        m_pTypeSortTable[i] = m_pTypeSortTable[j];
        m_pTypeSortTable[j] = tmp;
      }
    }
  }

  if( pocket_id == BAG_POCKET_FREE )
  {
    m_itemTableMax[m_pocket] = 0;
    for( int i=0; i<max; i++ )
    {
      Savedata::MyItem::ITEM_ST * item_st = m_myItem->GetItem( m_pTypeSortTable[i] & 0xffff );
      m_myItem->AddFreeSpace( m_itemTable[m_pocket], item_st, &m_itemTableMax[m_pocket], true );
    }
    SetCountNew( PlayData::FREE_SPACE_POCKET );
    m_pDraw->PutPocketNewIcon( PlayData::FREE_SPACE_POCKET, m_pocketNewMax[PlayData::FREE_SPACE_POCKET] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SortName
 * @brief   ソート処理（名前順）
 * @date    2015.04.27
 */
//-----------------------------------------------------------------------------
void BagFrame::SortName(void)
{
  if( GetFieldPocketID( m_pocket ) == BAG_POCKET_WAZA )
  {
    m_myItem->SortByWazaName( m_pSortWaza, m_itemTable[m_pocket], m_itemTableMax[m_pocket] );
  }
  else
  {
    m_myItem->SortByName( m_pSortItem, m_itemTable[m_pocket], m_itemTableMax[m_pocket] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SortNew
 * @brief   ソート処理（New）
 * @date    2015.04.27
 */
//-----------------------------------------------------------------------------
void BagFrame::SortNew(void)
{
  if( m_pocket == PlayData::FREE_SPACE_POCKET )
  {
    GFL_ASSERT( 0 );
    return;
  }

  Savedata::MyItem::ITEM_ST ** table = m_itemTable[m_pocket];
  int max = m_itemTableMax[m_pocket];

  for( int i=0; i<max-1; i++ )
  {
    if( table[i]->id == ITEM_DUMMY_DATA )
    {
      GFL_ASSERT( 0 );
      break;
    }
    if( m_myItem->IsNew(table[i]) == false )
    {
      for( int j=i+1; j<max; j++ )
      {
        if( m_myItem->IsNew(table[j]) != false )
        {
          m_myItem->Change( table[i], table[j] );
          break;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCountNew
 * @brief   ポケット内の新規アイテム数を設定
 * @date    2015.04.28
 *
 * @param   pocket  ポケット
 */
//-----------------------------------------------------------------------------
void BagFrame::SetCountNew( u32 pocket )
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    return;
  }

  Savedata::MyItem::ITEM_ST ** table = m_itemTable[pocket];
  m_pocketNewMax[pocket] = 0;
  for( u32 i=0; i<m_itemTableMax[pocket]; i++ )
  {
    if( m_myItem->IsNew(table[i]) != false )
    {
      m_pocketNewMax[pocket]++;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePocketNewIcon
 * @brief   Newアイコン表示更新処理
 * @date    2015.04.28
 */
//-----------------------------------------------------------------------------
void BagFrame::UpdatePocketNewIcon(void)
{
  if( m_pocketNewMax[m_pocket] == 0 )
  {
    return;
  }

  app::tool::PaneList * list = m_pPaneListView->GetPaneList();

#if 1 // カウンタ版
  if( m_listUpdateLock != false || list->IsInputEnabled() == false || list->GetUpdateResult() != app::tool::PaneList::RET_NONE )
  {
    if( m_upperPutCount != START_ITEM_NEW_RESET_COUNT || m_itemTableMax[m_pocket] == 0 )
    {
      m_upperPutCount = ITEM_NEW_RESET_COUNT;
    }
  }
  else
  {
    if( m_itemTableMax[m_pocket] != 0 )
    {
      Savedata::MyItem::ITEM_ST * pos_item = GetListPosItem();
      if( pos_item->id == m_upperItem )
      {
        if( m_upperPutCount > 0 )
        {
          m_upperPutCount--;
        }
        else
        {
          ResetNewItemFlag( pos_item );
        }
      }
      else
      {
        m_upperItem = pos_item->id;
        if( m_upperPutCount != START_ITEM_NEW_RESET_COUNT )
        {
          m_upperPutCount = ITEM_NEW_RESET_COUNT;
        }
      }
    }
    else
    {
      m_upperPutCount = ITEM_NEW_RESET_COUNT;
    }
  }
#else // 待ちなし版
  if( m_listUpdateLock == false && list->IsInputEnabled() != false && list->GetUpdateResult() == app::tool::PaneList::RET_NONE )
  {
    if( m_itemTableMax[m_pocket] != 0 )
    {
      ResetNewItemFlag( GetListPosItem() );
    }
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetNewItemFlag
 * @brief   アイテムの新規フラグを落とす
 * @date    2015.04.30
 *
 * @param   item  アイテムデータ
 */
//-----------------------------------------------------------------------------
void BagFrame::ResetNewItemFlag( Savedata::MyItem::ITEM_ST * item )
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    return;
  }

  if( item == NULL )
  {
    return;
  }

  if( m_myItem->IsNew( item ) == false )
  {
    return;
  }

  m_myItem->ResetNew( item );
//  if( CheckCnvButtonItem( item->id ) == false )
  {
    app::tool::PaneList * list = m_pPaneListView->GetPaneList();
    u32 pos;
    f32 scroll;
    list->GetCursorData( &pos, &scroll );
    m_pDraw->PutItemNewIcon( list->GetPosPaneIndex(pos), false );
  }
  m_pocketNewMax[m_pocket]--;
  if( m_pocketNewMax[m_pocket] == 0 )
  {
    m_pDraw->PutPocketNewIcon( m_pocket, 0 );
  }
}

/*
bool BagFrame::CheckCnvButtonItem( u32 item )
{
  for( u32 i=0; i<2; i++ )
  {
    if( m_debugCnvItem[i] == item )
    {
      return true;
    }
  }
  return false;
}

bool BagFrame::AddCnvButtonItem( u32 item )
{
  for( u32 i=0; i<2; i++ )
  {
    if( m_debugCnvItem[i] == ITEM_DUMMY_DATA )
    {
      m_debugCnvItem[i] = item;
      return true;
    }
  }
  return false;
}

bool BagFrame::SubCnvButtonItem( u32 item )
{
  for( u32 i=0; i<2; i++ )
  {
    if( m_debugCnvItem[i] == item )
    {
      m_debugCnvItem[i] = ITEM_DUMMY_DATA;
      return true;
    }
  }
  return false;
}

void BagFrame::MenuAddCnvButton(void)
{
  AddCnvButtonItem( GetListPosItemID() );
  SetCnvButtonIcon( true );
  m_pDraw->SetDispActive( false );
  m_pDraw->PrintMessage( BagDraw::MSG_ID_ADD_CNV_BUTTON );
  SetSubSeq( SEQ_MSG_WAIT );
}

void BagFrame::MenuSubCnvButton(void)
{
  SubCnvButtonItem( GetListPosItemID() );
  SetCnvButtonIcon( false );
  m_pDraw->SetDispActive( false );
  m_pDraw->PrintMessage( BagDraw::MSG_ID_SUB_CNV_BUTTON );
  SetSubSeq( SEQ_MSG_WAIT );
}

void BagFrame::SetCnvButtonIcon( bool flg )
{
  app::tool::PaneList * list = m_pPaneListView->GetPaneList();
  u32 pos;
  f32 scroll;
  list->GetCursorData( &pos, &scroll );
  m_pDraw->PutCnvButtonIcon( list->GetPosPaneIndex(pos), flg );
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    StartItemChange
 * @brief   リストの道具入れ替え開始
 * @date    2015.04.30
 */
//-----------------------------------------------------------------------------
void BagFrame::StartItemChange(void)
{
  ResetNewItemFlag( GetListPosItem() );

  if( m_pocket == PlayData::FREE_SPACE_POCKET )
  {
    m_cpyFreeSpaceItem = GFL_NEW_LOW_ARRAY( m_heap->GetDeviceHeap() ) u16[m_itemTableMax[m_pocket]];
    Savedata::MyItem::ITEM_ST ** table = m_itemTable[m_pocket];
    for( u32 i=0; i<m_itemTableMax[m_pocket]; i++ )
    {
      m_cpyFreeSpaceItem[i] = table[i]->id;
    }
  }
  else
  {
    u32	max;
    Savedata::MyItem::ITEM_ST * pocket_item = m_myItem->GetPocketItem( GetFieldPocketID(m_pocket), &max );
		m_cpyPocketItem = GFL_NEW_LOW_ARRAY( m_heap->GetDeviceHeap() ) Savedata::MyItem::ITEM_ST[max];
    gfl2::std::MemCopy( pocket_item, m_cpyPocketItem, sizeof(Savedata::MyItem::ITEM_ST)*max );
  }

  m_pDraw->SetItemChangeMode( true );

  m_pDraw->ChangePaneListItemColor( true );

  if( m_pInfoField != NULL )
  {
    m_pInfoField->SetVisibleXButton( false );
  }

  ChangeModeItemChange();

  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndItemChange
 * @brief   リストの道具入れ替え終了
 * @date    2015.04.30
 *
 * @param   flg         true = 実行, false = キャンセル
 * @param   is_play_se  true = SE再生
 */
//-----------------------------------------------------------------------------
void BagFrame::EndItemChange( bool flg, bool is_play_se )
{
  if( flg == false )
  {
    if( m_pocket == PlayData::FREE_SPACE_POCKET )
    {
      Savedata::MyItem::ITEM_ST ** table = m_itemTable[m_pocket];
      for( u32 i=0; i<m_itemTableMax[m_pocket]; i++ )
      {
        for( u32 j=0; j<m_itemTableMax[m_pocket]; j++ )
        {
          if( table[j]->id == m_cpyFreeSpaceItem[i] )
          {
            table[j]->free_space = i+1;
            break;
          }
        }
      }
      m_itemTableMax[m_pocket] = m_myItem->MakeFreeSpaceItemTable( m_itemTable[m_pocket], Savedata::MyItem::TOTAL_ITEM_MAX );
    }
    else
    {
      u32	max;
      Savedata::MyItem::ITEM_ST * pocket_item = m_myItem->GetPocketItem( GetFieldPocketID(m_pocket), &max );
      gfl2::std::MemCopy( m_cpyPocketItem, pocket_item, sizeof(Savedata::MyItem::ITEM_ST)*max );
    }

//    Sound::PlaySE( SEQ_SE_CANCEL1 );  // ボタンでSEが再生されるのでいらない
  }
  else
  {
    if( is_play_se != false )
    {
      Sound::PlaySE( SEQ_SE_DECIDE1 );
    }
  }

  GFL_SAFE_DELETE_ARRAY( m_cpyPocketItem );
  GFL_SAFE_DELETE_ARRAY( m_cpyFreeSpaceItem );

  m_pDraw->SetItemChangeMode( false );

  if( m_pInfoField != NULL )
  {
    m_pInfoField->SetVisibleXButton( true );
  }

  ChangeModeItemSelect();
}


//-----------------------------------------------------------------------------
/**
 * @func    GetPokemonItem
 * @brief   ポケモンの持ち物を取得開始
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
void BagFrame::GetPokemonItem(void)
{
  u32 item_id = m_pBagPokemon->GetItem( m_pBagPokemon->GetSelectPos() );
  ChangeItemIconRequest( item_id, ITEMICON_ID_CATCH );
  if( m_pInfoField != NULL )
  {
    ChangeItemIconRequest( item_id, ITEMICON_ID_INFO );
    m_pInfoField->UpdateInfo( item_id );
    m_pInfoField->UpdatePokeItemCategory();
  }
  { // アイコン移動
    u16 x, y;
    m_touchPanel->GetXY( &x, &y );
    m_pDraw->MoveItemIcon( x, y );
  }
  { // ポケットのボタンをパッシブにする
    u32 bit = 0xffffffff;
    if( item::ITEM_CheckPiece(item_id ) == false )
    {
      if( m_myItem->IsRegisterFreeSpace(item_id) != false )
      {
        bit ^= ( 1 << PlayData::FREE_SPACE_POCKET );
      }
      else
      {
        bit ^= ( 1 << GetFieldPocketNum(m_pItemManager->GetParam(item_id,item::ITEM_DATA::PRM_ID_F_POCKET)) );
      }
    }
    m_pDraw->SetPocketButtonActive( bit, false );
  }
//  m_pDraw->ChangePkeItemCatchMode();  // ChangeModeItemSelect()で戻る
  m_pDraw->SetInputEnabled( false );    // @fix niji NMCat[98]: 入力できないようにする
  m_pDraw->SetPaneListEnable( false );  // @fix niji NMCat[98]: 入力できないようにする
  m_listUpdateLock = true;
  m_pokeItemCatchPos = m_pBagPokemon->GetSelectPos();   // @fix NMCat[3917]: 取得位置を記憶
  m_subSeq = 0;
  SetSubSeq( SEQ_POKE_ITEM_MOVE );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPokemonItem
 * @brief   取得中の道具をポケモンに持たせる
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
void BagFrame::PutPokemonItem(void)
{
  if( m_pDraw->GetItemPutPosID() == BagDraw::PUT_ID_SCROLL )
  {
    m_pDraw->ChangeScrollPutData();
  }

#if PM_DEBUG
  // NMCat[3917]の確認
  if( m_pBagPokemon->GetSelectPos() == pml::PokeParty::MAX_MEMBERS )
  {
    GFL_RELEASE_PRINT( "★NMCat[3917]の発生条件になりました\n" );
  }
#endif  // PM_DEBUG

  u32 get_pos = m_pokeItemCatchPos;   // @fix NMCat[3917]: キャッチムーブ開始時の位置

  const pml::pokepara::PokemonParam * get_pp = m_appParam->pokemon->GetPokeConst( get_pos );

  switch( m_pDraw->GetItemPutPosID() )
  {
  case BagDraw::PUT_ID_CANCEL:
    {
      gfl2::math::VEC3 pos = m_pBagPokemon->GetItemIconPos( get_pos );
      m_pDraw->EndItemCatch( &pos );
    }
    break;

  case BagDraw::PUT_ID_PLATE:
    {
      gfl2::math::VEC3 pos;
      u32 put_pos = m_pDraw->GetItemPutIndex();
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( put_pos );
      // タマゴ
      if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
      {
        pos = m_pBagPokemon->GetItemIconPos( get_pos );
      }
      else
      {
        pos = m_pBagPokemon->GetItemIconPos( put_pos );
      }
      m_pDraw->EndItemCatch( &pos );
    }
    break;

  case BagDraw::PUT_ID_LIST:
    {
      u32 item_id = get_pp->GetItem();
      // ピース
      if( item::ITEM_CheckPiece(item_id) != false )
      {
        m_pDraw->EndItemCatch( NULL );
        m_pDraw->ChangePutDataDelete();
      }
      else
      {
        // バッグに戻せない
        if( m_myItem->AddCheck(item_id,1) == false )
        {
          gfl2::math::VEC3 pos = m_pBagPokemon->GetItemIconPos( get_pos );
          m_pDraw->EndItemCatch( &pos );
        }
        else
        {
          u32 pocket_num;
          if( m_myItem->IsRegisterFreeSpace(item_id) != false )
          {
            pocket_num = PlayData::FREE_SPACE_POCKET;
          }
          else
          {
            pocket_num = GetFieldPocketNum( m_pItemManager->GetParam(item_id,item::ITEM_DATA::PRM_ID_F_POCKET) );
          }
          if( static_cast<u32>(m_pocket) == pocket_num )
          {
            m_pDraw->EndItemCatch( NULL );
          }
          else
          {
            gfl2::math::VEC3 pos = m_pDraw->GetPocketIconPos( pocket_num );
            m_pDraw->EndItemCatch( &pos );
          }
        }
      }
    }
    break;

  case BagDraw::PUT_ID_POCKET:
    {
      u32 item_id = get_pp->GetItem();
      // ピース
      if( item::ITEM_CheckPiece(item_id) != false )
      {
        m_pDraw->EndItemCatch( NULL );
        m_pDraw->ChangePutDataDelete();
      }
      else
      {
        gfl2::math::VEC3 pos;
        // バッグに戻せない
        if( m_myItem->AddCheck(item_id,1) == false )
        {
          pos = m_pBagPokemon->GetItemIconPos( get_pos );
        }
        // フリースペースへ
        else if( m_myItem->IsRegisterFreeSpace(item_id) != false )
        {
          pos = m_pDraw->GetPocketIconPos( PlayData::FREE_SPACE_POCKET );
        }
        // 通常のポケットへ
        else
        {
          u32 pocket_id = m_pItemManager->GetParam( item_id, item::ITEM_DATA::PRM_ID_F_POCKET );
          pos = m_pDraw->GetPocketIconPos( GetFieldPocketNum(pocket_id) );
        }
        m_pDraw->EndItemCatch( &pos );
      }
    }
    break;
  }

  ChangeModeItemCatchMove();
  m_subSeq++;
}

/*
void BagFrame::ScrollItemListEx( int mv )
{
  if( m_isExScroll == false )
  {
    if( m_pPaneListView->GetPaneList()->InitExScroll( mv ) != false )
    {
      m_isExScroll = true;
    }
  }
  m_exScrollVec = mv;
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    PokeItemMoveSub
 * @brief   道具キャッチムーブ処理
 * @date    2015.05.08
 */
//-----------------------------------------------------------------------------
void BagFrame::PokeItemMoveSub(void)
{
  if( m_isExScroll != false )
  {
    app::tool::PaneList * list = m_pPaneListView->GetPaneList();
    if( list->MainExScroll() == false )
    {
      m_isExScroll = false;
      if( m_isCatchItemPut != false )
      {
        return;
      }
    }
  }

  if( m_subSeq >= 2 )
  {
    return;
  }

  switch( m_pDraw->GetItemPutPosID() )
  {
  case BagDraw::PUT_ID_SCROLL:
    {
      int mv = m_pDraw->GetItemPutIndex();
      if( m_isExScroll == false )
      {
        if( m_pPaneListView->GetPaneList()->InitExScroll( mv ) != false )
        {
          m_isExScroll = true;
        }
      }
      m_exScrollVec = mv;
    }
    m_catchMovePocket = PlayData::POCKET_DATA_MAX;
    m_catchMovePocketCount = 0;
    break;

  case BagDraw::PUT_ID_POCKET:
    if( m_isExScroll == false )
    {
      int pocket = m_pDraw->GetItemPutIndex();
      if( m_pDraw->IsPocketButtonActive( pocket ) != false &&
          pocket == m_catchMovePocket &&
          pocket != m_pocket )
      {
        m_catchMovePocketCount++;
        if( m_catchMovePocketCount >= app::util::DEFAULT_GET_COUNT_30F )
        {
          ChangePocketCatch( m_catchMovePocket );
          Sound::PlaySE( SEQ_SE_PAGE2 );
          m_catchMovePocketCount = 0;
        }
      }
      else
      {
        m_catchMovePocket = pocket;
        m_catchMovePocketCount = 0;
      }
    }
    else
    {
      m_catchMovePocket = PlayData::POCKET_DATA_MAX;
      m_catchMovePocketCount = 0;
    }
    m_exScrollVec = 0;
    break;

  case BagDraw::PUT_ID_CANCEL:
  case BagDraw::PUT_ID_PLATE:
  case BagDraw::PUT_ID_LIST:
    m_exScrollVec = 0;
    m_catchMovePocket = PlayData::POCKET_DATA_MAX;
    m_catchMovePocketCount = 0;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetPocketItemTable
 * @brief   指定アイテムのポケットのテーブルを再設定
 * @date    2015.05.08
 *
 * @param   item  アイテムID
 */
//-----------------------------------------------------------------------------
void BagFrame::ResetPocketItemTable( u32 item )
{
  if( item::ITEM_CheckPiece(item) != false )
  {
    return;
  }
  // @note 安全のため、保存場所とフリースペースの両方を再取得する
  // 元の保存場所を再取得
  u32 save_pocket = m_pItemManager->GetParam( item, item::ITEM_DATA::PRM_ID_F_POCKET );
  u32 bag_pocket  = GetFieldPocketNum( save_pocket );
  u32 max;
  m_myItem->GetPocketItem( save_pocket, &max );
  m_itemTableMax[bag_pocket] = m_myItem->MakePocketItemTable( save_pocket, m_itemTable[bag_pocket], max );
  SetCountNew( bag_pocket );
  // フリーポケットを再取得
  m_itemTableMax[PlayData::FREE_SPACE_POCKET] = m_myItem->MakeFreeSpaceItemTable( m_itemTable[PlayData::FREE_SPACE_POCKET], Savedata::MyItem::TOTAL_ITEM_MAX );
  SetCountNew( PlayData::FREE_SPACE_POCKET );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutListItem
 * @brief   リストからキャッチした道具の配置
 * @date    2015.05.12
 *
 * @retval  true  = 次の処理へ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::PutListItem(void)
{
  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    return PutListItemNormal();
  }
  PutListItemBattle();
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutListItemNormal
 * @brief   リストからキャッチした道具の配置（通常）
 * @date    2015.05.13
 *
 * @retval  true  = 次の処理へ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::PutListItemNormal(void)
{
  if( m_pDraw->GetItemPutPosID() == BagDraw::PUT_ID_SCROLL )
  {
    m_pDraw->ChangeScrollPutData();
  }

  Savedata::MyItem::ITEM_ST * item = GetCatchListItem();

  switch( m_pDraw->GetItemPutPosID() )
  {
  case BagDraw::PUT_ID_CANCEL:
    ResetCatchMoveItemList();
    {
      gfl2::math::VEC3 pos = m_pDraw->GetListItemPos( m_listCatchPos );
      m_pDraw->EndItemCatch( &pos );
    }
    break;

  case BagDraw::PUT_ID_PLATE:
    ResetCatchMoveItemList();
    {
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( m_pDraw->GetItemPutIndex() );
      gfl2::math::VEC3 pos;
      // タマゴ
      if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
      {
        pos = m_pDraw->GetListItemPos( m_listCatchPos );
      }
      else
      {
        u32 cnt = MakeCatchMoveMenuTable( item->id );
        //「やめる」のみ
        if( cnt == 1 )
        {
          pos = m_pDraw->GetListItemPos( m_listCatchPos );
        }
        //「つかう」「もたせる」あり
        else if( cnt == 3 )
        {
          if( m_pDraw->ReleaseItemIcon() != false )
          {
            GFL_ASSERT( 0 );
          }

          m_pDraw->ResetCatchItemChangeModeFlag();  // フラグだけ下げる
          m_pDraw->SetPocketButtonActive( 0xffffffff, true );

          m_pDraw->PutMessage( BagDraw::MSG_ID_ITEM_CATCH_MENU, item, m_appParam->pokemon->GetPokeConst(m_pDraw->GetItemPutIndex()) );
          StartMenuWindow( cnt );

          m_listUpdateLock = false;
          m_subSeq = 0;
          SetSubSeq( SEQ_MAIN );
          return true;
        }
        else
        {
          //「つかう」
          if( m_menuTable[0] == MENU_ID_CATCH_USE )
          {
            // 使用可
            if( CheckCatchItemUse() != false )
            {
              pos = m_pBagPokemon->GetItemIconPos( m_pDraw->GetItemPutIndex() );
            }
            // 使用不可
            else
            {
              pos = m_pDraw->GetListItemPos( m_listCatchPos );
            }
          }
          //「もたせる」
          else
          {
            u32 poke_item = pp->GetItem();
            // 持ち物を持っている場合は交換へ
            if( poke_item != ITEM_DUMMY_DATA )
            {
              if( m_pDraw->ReleaseItemIcon() != false )
              {
                GFL_ASSERT( 0 );
              }

              m_pDraw->SetCatchItemChangeMode( false );
              m_pDraw->SetPocketButtonActive( 0xffffffff, true );

              m_pDraw->SetDispActive( false );
              ChangeModeMessage();
              m_pDraw->PrintMessage( BagDraw::MSG_ID_CHANGE_CHECK, poke_item, ITEM_DUMMY_DATA, pp );

              m_listUpdateLock = false;
              m_subSeq = 2;
              SetSubSeq( SEQ_POKE_SET );
              return true;
            }
            else
            {
              pos = m_pBagPokemon->GetItemIconPos( m_pDraw->GetItemPutIndex() );
            }
          }
        }
      }
      m_pDraw->EndItemCatch( &pos );
    }
    break;

  case BagDraw::PUT_ID_LIST:
    m_pDraw->EndItemCatch( NULL );
    break;

  case BagDraw::PUT_ID_POCKET:
    {
      gfl2::math::VEC3 pos;
      u32 put = m_pDraw->GetItemPutIndex();
      if( put == m_listCatchPocket || m_pDraw->IsPocketButtonActive( put ) == false )
      {
        ResetCatchMoveItemList();
        pos = m_pDraw->GetListItemPos( m_listCatchPos );
      }
      else
      {
        pos = m_pDraw->GetPocketIconPos( put );
      }
      m_pDraw->EndItemCatch( &pos );
    }
    break;
  }

  ChangeModeItemCatchMove();
  m_subSeq++;

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutListItemBattle
 * @brief   リストからキャッチした道具の配置（バトル用）
 * @date    2015.05.13
 *
 * @retval  true  = 次の処理へ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
void BagFrame::PutListItemBattle(void)
{
  Savedata::MyItem::ITEM_ST * item = GetCatchListItem();
  gfl2::math::VEC3 pos;

  switch( m_pDraw->GetItemPutPosID() )
  {
  case BagDraw::PUT_ID_CANCEL:
  case BagDraw::PUT_ID_LIST:
  case BagDraw::PUT_ID_POCKET:
  case BagDraw::PUT_ID_SCROLL:
    pos = m_pDraw->GetListItemPos( m_listCatchPos );
    break;

  case BagDraw::PUT_ID_PLATE:
    {
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPokeConst( m_pDraw->GetItemPutIndex() );
      // タマゴ
      if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
      {
        pos = m_pDraw->GetListItemPos( m_listCatchPos );
        m_pDraw->ChangePutDataCancel();
      }
      else
      {
        u32 pocket = m_pItemManager->GetParam( item->id, item::ITEM_DATA::PRM_ID_B_POCKET );
        // 使用できない道具
        // @fix MMCat[345]: ロトムパワーポケットの道具はキャンセルになるようにした
        if( ( pocket & item::GetBattlePocketID(item::BTLPOCKET_ROTOPON) ) != 0 ||
            ( pocket & (item::GetBattlePocketID(item::BTLPOCKET_HP)|item::GetBattlePocketID(item::BTLPOCKET_STATUS)) ) == 0 )
        {
          pos = m_pDraw->GetListItemPos( m_listCatchPos );
          m_pDraw->ChangePutDataCancel();
        }
        // さしおさえ
        // @fix MMCat[338]: ダブルバトルがあるのでビット管理に変更
        else if( ( m_appParam->sasiosae_index & (1<<m_pDraw->GetItemPutIndex()) ) != 0 )
        {
          pos = m_pDraw->GetListItemPos( m_listCatchPos );
          m_pDraw->ChangePutDataCancel();
          m_isSasiosaeErr = true;
        }
        else
        {
          pos = m_pBagPokemon->GetItemIconPos( m_pDraw->GetItemPutIndex() );
        }
      }
    }
    break;
  }

  m_pDraw->EndItemCatch( &pos );
  ChangeModeItemCatchMove();
  m_subSeq++;
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetCatchMoveItemList
 * @brief   リストからキャッチした道具の配置後のリスト再設定
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagFrame::ResetCatchMoveItemList(void)
{
  if( static_cast<u32>(m_pocket) != m_listCatchPocket )
  {
    SetPocketData( m_pocket );
    m_pDraw->ChangePocket( m_pocket, m_listCatchPocket, m_itemTable[m_listCatchPocket], m_itemTableMax[m_listCatchPocket], m_listCatchPos, m_listCatchScroll );
    m_pDraw->SetPaneListEnable( false );  // リストが再生成されるので、止める
    m_pocket = m_listCatchPocket;
  }
  else
  {
    app::tool::PaneList * list = m_pPaneListView->GetPaneList();
    u32 pos;
    f32 scroll;
    list->GetCursorData( &pos, &scroll );
    if( pos != m_listCatchPos || scroll != m_listCatchScroll )
    {
      list->SetCursorData( m_listCatchPos, m_listCatchScroll );
      list->InitListPut();
      m_pDraw->SetPaneListEnable( false );  // リストが更新されるので、止める
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckCatchItemUse
 * @brief   キャッチした道具の使用チェック
 * @date    2015.05.12
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckCatchItemUse(void)
{
  Savedata::MyItem::ITEM_ST * item = GetCatchListItem();

  pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_pDraw->GetItemPutIndex() );

  { // 全員瀕死回復
    AllDeathRcvID rcv_id = CheckAllDeathRecoverItemUse( item->id );
    if( rcv_id == ALL_DEATH_RECOVER_USE_TRUE )
    {
      return true;
    }
    else if( rcv_id == ALL_DEATH_RECOVER_USE_FALSE )
    {
      return false;
    }
  }

  // 技マシン
  if( item::ITEM_CheckWazaMachine(item->id) != false )
  {
    WazaNo waza = static_cast<WazaNo>( item::ITEM_GetWazaNo(item->id) );
    // 覚えている
    if( pp->HaveWaza(waza) != false )
    {
      return false;
    }
    // 覚えられない
    else if( pp->CheckWazaMachine( item::ITEM_GetWazaMashineNo(item->id) ) == false )
    {
      return false;
    }
    return true;
  }
  // 進化アイテム
  else if( CheckItemUseEvolution(item->id,pp) != false  )
  {
    return true;
  }
  // 特性カプセル
  else if( CheckItemUseTokusei(item->id) != false )
  {
    // 特性２がない
    pml::personal::LoadPersonalData( pp->GetMonsNo(), pp->GetFormNo() );
    if( pml::personal::GetTokuseiPattern() != pml::personal::TOKUSEI_PATTERN_ABC )
    {
      return false;
    }
    // 特性３が設定されている
    u32	index = pp->GetTokuseiIndex();
    if( index == 2 )
    {
      return false;
    }
    return true;
  }
  // フォルムチェンジアイテム
  else if( CheckItemUseFormChange(item->id,pp) != false )
  {
    return true;
  }
  // ビーズ
  else if( item::ITEM_CheckBeads(item->id) != false )
  {
    return true;
  }
  // その他
  else
  {
    item::ITEM_DATA item_data;
    m_pItemManager->GetData( item->id, item_data );
    // 技選択
    if( ITEM_UTIL_IsNeedSelectSkill( &item_data ) != false )
    {
      return true;
    }
    // 使用できるか
    return PokeTool::ITEM_RCV_RecoverCheck(pp,item->id,0,m_pItemManager);
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCatchListItem
 * @brief   リストからキャッチしたアイテムのデータを取得
 * @date    2015.05.12
 *
 * @return  アイテムデータ
 */
//-----------------------------------------------------------------------------
Savedata::MyItem::ITEM_ST * BagFrame::GetCatchListItem(void)
{
  Savedata::MyItem::ITEM_ST ** table = m_itemTable[m_listCatchPocket];
  return table[static_cast<u32>(m_listCatchScroll)+m_listCatchPos];
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuCatchUse
 * @brief   キャッチムーブ用メニュー"つかう"
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuCatchUse(void)
{
  Savedata::MyItem::ITEM_ST * item = GetCatchListItem();

  // 全員瀕死回復
  AllDeathRcvID rcv_id = CheckAllDeathRecoverItemUse( item->id );
  if( rcv_id == ALL_DEATH_RECOVER_USE_TRUE )
  {
    m_pDraw->SetDispActive( true );
    SetSubSeq( SEQ_ALL_DEATH_RECOVER );
  }
  else if( rcv_id == ALL_DEATH_RECOVER_USE_FALSE )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    SetSubSeq( SEQ_MSG_WAIT );
  }
  // 道具使用
  else
  {
    PokeItemUse( m_pDraw->GetItemPutIndex(), item, true );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuCatchSet
 * @brief   キャッチムーブ用メニュー"もたせる"
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuCatchSet(void)
{
  PokeItemSet( m_pDraw->GetItemPutIndex(), true );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSubSeq_MsgWaitItemSub
 * @brief   メッセージ後、道具を減らす処理へ
 * @date    2016.03.07
 *
 * @param   item  アイテム番号
 */
//-----------------------------------------------------------------------------
void BagFrame::SetSubSeq_MsgWaitItemSub( u32 item )
{
  m_useItem = item;
  SetSubSeq( SEQ_MSG_WAIT_ITEM_SUB );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSubSeq_MsgWaitItemSubPokeSelect
 * @brief   メッセージ後、道具を減らしてポケモン選択へ
 * @date    2016.03.07
 *
 * @param   item  アイテム番号
 * @param   pos   ポケモン位置
 */
//-----------------------------------------------------------------------------
void BagFrame::SetSubSeq_MsgWaitItemSubPokeSelect( u32 item, u32 pos )
{
  m_useItem = item;
  m_rcvPokeSelectPos = pos;
  SetSubSeq( SEQ_MSG_WAIT_ITEM_SUB_POKE_SELECT );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSubSeq_MsgWaitPokeSelect
 * @brief   メッセージ後、ポケモン選択へ
 * @date    2015.05.13
 *
 * @param   pos   ポケモン位置
 */
//-----------------------------------------------------------------------------
void BagFrame::SetSubSeq_MsgWaitPokeSelect( u32 pos )
{
  m_rcvPokeSelectPos = pos;
  SetSubSeq( SEQ_MSG_WAIT_POKE_SELECT );
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSingleUse
 * @brief   単体モード用メニュー"つかう"
 * @date    2015.05.15
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSingleUse(void)
{
  Savedata::MyItem::ITEM_ST * item = GetListPosItem();

  // 全員瀕死回復
  AllDeathRcvID rcv_id = CheckAllDeathRecoverItemUse( item->id );
  if( rcv_id == ALL_DEATH_RECOVER_USE_TRUE )
  {
    m_pDraw->SetDispActive( true );
    SetSubSeq( SEQ_ALL_DEATH_RECOVER );
  }
  else if( rcv_id == ALL_DEATH_RECOVER_USE_FALSE )
  {
    m_pDraw->SetDispActive( false );
    m_pDraw->PrintMessage( BagDraw::MSG_ID_USE_ERROR_RCV );
    SetSubSeq( SEQ_MSG_WAIT );
  }
  // 道具使用
  else
  {
    PokeItemUse( m_pBagPokemon->GetSelectPos(), item, true );   // キャッチはtrueにしておく
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MenuSingleSet
 * @brief   単体モード用メニュー"もたせる"
 * @date    2015.05.15
 */
//-----------------------------------------------------------------------------
void BagFrame::MenuSingleSet(void)
{
  PokeItemSet( m_pBagPokemon->GetSelectPos(), true );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPrice
 * @brief   売値取得
 * @date    2015.07.17
 *
 * @param   item  アイテムデータ
 *
 * @return  売値
 */
//-----------------------------------------------------------------------------
u32 BagFrame::GetPrice( const Savedata::MyItem::ITEM_ST * item )
{
  if( item == NULL )
  {
    return 0;
  }

  if( m_pItemManager->GetParam( item->id, item::ITEM_DATA::PRM_ID_IMP ) != 0 )
  {
    return 0;
  }

  u32 pocket = m_pItemManager->GetParam( item->id, item::ITEM_DATA::PRM_ID_F_POCKET );
  if( pocket == BAG_POCKET_WAZA || pocket == BAG_POCKET_EVENT )
  {
    return 0;
  }

  return m_pItemManager->GetParam( item->id, item::ITEM_DATA::PRM_ID_PRICE ) / 2;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectSaleItem
 * @brief   売却アイテム選択
 * @date    2015.07.17
 *
 * @param   item  アイテムデータ
 */
//-----------------------------------------------------------------------------
void BagFrame::SelectSaleItem( Savedata::MyItem::ITEM_ST * item )
{
  m_pDraw->SetDispActive( false );
  ChangeModeMessage();

  u32 price = GetPrice( item );

  // 売れない
  if( price == 0 )
  {
    m_pDraw->PrintSaleMessage( BagDraw::MSG_ID_SALE_NOT, item->id );
    SetSubSeq( SEQ_MSG_WAIT );
  }
  // 所持数１
  else if( item->num == 1 )
  {
    m_pSaleWindow->StartOnce( item, price );
    SetSubSeq( SEQ_SALE );
    m_subSeq = 4;
  }
  else
  {
    m_pDraw->PrintSaleMessage( BagDraw::MSG_ID_SALE_SELECT, item->id );
    SetSubSeq( SEQ_SALE );
    m_subSeq = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsBeadsPocket
 * @brief   ビーズポケットを表示するか
 * @date    2016.01.13
 *
 * @retval  true  = 表示
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsBeadsPocket(void)
{
  u32 max = 0;
  const u16 * tbl = item::ITEM_GetBeadsTable( &max );

  for( u32 i=0; i<max; i++ )
  {
    if( m_myItem->Check( tbl[i], 1 ) != false )
    {
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotoponPocket
 * @brief   ロトポンポケットを表示するか
 * @date    2017.02.01
 *
 * @retval  true  = 表示
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsRotoponPocket(void)
{
  if( m_misc->IsBagRotoponItemEnable() != false )
  {
    return true;
  }

  u32 max = 0;
  const u16 * tbl = item::ITEM_GetRotoponTable( &max );

  for( u32 i=0; i<max; i++ )
  {
    if( m_myItem->Check( tbl[i], 1 ) != false )
    {
      m_misc->SetBagRotoponItemEnable();
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitDemoMode
 * @brief   デモモードにする
 * @date    2015.06.25
 */
//-----------------------------------------------------------------------------
void BagFrame::InitDemoMode(void)
{
  m_isDemoMode = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDemoModeRequest
 * @brief   デモモード用リクエスト
 * @date    2015.06.25
 */
//-----------------------------------------------------------------------------
void BagFrame::SetDemoModeRequest( DemoModeRequest req )
{
  if( m_isDemoMode == false )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_demoReq = req;

  switch( m_demoReq )
  {
  case REQ_SELECT_BALL_POCKET:
//    m_pDraw->StartSelectPocketAnime( 2 );
    Sound::PlaySE( SEQ_SE_PAGE2 );
    SetSubSeq( SEQ_DEMO_MODE );
    break;

  case REQ_SELECT_ITEM_BALL:
    m_pDraw->StartSelectItemAnime();
    Sound::PlaySE( SEQ_SE_DECIDE1 );
    SetSubSeq( SEQ_DEMO_MODE );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqDemoMode
 * @brief   デモモード用更新処理
 * @date    2015.06.25
 */
//-----------------------------------------------------------------------------
void BagFrame::Update_SeqDemoMode(void)
{
  switch( m_demoReq )
  {
  case REQ_SELECT_BALL_POCKET:
    if( m_pDraw->IsDemoAnimeEnd() != false )
    {
      SetSubSeq( SEQ_MAIN );
      ChangePocket( 3, ITEM_DUMMY_DATA, BagDraw::RESET_LIST_POS_NORMAL );
    }
    break;

  case REQ_SELECT_ITEM_BALL:
    if( m_pDraw->IsDemoAnimeEnd() != false )
    {
      SetSubSeq( SEQ_MAIN );
      StopUpdate( ITEM_MONSUTAABOORU );
    }
    break;
  }
}

/*
#if PM_DEBUG
void BagFrame::DebugUpdateDemoMode(void)
{
  static u32 cnt = 0;

  if( cnt == 0x7f )
  {
    SetDemoModeRequest( REQ_SELECT_BALL_POCKET );
  }
  else if( cnt == 0xff )
  {
    SetDemoModeRequest( REQ_SELECT_ITEM_BALL );
  }

  cnt++;
}
#endif  // PM_DEBUG
*/

//-----------------------------------------------------------------------------
/**
 * @func    CheckZigarudeCore
 * @brief   ジガルデコアを取得したかをチェック
 * @date    2016.01.21
 *
 * @param   id  コアのID
 *
 * @retval  true  = した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CheckZigarudeCore( u32 id )
{
  static const u32 tbl[] =
  {
    SYS_FLAG_ZIGARUDE_CORE1_GET,
    SYS_FLAG_ZIGARUDE_CORE2_GET,
    SYS_FLAG_ZIGARUDE_CORE3_GET,
    SYS_FLAG_ZIGARUDE_CORE4_GET,
    SYS_FLAG_ZIGARUDE_CORE5_GET,
  };

  return m_eventWork->CheckEventFlag( tbl[id] );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateEffect
 * @brief   エフェクト管理クラス生成
 * @date    2016.01.19
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::CreateEffect(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU || m_appParam->call_mode == CALL_SHOP )
  {
    m_pItemEffect = NULL;
    return true;
  }

  switch( m_subSeq )
  {
  case 0:
    m_pItemEffect = GFL_NEW(m_heap->GetSystemHeap()) App::Tool::ItemEffect( m_heap, m_renderingManager, app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pItemEffect->Initialize( false ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteEffect
 * @brief   エフェクト管理クラス削除
 * @date    2016.01.19
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::DeleteEffect(void)
{
  if( m_pItemEffect != NULL )
  {
    if( m_pItemEffect->Terminate( false ) == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pItemEffect );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemEffect
 * @brief   道具関連のエフェクトを生成
 * @date    2016.01.19
 *
 * @param   poke_pos    ポケモン位置
 * @param   eff_index   エフェクトのインデックス
 * @param   type        エフェクトタイプ
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateItemEffect( u32 poke_pos, u32 eff_index, App::Tool::ItemEffect::Type type )
{
  gfl2::math::VEC3 pos = m_pBagPokemon->GetPokeIconPos( poke_pos );
  m_pItemEffect->Create( eff_index, type, &pos, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFormChangeEffect
 * @brief   フォルムチェンジ関連のエフェクトを生成
 * @date    2016.02.01
 *
 * @param   pp          ポケモンデータ
 * @param   item        アイテム番号
 * @param   poke_pos    ポケモン位置
 * @param   eff_index   エフェクトのインデックス
 */
//-----------------------------------------------------------------------------
void BagFrame::CreateFormChangeEffect( const pml::pokepara::PokemonParam * pp, u32 item, u32 poke_pos, u32 eff_index )
{
  App::Tool::ItemEffect::Type type = m_pItemEffect->GetFormChangeEffectType( pp->GetMonsNo(), pp->GetFormNo(), item );
  CreateItemEffect( poke_pos, eff_index, type );
}


//-----------------------------------------------------------------------------
/**
 * @func    GetUnionWaza
 * @brief   合体/分離時に変化させる技を取得
 * @date    2017.01.19
 *
 * @param   item  アイテム番号
 *
 * @return  アイテムに対応した技
 */
//-----------------------------------------------------------------------------
WazaNo BagFrame::GetUnionWaza( u16 item )
{
  struct DATA_TABLE
  {
    u32 item;
    WazaNo waza;
  };

  static const DATA_TABLE tbl[] =
  {
    { ITEM_SORUPURASU, WAZANO_METEODORAIBU },
    { ITEM_RUNAPURASU, WAZANO_SYADOOREI },
    { ITEM_SORUPURASU_1, WAZANO_METEODORAIBU },
    { ITEM_RUNAPURASU_1, WAZANO_SYADOOREI },
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( item == tbl[i].item )
    {
      return tbl[i].waza;
    }
  }

  GFL_ASSERT( 0 );
  return WAZANO_NULL;
}

//-----------------------------------------------------------------------------
/**
 * @func    AddUnionWaza
 * @brief   合体時の技追加
 * @date    2017.01.19
 *
 * @param   pp    ポケモンデータ
 * @param   item  アイテム番号
 *
 * @retval  true  = 追加した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::AddUnionWaza( pml::pokepara::PokemonParam * pp, u16 item )
{
  WazaNo waza = GetUnionWaza( item );

  if( waza == WAZANO_NULL )
  {
    GFL_ASSERT( 0 );
    return true;
  }

  switch( pp->AddWazaIfEmptyExist(waza) )
  {
  case pml::pokepara::WAZA_LEARNING_FAILED_SAME:  // すでに覚えているので変化なし
    GFL_ASSERT( 0 );
    /* FALL THROUGH */
  case pml::pokepara::WAZA_LEARNING_SUCCEEDED:    // 新しい技を覚えた
    return true;
  case pml::pokepara::WAZA_LEARNING_FAILED_FULL:  // 手持ち技がいっぱいで覚えられなかった
    return false;
  }

  GFL_ASSERT( 0 );  // こないはず
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SubUnionWaza
 * @brief   分離時の技削除
 * @date    2017.01.19
 *
 * @param   pp    ポケモンデータ
 * @param   item  アイテム番号
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
BagFrame::SubUnionWazaResult BagFrame::SubUnionWaza( pml::pokepara::PokemonParam * pp, u16 item )
{
  WazaNo sub_waza = GetUnionWaza( item );

  if( sub_waza == WAZANO_NULL )
  {
    GFL_ASSERT( 0 );
    return SUB_UNION_WAZA_NONE;
  }

  u32 waza_pos = pml::MAX_WAZA_NUM;
  u32 waza_count = 0;

  for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    WazaNo waza = pp->GetWazaNo( i );
    if( waza != WAZANO_NULL )
    {
      if( waza == sub_waza )
      {
        waza_pos = i;
      }
      waza_count++;
    }
  }

  // 覚えていない
  if( waza_pos == pml::MAX_WAZA_NUM )
  {
    return SUB_UNION_WAZA_NONE;
  }

  // 技が１つのみ
  if( waza_count <= 1 )
  {
    pp->SetWaza( waza_pos, WAZANO_NENRIKI );
    return SUB_UNION_WAZA_NENRIKI;
  }

  pp->RemoveWaza( waza_pos );   // 技削除
  pp->CloseUpWazaPos();         // 空きを詰める

  return SUB_UNION_WAZA_DELETE;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsNekurozumaCombineItem
 * @brief   ネクロズマ用の合体アイテムか
 * @date    2017.01.19
 *
 * @param   item  アイテム番号
 *
 * @retval  true  = 合体アイテム
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsNekurozumaCombineItem( u16 item )
{
  static const u16 tbl[] =
  {
    ITEM_SORUPURASU,
    ITEM_RUNAPURASU,
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( item == tbl[i] )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsNekurozumaSeparateItem
 * @brief   ネクロズマ用の分離アイテムか
 * @date    2017.01.19
 *
 * @param   item  アイテム番号
 *
 * @retval  true  = 分離アイテム
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagFrame::IsNekurozumaSeparateItem( u16 item )
{
  static const u16 tbl[] =
  {
    ITEM_SORUPURASU_1,
    ITEM_RUNAPURASU_1,
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( item == tbl[i] )
    {
      return true;
    }
  }
  return false;
}



/*
void BagFrame::InitPokeIcon(void)
{
  if( m_pBagPokemon != NULL )
  {
    m_pBagPokemon->InitPokeIcon();
  }
}
*/


#if PM_DEBUG
void BagFrame::DebugPrintTime( u32 id )
{
#if defined(GF_PLATFORM_CTR)
  static u32 total = 0;
  static nn::os::Tick start;

  if( id == DEBUG_PRINT_TIME_ID_INIT )
  {
    total = 0;
  }
  else
  {
    nn::os::Tick now = nn::os::Tick::GetSystemCurrent();
    nn::fnd::TimeSpan ss = start.ToTimeSpan();
    nn::fnd::TimeSpan es = now.ToTimeSpan();
    u32 sec = es.GetMilliSeconds() - ss.GetMilliSeconds();
    total += sec;
    GFL_PRINT( "<mil>[%03d]: %04d total[%04d]\n", id, sec, total );
  }

  start = nn::os::Tick::GetSystemCurrent();
#endif
}
#endif


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)
