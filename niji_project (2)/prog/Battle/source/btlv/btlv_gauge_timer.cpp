//======================================================================
/**
 * @file btlv_gauge_timer.cpp
 * @date 2015/11/11 18:56:35
 * @author kijima_satoshi
 * @brief 
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include <niji_conv_header/app/battle/upper/minupp_pane.h>       //ゲージ類初期化用
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>  //ゲージ類初期化用

#include <niji_conv_header/message/msg_btl_ui.h>                // 文字列定義用
//#include <gflnet2/include/base/gflnet2_InitParam.h>


#include "Battle/source/btlv/BtlvUiMsgWin.h"
#include "./btlv_core.h"
#include "./btlv_gauge_timer.h"

#if PM_DEBUG
#include "../btl_mainmodule.h"
#include  "Battle/include/battle_SetupParam.h"
#endif

GFL_NAMESPACE_BEGIN(btl)

//-------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param[in] param : クラス初期化用パラメーター
 */
//-------------------------------------------------------------------------
BtlvGaugeTimer::BtlvGaugeTimer( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin)
  :mBtlvCore(btlvCore)
  ,mG2dUtil(g2dUtil)
  ,mUiMsgWin(uiMsgWin)
{
}

//-------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-------------------------------------------------------------------------
BtlvGaugeTimer:: ~BtlvGaugeTimer()
{
  if( mbEnableTimeStop )
    {
      NetFinalize();
      EnableInput();  //キー入力の復帰
    }
}

//-------------------------------------------------------------------------
/**
 * @brief 使用するレイアウトの初期化
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::Initialize (void)
{
  mLytIndex = 0; // レイアウトは１つのみ
  mGaugeLyt = mUiMsgWin->GetLayoutWork(mLytIndex);

  // 自プレイヤー時間表示用 Pane ID
  mPaneIdx[DISP_TIMER_TOTAL_TIME] = PANENAME_BATTLE_MIN_UPP_000_PANE_TOTALTIME;
  mPaneIdx[DISP_TIMER_COMMAND_TIME] = PANENAME_BATTLE_MIN_UPP_000_PANE_COMMANDTIME;

  // 通常対戦用敵プレイヤー時間表示 Pane ID
  mOtherPlayerPaneIdx[ENEMY_TIME_DISP_SINGLE] = PANENAME_BATTLE_MIN_UPP_000_PANE_SINGLE_TIME;

  // バトルロイヤル用 Pane ID
  mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS1] = PANENAME_BATTLE_MIN_UPP_000_PANE_ROYAL_TIME_02;  // 右下
  mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS2] = PANENAME_BATTLE_MIN_UPP_000_PANE_SINGLE_TIME;    // 右上
  mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS3] = PANENAME_BATTLE_MIN_UPP_000_PANE_ROYAL_TIME_00;  // 左上

  // TextBox
  mDispTimeTextBox[DISP_TIMER_TOTAL_TIME] = GetTimeDispPartsTextBox( mPaneIdx[DISP_TIMER_TOTAL_TIME] );
  mDispTimeTextBox[DISP_TIMER_COMMAND_TIME] = GetTimeDispPartsTextBox( mPaneIdx[DISP_TIMER_COMMAND_TIME] );

  mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_PLUS1] = GetTimeDispPartsTextBox( mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS1] );
  mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_PLUS2] = GetTimeDispPartsTextBox( mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS2] );
  mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_PLUS3] = GetTimeDispPartsTextBox( mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS3] );
  mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_SINGLE] = GetTimeDispPartsTextBox( mOtherPlayerPaneIdx[ENEMY_TIME_DISP_SINGLE] );

  // Animation list
  mAnmTbl[PLAYER_GAUGE_IN_DISP_ANIME]        = LA_MINUPP_BATTLE_MIN_UPP_000_IN_TIME;
  mAnmTbl[SINGLE_ENEMY_GAUGE_IN_DISP_ANIME]  = LA_MINUPP_BATTLE_MIN_UPP_000_TIME_SINGLE;
  mAnmTbl[ROYAL_GAUGE_IN_DISP_ANIME]         = LA_MINUPP_BATTLE_MIN_UPP_000_TIMEROYAL;

  mAnmTbl[UPPER_GAUGE_NAME_DISP_TOTAL_TIME]  = LA_MINUPP_BATTLE_MIN_UPP_000__TOTALTIME_TEXTPATERN_00;
  mAnmTbl[UPPER_GAUGE_NAME_DISP_YOUR_TIME]   = LA_MINUPP_BATTLE_MIN_UPP_000__TOTALTIME_TEXTPATERN_01;
  mAnmTbl[LOWER_GAUGE_NAME_DISP_COMMAND]     = LA_MINUPP_BATTLE_MIN_UPP_000__COMMANDTIME_TEXTPATERN_02;


  mTimerType = TIMER_TYPE_MAX;
  mClientID = mBtlvCore->GetClientID();
  mUpperTimeStarted = false;


  // カバー閉じ関連の初期化
  mpNetGame = NULL;
  mbPause = false;
  mbEnableTimeStop = false;

  if( ( mBtlvCore->GetEnableTimeStop() ) && (mBtlvCore->GetBattleRule() != BTL_RULE_ROYAL) )
    {
      // ライブ大会かつロイヤルではない
      mbEnableTimeStop = true;
    }

//#ifdef DEBUG_ONLY_FOR_kijima_satoshi
//  mbEnableTimeStop = true;
//#endif

  if( mbEnableTimeStop )
    {
      NetInitialize();
    }

#if PM_DEBUG
  if( mBtlvCore->GetBtlvMode() == BTLV_MODE_BATTLE )
  {
    bool isLiveRecPlayMode = mBtlvCore->GetMainModule()->GetBattleSetupParam()->isLiveRecPlayMode;
    if( isLiveRecPlayMode )
    {//観戦モード用
      mG2dUtil->SetPaneVisible( mLytIndex, mPaneIdx[DISP_TIMER_COMMAND_TIME], false );
    }
  }
#endif
}


//-------------------------------------------------------------------------
/**
 * @brief タイマーの初期化
 *
 * バトル開始時に呼ばれる
 */
