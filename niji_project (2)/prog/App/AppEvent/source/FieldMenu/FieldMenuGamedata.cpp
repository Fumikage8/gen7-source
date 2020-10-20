//=============================================================================
/**
 * @file    FieldMenuGamedata.h
 * @brief   フィールド下画面 ゲーム中に保持し続けるデータ
 * @author  Hiroyuki Nakamura
 * @date    2015.06.23
 */
//=============================================================================

// module
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
// momiji
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
PlayData::PlayData(void)
  : m_cursorPos( 0 )
  , m_rideCursorPos( 0 )
  , m_selectRideID( 0 )
  , m_endContinueRotomMsg( false )
  , m_isInput( true )
  , m_isRotomRequestEvolution( false )
#if PM_DEBUG
  , m_debugIsRotomAnimeLoop( 0 )
  , m_debugRotomReq( Savedata::FieldMenu::ROTOM_REQ_NONE )
  , m_debugIsRotomReport( false )
  , m_debugRotomFreeTalkID( 0 )
  , m_debugRotomGameClearMsgID( 0 )
#endif  // PM_DEBUG
{
  InitRotomMessage();
  gfl2::std::MemClear( &m_rideParam, sizeof(App::FieldMenu::RIDE_PARAM) );
  gfl2::std::MemClear( &m_rotomFriendshipData, sizeof(ROTOM_FRIENDSHIP_DATA) );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
PlayData::~PlayData()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCursorPos
 * @brief   カーソル位置を取得
 * @date    2015.06.23
 *
 * @return  カーソル位置
 */
//-----------------------------------------------------------------------------
u32 PlayData::GetCursorPos(void)
{
  return m_cursorPos;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCursorPos
 * @brief   カーソル位置をセット
 * @date    2015.06.23
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void PlayData::SetCursorPos( u32 pos )
{
  m_cursorPos  = pos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRideCursorPos
 * @brief   ライド選択画面のカーソル位置を取得
 * @date    2015.08.06
 *
 * @return  カーソル位置
 */
//-----------------------------------------------------------------------------
u32 PlayData::GetRideCursorPos(void)
{
  return m_rideCursorPos;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRideCursorPos
 * @brief   ライド選択画面のカーソル位置をセット
 * @date    2015.08.06
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void PlayData::SetRideCursorPos( u32 pos )
{
  m_rideCursorPos  = pos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetSelectRideID
 * @brief   ライド選択画面で選択されたIDを取得（説明デモ用）
 * @date    2016.04.13
 *
 * @return  ライドID
 */
//-----------------------------------------------------------------------------
u32 PlayData::GetSelectRideID(void)
{
  return m_selectRideID;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelectRideID
 * @brief   ライド選択画面で選択されたIDをセット（説明デモ用）
 * @date    2016.04.13
 *
 * @param   id  ライドID
 */
//-----------------------------------------------------------------------------
void PlayData::SetSelectRideID( u32 id )
{
  m_selectRideID = id;
}


//-----------------------------------------------------------------------------
/**
 * @func    InitRotomMessage
 * @brief   ロトムメッセージデータクリア（個別）
 * @date    2015.11.10
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void PlayData::InitRotomMessage( u32 index )
{
  m_rotomReq[index].text = ROTOM_TEXT_NONE;
  m_rotomReq[index].work = ROTOM_WORK_NONE;
  m_rotomReq[index].is_update = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitRotomMessage
 * @brief   ロトムメッセージデータクリア（全体）
 * @date    2015.11.10
 */
//-----------------------------------------------------------------------------
void PlayData::InitRotomMessage(void)
{
  for( u32 i=0; i<ROTOM_MSG_MAX; i++ )
  {
    InitRotomMessage( i );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomMessage
 * @brief   ロトムメッセージセット
 * @date    2015.11.10
 *
 * @param   index       インデックス
 * @param   text        テキスト
 * @param   work        ワーク
 * @param   is_update   true = 表示中
 */
//-----------------------------------------------------------------------------
void PlayData::SetRotomMessage( PlayData::RotomMsgIndex index, u32 text, u32 work, bool is_update )
{
  ROTOM_MSG_DATA * data = &m_rotomReq[index];

  // 同じメッセージは再登録しない
  if( data->text == text )
  {
    return;
  }

  data->text = text;
  data->work = work;
  if( is_update == false )
  {
    data->is_update = 0;
  }
  else
  {
    data->is_update = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetEnableRottomMsgIndex
 * @brief   ロトムメッセージインデックスを取得
 * @date    2015.11.10
 *
 * @return  優先度の高いインデックス
 */
//-----------------------------------------------------------------------------
PlayData::RotomMsgIndex PlayData::GetEnableRottomMsgIndex(void)
{
  for( u32 i=0; i<ROTOM_MSG_MAX; i++ )
  {
    if( m_rotomReq[i].text != ROTOM_TEXT_NONE )
    {
      return static_cast<RotomMsgIndex>( i );
    }
  }
  return ROTOM_MSG_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageText
 * @brief   ロトムメッセージテキストを取得
 * @date    2015.11.10
 *
 * @param   index   インデックス
 *
 * @return  テキストID
 */
//-----------------------------------------------------------------------------
u32 PlayData::GetRotomMessageText( u32 index )
{
  return m_rotomReq[index].text;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageText
 * @brief   ロトムメッセージワークを取得
 * @date    2016.01.14
 *
 * @param   index   インデックス
 *
 * @return  ワーク
 */
//-----------------------------------------------------------------------------
u32 PlayData::GetRotomMessageWork( u32 index )
{
  return m_rotomReq[index].work;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageIsUpdate
 * @brief   ロトムメッセージの表示中更新フラグを取得
 * @date    2016.01.15
 *
 * @param   index   インデックス
 *
 * @retval  true  = 表示中に更新された
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PlayData::GetRotomMessageIsUpdate( u32 index )
{
  if( m_rotomReq[index].is_update == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomMessageIsUpdate
 * @brief   ロトムメッセージの表示中更新フラグを落とす
 * @date    2016.01.15
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void PlayData::ResetRotomMessageIsUpdate( u32 index )
{
  m_rotomReq[index].is_update = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEndContinueRotomMessage
 * @brief   ゲーム開始時のロトムメッセージを表示したか
 * @date    2016.01.14
 *
 * @retval  true  = 表示した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PlayData::IsEndContinueRotomMessage(void)
{
  return m_endContinueRotomMsg;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEndContinueRotomMessage
 * @brief   ゲーム開始時のロトムメッセージを表示済みにする
 * @date    2016.01.14
 */
//-----------------------------------------------------------------------------
void PlayData::SetEndContinueRotomMessage(void)
{
  m_endContinueRotomMsg = true;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetEndInputMode
 * @brief   下画面終了時の入力モードを記憶
 * @date    2015.11.19
 *
 * @param   flg   true = アクティブ, false = パッシブ
 */
//-----------------------------------------------------------------------------
void PlayData::SetEndInputMode( bool flg )
{
  m_isInput = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetEndInputMode
 * @brief   下画面終了時の入力モードを記憶
 * @date    2015.11.19
 *
 * @retval  true  = アクティブ
 * @retval  false = パッシブ
 */
//-----------------------------------------------------------------------------
bool PlayData::GetEndInputMode(void)
{
  return m_isInput;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRideParam
 * @brief   ライド選択画面のパラメータを設定
 * @date    2016.03.30
 *
 * @param   param   セットするパラメータ
 */
//-----------------------------------------------------------------------------
void PlayData::SetRideParam( App::FieldMenu::RIDE_PARAM * param )
{
  m_rideParam = *param;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRideParam
 * @brief   ライド選択画面のパラメータを取得
 * @date    2016.03.30
 *
 * @return  パラメータ
 */
//-----------------------------------------------------------------------------
const App::FieldMenu::RIDE_PARAM * PlayData::GetRideParam(void) const
{
  return &m_rideParam;
}

//-----------------------------------------------------------------------------
/**
 * @func    RotomTouch
 * @brief   ロトムにタッチ
 * @date    2016.12.01
 *
 * @param   pos   タッチした位置
 *
 * @retval  true  = 通常
 * @retval  false = 連続でタッチしすぎた
 */
//-----------------------------------------------------------------------------
bool PlayData::RotomTouch( u32 pos )
{
  pos++;  // １オリジンに変更

  if( m_rotomFriendshipData.touch_pos == pos )
  {
#if defined(GF_PLATFORM_CTR)
    nn::os::Tick now = nn::os::Tick::GetSystemCurrent();
    nn::fnd::TimeSpan ss = m_rotomFriendshipData.touch_start_tick.ToTimeSpan();
    nn::fnd::TimeSpan es = now.ToTimeSpan();
    u32 sec = es.GetSeconds() - ss.GetSeconds();
    if( sec >= ROTOM_TOUCH_CONTINUE_SEC )
    {
      // 最初から
      m_rotomFriendshipData.touch_count = 1;
      m_rotomFriendshipData.touch_start_tick = now;
      return true;
    }
#endif  // GF_PLATFORM_CTR
    m_rotomFriendshipData.touch_count++;
    if( m_rotomFriendshipData.touch_count >= ROTOM_TOUCH_CONTINUE_MAX )
    {
      m_rotomFriendshipData.touch_count = ROTOM_TOUCH_CONTINUE_MAX;
      return false;
    }
  }
  else
  {
    m_rotomFriendshipData.touch_pos = pos;
    m_rotomFriendshipData.touch_count = 1;
#if defined(GF_PLATFORM_CTR)
    m_rotomFriendshipData.touch_start_tick = nn::os::Tick::GetSystemCurrent();
#endif  // GF_PLATFORM_CTR
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitRotomTouch
 * @brief   ロトムのタッチデータを初期化
 * @date    2016.12.01
 */
//-----------------------------------------------------------------------------
void PlayData::InitRotomTouch(void)
{
  m_rotomFriendshipData.touch_pos   = 0;
  m_rotomFriendshipData.touch_count = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomTouchFriendshipUpFlag
 * @brief   ロトムをタッチした際の親密度アップ許可設定
 * @date    2016.12.01
 *
 * @param   flg   true = 許可, false = 禁止
 */
//-----------------------------------------------------------------------------
void PlayData::SetRotomTouchFriendshipUpFlag( bool flg )
{
  if( flg == false )
  {
    m_rotomFriendshipData.up_enable = 0;
  }
  else
  {
    m_rotomFriendshipData.up_enable = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomTouchFriendshipUp
 * @brief   ロトムをタッチした際に親密度を上げられるか
 * @date    2016.12.01
 *
 * @retval  true  = 上げられる
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PlayData::IsRotomTouchFriendshipUp(void)
{
  if( m_rotomFriendshipData.up_enable == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateTime
 * @brief   時間経過処理
 * @date    2016.12.12
 *
 * @param   diff_minute   経過時間（分）
 */
//-----------------------------------------------------------------------------
void PlayData::UpdateTime( u32 diff_minute )
{
  // 親密度アップフラグ
  if( IsRotomTouchFriendshipUp() == false )
  {
    u32 diff = m_rotomFriendshipData.up_diff_minute + diff_minute;
    if( diff >= FIRIENDSHIP_UP_DIFF_MINUTE )
    {
      SetRotomTouchFriendshipUpFlag( true );
      m_rotomFriendshipData.up_diff_minute = 0;
    }
    else
    {
      m_rotomFriendshipData.up_diff_minute = static_cast<u8>( diff );
    }
  }

  // 会話プレイ時間
  if( m_rotomFriendshipData.talk_diff_minute < TALK_PLAYTIME_DIFF_MINUTE )
  {
    u32 diff = m_rotomFriendshipData.talk_diff_minute + diff_minute;
    if( diff > TALK_PLAYTIME_DIFF_MINUTE )
    {
      if( IsRotomReportTalkEnable() != false )
      {
        m_rotomFriendshipData.talk_diff_minute = TALK_PLAYTIME_DIFF_MINUTE;
      }
      else
      {
        SetRotomReportTalkEnable( true );
        m_rotomFriendshipData.talk_diff_minute = 0;
      }
    }
    else
    {
      m_rotomFriendshipData.talk_diff_minute = static_cast<u8>( diff );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomTalkDiffTime
 * @brief   プレイ時間関連のロトム会話が発生するか
 * @date    2016.12.12
 *
 * @retval  true  = する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PlayData::IsRotomTalkDiffTime(void)
{
  if( m_rotomFriendshipData.talk_diff_minute >= TALK_PLAYTIME_DIFF_MINUTE )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetRotomTalkDiffTime
 * @brief   ロトム会話用時間カウンタをリセット
 * @date    2016.12.12
 */
//-----------------------------------------------------------------------------
void PlayData::ResetRotomTalkDiffTime(void)
{
  m_rotomFriendshipData.talk_diff_minute = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomReportTalkEnable
 * @brief   ロトムがレポート喚起するか
 * @date    2017.03.28
 *
 * @retval  true  = する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PlayData::IsRotomReportTalkEnable(void)
{
  if( m_rotomFriendshipData.is_report_talk == 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomReportTalkEnable
 * @brief   ロトムのレポート喚起フラグを設定
 * @date    2017.03.28
 *
 * @param   flg   true  = する, false = しない
 */
//-----------------------------------------------------------------------------
void PlayData::SetRotomReportTalkEnable( bool flg )
{
  if( flg == false )
  {
    m_rotomFriendshipData.is_report_talk = 1;
  }
  else
  {
    m_rotomFriendshipData.is_report_talk = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomRequestAchieved
 * @brief   ロトムのお題を達成
 * @date    2017.02.07
 *
 * @param   req   達成したお題
 *
 * @note  常駐領域なのでここに置く
 */
//-----------------------------------------------------------------------------
void PlayData::SetRotomRequestAchieved( Savedata::FieldMenu::RotomRequest req )
{
  Savedata::FieldMenu * sv_fieldmenu = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu();

  // @note チュートリアルは例外処理、ここでリクエスト＞即クリアにする
  if( req == Savedata::FieldMenu::ROTOM_REQ_TUTORIAL )
  {
    sv_fieldmenu->SetRotomRequest( Savedata::FieldMenu::ROTOM_REQ_TUTORIAL );
    sv_fieldmenu->SetRotomRequestClearFlag( 1 );
  }
  else
  {
    Savedata::FieldMenu::RotomRequest save_req = sv_fieldmenu->GetRotomRequest();
    // @note 進化の達成条件はファインダースタジオと同じ
    if( req == save_req ||
        ( req == Savedata::FieldMenu::ROTOM_REQ_FINDER_STUDIO && save_req == Savedata::FieldMenu::ROTOM_REQ_EVOLUTION ) )
    {
      sv_fieldmenu->SetRotomRequestClearFlag( 1 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomRequestEvolution
 * @brief   ロトポンの進化後のお願い発生フラグをセット
 * @date    2017.03.23
 *
 * @param   flg   発生フラグ
 */
//-----------------------------------------------------------------------------
void PlayData::SetRotomRequestEvolution( bool flg )
{
  m_isRotomRequestEvolution = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomRequestEvolution
 * @brief   ロトポンの進化後のお願いを発生させるか
 * @date    2017.03.23
 *
 * @retval  true  = 発生させる
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PlayData::IsRotomRequestEvolution(void)
{
  return m_isRotomRequestEvolution;
}



#if PM_DEBUG
void PlayData::DebugSetRotomAnimeLoop( u32 flg )
{
  m_debugIsRotomAnimeLoop = flg;
}
u32 PlayData::DebugIsRotomAnimeLoop(void)
{
  return m_debugIsRotomAnimeLoop;
}

void PlayData::DebugSetRotomRequest( Savedata::FieldMenu::RotomRequest req )
{
  if( req >= Savedata::FieldMenu::ROTOM_REQ_FINDER_STUDIO && req <= Savedata::FieldMenu::ROTOM_REQ_FES_CIRCLE )
  {
    m_debugRotomReq = req;
  }
  else
  {
    m_debugRotomReq = Savedata::FieldMenu::ROTOM_REQ_NONE;
  }
}
Savedata::FieldMenu::RotomRequest PlayData::DebugGetRotomRequest(void)
{
  return m_debugRotomReq;
}

void PlayData::DebugSetRotomReportFlag( bool flg )
{
  m_debugIsRotomReport = flg;
}
bool PlayData::DebugGetRotomReportFlag(void)
{
  return m_debugIsRotomReport;
}

void PlayData::DebugSetRotomFreeTalkID( u32 val )
{
  m_debugRotomFreeTalkID = val;
}
u32 PlayData::DebugGetRotomFreeTalkID(void)
{
  return m_debugRotomFreeTalkID;
}

void PlayData::DebugSetRotomGameClearMsgID( u32 val )
{
  m_debugRotomGameClearMsgID = val;
}
u32 PlayData::DebugGetRotomGameClearMsgID(void)
{
  return m_debugRotomGameClearMsgID;
}

#endif  // PM_DEBUG


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
