//=============================================================================
/**
 * @file    AppToolMapManager.h
 * @brief   2D分割マップ関連処理
 * @author  Hiroyuki Nakamura
 * @date    2015.08.25
 */
//=============================================================================

// module
#include "AppLib/include/Tool/AppToolMapManager.h"
// niji
#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/AppRenderingManager.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/NaviMap.gaix"
#include "arc_index/FieldMenu.gaix"
#include "niji_conv_header/app/field_menu/Navi.h"
#include "niji_conv_header/app/field_menu/Navi_pane.h"
#include "niji_conv_header/app/field_menu/Navi_anim_list.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

#include "MapManagerData.cdat"

const f32 MapManager::MAP_SCALE_H = ( static_cast<f32>(MAP_2D_DEFAULT_X_SIZE*2) ) / static_cast<f32>(MAP_3D_DEFAULT_X_SIZE);   //!< マップ横拡縮値
const f32 MapManager::MAP_SCALE_V = ( static_cast<f32>(MAP_2D_DEFAULT_Y_SIZE*2) ) / static_cast<f32>(MAP_3D_DEFAULT_Y_SIZE);   //!< マップ縦拡縮値
const f32 MapManager::FIELD_SCALE_H = static_cast<f32>(MAP_3D_DEFAULT_X_SIZE) / ( static_cast<f32>(MAP_2D_DEFAULT_X_SIZE*2) ); //!< フィールド横拡縮値
const f32 MapManager::FIELD_SCALE_V = static_cast<f32>(MAP_3D_DEFAULT_Y_SIZE) / ( static_cast<f32>(MAP_2D_DEFAULT_Y_SIZE*2) ); //!< フィールド縦拡縮値

//! イベントアイコンデータ
const MapManager::EventIcon MapManager::EventIconTable[MapManager::EVENT_ICON_ID_MAX] =
{
  {
    PANENAME_FIELDMENU_MAP_LOW_001_PANE_EVENTICON_00,
//    LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_EVENT_02,
//    LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_EVENT_00,
  },
};

const f32 MapManager::HERO_ROTATE_MOVE_MAX = 45.0f;   //!< 主人公の向きを１フレームで変更できる最大角度


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap      ヒープ
 * @param   g2d       G2DUtil
 * @param   is_navi   true = ナビ画面, false = それ以外
 */
