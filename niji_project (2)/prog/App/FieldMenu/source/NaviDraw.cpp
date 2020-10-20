//=============================================================================
/**
 * @file    NaviDraw.cpp
 * @brief   フィールド下画面マップ表示
 * @author  Hiroyuki Nakamura
 * @date    2015.05.20
 */
//=============================================================================

// module
#include "App/FieldMenu/include/FieldMenuListener.h"
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "NaviDraw.h"
#include "MenuDefine.h"
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/AppToolMapManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/EventList/EventListDataManager.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h" 
#include "Sound/include/Sound.h"
// resource
#include "arc_index/message.gaix"
#include "niji_conv_header/app/field_menu/Navi.h"
#include "niji_conv_header/app/field_menu/Navi_pane.h"
#include "niji_conv_header/app/field_menu/Navi_anim_list.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap          ヒープ
 * @param   render_man    描画マネージャ
 * @param   is_open       メニューを開いているか
 * @param   mode          表示モード
 * @param   center_pane   マップ中央のペイン
 */
//-----------------------------------------------------------------------------
NaviDraw::NaviDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const bool * is_open, const bool * is_passive, const s32 * mode, const gfl2::lyt::LytPane * center_pane )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_dispMode( mode )
  , m_frameListener( NULL )
  , m_isMenuOpen( is_open )
  , m_isPassive( is_passive )
  , m_gameManager( NULL )
  , m_fileManager( NULL )
  , m_saveFieldMenu( NULL )
  , m_saveMisc( NULL )
  , m_myStatus( NULL )
  , m_zoneDataLoader( NULL )
  , m_configSave( NULL )
  , m_pLytResBuff( NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
  , m_pMapManager( NULL )
  , m_pEventListDataManager( NULL )
  , m_mainEventID( EVENT_ID_NONE )
  , m_mainEventIndex( 0 )
  , m_posIconMode( MODE_HERO_ICON )
{
  SetInputEnabled( false );   // デフォルトがONなのでOFFにしておく
  GetGameData();
  InitMapCenterPane( center_pane );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
NaviDraw::~NaviDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ関連取得
 * @date    2015.09.01
 */
//-----------------------------------------------------------------------------
void NaviDraw::GetGameData(void)
{
  m_gameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  m_fileManager = m_gameManager->GetAsyncFileManager();

  GameSys::GameData * gd = m_gameManager->GetGameData();

  m_saveFieldMenu  = gd->GetFieldMenu();
  m_saveMisc       = gd->GetMisc();
  m_myStatus       = gd->GetPlayerStatus();
  m_zoneDataLoader = gd->GetFieldZoneDataLoader();
  m_configSave     = gd->GetConfig();
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期化
 * @date    2015.07.14
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NaviDraw::Initialize(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateEventListData() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    Create2D();
    CreateMapManager();
    m_mainSeq++;
    /* FALL THROUGH */
  case 3:
    if( m_pMapManager->Initialize() == false )
    {
      break;
    }
    InitMapManagerIcon();
    m_mainSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了処理
 * @date    2015.07.14
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NaviDraw::Terminate(void)
{
  if( DeleteMapManager() == false )
  {
    return false;
  }
  Delete2D();
  DeleteEventListData();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   アップデート
 * @date    2015.05.25
 */
//-----------------------------------------------------------------------------
void NaviDraw::Update(void)
{
  app::ui::UIView::Update();
  if( *m_dispMode == DISP_MODE_NORMAL )
  {
    UpdateMapPosition();
    m_pMapManager->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.05.25
 *
 * @param   displayNo   描画ディスプレイ番号
 */
//-----------------------------------------------------------------------------
void NaviDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( *m_dispMode == DISP_MODE_NORMAL )
  {
    m_pMapManager->Draw( m_renderingManager, displayNo, DRAW_PRI_LOWER_NAVI );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Create2D
 * @brief   2D関連生成
 * @date    2015.09.04
 */
//-----------------------------------------------------------------------------
void NaviDraw::Create2D(void)
{
  UIView::Create2D(
    m_heap,
    NULL,
    1,
    NULL,
    0,
    NULL,
    0,
    app::ui::UIView::UNUSE_MESSAGE_DATA );
}

//-----------------------------------------------------------------------------
/**
 * @func    Delete2D
 * @brief   2D関連削除
 * @date    2015.09.04
 */
//-----------------------------------------------------------------------------
void NaviDraw::Delete2D(void)
{
  UIView::Delete2D();
}


//-----------------------------------------------------------------------------
/**
 * @func    SetFrameListener
 * @brief   リスナーをセット
 * @date    2015.05.22
 *
 * @param   listener  リスナー
 */
//-----------------------------------------------------------------------------
void NaviDraw::SetFrameListener( FrameListener * listener )
{
  m_frameListener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始
 * @date    2015.05.25
 */
//-----------------------------------------------------------------------------
void NaviDraw::Start(void)
{
  m_pMapManager->SetVisible( true );
  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    Stop
 * @brief   停止
 * @date    2015.05.25
 */
//-----------------------------------------------------------------------------
void NaviDraw::Stop(void)
{
  m_pMapManager->SetVisible( false );
  SetInputListener( NULL );
}

//-----------------------------------------------------------------------------
/**
 * @func    OpenMenu
 * @brief   フィールドメニューが開いた時の処理
 * @date    2015.06.09
 */
//-----------------------------------------------------------------------------
void NaviDraw::OpenMenu(void)
{
  if( *m_dispMode == DISP_MODE_NORMAL )
  {
    m_pMapManager->SetVisible( false );
    SetInputListener( NULL );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseMenu
 * @brief   フィールドメニューが閉じた時の処理
 * @date    2015.06.09
 */
//-----------------------------------------------------------------------------
void NaviDraw::CloseMenu(void)
{
  if( *m_dispMode == DISP_MODE_NORMAL )
  {
    m_pMapManager->SetVisible( true );
    SetInputListener( this );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMapManager
 * @brief   マップマネージャ生成
 * @date    2016.02.23
 */
//-----------------------------------------------------------------------------
void NaviDraw::CreateMapManager(void)
{
  m_pMapManager = GFL_NEW(m_heap->GetSystemHeap()) App::Tool::MapManager( m_heap, GetG2DUtil(), true );
  m_pMapManager->SetPaneSizeNavi();
  {
    u32 map_id = 0;
    f32 x = 0.0f;
    f32 y = 0.0f;

    Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();

    if( fieldmap != NULL )
    {
      Field::ZoneID zone_id = fieldmap->GetZoneID();
      Field::ZoneDataAccessor accessor( m_zoneDataLoader->GetZoneData(zone_id) );

      f32 r;
      bool is_arrow;
      GetMapData( &map_id, &x, &y, &r, &is_arrow, zone_id, &accessor );
    }
    m_pMapManager->Setup( map_id, x, y );
  }
  m_pMapManager->SetupPlayerSex( m_myStatus->GetSex() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMapManager
 * @brief   マップマネージャ削除
 * @date    2016.02.23
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NaviDraw::DeleteMapManager(void)
{
  if( m_pMapManager != NULL )
  {
    if( m_pMapManager->Terminate() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pMapManager );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMapManagerIcon
 * @brief   マップマネージャのアイコン初期化
 * @date    2015.09.07
 */
//-----------------------------------------------------------------------------
void NaviDraw::InitMapManagerIcon(void)
{
  m_pMapManager->SetupEventArea();

  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();

  if( fieldmap != NULL )
  {
    Field::ZoneDataAccessor accessor( m_zoneDataLoader->GetZoneData(fieldmap->GetZoneID()) );

    // イベントアイコン
    PutMainEventIcon( m_pEventListDataManager->GetTargetIndexForMain(), &accessor );

    // 主人公アイコン
    if( accessor.IsSubscreenOutsideMapMode() != false )
    {
      gfl2::math::Vector3 vec = fieldmap->GetPlayerPosition();
      m_pMapManager->PutHeroIcon( vec.GetX(), vec.GetZ(), GetHeroRotate() );
      if( m_posIconMode == MODE_HERO_ICON )
      {
        m_pMapManager->SetVisibleHeroArrow( true );
      }
    }
    else
    {
      f32 x, y;
//      m_saveFieldMenu->GetMapPosition( &x, &y );
      x = accessor.GetSubscreenBasisPositionX();
      y = accessor.GetSubscreenBasisPositionZ();
      m_pMapManager->PutHeroIcon( x, y, 0.0f );
      m_pMapManager->SetVisibleHeroArrow( false );
    }
  }

  m_pMapManager->SetVisibleHeroIcon( true );

  if( m_configSave->GetButtonMode() == Savedata::ConfigSave::BUTTON_MODE_L_EQUAL_A )
  {
    m_pMapManager->VanishFinderButtonPane();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMapPosition
 * @brief   マップ表示更新
 * @date    2015.09.01
 */
//-----------------------------------------------------------------------------
void NaviDraw::UpdateMapPosition(void)
{
  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();

  if( fieldmap != NULL && *m_isPassive == false )
  {
    Field::ZoneID zone_id = fieldmap->GetZoneID();
    Field::ZoneDataAccessor accessor( m_zoneDataLoader->GetZoneData(zone_id) );

    u32 map_id;
    f32 x, y, r;
    bool is_arrow;

    GetMapData( &map_id, &x, &y, &r, &is_arrow, zone_id, &accessor );

    m_pMapManager->UpdatePosition( map_id, x, y );
    m_pMapManager->PutHeroIcon( x, y, r );
    m_pMapManager->SetVisibleHeroArrow( is_arrow );

    // イベントアイコン
    PutMainEventIcon( m_pEventListDataManager->GetTargetIndexForMain(), &accessor );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMapData
 * @brief   マップデータ取得
 * @date    2016.02.17
 *
 * @param   map_id    マップID格納場所
 * @param   x         X座標格納場所
 * @param   y         Y座標格納場所
 * @param   r         向き格納場所
 * @param   is_arrow  向きの矢印を表示するかの格納場所
 * @param   zone_id   ゾーンID
 * @param   accessor  ゾーンデータアクセサー
 */
//-----------------------------------------------------------------------------
void NaviDraw::GetMapData( u32 * map_id, f32 * x, f32 * y, f32 * r, bool * is_arrow, Field::ZoneID zone_id, Field::ZoneDataAccessor * accessor )
{
  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();  // @note NULLチェックはこの関数を使用する箇所でやる

  *is_arrow = false;

  *map_id = m_pMapManager->GetMapDataIndex( static_cast<SubScreenMapType>(accessor->GetSubscreenMapType()) );  

  if( accessor->IsSubscreenOutsideMapMode() != false )
  {
    gfl2::math::Vector3 vec = fieldmap->GetPlayerPosition();
    *x = vec.GetX();
    *y = vec.GetZ();

    *r = GetHeroRotate();

    if( m_posIconMode == MODE_HERO_ICON )
    {
      *is_arrow = true;
    }
  }
  else
  {
    *x = accessor->GetSubscreenBasisPositionX();
    *y = accessor->GetSubscreenBasisPositionZ();
    *r = 0.0f;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateEventListData
 * @brief   イベントリストデータ生成
 * @date    2015.09.07
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NaviDraw::CreateEventListData(void)
{
  switch( m_subSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase * heap = m_heap->GetSystemHeap();
      m_pEventListDataManager = GFL_NEW(heap) Field::EventList::EventListDataManager( m_gameManager, heap, m_fileManager );
    }
    m_pEventListDataManager->Initialize( Field::EventList::EventListDataManager::EVENT_LIST_DATA_TYPE_APP );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pEventListDataManager->InitializeWait() == false )
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
 * @func    DeleteEventListData
 * @brief   イベントリストデータ削除
 * @date    2015.09.07
 */
//-----------------------------------------------------------------------------
void NaviDraw::DeleteEventListData(void)
{
  m_pEventListDataManager->Terminate();
  GFL_SAFE_DELETE( m_pEventListDataManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutMainEventIcon
 * @brief   メインイベントのアイコンを配置
 * @date    2015.09.17
 *
 * @param   accessor  ゾーンデータアクセサー
 */
//-----------------------------------------------------------------------------
void NaviDraw::PutMainEventIcon( s32 id, Field::ZoneDataAccessor * accessor )
{
  if( id == -1 )
  {
    m_pMapManager->SetVisibleEventIcon( App::Tool::MapManager::EVENT_ICON_ID_MAIN, false );
    m_mainEventID = EVENT_ID_NONE;
  }
  else
  {
    const Field::EventList::EventListData * list_data = m_pEventListDataManager->GetMainList( id );
    u32 index = list_data->GetCurrentIndex();

    bool is_put_map = false;
    { // 表示できるマップかをチェック
      static const SubScreenMapType tbl[] =
      {
        SUBSCREEN_MAP_TYPE_W_F0102,
        SUBSCREEN_MAP_TYPE_W_F0201,
        SUBSCREEN_MAP_TYPE_W_F0301,
        SUBSCREEN_MAP_TYPE_W_F0401,
        SUBSCREEN_MAP_TYPE_W_F0501I0101,
      };
      SubScreenMapType type = static_cast<SubScreenMapType>( accessor->GetSubscreenMapType() );
      for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
      {
        if( type == tbl[i] )
        {
          is_put_map = true;
          break;
        }
      }
    }

    if( is_put_map == false )
    {
      m_pMapManager->SetVisibleEventIcon( App::Tool::MapManager::EVENT_ICON_ID_MAIN, false );
    }
    else
    {
      const Field::EventList::EventListData::DetailField * data = list_data->GetDetailFieldCurrent();
      if( data->iland_id == accessor->GetIslandIndex() )
      {
        m_pMapManager->SetVisibleEventIcon( App::Tool::MapManager::EVENT_ICON_ID_MAIN, true );
        m_pMapManager->PutEventIcon( App::Tool::MapManager::EVENT_ICON_ID_MAIN, data->pos_x, data->pos_z );
      }
      else
      {
        m_pMapManager->SetVisibleEventIcon( App::Tool::MapManager::EVENT_ICON_ID_MAIN, false );
      }
    }

    m_mainEventID    = id;
    m_mainEventIndex = index;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetHeroRotate
 * @brief   主人公の向きを取得
 * @date    2015.09.11
 *
 * @return  主人公の向き
 */
//-----------------------------------------------------------------------------
f32 NaviDraw::GetHeroRotate(void)
{
  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();

  if( fieldmap == NULL )
  {
    return 0.0f;
  }

  f32 x, y, z;
  fieldmap->GetPlayerQuaternion().QuaternionToRadianYXZ( &x, &y, &z );

  f32 rotate = gfl2::math::ConvRadToDeg( z );
  if( y == 0.0f )
  {
    rotate += 180.0f;
  }
  else
  {
    rotate = 360.0f - rotate;
    if( rotate >= 360.0f )
    {
      rotate -= 360.0f;
    }
  }
  return rotate;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMainEventMsgID
 * @brief   メインイベントのメッセージIDを取得
 * @date    2015.09.17
 *
 * @retrun  メッセージID
 */
//-----------------------------------------------------------------------------
u32 NaviDraw::GetMainEventMsgID(void)
{
  if( m_mainEventID != EVENT_ID_NONE )
  {
    const Field::EventList::EventListData::DetailField * data = m_pEventListDataManager->GetMainList(m_mainEventID)->GetDetailFieldCurrent();
    return data->msg_rotom_info;
  }
  return EVENT_MSG_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMainEventWorkValue
 * @brief   メインイベントのワーク値を取得
 * @date    2015.11.20
 *
 * @return  ワーク値
 */
//-----------------------------------------------------------------------------
u32 NaviDraw::GetMainEventWorkValue(void)
{
  if( m_mainEventID != EVENT_ID_NONE )
  {
    const Field::EventList::EventListData::DetailInfo * info = m_pEventListDataManager->GetMainList(m_mainEventID)->GetDetailInfoCurrent();
    if( info->work_value >= Savedata::Misc::EVENT_LIST_BIT_MAX )
    {
      return PlayData::ROTOM_WORK_NONE;
    }
    return info->work_value;
  }
  return PlayData::ROTOM_WORK_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMapCenterPane
 * @brief   マップ中央のペインを初期化
 * @date    2016.01.13
 *
 * @param   pane  ペイン
 */
//-----------------------------------------------------------------------------
void NaviDraw::InitMapCenterPane( const gfl2::lyt::LytPane * pane )
{
  for( u32 i=0; i<MAP_CENTER_PANE_MAX; i++ )
  {
    m_centerPane[i] = NULL;
  }

  const gfl2::lyt::LytPane * parent = NULL;

  for( u32 i=0; i<MAP_CENTER_PANE_MAX; i++ )
  {
    parent = pane->GetParent();
    if( parent == NULL )
    {
      break;
    }
    m_centerPane[i] = pane;
    pane = parent;
  }

  // ペインが取得できていない
  if( m_centerPane[0] == NULL )
  {
    GFL_ASSERT( 0 );
    m_centerPane[0] = pane;
    return;
  }
  // ルートペインまで取得できていない
  if( pane->GetParent() != NULL )
  {
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveMapCenter
 * @brief   マップの中心を移動
 * @date    2016.01.12
 */
//-----------------------------------------------------------------------------
void NaviDraw::MoveMapCenter(void)
{
  if( m_pMapManager != NULL )
  {
    gfl2::math::VEC2 s = m_centerPane[0]->GetScale();
    gfl2::math::VEC3 r = m_centerPane[0]->GetRotate();
    gfl2::math::VEC3 t = m_centerPane[0]->GetTranslate();
    for( u32 i=1; i<MAP_CENTER_PANE_MAX; i++ )
    {
      if( m_centerPane[i] == NULL )
      {
        break;
      }
      {
        gfl2::math::VEC2 tmp_s = m_centerPane[i]->GetScale();
        s.x *= tmp_s.x;
        s.y *= tmp_s.y;
      }
      {
        gfl2::math::VEC3 tmp_r = m_centerPane[i]->GetRotate();
        r += tmp_r;
      }
      {
        gfl2::math::VEC3 tmp_t = m_centerPane[i]->GetTranslate();
        t += tmp_t;
      }
    }
    m_pMapManager->SetCenterSRT( &s, &r, &t );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePosIcon
 * @brief   現在地アイコンの表示モードを変更
 * @date    2016.01.15
 *
 * @param   icon  表示モード
 */
//-----------------------------------------------------------------------------
void NaviDraw::ChangePosIcon( PosIconMode icon )
{
  if( m_pMapManager != NULL && m_posIconMode != icon )
  {
    switch( icon )
    {
    case MODE_HERO_ICON:
      m_pMapManager->ChangeHeroIcon( App::Tool::MapManager::HERO_ICON_ID_PLAYER );
      break;
    case MODE_FINDER_NORMAL:
      m_pMapManager->ChangeHeroIcon( App::Tool::MapManager::HERO_ICON_ID_FINDER );
      m_pMapManager->SetVisibleHeroArrow( false );
      Sound::PlaySE( SEQ_SE_ICON );
      break;
    case MODE_FINDER_RARE:
      m_pMapManager->ChangeHeroIcon( App::Tool::MapManager::HERO_ICON_ID_FINDER_RARE );
      m_pMapManager->SetVisibleHeroArrow( false );
      Sound::PlaySE( SEQ_SE_ICON );
      break;
    }

    m_posIconMode = icon;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFinderIcon
 * @brief   現在地アイコンの表示モードがファインダーか
 * @date    2016.01.15
 *
 * @retval  true  = ファインダー
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NaviDraw::IsFinderIcon(void)
{
  if( m_posIconMode == MODE_FINDER_NORMAL || m_posIconMode == MODE_FINDER_RARE )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
