//=============================================================================
/**
 * @file    ShopDraw.cpp
 * @brief   ショップ購入画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================

// module
#include "ShopDraw.h"
#include "App/Shop/include/ShopDrawListener.h"
// niji
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"
#include "Sound/include/Sound.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/app_tool_ScrollBar.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "Print/include/ItemMessage.h"
// resource
#include "niji_conv_header/app/shop/Shop.h"
#include "niji_conv_header/app/shop/Shop_pane.h"
#include "niji_conv_header/app/shop/Shop_anim_list.h"
#include "niji_conv_header/message/msg_shop.h"
#include "niji_conv_header/message/msg_jf_shop.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)


const f32 ShopDraw::CURSOR_MOVE_Y = 20.0f;  //!< カーソルY座標表示間隔

//! ショップタイプデータテーブル
const ShopDraw::SHOP_TYPE_DETA ShopDraw::ShopTypeData[TYPE_MAX] =
{
  { // TYPE_FR_SHOP
    msg_shop_02_01,                             // おこづかい
    msg_shop_03_01,                             // 円
    Savedata::Misc::MY_GOLD_KETA,               // 桁数
    LA_SHOP_SHOP_MIN_UPP_000_WALLET_TYPE_00,    // 背景色アニメ
    LA_SHOP_SHOP_MIN_UPP_000_MESSAGE_PATERN_00, // 説明表示切り替えアニメ
  },
  { // TYPE_BP_SHOP
    msg_shop_02_03,                             // バトルポイント
    msg_shop_03_06,                             // BP
    Savedata::Misc::BP_KETA,                    // 桁数
    LA_SHOP_SHOP_MIN_UPP_000_WALLET_TYPE_02,    // 背景色アニメ
    LA_SHOP_SHOP_MIN_UPP_000_MESSAGE_PATERN_00, // 説明表示切り替えアニメ
  },
  { // TYPE_FC_ITEM
    msg_shop_02_04,                             // フェスコイン
    msg_shop_03_07,                             // FC
    Savedata::Misc::FC_KETA,                    // 桁数
    LA_SHOP_SHOP_MIN_UPP_000_WALLET_TYPE_03,    // 背景色アニメ
    LA_SHOP_SHOP_MIN_UPP_000_MESSAGE_PATERN_00, // 説明表示切り替えアニメ
  },
  { // TYPE_FC_ACTION
    msg_shop_02_04,                             // フェスコイン
    msg_shop_03_07,                             // FC
    Savedata::Misc::FC_KETA,                    // 桁数
    LA_SHOP_SHOP_MIN_UPP_000_WALLET_TYPE_03,    // 背景色アニメ
    LA_SHOP_SHOP_MIN_UPP_000_MESSAGE_PATERN_01, // 説明表示切り替えアニメ
  },

  { // TYPE_BP_WAZA
    msg_shop_02_03,                             // バトルポイント
    msg_shop_03_06,                             // BP
    Savedata::Misc::BP_KETA,                    // 桁数
    LA_SHOP_SHOP_MIN_UPP_000_WALLET_TYPE_02,    // 背景色アニメ
    LA_SHOP_SHOP_MIN_UPP_000_MESSAGE_PATERN_01, // 説明表示切り替えアニメ
  },
};



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   type        ショップタイプ
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   cursor      メニューカーソル
 */