//-------------------------------------------------------------------------

// タイマーの初期化
void BtlvGaugeTimer::SetTime ( int game_time, int client_time, int command_time )
{

  int total_time = 0;

  StartAnime( LOWER_GAUGE_NAME_DISP_COMMAND );     // ゲージ名セットアニメ

  if( client_time != 0)
    {
      // 持ち時間が設定されている場合は持ち時間制
      mTimerType = TIMER_TYPE_CLIENT_TIME;
      total_time = client_time;
      StartAnime( UPPER_GAUGE_NAME_DISP_YOUR_TIME );     // ゲージ名セットアニメ

      KIJIMA_PRINT( ">>!>> [Cliect ID:%d] SetTimerTime Moti time : %d\n", mClientID, client_time);
    }
  else
    {
      // 試合時間制
      mTimerType = TIMER_TYPE_GAME_TIME;
      total_time = game_time;
      StartAnime( UPPER_GAUGE_NAME_DISP_TOTAL_TIME );     // ゲージ名セットアニメ

      KIJIMA_PRINT( ">>!>>  [Client ID:%d] SetTimerTime Game time : %d, %d\n",mClientID, game_time, command_time );
    }

  // 自分用タイマーの初期化
  mTimerCount[DISP_TIMER_TOTAL_TIME].SetTimeLimit( total_time );
  mTimerCount[DISP_TIMER_COMMAND_TIME].SetTimeLimit( command_time );


  // 持ち時間制の場合
  if( mTimerType == TIMER_TYPE_CLIENT_TIME )
    {

      // 各プレイヤーの持ち時間初期化
      for(int i=0; i<PLAYER_MAX; i++)
        {
          mPlayerTimeLeft[i] = client_time;
        }

      // コマンドタイムが０場合は非表示
      if( command_time == 0 )
      {
        mG2dUtil->SetPaneVisible( mLytIndex, mPaneIdx[DISP_TIMER_COMMAND_TIME], false );
      }

      // 持ち時間制は相手のゲージも表示
      UpdateOtherPlayerTimerDisp();

      if( mBtlvCore->GetBattleRule() == BTL_RULE_ROYAL ) 
        {
          // 全ゲージ入場アニメ
          StartAnime( ROYAL_GAUGE_IN_DISP_ANIME );
        }
      else
        {
          // シングル用敵持ち時間制表示アニメ 
          StartAnime( SINGLE_ENEMY_GAUGE_IN_DISP_ANIME );

          // プレイヤー時間制限表示用ゲージ入場アニメ
          StartAnime( PLAYER_GAUGE_IN_DISP_ANIME ); 
        }
    }
  else
    {
      // 試合時間制の場合
      mTimerCount[DISP_TIMER_TOTAL_TIME].StartTimer();  // カウント開始
      mUpperTimeStarted = true;

      // プレイヤー時間制限表示用ゲージ入場アニメ
      StartAnime( PLAYER_GAUGE_IN_DISP_ANIME ); 
    }


  // 自分用タイマー更新
  UpdateTimerDisp();

}

