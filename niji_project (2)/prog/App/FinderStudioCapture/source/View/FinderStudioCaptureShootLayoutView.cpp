//======================================================================
/**
 * @file    FinderStudioCaptureShootLayoutView.cpp
 * @date    2017/01/25 10:33:59
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影レイアウトView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureShootLayoutView.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCaptureShootCommonView.h"

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/MyStatus.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_pane.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>
#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影メインView
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
LayoutView::LayoutView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_pCommonView( NULL )
  , m_LayoutType( TYPE_NULL )
  , m_UpdateState( UPD_STATE_NONE )
  , m_InputType( INPUT_TYPE_NONE )
  , m_PrevInputType( INPUT_TYPE_KEY )
  , m_PokeMoveState(POKE_MOVE_STATE_NONE)
  , m_TurnTableRotate(0.0f)
  , m_bCollision( false )
  , m_PokemonPosition(0, 0, 0)
  , m_PrevPokemonPosition( 0, 0, 0 )
  , m_LastTouchPoint(0, 0, 0)
  , m_vecTouchRotate( 0, 0, 0 )
  , m_TouchState(TOUCH_STATE_NONE)
  , m_pTurnTablePane( NULL )
  , m_pPlayerNullPane( NULL )
  , m_pPlayerAreaPane( NULL )
  , m_pPlayerArrowPane( NULL )
  , m_pPokeNullPane( NULL )
  , m_pPokeIconPane( NULL )
  , m_pPokeAreaPane( NULL )
  , m_pPokeArrowPane( NULL )
  , m_pPokeBoundPane( NULL )
  , m_MsgType( MSG_TYPE_NULL )
  , m_bShowWindow( false )
  , m_pMessageWindowParts( NULL )
  , m_pMessageWindowTextBox( NULL )
{
  for( u32 i = 0; i < TYPE_NUM; ++i )
  {
    m_pObiPane[i] = NULL;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void LayoutView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // アニメの初期化
  InitAnim();
  // ペインの初期化
  InitPane();
  // ボタンの初期化
  InitButton();
}

/**
 * @brief   2Dの初期化
 */
void LayoutView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // UIInputListenerのセット
  this->SetInputListener( this );
  // Viewの初期設定
  this->_InitView();
}

/**
 * @brief   ペインの初期化
 */
void LayoutView::InitPane( void )
{
  // 回転盤
  m_pTurnTablePane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_ROT );

  // プレイヤー用
  m_pPlayerNullPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_NULL_PLAYER );
  m_pPlayerAreaPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_PLAYERAREA );
  m_pPlayerArrowPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_NULL_PLAYER_YAJIRUSHI );

  // ポケモン用
  m_pPokeNullPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_NULL_POKE );
  m_pPokeIconPane = m_pUtil->GetPane(PANENAME_STUDIO_LAYOUT_LOW_00_PANE_POKEICON);
  m_pPokeAreaPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_POKEAREA );
  m_pPokeArrowPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_NULL_POKE_YAJIRUSHI );
  m_pPokeBoundPane = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_BOUNDING_00 );

  // メッセージウィンドウ用
  m_pMessageWindowParts = m_pUtil->GetParts( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_MESSAGE_L1 );
  m_pMessageWindowTextBox = m_pUtil->GetTextBox( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, m_pMessageWindowParts );

  // 帯
  const u32 c_ObiPaneIndex[] = {
    PANENAME_STUDIO_LAYOUT_LOW_00_PANE_BGOBI_1,
    PANENAME_STUDIO_LAYOUT_LOW_00_PANE_BGOBI_2,
    PANENAME_STUDIO_LAYOUT_LOW_00_PANE_BGOBI_3,
  };

  for( u32 i = 0; i < TYPE_NUM; ++i )
  {
    m_pObiPane[i] = m_pUtil->GetPane( c_ObiPaneIndex[i] );
  }
}

/**
 * @brief   アニメの初期化
 */
void LayoutView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00__BG01_BG_LOOP_01 );
}

/**
 * @brief   ボタンの初期化
 */
