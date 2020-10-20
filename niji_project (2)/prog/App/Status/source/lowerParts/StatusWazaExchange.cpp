//======================================================================
/**
 * @file    StatusWazaExchange.cpp
 * @date    2015/12/09 17:23:13
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技入れ替え(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusWazaExchange.h"

#include <App/Status/source/StatusDrawerBase.h>
#include <App/Status/source/StatusDrawListener.h>
#include <App/Status/source/StatusLowerDraw.h>

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>
#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
enum AppendLevel {
  APPEND_LV_LOW,
  APPEND_LV_MIDDLE,
  APPEND_LV_HIGH,
  APPEND_LV_NUM,
};

static const u32 c_UnselectAnim[] = {
  LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_SELECT_STOP,
  LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_SELECT_STOP,
  LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_SELECT_STOP,
  LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_SELECT_STOP,
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
WazaExchange::WazaExchange( DrawerBase* base )
  : m_pBase( base )
  , m_pMsgWndListener(NULL)
  , m_pPokeParam(NULL)
  , m_Seq(SEQ_INIT)
  , m_MoveIndex(PLATE_MAX)
  , m_DestIndex(PLATE_MAX)
  , m_isDrag(false)
  , m_isRelease(false)
  , m_SelectAnim(0xFFFF)
  , m_TouchPlate(PLATE_MAX)
{
  m_TriggerPlateID = PLATE_MAX;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   initParam   初期化パラメータ
 */
