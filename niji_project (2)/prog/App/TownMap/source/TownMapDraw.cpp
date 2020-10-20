//=============================================================================
/**
 * @file    TownMapDraw.cpp
 * @brief   タウンマップ画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.13
 */
//=============================================================================

// module
#include "TownMapDraw.h"
#include "App/TownMap/include/TownMapDef.h"
// niji
#include "GameSys/include/GameData.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h" 
#include "Sound/include/Sound.h"
// resource
#include "niji_conv_header/app/town_map/TownMapUpp.h"
#include "niji_conv_header/app/town_map/TownMapUpp_pane.h"
#include "niji_conv_header/app/town_map/TownMapUpp_anim_list.h"
#include "niji_conv_header/app/rotom_fade/rotom_fade.h"
#include "niji_conv_header/app/rotom_fade/rotom_fade_anim_list.h"
#include "niji_conv_header/message/msg_townmap.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

//! 島ごとのデータ
const TownMapDraw::LAND_DATA TownMapDraw::LandData[LAND_DATA_MAX] =
{
  {
    Z_F0102_S0101,
    Z_F0102_S0201,
    Z_F0102_S0301,
    Z_F0102_S0401,
    Z_F0102_S0501,
    Z_F0102_S0601,
    Z_F0102_S0701,
    Z_F0102_T0101,
    Z_F0102_C0101,
    Z_F0102_C0102,
    Z_F0102_C0103,
    Z_F0102_S0801,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
  },
  {
    Z_F0201_S0101,
    Z_F0201_S0201,
    Z_F0201_S0301,
    Z_F0201_S0401,
    Z_F0201_S0701,
    Z_F0201_S0801,
    Z_F0201_S0901,
    Z_F0201_S1001,
    Z_F0201_T0201,
    Z_F0201_C0201,
    Z_F0201_C0301,
    Z_F0201_C0401,
    Z_F0201_S1101,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
  },
  {
    Z_F0301_S0101,
    Z_F0301_S0201,
    Z_F0301_S0301,
    Z_F0301_S0401,
    Z_F0301_S0501,
    Z_F0301_S0601,
    Z_F0301_S0701,
    Z_F0301_S0801,
    Z_F0301_S0901,
    Z_F0301_S1001,
    Z_F0301_C0501,
    Z_F0301_C0502,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
  },
  {
    Z_F0401_S0101,
    Z_F0401_S0201,
    Z_F0401_S0301,
    Z_F0401_S0401,
    Z_F0401_S0501,
    Z_F0401_S0601,
    Z_F0401_S0701,
    Z_F0401_T0401,
    Z_F0401_S0801,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
  },
  {
    Z_F0501I0101,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
    ZONE_ID_MAX,
  },
};



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
TownMapDraw::TownMapDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_eventWork( NULL )
  , m_zoneDataLoader( NULL )
  , m_resZoneData( NULL )
  , m_resZoneDataLen( 0 )
  , m_resIconData( NULL )
  , m_resIconDataLen( 0 )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_infoZoneID( ZONE_ID_MAX )
  , m_infoPutCount( 0 )
  , m_isInfoChange( false )
  , m_eventTitle( EVENT_NONE )
  , m_eventInfo( EVENT_NONE )
  , m_isEventChange( false )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
{
  SetInputEnabled( false );
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
TownMapDraw::~TownMapDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.10.30
 */
//-----------------------------------------------------------------------------
void TownMapDraw::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_eventWork      = gd->GetEventWork();
  m_zoneDataLoader = gd->GetFieldZoneDataLoader();
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void TownMapDraw::Update(void)
{
  app::ui::UIView::Update();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.10.13
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void TownMapDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    for( s32 i=0; i<LYTID_MAX; i++ )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, i, gfl2::lyt::ORDER_NORMAL+1 );  // @note TownMapPartsより上に表示する
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTownmapData
 * @brief   タウンマップデータセット
 * @date    2015.10.29
 *
 * @param   zone      ゾーンデータリソース
 * @param   zone_len  ゾーンリソース長
 * @param   icon      アイコンデータリソース
 * @param   icon_len  アイコンリソース長
 */
//-----------------------------------------------------------------------------
void TownMapDraw::SetTownmapData( ZONE_RESOURCE * zone, u32 zone_len, ICON_RESOURCE * icon, u32 icon_len )
{
  m_resZoneData = zone;
  m_resZoneDataLen = zone_len;

  m_resIconData = icon;
  m_resIconDataLen = icon_len;
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.10.13
 *
 * @param   res_buf       レイアウトリソースバッファ
 * @param   rotom_buf     ロトムレイアウトリソースバッファ
 * @param   msg_buf       メッセージリソースバッファ
 * @param   wset_loader   WordSetLoader
 */
//-----------------------------------------------------------------------------
void TownMapDraw::Init( void * res_buf, void * rotom_buf, void * msg_buf, WordSetLoader * wset_loader )
{
  Initialize2D( res_buf, rotom_buf, msg_buf, wset_loader );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.10.13
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  Terminate2D();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2015.10.13
 *
 * @param   res_buf       レイアウトリソースバッファ
 * @param   rotom_buf     ロトムレイアウトリソースバッファ
 * @param   msg_buf       メッセージリソースバッファ
 * @param   wset_loader   WordSetLoader
 */
//-----------------------------------------------------------------------------
void TownMapDraw::Initialize2D( void * res_buf, void * rotom_buf, void * msg_buf, WordSetLoader * wset_loader )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { res_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
    { rotom_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_BFLYT,
      LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_BFLYT,
      LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },

    {
      1,
      LYTRES_ROTOM_FADE_ROTOM_FADE_UPP_000_BFLYT,
      LA_ROTOM_FADE_ROTOM_FADE_UPP_000___NUM,
      rotom_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      1,
      LYTRES_ROTOM_FADE_ROTOM_FADE_LOW_000_BFLYT,
      LA_ROTOM_FADE_ROTOM_FADE_LOW_000___NUM,
      rotom_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
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

  GetG2DUtil()->StartAnime( LYTID_MAP, LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_BG_LOOP_00 );

  WriteDefaultString();
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void TownMapDraw::Terminate2D(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2015.10.13
 *
 * @param   land_no   島番号
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapDraw::Start( u32 land_no )
{
  ChangeLand( land_no );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStart
 * @brief   開始処理が終了したかをチェック
 * @date    2015.10.14
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapDraw::IsStart(void)
{
  if( IsRotom() == false )
  {
    return false;
  }
  SetInputEnabled( true );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultSrring
 * @brief   デフォルト文字列描画
 * @date    2015.10.22
 */
//-----------------------------------------------------------------------------
void TownMapDraw::WriteDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MAP );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  // アローラ地方
  SetTextboxPaneMessage(
    lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_TOWNMAP_MAP_UPP_000_PANE_REGION),  PANENAME_TOWNMAP_PLT_UPP_000_PANE_TEXTBOX_00, &res_id ),
    TOWNMAP_01_02 );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeLand
 * @brief   島選択アニメを変更
 * @date    2015.10.22
 *
 * @param   land_no   島番号
 */
//-----------------------------------------------------------------------------
void TownMapDraw::ChangeLand( u32 land_no )
{
  static const u32 tbl[][2] =
  {
    // on, off
    { LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_01_ON, LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_01_OFF },
    { LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_02_ON, LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_02_OFF },
    { LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_03_ON, LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_03_OFF },
    { LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_04_ON, LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_04_OFF },
    { LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_05_ON, LA_TOWNMAPUPP_TOWNMAP_MAP_UPP_000_LAND_SEL_05_OFF },
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  for( u32 i=0; i<LAND_DATA_MAX; i++ )
  {
    if( i == land_no )
    {
      g2d->StopAnime( LYTID_MAP, tbl[i][1] );
      g2d->StartAnime( LYTID_MAP, tbl[i][0] );
    }
    else
    {
      g2d->StopAnime( LYTID_MAP, tbl[i][0] );
      g2d->StartAnime( LYTID_MAP, tbl[i][1] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetWarpPointMessage
 * @brief   ワープポイントのメッセージを取得
 * @date    2015.10.29
 *
 * @param   id              文字列ID
 * @param   is_place_name   1 = place_name.mstxtから取得
 *
 * @return  文字列
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * TownMapDraw::GetWarpPointMessage( s32 id, u32 is_place_name )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  if( id == PUT_WARPMSG_NONE )
  {
    g2d->GetString( str, TOWNMAP_SKY_000 );
  }
  else if( is_place_name == 0 )
  {
    g2d->SetRegisterWord( 0, id );
    g2d->GetStringExpand( str, TOWNMAP_SKY_001 );
  }
  else
  {
    m_pWordSet->RegisterPlaceName( 0, id );
    g2d->GetStringExpand( str, TOWNMAP_SKY_001 );
  }

  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutZoneInfo
 * @brief   ゾーン情報表示
 * @date    2015.10.29
 *
 * @param   zone_id   ゾーンID
 */
//-----------------------------------------------------------------------------
void TownMapDraw::PutZoneInfo( ZoneID zone_id )
{
  if( m_infoPutCount < INFO_PUT_FRAME )
  {
    m_infoPutCount++;
    return;
  }

  if( m_eventTitle != EVENT_NONE && m_eventInfo != EVENT_NONE )
  {
    VanishZoneInfo();
    return;
  }

  if( zone_id == m_infoZoneID )
  {
    if( m_isInfoChange != false )
    {
      WriteZoneInfo();
      m_isInfoChange = false;
    }
    return;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();

  if( zone_id == ZONE_ID_MAX )
  {
    if( m_infoZoneID != ZONE_ID_MAX )
    {
      g2d->StopAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_IN );
      g2d->StartAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_OUT );
      m_infoZoneID = zone_id;
      m_isInfoChange = false;
    }
  }
  else
  {
    const ZONE_RESOURCE * zone_data = NULL;
    for( u32 i=0; i<m_resZoneDataLen; i++ )
    {
      if( m_resZoneData[i].zone_id == zone_id )
      {
        zone_data = &m_resZoneData[i];
        break;
      }
    }
    if( zone_data == NULL )
    {
      if( m_infoZoneID != ZONE_ID_MAX )
      {
        g2d->StopAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_IN );
        g2d->StartAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_OUT );
        m_infoZoneID = ZONE_ID_MAX;
        m_isInfoChange = false;
      }
    }
    else
    {
      // @note アニメ開始と同時に情報を更新すると見えてしまうため、別フレームで書き換える
      g2d->StopAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_OUT );
      g2d->StartAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_IN );
      m_infoZoneID = zone_id;
      m_isInfoChange = true;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteZoneInfo
 * @brief   ゾーン情報書き込み
 * @date    2015.11.02
 */
//-----------------------------------------------------------------------------
void TownMapDraw::WriteZoneInfo(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( m_infoZoneID == ZONE_ID_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }

  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_INFO );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  const ZONE_RESOURCE * zone_data = NULL;
  for( u32 i=0; i<m_resZoneDataLen; i++ )
  {
    if( m_resZoneData[i].zone_id == m_infoZoneID )
    {
      zone_data = &m_resZoneData[i];
      break;
    }
  }

  if( zone_data == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  { // 場所名
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_TOWNMAP_MIN_UPP_000_PANE_ZONE_NAME );
    Field::ZoneDataAccessor accessor( m_zoneDataLoader->GetZoneData(m_infoZoneID) );
    gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
    u32 str_id = accessor.GetPlaceLabelID();
    // 2行目
    m_pWordSet->RegisterPlaceName( 0, str_id+1 );
    g2d->GetStringExpand( str, TOWNMAP_01_01 );
    // 2行表示
    if( str->GetLength() != 0 )
    {
      lytwk->GetPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXT_1_LINE, &res_id )->SetVisible( false );
      lytwk->GetPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXT_2_LINE, &res_id )->SetVisible( true );
      { // 2行目
        gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXTBOX_02, &res_id );
        g2d->SetTextBoxPaneString( text, str );
      }
      { // 1行目
        gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXTBOX_01, &res_id );
        m_pWordSet->RegisterPlaceName( 0, str_id );
        g2d->SetTextBoxPaneStringExpand( text, TOWNMAP_01_01 );
      }
    }
    // 1行表示
    else
    {
      lytwk->GetPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXT_1_LINE, &res_id )->SetVisible( true );
      lytwk->GetPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXT_2_LINE, &res_id )->SetVisible( false );
      gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_TOWNMAP_PLT_UPP_001_PANE_TEXTBOX_00, &res_id );
      m_pWordSet->RegisterPlaceName( 0, str_id );
      g2d->SetTextBoxPaneStringExpand( text, TOWNMAP_01_01 );
    }
  }

  { // 説明
    gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane(
                                    lytwk->GetPartsPane(PANENAME_TOWNMAP_MIN_UPP_000_PANE_MESSAGE_GUIDE),
                                    PANENAME_TOWNMAP_PLT_UPP_003_PANE_TEXTBOX_00,
                                    &res_id );
    pane->SetVisible( true );
    if( zone_data->info_txt != TM_DUMMY_DATA )
    {
      g2d->SetTextBoxPaneString( pane, zone_data->info_txt );
    }
  }

  // 未到着
  if( zone_data->arrival_flag != TM_DUMMY_DATA && m_eventWork->CheckEventFlag(zone_data->arrival_flag) == false )
  {
    // 絵
    g2d->SetPaneVisible( LYTID_INFO, PANENAME_TOWNMAP_MIN_UPP_000_PANE_ZONE_TEXTURE, false );
    // 施設名
    g2d->SetPaneVisible( LYTID_INFO, PANENAME_TOWNMAP_MIN_UPP_000_PANE_PLACE_LIST, false );
  }
  // 到着
  else
  {
    // 絵（アニメ）
    for( u32 i=0; i<LAND_DATA_MAX; i++ )
    {
      bool is_chg = false;
      for( u32 j=0; j<LAND_ZONE_DATA_MAX; j++ )
      {
        if( LandData[i].zone_id[j] == m_infoZoneID )
        {
          g2d->StartAnimeFrame( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_TEX, static_cast<f32>(i*LAND_ZONE_DATA_MAX+j), false );
          is_chg = true;
          break;
        }
      }
      if( is_chg != false )
      {
        break;
      }
    }
    g2d->SetPaneVisible( LYTID_INFO, PANENAME_TOWNMAP_MIN_UPP_000_PANE_ZONE_TEXTURE, true );

    { // 施設名
      static const gfl2::lyt::LytPaneIndex tbl[] =
      {
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_00,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_01,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_02,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_03,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_04,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_05,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_06,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_07,
        PANENAME_TOWNMAP_PLT_UPP_002_PANE_TEXTBOX_08,
      };

      gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_TOWNMAP_MIN_UPP_000_PANE_PLACE_LIST );

      u32 cnt = 0;
      for( u32 i=0; i<m_resIconDataLen; i++ )
      {
        if( m_resIconData[i].zone_id == m_infoZoneID &&
            ( ( m_resIconData[i].icon_type >= PLACE_TYPE_PC && m_resIconData[i].icon_type <= PLACE_TYPE_SHOP ) || m_resIconData[i].icon_type == PLACE_TYPE_NONE ) &&
            ( m_resIconData[i].arrival_flag == TM_DUMMY_DATA || m_eventWork->CheckEventFlag(m_resIconData[i].arrival_flag) != false ) )
        {
          if( m_resIconData[i].facility_name != TM_DUMMY_DATA )
          {
            if( cnt >= GFL_NELEMS(tbl) )
            {
              GFL_ASSERT( 0 );
              break;
            }

            gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane( parts, tbl[cnt], &res_id );

            if( m_resIconData[i].is_place_name == 0 )
            {
              g2d->SetTextBoxPaneString( pane, m_resIconData[i].facility_name );
            }
            else
            {
              m_pWordSet->RegisterPlaceName( 0, m_resIconData[i].facility_name );
              g2d->SetTextBoxPaneStringExpand( pane, TOWNMAP_01_01 );
            }
            pane->SetVisible( true );
            cnt++;
          }
        }
      }
      for( u32 i=cnt; i<GFL_NELEMS(tbl); i++ )
      {
        gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane( parts, tbl[i], &res_id );
        pane->SetVisible( false );
      }

      if( cnt == 0 )
      {
        parts->SetVisible( false );
      }
      else
      {
        parts->SetVisible( true );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishZoneInfo
 * @brief   ゾーン情報の表示を消す
 * @date    2015.11.03
 */
//-----------------------------------------------------------------------------
void TownMapDraw::VanishZoneInfo(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->BindAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_OUT );
  g2d->StartAnimeFrame(
    LYTID_INFO,
    LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_OUT,
    g2d->GetAnimeMaxFrame(LYTID_INFO,LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_OUT) );
  g2d->StopAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_ZONE_IN );
  m_infoZoneID = ZONE_ID_MAX;
  m_isInfoChange = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutEventInfo
 * @brief   イベント情報表示
 * @date    2015.11.03
 *
 * @param   title   タイトルのメッセージ番号
 * @param   info    情報のメッセージ番号
 *
 * @note  titleとinfoがEVENT_NONEの場合は非表示
 */
//-----------------------------------------------------------------------------
void TownMapDraw::PutEventInfo( u16 title, u16 info )
{
/*
  if( m_infoPutCount < INFO_PUT_FRAME )
  {
    return;
  }

  if( title == m_eventTitle && info == m_eventInfo )
  {
    if( m_isEventChange != false )
    {
      WriteEventInfo();
      m_isEventChange = false;
    }
    return;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();

  if( title == EVENT_NONE || info == EVENT_NONE )
  {
    g2d->StopAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_EVENT_IN );
    g2d->StartAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_EVENT_OUT );
    m_isEventChange = false;
  }
  else
  {
    VanishZoneInfo();
    g2d->StopAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_EVENT_OUT );
    g2d->StartAnime( LYTID_INFO, LA_TOWNMAPUPP_TOWNMAP_MIN_UPP_000_EVENT_IN );
    m_isEventChange = true;
  }

  m_eventTitle = title;
  m_eventInfo  = info;
*/
}


//-----------------------------------------------------------------------------
/**
 * @func    PutEventInfo
 * @brief   イベント情報書き込み
 * @date    2015.11.03
 */
//-----------------------------------------------------------------------------
/*
void TownMapDraw::WriteEventInfo(void)
{
}
*/


//-----------------------------------------------------------------------------
/**
 * @func    StartRotom
 * @brief   ロトムアニメ開始
 * @date    2016.01.12
 */
//-----------------------------------------------------------------------------
void TownMapDraw::StartRotom(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StartAnime( LYTID_UPPER_ROTOM, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 );
  g2d->StartAnime( LYTID_LOWER_ROTOM, LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00 );
  Sound::PlaySE( SEQ_SE_SYS_NJ_028 );
  Sound::PlaySE( SEQ_SE_SYS_NJ_031 );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotom
 * @brief   ロトムアニメ終了チェック
 * @date    2016.01.12
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapDraw::IsRotom(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  if( g2d->IsAnimeEnd( LYTID_UPPER_ROTOM, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 ) == false ||
      g2d->IsAnimeEnd( LYTID_LOWER_ROTOM, LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00 ) == false )
  {
    return false;
  }
  return true;
}


GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)