//-----------------------------------------------------------------------------
MapManager::MapManager( app::util::Heap * heap, app::util::G2DUtil * g2d, bool is_navi )
  : m_heap( heap )
  , m_g2dUtil( g2d )
  , m_isNaviMode( is_navi )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
  , m_pLytResBuff( NULL )
  , m_isVisible( false )
  , m_lytsys( NULL )
  , m_pLytWk( NULL )
  , m_centerPane( NULL )
  , m_pTexReplaceSystem( NULL )
  , m_isTexReplace( false )
  , m_paneWidth( PANE_WIDTH )
  , m_paneHeight( PANE_HEIGHT )
  , m_panePutLimitUp( PANE_PUT_LIMIT_UP )
  , m_panePutLimitDown( PANE_PUT_LIMIT_DOWN )
  , m_panePutLimitRight( PANE_PUT_LIMIT_RIGHT )
  , m_panePutLimitLeft( PANE_PUT_LIMIT_LEFT )
  , m_mapScaleH( MAP_SCALE_H )
  , m_mapScaleV( MAP_SCALE_V )
  , m_fieldScaleH( FIELD_SCALE_H )
  , m_fieldScaleV( FIELD_SCALE_V )
  , m_mapID( 0 )
  , m_centerX( 0.0f )
  , m_centerY( 0.0f )
  , m_centerMinX( 0.0f )
  , m_centerMinY( 0.0f )
  , m_centerMaxX( 0.0f )
  , m_centerMaxY( 0.0f )
  , m_evLimUp( static_cast<f32>( DISPLAY_LOWER_HEIGHT / 2 ) )
  , m_evLimDown( -static_cast<f32>( DISPLAY_LOWER_HEIGHT / 2 ) )
  , m_evLimLeft( -static_cast<f32>( DISPLAY_LOWER_WIDTH / 2 ) )
  , m_evLimRight( static_cast<f32>( DISPLAY_LOWER_WIDTH / 2 ) )
  , m_isInitialize( false )
  , m_isTerminate( false )
  , m_playerSex( PM_MALE )
  , m_isScaleLinear( false )
  , m_appScaleA( 1 )
  , m_appScaleB( 1 )
{
  for( u32 i=0; i<PANE_MAX; i++ )
  {
    m_mapPane[i] = NULL;
  }
  gfl2::std::MemClear( m_texRepWork, sizeof(TexRepWork)*PANE_MAX );
  gfl2::std::MemClear( &m_mapData, sizeof(MAP_DATA) );
  gfl2::std::MemClear( m_eventIconWork, sizeof(EventIconWork)*EVENT_ICON_ID_MAX );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MapManager::~MapManager()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.08.26
 */
//-----------------------------------------------------------------------------
void MapManager::Update(void)
{
  if( m_pLytWk != NULL )
  {
    UpdateTextureReplace();
//    UpdateEventIcon();
    m_pLytWk->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.08.26
 *
 * @param   render_man  描画マネージャ
 * @param   disp_no     ディスプレイ番号
 * @param   order       描画順
 * @param   layer_no    レイヤー番号
 */
//-----------------------------------------------------------------------------
void MapManager::Draw( app::util::AppRenderingManager * render_man, gfl2::gfx::CtrDisplayNo disp_no, u8 order, u32 layer_no )
{
  if( m_isVisible == false || m_isTexReplace == false )
  {
    return;
  }

  if( disp_no == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    render_man->AddDrawableLytWkForOneFrame(
      app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pLytWk, layer_no, order );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisible
 * @brief   表示切り替え
 * @date    2015.08.26
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MapManager::SetVisible( bool flg )
{
  m_isVisible = flg;
}


//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期化
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::Initialize(void)
{
  if( m_isInitialize != false )
  {
    return true;
  }

  switch( m_mainSeq )
  {
  case 0:
    CreateTextureReplaceSystem();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( CreateLayout() == false )
    {
      break;
    }
    InitMapPane();
    m_mainSeq++;
    /* FALL THROUGH */
  case 3:
    if( OpenTextureFile() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 4:
    // ナビモードのみ初期表示
    if( m_isNaviMode != false )
    {
      InitPanePos();
    }
    InitIcon();
    m_isInitialize = true;
    m_mainSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitializeSync
 * @brief   初期化（同期版）
 * @date    2016.02.23
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::InitializeSync(void)
{
  if( m_isInitialize != false )
  {
    return true;
  }

  CreateTextureReplaceSystem();
  LoadLayoutResourceSync();
  CreateLayout();
  InitMapPane();
  OpenTextureFileSync();
  // ナビモードのみ初期表示
  if( m_isNaviMode != false )
  {
    InitPanePos();
  }
  InitIcon();
  m_isInitialize = true;
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了処理
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::Terminate(void)
{
  if( m_isTerminate != false )
  {
    return true;
  }

  switch( m_mainSeq )
  {
  case 0:
    // 初期化されてない
    if( m_isInitialize == false )
    {
      GFL_ASSERT( 0 );
      return true;
    }
    SetVisible( false );
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsTextureReplaceSystemFree() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( CloseTextureFile() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 3:
    if( DeleteLayout() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 4:
    if( UnloadLayoutResource() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 5:
    DeleteTextureReplaceSystem();
    m_isTerminate = true;
    m_mainSeq = 0;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_FIELD_MENU, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( ARCID_FIELD_MENU ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_FIELD_MENU,
      GARC_FieldMenu_Navi_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_FIELD_MENU, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( ARCID_FIELD_MENU ) == false )
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
 * @func    LoadLayoutResourceSync
 * @brief   レイアウトリソース読み込み（同期版）
 * @date    2016.02.23
 */
//-----------------------------------------------------------------------------
void MapManager::LoadLayoutResourceSync(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_FIELD_MENU, m_heap->GetSystemHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_FIELD_MENU,
    GARC_FieldMenu_Navi_applyt_COMP,
    &m_pLytResBuff,
    m_heap->GetDeviceHeap(),
    true,
    nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
  app::util::FileAccessor::FileCloseSync( ARCID_FIELD_MENU );
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayoutResource
 * @brief   レイアウトリソース削除
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetLayoutResource
 * @brief   レイアウトリソース取得
 * @date    2015.09.04
 *
 * @return  レイアウトリソース
 */
//-----------------------------------------------------------------------------
void * MapManager::GetLayoutResource(void)
{
  return m_pLytResBuff;
}


//-----------------------------------------------------------------------------
/**
 * @func    OpenTextureFile
 * @brief   マップテクスチャファイルを開く
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::OpenTextureFile(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_NAVI_MAP, m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( ARCID_NAVI_MAP ) == false )
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
 * @func    OpenTextureFileSync
 * @brief   マップテクスチャファイルを開く（同期版）
 * @date    2016.02.23
 */
//-----------------------------------------------------------------------------
void MapManager::OpenTextureFileSync(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_NAVI_MAP, m_heap->GetDeviceHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseTextureFile
 * @brief   マップテクスチャファイルを閉じる
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::CloseTextureFile(void)
{
  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileClose( ARCID_NAVI_MAP, m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileClose( ARCID_NAVI_MAP ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;   // @fix GFNMCat[5690]: 完了していないのに完了を返していたので修正
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト生成
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::CreateLayout(void)
{
  app::util::AppLytAccessor * accessor = m_g2dUtil->GetAppLytAccessor();
  accessor->SetAppLytBuf( m_pLytResBuff );

  m_lytsys = m_g2dUtil->GetLayoutSystem();
  m_resID  = m_lytsys->InitMultiResource( m_heap->GetDeviceHeap(), 1 );
  m_resID  = m_lytsys->LoadLytRes( &m_resID, m_heap->GetDeviceAllocator(), accessor->GetLytDat(), false, 0 );

  u32 def_anm_max = accessor->GetAnimeMax( LYTRES_NAVI_FIELDMENU_MAP_LOW_001_BFLYT );

  m_pLytWk = m_lytsys->CreateLytWk(
              &m_resID,
              LYTRES_NAVI_FIELDMENU_MAP_LOW_001_BFLYT,
              accessor->GetAnimeTable(LYTRES_NAVI_FIELDMENU_MAP_LOW_001_BFLYT),
              def_anm_max,
              LA_NAVI_FIELDMENU_MAP_LOW_001___NUM,
              m_g2dUtil->GetLayoutWorkSetup(gfl2::lyt::DISPLAY_LOWER) );

  m_g2dUtil->CreatePartsAnime( m_pLytWk, &m_resID, LYTRES_NAVI_FIELDMENU_MAP_LOW_001_BFLYT, &def_anm_max, NULL, 0 );

  m_pLytWk->SetInvisiblePaneCalculateMtx( false );
  m_pLytWk->SetAutoDrawFlag( false );
  SetVisible( false );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteLayout
 * @brief   レイアウト削除
 * @date    2015.08.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::DeleteLayout(void)
{
  if( m_pLytWk == NULL )
  {
    return true;
  }

  if( m_pLytWk->IsDrawing() == false )
  {
    m_lytsys->RemoveLytWk( m_pLytWk );
    m_pLytWk = NULL;

    if( m_resID.IsEnable() != false )
    {
      m_lytsys->ReleaseLytRes( &m_resID );
    }
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetLayoutResourceID
 * @brief   レイアウトリソースID取得
 * @date    2015.09.04
 *
 * @return  レイアウトリソースID
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytMultiResID * MapManager::GetLayoutResourceID(void)
{
  return &m_resID;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMapPane
 * @brief   マップのペインを取得
 * @date    2015.12.02
 */
//-----------------------------------------------------------------------------
void MapManager::InitMapPane(void)
{
  static const gfl2::lyt::LytPaneIndex tbl[PANE_MAX] =
  {
    PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_00, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_01, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_02, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_03,
    PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_10, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_11, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_12, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_13,
    PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_20, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_21, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_22, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_23,
    PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_30, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_31, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_32, PANENAME_FIELDMENU_MAP_LOW_001_PANE_MAP_33,
  };

  nw::lyt::Size size( static_cast<f32>(m_paneWidth), static_cast<f32>(m_paneHeight) );

  for( u32 i=0; i<PANE_MAX; i++ )
  {
    m_mapPane[i] = m_pLytWk->GetPicturePane( tbl[i] );
    m_mapPane[i]->SetSize( size );
    if( m_isScaleLinear != false )
    {
      m_mapPane[i]->GetMaterial()->SetTexMapFilter( 0, nw::lyt::TEXFILTER_LINEAR, nw::lyt::TEXFILTER_LINEAR );
    }
  }

  m_centerPane = m_pLytWk->GetPane( PANENAME_FIELDMENU_MAP_LOW_001_PANE_ALL );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateTextureReplaceSystem
 * @brief   テクスチャ管理システム生成
 * @date    2015.08.26
 */
//-----------------------------------------------------------------------------
void MapManager::CreateTextureReplaceSystem(void)
{
	m_pTexReplaceSystem = GFL_NEW(m_heap->GetSystemHeap()) app::tool::LytTexReplaceSystem( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), TEX_SIZE, PANE_MAX );
  for( u32 i=0; i<PANE_MAX; i++ )
  {
    m_pTexReplaceSystem->AllocTextureBuffer( i );
    // ワーク初期化
    m_texRepWork[i].seq = 0;
    m_texRepWork[i].tex_id = TEXREP_REQ_ID_NONE;
    m_texRepWork[i].req_id = TEXREP_REQ_ID_NONE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteTextureReplaceSystem
 * @brief   テクスチャ管理システム削除
 * @date    2015.08.26
 */
//-----------------------------------------------------------------------------
void MapManager::DeleteTextureReplaceSystem(void)
{
  GFL_SAFE_DELETE( m_pTexReplaceSystem );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsTextureReplaceSystemFree
 * @brief   テクスチャ管理システムを削除できるか
 * @date    2015.09.10
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MapManager::IsTextureReplaceSystemFree(void)
{
  if( m_pTexReplaceSystem == NULL )
  {
    return true;
  }
  return m_pTexReplaceSystem->IsModuleFree();
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateTextureReplace
 * @brief   マップテクスチャ更新（全体）
 * @date    2015.08.27
 */
//-----------------------------------------------------------------------------
void MapManager::UpdateTextureReplace(void)
{
  bool is_rep = true;

  for( u32 i=0; i<PANE_MAX; i++ )
  {
    if( UpdateTextureReplace( i ) == false )
    {
      if( IsDispAreaPane( i ) != false )
      {
        is_rep = false;
      }
    }
  }

  m_isTexReplace = is_rep;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateTextureReplace
 * @brief   マップテクスチャ更新（個別）
 * @date    2015.08.27
 *
 * @param   id  ワークID
 *
 * @retval  false = 読み込み中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::UpdateTextureReplace( u32 id )
{
  TexRepWork * wk = &m_texRepWork[id];

  if( wk->req_id == TEXREP_REQ_ID_NONE )
  {
    return true;
  }

  switch( wk->seq )
  {
  case 0:
    m_pTexReplaceSystem->LoadTextureRequestCore(
      m_heap->GetSystemHeap(),
      id,
      ARCID_NAVI_MAP,
      wk->req_id,
      false );
    wk->tex_id = wk->req_id;
    wk->seq = 1;
    /* FALL THROUGH */
  case 1:
    if( m_pTexReplaceSystem->IsLoadTextureFinished( id ) == false )
    {
      break;
    }
    // 新しいリクエストが発行されていないか
    wk->seq = 0;
    if( wk->tex_id == wk->req_id )
    {
      gfl2::lyt::LytPicture * pane = GetMapPane( id );
      m_pTexReplaceSystem->LoadTexture( id );
      m_pTexReplaceSystem->ReplaceTexture( id, pane , 0, 0 );
      pane->SetVisible( true );
      wk->req_id = TEXREP_REQ_ID_NONE;
      return true;
    }
    break;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestTextureReplace
 * @brief   マップテクスチャ変更リクエスト
 * @date    2015.08.27
 *
 * @param   buf_id  ワークID
 * @param   tex_id  テクスチャID
 */
//-----------------------------------------------------------------------------
void MapManager::SetRequestTextureReplace( u32 buf_id, u32 tex_id )
{
  if( tex_id >= (m_mapData.tex_id+m_mapData.tex_width*m_mapData.tex_height) ||
      tex_id < m_mapData.tex_id )
  {
    return;
  }

  TexRepWork * wk = &m_texRepWork[buf_id];

  if( wk->tex_id != tex_id )
  {
    wk->req_id = tex_id;
    GetMapPane(buf_id)->SetVisible( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMapTextureReplace
 * @brief   画面内のテクスチャを変更中か
 * @date    2015.09.05
 *
 * @retval  true  = 変更中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::IsMapTextureReplace(void)
{
  return m_isTexReplace;
}


//-----------------------------------------------------------------------------
/**
 * @func    CalcPosition
 * @brief   フィールドの座標をマップ座標に変換
 * @date    2015.09.01
 *
 * @param   x   X座標
 * @param   y   Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::CalcPosition( f32 * x, f32 * y )
{
  *x = *x * m_mapScaleH + ( static_cast<f32>( m_mapData.center_x ) / m_appScaleA * m_appScaleB );
  *y = *y * m_mapScaleV + ( static_cast<f32>( m_mapData.center_y ) / m_appScaleA * m_appScaleB );
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcIconPosition
 * @brief   フィールドの座標をマップ座標に変換（アイコン用）
 * @date    2016.03.03
 *
 * @param   x   X座標
 * @param   y   Y座標
 * @param   cx  中心X座標
 * @param   cy  中心Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::CalcIconPosition( f32 * x, f32 * y, f32 cx, f32 cy )
{
  CalcPosition( &cx, &cy );
  CalcPosition( x, y );
  *x = *x - cx;
  *y = cy - *y;
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcField
 * @brief   マップの座標をフィールド座標に変換
 * @date    2015.09.12
 *
 * @param   x   X座標
 * @param   y   Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::CalcField( f32 * x, f32 * y )
{
  *x = ( *x - ( static_cast<f32>( m_mapData.center_x ) / m_appScaleA * m_appScaleB ) ) * m_fieldScaleH;
  *y = ( *y - ( static_cast<f32>( m_mapData.center_y ) / m_appScaleA * m_appScaleB ) ) * m_fieldScaleV;
}


//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   マップデータ設定
 * @date    2015.08.26
 *
 * @param   id  マップID
 * @param   x   中心X座標
 * @param   y   中心Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::Setup( u32 id, f32 x, f32 y )
{
  m_mapID = id;
  if( m_mapID >= MAP_DATA_MAX )
  {
    GFL_ASSERT( 0 );
    m_mapID = 0;
  }
  m_mapData = MapDataTable[m_mapID];
  m_mapData.end_x = m_paneWidth * m_mapData.tex_width;
  m_mapData.end_y = m_paneHeight * m_mapData.tex_height;

  // 中心座標
  m_centerX = x;
  m_centerY = y;
  // 中心座標最小値
  m_centerMinX = static_cast<f32>(m_mapData.start_x);
  m_centerMinY = static_cast<f32>(m_mapData.start_y);
  AdjustCenter( &m_centerMinX, &m_centerMinY );
  CalcField( &m_centerMinX, &m_centerMinY );
  // 中心座標最大値
  m_centerMaxX = static_cast<f32>(m_mapData.end_x);
  m_centerMaxY = static_cast<f32>(m_mapData.end_y);
  AdjustCenter( &m_centerMaxX, &m_centerMaxY );
  CalcField( &m_centerMaxX, &m_centerMaxY );
  // 中心座標補正
  if( m_centerX < m_centerMinX )
  {
    m_centerX = m_centerMinX;
  }
  else if( m_centerX > m_centerMaxX )
  {
    m_centerX = m_centerMaxX;
  }
  if( m_centerY < m_centerMinY )
  {
    m_centerY = m_centerMinY;
  }
  else if( m_centerY > m_centerMaxY )
  {
    m_centerY = m_centerMaxY;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupEventArea
 * @brief   イベントアイコンの表示範囲を設定（範囲指定）
 * @date    2015.09.04
 *
 * @param   up      上
 * @param   down    下
 * @param   left    左
 * @param   right   右
 */
//-----------------------------------------------------------------------------
void MapManager::SetupEventArea( f32 up, f32 down, f32 left, f32 right )
{
  m_evLimUp    = up;
  m_evLimDown  = down;
  m_evLimLeft  = left;
  m_evLimRight = right;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupEventArea
 * @brief   イベントアイコンの表示範囲を設定（ペインから）
 * @date    2015.09.07
 */
//-----------------------------------------------------------------------------
void MapManager::SetupEventArea(void)
{
  gfl2::lyt::LytPane * pane = m_pLytWk->GetPane( PANENAME_FIELDMENU_MAP_LOW_001_PANE_ICONRANGE );
  gfl2::math::VEC3 pos = pane->GetTranslate();
  const nw::lyt::Size & size = pane->GetSize();

  SetupEventArea(
    size.height/2.0f+pos.y,
    -size.height/2.0f+pos.y,
    -size.width/2.0f+pos.x,
    size.width/2.0f+pos.x );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupPlayerSex
 * @brief   主人公の性別を設定
 * @date    2015.09.07
 *
 * @param   sex   性別
 */
//-----------------------------------------------------------------------------
void MapManager::SetupPlayerSex( u32 sex )
{
  m_playerSex = sex;
}

//-----------------------------------------------------------------------------
/**
 * @func    AdjustCenter
 * @brief   中心座標を補正
 * @date    2015.09.12
 *
 * @param   x   中心X座標
 * @param   y   中心Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::AdjustCenter( f32 * x, f32 * y )
{
  { // x
    s32 half  = DISPLAY_LOWER_WIDTH / 2;
    f32 start = static_cast<f32>( m_mapData.start_x + half );
    f32 end   = static_cast<f32>( m_mapData.end_x - half );
    if( *x < start )
    {
      *x = start;
    }
    else if( *x > end )
    {
      *x = end;
    }
  }
  { // y
    s32 half  = DISPLAY_LOWER_HEIGHT / 2;
    f32 start = static_cast<f32>( m_mapData.start_y + half );
    f32 end   = static_cast<f32>( m_mapData.end_y - half );
    if( *y < start )
    {
      *y = start;
    }
    else if( *y > end )
    {
      *y = end;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetBasePaneIndex
 * @brief   基準マップペインのインデックスを取得
 * @date    2015.09.12
 *
 * @return  ペインテーブルのインデックス
 */
//-----------------------------------------------------------------------------
u32 MapManager::GetBasePaneIndex(void)
{
  u32 idx = 0;
  gfl2::math::VEC3 pos = m_mapPane[0]->GetTranslate();
  f32 len = gfl2::math::FAbs(pos.x) + gfl2::math::FAbs(pos.y);

  for( u32 i=1; i<PANE_MAX; i++ )
  {
    pos = m_mapPane[i]->GetTranslate();
    f32 tmp = gfl2::math::FAbs(pos.x) + gfl2::math::FAbs(pos.y);
    if( tmp < len )
    {
      idx = i;
      len = tmp;
    }
  }

  return idx;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPanePos
 * @brief   マップペインの座標を初期化
 * @date    2015.08.26
 */
//-----------------------------------------------------------------------------
void MapManager::InitPanePos(void)
{
  // 中心座標変換
  f32 cx = m_centerX;
  f32 cy = m_centerY;
  CalcPosition( &cx, &cy );
  AdjustCenter( &cx, &cy );

  // ペイン内の座標に変換、表示座標を決定
  s32 pane_px = ( m_paneWidth / 2 ) - ( static_cast<s32>(cx) % m_paneWidth );
  s32 pane_py = ( static_cast<s32>(cy) % m_paneHeight ) - ( m_paneHeight / 2 );

  // ペインを配置する
  u32 start_tex_id = m_mapData.tex_id + ( static_cast<u32>(cy) / m_paneHeight ) * m_mapData.tex_width + ( static_cast<u32>(cx) / m_paneWidth );
  s16 tbl_x = 1;
  s16 tbl_y = 1;
  for( u32 i=0; i<PANE_MAX; i++ )
  {
    if( m_texRepWork[i].tex_id == start_tex_id || m_texRepWork[i].req_id == start_tex_id )
    {
      tbl_x = i % PANE_H_MAX;
      tbl_y = i / PANE_H_MAX;
      break;
    }
  }

  for( u32 i=0; i<PANE_V_MAX; i++ )
  {
    s32 tex_id = start_tex_id;
    for( u32 j=0; j<PANE_H_MAX; j++ )
    {
      s32 tbl = tbl_y * PANE_H_MAX + tbl_x;
      gfl2::lyt::LytPane * pane = m_mapPane[tbl];
      gfl2::math::VEC3 pos( pane_px, pane_py, 0.0f );
      pane->SetTranslate( pos );

      SetRequestTextureReplace( tbl, tex_id );

      pane_px += m_paneWidth;
      tex_id++;
      if( pane_px >= m_panePutLimitRight )
      {
        pane_px -= ( m_paneWidth * PANE_H_MAX );
        tex_id -= PANE_H_MAX;
      }

      tbl_x++;
      if( tbl_x >= PANE_H_MAX )
      {
        tbl_x = 0;
      }
    }
    pane_py -= m_paneHeight;
    start_tex_id += m_mapData.tex_width;
    if( pane_py <= m_panePutLimitDown )
    {
      pane_py += ( m_paneHeight * PANE_V_MAX );
      start_tex_id -= ( m_mapData.tex_width * PANE_V_MAX );
    }

    tbl_y++;
    if( tbl_y >= PANE_V_MAX )
    {
      tbl_y = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetMapData
 * @brief   マップデータを再設定
 * @date    2015.09.02
 *
 * @param   id  マップID
 * @param   x   中心X座標
 * @param   y   中心Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::ResetMapData( u32 id, f32 x, f32 y )
{
  Setup( id, x, y );
  InitPanePos();
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePosition
 * @brief   座標更新
 * @date    2015.08.26
 *
 * @param   id  マップID
 * @param   x   X座標
 * @param   y   Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::UpdatePosition( u32 id, f32 x, f32 y )
{
  ResetMapData( id, x, y );
  for( u32 i=0; i<EVENT_ICON_ID_MAX; i++ )
  {
    if( m_eventIconWork[i].visible != false )
    {
      PutEventIcon( static_cast<EventIconID>(i), m_eventIconWork[i].x, m_eventIconWork[i].y );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MovePosition
 * @brief   座標変更
 * @date    2015.10.21
 *
 * @param   x   X移動値
 * @param   y   Y移動値
 */
//-----------------------------------------------------------------------------
void MapManager::MovePosition( f32 x, f32 y )
{
  UpdatePosition( m_mapID, m_centerX+x, m_centerY+y );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMapPane
 * @brief   マップペインを取得
 * @date    2015.08.27
 *
 * @param   id    ペインテーブルID
 *
 * @return  ペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPicture * MapManager::GetMapPane( u32 id )
{
  return m_mapPane[id];
}

//-----------------------------------------------------------------------------
/**
 * @func    IsDispAreaPane
 * @brief   画面内のマップペインか
 * @date    2015.08.28
 *
 * @param   id    ペインテーブルID
 *
 * @retval  true  = 画面内のペイン
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::IsDispAreaPane( u32 id )
{
  gfl2::math::VEC3 pos = GetMapPane(id)->GetTranslate();
  f32 x = static_cast<f32>( DISPLAY_LOWER_WIDTH + m_paneWidth ) / 2.0f;
  f32 y = static_cast<f32>( DISPLAY_LOWER_HEIGHT + m_paneHeight ) / 2.0f;
  if( pos.x >= -x && pos.x < x && pos.y <= y && pos.y > -y )
  {
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    StartAnimeFrame
 * @brief   アニメ開始
 * @date    2015.09.02
 *
 * @param   anm     アニメ番号
 * @param   frame   開始フレーム
 */
//-----------------------------------------------------------------------------
void MapManager::StartAnimeFrame( u32 anm, f32 frame )
{
  if( m_pLytWk->IsBindAnime( anm ) == false )
  {
    m_pLytWk->BindAnime( anm, true );
  }
  m_pLytWk->SetAutoAnime( anm, true );
  m_pLytWk->SetAnimeFrame( anm, frame );
}

//-----------------------------------------------------------------------------
/**
 * @func    StopAnime
 * @brief   アニメ停止
 * @date    2015.09.02
 *
 * @param   anm   アニメ番号
 */
//-----------------------------------------------------------------------------
void MapManager::StopAnime( u32 anm )
{
  if( m_pLytWk->IsBindAnime( anm ) == false )
  {
    return;
  }
  m_pLytWk->UnBindAnime( anm );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsAnimeEnd
 * @brief   アニメが終了したか
 * @date    2015.09.02
 *
 * @param   anm   アニメ番号
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MapManager::IsAnimeEnd( u32 anm )
{
  if( m_pLytWk->IsAnime( anm ) != false &&
      m_pLytWk->IsBindAnime( anm ) != false &&
      m_pLytWk->IsAnimeEnd( anm ) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetAnimeMaxFrame
 * @brief   アニメの最大フレームを取得
 * @date    2015.09.03
 *
 * @param   anm   アニメ番号
 *
 * @return  最大フレーム
 */
//-----------------------------------------------------------------------------
f32 MapManager::GetAnimeMaxFrame( u32 anm )
{
  if( m_pLytWk->IsBindAnime( anm ) == false )
  {
    m_pLytWk->BindAnime( anm, true );
  }
  return m_pLytWk->GetAnimeMaxFrame( anm );
}


//-----------------------------------------------------------------------------
/**
 * @func    InitIcon
 * @brief   アイコン初期化
 * @date    2015.09.02
 */
//-----------------------------------------------------------------------------
void MapManager::InitIcon(void)
{
  // 主人公
  InitHeroIcon();
  // メイン
  InitEventIcon( EVENT_ICON_ID_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitHeroIcon
 * @brief   主人公アイコン初期化
 * @date    2015.09.07
 */
//-----------------------------------------------------------------------------
void MapManager::InitHeroIcon(void)
{
  ChangeHeroIcon( HERO_ICON_ID_PLAYER );
  SetVisibleHeroIcon( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitEventIcon
 * @brief   イベントアイコン初期化
 * @date    2015.09.02
 *
 * @param   id  イベントアイコンID
 */
//-----------------------------------------------------------------------------
void MapManager::InitEventIcon( EventIconID id )
{
  EventIconWork * wk = &m_eventIconWork[id];
  const EventIcon * icon = &EventIconTable[id];

  SetVisibleEventIcon( id, false );
//  StartAnimeFrame( icon->arrow_off_anime, GetAnimeMaxFrame(icon->arrow_off_anime) );  // 矢印は非表示にしておく

//  wk->last_arrow_anime = icon->arrow_off_anime;
  wk->visible = false;
//  wk->visible_req = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutHeroIcon
 * @brief   主人公アイコン配置
 * @date    2015.08.31
 *
 * @param   x   X座標
 * @param   y   Y座標
 * @param   r   向き
 */
//-----------------------------------------------------------------------------
void MapManager::PutHeroIcon( f32 x, f32 y, f32 r )
{
  CalcIconPosition( &x, &y, m_centerX, m_centerY );

  gfl2::lyt::LytParts * parts = m_pLytWk->GetPartsPane( PANENAME_FIELDMENU_MAP_LOW_001_PANE_PLAYERICON_00 );

  { // 座標
    gfl2::math::VEC3 pos( x, y, 0.0f );
    parts->SetTranslate( pos );
  }
  { // 回転
    gfl2::lyt::LytPane * pane = m_pLytWk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_013_PANE_PLAYERBASE_00, &m_resID );
    gfl2::math::VEC3 rotate = pane->GetRotate();
    f32 p = 0.0f;
    if( r > rotate.z )
    {
      p = r - rotate.z;
      if( p > 180.0f )
      {
        p = ( 360.0f - p ) * -1.0f;
      }
    }
    else if( r < rotate.z )
    {
      p = ( rotate.z - r ) * -1.0f;
      if( p < -180.0f )
      {
        p = 360.0f + p;
      }
    }
    // 矢印が表示されているときはHERO_ROTATE_MOVE_MAX度以上は動かさないようにする
    if( m_pLytWk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_013_PANE_PLAYER_ARR, &m_resID )->IsVisible() != false )
    {
      if( p > HERO_ROTATE_MOVE_MAX )
      {
        p = HERO_ROTATE_MOVE_MAX;
      }
      else if( p < -HERO_ROTATE_MOVE_MAX )
      {
        p = -HERO_ROTATE_MOVE_MAX;
      }
    }
    rotate.z += p;
    if( rotate.z < 0 )
    {
      rotate.z += 360.0f;
    }
    else if( rotate.z >= 360.0f )
    {
      rotate.z -= 360.0f;
    }
    pane->SetRotate( rotate );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleHeroIcon
 * @brief   主人公アイコン表示切り替え
 * @date    2015.08.31
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MapManager::SetVisibleHeroIcon( bool flg )
{
  m_pLytWk->GetPane( PANENAME_FIELDMENU_MAP_LOW_001_PANE_PLAYERICON_00 )->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleHeroArrow
 * @brief   主人公の向きの表示切り替え
 * @date    2015.09.09
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MapManager::SetVisibleHeroArrow( bool flg )
{
  gfl2::lyt::LytParts * parts = m_pLytWk->GetPartsPane( PANENAME_FIELDMENU_MAP_LOW_001_PANE_PLAYERICON_00 );
  m_pLytWk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_013_PANE_PLAYER_ARR, &m_resID )->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutEventIcon
 * @brief   イベントアイコン配置
 * @date    2015.08.31
 *
 * @param   id  イベントアイコンID
 * @param   x   X座標
 * @param   y   Y座標
 */
//-----------------------------------------------------------------------------
void MapManager::PutEventIcon( EventIconID id, f32 x, f32 y )
{
  EventIconWork * wk = &m_eventIconWork[id];
  const EventIcon * icon = &EventIconTable[id];

  wk->x = x;
  wk->y = y;

  CalcIconPosition( &x, &y, m_centerX, m_centerY );

  gfl2::lyt::LytParts * parts = m_pLytWk->GetPartsPane( icon->parent );
  gfl2::math::VEC3 pos( x, y, 0.0f );
  parts->SetTranslate( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleEventIcon
 * @brief   イベントアイコン表示切り替え
 * @date    2015.08.31
 *
 * @param   id    イベントアイコンID
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MapManager::SetVisibleEventIcon( EventIconID id, bool flg )
{
  EventIconWork * wk = &m_eventIconWork[id];
  const EventIcon * icon = &EventIconTable[id];

  m_pLytWk->GetPane( icon->parent )->SetVisible( flg );
  wk->visible = flg;
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateButtonInfo
 * @brief   ボタンデータ生成
 * @date    2015.09.07
 *
 * @param   info  生成場所
 * @param   id    ボタンID
 */
//-----------------------------------------------------------------------------
void MapManager::CreateButtonInfo( app::ui::ButtonInfoLytWk * info, app::tool::ButtonManager::ButtonId id )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_FIELDMENU_MAP_LOW_001_PANE_EVENTICON_00,
  };
  static const u32 anm_tbl[][6] =
  {
    {
      LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_TOUCH,
      LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_RELEASE,
      LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_CANSEL,
      LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_TOUCH_RELEASE,
      LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_ACTIVE,
      LA_NAVI_FIELDMENU_MAP_LOW_001__EVENTICON_00_PASSIVE,
    },
  };

  for( u32 i=0; i<GFL_NELEMS(parts_tbl); i++ )
  {
    gfl2::lyt::LytParts * parts = m_pLytWk->GetPartsPane( parts_tbl[i] );
    info[i].button_id = id+i;
    info[i].picture_pane = parts;
    info[i].bound_pane   = m_pLytWk->GetPane( parts, PANENAME_FIELDMENU_BTN_LOW_011_PANE_BOUND_00, &m_resID );
    info[i].lyt_wk       = m_pLytWk;
    info[i].touch_anime_index      = anm_tbl[i][0];
    info[i].release_anime_index    = anm_tbl[i][1];
    info[i].cancel_anime_index     = anm_tbl[i][2];
    info[i].key_select_anime_index = anm_tbl[i][3];
    info[i].active_anime_index     = anm_tbl[i][4];
    info[i].passive_anime_index    = anm_tbl[i][5];
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    GetCenterPos
 * @brief   中心座標取得
 * @date    2015.09.12
 *
 * @param   x   X座標取得場所
 * @param   y   Y座標取得場所
 */
//-----------------------------------------------------------------------------
void MapManager::GetCenterPos( f32 * x, f32 * y )
{
  *x = m_centerX;
  *y = m_centerY;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetPaneSizeNavi
 * @brief   ナビ用のペインサイズ設定
 * @date    2016.01.09
 */
//-----------------------------------------------------------------------------
void MapManager::SetPaneSizeNavi(void)
{
  /*
    80%だと整数にならないので、近い値になるように計算

      256 * 0.8 = 204.8
      128 * 0.8 = 102.4

      256 / 16 * 13 = 208
      128 / 16 * 13 = 104
  */

/*
  // 81%相当
  m_appScaleA = 16;
  m_appScaleB = 13;
*/
  // 66%相当
  m_appScaleA = 32;
  m_appScaleB = 21;

  m_paneWidth  = PANE_WIDTH / m_appScaleA * m_appScaleB;
  m_paneHeight = PANE_HEIGHT / m_appScaleA * m_appScaleB;

  m_panePutLimitUp    = m_paneHeight * ( PANE_V_MAX / 2 );
  m_panePutLimitDown  = -m_panePutLimitUp;
  m_panePutLimitRight = m_paneWidth * ( PANE_H_MAX / 2 );
  m_panePutLimitLeft  = -m_panePutLimitRight;

  {
    f32 def_2d_x_size = static_cast<f32>( MAP_2D_DEFAULT_X_SIZE * 2 ) / static_cast<f32>(m_appScaleA) * static_cast<f32>(m_appScaleB);
    f32 def_2d_y_size = static_cast<f32>( MAP_2D_DEFAULT_Y_SIZE * 2 ) / static_cast<f32>(m_appScaleA) * static_cast<f32>(m_appScaleB);
    m_mapScaleH = def_2d_x_size / static_cast<f32>(MAP_3D_DEFAULT_X_SIZE);
    m_mapScaleV = def_2d_y_size / static_cast<f32>(MAP_3D_DEFAULT_Y_SIZE);
    m_fieldScaleH = static_cast<f32>(MAP_3D_DEFAULT_X_SIZE) / def_2d_x_size;
    m_fieldScaleV = static_cast<f32>(MAP_3D_DEFAULT_Y_SIZE) / def_2d_y_size;
  }

  {
    nw::lyt::Size size( static_cast<f32>(m_paneWidth), static_cast<f32>(m_paneHeight) );

    for( u32 i=0; i<PANE_MAX; i++ )
    {
      if( m_mapPane[i] != NULL )
      {
        m_mapPane[i]->SetSize( size );
        m_mapPane[i]->GetMaterial()->SetTexMapFilter( 0, nw::lyt::TEXFILTER_LINEAR, nw::lyt::TEXFILTER_LINEAR );
      }
    }
  }

  m_isScaleLinear = true;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetCenterSRT
 * @brief   中心のSRTをセット
 * @date    2016.01.12
 *
 * @param   scale   拡縮
 * @param   rotate  回転
 * @param   trans   座標
 */
//-----------------------------------------------------------------------------
void MapManager::SetCenterSRT( gfl2::math::VEC2 * scale, gfl2::math::VEC3 * rotate, gfl2::math::VEC3 * trans )
{
  if( m_centerPane == NULL )
  {
    return;
  }
  m_centerPane->SetScale( *scale );
  m_centerPane->SetRotate( *rotate );
  m_centerPane->SetTranslate( *trans );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCenterSRT
 * @brief   中心のSRTをセット
 * @date    2016.01.12
 *
 * @param   mtx   グローバルマトリックス
 */
//-----------------------------------------------------------------------------
/*
void MapManager::SetCenterSRT( const gfl2::math::MTX34 & mtx )
{
  if( m_centerPane == NULL )
  {
    return;
  }
  m_centerPane->SetGlobalMtx( mtx );
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    ChangeHeroIcon
 * @brief   主人公アイコンの表示を変更
 * @date    2016.01.14
 *
 * @param   id  アイコンID
 */
//-----------------------------------------------------------------------------
void MapManager::ChangeHeroIcon( HeroIconID id )
{
  static const u32 tbl[] =
  {
    LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_00,
    LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_02,
    LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_03,
  };

  u32 anm = tbl[id];

  // ループアニメ停止
  if( anm != LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_02 )
  {
    StopAnime( LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_02 );
  }
  if( anm != LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_03 )
  {
    StopAnime( LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_03 );
  }

  if( id == HERO_ICON_ID_PLAYER && m_playerSex == PM_FEMALE )
  {
    anm = LA_NAVI_FIELDMENU_MAP_LOW_001__PLAYERICON_00_PLAYER_01;
  }
  StartAnimeFrame( anm, GetAnimeMaxFrame(anm) );
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishFinderButtonPane
 * @brief   ファインダーアイコンのボタンペインを非表示にする
 * @date    2017.03.15
 */
//-----------------------------------------------------------------------------
void MapManager::VanishFinderButtonPane(void)
{
  gfl2::lyt::LytPane * pane = m_pLytWk->GetPane(
                                m_pLytWk->GetPartsPane( PANENAME_FIELDMENU_MAP_LOW_001_PANE_PLAYERICON_00 ),
                                PANENAME_FIELDMENU_BTN_LOW_013_PANE_BUTTONICON_R, &m_resID );
  pane->SetVisible( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMapDataIndex
 * @brief   マップデータのインデックスを取得
 * @date    2016.03.03
 *
 * @param   type  SubScreenMapType
 *
 * @return  マップデータのインデックス
 */
//-----------------------------------------------------------------------------
u32 MapManager::GetMapDataIndex( SubScreenMapType type )
{
  for( u32 i=0; i<MAP_DATA_MAX; i++ )
  {
    if( MapDataTable[i].data_id == type )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return 0;
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