//-------------------------------------------------------------------------
/**
 * @brief 各ゲージの表示
 * @param isReset              残り時間をリセットするか？
 * @param isCommandTimeStart   残り時間のカウントダウンを開始するか？
 *
 * 各ゲージの表示タイミングで呼ばれる
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::DispTimer( BTLV_TIMER_TYPE type,  bool isReset, bool isCountDownStart )
{

#if PM_DEBUG
  if( mBtlvCore->GetBtlvMode() == BTLV_MODE_BATTLE )
  {
    bool isLiveRecPlayMode = mBtlvCore->GetMainModule()->GetBattleSetupParam()->isLiveRecPlayMode;
    if( isLiveRecPlayMode )
    {//観戦モード用
      if( type != BTLV_TIMER_TYPE_GAME_TIME )
      {//トータルタイム以外は表示させない
        return;
      }
    }
  }

#endif

  if( isReset )
    {
      switch( type )
        {

        case BTLV_TIMER_TYPE_GAME_TIME:
          {
            mG2dUtil->SetPaneVisible( mLytIndex, mPaneIdx[DISP_TIMER_TOTAL_TIME], true );
          }
          break;

        case BTLV_TIMER_TYPE_CLIENT_TIME:
          {
            // 敵の持ち時間表示ON
            SetEnemyTimeLeftVisible( true ) ;

            // 表示時にカウント再開
            if( mUpperTimeStarted )
              {
                mTimerCount[DISP_TIMER_TOTAL_TIME].Restart();
              }
          }
          break;
          
        case BTLV_TIMER_TYPE_COMMAND_TIME:
          {
            mG2dUtil->SetPaneVisible( mLytIndex, mPaneIdx[DISP_TIMER_COMMAND_TIME], true ); 

            // リセット
            mTimerCount[DISP_TIMER_COMMAND_TIME].Reset();
          }
          break;
        }

    }
  else
    {
      // にここには来ない模様 (常にisReset=true )
      KIJIMA_PRINT(">>> No isReset\n");
    }

  // カウント開始
  if( isCountDownStart )
  {
    switch( type )
    {
      
    case BTLV_TIMER_TYPE_CLIENT_TIME:
      // 持ち時間制の場合はここでスタート
      mTimerCount[DISP_TIMER_TOTAL_TIME].StartTimer();
      mUpperTimeStarted = true;
      break;

    case BTLV_TIMER_TYPE_COMMAND_TIME:
      mTimerCount[DISP_TIMER_COMMAND_TIME].StartTimer();
      break;

    }
  }

}

//-------------------------------------------------------------------------
/**
 * @brief 各ゲージの表示OFF
 *
 * コマンド選択終了時に呼ばれる
 *
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::HideTimer( BTLV_TIMER_TYPE type )
{

  switch( type )
    {

    case BTLV_TIMER_TYPE_GAME_TIME:
      {
        mG2dUtil->SetPaneVisible( mLytIndex, mPaneIdx[DISP_TIMER_TOTAL_TIME], false );
      }
      break;

    case BTLV_TIMER_TYPE_CLIENT_TIME:
      {
        // 敵の持ち時間表示OFF
        SetEnemyTimeLeftVisible( false );

        // 消さずにポーズ
        mTimerCount[DISP_TIMER_TOTAL_TIME].Pause();
      }
      break;

    case BTLV_TIMER_TYPE_COMMAND_TIME:
      {
        mG2dUtil->SetPaneVisible( mLytIndex, mPaneIdx[DISP_TIMER_COMMAND_TIME], false );
      }
      break;
    }



}

//-------------------------------------------------------------------------
/**
 * @brief 指定のタイマーがタイムオーバーかのチェック
 *
 * 試合時間制の時のみ呼ばれる
 */
//-------------------------------------------------------------------------
bool BtlvGaugeTimer::IsFinishTimer(  BTLV_TIMER_TYPE type )
{

  bool ret = false;
 
  switch( type )
    {

    case BTLV_TIMER_TYPE_GAME_TIME:
    case BTLV_TIMER_TYPE_CLIENT_TIME:
      {
        ret = mTimerCount[DISP_TIMER_TOTAL_TIME].IsFinish();
      }
      break;

    case BTLV_TIMER_TYPE_COMMAND_TIME:
      {
        ret = mTimerCount[DISP_TIMER_COMMAND_TIME].IsFinish();
      }
      break;
    }

  return ret;

}

//-------------------------------------------------------------------------
/**
 * @brief 各プレイヤーの持ち時間の取得
 *
 * 持ち時間制の時のみ呼ばれる
 */
//-------------------------------------------------------------------------
u32 BtlvGaugeTimer::GetClientTime( u8 clientID )
{
  if( clientID == mClientID )
    {
      s64 tleft = mTimerCount[DISP_TIMER_TOTAL_TIME].GetTimeLeft();
      if( tleft < 0 )
        {
          tleft = 0;
        }

      mPlayerTimeLeft[mClientID] = (u32)tleft;

      KIJIMA_PRINT( ">>!>>Moti Time stored [%d] = %d",clientID, tleft );

    }

  u32 ret =   mPlayerTimeLeft[clientID];

  KIJIMA_PRINT( ">>!>> [Client ID:%d]GetPlayerMotiTime [%d] = %d\n", mClientID, clientID, ret );

  return( ret );

}

