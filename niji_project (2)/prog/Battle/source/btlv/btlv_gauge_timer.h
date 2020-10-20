//======================================================================
/**
 * @file btlv_gauge_timer.h
 * @date 2015/11/11 18:56:15
 * @author kijima_satoshi
 * @brief 
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_GAUGE_TIMER_H__
#define __BTLV_GAUGE_TIMER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <System/include/SystemEventManager.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>


GFL_NAMESPACE_BEGIN(btl)


// クラス前方宣言
class BattleViewSystem;
GFL_NAMESPACE_BEGIN(app)
class BtlvUiMsgWin;
GFL_NAMESPACE_END(app)

//----------------------------------------------------------------------
/**
 * @class BtlvGaugeLevelUp
 * @brief 対戦用時間表示ウインドウクラス
 */
//----------------------------------------------------------------------
class BtlvGaugeTimer : public System::SystemEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvGaugeTimer);
 public:

  /**
   * @enum ANIM_IDX
   * アニメーションリスト
   */
  enum ANIM_IDX
  {
    PLAYER_GAUGE_IN_DISP_ANIME,        // プレイヤー時間制限表示用ゲージ入場アニメ
    SINGLE_ENEMY_GAUGE_IN_DISP_ANIME,  // シングル用敵持ち時間制表示アニメ
    ROYAL_GAUGE_IN_DISP_ANIME,         // ロイヤル用全ゲージ表示アニメ
    
    UPPER_GAUGE_NAME_DISP_TOTAL_TIME,   // 上側ゲージ名表示「TOTAL TIME」
    UPPER_GAUGE_NAME_DISP_YOUR_TIME,    // 上側ゲージ名表示「YOUR TIME」
    LOWER_GAUGE_NAME_DISP_COMMAND,      // 下側ゲージ名表示「COMMAND」

    ANM_MAX,
  };

  /**
   * @enum TIMER_TYPE
   * 制限時間のタイプ
   */
  enum TIMER_TYPE
  {
    TIMER_TYPE_GAME_TIME,       // 試合時間制
    TIMER_TYPE_CLIENT_TIME,     // 持ち時間制
    
    TIMER_TYPE_MAX,
  };

  // 表示するタイマー
  enum DISP_TIMER
  {
    DISP_TIMER_TOTAL_TIME,      // 試合時間 or 持ち時間
    DISP_TIMER_COMMAND_TIME,    // コマンド選択時間

    DISP_TIMER_MAX,
  };

  enum 
  {
    // 敵の持ち時間表示用
    ENEMY_TIME_DISP_PLUS1 = 0,
    ENEMY_TIME_DISP_PLUS2 = 1,
    ENEMY_TIME_DISP_PLUS3 = 2,
    ENEMY_TIME_DISP_SINGLE = 3,
    PLAYER_MAX = 4,
    
    // カバー閉じ処理の通信用
    P2P_MEMBER_NUM = 2,


    // 時間表示で警告色になる時間
    UPPER_TIME_ALERT_SEC = 5 * 60,
    COMMAND_TIME_ALERT_SEC = 15,

    
  };

  //通信コマンド ＝ ステータス
  enum
  {  
    _COVER_CLOSE,
    _COVER_CLOSE_SENDEND,
    
    _PUSH_START,
    _PUSH_START_SENDEND,
    
    _SEND_TIMER,
    _SEND_TIMER_SENDEND,
  };


  // -------------------------
  // Timer class
  // -------------------------
 private:

  class Timer
  {
  public:
    void SetTimeLimit( int time );
    s64 GetTimeLeft(void);
    bool IsFinish(void);
    void StartTimer(void);
    void AddTimeLimit( int time );

    void Clear(void);
    void Pause(bool isNet = false );
    s64 Restart(bool isNet = false );
    void Reset(void);
    
    void SetTimeLeft( int sec );

  private:
    s64 mTimeLimit;
    s64 mElapsedTick;
    bool mIsPause;
    bool mIsNetPause;

#if defined(GF_PLATFORM_CTR)
    nn::os::Tick mStartTick;
    nn::os::Tick mPauseTick;
#endif

#if defined(GF_PLATFORM_WIN32)
    u64 mStartTick;
    u64 mPauseTick;

    u64 getWinTick(void){
      LARGE_INTEGER longInteger;
      return (::QueryPerformanceCounter  (&longInteger) != FALSE) ? (u64) longInteger.QuadPart : 0ULL;
    }
    
    u64 getWinFrequency(void){
      LARGE_INTEGER longInteger;
      return (::QueryPerformanceFrequency(&longInteger) != FALSE) ? (u64) longInteger.QuadPart : 0ULL;
    }
    
    u64 winTickToSecond( u64 win_tick )
    {
      return (win_tick/getWinFrequency());
    }
#endif
    

  };

  // -------------------------
  // Constructor & Destructor
  // -------------------------
 public:
  BtlvGaugeTimer( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin);
  virtual ~BtlvGaugeTimer();


  // -------------------------
  // Methods
  // -------------------------

  void Initialize (void);
  void StartAnime( ANIM_IDX anime_num );
  void Update(void);
  void UpdateTimerDisp(void);
  void UpdateOtherPlayerTimerDisp(void);
  void SetEnemyTimeLeftVisible(bool visible);


  // バトルコア側から呼ばれるメソッド
  void SetTime( int game_time, int client_time, int command_time );
  void DispTimer( BTLV_TIMER_TYPE type,  bool isReset, bool isCountDownStart );
  void HideTimer( BTLV_TIMER_TYPE type );
  bool IsFinishTimer( BTLV_TIMER_TYPE type );
  u32 GetClientTime( u8 clientID );               // 各プレイヤーの持ち時間の取得
  void SetClientTime( u8 clientID, u32 time );    // 各プレイヤーの持ち時間のセット


 private:

  // レイアウト表示用
  gfl2::lyt::LytTextBox *GetTimeDispPartsTextBox( u32 pane_idx );
  void SetTimeToTextBox_SEC( int time_sec, gfl2::lyt::LytTextBox *textBox, print::NumberCodeType codeType = print::NUM_CODE_ZENKAKU );
  void SetTimeToTextBox_MMSS( int time_sec, gfl2::lyt::LytTextBox *textBox, print::NumberCodeType codeType = print::NUM_CODE_ZENKAKU );
  void SetTextBoxColor( gfl2::lyt::LytTextBox *textBox, u32 color_rgba8 );



  // カバー閉じ関連
  void DisableInput(void);  // ボタン系入力無効
  void EnableInput(void);   // ボタン系入力有効
  void ButtonOff( gfl2::ui::Button *pButton );
  void ButtonOn( gfl2::ui::Button *pButton );

  bool NetUpdate(void);
  void NetInitialize(void);
  void NetFinalize(void);
  void NetRecvTimer(void);
  bool NetSendCommand(u32 cmd,u32 num1,u32 num2);
  bool NetSendTimer(void);

  bool IsStop(void);
  bool AllStatus(u32 status);
  void BackupTime(void);
  void ReStartTime(u32 tim1,u32 tim2);


  // カバー閉じトリガ (Override from System::SystemEventListener)
  virtual void CallCloseCover( void );


  // -------------------------
  // Members
  // -------------------------
 private:
  BattleViewSystem          *mBtlvCore;
  ::app::util::G2DUtil      *mG2dUtil;
  app::BtlvUiMsgWin         *mUiMsgWin;

  int                       mLytIndex;
  gfl2::lyt::LytWk          *mGaugeLyt;

  gfl2::lyt::LytTextBox     *mDispTimeTextBox[DISP_TIMER_MAX];      // 自分用時間表示のTextBox
  gfl2::lyt::LytTextBox     *mOtherPlayerTimeTextBox[PLAYER_MAX];   // 対戦相手用時間表示のTextBox

  s32                       mPaneIdx[DISP_TIMER_MAX];               // 自分用時間表示のPane Index
  s32                       mOtherPlayerPaneIdx[PLAYER_MAX];        // 対戦相手用時間表示のPane Index

  s32                       mAnmTbl[ANM_MAX];                       // アニメーションID

  u8                        mClientID;                              // 自分のClientID
  u32                       mPlayerTimeLeft[PLAYER_MAX];            // 各プレイヤーの残り持ち時間(sec)

  TIMER_TYPE                mTimerType;                             // 制限時間のタイプ
  Timer                     mTimerCount[DISP_TIMER_MAX];            // 自分用タイマーカウント

  bool                      mUpperTimeStarted;                      // 試合時間or持ち時間のカウントが開始されたかどうか


  // カバー閉じ関連
  bool                      mbEnableTimeStop;        // カバー閉じポーズが有効かどうか
  bool                      mbPause;                 // カバー閉じポーズ中かどうか

  gflnet2::p2p::NetGame     *mpNetGame;
  u32                       mStatus[P2P_MEMBER_NUM];
  u32                       myStatusNo;
  bool                      mbCloseHook;
  bool                      mbPushHook;
  bool                      mbStartHook;


};      // End of class

GFL_NAMESPACE_END(btl)

#endif // __BTLV_GAUGE_TIMER_H__
