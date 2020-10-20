//==============================================================================
/**
 * @file    PokeListPokeExchange.cpp
 * @author  fukushima_yuya
 * @date    2015.03.27
 * @brief   ポケモンの並び替え
 */
//==============================================================================

// niji
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/parts/PokeListPokeExchange.h"
#include "App/PokeList/source/parts/PokeListWindow.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Ui/UIView.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
const u32 c_START_EXCHANGE_FRAME = 10;


//------------------------------------------------------------------------------
// @brief   変数
//------------------------------------------------------------------------------
//! @brief  プレートペインID
const u32 c_PlatePaneIndex[PLATE_NUM] = {
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_00,  //!< 左上
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_03,  //!< 右上
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_01,  //!< 左中
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_04,  //!< 右中
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_02,  //!< 左下
  PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_05,  //!< 右下
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pBase         DrawerBaseクラスポインタ
 * @param   ppPokePlate   PlateDrawクラスポインタ配列
 * @param   pCursor       CursorDrawクラスポインタ
 * @param   pMsgWindow    MessageWindowクラスポインタ
 * @param   memberCount   メンバー数
 */
//------------------------------------------------------------------------------
PokeExchange::PokeExchange(
  DrawerBase* pBase, PlateDraw** ppPokePlate, CursorDraw* pCursor,
  MessageWindow* pMsgWindow, u8 memberCount )
  : m_pBase( pBase )
  , m_ppPokePlate( ppPokePlate )
  , m_pCursor( pCursor )
  , m_pMsgWindow( pMsgWindow )
  , m_MemberCount( memberCount )
  , m_DragMode( false )
  , m_OneTime( false )
  , m_Seq( SEQ_POKE_NONE )
  , m_MovePos( CPOS_NULL )
  , m_DestPos( CPOS_NULL )
  , m_pMovePlate( NULL )
  , m_pDestPlate( NULL )
  , m_pNullVisible( NULL )
  , m_SelectPlate( CPOS_NULL )
  , m_DecidePlate( CPOS_NULL )
  , m_HoldFrame( 0 )
{
  m_TriggerBtnID  = BTN_ID_NUM;
  m_DestBtnID     = BTN_ID_NUM;

  Init();
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeExchange::~PokeExchange( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void PokeExchange::Init( void )
{
  // 機能ボタンの表示切替用ペイン
  m_pNullVisible = m_pBase->GetPane( PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_00 );
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void PokeExchange::Update( void )
{
  switch( m_Seq )
  {
  case SEQ_POKE_EXCHANGE:
    {
      m_Seq = SEQ_POKE_EXCHANGE_END;

      if( m_MovePos == CPOS_NULL ) break;
      if( m_DestPos == CPOS_NULL ) break;

      if( m_MovePos != m_DestPos )
      {
        FUKUSHIMA_PRINT( "Start Exchange\n" );
        FUKUSHIMA_PRINT( "Move[%d] <=> Dest[%d]\n", m_MovePos, m_DestPos );

        Exchange();   //!< 入れ替え処理

        m_Seq = SEQ_POKE_EXCHANGING;
      }
    } break;

  case SEQ_POKE_EXCHANGING:   //!< 入れ替え
    {
      if( !m_DragMode )
      {
        if( m_pMovePlate->IsPlateCatch() ) break;
        if( m_pDestPlate->IsPlateCatch() ) break;
      }

      m_pDestPlate->StartPokeIconSelectAnim( true );

      m_Seq = SEQ_POKE_EXCHANGE_END;
    } break;

  case SEQ_POKE_EXCHANGE_END:
    {
      FUKUSHIMA_PRINT( "Finish Exchange\n" );

      // カーソル
      {
        CursorPos cpos = (m_DestPos != CPOS_NULL) ? m_DestPos : m_MovePos;
        CursorID cid = (m_OneTime) ? CID_POKE : CID_MOVE;
        m_pCursor->SetCursorID( cid, cpos );
        m_pCursor->SetVisible( true );
      }

      if( !m_OneTime )
      {
        StartExchange();
      }
      else {
        EndExchange();
        m_pBase->GetLocalWork()->SetDispMode( DISP_MODE_NORMAL );
      }

      SetPokeListEnable( true );
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの並び替えの開始
 */
//------------------------------------------------------------------------------
void PokeExchange::StartExchange( bool oneTime )
{
  Reset();

  // 機能ボタンを非表示
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
  {
    m_pBase->SetVisiblePane( m_pNullVisible, false );

    btnMan->SetButtonInputEnable( BTN_ID_X, false );
    btnMan->SetButtonInputEnable( BTN_ID_Y, false );

    // ボタンの入力モードをキーのみにする
    btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );
  }

  // メッセージウィンドウを開く
  if( m_pMsgWindow )
  {
    m_pMsgWindow->SetMessage( mes_pokelist_02_02 );
    m_pMsgWindow->Open();
  }

  // カーソル
  CursorPos cpos = m_pCursor->GetCursorPos();
  m_pCursor->SetCursorID( CID_MOVE, cpos );

  m_OneTime = oneTime;

  m_Seq = SEQ_POKE_MOVE_SELECT;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの並び替えの終了
 */
//------------------------------------------------------------------------------
void PokeExchange::EndExchange( void )
{
  Reset();

  // メッセージウィンドウを閉じる
  if( m_pMsgWindow ) m_pMsgWindow->Close();

  // 機能ボタンの表示
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
  {
    m_pBase->SetVisiblePane( m_pNullVisible, true );

    btnMan->SetButtonInputEnable( BTN_ID_X, true );
    btnMan->SetButtonInputEnable( BTN_ID_Y, true );

    // ボタンの入力モードをデフォルトに戻す
    btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_BOTH );
  }

  if( m_Seq == SEQ_POKE_MOVE_SELECT )
  {
    CursorPos cpos = m_pCursor->GetCursorPos( CID_MOVE );
    m_pCursor->SetCursorID( CID_POKE, cpos );
  }
  else if( m_Seq == SEQ_POKE_DEST_SELECT )
  {
    CursorPos cpos = m_pCursor->GetCursorPos( CID_DEST );
    m_pCursor->SetCursorID( CID_POKE, cpos );
  }

  m_Seq = SEQ_POKE_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタン選択処理
 * @param   cpos    カーソル位置
 */
//------------------------------------------------------------------------------
void PokeExchange::Decide( CursorPos cpos )
{
  if( m_Seq == SEQ_POKE_MOVE_SELECT )
  {
    SetMovePos( cpos );
  }
  else if( m_Seq == SEQ_POKE_DEST_SELECT )
  {
    if( m_MovePos != cpos )
    {
      SetDestPos( cpos );
    }
    else {
      Cancel();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセル処理
 */
//------------------------------------------------------------------------------
bool PokeExchange::Cancel( void )
{
  if( m_Seq == SEQ_POKE_MOVE_SELECT )
  {
    EndExchange();

    return true;    //!< ポケモン入れ替えの終了を通知
  }
  else if( m_Seq == SEQ_POKE_DEST_SELECT )
  {
    StartExchange();
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   移動元のセット
 * @param   cpos    移動元
 */
//------------------------------------------------------------------------------
void PokeExchange::SetMovePos( CursorPos cpos, bool isDrag )
{
  GFL_ASSERT( m_Seq == SEQ_POKE_MOVE_SELECT );

  // プレートが選択できるかどうか
  if( !IsSelectPlate( cpos ) )
  {
    return;   //!< 選択できないプレートだった
  }

  m_MovePos  = cpos;
  m_DragMode = isDrag;

  // 移動元プレートの設定
  m_pMovePlate = m_ppPokePlate[m_MovePos];
  {
    // 移動元プレートを選択状態に
    m_pMovePlate->SetPlateState( PlateDraw::PLATE_STATE_SELECTED );
  }

  // カーソル
  {
    //CursorPos nowPos = m_pCursor->GetCursorPos();
    //m_pCursor->SetCursorID( CID_DEST, nowPos );
    m_pCursor->SetCursorPos( m_MovePos );
    m_pCursor->SetCursorID( CID_DEST, m_MovePos );   // @fix NMCat[275]
  }

  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

  // 
  if( m_DragMode )
  {
    AppendChild( m_MovePos );

    m_pMovePlate->SetPlateCatch();        //!< プレートをキャッチ状態に
    m_pMovePlate->SetPlateDrag( true );   //!< ドラッグ状態に

    m_pCursor->SetVisible( false );       //!< カーソルを非表示に

    btnMan->ResetHoldState();
  }

  // ボタンの入力モードをデフォルトに戻す
  btnMan->SetInputMode( app::tool::ButtonManager::INPUT_MODE_BOTH );

  if( m_pMsgWindow ) m_pMsgWindow->SetMessage( mes_pokelist_02_04 );

  m_Seq = SEQ_POKE_DEST_SELECT;   //!< 移動先の選択へ

  FUKUSHIMA_PRINT( "%s( cpos[%d], isDrag )\n", __FUNCTION__, m_MovePos, isDrag );
}

//------------------------------------------------------------------------------
/**
 * @brief   移動先のセット
 * @param   cpos    移動先
 */
//------------------------------------------------------------------------------
void PokeExchange::SetDestPos( CursorPos cpos )
{
  GFL_ASSERT( m_Seq == SEQ_POKE_DEST_SELECT );

  // プレートが選択できるかどうか
  // キー操作とドラッグ操作では移動先が選択できないなどの条件の場合、後のシーケンスが違うことがある。
  if( !IsSelectPlate( cpos ) )
  {
    m_pMovePlate->SetPlateDrag( false );    //!< ドラッグ状態を解除
    m_Seq = SEQ_POKE_EXCHANGE;
    return;
  }

  m_DestPos = cpos;
  m_pCursor->SetCursorPos( m_DestPos );

  if( m_DestPos != CPOS_NULL )
  {
    m_pDestPlate = m_ppPokePlate[m_DestPos];
    m_pDestPlate->StartPokeIconSelectAnim( false );

    if( m_DragMode )
    {
      AppendChild( m_DestPos );

      m_pDestPlate->SetPlateCatch();        //!< プレートをキャッチ状態に
      m_pDestPlate->SetPlateDrag( true );   //!< ドラッグ状態に
    }
  }

  m_Seq = SEQ_POKE_EXCHANGE;

  m_pCursor->SetVisible( false );
  SetPokeListEnable( false );   //!< ポケモンリストの入力を無効に

  FUKUSHIMA_PRINT( "%s( cpos[%d] )\n", __FUNCTION__, m_DestPos );
}



//------------------------------------------------------------------------------
/**
 * @brief   データの初期化
 */
//------------------------------------------------------------------------------
void PokeExchange::Reset( void )
{
  m_MovePos = CPOS_NULL;
  m_DestPos = CPOS_NULL;

  if( m_pMovePlate ) m_pMovePlate->SetPlateDrag( false );
  m_pMovePlate = NULL;
  m_pDestPlate = NULL;

  for( u8 i=0; i<m_MemberCount; ++i )
  {
    m_ppPokePlate[i]->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT );
  }

  m_DragMode = false;
  m_OneTime = false;
}

//------------------------------------------------------------------------------
/**
 * @brief   プレートが選択できるかどうか
 * @return  true = 選択可、false = 選択不可
 */
//------------------------------------------------------------------------------
bool PokeExchange::IsSelectPlate( CursorPos cpos )
{
  // 選択された箇所がプレートではない
  if( cpos > CPOS_PLATE_BR ) return false;

  // 選択された箇所がメンバー数以上の位置
  if( cpos >= m_MemberCount ) return false;

  // プレートの状態がDISABLE
  if( m_ppPokePlate[cpos]->GetPlateState() == PlateDraw::PLATE_STATE_DISABLE ) return false;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン入れ替え
 */
//------------------------------------------------------------------------------
void PokeExchange::Exchange( void )
{
  StaticWork* pStaticWork = m_pBase->GetStaticWork();

  // 入れ替え
  pStaticWork->ExchangePlateOrder( m_MovePos, m_DestPos );

  // プレートの表示を更新
  {
    FUKUSHIMA_PRINT( "MovePlate\n" );
    const pml::pokepara::PokemonParam* pMovePoke = pStaticWork->GetPokeConst( m_MovePos );
    m_pMovePlate->SetupPlatePokeDisplay( pMovePoke );
    m_pMovePlate->SetModePlate( PLATE_MODE_PARAM );
    m_pMovePlate->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT );

    FUKUSHIMA_PRINT( "DestPlate\n" );
    const pml::pokepara::PokemonParam* pDestPoke = pStaticWork->GetPokeConst( m_DestPos );
    m_pDestPlate->SetupPlatePokeDisplay( pDestPoke );
    m_pDestPlate->SetModePlate( PLATE_MODE_PARAM );
    m_pDestPlate->SetPlateState( PlateDraw::PLATE_STATE_DEFAULT );
  }

  // プレートの位置を変更
  {
    PlatePosition* pMovePlatePos = m_pMovePlate->GetPlatePosition();
    PlatePosition* pDestPlatePos = m_pDestPlate->GetPlatePosition();

    if( !m_DragMode )
    {
      gfl2::math::Vector3 movePos = pMovePlatePos->GetInitPos();
      gfl2::math::Vector3 destPos = pDestPlatePos->GetInitPos();

      pMovePlatePos->SetNowPos( destPos );
      pDestPlatePos->SetNowPos( movePos );
    }
    else {
      gfl2::math::Vector3 movePos = pMovePlatePos->GetNowPos();
      gfl2::math::Vector3 destPos = pDestPlatePos->GetInitPos();

      pMovePlatePos->SetNowPos( destPos );
      pDestPlatePos->SetNowPos( movePos );
    }
  }

  if( !m_DragMode )
  {
    m_pMovePlate->SetPlateCatch();  //!< プレートをキャッチ状態に
    m_pDestPlate->SetPlateCatch();  //!< プレートをキャッチ状態に

    AppendChild( m_MovePos );
    AppendChild( m_DestPos );
  }
  else {
    m_pMovePlate->SetPlateDrag( false );    //!< ドラッグ状態を解除
    m_pDestPlate->SetPlateDrag( false );    //!< ドラッグ状態を解除
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの入力の有効設定の変更
 * @param   enable    有効フラグ(true = 有効、false = 無効)
 */
//------------------------------------------------------------------------------
void PokeExchange::SetPokeListEnable( bool enable )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    view->SetInputEnabled( enable );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   移動元プレート位置クラスの取得
 * @return  PlatePositionクラスポインタ
 */
//------------------------------------------------------------------------------
PlatePosition* PokeExchange::GetMovePlatePos( void )
{
  if( !m_pMovePlate ) return NULL;

  return m_pMovePlate->GetPlatePosition();
}



//------------------------------------------------------------------------------
/**
 * @brief   プレートの描画プライオリティの変更
 */
//------------------------------------------------------------------------------
void PokeExchange::AppendChild( CursorPos cpos )
{
  app::util::G2DUtil* pG2D = m_pBase->GetG2D();
  u32 lytID = m_pBase->GetLytID();

  gfl2::lyt::LytPaneIndex parent_index = PANENAME_POKELIST_MIN_LOW_000_PANE_ALL;
  gfl2::lyt::LytPaneIndex child_index = c_PlatePaneIndex[cpos];
  gfl2::lyt::LytPaneIndex offset_index = PANENAME_POKELIST_MIN_LOW_000_PANE_NULL_01;
  u32 mode = app::util::G2DUtil::APPEND_CHILD_MODE_INSERT;

  pG2D->AppendChild( lytID, parent_index, child_index, mode, offset_index );
}





//------------------------------------------------------------------------------
/**
 * @brief   タッチ処理
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == 入れ替えイベント開始"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool PokeExchange::TouchEvent( gfl2::ui::TouchPanel* touchPanel )
{
  LocalWork* localWork = m_pBase->GetLocalWork();

  switch( localWork->GetDispMode() )
  {
  case DISP_MODE_NORMAL:
    return TouchEvent_ListMode( touchPanel );
    // no break;

  case DISP_MODE_POKE_EXCHANGE:
    return TouchEvent_Exchange( touchPanel );
    // no break;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：リスト通常時
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool PokeExchange::TouchEvent_ListMode( gfl2::ui::TouchPanel* touchPanel )
{
  u32 btnID = BTN_ID_NUM;

  // タッチイベント：トリガー
  if( TouchEvent_Trigger( touchPanel, &btnID ) )
  {
    m_TriggerBtnID = btnID;
    m_HoldFrame    = 0;

    // タッチイベントあり
    return true;
  }
  // タッチイベント：トリガー
  else if( TouchEvent_Hold( touchPanel, &btnID ) )
  {
    // ホールドしているボタンとトリガーのボタンが同じ場合はフレームをカウント
    if( btnID == m_TriggerBtnID )
    {
      // 一定時間未満の場合はフレームをカウント
      if( m_HoldFrame < c_START_EXCHANGE_FRAME )
      {
        m_HoldFrame++;
      }
      // 一定時間経過したら入替開始
      else {
        // 入れ替えの開始
        StartExchange( true );
        // 移動元の設定
        SetMovePos( static_cast<CursorPos>(m_TriggerBtnID), true );

        PlatePosition* platePosition = GetMovePlatePos();
        {
          // タッチ座標の取得
          gfl2::math::Vector3 pos = GetTouchPos( touchPanel );
          // タッチ位置とプレートの中心の差分をセット
          platePosition->SetPlateSubVec( pos );
        }

        // 画面モードをポケモン入替に変更
        m_pBase->GetLocalWork()->SetDispMode( DISP_MODE_POKE_EXCHANGE );

        // SE再生：キャッチ音
        Sound::PlaySE( SEQ_SE_CATCH );

        m_HoldFrame    = 0;
      }
    }

    return true;
  }
  // タッチイベント：リリース
  else if( TouchEvent_Release( touchPanel ) )
  {
    m_TriggerBtnID = BTN_ID_NUM;
    m_HoldFrame    = 0;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：リスト入替時
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool PokeExchange::TouchEvent_Exchange( gfl2::ui::TouchPanel* touchPanel )
{
  u32 btnID = BTN_ID_NUM;

  // 移動元選択
  if( GetSequence() == SEQ_POKE_MOVE_SELECT )
  {
    // タッチイベント：トリガー
    if( TouchEvent_Trigger( touchPanel, &btnID ) )
    {
      m_TriggerBtnID = btnID;

      // 移動元の設定
      SetMovePos( static_cast<CursorPos>(m_TriggerBtnID), true );

      PlatePosition* platePosition = GetMovePlatePos();
      {
        // タッチ座標の取得
        gfl2::math::Vector3 pos = GetTouchPos( touchPanel );
        // タッチ位置とプレートの中心の差分をセット
        platePosition->SetPlateSubVec( pos );
      }

      // SE再生：キャッチ音
      Sound::PlaySE( SEQ_SE_CATCH );

      m_DestBtnID = BTN_ID_NUM;

      return true;
    }
  }
  // 移動先選択
  else if( GetSequence() == SEQ_POKE_DEST_SELECT )
  {
    if( !IsDragMode() )
    {
      // ドラッグモードで無ければタッチ処理は必要ない
      return false;
    }

    // タッチイベント：リリース
    if( TouchEvent_Release( touchPanel ) )
    {
      if( m_DestBtnID != BTN_ID_NUM )
      {
        // 移動先が選択されている
        Decide( static_cast<CursorPos>(m_DestBtnID) );
      }
      else {
        Decide( CPOS_NULL );
      }

      // SE再生：リリース音
      Sound::PlaySE( SEQ_SE_RELEASE );

      m_TriggerBtnID = BTN_ID_NUM;
      m_DestBtnID    = BTN_ID_NUM;
    }
    else {
      // タッチイベント：ホールド
      TouchEvent_Hold( touchPanel, &btnID, m_TriggerBtnID );
      // 移動先のボタンIDを設定
      m_DestBtnID = btnID;

      PlatePosition* platePosition = GetMovePlatePos();
      {
        // タッチ座標の取得
        gfl2::math::Vector3 pos = GetTouchPos( touchPanel );
        // ドラッグ位置の更新
        platePosition->UpdatePlateDrag( pos );
      }
    }

    return true;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：トリガー
 *
 * @param   touchPanel    タッチパネル
 * @param   destID        選択ボタンID格納先
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool PokeExchange::TouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel, u32* destID )
{
  if( touchPanel->IsTouchTrigger() )
  {
    // タッチしているボタンIDを取得
    u32 btnID = GetTouchBtnID( touchPanel );

    if( btnID == BTN_ID_NUM )
    {
      // タッチイベントなし
      return false;
    }

    // タッチしたボタンのIDを渡す
    if( destID )
    {
      *destID = btnID;
    }

    return true;
  }

  // タッチイベントなし
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：ホールド
 *
 * @param   touchPanel    タッチパネル
 * @param   destID        選択ボタンID格納先
 * @param   except        除外ボタンID
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool PokeExchange::TouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel, u32* destID, u32 except )
{
  if( touchPanel->IsTouch() )
  {
    // タッチしているボタンIDを取得
    u32 btnID = GetTouchBtnID( touchPanel, except );

    if( btnID == BTN_ID_NUM )
    {
      // タッチイベントなし
      return false;
    }

    // タッチしたボタンのIDを渡す
    if( destID )
    {
      *destID = btnID;
    }

    return true;
  }

  // タッチイベントなし
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント：リリース
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  "true  == タッチイベントあり"
 * @return  "false == タッチイベントなし"
 */
//------------------------------------------------------------------------------
bool PokeExchange::TouchEvent_Release( gfl2::ui::TouchPanel* touchPanel )
{
  if( touchPanel->IsTouchRelease() )
  {
    return true;
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   タッチしているボタンIDの取得
 *
 * @param   touchPanel    タッチパネル
 * @param   except        除外ボタンID
 *
 * @return  ボタンID
 */
//------------------------------------------------------------------------------
u32 PokeExchange::GetTouchBtnID( gfl2::ui::TouchPanel* touchPanel, u32 except )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
  {
    // タッチ座標
    u16 tx = touchPanel->GetX();
    u16 ty = touchPanel->GetY();

    // 検索ボタンIDの開始と終端
    u32 start = BTN_ID_PLATE_TL;
    u32 end   = BTN_ID_PLATE_BR;

    for( u32 i=start; i<=end; ++i )
    {
      // 除外ボタンIDならタッチ判定しない
      if( except == i ) continue;
      // ボタンが無効になっている場合は判定しない
      if( !btnMan->IsButtonVisible(i) )     continue;
      if( !btnMan->IsButtonInputEnable(i) ) continue;

      // ボタンのタッチ判定
      if( btnMan->IsButtonHit( i, tx, ty ) )
      {
        // ボタンあり
        return i;
      }
    }
  }

  // ボタンなし
  return BTN_ID_NUM;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチ座標の取得
 *
 * @param   touchPanel    タッチパネル
 *
 * @return  タッチ座標
 */
//------------------------------------------------------------------------------
gfl2::math::Vector3 PokeExchange::GetTouchPos( gfl2::ui::TouchPanel* touchPanel )
{
  gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
  {
    pos.x = static_cast<f32>( touchPanel->GetX() );
    pos.y = static_cast<f32>( touchPanel->GetY() );
  }

  return pos;
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