//-------------------------------------------------------------------------
/**
 * @brief 各プレイヤーの持ち時間のセット
 *
 * 持ち時間制の時のみ呼ばれる
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::SetClientTime( u8 clientID, u32 time )
{
  KIJIMA_PRINT( ">>!>> [Client ID:%d]SetPlayerMotiTime [%d] = %d\n",mClientID, clientID, time );

  mPlayerTimeLeft[clientID] = time;

  // 対戦相手の表示更新
  UpdateOtherPlayerTimerDisp();

}

//-------------------------------------------------------------------------
// アニメーション再生
//-------------------------------------------------------------------------
void BtlvGaugeTimer::StartAnime (ANIM_IDX anime_num )
{
  mG2dUtil->StartAnime( mLytIndex, mAnmTbl[anime_num] );
}

//-------------------------------------------------------------------------
/**
 * @brief アップデート
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::Update(void)
{
  if( NetUpdate() )
    {
      // NetUpdate()の処理がある場合は通常の処理はスキップ
      //タイマーの表示更新処理は必要
      UpdateTimerDisp();
      return;
    }

  if( mTimerType != TIMER_TYPE_MAX )
    {
      UpdateTimerDisp();
    }
}

//-------------------------------------------------------------------------
/**
 * @brief 自分用タイマー表示の時間をアップデート
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::UpdateTimerDisp(void)
{
  int time_sec;

  //  試合時間 or 持ち時間
  time_sec = mTimerCount[DISP_TIMER_TOTAL_TIME].GetTimeLeft();

  // 表示はマイナスにならいように
  if (time_sec < 0)
    {
      time_sec = 0;
    }

  //  SetTimeToTextBox_SEC( time_sec, mDispTimeTextBox[DISP_TIMER_TOTAL_TIME] );  // 秒表示 (tmp)
  SetTimeToTextBox_MMSS( time_sec, mDispTimeTextBox[DISP_TIMER_TOTAL_TIME] );  // MM:SS 形式


  // コマンド選択時間
  time_sec = mTimerCount[DISP_TIMER_COMMAND_TIME].GetTimeLeft();
  
  // 表示はマイナスにならいように
  if (time_sec < 0)
    {
      time_sec = 0;
    }
  
  SetTimeToTextBox_SEC( time_sec, mDispTimeTextBox[DISP_TIMER_COMMAND_TIME] );

}

//-------------------------------------------------------------------------
/**
 * @brief 敵プレイヤーのタイマー表示の時間をアップデート
 *
 * Note : 持ち時間表示リアルタイム更新の必要はない？
 */
//-------------------------------------------------------------------------
void BtlvGaugeTimer::UpdateOtherPlayerTimerDisp(void)
{

  if( mTimerType != TIMER_TYPE_CLIENT_TIME )
    {
      return;
    }

  if( mBtlvCore->GetBattleRule() == BTL_RULE_ROYAL ) 
    {
      int client_id;

      // ClientIDは自分相対で反時計回り

      // 右下
      client_id = (mClientID + 1) % PLAYER_MAX;
      SetTimeToTextBox_MMSS( mPlayerTimeLeft[client_id], mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_PLUS1] );


      // 右上
      client_id = (mClientID + 2) % PLAYER_MAX;
      SetTimeToTextBox_MMSS( mPlayerTimeLeft[client_id], mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_PLUS2] );


      // 左上
      client_id = (mClientID + 3) % PLAYER_MAX;
      SetTimeToTextBox_MMSS( mPlayerTimeLeft[client_id], mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_PLUS3] );
    }
  else
    {
      // 相手の情報を表示
      int enemy_cid;
      if(mClientID == 0 )
        {
          enemy_cid = 1;
        }
      else
        {
          enemy_cid = 0;
        }

      SetTimeToTextBox_MMSS( mPlayerTimeLeft[enemy_cid], mOtherPlayerTimeTextBox[ENEMY_TIME_DISP_SINGLE] );
    }

}

// 時間表示用パーツからTextBoxPaneを取得
// Note : パーツ内パーツにテキストボックスがある
gfl2::lyt::LytTextBox* BtlvGaugeTimer::GetTimeDispPartsTextBox( u32 pane_idx )
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::lyt::LytParts *parts = mGaugeLyt->GetPartsPane( pane_idx );
  gfl2::lyt::LytParts *sub_parts = mGaugeLyt->GetPartsPane( parts, PANENAME_BATTLE_MIN_UPP_000_PANE_TIME, &rMultiResId );
  gfl2::lyt::LytTextBox *textBox = mGaugeLyt->GetTextBoxPane( sub_parts, PANENAME_BATTLE_PLT_UPP_003_PANE_TEXTIME_00, &rMultiResId );

  return( textBox );
}