//------------------------------------------------------------------------------
void WazaExchange::Initialize( InitParam& initParam )
{
  // リスナーの設定
  m_pCursorListener   = initParam.curListener;
  m_pMsgWndListener   = initParam.msgListner;
  m_pExplainListener  = initParam.expListener;
  m_uiListener        = initParam.uiListener;

  // ペインの設定
  {
    const u32 c_PlateParts[PLATE_MAX] = {
      PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_00,
      PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_01,
      PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_02,
      PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_03,
    };

    for( u8 i=0; i<PLATE_MAX; ++i )
    {
      m_PlateData[i].parts    = m_pBase->GetParts( c_PlateParts[i] );
      m_PlateData[i].bound    = m_pBase->GetPane( PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00, m_PlateData[i].parts );
      m_PlateData[i].defIdx   = i;
      m_PlateData[i].defPos   = m_pBase->GetPanePos( m_PlateData[i].parts );
      m_PlateData[i].excFlg   = false;
      m_PlateData[i].dragFlg  = false;

      // 初期データのセット
      m_DefPlateData[i] = m_PlateData[i];
    }
  }

  m_Seq = SEQ_IDLE;
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaExchange::Setup( pml::pokepara::PokemonParam* pp )
{
  m_pPokeParam = pp;

  // 初期化
  for( u8 i=0; i<PLATE_MAX; ++i )
  {
    m_PlateData[i] = m_DefPlateData[i];

    m_pBase->SetPanePos( m_PlateData[i].parts, m_PlateData[i].defPos );
  }

  // プレートデータのセット
  if( m_pCursorListener ) m_pCursorListener->SetPlateData( m_PlateData );
}



//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void WazaExchange::Update( void )
{
  // 初期位置へバインド
  UpdateBind();
  // ドラッグ位置の更新
  UpdateDragPlate();

  switch( m_Seq )
  {
  case SEQ_EXCHANGE:
    {
      // 入れ替えが完了したか
      if( IsExchange() )
      {
        End();    //!< 入れ替えの終了処理
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期位置へバインド
 */
//------------------------------------------------------------------------------
void WazaExchange::UpdateBind( void )
{
  for( u8 i=0; i<PLATE_MAX; ++i )
  {
    // 初期位置へバインド
    _UpdateBind( i );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期位置へバインド
 *
 * @param   plateIndex    プレートインデックス
 */
//------------------------------------------------------------------------------
void WazaExchange::_UpdateBind( u32 plateIndex )
{
  // 対象のプレートデータ
  PlateData* data = &m_PlateData[plateIndex];

  if( !data->excFlg || data->dragFlg )
  {
    // 入れ替え対象以外 or ドラッグ対象は何もしない
    return;
  }

  // 現在位置の取得
  gfl2::math::VEC3 now = m_pBase->GetPanePos( data->parts );

  // 元の位置までのベクトル
  gfl2::math::VEC3 move = data->defPos - now;

  // 移動量の設定
  if( !data->moveSet )
  {
    data->moveVec   = move;
    data->moveVec.y = (move.y / 5.0f);
    data->moveSet   = true;
  }

  // 次の座標の計算
  //gfl2::math::VEC3 next = now + (move *  0.4f);
  gfl2::math::VEC3 next = now + data->moveVec;

  // 補正
  u32 flg = 0;
  {
    const f32 margin = 0.1f;

    if( gfl2::math::FAbs(move.x) < margin )
    {
      next.x = data->defPos.x;
      flg |= 1;
    }
    if( gfl2::math::FAbs(move.y) < margin )
    {
      next.y = data->defPos.y;
      flg |= 2;
    }
    next.z = 0.0f;
  }

  // 座標値の更新
  m_pBase->SetPanePos( data->parts, next );

  // 初期位置まで戻った
  if( flg == 3 )
  {
    // プレートを元のプライオリティに戻す
    AppendChildPane( data->defIdx, APPEND_LV_LOW );
    // 入れ替えフラグを戻す
    data->excFlg = false;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   対象のプレートをセット
 *
 * @param   move_plate    初期のプレートインデックス
 * @param   drag_flag     ドラッグでの入れ替えかどうか
 */
//------------------------------------------------------------------------------
void WazaExchange::SetPlate_Init( u32 plateIndex, bool dragFlag )
{
  for( u32 i=0; i<PLATE_MAX; ++i )
  {
    if( m_PlateData[i].defIdx == plateIndex )
    {
      SetPlate( i, dragFlag );

      break;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   対象のプレートをセット
 *
 * @param   plateIndex    現在のプレートインデックス
 * @param   drag_flag     ドラッグでの入れ替えかどうか
 */
//------------------------------------------------------------------------------
void WazaExchange::SetPlate_Now( u32 plateIndex, bool dragFlag )
{
  SetPlate( plateIndex, dragFlag );
}

//------------------------------------------------------------------------------
/**
 * @brief   対象のプレートをセット
 *
 * @param   move_plate    移動元のプレートインデックス
 * @param   drag_flag     ドラッグでの入れ替えかどうか
 */
//------------------------------------------------------------------------------
void WazaExchange::SetPlate( u32 plateIndex, bool dragFlag )
{
  if( m_Seq == SEQ_IDLE )
  {
    FUKUSHIMA_PRINT( "SetPlate : MovePlate[%d]\n", plateIndex );

    m_MoveIndex = plateIndex;
    m_DestIndex = PLATE_MAX;
    m_isDrag    = dragFlag;
    m_WazaNo    = GetWazaNo( m_MoveIndex );
    
    // ドラッグフラグを更新
    if( m_isDrag ) m_PlateData[m_MoveIndex].dragFlg = true;

    // 入れ替え元のプライオリティを一番高く設定
    AppendChildPane( m_PlateData[m_MoveIndex].defIdx, APPEND_LV_HIGH );

    // 選択アニメの再生
    StartPlateSelectAnim( m_PlateData[m_MoveIndex].defIdx );

    // メッセージの設定
    m_pMsgWndListener->SetMessageID( PARAM_WAZA_CHANGE_02 );

    m_Seq = SEQ_DEST;
  }
  else if( m_Seq == SEQ_DEST )
  {
    FUKUSHIMA_PRINT( "SetPlate : DestPlate[%d]\n", plateIndex );

    m_DestIndex = plateIndex;

    // 入れ替え先のプライオリティを中間に設定
    AppendChildPane( m_PlateData[m_DestIndex].defIdx, APPEND_LV_MIDDLE );

    // 入れ替え処理
    Exchange();

    m_Seq = SEQ_EXCHANGE;

    if( m_isDrag )
    {      
      // 位置の更新
      m_MoveIndex = m_DestIndex;

      // ドラッグ処理の場合は離されるまでは処理を続ける
      m_Seq = SEQ_DEST;
    }
    else {
      // 選択アニメの停止
      StartPlateUnselectAnim( m_PlateData[m_DestIndex].defIdx );

      SetParentInputEnable( false );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入れ替えの終了
 */
//------------------------------------------------------------------------------
void WazaExchange::End( void )
{
  // 親ビューの入力を有効に戻す
  SetParentInputEnable( true );

  // メッセージの設定
  m_pMsgWndListener->SetMessageID( PARAM_WAZA_CHANGE_01 );

  // 技説明の更新
  m_pExplainListener->SetWazaExplain( m_WazaNo );

  // カーソル位置を変更
  if( m_DestIndex != PLATE_MAX )
  {
    m_pCursorListener->SetPosCursor( m_DestIndex );
  }
  else if( m_MoveIndex != PLATE_MAX )
  {
    m_pCursorListener->SetPosCursor( m_MoveIndex );
  }

  // カーソルを表示
  m_pCursorListener->SetVisibleCursor( true );

  // モードを技をみるモードに戻す
  m_uiListener->SetDispMode( DISP_MODE_WAZA );

  // リセット
  Reset();
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのリセット
 */
//------------------------------------------------------------------------------
void WazaExchange::Reset( void )
{
  m_WazaNo    = WAZANO_NULL;
  m_MoveIndex = PLATE_MAX;
  m_DestIndex = PLATE_MAX;
  m_Seq       = SEQ_IDLE;

  {
    // 選択アニメを停止
    m_pBase->StopAnim( m_SelectAnim );

    m_SelectAnim = 0xFFFF;

    // 非選択アニメの再生
    for( u32 i=0; i<PLATE_MAX; ++i )
    {
      m_pBase->StartAnim( c_UnselectAnim[i] );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   入れ替え
 */
//------------------------------------------------------------------------------
void WazaExchange::Exchange( void )
{
  GFL_ASSERT( m_MoveIndex < PLATE_MAX );
  GFL_ASSERT( m_DestIndex < PLATE_MAX );

  // 入れ替えフラグを更新：プレートのデフォルト位置へのバインドが開始される
  {
    if( !m_PlateData[m_MoveIndex].dragFlg )
    {
      m_PlateData[m_MoveIndex].excFlg  = true;
      m_PlateData[m_MoveIndex].moveSet = false;
    }
    m_PlateData[m_DestIndex].excFlg  = true;
    m_PlateData[m_DestIndex].moveSet = false;
  }

  // プレートデータの入れ替え
  {
    // 初期位置
    gfl2::math::VEC3 tmpVec = m_PlateData[m_MoveIndex].defPos;
    m_PlateData[m_MoveIndex].defPos = m_PlateData[m_DestIndex].defPos;
    m_PlateData[m_DestIndex].defPos = tmpVec;

    // プレートデータの入れ替え
    PlateData tmpPlate = m_PlateData[m_MoveIndex];
    m_PlateData[m_MoveIndex] = m_PlateData[m_DestIndex];
    m_PlateData[m_DestIndex] = tmpPlate;
  }

  // 技の入れ替え(ポケモンパラメータ)
  bool fastmode_flag = m_pPokeParam->StartFastMode();
  {
    m_pPokeParam->ExchangeWazaPos( m_MoveIndex, m_DestIndex );
  }
  m_pPokeParam->EndFastMode( fastmode_flag );

  // プレートデータのセット
  if( m_pCursorListener ) m_pCursorListener->SetPlateData( m_PlateData );
}

//------------------------------------------------------------------------------
/**
 * @brief   入れ替えの終了判定
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool WazaExchange::IsExchange( void )
{
  // ドラッグ処理ではない
  if( !m_isDrag )
  {
    PlateData move = m_PlateData[m_MoveIndex];
    PlateData dest = m_PlateData[m_DestIndex];

    // 移動元、移動先の入れ替えフラグがfalseになれば終了
    if( !move.excFlg && !dest.excFlg )
    {
      return true;
    }
  }
  // ドラッグ処理
  else {
    return m_isRelease;
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   親ビューの入力切替
 *
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void WazaExchange::SetParentInputEnable( bool enable )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  // 入力の切り替え
  view->SetInputEnabled( enable );
}



//------------------------------------------------------------------------------
/**
 * @brief   プレート選択アニメの開始
 *
 * @param   plateIndex    対象プレートインデックス
 */
//------------------------------------------------------------------------------
void WazaExchange::StartPlateSelectAnim( u32 plateIndex )
{
  static const u32 c_SelectAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_SELECT,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_SELECT,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_SELECT,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_SELECT,
  };

  if( plateIndex < GFL_NELEMS(c_SelectAnim) )
  {
    if( m_SelectAnim != 0xFFFF )
    {
      m_pBase->StopAnim( m_SelectAnim );
    }

    m_SelectAnim = c_SelectAnim[plateIndex];

    m_pBase->StartAnim( m_SelectAnim );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   プレート選択解除アニメの開始
 *
 * @param   plateIndex    対象プレートインデックス
 */
//------------------------------------------------------------------------------
void WazaExchange::StartPlateUnselectAnim( u32 plateIndex )
{
  if( plateIndex < GFL_NELEMS(c_UnselectAnim) )
  {
    m_pBase->StopAnim( m_SelectAnim );

    m_SelectAnim = c_UnselectAnim[plateIndex];

    m_pBase->StartAnim( m_SelectAnim );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   子ペインの追加
 *
 * @param   plate_index   対象のプレートインデックス
 * @param   append_pos    追加する場所の指定
 */
//------------------------------------------------------------------------------
void WazaExchange::AppendChildPane( u32 plate_index, u8 append_pos )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();

  // 親ペイン
  const u32 parent_index = PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ALL_EGG;
  // 子ペイン
  const u32 child_index  = PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_00 + plate_index;

  // 追加に必要なデータ
  const struct AppendData
  {
    u32 offset_index;   //!< 追加するペインのインデックス
    u32 mode;           //!< 追加モード
  }
  c_AppendData[APPEND_LV_NUM] = {
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_NULL_00, app::util::G2DUtil::APPEND_CHILD_MODE_INSERT },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_NULL_01, app::util::G2DUtil::APPEND_CHILD_MODE_INSERT },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_NULL_02, app::util::G2DUtil::APPEND_CHILD_MODE_INSERT },
  };

  // 子ペインの追加処理
  const AppendData appendData = c_AppendData[append_pos];
  g2d->AppendChild( 0, parent_index, child_index, appendData.mode, appendData.offset_index );
}


//------------------------------------------------------------------------------
/**
 * @brief   技の取得
 *
 * @param   plateIndex    現在のプレートインデックス
 *
 * @return  技No
 */
//------------------------------------------------------------------------------
WazaNo WazaExchange::GetWazaNo( u32 plateIndex )
{
  GFL_ASSERT( m_pPokeParam );

  WazaNo wazaNo = WAZANO_NULL;

  bool fastmode_flag = m_pPokeParam->StartFastMode();
  {
    GFL_ASSERT( plateIndex < m_pPokeParam->GetWazaCount() );

    // 技の取得
    wazaNo = m_pPokeParam->GetWazaNo( plateIndex );
  }
  m_pPokeParam->EndFastMode( fastmode_flag );

  return wazaNo;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期プレートインデックスの取得
 *
 * @param   plateIndex    現在のプレートインデックス
 *
 * @return  初期インデックス
 */
//------------------------------------------------------------------------------
u32 WazaExchange::GetPlateIndex_Init( u32 plateIndex ) const
{
  return m_PlateData[plateIndex].defIdx;
}

//------------------------------------------------------------------------------
/**
 * @brief   現在のプレートインデックスの取得
 *
 * @param   plateIndex    初期プレートインデックス
 *
 * @return  現在のインデックス
 */
//------------------------------------------------------------------------------
u32 WazaExchange::GetPlateIndex_Now( u32 plateIndex ) const
{
  for( u32 i=0; i<PLATE_MAX; ++i )
  {
    if( m_PlateData[i].defIdx == plateIndex )
    {
      return i;
    }
  }

  // ここへはこない
  GFL_ASSERT(0);
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @brief   タッチパネルのセット
 *
 * @param   touchPanel    タッチパネル
 */
//------------------------------------------------------------------------------
u32 WazaExchange::SetTouchPanel( gfl2::ui::TouchPanel* touchPanel )
{
  if( touchPanel->IsTouchTrigger() )
  {
    // タッチしているプレートのインデックスを取得
    u32 plateIndex = GetTouchPlateIndex( touchPanel );

    // 見つからないので何もしない
    if( plateIndex == PLATE_MAX ) return PLATE_MAX;

    m_TouchPlate = plateIndex;

    return plateIndex;
  }
  // タッチ中
  else if( touchPanel->IsTouch() )
  {
    // タッチ座標の取得
    touchPanel->GetXY( &m_TouchPos );
    {
      m_TouchPos.x = m_TouchPos.x + 160.0f;
      m_TouchPos.y = 120.0f - m_TouchPos.y;
    }

    // タッチしているプレートのインデックスを取得
    u32 plateIndex = GetTouchPlateIndex( touchPanel );

    // 見つからないので何もしない
    if( plateIndex == PLATE_MAX ) return PLATE_MAX;
    if( m_TouchPlate == PLATE_MAX ) return PLATE_MAX;

    if( !m_isDrag )
    {
      gfl2::math::VEC3 margin;
      gfl2::math::VEC3 platePos;

      // タッチ座標をセット
      touchPanel->GetXY( &margin );
      {
        margin.x = margin.x + 160.0f;
        margin.y = 120.0f - margin.y;
      }
      // プレートの座標を取得
      platePos = m_PlateData[plateIndex].defPos;

      m_TouchMarginPos = platePos - margin;

      // カーソルを非表示
      m_pCursorListener->SetVisibleCursor( false );
    }

    // プレートをセット
    SetPlate_Now( plateIndex, true );

    return plateIndex;
  }
  // リリース
  else if( touchPanel->IsTouchRelease() )
  {
    if( m_Seq != SEQ_IDLE )
    {
      // 選択アニメの停止
      StartPlateUnselectAnim( m_PlateData[m_DestIndex].defIdx );

      m_PlateData[m_MoveIndex].excFlg  = true;
      m_PlateData[m_MoveIndex].moveSet = false;
      m_PlateData[m_MoveIndex].dragFlg = false;

      // 入力を無効に
      SetParentInputEnable( false );

      // ドラッグの終了
      m_isRelease = true;
      m_isDrag    = false;

      m_Seq = SEQ_EXCHANGE;
    }

    m_TouchPlate = PLATE_MAX;
  }

  return PLATE_MAX;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチしているプレートのインデックスの取得
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  タッチしているプレートのインデックス
 */
//------------------------------------------------------------------------------
u32 WazaExchange::GetTouchPlateIndex( gfl2::ui::TouchPanel* touchPanel )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

  for( u32 i=0; i<PLATE_MAX; ++i )
  {
    // 非表示なら判定しない
    if( !m_pBase->IsVisiblePane( m_PlateData[i].parts ) ) continue;
    // ボタンの入力が無効になっていたら判定しない
    if( !btnMan->IsButtonInputEnable( BTN_ID_WAZA_00 + i ) ) continue;
    // ドラッグ中のプレートのインデックスは判定しない
    if( m_isDrag && m_MoveIndex == i ) continue;
    // 入れ替えプレートも判定しない
    if( m_PlateData[i].excFlg ) continue;

    // プレートをタッチしているか判定
    if( m_pBase->IsHoldPane( touchPanel, m_PlateData[i].bound ) )
    {
      return i;
    }
  }

  // 見つからない
  return PLATE_MAX;
}

//------------------------------------------------------------------------------
/**
 * @brief   ドラッグによるプレートの座標更新
 */
//------------------------------------------------------------------------------
void WazaExchange::UpdateDragPlate( void )
{
  if( m_MoveIndex == PLATE_MAX ) return;

  // ドラッグ中
  if( m_isDrag )
  {
    gfl2::math::VEC3 next = m_TouchPos + m_TouchMarginPos;
    {
      next.x = m_PlateData[m_MoveIndex].defPos.x;   //!< xは固定
    }
    
    m_pBase->SetPanePos( m_PlateData[m_MoveIndex].parts, next );
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   入替開始：ドラッグモード
 *
 * @param   plateID   移動元のプレートID
 */
//------------------------------------------------------------------------------
void WazaExchange::StartExchange_Drag( u32 plateID )
{
  m_TriggerPlateID = plateID;

  // 移動元プレートをセット
  SetPlate_Now( plateID, true );

  // カーソルを非表示
  m_pCursorListener->SetVisibleCursor( false );

  // 技入替モードへ
  m_uiListener->SetDispMode( DISP_MODE_WAZA_EXCHANGE );
}

//------------------------------------------------------------------------------
/**
 * @brief   入替終了：ドラッグモード
 */
//------------------------------------------------------------------------------
void WazaExchange::EndExchange_Drag( void )
{
  // 選択アニメの停止
  StartPlateUnselectAnim( m_PlateData[m_DestIndex].defIdx );

  m_PlateData[m_MoveIndex].excFlg  = true;
  m_PlateData[m_MoveIndex].moveSet = false;
  m_PlateData[m_MoveIndex].dragFlg = false;

  // 入力を無効に
  SetParentInputEnable( false );

  // ドラッグの終了
  m_isRelease = true;
  m_isDrag    = false;

  m_TriggerPlateID = PLATE_MAX;

  m_Seq = SEQ_EXCHANGE;
}



//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  プレートインデックス
 */
//------------------------------------------------------------------------------
u32 WazaExchange::TouchEvent( gfl2::ui::TouchPanel* touchPanel )
{
  switch( m_uiListener->GetDispMode() )
  {
  case DISP_MODE_STATUS:
  case DISP_MODE_WAZA:
    if( m_Seq != SEQ_IDLE ) break;
    return TouchEvent_MoveSelect( touchPanel );
    // no break;

  case DISP_MODE_WAZA_EXCHANGE:
    if( m_Seq != SEQ_DEST ) break;
    return TouchEvent_DestSelect( touchPanel );
    // no break;
  }

  return PLATE_MAX;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：移動元選択
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  プレートインデックス
 */
//------------------------------------------------------------------------------
u32 WazaExchange::TouchEvent_MoveSelect( gfl2::ui::TouchPanel* touchPanel )
{
  u32 plateID = PLATE_MAX;

  if( PlateTouchEvent_Trigger( touchPanel, &plateID ) )
  {
    if( plateID != PLATE_MAX )
    {
      // タッチ座標を更新
      m_TouchPos = GetTouchPos_Layout( touchPanel );
      // プレートの中心とタッチ座標との差分
      m_TouchMarginPos = m_PlateData[plateID].defPos - m_TouchPos;

      // SE再生：キャッチ音
      Sound::PlaySE( SEQ_SE_CATCH );

      // 入替開始：ドラッグ
      StartExchange_Drag( plateID );

      return plateID;
    }
  }

  return PLATE_MAX;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：移動先選択
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  プレートインデックス
 */
//------------------------------------------------------------------------------
u32 WazaExchange::TouchEvent_DestSelect( gfl2::ui::TouchPanel* touchPanel )
{
  u32 plateID = PLATE_MAX;

  if( IsPlateDrag() )
  {
    if( BasicTouchEvent_Release( touchPanel ) )
    {
      // 入替の終了
      EndExchange_Drag();

      // SE再生：リリース音
      Sound::PlaySE( SEQ_SE_RELEASE );
    }
    else {
      // タッチ座標を更新
      m_TouchPos = GetTouchPos_Layout( touchPanel );

      if( PlateTouchEvent_Hold( touchPanel, &plateID ) )
      {
        // 移動元プレートをセット
        SetPlate_Now( plateID, true );

        return plateID;
      }
    }
  }
  else {
    if( m_TriggerPlateID == PLATE_MAX )
    {
      if( PlateTouchEvent_Trigger( touchPanel, &plateID ) )
      {
        m_TriggerPlateID = plateID;
        m_HoldPlateID = plateID;
      }
    }
    else if( BasicTouchEvent_Release( touchPanel ) )
    {
      if( m_HoldPlateID != PLATE_MAX )
      {
        SetPlate_Now( m_HoldPlateID );
      }

      m_TriggerPlateID = PLATE_MAX;
      m_HoldPlateID = PLATE_MAX;
    }
    else {
      PlateTouchEvent_Hold( touchPanel, &plateID );

      if( m_TriggerPlateID == plateID )
      {
        m_HoldPlateID = plateID;
      }
      else {
        m_HoldPlateID = PLATE_MAX;
      }
    }
  }

  return PLATE_MAX;
}


//------------------------------------------------------------------------------
/**
 * @brief   基本タッチイベント：トリガー
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == トリガーあり"
 * @return  "false == トリガーなし"
 */
//------------------------------------------------------------------------------
bool WazaExchange::BasicTouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel )
{
  if( touchPanel->IsTouchTrigger() )
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   基本タッチイベント：ホールド
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == ホールドあり"
 * @return  "false == ホールドなし"
 */
//------------------------------------------------------------------------------
bool WazaExchange::BasicTouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel )
{
  if( touchPanel->IsTouch() )
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   基本タッチイベント：リリース
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == リリースあり"
 * @return  "false == リリースなし"
 */
//------------------------------------------------------------------------------
bool WazaExchange::BasicTouchEvent_Release( gfl2::ui::TouchPanel* touchPanel )
{
  if( touchPanel->IsTouchRelease() )
  {
    return true;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   プレートタッチイベント：トリガー
 *
 * @param   touchPanel    タッチパネル
 * @param   destID        プレートID格納先
 *
 * @return  "true  == トリガーあり"
 * @return  "false == トリガーなし"
 */
//------------------------------------------------------------------------------
bool WazaExchange::PlateTouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel, u32* destID )
{
  if( BasicTouchEvent_Trigger( touchPanel ) )
  {
    u32 plateID = GetTouchPlate( touchPanel );

    if( plateID != PLATE_MAX )
    {
      *destID = plateID;

      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   プレートタッチイベント：ホールド
 *
 * @param   touchPanel    タッチパネル
 * @param   destID        プレートID格納先
 *
 * @return  "true  == トリガーあり"
 * @return  "false == トリガーなし"
 */
//------------------------------------------------------------------------------
bool WazaExchange::PlateTouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel, u32* destID )
{
  if( BasicTouchEvent_Hold( touchPanel ) )
  {
    u32 plateID = GetTouchPlate( touchPanel, m_TriggerPlateID );

    if( plateID != PLATE_MAX )
    {
      *destID = plateID;

      return true;
    }
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   タッチ座標の取得
 *
 * @return  タッチ座標
 */
//------------------------------------------------------------------------------
gfl2::math::VEC3 WazaExchange::GetTouchPos( gfl2::ui::TouchPanel* touchPanel )
{
  gfl2::math::VEC3 vec;
  {
    touchPanel->GetXY( &vec );
  }

  return vec;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチ座標の取得：レイアウト座標に変換
 *
 * @return  レイアウト座標
 */
//------------------------------------------------------------------------------
gfl2::math::VEC3 WazaExchange::GetTouchPos_Layout( gfl2::ui::TouchPanel* touchPanel )
{
  // タッチ座標の取得
  gfl2::math::VEC3 vec = GetTouchPos( touchPanel );
  {
    vec.x = vec.x + 160.0f;
    vec.y = 120.0f - vec.y;
  }

  return vec;
}


//------------------------------------------------------------------------------
/**
 * @brief   タッチされたプレートの取得
 *
 * @param   touchPanel    タッチパネル
 * @param   except        除外プレートID
 *
 * @return  プレートID
 */
//------------------------------------------------------------------------------
u32 WazaExchange::GetTouchPlate( gfl2::ui::TouchPanel* touchPanel, u32 except )
{
  //app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
  //{
  //  gfl2::math::VEC3 pos = GetTouchPos( touchPanel );

  //  u32 start = LowerMainDraw::BTN_ID_WAZA_00;
  //  u32 end   = LowerMainDraw::BTN_ID_WAZA_03;

  //  u32 exceptID = except + start;

  //  for( u32 i=start; i<=end; ++i )
  //  {
  //    u32 plateID = (i - start);

  //    if( exceptID == i )                 continue; //!< 除外プレートは判定しない
  //    if( !btnMan->IsButtonVisible( i ) ) continue; //!< 非表示プレートは判定しない
  //    if( m_PlateData[plateID].excFlg )   continue; //!< 入れ替えプレートは判定しない

  //    if( btnMan->IsButtonHit( i, static_cast<u16>(pos.x), static_cast<u16>(pos.y) ) )
  //    {
  //      return plateID;
  //    }
  //  }
  //}

  //return PLATE_MAX;
  return GetTouchPlateIndex( touchPanel );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
