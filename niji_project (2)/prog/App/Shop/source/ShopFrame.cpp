//=============================================================================
/**
 * @file    ShopFrame.cpp
 * @brief   ショップ購入画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================

// module
#include "App/Shop/include/ShopAppParam.h"
#include "App/Shop/include/ShopFrame.h"
#include "ShopDraw.h"
// niji
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/TalkWindow.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/app_tool_MenuWindow.h"
#include "Print/include/ItemMessage.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/ShopGraphic.gaix"
#include "arc_index/message.gaix"
#include "arc_index/fieldwindow.gaix"
#include "niji_conv_header/message/msg_shop.h"

// ロトムパワー
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)

//! メインシーケンス
enum MainSeq
{
  SEQ_START = 0,      //!< 開始
  SEQ_MAIN,           //!< メイン
  SEQ_NUM_SELECT,     //!< 個数選択
  SEQ_MESSAGE,        //!< メッセージ表示
  SEQ_END             //!< 終了
};

//! 個数選択のシーケンス
enum
{
  UPDATE_SEQ_NUM_SELECT_IN = 0,
  UPDATE_SEQ_NUM_SELECT_IN_WAIT,
  UPDATE_SEQ_NUM_SELECT_MAIN,
  UPDATE_SEQ_NUM_SELECT_DECIDE,
  UPDATE_SEQ_NUM_SELECT_CANCEL,
};

//! ショップデータ
#include "niji_conv_header/app/data/shop/shopdata.cdat"
#include "niji_conv_header/app/data/shop/bpshopdata.cdat"
#include "niji_conv_header/app/data/shop/bpwazaoboeshopdata.cdat"


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
ShopFrame::ShopFrame(void)
  : m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_saveMisc( NULL )
  , m_myItem( NULL )
  , m_saveJoinFesta( NULL )
  , m_eventWork( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pWordSetLoader( NULL )
  , m_pItemManager( NULL )
  , m_pItemMsgBuff( NULL )
  , m_pItemIcon( NULL )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_pDraw( NULL )
	, m_pMenuWindow( NULL )
  , m_pTalkWindow( NULL )
  , m_pTalkWindowBuff( NULL )
  , m_pShopItem( NULL )
  , m_itemMax( 0 )
  , m_myGold( 0 )
  , m_cashBack( 0 )
  , m_cursorPos( 0 )
  , m_cursorScroll( 0 )
  , m_cursorScrollMax( 0 )
  , m_selectPos( 0 )
  , m_selectNum( 0 )
  , m_selectMax( 0 )
  , m_msgID( 0 )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ShopFrame::~ShopFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopFrame::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_saveMisc      = gd->GetMisc();
  m_myItem        = gd->GetMyItem();
  m_saveJoinFesta = gd->GetJoinFestaData();
  m_eventWork     = gd->GetEventWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.10.02
 *
 * @param   param       外部設定パラメータ
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void ShopFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_appParam = param;
  m_heap = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.10.02
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ShopFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    GetGameData();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( CreateItemManager() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 2:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 3:
    if( LoadMessage() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 4:
    if( CreateWordSetLoader() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 5:
    if( CreateItemIcon() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 6:
    if( CreateMenuCursor() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 7:
    CreateShopTypeData();
    InitListData();
    CreateDraw();
    AddSubSeq();
    /* FALL THROUGH */
  case 8:
    if( CreateTalkWindow() == false )
    {
      break;
    }
    CreateMenuWindow();
    return applib::frame::RES_FINISH;

  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.10.02
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ShopFrame::EndFunc(void)
{
  if( DeleteMenuWindow() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteTalkWindow() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteMenuCursor() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteItemIcon() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteItemManager() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  SetEndParam();  // リストを消す前にセットする

  DeleteShopTypeData();
  DeleteWordSetLoader();
  UnloadMessage();
  UnloadLayoutResource();

  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.10.02
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ShopFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    if( m_pMenuWindow->IsCreate() == false )
    {
      break;
    }
    if( m_pDraw->Start() != false )
    {
      m_pMenuWindow->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );
      ChangeSequence( SEQ_MAIN );
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

  case SEQ_NUM_SELECT:
    Update_SeqNumSelect();
    break;

  case SEQ_MESSAGE:
    Update_SeqMessage();
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
 * @date    2015.10.02
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void ShopFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pDraw != NULL )
  {
    m_pDraw->DrawTree( no );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateShopTypeData
 * @brief   ショップタイプごとのデータを生成
 * @date    2015.10.08
 */
//-----------------------------------------------------------------------------
void ShopFrame::CreateShopTypeData(void)
{
  switch( m_appParam->type )
  {
  case TYPE_FR_SHOP:
    m_itemMax = shop_itemnum_table[m_appParam->id];
    m_pShopItem = GFL_NEW_ARRAY(m_heap->GetSystemHeap()) SHOP_ITEM[m_itemMax];
    {
      const u16 * table = shop_data_table[m_appParam->id];
      for( u32 i=0; i<m_itemMax; i++ )
      {
        m_pShopItem[i].id    = table[i];
        m_pShopItem[i].price = m_pItemManager->GetParam( m_pShopItem[i].id, item::ITEM_DATA::PRM_ID_PRICE );
        m_pShopItem[i].price = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcSale(m_pShopItem[i].price);
      }
    }
    m_myGold = m_saveMisc->GetGold();
    break;

  case TYPE_BP_SHOP:
    m_itemMax = bp_shop_itemnum_table[m_appParam->id];
    m_pShopItem = GFL_NEW_ARRAY(m_heap->GetSystemHeap()) SHOP_ITEM[m_itemMax];
    {
      const BP_SHOP_ITEM * table = bp_shop_data_table[m_appParam->id];
      for( u32 i=0; i<m_itemMax; i++ )
      {
        m_pShopItem[i].id    = table[i].item;
        m_pShopItem[i].price = table[i].point;
      }
    }
    m_myGold = m_saveMisc->GetBP();
    break;

  /*  FesCircle  */
  case TYPE_FC_ITEM:    /*  fallthfough  */
  case TYPE_FC_ACTION:
    GFL_ASSERT(m_appParam->pShopFeatureDelegate);
    m_appParam->pShopFeatureDelegate->ShopFeatureDelegate_CreateShopItemList(&m_pShopItem, m_itemMax, m_heap->GetSystemHeap(), m_appParam->id);   /*  m_appParam->idにはShopItemInfoTable::ShopItemIDが格納されている  */

    m_myGold = m_saveJoinFesta->GetCoin();
    break;

  case TYPE_BP_WAZA:
    m_itemMax = bp_wazaoboe_shop_itemnum_table[m_appParam->id];
    m_pShopItem = GFL_NEW_ARRAY(m_heap->GetSystemHeap()) SHOP_ITEM[m_itemMax];
    {
      const BP_WAZASHOP_ITEM * table = bp_wazaoboe_shop_data_table[m_appParam->id];
      for( u32 i=0; i<m_itemMax; i++ )
      {
        m_pShopItem[i].id    = static_cast<u32>( table[i].waza );
        m_pShopItem[i].price = table[i].point;
      }
    }
    m_myGold = m_saveMisc->GetBP();
    break;

  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteShopTypeData
 * @brief   ショップタイプごとのデータを削除
 * @date    2015.10.08
 */
//-----------------------------------------------------------------------------
void ShopFrame::DeleteShopTypeData(void)
{
  GFL_SAFE_DELETE_ARRAY( m_pShopItem );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.10.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetDeviceHeap()) ShopDraw( m_appParam->type, m_heap, m_renderingManager, m_pMenuCursor );
  m_pDraw->SetupItem( m_pShopItem, m_itemMax, m_pItemManager, m_pItemIcon );
  m_pDraw->CreateItemMessage( m_pItemMsgBuff );
  m_pDraw->Init( m_pLytResBuff, m_pMsgBuff, m_pWordSetLoader, m_cursorScrollMax );
  m_pDraw->PutGold( m_myGold );
  if( m_appParam->type == TYPE_FC_ACTION || m_appParam->type == TYPE_BP_WAZA )
  {
    m_pDraw->VanishMyItemNum();
  }
  m_pDraw->SetDrawListener( this );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.10.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    m_pDraw->DeleteItemMessage();
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.10.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_SHOP_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_SHOP_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_SHOP_GRA,
      GARC_ShopGraphic_Shop_applyt_COMP,
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
    app::util::FileAccessor::FileClose( ARCID_SHOP_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_SHOP_GRA ) == false )
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
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.10.02
 */
//-----------------------------------------------------------------------------
void ShopFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.10.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::LoadMessage(void)
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
      GARC_message_shop_DAT,
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
    // フェスコイン
    if( m_appParam->type == TYPE_FC_ITEM || m_appParam->type == TYPE_FC_ACTION )
    {
      app::util::FileAccessor::FileLoad(
        arc_id,
        GARC_message_jf_shop_DAT,
        &m_pItemMsgBuff,
        m_heap->GetDeviceHeap(),
        false,
        4 );
    }
    // BP技覚え
    else if( m_appParam->type == TYPE_BP_WAZA )
    {
      app::util::FileAccessor::FileLoad(
        arc_id,
        GARC_message_wazainfo_DAT,
        &m_pItemMsgBuff,
        m_heap->GetDeviceHeap(),
        false,
        4 );
    }
    // 通常/BP
    else
    {
      app::util::FileAccessor::FileLoad(
        arc_id,
        print::ItemMessage::GetArcDatID(),
        &m_pItemMsgBuff,
        m_heap->GetDeviceHeap(),
        false,
        4 );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileLoad( &m_pItemMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:
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
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pItemMsgBuff );
  GflHeapFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateWordSetLoader
 * @brief   WordSetLoader生成
 * @date    2015.10.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CreateWordSetLoader(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->type == TYPE_FC_ACTION )
    {
      m_pWordSetLoader = NULL;
      return true;
    }
    else if( m_appParam->type == TYPE_FC_ITEM )
    {
      m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), PRELOAD_ITEMNAME );
    }
    else if( m_appParam->type == TYPE_BP_WAZA )
    {
      m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), PRELOAD_WAZANAME );
    }
    else
    {
      m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), PRELOAD_ITEMNAME|PRELOAD_WAZANAME );
    }
    m_pWordSetLoader->StartLoad( m_heap->GetDeviceHeap() );
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
 * @brief   WordSetLoader削除
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopFrame::DeleteWordSetLoader(void)
{
  GFL_SAFE_DELETE( m_pWordSetLoader );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemManager
 * @brief   アイテムマネージャ生成
 * @date    2015.10.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CreateItemManager(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->type == TYPE_FC_ACTION || m_appParam->type == TYPE_BP_WAZA )
    {
      m_pItemManager = NULL;
      return true;
    }
    m_pItemManager = GFL_NEW(m_heap->GetDeviceHeap()) itemman::ITEM_MANAGER( m_heap->GetSystemHeap(), ITEM_MANAGER_MAX, false );
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
 * @date    2015.10.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::DeleteItemManager(void)
{
  if( m_pItemManager == NULL )
  {
    return true;
  }

  switch( m_subSeq )
  {
  case 0:
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
 * @func    CreateItemIcon
 * @brief   アイテムアイコン生成
 * @date    2015.10.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CreateItemIcon(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->type == TYPE_FC_ACTION || m_appParam->type == TYPE_BP_WAZA )
    {
      m_pItemIcon = NULL;
      return true;
    }
    m_pItemIcon = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::ItemIcon( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), 1 );
    m_pItemIcon->AllocTextureBuffer( 0 );
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
 * @date    2015.10.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::DeleteItemIcon(void)
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


//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursor
 * @brief   メニューカーソル生成
 * @date    2015.10.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CreateMenuCursor(void)
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
 * @date    2015.10.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::DeleteMenuCursor(void)
{
  if( m_pMenuCursor != NULL )
  {
    if( m_pMenuCursor->IsModuleFree() == false )
    {
      return false;
    }
    m_pMenuCursor->DeleteResource();
    GFL_SAFE_DELETE( m_pMenuCursor );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuWindow
 * @brief   メニューウィンドウ作成
 * @date    2015.10.06
 */
//-----------------------------------------------------------------------------
void ShopFrame::CreateMenuWindow(void)
{
	m_pMenuWindow = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuWindow( m_heap, m_renderingManager, m_pMenuCursorBuffer, app::util::G2DUtil::SETUP_UPPER );
  {
    gfl2::str::StrBuf * str = m_pDraw->GetStrYes();
    m_pMenuWindow->AddItem( *str );
  }
  {
    gfl2::str::StrBuf * str = m_pDraw->GetStrNo();
    m_pMenuWindow->AddItem( *str, true );
  }
  m_pMenuWindow->SetListener( this );
  m_pDraw->AddSubView( m_pMenuWindow );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuWindow
 * @brief   メニューウィンドウ削除
 * @date    2015.10.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::DeleteMenuWindow(void)
{
  if( m_pMenuWindow != NULL )
  {
    if( m_pMenuWindow->IsModuleFree() == false )
    {
      return false;
    }
    m_pMenuWindow->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pMenuWindow );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateTalkWindow
 * @brief   会話ウィンドウ生成
 * @date    2015.10.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CreateTalkWindow(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_FIELDWINDOW, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( ARCID_FIELDWINDOW ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_FIELDWINDOW,
      GARC_fieldwindow_fieldwindow_applyt_COMP,
      &m_pTalkWindowBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pTalkWindowBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_FIELDWINDOW, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( ARCID_FIELDWINDOW ) == false )
    {
      break;
    }
    m_pTalkWindow = GFL_NEW(m_heap->GetSystemHeap()) App::Tool::TalkWindow( m_heap, m_renderingManager );
    m_pTalkWindow->SetInputEnabled( false );
    m_pTalkWindow->SetWindowType( App::Tool::WIN_TYPE_TALK_DOWN );
    m_pTalkWindow->Setup( m_pTalkWindowBuff );
    m_pTalkWindow->InitMsgData( m_pDraw->GetMsgData() );
    m_pDraw->AddSubView( m_pTalkWindow );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteTalkWindow
 * @brief   会話ウィンドウ削除
 * @date    2015.10.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::DeleteTalkWindow(void)
{
  if( m_pTalkWindow != NULL )
  {
    m_pTalkWindow->End();
    m_pTalkWindow->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pTalkWindow );
    GflHeapFreeMemory( m_pTalkWindowBuff );
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetEndParam
 * @brief   終了パラメータ設定
 * @date    2015.10.08
 */
//-----------------------------------------------------------------------------
void ShopFrame::SetEndParam(void)
{
  switch( m_appParam->type )
  {
  case TYPE_FR_SHOP:
    Savedata::AddRecord( Savedata::Record::RECID_SHOPPING_MONEY, m_saveMisc->GetGold()-m_myGold );  // レコード
    m_saveMisc->SetGold( m_myGold );
    break;

  case TYPE_BP_SHOP:
    Savedata::AddRecord( Savedata::Record::RECID_USE_BP, m_saveMisc->GetBP()-m_myGold );  // レコード
    m_saveMisc->SetBP( m_myGold );
    break;

  case TYPE_FC_ITEM:
    m_saveJoinFesta->SubCoin( m_saveJoinFesta->GetCoin() - m_myGold );
    break;

  case TYPE_FC_ACTION:
    if( m_selectPos == m_itemMax )
    {
      m_appParam->select_action = FC_SELECT_ACTION_CANCEL;
      m_appParam->select_cost   = FC_SELECT_COST_CANCEL;
    }
    else
    {
      m_appParam->select_action = m_pShopItem[m_selectPos].item;
      m_appParam->select_cost   = m_pShopItem[m_selectPos].price;
      m_appParam->select_pos    = m_selectPos;
    }
    break;

  case TYPE_BP_WAZA:
    if( m_selectPos == m_itemMax )
    {
      m_appParam->select_action = BP_WAZA_SELECT_ACTION_CANCEL;
      m_appParam->select_cost   = BP_WAZA_SELECT_COST_CANCEL;
    }
    else
    {
      m_appParam->select_action = m_pShopItem[m_selectPos].id;
      m_appParam->select_cost   = m_pShopItem[m_selectPos].price;
      m_appParam->select_pos    = m_selectPos;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListData
 * @brief   リストデータ初期化
 * @date    2015.10.06
 */
//-----------------------------------------------------------------------------
void ShopFrame::InitListData(void)
{
  m_cursorPos = 0;
  m_cursorScroll = 0;

  if( m_itemMax < ShopDraw::ITEM_PUT_MAX )
  {
    m_cursorScrollMax = 0;
  }
  else
  {
    m_cursorScrollMax = m_itemMax - ShopDraw::ITEM_PUT_MAX;
    m_cursorScrollMax += 1;   // "やめる"
  }

}


//-----------------------------------------------------------------------------
/**
 * @func    MoveCursor
 * @brief   カーソル移動
 * @date    2015.10.06
 *
 * @param   mv      移動値
 * @param   is_trg  true = トリガー入力
 */
//-----------------------------------------------------------------------------
void ShopFrame::MoveCursor( s32 mv, bool is_trg )
{
  s16 pos    = m_cursorPos;
  s16 scroll = m_cursorScroll;

  s16 pos_max = ShopDraw::ITEM_PUT_MAX - 1;
  if( pos_max > m_itemMax )
  {
    pos_max = m_itemMax;
  }

  // ページ送り
  if( gfl2::math::Abs(mv) == ShopDraw::ITEM_PUT_MAX )
  {
    if( mv > 0 && scroll == m_cursorScrollMax )
    {
      if( pos == pos_max && is_trg != false )
      {
        pos = 0;
        scroll = 0;
      }
      else
      {
        pos = pos_max;
      }
    }
    else if( mv < 0 && scroll == 0 )
    {
      if( pos == 0 && is_trg != false )
      {
        pos = pos_max;
        scroll = m_cursorScrollMax;
      }
      else
      {
        pos = 0;
      }
    }
    else
    {
      scroll += mv;
      if( scroll < 0 )
      {
        scroll = 0;
      }
      else if( scroll > m_cursorScrollMax )
      {
        scroll = m_cursorScrollMax;
      }
    }
  }
  else
  {
    if( mv > 0 )
    {
      if( pos < (ShopDraw::ITEM_PUT_MAX/2) )
      {
        if( pos == pos_max )
        {
          if( is_trg != false )
          {
            pos = 0;
          }
        }
        else
        {
          pos += mv;
        }
      }
      else if( scroll == m_cursorScrollMax )
      {
        if( pos == pos_max )
        {
          if( is_trg != false )
          {
            pos = 0;
            scroll = 0;
          }
        }
        else
        {
          pos += mv;
        }
      }
      else
      {
        scroll += mv;
      }
    }
    else
    {
      if( pos > (ShopDraw::ITEM_PUT_MAX/2) )
      {
        pos += mv;
      }
      else if( scroll == 0 )
      {
        if( pos == 0 )
        {
          if( is_trg != false )
          {
            pos = pos_max;
            scroll = m_cursorScrollMax;
          }
        }
        else
        {
          pos += mv;
        }
      }
      else
      {
        scroll += mv;
      }
    }
  }

  if( pos == m_cursorPos && scroll == m_cursorScroll )
  {
    return;
  }

  if( scroll != m_cursorScroll )
  {
    m_pDraw->PutItemList( scroll, m_cursorScrollMax );
  }
  m_pDraw->MoveCursor( pos, scroll );
  m_cursorPos    = pos;
  m_cursorScroll = scroll;

  Sound::PlaySE( SEQ_SE_SELECT0 );
}


//-----------------------------------------------------------------------------
/**
 * @func    CheckBuyError
 * @brief   購入できるかをチェック
 * @date    2015.10.07
 *
 * @retval  BUY_ERROR_NONE     = エラーなし
 * @retval  BUY_ERROR_MAX      = 持てない
 * @retval  BUY_ERROR_NO_MONEY = 所持金不足
 * @retval  BUY_ERRPR_SELECTED = 選択済み（フェスコイン用）
 */
//-----------------------------------------------------------------------------
ShopFrame::BuyErrorID ShopFrame::CheckBuyError(void)
{
  u16 item_id = m_pShopItem[m_selectPos].id;

  if( m_appParam->type == TYPE_FC_ACTION )
  {
    // １日１回
    if( m_saveJoinFesta->IsFacilitiesBuy( static_cast<u32>(m_selectPos) ) )
    {
      return BUY_ERRPR_SELECTED;
    }
    // 所持金
    if( CheckGold() == false )
    {
      return BUY_ERROR_NO_MONEY;
    }
    m_selectMax = 1;
  }
  else if( m_appParam->type == TYPE_FC_ITEM )
  {
    // １日１回 
    if( m_saveJoinFesta->IsFacilitiesBuy( static_cast<u32>(m_selectPos) ) )
    {
      return BUY_ERRPR_SELECTED;
    }
    // 所持数
    item_id = m_pShopItem[m_selectPos].item;
    if( m_myItem->AddCheck(item_id,m_pShopItem[m_selectPos].num) == false )
    {
      return BUY_ERROR_MAX;
    }
    // 所持金
    if( CheckGold() == false )
    {
      return BUY_ERROR_NO_MONEY;
    }
    m_selectMax = 1;
  }
  else if( m_appParam->type == TYPE_BP_WAZA )
  {
    // 所持金
    if( CheckGold() == false )
    {
      return BUY_ERROR_NO_MONEY;
    }
    m_selectMax = 1;
  }
  else
  {
    // 技マシン
    if( item::ITEM_CheckWazaMachine(item_id) != false )
    {
      // 所持数
      if( m_myItem->Check(item_id,1) != false )
      {
        return BUY_ERROR_MAX;
      }
      m_selectMax = 1;
      // 所持金
      if( CheckGold() == false )
      {
        return BUY_ERROR_NO_MONEY;
      }
    }
    else
    {
      // 所持数
      s32 max = m_myItem->GetItemNum( item_id );
      if( max == ITEM_HAVE_MAX )
      {
        return BUY_ERROR_MAX;
      }
      // 所持金
      if( CheckGold() == false )
      {
        return BUY_ERROR_NO_MONEY;
      }
      m_selectMax = ITEM_HAVE_MAX - max;
      if( m_selectMax > ITEM_BUY_MAX )
      {
        m_selectMax = ITEM_BUY_MAX;
      }
      u32 buy_max = m_myGold / m_pShopItem[m_selectPos].price;
      if( m_selectMax > buy_max )
      {
        m_selectMax = buy_max;
      }
    }
  }

  return BUY_ERROR_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckGold
 * @brief   お金が足りるか
 * @date    2015.10.07
 *
 * @retval  true  = 足りる
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::CheckGold(void)
{
  u32 price = m_pShopItem[m_selectPos].price * m_selectNum;

  if( price > m_myGold )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    AddPremiereBall
 * @brief   プレミアボール追加処理
 * @date    2015.10.07
 *
 * @retval  true  = 追加した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::AddPremiereBall(void)
{
  if( m_appParam->type == TYPE_FR_SHOP &&
      m_pShopItem[m_selectPos].id == ITEM_MONSUTAABOORU &&
      m_selectNum >= 10 )
  {
    if( m_myItem->Add(ITEM_PUREMIABOORU,1) != false )
    {
      Savedata::IncRecord( Savedata::Record::RECID_PREMIUM_BALL );  // レコード
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateNumSelect
 * @brief   個数選択ウィンドウ更新
 * @date    2015.10.06
 *
 * @param   mv      個数増減値
 * @param   is_trg  true = トリガー入力
 */
//-----------------------------------------------------------------------------
void ShopFrame::UpdateNumSelect( s32 mv, bool is_trg )
{
  s16 tmp = m_selectNum;

  m_selectNum += mv;

  if( m_selectNum < 1 )
  {
//    if( gfl2::math::Abs(mv) != 10 && is_trg != false )
    if( is_trg != false && tmp == 1 )
    {
      m_selectNum = m_selectMax;
    }
    else
    {
      m_selectNum = 1;
    }
  }
  else if( m_selectNum > m_selectMax )
  {
//    if( gfl2::math::Abs(mv) != 10 && is_trg != false )
    if( is_trg != false && tmp == m_selectMax )
    {
      m_selectNum = 1;
    }
    else
    {
      m_selectNum = m_selectMax;
    }
  }

  if( tmp != m_selectNum )
  {
    m_pDraw->UpdateNumCounter( m_pShopItem[m_selectPos].price, m_selectNum );
    Sound::PlaySE( SEQ_SE_SELECT0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCoupon
 * @brief   クーポンを持っているか
 * @date    2016.01.09
 *
 * @retval  true  = 持っている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopFrame::IsCoupon(void)
{
  if( m_appParam->type == TYPE_FR_SHOP && m_eventWork->CheckEventFlag(SYS_FLAG_COUPON) != false )
  {
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    ChangeSequence
 * @brief   シーケンス切り替え
 * @date    2015.10.07
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void ShopFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeMessageSequence
 * @brief   メッセージ表示シーケンスへ切り替え
 * @date    2015.10.07
 *
 * @param   id        メッセージID
 * @param   str       文字列バッファ
 * @param   is_input  true = 入力で終了
 */
//-----------------------------------------------------------------------------
void ShopFrame::ChangeMessageSequence( u32 id, gfl2::str::StrBuf * str, bool is_input )
{
  m_msgID = id;
  if( is_input != false )
  {
    m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );
  }
  else
  {
    m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_NONE );
  }
  if( str != NULL )
  {
    m_pTalkWindow->SetMessage( str );
  }
  else
  {
    m_pTalkWindow->SetMessage( m_msgID );
  }
  m_pDraw->SetInputEnabled( false );
  ChangeSequence( SEQ_MESSAGE );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqNumSelect
 * @brief   メイン処理：個数選択
 * @date    2015.10.07
 */
//-----------------------------------------------------------------------------
void ShopFrame::Update_SeqNumSelect(void)
{
  switch( m_mainSeq )
  {
  case UPDATE_SEQ_NUM_SELECT_IN:
    m_pDraw->InNumCounter( m_pShopItem[m_selectPos].price, m_selectNum );
    m_mainSeq = UPDATE_SEQ_NUM_SELECT_IN_WAIT;
    break;

  case UPDATE_SEQ_NUM_SELECT_IN_WAIT:
    if( m_pDraw->IsNumCounterAnimeEnd() != false )
    {
      m_pDraw->SetInputEnabled( true );
      m_mainSeq = UPDATE_SEQ_NUM_SELECT_MAIN;
    }
    break;

  case UPDATE_SEQ_NUM_SELECT_MAIN:
    break;

  case UPDATE_SEQ_NUM_SELECT_DECIDE:
    if( m_pDraw->IsNumCounterAnimeEnd() != false )
    {
      s32 gold = m_pShopItem[m_selectPos].price * m_selectNum;
      if( m_selectNum == 1 )
      {
        ChangeMessageSequence(
          m_appParam->really_msg,
          m_pDraw->GetMessageReally( m_appParam->really_msg, m_pShopItem[m_selectPos].id, m_selectNum, gold ),
          false );
      }
      else
      {
        ChangeMessageSequence(
          m_appParam->really_msg2,
          m_pDraw->GetMessageReally( m_appParam->really_msg2, m_pShopItem[m_selectPos].id, m_selectNum, gold ),
          false );
      }
    }
    break;

  case UPDATE_SEQ_NUM_SELECT_CANCEL:
    if( m_pDraw->IsNumCounterAnimeEnd() != false )
    {
      m_pTalkWindow->CloseMsgWin();
      m_pMenuCursor->SetVisible( true );
      m_pDraw->SetVisibleSelectedCursor( false );
      m_pDraw->SetVisibleInfoWindow( true );
      m_pDraw->SetInputEnabled( true );
      ChangeSequence( SEQ_MAIN );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMessage
 * @brief   メイン処理：メッセージ表示
 * @date    2015.10.07
 */
//-----------------------------------------------------------------------------
void ShopFrame::Update_SeqMessage(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pMenuCursor->SetVisible( false );
    m_pDraw->SetVisibleInfoWindow( false );
    m_pTalkWindow->CallMsgWin();
    m_pTalkWindow->SetInputEnabled( true );
    m_mainSeq++;
    break;

  case 1:
    if( m_pTalkWindow->GetStatus() != App::Tool::TalkWindow::WIN_STATUS_WIN_EXIT )
    {
      break;
    }
    if( m_pTalkWindow->GetStrWinResult() == gfl2::str::StrWin::RES_FINISH )
    {
      Sound::PlaySE( SEQ_SE_MESSAGE );
    }
    // いくつ買う？
    if( m_msgID == m_appParam->howmany_msg )
    {
      m_pTalkWindow->SetInputEnabled( false );
      ChangeSequence( SEQ_NUM_SELECT );
    }
    // xxコでxx円です
    else if( m_msgID == m_appParam->really_msg || m_msgID == m_appParam->really_msg2 )
    {
      m_pTalkWindow->SetInputEnabled( false );
      m_pMenuWindow->StartMenuDisplay( 0, app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_X_UPPER, app::tool::MenuWindow::DEFAULT_PUT_Y_UPPER );
      ChangeSequence( SEQ_MAIN );
    }
    // まいど
    else if( m_msgID == m_appParam->hereitis_msg )
    {
      m_msgID = msg_shop_13_01;   // @note チェック用
      m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );
      if( m_appParam->type == TYPE_FC_ITEM )
      {
        m_pTalkWindow->SetMessage( m_pDraw->GetMessageItemSet(m_pShopItem[m_selectPos].item) );
      }
      else
      {
        m_pTalkWindow->SetMessage( m_pDraw->GetMessageItemSet(m_pShopItem[m_selectPos].id) );
      }
      m_pTalkWindow->CallMsgWin();
    }
    // しまった
    else if( m_msgID == msg_shop_13_01 )
    {
      if( AddPremiereBall() != false )
      {
        m_msgID = mes_shop_13_02;
        m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );
        m_pTalkWindow->SetMessage( m_msgID );
        m_pTalkWindow->CallMsgWin();
      }
      else if( m_cashBack != 0 )
      {
        m_msgID = mes_shop_14_01;
        m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );
        m_pTalkWindow->SetMessage( m_pDraw->GetMessageCoupon(m_msgID,m_cashBack) );
        m_pTalkWindow->CallMsgWin();
      }
      else
      {
        m_pTalkWindow->CloseMsgWin();
        m_mainSeq++;
      }
    }
    // 持ちきれない
    // お金が足りない
    // 今日は終了しました
    // クーポンは今日いっぱい
    else if( m_msgID == m_appParam->youcanttake_msg ||
             m_msgID == m_appParam->nomoney_msg ||
             m_msgID == m_appParam->selected_msg ||
             m_msgID == mes_shop_14_02 )
    {
      m_pTalkWindow->CloseMsgWin();
      m_mainSeq++;
    }
    // プレミアボール
    else if( m_msgID == mes_shop_13_02 )
    {
      if( m_cashBack != 0 )
      {
        m_msgID = mes_shop_14_01;
        m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );
        m_pTalkWindow->SetMessage( m_pDraw->GetMessageCoupon(m_msgID,m_cashBack) );
        m_pTalkWindow->CallMsgWin();
      }
      else
      {
        m_pTalkWindow->CloseMsgWin();
        m_mainSeq++;
      }
    }
    // クーポンをもっている
    else if( m_msgID == mes_shop_14_01 )
    {
      m_myGold += m_cashBack;
      m_pDraw->PutGold( m_myGold );
      Sound::PlaySE( SEQ_SE_SYS_006 );

      m_msgID = mes_shop_14_02;
      m_pTalkWindow->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER );
      m_pTalkWindow->SetMessage( m_msgID );
      m_pTalkWindow->CallMsgWin();
    }

    break;

  case 2:
    m_pTalkWindow->SetInputEnabled( false );
    m_pMenuCursor->SetVisible( true );
    m_pDraw->SetVisibleSelectedCursor( false );
    m_pDraw->SetVisibleInfoWindow( true );
    m_pDraw->SetInputEnabled( true );
    ChangeSequence( SEQ_MAIN );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力時のリスナー
 * @date    2015.10.02
 *
 * @param   id    動作ID
 *
 * @note  override ShopDrawListener
 */
//-----------------------------------------------------------------------------
void ShopFrame::OnAction( ShopDrawListener::ActionID id )
{
  int seq = GetSubSeq();

  if( seq == SEQ_MAIN )
  {
    switch( id )
    {
    case ShopDrawListener::ACTION_ID_CANCEL:
      ChangeSequence( SEQ_END );
      Sound::PlaySE( SEQ_SE_CANCEL0 );
      m_selectPos = m_itemMax;
      break;
    case ShopDrawListener::ACTION_ID_SELECT:
      m_selectPos = m_cursorPos + m_cursorScroll;
      m_selectNum = 1;
      if( m_selectPos == m_itemMax )
      {
        ChangeSequence( SEQ_END );
      }
      else
      {
        BuyErrorID err = CheckBuyError();
        if( err == BUY_ERROR_MAX )
        {
          ChangeMessageSequence( m_appParam->youcanttake_msg, NULL, true );
        }
        else if( err == BUY_ERROR_NO_MONEY )
        {
          ChangeMessageSequence( m_appParam->nomoney_msg, NULL, true );
        }
        else if( err == BUY_ERRPR_SELECTED )
        {
          ChangeMessageSequence( m_appParam->selected_msg, NULL, true );
        }
        else
        {
          if( m_appParam->type == TYPE_BP_WAZA )
          {
            ChangeSequence( SEQ_END );
            Sound::PlaySE( SEQ_SE_DECIDE0 );
            break;
          }

          if( m_selectMax == 1 )
          {
            s32 gold = m_pShopItem[m_selectPos].price * m_selectNum;
            ChangeMessageSequence(
              m_appParam->really_msg,
              m_pDraw->GetMessageReally( m_appParam->really_msg, m_pShopItem[m_selectPos].id, m_selectNum, gold ),
              false );
          }
          else
          {
            ChangeMessageSequence(
              m_appParam->howmany_msg,
              m_pDraw->GetMessageHowMany( m_appParam->howmany_msg, m_pShopItem[m_selectPos].id ),
              false );
          }
        }
        m_pDraw->SetVisibleSelectedCursor( true, m_cursorPos );
      }
      Sound::PlaySE( SEQ_SE_DECIDE0 );
      break;
    case ShopDrawListener::ACTION_ID_KEY_UP:
      MoveCursor( -1, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_DOWN:
      MoveCursor( 1, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_LEFT:
      MoveCursor( -ShopDraw::ITEM_PUT_MAX, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_RIGHT:
      MoveCursor( ShopDraw::ITEM_PUT_MAX, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_UP_TRG:
      MoveCursor( -1, true );
      break;
    case ShopDrawListener::ACTION_ID_KEY_DOWN_TRG:
      MoveCursor( 1, true );
      break;
    case ShopDrawListener::ACTION_ID_KEY_LEFT_TRG:
      MoveCursor( -ShopDraw::ITEM_PUT_MAX, true );
      break;
    case ShopDrawListener::ACTION_ID_KEY_RIGHT_TRG:
      MoveCursor( ShopDraw::ITEM_PUT_MAX, true );
      break;
    }
  }
  else if( seq == SEQ_NUM_SELECT )
  {
    switch( id )
    {
    case ShopDrawListener::ACTION_ID_CANCEL:
      m_pDraw->SetInputEnabled( false );
      m_pDraw->OutNumCounter();
      m_mainSeq = UPDATE_SEQ_NUM_SELECT_CANCEL;
      Sound::PlaySE( SEQ_SE_CANCEL0 );
      break;
    case ShopDrawListener::ACTION_ID_SELECT:
      m_pDraw->SetInputEnabled( false );
      m_pDraw->OutNumCounter();
      m_mainSeq = UPDATE_SEQ_NUM_SELECT_DECIDE;
      Sound::PlaySE( SEQ_SE_DECIDE0 );
      break;
    case ShopDrawListener::ACTION_ID_KEY_UP:
      UpdateNumSelect( 1, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_DOWN:
      UpdateNumSelect( -1, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_LEFT:
      UpdateNumSelect( -10, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_RIGHT:
      UpdateNumSelect( 10, false );
      break;
    case ShopDrawListener::ACTION_ID_KEY_UP_TRG:
      UpdateNumSelect( 1, true );
      break;
    case ShopDrawListener::ACTION_ID_KEY_DOWN_TRG:
      UpdateNumSelect( -1, true );
      break;
    case ShopDrawListener::ACTION_ID_KEY_LEFT_TRG:
      UpdateNumSelect( -10, true );
      break;
    case ShopDrawListener::ACTION_ID_KEY_RIGHT_TRG:
      UpdateNumSelect( 10, true );
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー
 * @date    2015.10.06
 *
 * @param   id  メニュー項目のID
 *
 * @note  override app::tool::MenuWindowListener
 */
//-----------------------------------------------------------------------------
void ShopFrame::SelectMenu( u32 id )
{
  m_pMenuWindow->EndMenu( false );

  switch( id )
  {
  case 0:   // はい
    if( m_appParam->type == TYPE_FC_ACTION )
    {
      ChangeSequence( SEQ_END );
      break;
    }
    else if( m_appParam->type == TYPE_FC_ITEM )
    {
      m_myItem->Add( m_pShopItem[m_selectPos].item, m_pShopItem[m_selectPos].num );
      m_pDraw->PutMyItemNum( m_pShopItem[m_selectPos].item );
      // 1日1回フラグ
      m_saveJoinFesta->SetFacilitiesBuy( static_cast<u32>(m_selectPos) );
      m_pDraw->PutItemList( m_cursorScroll, m_cursorScrollMax );
    }
    else
    {
      if( m_appParam->type == TYPE_FR_SHOP )
      {
        Savedata::IncRecord( Savedata::Record::RECID_SHOPPING_CNT );                            // レコード
        PokeDiary::Set( PDID_COUNT_FRIENDLY_SHOP, m_pShopItem[m_selectPos].id, m_selectNum );   // ダイアリー
      }
      else
      {
        PokeDiary::Set( PDID_COUNT_BP_SHOP, m_pShopItem[m_selectPos].id, m_selectNum );   // ダイアリー
      }
      m_myItem->Add( m_pShopItem[m_selectPos].id, m_selectNum );
      m_pDraw->PutMyItemNum( m_pShopItem[m_selectPos].id );
      if( item::ITEM_CheckWazaMachine(m_pShopItem[m_selectPos].id) != false )
      {
        m_pDraw->PutItemList( m_cursorScroll, m_cursorScrollMax );
      }
    }
    {
      u32 price = m_pShopItem[m_selectPos].price * m_selectNum;
      m_myGold -= price;
      if( IsCoupon() == false )
      {
        m_cashBack = 0;
      }
      else
      {
//        m_cashBack = price / 10 * 4;  // ４割引き
        m_cashBack = price / 2;   // 杉中くんの要望で5割引きに変更 2016/04/04
      }
    }
    m_pDraw->PutGold( m_myGold );
    ChangeMessageSequence( m_appParam->hereitis_msg, NULL, true );
    Sound::PlaySE( SEQ_SE_SYS_006 );
    break;
  case 1:   // いいえ
    m_pTalkWindow->CloseMsgWin();
    m_pMenuCursor->SetVisible( true );
    m_pDraw->SetVisibleSelectedCursor( false );
    m_pDraw->SetVisibleInfoWindow( true );
    m_pDraw->SetInputEnabled( true );
    ChangeSequence( SEQ_MAIN );
    break;
  }
}


GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)