// 指定テキストボックスに時間をセット：３桁秒形式表示
void BtlvGaugeTimer::SetTimeToTextBox_SEC( int time_sec, gfl2::lyt::LytTextBox *textBox, print::NumberCodeType codeType )
{
  gfl2::str::StrBuf *pMsgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *pExpandStr = mBtlvCore->GetWorkStr(1);
  print::WordSet *pWset = mBtlvCore->GetWordSet();

  mBtlvCore->GetStringUi( pMsgStr, BTLSTR_UI_TIMER_TIME2 );
  pWset->RegisterNumber( 0 , time_sec, 3 , print::NUM_DISP_SPACE , codeType );
  pWset->Expand( pExpandStr , pMsgStr );
  print::SetStringForLytTextBox( textBox , pExpandStr );

  // 文字色の設定
  if( time_sec <= COMMAND_TIME_ALERT_SEC )
    {
      SetTextBoxColor( textBox, nw::ut::Color8::RED );
    }
  else
    {
      SetTextBoxColor( textBox, nw::ut::Color8::WHITE );
    }
}

// 指定テキストボックスに時間をセット：MM:SS形式表示
void BtlvGaugeTimer::SetTimeToTextBox_MMSS( int time_sec, gfl2::lyt::LytTextBox *textBox, print::NumberCodeType codeType )
{
  gfl2::str::StrBuf *pMsgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *pExpandStr = mBtlvCore->GetWorkStr(1);
  print::WordSet *pWset = mBtlvCore->GetWordSet();

  // 文字色の設定
  if( time_sec <= UPPER_TIME_ALERT_SEC )
    {
      SetTextBoxColor( textBox, nw::ut::Color8::RED );
    }
  else
    {
      SetTextBoxColor( textBox, nw::ut::Color8::WHITE );
    }

  int time_min;

  time_min = time_sec/60;
  time_sec = time_sec%60;
  
  mBtlvCore->GetStringUi( pMsgStr, BTLSTR_UI_TIMER_TIME1 );
  pWset->RegisterNumber( 0 , time_min , 2 , print::NUM_DISP_SPACE, codeType );
  pWset->RegisterNumber( 1 , time_sec , 2 , print::NUM_DISP_ZERO, codeType );

  
  pWset->Expand( pExpandStr , pMsgStr );
  print::SetStringForLytTextBox( textBox , pExpandStr );

}

// テキストボックスの文字色を設定
void BtlvGaugeTimer::SetTextBoxColor( gfl2::lyt::LytTextBox *textBox, u32 color_rgba8 )
{
  nw::ut::Color8 vcol = color_rgba8;

  textBox->SetVtxColor( nw::lyt::VERTEXCOLOR_LT, vcol );
  textBox->SetVtxColor( nw::lyt::VERTEXCOLOR_LB, vcol );
  textBox->SetVtxColor( nw::lyt::VERTEXCOLOR_RT, vcol );
  textBox->SetVtxColor( nw::lyt::VERTEXCOLOR_RB, vcol );

}

// 敵の残り持ち時間表示のVisible設定
void BtlvGaugeTimer::SetEnemyTimeLeftVisible(bool visible)
{
  if( mTimerType != TIMER_TYPE_CLIENT_TIME )
    {
      return;
    }

  if( mBtlvCore->GetBattleRule() == BTL_RULE_ROYAL ) 
    {
      mG2dUtil->SetPaneVisible( mLytIndex, mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS1], visible );
      mG2dUtil->SetPaneVisible( mLytIndex, mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS2], visible );
      mG2dUtil->SetPaneVisible( mLytIndex, mOtherPlayerPaneIdx[ENEMY_TIME_DISP_PLUS3], visible );
    }
  else
    {
      mG2dUtil->SetPaneVisible( mLytIndex, mOtherPlayerPaneIdx[ENEMY_TIME_DISP_SINGLE], visible );
    }
}

//-------------------------------------------------------------------------
/**
 * @brief タイマー用インナークラス
 */
//-------------------------------------------------------------------------


// --------------------------
// 制限時間のセット
void BtlvGaugeTimer::Timer::SetTimeLimit( int time )
{
  mTimeLimit = time;
  mElapsedTick = 0;
  mIsPause = true;
}

// --------------------------
// リセット
// （制限時間設定はそのままで経過時間をリセット＋ポーズ）
void  BtlvGaugeTimer::Timer::Reset(void)
{
  mElapsedTick = 0;
  mIsPause = true;
}

// --------------------------
// 時間計測スタート
void BtlvGaugeTimer::Timer::StartTimer(void)
{

#if defined(GF_PLATFORM_CTR)
  mStartTick = nn::os::Tick::GetSystemCurrent();
  //  mPauseTick = nn::os::Tick::GetSystemCurrent();
  mPauseTick = mStartTick;
#endif

#if defined(GF_PLATFORM_WIN32)
  mStartTick = getWinTick();
  mPauseTick = mStartTick;
#endif
  mIsPause = false;
}


