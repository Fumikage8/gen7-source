//=============================================================================
/**
 * @file    BagInfoField.cpp
 * @brief   バッグ画面フィールド用アイテム情報
 * @author  Hiroyuki Nakamura
 * @date    2015.03.26
 */
//=============================================================================

// module
#include "App/AppEvent/include/Bag/BagGamedata.h"
#include "App/FieldBag/include/BagInfoField.h"
// niji
#include "Savedata/include/MyItemSaveDef.h"
#include "Savedata/include/MiscSave.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/FieldBag.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/app/field_bag/FieldBag.h"
#include "niji_conv_header/app/field_bag/FieldBag_pane.h"
#include "niji_conv_header/app/field_bag/FieldBag_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   item_man    アイテムマネージャ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
BagInfoField::BagInfoField(
      app::util::Heap * heap,
      itemman::ITEM_MANAGER * item_man,
      app::util::AppRenderingManager * render_man,
      app::tool::AppCommonGrpIconData * common_icon )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_itemManager( item_man )
  , m_renderingManager( render_man )
  , m_commonIcon( common_icon )
  , m_pItemMessage( NULL )
  , m_putItem( START_PUT_ITEM )
  , m_isDrawEnable( true )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagInfoField::~BagInfoField()
{
  DleteItemInfo();
  Delete2D();
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   セットアップ
 * @date    2015.04.10
 *
 * @param   lyt_buf   レイアウトリソースバッファ
 * @param   msg_info  道具説明のメッセージリソースバッファ
 * @param   msg_data  バッグ画面のメッセージデータ
 * @param   wset      単語セット
 */
//-----------------------------------------------------------------------------
void BagInfoField::Setup( void * lyt_buf, void * msg_info, gfl2::str::MsgData * msg_data, print::WordSet * wset, WordSetLoader * wset_loader )
{
  CreateLayout( lyt_buf, msg_data, wset, wset_loader );
  CreateItemInfo( msg_info );
  SetDefaultString();
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEnd
 * @brief   終了できるかチェック
 * @date    2015.08.21
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool BagInfoField::IsEnd(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.03.26
 */
//-----------------------------------------------------------------------------
void BagInfoField::Update(void)
{
  app::ui::UIView::Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.03.26
 */
//-----------------------------------------------------------------------------
void BagInfoField::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartInAnime
 * @brief   開始アニメ開始
 * @date    2015.06.02
 */
//-----------------------------------------------------------------------------
void BagInfoField::StartInAnime(void)
{
//  GetG2DUtil()->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_IN );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsInAnime
 * @brief   開始アニメ終了チェック
 * @date    2015.06.02
 *
 * @retval  false = 処理中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagInfoField::IsInAnime(void)
{
//  return GetG2DUtil()->IsAnimeEnd( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_IN );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartOutAnime
 * @brief   終了アニメ開始
 * @date    2015.06.02
 */
//-----------------------------------------------------------------------------
void BagInfoField::StartOutAnime(void)
{
//  GetG2DUtil()->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_OUT );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsOutAnime
 * @brief   終了アニメ終了チェック
 * @date    2015.06.02
 *
 * @retval  false = 処理中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagInfoField::IsOutAnime(void)
{
//  return GetG2DUtil()->IsAnimeEnd( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_OUT );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2015.03.26
 */
//-----------------------------------------------------------------------------
void BagInfoField::CreateLayout( void * buf, gfl2::str::MsgData * msg_data, print::WordSet * wset, WordSetLoader * wset_loader )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_FIELDBAG_FIELDBAG_MIN_UPP_000_BFLYT,
      LA_FIELDBAG_FIELDBAG_MIN_UPP_000___NUM,
      buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  SetWordSetLoader( wset_loader );

  Create2D(
    m_heap,
    NULL,
    GFL_NELEMS(data_tbl),
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    msg_data,
    wset );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemInfo
 * @brief   道具説明メッセージクラス生成
 * @date    2015.04.24
 *
 * @param   buf   メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void BagInfoField::CreateItemInfo( void * buf )
{
  m_pItemMessage = GFL_NEW(m_heap->GetSystemHeap()) print::ItemMessage( m_heap->GetDeviceHeap(), buf );
}

//-----------------------------------------------------------------------------
/**
 * @func    DleteItemInfo
 * @brief   道具説明メッセージクラス破棄
 * @date    2015.04.24
 */
//-----------------------------------------------------------------------------
void BagInfoField::DleteItemInfo(void)
{
  GFL_SAFE_DELETE( m_pItemMessage );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDefaultString
 * @brief   デフォルト文字列書き込み
 * @date    2015.03.27
 */
//-----------------------------------------------------------------------------
void BagInfoField::SetDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  //「ｘ」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_02, msg_bag_01_15 );
  //「おこづかい」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_05, msg_bag_01_25 );
  { //「せいりする」
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_FIELDBAG_MIN_UPP_000_PANE_PARTS_X );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_PLT_000_PANE_TEXTBOX_00, &res_id );
    g2d->SetTextBoxPaneString( text, msg_bag_01_30 );
  }
  { //「いれかえる」
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_FIELDBAG_MIN_UPP_000_PANE_PARTS_Y );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_PLT_000_PANE_TEXTBOX_00, &res_id );
    g2d->SetTextBoxPaneString( text, msg_bag_01_31 );
  }
  //「タイプ」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_07, msg_bag_01_06 );
  //「ぶんるい」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_08, msg_bag_01_07 );
  //「いりょく」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_09, msg_bag_01_08 );
  //「めいちゅう」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_10, msg_bag_01_09 );
  //「PP」
  g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_11, msg_bag_01_10 );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemIcon
 * @brief   アイテムアイコンのピクチャペインを取得
 * @date    2015.03.27
 *
 * @return  ピクチャペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPicture * BagInfoField::GetItemIcon(void)
{
  return GetG2DUtil()->GetLayoutWork(0)->GetPicturePane( PANENAME_FIELDBAG_MIN_UPP_000_PANE_ICON_ITEM );
}

//-----------------------------------------------------------------------------
/**
 * @func    VisibleItemIcon
 * @brief   アイテムアイコンの表示切り替え
 * @date    2015.03.27
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagInfoField::VisibleItemIcon( bool flg )
{
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_ICON_ITEM, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateInfo
 * @brief   アイテム情報表示更新
 * @date    2015.03.27
 *
 * @param   id    アイテムID
 * @param   num   所持数
 */
//-----------------------------------------------------------------------------
void BagInfoField::UpdateInfo( u32 id, u32 num )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  // 道具なし
  if( id == ITEM_DUMMY_DATA )
  {
    if( m_putItem != id )
    {
      g2d->StopAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_ITEM_1 );
      g2d->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_ITEM_0 );
    }
    // 技マシンアニメ（通常の表示にしておく）
    if( m_putItem != START_PUT_ITEM && item::ITEM_CheckWazaMachine(m_putItem) != false )
    {
      g2d->StopAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_VISIBLE_WAZAMACHINE );
      g2d->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_VISIBLE_NORMAL );
    }
    m_putItem = ITEM_DUMMY_DATA;
  }
  else
  {
    { // アイテム説明
      gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
      m_pItemMessage->GetInfoMessage( id, str );
      g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_04, str );
    }

    WazaNo waza = static_cast<WazaNo>( item::ITEM_GetWazaNo(id) );
    // 技マシン
    if( waza != WAZANO_NULL )
    {
      // 名前
      u32	no = item::ITEM_GetWazaMashineNo(id) + 1;
      if( no < 100 ){
        g2d->SetRegisterNumber( 0, no, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      }else{
        g2d->SetRegisterNumber( 0, no, 3, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      }
      g2d->SetRegisterWazaName( 1, waza );
      g2d->SetTextBoxPaneStringExpand( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_01, msg_bag_01_05 );
      // タイプ
      {
        gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
        gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
        m_commonIcon->ReplacePaneTextureByWazaTypeIcon(
          static_cast<PokeType_tag>(pml::wazadata::GetType(waza)),
          lytwk->GetPicturePane( lytwk->GetPartsPane(PANENAME_FIELDBAG_MIN_UPP_000_PANE_PARTS_WAZATYPE), PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &res_id ) );
      }
      // 分類
      {
        static const u32 bunrui_anm[] =
        {
          LA_FIELDBAG_FIELDBAG_MIN_UPP_000__PARTS_WAZABUNRUI_TEXTURE_00,
          LA_FIELDBAG_FIELDBAG_MIN_UPP_000__PARTS_WAZABUNRUI_TEXTURE_01,
          LA_FIELDBAG_FIELDBAG_MIN_UPP_000__PARTS_WAZABUNRUI_TEXTURE_02,
        };
        g2d->StartAnime( 0, bunrui_anm[pml::wazadata::GetDamageType(waza)] );
      }

      // 威力
      {
        u32 pow = pml::wazadata::GetPower( waza );
        if( pow <= 1 ){
          g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_12, msg_bag_01_19 );
        }else{
          g2d->SetTextBoxPaneNumber(
            0,
            PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_12,
            msg_bag_01_17,
            pow,
            3,
            0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        }
      }
			// 命中
      {
        u32 hit = pml::wazadata::GetParam( waza, pml::wazadata::PARAM_ID_HITPER );
        if( pml::wazadata::IsAlwaysHit(waza) == true ){
          g2d->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_13, msg_bag_01_19 );
        }else{
          g2d->SetTextBoxPaneNumber(
            0,
            PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_13,
            msg_bag_01_17,
            hit,
            3,
            0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        }
      }
      // PP
      g2d->SetTextBoxPaneNumber(
        0,
        PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_14,
        msg_bag_01_17,
        pml::wazadata::GetMaxPP(waza,0),
        3,
        0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );

      // アニメ
      if( m_putItem != id && item::ITEM_CheckWazaMachine(m_putItem) == false )
      {
        g2d->StopAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_VISIBLE_NORMAL );
        g2d->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_VISIBLE_WAZAMACHINE );
      }
    }
    // 通常の道具
    else
    {
      // 名前
      g2d->SetRegisterItemName( 0, id );
      g2d->SetTextBoxPaneStringExpand( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_01, msg_bag_01_01 );

      // アニメ
      if( m_putItem != id && item::ITEM_CheckWazaMachine(m_putItem) != false )
      {
        g2d->StopAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_VISIBLE_WAZAMACHINE );
        g2d->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_VISIBLE_NORMAL );
      }
    }

    // 個数
    if( num == NUM_PUT_NONE )
    {
      g2d->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_02, false );
      g2d->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_03, false );
    }
    else
    {
      u32	pocket = m_itemManager->GetParam( id, item::ITEM_DATA::PRM_ID_F_POCKET );
      if( pocket == BAG_POCKET_EVENT || pocket == BAG_POCKET_WAZA || pocket == BAG_POCKET_BEADS )
      {
        g2d->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_02, false );
        g2d->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_03, false );
      }
      else
      {
        UpdateItemNum( num );
        g2d->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_02, true );
        g2d->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_03, true );
      }
    }

    g2d->StopAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_ITEM_0 );
    g2d->StartAnime( 0, LA_FIELDBAG_FIELDBAG_MIN_UPP_000_ITEM_1 );

    m_putItem = id;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateInfo
 * @brief   アイテム情報表示更新
 * @date    2015.05.07
 *
 * @param   item  アイテムデータ
 */
