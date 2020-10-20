//======================================================================
/**
 * @file btlv_convention_win.h
 * @date 2016/01/19 15:31:40
 * @author kijima_satoshi
 * @brief 大会演出用プレイヤー情報ウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_CONVENTION_WIN_H__
#define __BTLV_CONVENTION_WIN_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <AppLib/include/Util/app_util_2d.h>



GFL_NAMESPACE_BEGIN(btl)

// クラス前方宣言
class BattleViewSystem;
GFL_NAMESPACE_BEGIN(app)
class BtlvUiMsgWin;
GFL_NAMESPACE_END(app)


#ifdef DEBUG_ONLY_FOR_kijima_satoshi
//#define KIJIMA_LOCAL_TEST 1
#endif

//----------------------------------------------------------------------
/**
 * @class BtlvConventionWin
 * @brief 大会演出用プレイヤー情報ウィンドウ
 */
//----------------------------------------------------------------------
class BtlvConventionWin
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvConventionWin);
 public:

  //-------------------------------------
  // Constructor & Destructor
  //-------------------------------------
  BtlvConventionWin( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin);
  virtual ~BtlvConventionWin(){}

  struct EffectInfo
  {
    u32 effect_num;
    u32 message_id;
  };

  struct EffectDispInfo
  {
    s16 side;
    s16 ex_type;
    s16 sup_type;
    s16 turn;
    s32 msg_id;
  };

  // アニメーションID
  enum ANIM_ID
  {
    ANM_DISP_PLAYER0_INFO,
    ANM_DISP_PLAYER1_INFO,
    ANM_DISP_PLAYER2_INFO,
    ANM_DISP_PLAYER3_INFO,

    ANM_MAX,
  };


  // Status
  enum DISP_STATUS
  {
    STATUS_UNINIT,      // 未初期化
    STATUS_WAIT_START,  // 開始待ち待機
    STATUS_DISP,        // 表示中
    STATUS_IDLE,        // 中断中
  };


  // 表示するデータ種類 
  enum 
  {
    DATA_PLAYER_RATE,       ///< プレイヤー情報(Rate)
    DATA_TURN,              ///< ターン数
    DATA_POKE_TYPE,         ///< ポケモンのタイプ
    //    DATA_PLAYER_COUNTRY,    ///< 住んでいる国

    DATA_WEATHER,           // 天候
    DATA_GROUND_EFFECT,     // 地形効果
    DATA_SIDE_EFFECT,       // 陣営効果
    DATA_POSISION_EFFECT,   // 位置エフェクト
    DATA_PLACE_EFFECT,      // 場の効果

    DATA_POKE_SICK,         // ポケモン状態異常
    DATA_POKE_EX_TYPE,      // ポケモンタイプ拡張
    DATA_POKE_CONTINUE,     // ポケモン継続フラグ

    DATA_MAX,
  };


  static const int WINDOW_PANE_MAX = 4;
  static const int WAIT_FRAME = 150;    // 情報表示までの待ちフレーム数
  static const int DISP_FRAME = 60*2;   // 情報表示フレーム数

  static const int DISP_INFO_LIST_MAX = (14*BTL_SIDE_NUM);  // 表示情報保持リスト最大数（各タイプ共用）


  //-------------------------------------
  // Methods
  //-------------------------------------
  void Initialize(void);
  void Start (void);        // 開始、再開
  void Update (void);       // 毎フレーム更新
  void Break (void);        // 中断

  void ChangeNext(void);    // 次の表示へ切り替え
  int SearchNextDataType( int current_dtype );

  
  void HideWindow(void);
  void ClearWindowTextBox( int pos );

  void SetTextboxPanePlayerName( int pos, const Savedata::MyStatus *myst, u32 paneId );
  void SetTextboxPaneCountry( int pos, const Savedata::MyStatus *myst, u32 paneId );
  void SetTextboxPaneNumber( int pos, u32 messageId,u32 paneId, u32 value, u32 keta, int bufNum = 0 );
  void SetTextboxPaneMessage( int pos, gfl2::str::StrBuf *pMsg, u32 paneId );
  void SetTextboxPaneMessageUI( int pos, u32 messageId, u32 paneId );
  void SetTextboxPaneMessageSTATE( int pos, u32 messageId, u32 paneId );
  void SetTextboxPaneWaza( int pos, u32 messageId,u32 paneId, u32 wazaID, int bufNum = 0 );
  void SetTextboxPanePokeType( int pos, u32 messageId,u32 paneId, pml::PokeType typeID, int bufNum = 0 );



  // 表示情報の有無チェック
  bool IsExistDispData( int dtype );
  bool IsExistWeather(void);
  bool IsExistGroundEffect(void);
  bool IsExistSideEffect(void);
  bool IsExistPositionEffect(void);
  bool IsExistPlaceEffect(void);
  bool IsExistPokeSick(void);
  bool IsExistPokeExType(void);
  bool IsExistPokeContinue(void);
  bool IsExistPokeType(void);

  // 情報のセット
  void SetWindowInfo(void);

  void SetWindowInfo_PlayerCountry( int pos );
  void SetWindowInfo_PlayerRate( int pos );
  void SetWindowInfo_Weather( int pos );
  void SetWindowInfo_GroundEffect( int pos );
  void SetWindowInfo_SideEffect( int pos );
  void SetWindowInfo_PositionEffect( int pos );
  void SetWindowInfo_PlaceEffect( int pos );
  void SetWindowInfo_PokeSick( int pos );
  void SetWindowInfo_PokeExType( int pos );
  void SetWindowInfo_PokeContinue( int pos );
  void SetWindowInfo_Turn( int pos );
  void SetWindowInfo_PokeType( int pos );


#ifdef KIJIMA_LOCAL_TEST
  void SetWindowInfo_Test( int pos );   // For test
#endif

 private:


  // -------------------------
  // Members
  // -------------------------
  BattleViewSystem            *mBtlvCore;
  ::app::util::G2DUtil        *mG2dUtil;
  app::BtlvUiMsgWin           *mUiMsgWin;

  int                         mLytIndex;
  gfl2::lyt::LytWk            *mGaugeLyt;
  gfl2::lyt::LytParts         *mLytParts[WINDOW_PANE_MAX];
  s32                         mAnmTbl[ANM_MAX];

  int                         mWaitFrameCount;      // 待ちフレームカウント
  DISP_STATUS                 mDispStatus;          // 表示状態
  int                         mCurrentDataType;     // 現在表示しているデータタイプ
  int                         mCurrentPosition;     // 現在表示しているウィンドウ位置
  int                         mPokemonNum;          // 対戦するポケモン数
  int                         mPlayerNum;           // 対戦するプレイヤー数
  int                         mSubSeq;              // サブシーケンス
  int                         mWindowMax;           // 使用するウインドウの最大数
  int                         mDataTypeStart;       // 表示開始データータイプ

  int                         mDispInfoCount;                       // 表示する情報数
  EffectDispInfo              mDispInfoList[DISP_INFO_LIST_MAX];    // 表示する情報リスト


  // 国名用
  //  gfl2::str::STRCODE m_countryName[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH]; //!< 国名

};      // End of class


GFL_NAMESPACE_END(btl)

#endif // __BTLV_CONVENTION_WIN_H__