// --------------------------
// 残り時間の取得
// ret : 残り時間 (sec)
s64 BtlvGaugeTimer::Timer::GetTimeLeft(void)
{
  s64 time_left = 0;

  if( mIsPause == true || mIsNetPause == true )
    {
#if defined(GF_PLATFORM_CTR)
      nn::os::Tick elapsedTick(mElapsedTick);
      s64 past_sec = elapsedTick.ToSeconds();
      time_left = mTimeLimit - past_sec;
#endif

#if defined(GF_PLATFORM_WIN32)
      u64 past_sec = winTickToSecond( (u64)mElapsedTick );
      time_left = mTimeLimit - past_sec;
#endif

    }
  else
    {

#if defined(GF_PLATFORM_CTR)
      nn::os::Tick now_tick = nn::os::Tick::GetSystemCurrent();
      s64 now_sec = (now_tick-mStartTick).ToSeconds();
      time_left = (mTimeLimit - now_sec);

      nn::os::Tick elapsedTick(mElapsedTick);
      s64 past_sec = elapsedTick.ToSeconds();
      time_left -= past_sec;
#endif

#if defined(GF_PLATFORM_WIN32)
      u64 now_tick = getWinTick();
      u64 now_sec = winTickToSecond( (now_tick - mStartTick) );
      time_left = (mTimeLimit - now_sec);

      u64 past_sec = winTickToSecond( (u64)mElapsedTick );
      time_left -= past_sec;
#endif
    }

  return( time_left );

}

// --------------------------
// タイムオーバーチェック
bool BtlvGaugeTimer::Timer::IsFinish(void)
{
  if( GetTimeLeft() < 0 )
    {
      return true;
    }
  return false;
}

// --------------------------
// 制限時間の追加
void BtlvGaugeTimer::Timer::AddTimeLimit(int time)
{
  mTimeLimit += time;
}

// --------------------------
// 時間計測ポーズ
void BtlvGaugeTimer::Timer::Pause(bool isNet )
{

  // ポーズ中なら何もしない
  if( mIsPause == false && mIsNetPause == false )
  {
     
#if defined(GF_PLATFORM_CTR)
    mPauseTick = nn::os::Tick::GetSystemCurrent();
    nn::os::Tick now_tick = mPauseTick;
    mElapsedTick += (s64)(now_tick - mStartTick);
    nn::os::Tick zero(0);
    mStartTick = zero;
#endif
      
#if defined(GF_PLATFORM_WIN32)
    mPauseTick = getWinTick();
    u64 now_tick = mPauseTick;
    mElapsedTick += (now_tick - mStartTick);
    mStartTick = 0;
#endif
      
  }
  if( isNet == false )
  {
    //通常ポーズ
    mIsPause = true;
  }
  else
  {
    //ライブ大会用ポーズ
    mIsNetPause = true;
  }
}

// --------------------------
// 時間計測ポーズ再開
s64 BtlvGaugeTimer::Timer::Restart(bool isNet )
{
  s64 pause_time = 0;

  bool isRestart = false;

  if( isNet == false && mIsPause == true )
  {
    //通常ポーズ
    mIsPause = false;
    isRestart = true;
  }
  else
  if( isNet == true && mIsNetPause == true )
  {
    //ライブ大会用ポーズ
    mIsNetPause = false;
    isRestart = true;
  }

  // ポーズ中が解除された時に処理をする
  if( mIsPause == false && mIsNetPause == false && isRestart == true )
  {

#if defined(GF_PLATFORM_CTR)
    mStartTick = nn::os::Tick::GetSystemCurrent();
    pause_time = (mPauseTick - mStartTick).ToSeconds();
    nn::os::Tick zero(0);
    mPauseTick = zero;
      
    ////  AddTimeLimit( pauseTime );
#endif
  
#if defined(GF_PLATFORM_WIN32)
    mStartTick = getWinTick();
    pause_time = winTickToSecond(( mPauseTick - mStartTick) );
    mPauseTick = 0;
#endif

  }

  return (pause_time);
}

void BtlvGaugeTimer::Timer::Clear(void)
{
  mTimeLimit = 0;
  mElapsedTick = 0;
  mIsPause = true;

#if defined(GF_PLATFORM_CTR)
  nn::os::Tick zero(0);
  mStartTick = zero;
  mPauseTick = zero;
#endif

#if defined(GF_PLATFORM_WIN32)
  mStartTick = 0;
  mPauseTick = 0;
#endif


}

void BtlvGaugeTimer::Timer::SetTimeLeft( int sec )
{
  s64 diff_sec =  mTimeLimit - sec;

  if( diff_sec < 0 )
    {
      diff_sec = 0;
    }

#if defined(GF_PLATFORM_CTR)
  s64 passed_tick;

  passed_tick = diff_sec * nn::os::Tick::TICKS_PER_SECOND;
  mElapsedTick = passed_tick;
#endif

#if defined(GF_PLATFORM_WIN32)
  u64 passed_tick;
  u64 tick_per_sec = getWinFrequency();

  passed_tick = diff_sec * tick_per_sec;
  mElapsedTick = passed_tick;
#endif

}

//-------------------------------------------------------------------------
/**
 * @brief カバー閉じ処理関連
 */
//-------------------------------------------------------------------------