//-----------------------------------------------------------------------------
ShopDraw::ShopDraw( ShopType type, app::util::Heap * heap, app::util::AppRenderingManager * render_man, app::tool::MenuCursor * cursor )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_type( type )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_menuCursor( cursor )
  , m_shopData( NULL )
  , m_myItem( NULL )
  , m_saveJoinFesta( NULL )
  , m_listener( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pScrollBar( NULL )
  , m_shopItem( NULL )
  , m_itemTableMax( 0 )
  , m_itemManager( NULL )
  , m_pItemMessage( NULL )
  , m_itemIcon( NULL )
  , m_iconLoadID( 0 )
  , m_iconReqID( 0 )
  , m_iconSeq( 0 )
  , m_scroll( 0.0f )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
{
  m_shopData = &ShopTypeData[m_type];
  SetInputEnabled( false );
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ShopDraw::~ShopDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopDraw::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_myItem        = gd->GetMyItem();
  m_saveJoinFesta = gd->GetJoinFestaData();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupItem
 * @brief   道具データ設定
 * @date    2015.10.05
 *
 * @param   item  データリスト
 * @param   max   データ数
 * @param   man   アイテムマネージャ
 * @param   icon  アイテムアイコン
 */
//-----------------------------------------------------------------------------
void ShopDraw::SetupItem( const SHOP_ITEM * item, u32 max, itemman::ITEM_MANAGER * man, app::tool::ItemIcon * icon )
{
  m_shopItem     = item;
  m_itemTableMax = max;
  m_itemManager  = man;
  m_itemIcon     = icon;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.10.02
 */
//-----------------------------------------------------------------------------
void ShopDraw::Update(void)
{
  app::ui::UIView::Update();
  UpdateItemIcon();
  m_menuCursor->Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.10.02
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void ShopDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    m_pScrollBar->Draw( m_scroll, 255.0f );
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_BASE );
    m_menuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_UPPER );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDrawListener
 * @brief   リスナーをセット
 * @date    2015.10.02
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void ShopDraw::SetDrawListener( ShopDrawListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemMessage
 * @brief   道具説明メッセージ作成
 * @date    2015.10.05
 *
 * @param   buf   メッセージデータ
 *
 * @note  MsgDataを持っているだけなので、FCショップでもMsgData目的で使う
 */
//-----------------------------------------------------------------------------
void ShopDraw::CreateItemMessage( void * buf )
{
  m_pItemMessage = GFL_NEW(m_heap->GetSystemHeap()) print::ItemMessage( m_heap->GetDeviceHeap(), buf );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteItemMessage
 * @brief   道具説明メッセージ削除
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopDraw::DeleteItemMessage(void)
{
  GFL_SAFE_DELETE( m_pItemMessage );
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.10.02
 *
 * @param   res_buf       レイアウトリソースバッファ
 * @param   msg_buf       メッセージリソースバッファ
 * @param   wset_loader   WordSetLoader
 * @param   scroll_max    スクロール最大値
 */
//-----------------------------------------------------------------------------
void ShopDraw::Init( void * res_buf, void * msg_buf, WordSetLoader * wset_loader, u32 scroll_max )
{
  Initialize2D( res_buf, msg_buf, wset_loader );
  CreateScrollBar();
  CreateMenuCursorLayout();
  SetDefaultAnime();
  WriteDefaultString();
  PutItemList( 0, scroll_max );
  UpdateCursorPosInfo( 0 );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.10.02
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  if( DeleteMenuCursorLayout() == false )
  {
    return false;
  }
  DeleteScrollBar();
  Terminate2D();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2015.10.02
 *
 * @param   res_buf       レイアウトリソースバッファ
 * @param   msg_buf       メッセージリソースバッファ
 * @param   wset_loader   WordSetLoader
 */
//-----------------------------------------------------------------------------
void ShopDraw::Initialize2D( void * res_buf, void * msg_buf, WordSetLoader * wset_loader )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { res_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_SHOP_SHOP_MIN_UPP_000_BFLYT,
      LA_SHOP_SHOP_MIN_UPP_000___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  SetWordSetLoader( wset_loader );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.10.02
 */
//-----------------------------------------------------------------------------
void ShopDraw::Terminate2D(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMsgData
 * @brief   メッセージデータ取得
 * @date    2015.10.07
 *
 * @return  メッセージデータ
 */
//-----------------------------------------------------------------------------
gfl2::str::MsgData * ShopDraw::GetMsgData(void)
{
  return m_pMsgData;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2015.10.06
 */
//-----------------------------------------------------------------------------
void ShopDraw::CreateMenuCursorLayout(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  m_menuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_UPPER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  m_menuCursor->SetVisible( false );
  m_menuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2015.10.06
 */
//-----------------------------------------------------------------------------
bool ShopDraw::DeleteMenuCursorLayout(void)
{
  return m_menuCursor->DeleteLayoutWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    PutCursor
 * @brief   カーソル配置
 * @date    2015.10.06
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void ShopDraw::PutCursor( u32 pos )
{
  gfl2::math::VEC3 trans = GetG2DUtil()->GetPanePos( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_NULL_CUR );
  trans.y -= ( CURSOR_MOVE_Y * static_cast<f32>(pos) );
  m_menuCursor->SetPos( &trans );
  m_menuCursor->SetVisible( true );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateScrollBar
 * @brief   スクロールバー生成
 * @date    2015.10.06
 */
//-----------------------------------------------------------------------------
void ShopDraw::CreateScrollBar(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_SCROLL_00, true );

  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_BASE );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( LYTID_BASE );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_SHOP_MIN_UPP_000_PANE_SCROLL_00 );

  m_pScrollBar = GFL_NEW(m_heap->GetSystemHeap()) app::tool::ScrollBar(
                                                    parts,
                                                    lytwk->GetPane( parts, PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER, &res_id ),
                                                    lytwk->GetPane( parts, PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP, &res_id ),
                                                    lytwk->GetPane( parts, PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM, &res_id ),
                                                    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteScrollBar
 * @brief   スクロールバー削除
 * @date    2015.10.06
 */
//-----------------------------------------------------------------------------
void ShopDraw::DeleteScrollBar(void)
{
  GFL_SAFE_DELETE( m_pScrollBar );
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2015.10.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopDraw::Start(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  switch( m_subSeq )
  {
  case 0:
    g2d->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_IN );
    g2d->StartAnime( LYTID_BASE, m_shopData->bg_color );
    g2d->StartAnime( LYTID_BASE, m_shopData->info_win );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( g2d->IsAnimeEnd( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_IN ) == false ||
        g2d->IsAnimeEnd( LYTID_BASE, m_shopData->bg_color ) == false ||
        g2d->IsAnimeEnd( LYTID_BASE, m_shopData->info_win ) == false ||
        IsItemNumStrAnime() == false )
    {
      break;
    }
    PutCursor( 0 );
    SetInputEnabled( true );
    m_subSeq = 0;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetDefaultAnime
 * @brief   デフォルトアニメ再生
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopDraw::SetDefaultAnime(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_SELECTED_CUR );
  g2d->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_KEEP );
  g2d->StartAnimeFrame( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_OUT, GetAnimeMaxFrame(LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_OUT) );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetAnimeMaxFrame
 * @brief   アニメ最大フレーム取得
 * @date    2015.10.05
 *
 * @param   id  アニメID
 *
 * @return  最大フレーム
 */
//-----------------------------------------------------------------------------
f32 ShopDraw::GetAnimeMaxFrame( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->BindAnime( LYTID_BASE, id );
  return g2d->GetAnimeMaxFrame( LYTID_BASE, id );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultSrring
 * @brief   デフォルト文字列描画
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopDraw::WriteDefaultString(void)
{
  // おこづかい
  SetTextboxPaneMessage( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTBOX_00, m_shopData->gold_name );
  // 持っている数
  SetTextboxPaneMessage( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTBOX_02, msg_shop_04_03 );
  // x
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( LYTID_BASE );
    SetTextboxPaneMessage(
      g2d->GetLayoutWork(LYTID_BASE)->GetTextBoxPane( GetNumCounterParts(), PANENAME_SHOP_PLT_UPP_000_PANE_TEXTBOX_00, &res_id ),
      msg_shop_05_01 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutGold
 * @brief   所持金表示
 * @date    2015.10.05
 *
 * @param   gold  所持金
 */
//-----------------------------------------------------------------------------
void ShopDraw::PutGold( u32 gold )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  g2d->SetRegisterNumber( 0, gold, m_shopData->gold_keta );
  g2d->SetTextBoxPaneStringExpand( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTBOX_01, m_shopData->gold_unit );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItemList
 * @brief   道具リスト描画
 * @date    2015.10.05
 *
 * @param   scroll_now  現在のスクロール値
 * @param   scroll_max  スクロール最大値
 */
//-----------------------------------------------------------------------------
void ShopDraw::PutItemList( u32 scroll_now, u32 scroll_max )
{
  static const gfl2::lyt::LytPaneIndex tbl[][2] =
  {
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_00, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_00 },
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_01, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_01 },
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_02, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_02 },
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_03, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_03 },
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_04, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_04 },
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_05, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_05 },
    { PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMNAME_06, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_ITEMPRICE_06 },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  // 名前
  for( u32 i=0; i<ITEM_PUT_MAX; i++ )
  {
    u32 p = scroll_now + i;
    if( p < m_itemTableMax )
    {
      gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
      bool is_price = true;

      // 名前
      if( m_type == TYPE_FC_ITEM )
      {
        m_pItemMessage->GetInfoMessage( jf_shop_name_000+m_shopItem[p].id, str );
        // １日１回 
        if( m_saveJoinFesta->IsFacilitiesBuy( p ) )
        {
          is_price = false;
        }
      }
      else if( m_type == TYPE_FC_ACTION )
      {
        m_pItemMessage->GetInfoMessage( jf_shop_name_000+m_shopItem[p].id, str );
        // １日１回 
        if( m_saveJoinFesta->IsFacilitiesBuy( p ) )
        {
          is_price = false;
        }
      }
      else if( m_type == TYPE_BP_WAZA )
      {
        g2d->SetRegisterWazaName( 0, m_shopItem[p].id );
        g2d->GetStringExpand( str, msg_shop_04_02 );
      }
      else
      {
        if( item::ITEM_CheckWazaMachine(m_shopItem[p].id) == false )
        {
          g2d->SetRegisterItemName( 0, m_shopItem[p].id );
          g2d->GetStringExpand( str, msg_shop_04_01 );
        }
        else
        {
          g2d->SetRegisterWazaName( 0, item::ITEM_GetWazaNo(m_shopItem[p].id) );
          g2d->GetStringExpand( str, msg_shop_04_02 );
          if( m_myItem->Check(m_shopItem[p].id,1) != false )
          {
            is_price = false;
          }
        }
      }
      g2d->SetTextBoxPaneString( LYTID_BASE, tbl[i][0], str );

      // 値段
      if( is_price != false )
      {
        u32 gold = m_shopItem[p].price;
        g2d->SetRegisterNumber( 0, gold, m_shopData->gold_keta );
        g2d->SetTextBoxPaneStringExpand( LYTID_BASE, tbl[i][1], m_shopData->gold_unit );
      }
      else
      {
        SetTextboxPaneMessage( LYTID_BASE, tbl[i][1], msg_shop_03_04 );
      }

      g2d->SetPaneVisible( LYTID_BASE, tbl[i][0], true );
      g2d->SetPaneVisible( LYTID_BASE, tbl[i][1], true );
    }
    else if( p == m_itemTableMax )
    {
      // やめる
      SetTextboxPaneMessage( LYTID_BASE, tbl[i][0], msg_shop_03_08 );
      g2d->SetPaneVisible( LYTID_BASE, tbl[i][0], true );
      g2d->SetPaneVisible( LYTID_BASE, tbl[i][1], false );
    }
    else
    {
      // 名前
      g2d->SetPaneVisible( LYTID_BASE, tbl[i][0], false );
      // 値段
      g2d->SetPaneVisible( LYTID_BASE, tbl[i][1], false );
    }
  }

  if( scroll_max == 0 )
  {
    m_pScrollBar->SetVisible( false );
  }
  else
  {
    m_scroll = static_cast<f32>(scroll_now) / static_cast<f32>(scroll_max);
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItemInfo
 * @brief   道具説明描画
 * @date    2015.10.05
 *
 * @param   item  道具番号
 */
//-----------------------------------------------------------------------------
void ShopDraw::PutItemInfo( u32 item )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );

  if( item == PUT_INFO_CANCEL_MESSAGE )
  {
    g2d->GetString( str, msg_shop_04_05 );
  }
  else
  {
    u32 msg_id = item;

    if( m_type == TYPE_FC_ITEM )
    {
      msg_id = jf_shop_exp_000 + item;
    }
    else if( m_type == TYPE_FC_ACTION )
    {
      msg_id = jf_shop_exp_000 + item;
    }

    m_pItemMessage->GetInfoMessage( msg_id, str );
  }

  g2d->SetTextBoxPaneString( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_MESSAGE, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleInfoWindow
 * @brief   道具説明表示切り替え
 * @date    2015.10.07
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void ShopDraw::SetVisibleInfoWindow( bool flg )
{
  GetG2DUtil()->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_WINDOW_MESSAGE, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetItemNumStrAnime
 * @brief   持っている数表示変更アニメ開始
 * @date    2015.10.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopDraw::IsItemNumStrAnime(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  if( g2d->IsAnimeEnd( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_TEXTALPHA_00 ) == false ||
      g2d->IsAnimeEnd( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_TEXTALPHA_01 ) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutMyItemNum
 * @brief   持っている数表示
 * @date    2015.10.05
 *
 * @param   item  道具番号
 */
//-----------------------------------------------------------------------------
void ShopDraw::PutMyItemNum( u32 item )
{
  if( m_type != TYPE_FC_ACTION && m_type != TYPE_BP_WAZA )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();

    if( item != ITEM_DUMMY_DATA && item::ITEM_CheckWazaMachine(item) == false )
    {
      g2d->SetRegisterNumber( 0, m_myItem->GetItemNum(item), 3 );
      g2d->SetTextBoxPaneStringExpand( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTBOX_03, msg_shop_04_04 );
  //    g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTBOX_02, true );
  //    g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTBOX_03, true );
      g2d->StopAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_TEXTALPHA_01 );
      g2d->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_TEXTALPHA_00 );
    }
    else
    {
      g2d->StopAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_TEXTALPHA_00 );
      g2d->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000_TEXTALPHA_01 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetItemIconRequest
 * @brief   道具アイコン変更リクエスト
 * @date    2015.10.05
 *
 * @param   item  道具番号
 */
//-----------------------------------------------------------------------------
void ShopDraw::SetItemIconRequest( u32 item )
{
  if( m_iconLoadID == item )
  {
    return;
  }
  GetG2DUtil()->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_ICON_ITEM, false );
  m_iconReqID = item;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateItemIcon
 * @brief   道具アイコン更新
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopDraw::UpdateItemIcon(void)
{
  if( m_itemIcon == NULL )
  {
    return;
  }
  if( m_iconReqID == ITEM_DUMMY_DATA )
  {
    return;
  }

  switch( m_iconSeq )
  {
  case 0:
    m_itemIcon->LoadRequest( 0, m_iconReqID );
    m_iconLoadID = m_iconReqID;
    m_iconSeq = 1;
    /* FALL THROUGH */
  case 1:
    if( m_itemIcon->IsLoadFinished(0) == false )
    {
      break;
    }
    // 新しいリクエストが発行されていないか
    if( m_iconLoadID == m_iconReqID )
    {
      gfl2::lyt::LytPicture * pane = GetG2DUtil()->GetLayoutWork(LYTID_BASE)->GetPicturePane(PANENAME_SHOP_MIN_UPP_000_PANE_ICON_ITEM);
      m_itemIcon->ReplaceReadTexture( 0, pane );
      pane->SetVisible( true );
      m_iconReqID = ITEM_DUMMY_DATA;
    }
    m_iconSeq = 0;
    break;
  }
}




//-----------------------------------------------------------------------------
/**
 * @func    GetNumCounterParts
 * @brief   個数選択の部品ペインを取得
 * @date    2015.10.05
 *
 * @return 部品ペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytParts * ShopDraw::GetNumCounterParts(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  return g2d->GetLayoutWork( LYTID_BASE )->GetPartsPane( PANENAME_SHOP_MIN_UPP_000_PANE_PLT_COUNTER );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateNumCounter
 * @brief   個数選択ウィンドウ表示更新
 * @date    2015.10.05
 *
 * @param   gold  金額
 * @param   num   個数
 */
//-----------------------------------------------------------------------------
void ShopDraw::UpdateNumCounter( u32 gold, u32 num )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_BASE );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( LYTID_BASE );
  gfl2::lyt::LytParts * parts = GetNumCounterParts();

  { // 個数
    g2d->SetRegisterNumber( 0, num, 2 );
    g2d->SetTextBoxPaneStringExpand(
      lytwk->GetTextBoxPane( parts, PANENAME_SHOP_PLT_UPP_000_PANE_TEXTBOX_01, &res_id ),
      msg_shop_05_02 );
  }
  { // 合計金額
    g2d->SetRegisterNumber( 0, gold*num, m_shopData->gold_keta );
    g2d->SetTextBoxPaneStringExpand(
      lytwk->GetTextBoxPane( parts, PANENAME_SHOP_PLT_UPP_000_PANE_TEXTBOX_02, &res_id ),
      m_shopData->gold_unit );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InNumCounter
 * @brief   個数選択ウィンドウ表示
 * @date    2015.10.05
 *
 * @param   gold  金額
 * @param   num   個数
 */
//-----------------------------------------------------------------------------
void ShopDraw::InNumCounter( u32 gold, u32 num )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  UpdateNumCounter( gold, num );
  g2d->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_IN );
  g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_PLT_COUNTER, true );
}

//-----------------------------------------------------------------------------
/**
 * @func    OutNumCounter
 * @brief   個数選択ウィンドウ非表示
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
void ShopDraw::OutNumCounter(void)
{
  GetG2DUtil()->StartAnime( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_OUT );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsNumCounterAnimeEnd
 * @brief   個数選択ウィンドウIN/OUTアニメ動作チェック
 * @date    2015.10.05
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ShopDraw::IsNumCounterAnimeEnd(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  if( g2d->IsAnimeEnd( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_IN ) == false ||
      g2d->IsAnimeEnd( LYTID_BASE, LA_SHOP_SHOP_MIN_UPP_000__PLT_COUNTER_OUT ) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetStrMenu
 * @brief   メニュー項目文字列取得
 * @date    2015.10.06
 *
 * @param   id  メッセージID
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetStrMenu( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
  g2d->GetString( str, id );
  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetStrYes
 * @brief   メニュー項目文字列"はい"取得
 * @date    2015.10.06
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetStrYes(void)
{
  return GetStrMenu( msg_shop_07_02 );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetStrNo
 * @brief   メニュー項目文字列"いいえ"取得
 * @date    2015.10.06
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetStrNo(void)
{
  return GetStrMenu( msg_shop_07_03 );
}


//-----------------------------------------------------------------------------
/**
 * @func    MoveCursor
 * @brief   カーソル移動
 * @date    2015.10.06
 *
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 */
//-----------------------------------------------------------------------------
void ShopDraw::MoveCursor( s32 pos, s32 scroll )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( (pos+scroll) >= m_itemTableMax )
  {
    g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_ICON_ITEM, false );
    g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_MESSAGE, true );
    PutMyItemNum( ITEM_DUMMY_DATA );
    PutItemInfo( PUT_INFO_CANCEL_MESSAGE );
  }
  else
  {
    UpdateCursorPosInfo( pos+scroll );
  }

  PutCursor( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateCursorPosInfo
 * @brief   カーソル位置の情報を更新
 * @date    2015.10.10
 *
 * @param   pos   リスト位置
 */
//-----------------------------------------------------------------------------
void ShopDraw::UpdateCursorPosInfo( u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  const SHOP_ITEM * dat = &m_shopItem[pos];

  PutItemInfo( dat->id );
  if( m_type == TYPE_FC_ITEM )
  {
    PutMyItemNum( dat->item );
    SetItemIconRequest( dat->item );
    g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_ICON_ITEM, true );
  }
  else if( m_type == TYPE_FC_ACTION || m_type == TYPE_BP_WAZA )
  {
    PutMyItemNum( ITEM_DUMMY_DATA );
  }
  else
  {
    PutMyItemNum( dat->id );
    SetItemIconRequest( dat->id );
    g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_ICON_ITEM, true );
  }
  g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXT_MESSAGE, true );
}


//-----------------------------------------------------------------------------
/**
 * @func    GetMessageItemSet
 * @brief   "ポケットへしまった"メッセージ取得
 * @date    2015.10.07
 *
 * @param   item  道具番号
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetMessageItemSet( u32 item )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  u32 id;

  if( item::ITEM_CheckWazaMachine(item) == false )
  {
    id = msg_shop_13_01;
    g2d->SetRegisterItemName( 0, item );
  }
  else
  {
    id = msg_wazashop_04_01;
    g2d->SetRegisterWazaName( 0, item::ITEM_GetWazaNo(item) );
  }

  if( m_myItem->IsRegisterFreeSpace(item) == false )
  {
    m_pWordSet->RegisterFieldPocketName( 1, m_itemManager->GetParam(item,item::ITEM_DATA::PRM_ID_F_POCKET) );
  }
  else
  {
    m_pWordSet->RegisterFieldPocketName( 1, BAG_POCKET_FREE );
  }

  g2d->GetStringExpand( str, id );

  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMessageHowMany
 * @brief   "いくつ？"メッセージ取得
 * @date    2015.10.07
 *
 * @param   id    メッセージID
 * @param   item  道具番号
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetMessageHowMany( u32 id, u32 item )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  g2d->SetRegisterItemName( 0, item );

  g2d->GetStringExpand( str, id );

  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMessageReally
 * @brief   "xx円になりますが？"メッセージ取得
 * @date    2015.10.07
 *
 * @param   id    メッセージID
 * @param   item  道具番号
 * @param   num   個数
 * @param   gold  金額
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetMessageReally( u32 id, u32 item, u32 num, u32 gold )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  if( m_type == TYPE_FC_ITEM )
  {
    m_pItemMessage->GetInfoMessage( jf_shop_name_000+item, str );
    g2d->SetRegisterWord( 0, *str );
    g2d->SetRegisterNumber( 1, gold, m_shopData->gold_keta );
  }
  else if( m_type == TYPE_FC_ACTION )
  {
    m_pItemMessage->GetInfoMessage( jf_shop_name_000+item, str );
    g2d->SetRegisterWord( 0, *str );
    g2d->SetRegisterNumber( 1, gold, m_shopData->gold_keta );
  }
  else if( m_type == TYPE_BP_WAZA )
  {
    GFL_ASSERT( 0 );  // @note こないはず。
  }
  else
  {
    if( item::ITEM_CheckWazaMachine(item) == false )
    {
      g2d->SetRegisterItemName( 0, item, num );
      g2d->SetRegisterNumber( 1, num, 2 );
    }
    else
    {
      g2d->SetRegisterNumber( 0, item::ITEM_GetWazaMashineNo(item)+1, 3, print::NUM_DISP_ZERO );
      g2d->SetRegisterWazaName( 1, item::ITEM_GetWazaNo(item) );
    }
    g2d->SetRegisterNumber( 2, gold, m_shopData->gold_keta );
  }

  g2d->GetStringExpand( str, id );

  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMessageCoupon
 * @brief   クーポン使用メッセージ取得
 * @date    2016.01.09
 *
 * @param   id    メッセージID
 * @param   gold  金額
 *
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * ShopDraw::GetMessageCoupon( u32 id, u32 gold )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  g2d->SetRegisterNumber( 0, gold, Savedata::Misc::MY_GOLD_KETA );
  g2d->GetStringExpand( str, id );

  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishMyItemNum
 * @brief   "もっているかず"を非表示にする
 * @date    2015.11.30
 */
//-----------------------------------------------------------------------------
void ShopDraw::VanishMyItemNum(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTALPHA_00, false );
  g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_TEXTALPHA_01, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleSelectedCursor
 * @brief   選択した項目カーソルの表示切り替え
 * @date    2016.01.20
 *
 * @param   flg   true = 表示, false = 非表示
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void ShopDraw::SetVisibleSelectedCursor( bool flg, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( flg != false )
  {
    gfl2::math::VEC3 move = g2d->GetPanePos( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_NULL_CUR );
    gfl2::math::VEC3 stop = g2d->GetPanePos( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_SELECTED_CUR );
    stop.y = move.y - CURSOR_MOVE_Y * static_cast<f32>(pos);
    g2d->SetPanePos( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_SELECTED_CUR, &stop );
  }
  g2d->SetPaneVisible( LYTID_BASE, PANENAME_SHOP_MIN_UPP_000_PANE_SELECTED_CUR, flg );
}


//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.10.02
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult ShopDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_CANCEL );
    return DISABLE_ACCESS;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_SELECT );
    return DISABLE_ACCESS;
  }

  if( key->IsTrigger(gfl2::ui::DIR_UP) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_UP_TRG );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_DOWN_TRG );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_LEFT) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_LEFT_TRG );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_RIGHT) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_RIGHT_TRG );
    return DISABLE_ACCESS;
  }

  if( key->IsRepeat(gfl2::ui::DIR_UP) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_UP );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_DOWN );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_LEFT );
    return DISABLE_ACCESS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    m_listener->OnAction( ShopDrawListener::ACTION_ID_KEY_RIGHT );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)