void LayoutView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00__INFOBUTTON_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LAYOUT_LOW_00_PANE_INFOBUTTON,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
  };

  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    Static::ButtonInfo::SetupBtnInfo( &info[i], i, c_ButtonAnim[i] );

    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );
    gfl2::lyt::LytPane*  bound = m_pUtil->GetPane( c_ButtonBounding[i], parts );

    info[i].picture_pane = parts;
    info[i].bound_pane = bound;
  }

  // ボタンマネージャの生成
  CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, BTN_ID_NUM );

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool LayoutView::Terminate( void )
{
  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void LayoutView::Update( void )
{
  this->_Update();

  _UpdateLayout();
}

/**
 * @brief   更新：レイアウト
 */
void LayoutView::_UpdateLayout( void )
{
  switch( m_LayoutType )
  {
    case TYPE_POKE_MOVE:     _UpdatePokeMove(); break;
    case TYPE_POKE_ROTATE:   _UpdatePokeRotate(); break;
    case TYPE_PLAYER_ROTATE: _UpdatePlayerRotate(); break;
  }

  // 更新ステートのリセット
  m_UpdateState = UPD_STATE_NONE;

  // 入力タイプのリセット
  m_PrevInputType = m_InputType;
  m_InputType = INPUT_TYPE_NONE;
}

/**
 * @brief   更新：ポケモン移動
 */
void LayoutView::_UpdatePokeMove( void )
{
  // ポケモンのAABBサイズの更新
  _SetPokemonAABBAreaSize();
  // ポケモンのAABBの中心点を設定
  _SetPokemonAABBAreaCenter();

  if( m_InputType == INPUT_TYPE_NONE )
  {
    // キー入力の終了タイミングで、
    // Viewer側との位置を合わせる
    _SetPokemonPositionFromViewer();
  }

  // メッセージウィンドウの更新
  _UpdateMessage();
}

/**
 * @brief   更新：ポケモン回転
 */
void LayoutView::_UpdatePokeRotate( void )
{
  // ポケモンのAABBサイズの更新
  _SetPokemonAABBAreaSize();
  // ポケモンのAABBの中心点を設定
  _SetPokemonAABBAreaCenter();

  if( m_UpdateState == UPD_STATE_POKE_ROTATE )
  {
    // ポケモンの回転情報の反映
    _SetPokemonRotate();
  }

  if( m_InputType == INPUT_TYPE_NONE )
  {
    // Viewer側の回転値にUI側も合わせる
    _SetViewerPokemonRotate();
  }

  // メッセージウィンドウの更新
  _UpdateMessage();
}

/**
 * @brief   更新：プレイヤー回転
 */
void LayoutView::_UpdatePlayerRotate( void )
{
  // ポケモンのAABBサイズの更新    // @fix MMCat[715]
  _SetPokemonAABBAreaSize();
  // ポケモンのAABBの中心点を設定  // @fix MMCat[715]
  _SetPokemonAABBAreaCenter();

  // プレイヤーのAABBサイズの更新
  _SetPlayerAABBAreaSize();
  // プレイヤーのAABBの中心点を設定
  _SetPlayerAABBAreaCenter();

  if( m_UpdateState == UPD_STATE_PLAYER_ROTATE )
  {
    // プレイヤーの回転情報の反映
    _SetPlayerRotate();
  }

  if( m_InputType == INPUT_TYPE_NONE )
  {
    // Viewer側の回転値にUI側も合わせる
    _SetViewerPlayerRotate();
  }

  // メッセージウィンドウの更新
  _UpdateMessage();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ポケモン
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ポケモンAABBサイズの設定
 */
void LayoutView::_SetPokemonAABBAreaSize( void )
{
  const f32 BOUNDING_SIZE_MIN = 30.0f;

  gfl2::math::AABB aabb = m_pListener->GetPokemonAABB();
  nw::lyt::Size size = _ConvertBoundSizeFromAABB( aabb );

  // ペインサイズを更新
  m_pUtil->SetPaneSize( m_pPokeAreaPane, size );

  FUKUSHIMA_PRINT( "_SetPokemonAABBAreaSize\n" );
  FUKUSHIMA_PRINT( "size width=[%.1f] height=[%.1f]\n", size.width, size.height );
  FUKUSHIMA_PRINT( "aabb min x=[%.1f] y=[%.1f] z=[%.1f] w=[%.1f]\n", aabb.GetMin().x, aabb.GetMin().y, aabb.GetMin().z, aabb.GetMin().w );
  FUKUSHIMA_PRINT( "aabb max x=[%.1f] y=[%.1f] z=[%.1f] w=[%.1f]\n", aabb.GetMax().x, aabb.GetMax().y, aabb.GetMax().z, aabb.GetMax().w );
}

/**
 * @brief   ポケモンAABBの中心点の設定
 */
void LayoutView::_SetPokemonAABBAreaCenter( void )
{
  gfl2::math::VEC3 pos = _GetPokemonAABBAreaCenter();

  m_pUtil->SetPanePos( m_pPokeAreaPane, pos );
}

/**
 * @brief   ポケモン位置の設定
 *
 * @param   pos   位置座標
 */
void LayoutView::_SetPokemonPosition( gfl2::math::VEC3 pos )
{
  // ポケモン位置を設定
  _SetPokemonPosition( pos, &m_PokemonPosition );

  // Viewerに反映
  m_bCollision = m_pListener->SetPokemonPosition( m_PokemonPosition, true );

  // エリアカラー設定
  _SetPokemonAABBAreaColor( m_bCollision );

  if( !m_bCollision )
  {
    m_PrevPokemonPosition = m_PokemonPosition;
  }
}

/**
 * @brief   ポケモン位置を設定
 *
 * @param[in]   pos         位置座標
 * @param[out]  dstScrPos   スクリーン座標の格納先
 */
void LayoutView::_SetPokemonPosition( gfl2::math::VEC3 pos, gfl2::math::Vector3* dstScrPos )
{
  gfl2::math::VEC3 vecScreen( 0, 0, 0 );
  gfl2::math::VEC3 vecLayout( 0, 0, 0 );

  // 回転盤内のポケモン位置を取得
  _GetPokemonPositionInTurnTable( pos, &vecLayout, &vecScreen );

  // レイアウト上の座標を更新
  m_pUtil->SetPanePos( m_pPokeNullPane, vecLayout );

  // スクリーン座標を更新
  if( dstScrPos )
  {
    *dstScrPos = _ConvertVector_LayoutToScreen( vecScreen );
  }

  FUKUSHIMA_PRINT(
    "PokemonPosition : x=[%.1f] y=[%.1f] collision=[%d]\n"
    , vecLayout.x, vecLayout.y, m_bCollision );
}

/**
 * @brief   Viewer側のポケモン位置を反映
 */
void LayoutView::_SetPokemonPositionFromViewer( bool bInit/* = false*/ )
{
  // Viewer側の座標値を取得(2D座標)
  gfl2::math::Vector3 vecScreen = m_pListener->GetPokemonPosition();
  // レイアウト座標系に変換
  gfl2::math::VEC3 vecLayout = _ConvertVector_ScreenToLayout( vecScreen );

  // ポケモン位置の設定
  if( !bInit )
  {
    _SetPokemonPosition( vecLayout, NULL );
  }
  else {
    _SetPokemonPosition( vecLayout, &m_PrevPokemonPosition );
  }

  // Viewerに反映
  m_bCollision = false;

  // エリアカラー設定
  _SetPokemonAABBAreaColor( m_bCollision );

  m_PokemonPosition = m_PrevPokemonPosition;
}


/**
 * @brief   ポケモンの回転値を反映
 */
void LayoutView::_SetPokemonRotate( void )
{
  f32 degree = m_pListener->GetPokemonRotate();

  _SetPokemonRotate( degree );
}

/**
 * @brief   Viewerのポケモン回転値を反映
 */
void LayoutView::_SetViewerPokemonRotate( void )
{
  FUKUSHIMA_PRINT( "LayoutView::_SetViewerPokemonRotate\n" );

  f32 degree = m_pListener->GetViewerPokemonRotate();

  _SetPokemonRotate( degree );
  
  // UI用の回転値をViewerに合わせる
  m_bCollision = m_pListener->SetPokemonRotate( degree );

  // 衝突時の色変更
  _SetPokemonAABBAreaColor( m_bCollision );
}

/**
 * @brief   ポケモンの回転値を反映
 */
void LayoutView::_SetPokemonRotate( f32 degree )
{
  gfl2::math::VEC3 vecRot = m_pUtil->GetPaneRotate( m_pPokeArrowPane );
  {
    vecRot.z = degree + 90.0f;    //!< レイアウトのポケモンの向き値分加算
  }
  m_pUtil->SetPaneRotate( m_pPokeArrowPane, vecRot );
}



/**
 * @brief   ポケモンの回転を開始
 */
void LayoutView::_StartPokemonRotate( gfl2::ui::TouchPanel* tp )
{
  FUKUSHIMA_PRINT("LayoutView::_StartPokemonRotate\n");

  gfl2::math::Vector3 vecScreen( tp->GetX(), tp->GetY(), 0.0f );
  m_vecTouchRotate = _ConvertVector_ScreenToLayout( vecScreen );

  m_TouchState = TOUCH_STATE_ROTATE;
}

/**
 * @brief   ポケモンの回転を更新
 */
void LayoutView::_DragPokemonRotate( gfl2::ui::TouchPanel* tp )
{
  FUKUSHIMA_PRINT( "LayoutView::_DragPokemonRotate\n" );

  gfl2::math::VEC3 nowPos( 0, 0, 0 );

  // 角度を計算
  f32 degree = _GetDegreeThreePoint_Pokemon( m_pPokeNullPane, tp, &nowPos );

  FUKUSHIMA_PRINT( "degree=[%.1f]\n", degree );

  // 回転値をペインに反映
  gfl2::math::VEC3 vecRot = m_pUtil->GetPaneRotate( m_pPokeArrowPane );
  vecRot.z += degree;
  if( vecRot.z >= 360.0f ) { vecRot.z -= 360.0f; }
  if( vecRot.z <= -360.0f ) { vecRot.z += 360.0f; }
  m_pUtil->SetPaneRotate( m_pPokeArrowPane, vecRot );

  FUKUSHIMA_PRINT( "LayoutRotate.z=[%.1f]\n", vecRot.z );
  FUKUSHIMA_PRINT( "ViewerRotate.z=[%.1f]\n", vecRot.z - 90.0f );

  // 回転値をViewerに反映
  m_bCollision = m_pListener->SetPokemonRotate( vecRot.z - 90.0f );

  // 衝突時の色変更
  _SetPokemonAABBAreaColor( m_bCollision );

  m_vecTouchRotate = nowPos;
}

/**
 * @brief   ポケモンの回転を終了
 */
void LayoutView::_FinishPokemonRotate( void )
{
  FUKUSHIMA_PRINT( "LayoutView::_FinishPokemonRotate\n" );

  // Viewerの回転値にUI側もあわせる
  _SetViewerPokemonRotate();

  m_vecTouchRotate = gfl2::math::VEC3::Zero();
  m_TouchState = TOUCH_STATE_NONE;
}


/**
 * @brief   回転盤内のポケモンの位置を取得
 *
 * @param[in]   srcNowPos   現在位置座標(レイアウト座標)
 * @param[out]  dstLayout   レイアウト座標の格納先
 * @param[out]  dstScreen   スクリーン座標の格納先
 */
void LayoutView::_GetPokemonPositionInTurnTable( gfl2::math::VEC3 srcNowPos, gfl2::math::VEC3* dstLayout, gfl2::math::VEC3* dstScreen )
{
  // 中心座標の取得
  gfl2::math::VEC3 center = m_pUtil->GetPanePos( m_pTurnTablePane );

  // 中心点からの距離を取得
  f32 dist = _GetNextPositionDistance( center, srcNowPos );
  // 現在位置の角度を取得
  f32 degree = _GetNextPositionDegree( center, srcNowPos );

  // 回転盤の半径を超えないよう調整
  if( dist > Static::UI_MOVE_RADIUS )
  {
    dist = Static::UI_MOVE_RADIUS;
  }

  *dstLayout = _GetPokemonNextPosition( degree - m_TurnTableRotate, dist );
  *dstScreen = _GetPokemonNextPosition( degree, dist );
}

/**
 * @brief   移動先から中心点までの距離を取得
 *
 * @param   vec1    中心座標
 * @param   vec2    移動先の座標
 *
 * @retval  移動先から中心点の距離
 */
f32 LayoutView::_GetNextPositionDistance( gfl2::math::VEC3 vec1, gfl2::math::VEC3 vec2 )
{
  f32 dist = gfl2::math::FSqrt( ( vec2.x - vec1.x ) * ( vec2.x - vec1.x ) + ( vec2.y - vec1.y ) * ( vec2.y - vec1.y ) );
  return dist;
}

/**
 * @brief   移動先の角度を取得
 *
 * @param   vec1    中心座標
 * @param   vec2    移動先の座標
 *
 * @retval  移動先の角度を取得
 */
f32 LayoutView::_GetNextPositionDegree( gfl2::math::VEC3 vec1, gfl2::math::VEC3 vec2 )
{
  f32 degree = gfl2::math::Atan2Deg( ( vec2.y - vec1.y ), ( vec2.x - vec1.x ) );

  if( degree < 0.0f )
  {
    degree = gfl2::math::FAbs( degree );
  }
  else {
    degree = 360.0f - degree;
  }

  return degree;
}

/**
 * @brief   移動先の座標値を取得
 *
 * @param   degr    角度
 * @param   dist    距離
 */
gfl2::math::VEC3 LayoutView::_GetPokemonNextPosition( f32 deg, f32 dist )
{
  gfl2::math::VEC3 point( 0, 0, 0 );

  point.x = gfl2::math::CosDeg( deg ) * dist;
  point.y = gfl2::math::SinDeg( deg ) * dist * -1.0f;

  return point;
}

/**
 * @brief   ポケモンAABBの中心座標の取得
 */
gfl2::math::VEC3 LayoutView::_GetPokemonAABBAreaCenter( void )
{
  FUKUSHIMA_PRINT( "_GetPokemonAABBAreaCenter\n" );

  gfl2::math::AABB aabb = m_pListener->GetPokemonAABB();
  gfl2::math::Vector3 vecAABBMin( aabb.GetMin().x, aabb.GetMax().z, 0 );
  gfl2::math::Vector3 vecPokePos = m_pListener->GetPokemonViewerPosition();

  FUKUSHIMA_PRINT( " PokePos : x=[%.1f] y=[%.1f]\n", vecPokePos.x, vecPokePos.y );
  FUKUSHIMA_PRINT( " AABB min : x=[%.1f] y=[%.1f]\n", vecAABBMin.x, vecAABBMin.y );

  vecAABBMin = vecPokePos - vecAABBMin;

  vecAABBMin.x *= Static::MOVE_DIAMETER_RATE;
  vecAABBMin.y *= Static::MOVE_DIAMETER_RATE;

  FUKUSHIMA_PRINT( " AABB min(UI) : x=[%.1f] y=[%.1f]\n", vecAABBMin.x, vecAABBMin.y );

  nw::lyt::Size areaSize = m_pUtil->GetPaneSize( m_pPokeAreaPane );

  gfl2::math::VEC3 center( 0, 0, 0 );
  center.x = gfl2::math::FAbs( vecAABBMin.x ) - ( areaSize.width / 2.0f );
  center.y = gfl2::math::FAbs( vecAABBMin.y ) - ( areaSize.height / 2.0f );

  FUKUSHIMA_PRINT( " center(UI) : x=[%.1f] y=[%.1f]\n", center.x, center.y );

  return center;
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    プレイヤー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   プレイヤーAABBサイズの設定
 */
void LayoutView::_SetPlayerAABBAreaSize( void )
{
  gfl2::math::AABB aabb = m_pListener->GetPlayerAABB();
  nw::lyt::Size size = _ConvertBoundSizeFromAABB( aabb );

  m_pUtil->SetPaneSize( m_pPlayerAreaPane, size );

  FUKUSHIMA_PRINT( "_SetPlayerAABBAreaSize : size width=[%.1f] height=[%.1f]\n", size.width, size.height );
}

/**
 * @brief   プレイヤーAABBの中心点を設定
 */
void LayoutView::_SetPlayerAABBAreaCenter( void )
{
  gfl2::math::VEC3 pos = _GetPlayerAABBAreaCenter();

  m_pUtil->SetPanePos( m_pPlayerAreaPane, pos );
}

/**
 * @brief   AABBエリアカラー設定
 *
 * @param   bCollision  衝突フラグ
 */
void LayoutView::_SetPlayerAABBAreaColor( bool bCollision )
{
  if( !bCollision )
  {
    m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00_PLAYERCOLOR );
  }
  else {
    m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00_PLAYERCOLOR_00 );
  }
}


/**
 * @brief   UI用の回転値を反映
 */
void LayoutView::_SetPlayerRotate( void )
{
  f32 degree = m_pListener->GetPlayerRotate();

  _SetPlayerRotate( degree );
}

/**
 * @brief   Viewer用の回転値を反映
 */
void LayoutView::_SetViewerPlayerRotate( void )
{
  FUKUSHIMA_PRINT( "LayoutView::_SetViewerPlayerRotate\n" );

  f32 degree = m_pListener->GetViewerPlayerRotate();

  _SetPlayerRotate( degree );

  // UI用の回転値をViewerに合わせる
  m_bCollision = m_pListener->SetPlayerRotate( degree );

  // 衝突時の色変更
  _SetPlayerAABBAreaColor( m_bCollision );
}

/**
 * @brief   プレイヤー回転の反映
 */
void LayoutView::_SetPlayerRotate( f32 degree )
{
  f32 rotDeg = ( degree + 180.0f );
  if( rotDeg > 360.0f ) rotDeg -= 360.0f;

  gfl2::math::VEC3 vecRot = m_pUtil->GetPaneRotate( m_pPlayerArrowPane );
  vecRot.z = rotDeg - 90.0f;

  m_pUtil->SetPaneRotate( m_pPlayerArrowPane, vecRot );
}



/**
 * @brief   プレイヤー回転の開始
 */
void LayoutView::_StartPlayerRotate( gfl2::ui::TouchPanel* tp )
{
  FUKUSHIMA_PRINT( "LayoutView::_StartPlayerRotate\n" );

  gfl2::math::Vector3 vecScreen( tp->GetX(), tp->GetY(), 0.0f );
  m_vecTouchRotate = _ConvertVector_ScreenToLayout( vecScreen );

  m_TouchState = TOUCH_STATE_ROTATE;
}

/**
 * @brief   プレイヤー回転の更新
 */
void LayoutView::_DragPlayerRotate( gfl2::ui::TouchPanel* tp )
{
  FUKUSHIMA_PRINT( "LayoutView::_DragPlayerRotate\n" );

  gfl2::math::VEC3 nowPos( 0, 0, 0 );

  // 角度を計算
  f32 degree = _GetDegreeThreePoint_Player( m_pPlayerArrowPane, tp, &nowPos );

  // 回転値をペインに反映
  gfl2::math::VEC3 vecRot = m_pUtil->GetPaneRotate( m_pPlayerArrowPane );
  vecRot.z += degree;
  if( vecRot.z >= 360.0f ) { vecRot.z -= 360.0f; }
  if( vecRot.z <= -360.0f ) { vecRot.z += 360.0f; }
  m_pUtil->SetPaneRotate( m_pPlayerArrowPane, vecRot );

  // 回転値をViewerに反映
  f32 viewerDegree = ( vecRot.z + 90.0f ) - 180.0f;
  m_bCollision = m_pListener->SetPlayerRotate( viewerDegree );

  // 衝突時の色変更
  _SetPlayerAABBAreaColor( m_bCollision );

  m_vecTouchRotate = nowPos;
}

/**
 * @brief   プレイヤー回転の終了
 */
void LayoutView::_FinishPlayerRotate( void )
{
  FUKUSHIMA_PRINT( "LayoutView::_FinishPlayerRotate\n" );

  // Viewerの回転値にUI側もあわせる
  _SetViewerPlayerRotate();

  m_vecTouchRotate = gfl2::math::VEC3::Zero();
  m_TouchState = TOUCH_STATE_NONE;
}


/**
 * @brief   プレイヤーAABBの中心座標の取得
 */
gfl2::math::VEC3 LayoutView::_GetPlayerAABBAreaCenter( void )
{
  FUKUSHIMA_PRINT( "_GetPlayerAABBAreaCenter\n" );

  gfl2::math::AABB aabb = m_pListener->GetPlayerAABB();
  gfl2::math::Vector3 vecAABBMin( aabb.GetMin().x, aabb.GetMax().z, 0 );

  FUKUSHIMA_PRINT( " AABB min : x=[%.1f] y=[%.1f]\n", vecAABBMin.x, vecAABBMin.y );

  vecAABBMin.x *= Static::MOVE_DIAMETER_RATE;
  vecAABBMin.y *= Static::MOVE_DIAMETER_RATE;

  FUKUSHIMA_PRINT( " AABB min(UI) : x=[%.1f] y=[%.1f]\n", vecAABBMin.x, vecAABBMin.y );

  nw::lyt::Size areaSize = m_pUtil->GetPaneSize( m_pPlayerAreaPane );

  gfl2::math::VEC3 center( 0, 0, 0 );
  center.x = gfl2::math::FAbs( vecAABBMin.x ) - ( areaSize.width / 2.0f );
  center.y = gfl2::math::FAbs( vecAABBMin.y ) - ( areaSize.height / 2.0f );

  FUKUSHIMA_PRINT( " center(UI) : x=[%.1f] y=[%.1f]\n", center.x, center.y );

  return center;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void LayoutView::Show( void )
{
  this->SetModal( true );
  this->_Show();
}

/**
 * @brief   非表示
 */
void LayoutView::Hide( void )
{
  SetLayoutType( TYPE_NULL );

  this->SetModal( false );
  this->_Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   設定：タイプ(ポケモン位置、ポケモン回転、プレイヤー回転)
 *
 * @param   type    レイアウトタイプ
 */
void LayoutView::SetLayoutType( LayoutType type )
{
  FUKUSHIMA_PRINT( "LayoutView::SetLayoutType\n" );

  m_LayoutType = type;

  if( type != TYPE_NULL )
  {
    // 初期設定
    _SetInitLayout();

    // 全ペインを非表示
    _AllPaneInvisible();
    // 撮影モード
    _SetCaptureMode();
    // レイアウトタイプの設定
    _SetLayoutType( type );
    // メッセージウィンドウの更新
    _UpdateMessage();
  }
}

/**
 * @brief   設定：初期設定
 */
void LayoutView::_SetInitLayout( void )
{
  FUKUSHIMA_PRINT("LayoutView::_SetInitLayout\n");

  // カメラの回転値(Y軸)を取得
  _SetCameraRotate();

  // Viewer側の位置座標を設定
  _SetPokemonPositionFromViewer( true );
  // ポケモンのAABBのサイズを設定
  _SetPokemonAABBAreaSize();
  // ポケモンのAABBの中心点を設定
  _SetPokemonAABBAreaCenter();
  // ポケモンの回転
  _SetPokemonRotate();

  // プレイヤーのAABBのサイズを設定
  _SetPlayerAABBAreaSize();
  // プレイヤーのAABBの中心点を設定
  _SetPlayerAABBAreaCenter();
  // プレイヤーの回転
  _SetPlayerRotate();
}

/**
 * @brief   設定：撮影モードの対応
 */
void LayoutView::_SetCaptureMode( void )
{
  Viewer::CaptureMode mode = m_pWork->prepare.capture_mode;

  FUKUSHIMA_PRINT( "CaptureMode=[%d]\n", mode );

  // 撮影モードに合わせてプレイヤー関連の表示を切り替える
  if( mode == Viewer::CAPTURE_MODE_BOTH )
  {
    _SetVisiblePlayer( true );
  }
  else {
    _SetVisiblePlayer( false );
  }
}

/**
 * @brief   設定：レイアウトタイプ
 */
void LayoutView::_SetLayoutType( LayoutType type )
{
  void( LayoutView::*SET_LAYOUT_TYPE_FUNC[] )( void ) = {
    &LayoutView::_SetLayoutType_PokeMove,
    &LayoutView::_SetLayoutType_PokeRotate,
    &LayoutView::_SetLayoutType_PlayerRotate,
  };

  // レイアウトタイプごとの設定
  ( this->*SET_LAYOUT_TYPE_FUNC[type] )( );

  // 帯の表示切替
  _SetLayoutType_ObiPane( type );
}

/**
 * @brief   設定：ポケモン移動
 */
void LayoutView::_SetLayoutType_PokeMove( void )
{
  FUKUSHIMA_PRINT("LayoutView::_SetLayoutType_PokeMove\n");

  if( _IsTwoShoot() )
  {
    m_pUtil->SetVisiblePane( m_pPlayerAreaPane, true );
  }
  m_pUtil->SetVisiblePane( m_pPokeAreaPane, true );
}

/**
 * @brief   設定：ポケモン回転
 */
void LayoutView::_SetLayoutType_PokeRotate( void )
{
  FUKUSHIMA_PRINT( "LayoutView::_SetLayoutType_PokeRotate\n" );

  if( _IsTwoShoot() )
  {
    m_pUtil->SetVisiblePane( m_pPlayerAreaPane, true );
  }
  m_pUtil->SetVisiblePane( m_pPokeAreaPane, true );

  m_pUtil->SetVisiblePane( m_pPokeArrowPane, true );
}

/**
 * @brief   設定：プレイヤー回転
 */
void LayoutView::_SetLayoutType_PlayerRotate( void )
{
  FUKUSHIMA_PRINT( "LayoutView::_SetLayoutType_PlayerRotate\n" );

  m_pUtil->SetVisiblePane( m_pPlayerAreaPane, true );
  m_pUtil->SetVisiblePane( m_pPokeAreaPane, true );

  m_pUtil->SetVisiblePane( m_pPlayerArrowPane, true );
}

/**
 * @brief   プレイヤーの表示切替
 *
 * @param   visible   表示フラグ
 */
void LayoutView::_SetVisiblePlayer( bool visible )
{
  m_pUtil->SetVisiblePane( m_pPlayerNullPane, visible );
  m_pUtil->SetVisiblePane( m_pPlayerAreaPane, false );
  m_pUtil->SetVisiblePane( m_pPlayerArrowPane, false );
}

/**
 * @brief   帯の表示切替
 *
 * @param[in]   type    レイアウトタイプ
 */
void LayoutView::_SetLayoutType_ObiPane( LayoutType type )
{
  const LayoutType c_LayoutTypeTable[] = {
    TYPE_POKE_MOVE,
    TYPE_POKE_ROTATE,
    TYPE_PLAYER_ROTATE,
  };

  for( u32 i = 0; i < TYPE_NUM; ++i )
  {
    bool visible = (c_LayoutTypeTable[i] == type);

    // 表示切替
    m_pUtil->SetVisiblePane( m_pObiPane[i], visible );
  }
}

/**
 * @brief   全ペインの非表示
 */
void LayoutView::_AllPaneInvisible( void )
{
  gfl2::lyt::LytPane* paneList[] = {
    m_pPlayerNullPane,
    m_pPlayerAreaPane,
    m_pPlayerArrowPane,
    m_pPokeAreaPane,
    m_pPokeArrowPane,
  };

  for( u32 i = 0; i < GFL_NELEMS( paneList ); ++i )
  {
    m_pUtil->SetVisiblePane( paneList[i], false );
  }
}

/**
 * @brief   カメラの回転値(Y軸)を設定
 */
void LayoutView::_SetCameraRotate( void )
{
  // カメラの回転値を取得
  f32 cameraRotate = m_pListener->GetCameraRotateY();
  m_TurnTableRotate = ( cameraRotate + 180.0f );
  if( m_TurnTableRotate > 360.0f ) m_TurnTableRotate -= 360.0f;

  FUKUSHIMA_PRINT(
    "_SetCameraRotate : CameraRotate=[%.1f] TurnTableRotate=[%.1f]\n"
    , cameraRotate, m_TurnTableRotate );

  // 回転盤を回転
  {
    gfl2::math::VEC3 rotate = m_pUtil->GetPaneRotate( m_pTurnTablePane );
    rotate.z = m_TurnTableRotate * -1.0f;
    m_pUtil->SetPaneRotate( m_pTurnTablePane, rotate );
  }

  // ポケモンアイコンを逆回転
  {
    gfl2::math::VEC3 rotate = m_pUtil->GetPaneRotate( m_pPokeIconPane );
    rotate.z = m_TurnTableRotate;
    m_pUtil->SetPaneRotate( m_pPokeIconPane, rotate );
  }

  // ポケモン用の当たり判定を逆回転
  {
    gfl2::math::VEC3 rotate = m_pUtil->GetPaneRotate( m_pPokeBoundPane );
    rotate.z = m_TurnTableRotate;
    m_pUtil->SetPaneRotate( m_pPokeBoundPane, rotate );
  }
}

/**
 * @brief   AABBエリアカラー設定
 *
 * @param   bCollision  衝突フラグ
 */
void LayoutView::_SetPokemonAABBAreaColor( bool bCollision )
{
  if( !bCollision )
  {
    m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00_POKECOLOR );
  }
  else {
    m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_LAYOUT_LOW_00_POKECOLOR_00 );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンバート
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   AABBを2D用サイズに変換
 */
nw::lyt::Size LayoutView::_ConvertBoundSizeFromAABB( gfl2::math::AABB aabb )
{
  f32 width = gfl2::math::FAbs( aabb.GetMax().x - aabb.GetMin().x );
  f32 height = gfl2::math::FAbs( aabb.GetMax().z - aabb.GetMin().z );

  return nw::lyt::Size( ( width * Static::MOVE_DIAMETER_RATE ), ( height * Static::MOVE_DIAMETER_RATE ) );
}

/**
 * @brief   座標系の変換：スクリーン座標からレイアウト座標へ
 */
gfl2::math::VEC3 LayoutView::_ConvertVector_ScreenToLayout( gfl2::math::Vector3 vec )
{
  gfl2::math::VEC3 vecLayout( vec.x, vec.y, 0.0f );
  {
    vecLayout.x = vecLayout.x - 160.0f;
    vecLayout.y = ( vecLayout.y - 120.0f ) * -1.0f;
  }

  return vecLayout;
}

/**
 * @brief   座標系の変換：レイアウト座標からスクリーン座標へ
 */
gfl2::math::Vector3 LayoutView::_ConvertVector_LayoutToScreen( gfl2::math::VEC3 vec )
{
  gfl2::math::Vector3 vecScreen( vec.x, vec.y, 0.0f );
  {
    vecScreen.x = vecScreen.x + 160.0f;
    vecScreen.y = 120.0f - vecScreen.y;
  }

  return vecScreen;
}


/**
 * @brief   ３点の座標から角度を計算
 *
 * @retval  角度
 */
f32 LayoutView::_GetDegreeThreePoint_Pokemon( gfl2::lyt::LytPane* center, gfl2::ui::TouchPanel* tp, gfl2::math::VEC3* dstNowPos )
{
  // 現在のタッチ座標
  gfl2::math::Vector3 vecScreen( tp->GetX(), tp->GetY(), 0.0f );
  gfl2::math::VEC3 nowPos = _ConvertVector_ScreenToLayout( vecScreen );

  // 親ペインの回転情報を取得
  const gfl2::math::MTX34 turnTableMtx = m_pTurnTablePane->GetGlobalMtx();

  gfl2::math::VEC3 srcPos = center->GetTranslate();
  gfl2::math::VEC3 dstPos( 0, 0, 0 );
  gfl2::math::VEC3Transform( &dstPos, &turnTableMtx, &srcPos );

  return _GetDegreeThreePoint( dstPos, m_vecTouchRotate, nowPos, dstNowPos );
}

/**
 * @brief   ３点の座標から角度を計算
 *
 * @retval  角度
 */
f32 LayoutView::_GetDegreeThreePoint_Player( gfl2::lyt::LytPane* center, gfl2::ui::TouchPanel* tp, gfl2::math::VEC3* dstNowPos )
{
  // 現在のタッチ座標
  gfl2::math::Vector3 vecScreen( tp->GetX(), tp->GetY(), 0.0f );
  gfl2::math::VEC3 nowPos = _ConvertVector_ScreenToLayout( vecScreen );

  gfl2::math::VEC3 centerPos = center->GetTranslate();

  return _GetDegreeThreePoint( centerPos, m_vecTouchRotate, nowPos, dstNowPos );
}

/**
 * @brief   ３点の座標から角度を計算
 *
 * @param   center    中心座標
 * @param   lastPos   前のタッチ座標
 * @param   nowPos    現在のタッチ座標
 *
 * @retval  角度
 */
f32 LayoutView::_GetDegreeThreePoint( gfl2::math::VEC3 center, gfl2::math::VEC3 lastPoint, gfl2::math::VEC3 nowPoint, gfl2::math::VEC3* dstNowPos )
{
  // ベクトルを計算：center -> lastPoint
  gfl2::math::VEC3 vec1 = center - lastPoint;
  // ベクトルを計算：center -> nowPoint
  gfl2::math::VEC3 vec2 = center - nowPoint;

  // 外積を計算：(vec1 * vec2)
  gfl2::math::VEC3 vecCross( 0, 0, 0 );
  gfl2::math::VEC3Cross( &vecCross, &vec1, &vec2 );

  // 内積を計算：(vec1・vec2)
  f32 dot = gfl2::math::VEC3Dot( &vec1, &vec2 );

  // 逆正接(角度)：(外積, 内積)
  f32 degree = gfl2::math::Atan2Deg( vecCross.z, dot );

  FUKUSHIMA_PRINT( "LayoutView::_GetDegreeThreePoint：degree=[%.1f]\n", degree );

  if( dstNowPos )
  {
    *dstNowPos = nowPoint;
  }

  return degree;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージウィンドウ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージウィンドウ：表示
 */
void LayoutView::_ShowMessageWindow( void )
{
  // メッセージウィンドウが開いていないか？
  if( !_IsShowMessageWindow() )
  {
    // メッセージウィンドウを表示
    m_pUtil->SetVisiblePane( m_pMessageWindowParts, true );
    // メッセージウィンドウの表示フラグを更新
    _SetShowMessageWindow( true );
  }
}

/**
 * @brief   メッセージウィンドウ：非表示
 */
void LayoutView::_HideMessageWindow( void )
{
  // メッセージウィンドウが開いているか？
  if( _IsShowMessageWindow() )
  {
    // メッセージウィンドウを非表示
    m_pUtil->SetVisiblePane( m_pMessageWindowParts, false );
    // メッセージウィンドウの表示フラグを更新
    _SetShowMessageWindow( false );
  }
}

/**
 * @brief   メッセージウィンドウ：メッセージタイプの取得
 *
 * @retval  メッセージタイプ
 */
LayoutView::MsgType LayoutView::_GetShowMsgType( void )
{
  if( m_bCollision )
  {
    return MSG_TYPE_COLLISION;
  }
  else {
    // ポケモンのモーション再生割合の取得
    f32 rate = m_pListener->GetPokemonMotionPlayRate();

    if( rate <= 0.0f )
    {
      return MSG_TYPE_POSE_NONE;
    }
    else if( rate <= 0.5f )
    {
      return MSG_TYPE_POSE_LESS;
    }
    else if( rate < 1.0f )
    {
      return MSG_TYPE_POSE_MORE;
    }
    else {
      return MSG_TYPE_POSE_FULL;
    }
  }
}

/**
 * @brief   メッセージウィンドウ：メッセージ更新
 */
void LayoutView::_UpdateMessage( void )
{
  // メッセージタイプの取得
  MsgType msgType = _GetShowMsgType();

  if( msgType != MSG_TYPE_POSE_FULL )
  {
    // メッセージの設定
    _SetMessage( msgType );

    // メッセージウィンドウを表示
    _ShowMessageWindow();
  }
  else {
    // メッセージウィンドウを非表示
    _HideMessageWindow();
  }
}

/**
 * @brief   メッセージウィンドウ：メッセージ設定
 *
 * @param   msgType   表示するメッセージの種類
 */
void LayoutView::_SetMessage( const MsgType msgType )
{
  if( _GetMsgType() != msgType )
  {
    // メッセージウィンドウを表示
    m_pUtil->SetVisiblePane( m_pMessageWindowTextBox, true );

    // メッセージIDの取得
    u32 messageID = _GetMessageID( msgType );

    // テキストボックスに設定
    this->SetTextboxPaneMessage( m_pMessageWindowTextBox, messageID );

    _SetMsgType( msgType );
  }
}

/**
 * @brief   メッセージウィンドウ：メッセージ設定
 *
 * @param   msgType   表示するメッセージの種類
 */
u32 LayoutView::_GetMessageID( const MsgType msgType )
{
  // 移動
  const u32 c_MoveMsgList[] = {
    fs_photo_win_37,
    fs_photo_win_38,
    fs_photo_win_39,
    fs_photo_win_40,
  };
  // 回転
  const u32 c_RotMsgList[] = {
    fs_photo_win_41,
    fs_photo_win_38,
    fs_photo_win_39,
    fs_photo_win_40,
  };

  // 現在の操作タイプ
  LayoutType nowType = m_LayoutType;

  if( nowType == TYPE_POKE_MOVE )
  {
    return c_MoveMsgList[msgType];
  }
  else if( nowType == TYPE_POKE_ROTATE )
  {
    return c_RotMsgList[msgType];
  }
  else if( nowType == TYPE_PLAYER_ROTATE )
  {
    return c_RotMsgList[msgType];
  }

  GFL_ASSERT(0);    //!< ここへはこない
  return c_MoveMsgList[msgType];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォウィンドウ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   インフォウィンドウの初期化
 */
void LayoutView::_InitInfo( void )
{
  // メッセージ
  const u32 c_InfoMsgList[] = {
    fs_photo_info_16,
    fs_photo_info_17,
    fs_photo_info_18,
  };

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  // インフォ用パラメータの生成
  this->_CreateInfoParam( INFO_PAGE_MAX );

  //if( m_LayoutType == TYPE_PLAYER_ROTATE )
  //{
  //  GameSys::GameData* gd = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  //  Savedata::MyStatus* myStatus = gd->GetPlayerStatus();

  //  // プレイヤー名を登録
  //  g2d->SetRegisterPlayerName( 0, myStatus );
  //}

  // プレイヤー名をセットしたメッセージを取得
  g2d->GetStringExpand( m_pInfoStrArray[0], c_InfoMsgList[m_LayoutType] );

  // テクスチャ
  const u32 c_InfoTexList[] = {
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_12_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_13_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_14_BFLIM,
  };

  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( c_InfoTexList[m_LayoutType] );
}

/**
 * @brief   インフォウィンドウの表示
 */
void LayoutView::_ShowInfo( void )
{
  // 入力を無効に
  this->SetInputEnabled( false );

  // インフォの初期化
  _InitInfo();

  // インフォウィンドウのセットアップ
  Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );
  // ウィンドウコンテンツのセットアップ
  Static::InformationWindow::SetupContents( INFO_PAGE_MAX, m_pInfoStrArray, m_pInfoTexArray );
  // インフォウィンドウの表示
  gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_LAYOUT_LOW_00_PANE_INFOBUTTON );
  Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );

  this->SetResult( RES_INFO_SHOW );
}

/**
 * @brief   チュートリアルの表示
 */
void LayoutView::_ShowTutorial( void )
{
  // チュートリアルを表示するかどうか
  if( _IsShowTutorial() )
  {
    _ShowInfo();
  }
}

/**
 * @brief   チュートリアルの表示するかどうか
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool LayoutView::_IsShowTutorial( void )
{
  const Savedata::FinderStudioSave::TutorialState c_TutorialState[] = {
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_POKEMON_MOVE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_POKEMON_ROTATE,
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_TRAINER_ROTATE,
  };

  GFL_ASSERT( m_LayoutType < TYPE_NUM );

  // チュートリアルステート
  Savedata::FinderStudioSave::TutorialState state = c_TutorialState[m_LayoutType];

  // チュートリアルステートの確認
  if( !m_pFinderStudioSave->IsTutorialState( state ) )
  {
    // チュートリアルステートの更新
    m_pFinderStudioSave->SetTutorialState( state );

    return true;
  }

  return false;
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック用
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ツーショットモードかどうか
 *
 * @retval  "true"ならツーショット
 * @retval  "false"ならポケモンのみ
 */
bool LayoutView::_IsTwoShoot( void )
{
  return ( m_pWork->prepare.capture_mode == Viewer::CAPTURE_MODE_BOTH );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    タッチイベント
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   タッチイベント：ポケモン移動
 */
void LayoutView::_TouchEventPokeMove( gfl2::ui::TouchPanel* touchPanel )
{
  FUKUSHIMA_PRINT( "_TouchEventPokeMove\n" );

  GFL_ASSERT( m_LayoutType == TYPE_POKE_MOVE );

  if( m_PokeMoveState == POKE_MOVE_STATE_NONE )
  {
    /// ボタンアクション中は何もしない
    if( m_pUtil->IsButtonAction() )
    {
      return;
    }

    app::util::G2DUtil* g2d = m_pUtil->GetG2D();

    if( g2d->IsPaneTouchTrigger( 0, m_pPokeBoundPane ) )
    {
      m_PokeMoveState = POKE_MOVE_STATE_MOVING; //!< 移動中へ
    }
  }
  if( m_PokeMoveState == POKE_MOVE_STATE_MOVING )
  {
    // タッチ中
    if( touchPanel->IsTouch() )
    {
      f32 touchX = static_cast<f32>( touchPanel->GetX() );
      f32 touchY = static_cast<f32>( touchPanel->GetY() );

      gfl2::math::Vector3 vecScreen( touchX, touchY, 0.0f );
      gfl2::math::VEC3 vecLayout = _ConvertVector_ScreenToLayout( vecScreen );

      // ポケモンの移動
      _SetPokemonPosition( vecLayout );

      m_UpdateState = UPD_STATE_POKE_MOVE;
    }
    // リリース
    else {
      // ポケモンの移動
      _SetPokemonPositionFromViewer();

      m_PokeMoveState = POKE_MOVE_STATE_NONE;
    }
  }
}

/**
 * @brief   タッチイベント：ポケモン回転
 */
void LayoutView::_TouchEventPokeRotate( gfl2::ui::TouchPanel* touchPanel )
{
  FUKUSHIMA_PRINT( "_TouchEventPokeRotate\n" );

  GFL_ASSERT( m_LayoutType == TYPE_POKE_ROTATE );

  if( m_TouchState == TOUCH_STATE_NONE )
  {
    /// ボタンアクション中は何もしない
    if( m_pUtil->IsButtonAction() )
    {
      return;
    }

    if( touchPanel->IsTouchTrigger() )
    {
      // ポケモンの回転を開始
      _StartPokemonRotate( touchPanel );
    }
  }
  else if( m_TouchState == TOUCH_STATE_ROTATE )
  {
    if( touchPanel->IsTouch() )
    {
      // ポケモンの回転を更新
      _DragPokemonRotate( touchPanel );
    }
    else {
      // ポケモンの回転を終了
      _FinishPokemonRotate();
    }
  }
}

/**
 * @brief   タッチイベント：プレイヤー回転
 */
void LayoutView::_TouchEventPayerRotate( gfl2::ui::TouchPanel* touchPanel )
{
  FUKUSHIMA_PRINT( "_TouchEventPayerRotate\n" );

  GFL_ASSERT( m_LayoutType == TYPE_PLAYER_ROTATE );

  if( m_TouchState == TOUCH_STATE_NONE )
  {
    /// ボタンアクション中は何もしない
    if( m_pUtil->IsButtonAction() )
    {
      return;
    }

    if( touchPanel->IsTouchTrigger() )
    {
      // プレイヤーの回転を開始
      _StartPlayerRotate( touchPanel );
    }
  }
  else if( m_TouchState == TOUCH_STATE_ROTATE )
  {
    if( touchPanel->IsTouch() )
    {
      // プレイヤーの回転を更新
      _DragPlayerRotate( touchPanel );
    }
    else {
      // プレイヤーの回転を終了
      _FinishPlayerRotate();
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    キーアクション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   キーアクション：ポケモン移動
 */
void LayoutView::_KeyActionPokeMove( gfl2::ui::Button* button )
{
  FUKUSHIMA_PRINT( "_KeyActionPokeMove\n" );

  const f32 POKEMON_MOVE_SPEED = 1.0f;
  gfl2::math::Vector3 pos = m_PokemonPosition;

  if( button->IsHold( gfl2::ui::BUTTON_UP ) )
  {
    pos.y -= POKEMON_MOVE_SPEED;
    m_InputType = INPUT_TYPE_KEY;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_DOWN ) )
  {
    pos.y += POKEMON_MOVE_SPEED;
    m_InputType = INPUT_TYPE_KEY;
  }

  if( button->IsHold( gfl2::ui::BUTTON_LEFT ) )
  {
    pos.x -= POKEMON_MOVE_SPEED;
    m_InputType = INPUT_TYPE_KEY;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_RIGHT ) )
  {
    pos.x += POKEMON_MOVE_SPEED;
    m_InputType = INPUT_TYPE_KEY;
  }

  if( m_InputType == INPUT_TYPE_KEY )
  {
    gfl2::math::VEC3 vecLayout = _ConvertVector_ScreenToLayout( pos );

    // ポケモンを移動
    _SetPokemonPosition( vecLayout );

    m_UpdateState = UPD_STATE_POKE_MOVE;
  }
}

/**
 * @brief   キーアクション：ポケモン回転
 */
void LayoutView::_KeyActionPokeRotate( gfl2::ui::Button* button )
{
  FUKUSHIMA_PRINT( "_KeyActionPokeRotate\n" );

  if( button->IsHold( gfl2::ui::BUTTON_LEFT ) )
  {
    // ポケモンを回転(左)
    m_bCollision = m_pListener->PokemonRotateLeft();
    _SetPokemonAABBAreaColor( m_bCollision );

    m_UpdateState = UPD_STATE_POKE_ROTATE;
    m_InputType = INPUT_TYPE_KEY;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_RIGHT ) )
  {
    // ポケモンを回転(右)
    m_bCollision = m_pListener->PokemonRotateRight();
    _SetPokemonAABBAreaColor( m_bCollision );

    m_UpdateState = UPD_STATE_POKE_ROTATE;
    m_InputType = INPUT_TYPE_KEY;
  }
}

/**
 * @brief   キーアクション：プレイヤー回転
 */
void LayoutView::_KeyActionPayerRotate( gfl2::ui::Button* button )
{
  FUKUSHIMA_PRINT( "_KeyActionPokeMove\n" );

  if( button->IsHold( gfl2::ui::BUTTON_LEFT ) )
  {
    // プレイヤーを回転(左)
    m_bCollision = m_pListener->PlayerRotateLeft();
    _SetPlayerAABBAreaColor( m_bCollision );

    m_UpdateState = UPD_STATE_PLAYER_ROTATE;
    m_InputType = INPUT_TYPE_KEY;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_RIGHT ) )
  {
    // プレイヤーを回転(右)
    m_bCollision = m_pListener->PlayerRotateRight();
    _SetPlayerAABBAreaColor( m_bCollision );

    m_UpdateState = UPD_STATE_PLAYER_ROTATE;
    m_InputType = INPUT_TYPE_KEY;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：ペインアクション開始イベント
 */
void LayoutView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  if( paneId == BTN_ID_INFO )
  {
    // 共通ボタンをロック    // @fix MMCat[634]
    m_pCommonView->SetButtonLock();
  }
}

/**
 * @brief   UIInputListener：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult LayoutView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_INFO )
  {
    m_PokeMoveState = POKE_MOVE_STATE_NONE;
    m_TouchState = TOUCH_STATE_NONE;
    m_vecTouchRotate = gfl2::math::VEC3::Zero();

    // インフォの表示
    _ShowInfo();
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：タッチイベント
 */
app::ui::UIInputListener::ListenerResult LayoutView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  FUKUSHIMA_PRINT( "OnKeyAction : LayoutType=[%d]\n", m_LayoutType );

  void ( LayoutView::*TOUCH_EVENT_FUNC[] )( gfl2::ui::TouchPanel* ) = {
    &LayoutView::_TouchEventPokeMove,
    &LayoutView::_TouchEventPokeRotate,
    &LayoutView::_TouchEventPayerRotate,
  };

  if( isTouch )
  {
    m_InputType = INPUT_TYPE_TOUCH;
  }
  else {
    m_InputType = INPUT_TYPE_NONE;
  }

  if( m_InputType != INPUT_TYPE_KEY )
  {
    ( this->*TOUCH_EVENT_FUNC[m_LayoutType] )( pTouchPanel );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：キーアクションイベント
 */
app::ui::UIInputListener::ListenerResult LayoutView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  FUKUSHIMA_PRINT( "OnKeyAction : LayoutType=[%d]\n", m_LayoutType );

  void ( LayoutView::*KEY_ACTION_FUNC[] )( gfl2::ui::Button* ) = {
    &LayoutView::_KeyActionPokeMove,
    &LayoutView::_KeyActionPokeRotate,
    &LayoutView::_KeyActionPayerRotate,
  };

  if( m_InputType != INPUT_TYPE_TOUCH )
  {
    ( this->*KEY_ACTION_FUNC[m_LayoutType] )( pButton );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   InfoWindowBookTypeListener：ウィンドウを閉じきったイベント
 */
void LayoutView::OnClose( void )
{
  Static::InformationWindow::Hide();

  // インフォ用パラメータの破棄
  this->_DeleteInfoParam();

  // 共通ボタンをアンロック    // @fix MMCat[634]
  m_pCommonView->SetButtonUnlock();

  this->SetInputEnabled( true );
  this->SetResult( RES_INFO_HIDE );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