// カバー閉じトリガ
void BtlvGaugeTimer::CallCloseCover(void)
{
  int net_id = mpNetGame->GetNetID();
  if( net_id >= 0 )
    {
      KIJIMA_PRINT(">>! Cover close [Net ID = %d]\n", mpNetGame->GetNetID());
      if(mStatus[net_id] == _PUSH_START)
        {
          mbPushHook = false;
          mbCloseHook = true;
          mStatus[net_id] = _COVER_CLOSE;
          myStatusNo = _COVER_CLOSE;
        }
    }

}

void BtlvGaugeTimer::NetInitialize(void)
{

  gfl2::heap::HeapBase* pMemHeap = mBtlvCore->GetResidentHeap();


//  KIJIMA_PRINT("ResidentHeap before Total[%12x] Free[%12x] Alloc[%12x]\n",
//               pMemHeap->GetTotalSize(),
//               pMemHeap->GetTotalFreeSize(),
//               pMemHeap->GetTotalAllocatableSize());

  mpNetGame = GFL_NEW( pMemHeap ) gflnet2::p2p::NetGame();
  mpNetGame->Initialize();

//  KIJIMA_PRINT("ResidentHeap after Total[%12x] Free[%12x] Alloc[%12x]\n",
//               pMemHeap->GetTotalSize(),
//               pMemHeap->GetTotalFreeSize(),
//               pMemHeap->GetTotalAllocatableSize());


  for(int i=0 ; i < P2P_MEMBER_NUM ; i++)
    {
      mStatus[i] = _PUSH_START;
    }

  // Regist Cover close trigger
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
  myStatusNo = _PUSH_START_SENDEND;

  KIJIMA_PRINT (">>! P2P instace initialized. [%d]\n", mpNetGame->GetNetID());
 
}

void BtlvGaugeTimer::NetFinalize(void)
{
  if( mpNetGame )
    {
      mpNetGame->Finalize();
      GFL_SAFE_DELETE( mpNetGame );
      mpNetGame = NULL;
    }

  // Remove Cover close Trigger
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);

}

bool BtlvGaugeTimer::NetUpdate(void)
{

  if( false == mbEnableTimeStop )
    {
      return false;
    }

  if( NULL == mpNetGame )
    {
      return false;
    }

  if( !mpNetGame->IsInitialize() )
    {
      return false;
    }


  NetRecvTimer();  //受信


  // キー入力禁止中のキーチェック
  const gfl2::ui::Button *pButton = mBtlvCore->GetButton();

  if( pButton->IsHold(gfl2::ui::BUTTON_DOWN) &&  // 方向キーは禁止されていない
      pButton->IsTrigger(gfl2::ui::BUTTON_B, gfl2::ui::Button::INPUT_STATE_ORIGINAL) )  
    {
      int net_id = mpNetGame->GetNetID();
  
      if( net_id >= 0 )
        {
          KIJIMA_PRINT(">>! ↓Ｂおした [Net ID = %d] \n",mpNetGame->GetNetID());
          if(mStatus[net_id] == _COVER_CLOSE)
            {
              mbCloseHook = false;
              mbPushHook = true;
              mStatus[net_id] = _PUSH_START;
            }
        }
    }
  
  //送信部
  if(mbCloseHook){
    if(NetSendCommand(_COVER_CLOSE,0,0)){
      KIJIMA_PRINT(">>! NetSendCommand _COVER_CLOSE\n");
      mbCloseHook=false;
    }
  }
  if(mbPushHook){
    if(NetSendCommand(_PUSH_START,0,0)){
      KIJIMA_PRINT(">>! NetSendCommand _PUSH_START\n");
      mbPushHook=false;
    }
  }

  //バックアップ部
  if(AllStatus(_COVER_CLOSE)){
    BackupTime();
  }

  //再開のため時間を送信
  if(myStatusNo!=_PUSH_START_SENDEND){
    if(AllStatus(_PUSH_START)){
      if(NetSendTimer()){
        KIJIMA_PRINT(">>! Restart Timer. \n");
        myStatusNo = _PUSH_START_SENDEND;
      }
    }
  }

  return IsStop();
}


bool BtlvGaugeTimer::IsStop(void)
{
  return mbPause;
}

void BtlvGaugeTimer::NetRecvTimer(void)
{
  u32 buff[3];

  for(int i=0 ; i < P2P_MEMBER_NUM ; )
    {
      if(mpNetGame->RecvData( i, (char*)buff, sizeof(buff) ))
        {
          switch(buff[0])
            {

            case _COVER_CLOSE:
              KIJIMA_PRINT(">>! _COVER_CLOSE受信 %d\n",i);
              mStatus[i] = _COVER_CLOSE;
              break;

            case _PUSH_START:
              mStatus[i] = _PUSH_START;
              KIJIMA_PRINT(">>! _PUSH_START 受信 %d\n",i);
              break;

            case _SEND_TIMER:
              ReStartTime(buff[1],buff[2]);
              break;
            }
        }
      else
        {
          i++;
        }
    }
}