//-----------------------------------------------------------------------------
void BagInfoField::UpdateInfo( const Savedata::MyItem::ITEM_ST * item )
{
  if( item == NULL )
  {
    UpdateInfo( ITEM_DUMMY_DATA, 0 );
  }
  else
  {
    UpdateInfo( item->id, item->num );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateItemNum
 * @brief   アイテム所持数更新
 * @date    2015.04.15
 *
 * @param   num   所持数
 */
//-----------------------------------------------------------------------------
void BagInfoField::UpdateItemNum( u32 num )
{
  GetG2DUtil()->SetTextBoxPaneNumber(
    0,
    PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_03,
    msg_bag_01_17,
    num,
    3,
    0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePocketName
 * @brief   ポケット名表示更新
 * @date    2015.03.27
 *
 * @param   pocket  ポケット番号
 */
//-----------------------------------------------------------------------------
void BagInfoField::UpdatePocketName( u32 pocket )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  print::WordSet * wset = g2d->GetWordSet();
  wset->RegisterFieldPocketName( 0, pocket );
  g2d->SetTextBoxPaneStringExpand( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_00, msg_bag_01_04 );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePokeItemCategory
 * @brief   カテゴリ表示をポケモンの所持道具に更新
 * @date    2015.05.08
 */
//-----------------------------------------------------------------------------
void BagInfoField::UpdatePokeItemCategory(void)
{
  GetG2DUtil()->SetTextBoxPaneString( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_00, msg_bag_30_01 );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMyGold
 * @brief   所持金表示更新
 * @date    2015.03.27
 *
 * @param   gold  所持金
 */
//-----------------------------------------------------------------------------
void BagInfoField::UpdateMyGold( u32 gold )
{
  GetG2DUtil()->SetTextBoxPaneNumber(
    0,
    PANENAME_FIELDBAG_MIN_UPP_000_PANE_TEXTBOX_06,
    msg_bag_01_27,
    gold,
    Savedata::Misc::MY_GOLD_KETA,
    0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleXButton
 * @brief   Xボタンの表示を切り替え
 * @date    2015.05.15
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagInfoField::SetVisibleXButton( bool flg )
{
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_PARTS_X, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishYButton
 * @brief   Yボタンの表示を切り替え
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagInfoField::VanishYButton(void)
{
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_FIELDBAG_MIN_UPP_000_PANE_PARTS_Y, false );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePocketBg
 * @brief   背景アニメ切り替え
 * @date    2015.08.28
 *
 * @param   pocket  ポケット番号
 */
//-----------------------------------------------------------------------------
void BagInfoField::ChangePocketBg( u32 pocket )
{
  static const u32 tbl[] =
  {
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_07,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_00,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_01,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_03,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_04,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_05,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_06,
    LA_FIELDBAG_FIELDBAG_MIN_UPP_000_CATEGORY_02,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  for( int i=0; i<PlayData::POCKET_DATA_MAX; i++ )
  {
    g2d->StopAnime( 0, tbl[i] );
  }

  g2d->StartAnime( 0, tbl[pocket] );
}


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)