bool BtlvGaugeTimer::NetSendCommand(u32 cmd,u32 num1,u32 num2)
{
  u32 buff[3];

  buff[0] = cmd;
  buff[1] = num1;
  buff[2] = num2;
  if(mpNetGame->SendData( (char*)buff, sizeof(buff))){
    return true;
  }
  return false;
}


bool BtlvGaugeTimer::AllStatus(u32 status)
{
  for(int i=0 ; i < P2P_MEMBER_NUM ; i++){
    if(mStatus[i] != status){
      return false;
    }
  }
  return true;
}


void BtlvGaugeTimer::BackupTime(void)
{
  bool bFlg=false;

  if( mbPause == false ){

    // 各タイマーポーズ
    mTimerCount[DISP_TIMER_TOTAL_TIME].Pause(true);
    mTimerCount[DISP_TIMER_COMMAND_TIME].Pause(true);

    mbPause = true;
    bFlg = true;
  }

  if(bFlg)
    {
      //キー入力禁止
      DisableInput();
    }
}




void BtlvGaugeTimer::ReStartTime(u32 tim1,u32 tim2)
{

  // タイマーの同期は現在行っていない

  // 各タイマーリスタート
  mTimerCount[DISP_TIMER_TOTAL_TIME].Restart(true);
  mTimerCount[DISP_TIMER_COMMAND_TIME].Restart(true);


  mbPause = false;

  // キー入力許可
  EnableInput();

}

bool BtlvGaugeTimer::NetSendTimer(void)
{
  if(mpNetGame->IsMaster())
    {
      // タイマーの同期は現在行っていない

      //    int time =  mGameTimer.GetRestart();
      //    int ctime =  mCommandTimer.GetRestart();
      int time = 0; 
      int ctime = 0; 

      KIJIMA_PRINT("NetSendTimer %d %d\n",time,ctime);
      if(NetSendCommand( _SEND_TIMER, time, ctime ) ){
        return true;
      }
    }

  return false;
}


// タッチパネルとボタンの入力を無効にする
// （方向キーは効く）
void BtlvGaugeTimer::DisableInput (void)
{
  gfl2::ui::DeviceManager* manager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  if( manager ) 
    {
      manager->GetTouchPanel( 0 )->SetDeviceRunningEnable( false );
      
#if defined(GF_PLATFORM_CTR)
      ButtonOff(manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STANDARD ));
      ButtonOff(manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STICK_EMU ));
#endif
      
    }
    
  KIJIMA_PRINT (">>! Key input DISABLED.\n");

}

// 無効にしたタッチパネルとボタンの入力を有効にする
void BtlvGaugeTimer::EnableInput (void)
{
  gfl2::ui::DeviceManager* manager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  if( manager ) 
  {
    manager->GetTouchPanel( 0 )->SetDeviceRunningEnable( true );
      
#if defined(GF_PLATFORM_CTR)
    ButtonOn(manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STANDARD ));
    ButtonOn(manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STICK_EMU ));

    if( mBtlvCore->GetConfigSaveData() )
    {
      //L=Aモードの復帰
      mBtlvCore->GetConfigSaveData()->SetButtonMode( mBtlvCore->GetConfigSaveData()->GetButtonMode(),
                                                    manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STANDARD ),
                                                    manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STICK_EMU ));
    }
#endif
  }

  KIJIMA_PRINT (">>! Key input ENABLED.\n");

  // ボタン設定の復帰
  //  GameSys::GameData* gameData = mBtlvCore->GetGameData();
  //  if( gameData )
  //    {
  //      savedata::Config *cfg = gameData->GetConfig();
  //      cfg->SetButtonMode( cfg->GetButtonMode() , 
  //                          manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STANDARD ) , 
  //                          manager->GetButton( gfl2::ui::CTR_DeviceManager::BUTTON_STICK_EMU ) );
  //    }

}
  

void BtlvGaugeTimer::ButtonOff( gfl2::ui::Button *pButton )
{
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_A);
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);  //L=Aモード対応
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_B, gfl2::ui::BUTTONID_B);
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_X, gfl2::ui::BUTTONID_X);
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_Y, gfl2::ui::BUTTONID_Y);
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
  pButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
}

void BtlvGaugeTimer::ButtonOn(  gfl2::ui::Button* pButton )
{
  pButton->AddButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_A);
  pButton->AddButtonAssignment(gfl2::ui::BUTTONID_B, gfl2::ui::BUTTONID_B);
  pButton->AddButtonAssignment(gfl2::ui::BUTTONID_X, gfl2::ui::BUTTONID_X);
  pButton->AddButtonAssignment(gfl2::ui::BUTTONID_Y, gfl2::ui::BUTTONID_Y);
  pButton->AddButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
  pButton->AddButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
}


GFL_NAMESPACE_END(btl)
