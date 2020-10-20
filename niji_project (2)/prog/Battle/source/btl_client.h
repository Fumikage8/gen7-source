#if !defined( __BTL_CLIENT_H__ )
#define __BTL_CLIENT_H__
//=============================================================================================
/**
 * @file  btl_client.h
 * @brief ポケモンXY バトルシステム クライアントモジュール
 * @author  taya
 *
 * @date  2010.12.28  作成
 */
//=============================================================================================
#pragma once
#include <util/include/gfl2_std_string.h>

#include  "./btl_common.h"
#include  "./btl_pokecon.h"
#include  "./btl_adapter.h"
#include  "./btl_field.h"
#include  "./btl_EscapeInfo.h"
#include  "./btl_rec.h"
#include  "./btl_str.h"
#include  "./btl_server_const.h"
#include  "./btl_pokeselect.h"
#include  "./btl_sound.h"
#include  "./btl_PosEff.h"
#include  "./btl_TrainerMessage.h"
#include  "./btl_RoyalRankingContainer.h"
#include  "./btl_SendData_ActionSelect.h"
#include  "./btl_SendData_ClientLimitTime.h"
#include  "./tr_ai/btl_BattleAi.h"

#include  "./btlv/btlv_core.h"


GFL_NAMESPACE_BEGIN(btl)

#define WAZAEFF_SYNC_DAMAGE_EFF  (1)



class Adapter;
class MainModule;
class BTL_SERVER;
class SendDataContainer;
class TrainerMessageManager;
class ClientSeq_TrainerMessage;
class ClientSeq_WinWild;
class ClientSeq_Capture;
class ClientSeq_BattleRoyalShowdown;
class BattleAi;
class ZenryokuWazaStatus;

class BTL_CLIENT {
  GFL_FORBID_COPY_AND_ASSIGN( BTL_CLIENT );

public:
  BTL_CLIENT(
      MainModule*         mainModule,
      ServerFlow*         serverFlow,
      BtlSound*           soundSys,
      POKECON*            pokeCon,
      Adapter*            adapter,
      SendDataContainer*  sendDataContainer,
      SendDataContainer*  receiveDataContainer,
      ZenryokuWazaStatus* zenryokuWazaStatus,
      BtlCommMode         commMode,
      u16                 clientID,
      BtlClientType       clientType,
      BtlBagMode          bagMode,
      bool                fRecPlayMode,
      const u32           randSeed,
      HeapHandle          heapHandle
   );

  ~BTL_CLIENT();

  void        AttachViewCore( BattleViewSystem* viewCore );
  void        AttachAiExecHeap( HeapHandle heap );
  void        SetRecordPlayerMode( rec::Reader* recReader );
  void        AttachCmdCheckServer( BTL_SERVER* server );
  void        DetachCmdCheckServer( void );
  Adapter*    GetAdapter( void );
  bool        Main( void );

  void         GetEscapeInfo( EscapeInfo* dst ) const;
  const void*  GetRecordData( u32* size ) const;
  bool         IsGameTimeOver( void ) const;
  bool         IsRecPlayBufOver( void ) const;
  bool         IsWaitingAdapterCommand( void ) const;

  bool          IsRecPlayerMode( void ) const;
  u32           GetRecPlayerMaxChapter( void ) const;
  bool          IsChapterSkipMode( void ) const;

  void          NotifyFadeoutStartForRecPlay( void );
  void          SetChapterSkip( u32 nextTurnNum );
  void          StopChapterSkip( void );
  void          NotifyCommError( void );

  u8                GetClientID( void ) const;
  const BTL_PARTY*  GetParty( void ) const;
  BtlWeather        GetWeather( void ) const;
  BtlPokePos        GetProcPokePos( void ) const;
  u32               GetTurnCount( void ) const;

  const ZenryokuWazaStatus* GetZenryokuWazaStatus( void ) const;
  const RoyalRankingContainer& GetRoyalRankingContainer( void ) const;

  bool IsPuttablePokemonExist( void ) const;
  void GetBonusMoneyGettingStr( BTLV_STRPARAM* strParam, u32 bonus ) const;

  /**
   * 道具やワザの効果により、選択できないワザになっているか判定
   * @param bpp
   * @param waza
   * @return 選択できないワザであれば true
   */
  bool   IsUnselectableWaza( const BTL_POKEPARAM* bpp, WazaNo  waza ) const;


  enum PrintCallbackArg {
    PRINTCB_RUN,        ///< 表示処理中（文字が出力され続けている状態）
    PRINTCB_JUST_DONE,  ///< 表示終了フレーム（最後の１文字が表示されたタイミング）
    PRINTCB_AFTER_DONE, ///< 表示終了フレームの後
  };
  bool PrintCallback( PrintCallbackArg arg );

  #if PM_DEBUG
  static void  DebugAICtrl_Init( void );      ///< AIコントロール初期化
  static bool  DebugAICtrl_IsUsing( void );   ///< AIクライアントがUI操作中なら true を返します
  #endif



  /**
   * @brief 逃げ・交換禁止コード
   */
  typedef enum {

    CANTESC_START = 0,

    CANTESC_KAGEFUMI = CANTESC_START,
    CANTESC_ARIJIGOKU,
    CANTESC_JIRYOKU,
    CANTESC_TOOSENBOU,
    CANTESC_FAIRY_LOCK,

    CANTESC_MAX,
    CANTESC_NULL = CANTESC_MAX,

  }CantEscapeCode;

  //---------------------------------------------------------------------------
  /**
   * @brief 交換禁止チェック
   *
   * @param      procPoke   逃げ・交換が可能かどうかのチェック対象ポケモン
   * @param[out] pokeID     交換できない原因となっているポケモンのID
   * @param[out] tokuseiID  交換できない原因が相手とくせいの場合、そのとくせいID（それ以外TOKUSEI_NULL）
   *
   * @return 逃げ・交換が出来ない原因
   * @retval CANTESC_NULL  逃げ・交換が可能な場合
   */
  //---------------------------------------------------------------------------
  CantEscapeCode isForbidPokeChange( const BTL_POKEPARAM* procPoke, u8* pokeID, u16* tokuseiID ) const;


  //--------------------------------------------------------------------------
  /**
   * @brief 死亡時の選択可能ポケモン( 場に出ているポケモン以外で、生きているポケモン )をリストアップする
   *
   * @param[out] list 選択可能ポケモンのパーティ内インデックスを格納する配列( メガシンカ可能なポケモンは末尾になる )
   *
   * @return 選択可能ポケモンの数
   */
  //--------------------------------------------------------------------------
  u8 countPuttablePokemons( u8* list ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief AI 入れ替え判定用ワーク：すでに入れ替えることが決定しているメンバーかどうか判定
   * @param pokeIndexWithinParty  チェック対象ポケモンのパーティ内インデックス
   * @return  すでに入れ替え決定しているなら true
   */
  //---------------------------------------------------------------------------
  bool changeAI_CheckReserve( u8 index ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief ＡＩを、乱入ＡＩに変更する
   */
  //---------------------------------------------------------------------------
  void SetIntrudeAI( void );

  //---------------------------------------------------------------------------
  /**
   * @brief ＡＩを、援軍ＡＩに変更する
   */
  //---------------------------------------------------------------------------
  void SetReinforceAI( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 進化可能なメンバがいるか？
   */
  //---------------------------------------------------------------------------
  bool isEvolveEnablePokeExsist( void ) const;

private:

  /**
   *  メインループ関数型定義
   */
  typedef bool (BTL_CLIENT::*ClientMainProc)(void);

  /**
   *  サブループ関数型定義
   */
  typedef bool (BTL_CLIENT::*ClientSubProc)( int* );

  /**
   *  サーバーコマンド処理関数型定義
   */
  typedef bool (BTL_CLIENT::*ServerCmdProc)( int*, const int* );

  /**
   *  録画データコントロールコード
   */
  typedef enum {
    RECCTRL_NONE = 0,   ///< 操作無し
    RECCTRL_QUIT,       ///< 再生終了
    RECCTRL_CHAPTER,    ///< チャプター移動
  }RecCtrlCode;

  /**
   *  AIトレーナーのメッセージ種類
   */
  enum {
    AITRAINER_MSG_HP_HALF,        ///< HP半分以下
    AITRAINER_MSG_FIRST_DAMAGE,   ///< 最初のダメージ後
    AITRAINER_MSG_LAST,           ///< 最後の１体登場直後
    AITRAINER_MSG_LAST_HP_HALF,   ///< 最後の１体ＨＰ半分以下
    AITRAINER_MSG_MAX,
  };


  /**
   *  録画データ再生コントロール構造体
   */
  typedef struct {
    u8   seq;
    u8   ctrlCode;
    u8   fChapterSkip    : 1;
    u8   fFadeOutStart   : 1;
    u8   fFadeOutDone    : 1;
    u8   fTurnIncrement  : 1;
    u8   fLock           : 1;
    u8   fQuit           : 1;
    u16  handlingTimer;
    u16  turnCount;
    u16  nextTurnCount;
    u16  maxTurnCount;
    u16  skipTurnCount;
  }RECPLAYER_CONTROL;

  struct SEL_ITEM_WORK{
    u16  itemNo;
    u8   bFromPokeSelect;
  };


  /**
   *  ワザエフェクト処理コード
   */
  enum WazaEffectCmdProcResult {
    WAZAEFF_CMD_RESULT_NO_PROC,       ///< コマンド処理しない（エフェクト表示必要なし）
    WAZAEFF_CMD_RESULT_PROC_NO_SYNC,  ///< コマンド処理を単体で行う
    WAZAEFF_CMD_RESULT_PROC_SYNC,     ///< コマンド処理と、後に続く関連コマンド処理を同期させて行う
  };

  /**
   * 可変個引数の格納クラス
   */
  class VariableArgs {
  public:
    VariableArgs();
    ~VariableArgs(){};

    void Clear( void );
    void Setup( const int* cmd_args );
    u8   GetCount( void ) const;
    int  GetArg( u8 idx ) const;

  private:
    int  m_args[ BTL_SERVERCMD_ARG_MAX ];
    u8   m_cnt;
  };


  MainModule*                  m_mainModule;
  HeapHandle                   m_heapHandle;
  HeapHandle                   m_heapForAiScriptExec;
  POKECON*                     m_pokeCon;
  const BTL_POKEPARAM*         m_procPoke;
  BTL_ACTION_PARAM*            m_procAction;
  u8*                          m_currentActionCount;
  rec::Data*                   m_recData;
  rec::Reader*                 m_btlRecReader;
  RECPLAYER_CONTROL            m_recPlayer;
  ClientMainProc               m_mainProc;

  EscapeInfo                   m_escapeInfo;
  RoyalRankingContainer        m_royalRankingContainer;
  FieldStatus*                 m_fldSim;
  ZenryokuWazaStatus*          m_zenryokuWazaStatus;

  RandContext                  m_randContext;
  Adapter*                     m_adapter;
  SendDataContainer*           m_sendDataContainer;
  SendDataContainer*           m_receiveDataContainer;
  RandSys                      m_random;
  AIRandSys                    m_AIRand;


  TrainerMessageManager*         m_trainerMessageManager;
  ClientSeq_TrainerMessage*      m_seq_TrainerMessage;
  ClientSeq_WinWild*             m_seq_WinWild;
  ClientSeq_Capture*             m_seq_Capture;
  ClientSeq_BattleRoyalShowdown* m_seq_BattleRoyalShowdown;

  u32  m_turnCount;
  u16  m_EnemyPokeHPBase;
  SEL_ITEM_WORK  m_selItemWork[ BTL_POSIDX_MAX ];
  u8   m_myID;
  u8   m_myType;
  u8   m_myState;
  u8   m_commWaitInfoOn;
  u8   m_bagMode;
  u8   m_change_escape_code;
  u8   m_fForceQuitSelAct;
  u8   m_cmdCheckTimingCode;
  u8   m_actionCountWork[ BTL_POSIDX_MAX ];
  u8   m_wazaInfoPokeIdx;
  u8   m_wazaInfoWazaIdx;
//  u8   recPlayTimerOverState;

  u8   m_fAITrainerBGMChanged : 1;
  u8   m_fCommError           : 1;
  u8   m_fSelActForceFinish   : 1;
  u8   m_fCmdCheckEnable      : 1;
  u8   m_fRecPlayEndTimeOver  : 1;
  u8   m_fRecPlayEndBufOver   : 1;
  u8   m_bRecPlayFadeStarted  : 1;

  bool m_isWaitingAdapterCommand;
  bool m_bSetMegaEvoThisTurn;
  bool m_bSetUltraBurstThisTurn;

  u8          m_myChangePokeCnt;
  u8          m_myPuttablePokeCnt;
  BtlPokePos  m_myChangePokePos[ BTL_POSIDX_MAX ];

  SendDataSerialNumber m_returnDataSerialNumber;
  ServerSequence m_returnDataServerSeq;
  AdapterCommand m_returnDataCommandID;
  const void*    m_returnDataPtr;
  u32            m_returnDataSize;
  u32            m_dummyReturnData;
  SEND_DATA_ACTION_SELECT m_returnData_ActionSelect;
  SEND_DATA_CLIENT_LIMIT_TIME m_returnData_ClientLimitTime;

  u16            m_cmdLimitTime;
  u16            m_gameLimitTime;
  u16            m_clientLimitTime;

  const BTL_PARTY*  m_myParty;
  u8                m_procPokeIdx;    ///< 処理中ポケモンインデックス
  s8                m_prevPokeIdx;    ///< 前回の処理ポケモンインデックス
  u8                m_firstPokeIdx;   ///< アクション指示できる最初のポケモンインデックス
  u8                m_fStdMsgChanged; ///< 標準メッセージ内容を書き換えたフラグ
  u8                m_b1stReadyMsgDisped; ///< 戦闘開始後、最初の「どうする？」メッセージを出力した

  BTL_SERVER*          m_cmdCheckServer;
  BattleViewSystem*    m_viewCore;
  BtlSound*            m_soundSys;

  BTL_ACTION_PARAM     m_actionParam[ BTL_POSIDX_MAX * 2 ];  // メガ進化を別アクションとして保持するため、最大ポケ数*2が必要
  SCQUE*               m_cmdQue;
  int                  m_cmdArgs[ BTL_SERVERCMD_ARG_MAX ];
  int                  m_strArgs[ str::MSG_ARG_MAX ];
  VariableArgs         m_stdVariableArgs;
  const VariableArgs*  m_tmpVariableArgs;

  BattleAi*             m_ai;
  bool                  m_aiZenryokuSelected;
  u16                   m_AIItem[ BSP_TRAINER_DATA::USE_ITEM_MAX ];


  s8             m_AIChangeIndex[ BTL_PARTY_MEMBER_MAX ];
  u8             m_AITrainerMsgCheckedFlag[ AITRAINER_MSG_MAX ];

  ClientSubProc   m_subProc;
  int             m_subSeq;

  ClientSubProc   m_selActProc;
  int             m_selActSeq;

  BTLV_STRPARAM                m_strParam;
  BTLV_STRPARAM                m_strParamSub;

  PokeSelParam        m_pokeSelParam;
  PokeSelResult       m_pokeSelResult;

  ServerCmdProc        m_scProc;
  ServerCmd            m_serverCmd;
  int                  m_scSeq;

  BtlPokePos           m_deadPokePos[ BTL_POSIDX_MAX ];





  static u8 s_bMegaEvolved[ BTL_CLIENT_MAX ];
  static void s_ClearMegaEvoFlag( u8 clientID );
  static bool s_IsMegaEvolved( u8 clientID );
  static void s_SetMegaEvolved( u8 clientID );

  static u8 s_bUltraBursted[BTL_CLIENT_MAX];
  static void s_ClearUltraBurstFlag(const u8 clientID);
  static bool s_IsUltraBursted(const u8 clientID);
  static void s_SetUltraBursted(const u8 clientID);


  #if PM_DEBUG
  static BattleViewSystem*   s_ViewCore;
  static int                 s_ControlableAIClientID;
  static int                 s_YubiCtrlPos;
  const BTL_CLIENT*          m_viewOldClient;
  int                        m_yubifuruDebugRetSeq;
  bool                       m_isLiveRecSeedSetup;
  //@fix [fix]GFNMCat[5735]:[#5735 ライブ大会：観戦機能で全員のHPゲージと残りポケモン数が最初しか表示されない]
  u32                        m_liveRecWaitCameraSeq;
  u32                        m_liveRecSizeSave;

  bool  debugAICtrl_Delegate( void );
  void  debugAICtrl_RestoreViewClient( void );
  bool  debugAICtrl_IsMyFase( void );
  bool  selact_YubiFuruDebug( int* seq );

  void   printQue( const char* caption, SCQUE* que );
  #endif

  //------------------------------------------------------------------
  // ワザエフェクトとダメージ演出各種を同期させるためのコマンド保存の仕組み
  //------------------------------------------------------------------
  struct WAZAEFF_SYNCDAMAGE_CMD_WORK {
    u32  cmdCount;
    struct
    {
      ServerCmdProc cmdProc;
      ServerCmd     cmd;
      int           cmdArgs[ BTL_SERVERCMD_ARG_MAX ];
      int           seq;
      bool          bDone;
      VariableArgs  variableArgs;
    }store[ 16 ]; // これだけあれば充分
  };
  WAZAEFF_SYNCDAMAGE_CMD_WORK  m_wazaEffDmgSyncWork;
  bool      m_bWazaEffectDone;
  bool      m_bSyncEffectDone;

  void storeWazaEffectSyncDamageParams( WAZAEFF_SYNCDAMAGE_CMD_WORK* work );
  bool putWazaEffSyncDamage( WAZAEFF_SYNCDAMAGE_CMD_WORK* work );

  //------------------------------------------------------------------

  void   changeMainProc( ClientMainProc proc );
  bool   main_Normal( void );
  bool   main_ChapterSkip( void );

  /**
   *  行動選択ルートプロセスのシーケンス
   */
  enum SelActRootSeq {
    SELACT_ROOTSEQ_START = 0,
    SELACT_ROOTSEQ_FRIENDSHIP_MSG_WAIT,
    SELACT_ROOTSEQ_WAIT_MSG_CHECK,
    SELACT_ROOTSEQ_WAIT_MSG_WAIT,
    SELACT_ROOTSEQ_SEL_START,
    SELACT_ROOTSEQ_SEL_MAIN,
    SELACT_ROOTSEQ_WAIT_UI_RESTART,
    SELACT_ROOTSEQ_FREEFALL_WARN,
    SELACT_ROOTSEQ_FREEFALL_WARN_MSG,
    SELACT_ROOTSEQ_FREEFALL_WARN_WAIT,
  };
  u8     calcAddActionCountSum( u8 pokeIdx );
  void   setupCurrentPokeActionPtr( void );
  void   incrementAddActionCount( void );
  void   decrementAddActionCount( void );
  bool   selact_Start( int* seq );
  bool   selact_Root( int* seq );
  bool   selact_root_start( int* seq );
  bool   selact_root_friendship_msg_check( int* seq );
  bool   selact_root_friendship_msg_wait( int* seq );
  bool   selact_root_wait_msg_check( int* seq );
  bool   selact_root_sel_main( int* seq );

  bool   selact_ForceQuit( int* seq );
  bool   selact_Fight( int* seq );
  bool   selact_Item( int* seq );
  bool   selact_Escape( int* seq );
  bool   selact_SelectChangePokemon( int* seq );
  bool   selact_WazaInfoView( int* seq );
  bool   selact_CheckFinish( int* seq );
  bool   selact_Finish( int* seq );
  void   selact_ClearWorks( void );

  void registerReceivedData( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand commandId, const void* data, u32 dataSize );
  bool returnToServer( void ) const;
  ServerCmdProc dispatchServerCmdProc( ServerCmd cmd );

  bool   setSubProc( AdapterCommand cmd, bool* fRecCtrlLock );
  bool   callSubProc( void );
  bool   SubProc_UI_Setup( int* seq );
  bool   SubProc_AI_Setup( int* seq );
  bool   SubProc_REC_Setup( int* seq );
  bool   SubProc_UI_SelectAction( int* seq );
  bool   SubProc_AI_SelectAction( int* seq );
  bool   SubProc_REC_SelectAction( int* seq );
  bool   SubProc_UI_SelectChangeOrEscape( int* seq );
  bool   SubProc_UI_SelectPokemonForCover( int* seq );
  bool   SubProc_UI_SelectPokemonForChange( int* seq );
  bool   SubProc_AI_SelectPokemon( int* seq );
  bool   SubProc_REC_SelectPokemon( int* seq );
  bool   SubProc_UI_ConfirmIrekae( int* seq );
  bool   SubProc_UI_ServerCmd( int* seq );
  bool   SubProc_REC_ServerCmd( int* seq );
  bool   SubProc_UI_RecordData( int* seq );
  bool   SubProc_UI_ExitForNPC( int* seq );
  bool   SubProc_REC_ExitForNPC( int* seq );
  bool   SubProc_UI_ExitForSubwayTrainer( int* seq );
  bool   SubProc_REC_ExitForSubwayTrainer( int* seq );
  bool   SubProc_ExitForSubwayTrainer( int* seq, bool isRecPlayMode );
  bool   SubProc_UI_WinWild( int* seq );
  bool   SubProc_UI_LoseWild( int* seq );
  bool   SubProc_UI_CaptureWild( int* seq );
  bool   SubProc_UI_ExitCommTrainer( int* seq );
  bool   SubProc_REC_ExitCommTrainer( int* seq );
  bool   SubProc_ExitCommTrainer( int* seq, bool isRecPlayMode );
  bool   SubProc_UI_NotifyTimeUp( int* seq );
  bool   SubProc_UI_FadeOut( int* seq );
  bool   SubProc_REC_FadeOut( int* seq );
  bool   SubProc_UI_SendLastDataAgain( int* seq );
  bool   SubProc_UI_SendClientLimitTime( int* seq );
  bool   SubProc_UI_SyncClientLimitTime( int* seq );


  bool   scProc_MSG_Std( int* seq, const int* args );
  bool   scProc_MSG_StdSE( int* seq, const int* args );
  bool   scProc_OP_OutClear(  int* seq, const int* args );
  bool   scProc_OP_AddFldEff(  int* seq, const int* args );
  bool   scProc_OP_AddFldEffDepend(  int* seq, const int* args );
  bool   scProc_OP_ChangeGround( int* seq, const int* args );
  bool   scProc_OP_DelFldEffDepend(  int* seq, const int* args );
  bool   scProc_OP_RemoveFldEff(  int* seq, const int* args );
  bool   scProc_OP_SetPokeCounter(  int* seq, const int* args );
  bool   scProc_OP_IncKillCount( int*, const int* args );
  bool   scProc_OP_BatonTouch(  int* seq, const int* args );
  bool   scProc_OP_MigawariCreate(  int* seq, const int* args );
  bool   scProc_OP_MigawariDelete(  int* seq, const int* args );
  bool   scProc_OP_SetFakeSrc(  int* seq, const int* args );
  bool   scProc_OP_ClearConsumedItem(  int* seq, const int* args );
  bool   scProc_OP_CureSickDependPoke(  int* seq, const int* args );
  bool   scProc_OP_AddWazaDamage(  int* seq, const int* args );
  bool   scProc_OP_TurnCheck(  int* seq, const int* args );
  bool   scProc_OP_TurnCheckField(  int* seq, const int* args );
  bool   scProc_OP_SetDoryoku( int* , const int* args );
  bool   scProc_OP_StartPosEff( int* , const int* args );
  bool   scProc_OP_RemovePosEff( int* , const int* args );
  bool   scProc_OP_UpdatePosEffectParam( int* , const int* args );
  bool   scProc_OP_PGLRecord( int* , const int* args );
  bool   scProc_OP_SetStatus( int* seq, const int* args );
  bool   scProc_OP_SetWeight( int* seq, const int* args );
  bool   scProc_OP_RankRecover(  int* seq, const int* args );
  bool   scProc_OP_RankReset(  int* seq, const int* args );
  bool   scProc_OP_RankUpReset(  int* seq, const int* args );
  bool   scProc_OP_AddCritical(  int* seq, const int* args );
  bool   scProc_OP_SickSet(  int* seq, const int* args );
  bool   scProc_OP_CurePokeSick(  int* seq, const int* args );
  bool   scProc_OP_CureWazaSick(  int* seq, const int* args );
  bool   scProc_OP_MemberIn(  int* seq, const int* args );
  bool   scProc_OP_ChangePokeType(  int* seq, const int* args );
  bool   scProc_OP_ExPokeType( int* , const int* args );
  bool   scProc_OP_WSTurnCheck(  int* seq, const int* args );
  bool   scProc_OP_ConsumeItem(  int* seq, const int* args );
  bool   scProc_OP_UpdateUseWaza(  int* seq, const int* args );
  bool   scProc_OP_SetContFlag(  int* seq, const int* args );
  bool   scProc_OP_ResetContFlag(  int* seq, const int* args );
  bool   scProc_OP_SetTurnFlag(  int* seq, const int* args );
  bool   scProc_OP_ResetTurnFlag(  int* seq, const int* args );
  bool   scProc_OP_SetPermFlag( int*, const int* args );
  bool   scProc_OP_IncBattleTurnCount( int*, const int* args );
  bool   scProc_OP_IncPokeTurnCount( int*, const int* args );
  bool   scProc_OP_ChangeTokusei(  int* seq, const int* args );
  bool   scProc_OP_SetItem(  int* seq, const int* args );
  bool   scProc_OP_UpdateWazaNumber(  int* seq, const int* args );
  bool   scProc_OP_HpMinus(  int* seq, const int* args );
  bool   scProc_OP_HpPlus(  int* seq, const int* args );
  bool   scProc_OP_PPMinus(  int* seq, const int* args );
  bool   scProc_OP_PPMinus_Org(  int* seq, const int* args );
  bool   scProc_OP_WazaUsed(  int* seq, const int* args );
  bool   scProc_OP_HpZero(  int* seq, const int* args );
  bool   scProc_OP_PPPlus(  int* seq, const int* args );
  bool   scProc_OP_PPPlus_Org(  int* seq, const int* args );
  bool   scProc_OP_RankUp(  int* seq, const int* args );
  bool   scProc_OP_RankDown(  int* seq, const int* args );
  bool   scProc_OP_RankSet8(  int* seq, const int* args );
  bool   scProc_OP_SideEffect_Add( int* , const int* args );
  bool   scProc_OP_SideEffect_Remove( int* , const int* args );
  bool   scProc_OP_SideEffect_IncTurnCount( int* , const int* args );
  bool   scProc_OP_PublishClientInformation_AppaearPokemon( int* , const int* args );
  bool   scProc_OP_PublishClientInformation_HavePokemonItem( int* , const int* args );
  bool   scProc_OP_SetCurrentRoyalRanking( int* , const int* args );
  bool   scProc_OP_SetZenryokuWazaUsed( int* , const int* args );
  bool   scProc_OP_SetDirt( int* , const int* args );
  bool   scProc_OP_IncWeatherPassedTurn( int* , const int* args );
  bool   scProc_OP_SetSpActPriority( int* , const int* args );
  bool   scProc_ACT_BallThrow(  int* seq, const int* args );
  bool   scProc_ACT_BallThrowCaptured(  int* seq, const int* args );
  bool   scProc_ACT_BallThrowForbidden(  int* seq, const int* args );
  bool   scProc_ACTOP_ChangeTokusei(  int* seq, const int* args );
  bool   scProc_ACTOP_SwapTokusei(  int* seq, const int* args );
  bool   scProc_ACT_FakeDisable(  int* seq, const int* args );
  bool   scProc_ACT_EffectSimple(  int* seq, const int* args );
  bool   scProc_ACT_EffectByPos(  int* seq, const int* args );
  bool   scProc_ACT_EffectByVector( int* seq, const int* args );
  bool   scProc_ACT_EffectField( int* seq, const int* args );
  bool   scProc_ACT_PluralEx2ndHit( int* seq, const int* args );
  bool   scProc_ACT_ChangeForm(  int* seq, const int* args );
  bool   scProc_TOKWIN_In(  int* seq, const int* args );
  bool   scProc_TOKWIN_Out(  int* seq, const int* args );
  bool   scProc_ACT_WazaEffect(  int* seq, const int* args );
  bool   scProc_ACT_TameWazaHide(  int* seq, const int* args );
  bool   scProc_ACT_WazaDmg(  int* seq, const int* args );
  bool   scProc_ACT_WazaDmg_Plural(  int* seq, const int* args );
  bool   scProc_ACT_WazaIchigeki(  int* seq, const int* args );
  bool   scProc_ACT_SickIcon(  int* seq, const int* args );
  bool   scProc_ACT_ConfDamage(  int* seq, const int* args );
  bool   scProc_ACT_Dead(  int* seq, const int* args );
  bool   scProc_ACT_Relive(  int* seq, const int* args );
  bool   scProc_ACT_RankDown(  int* seq, const int* args );
  bool   scProc_ACT_RankUp(  int* seq, const int* args );
  bool   scProc_ACT_WeatherDmg(  int* seq, const int* args );
  bool   scProc_ACTOP_WeatherStart(  int* seq, const int* args );
  bool   scProc_ACTOP_WeatherEnd(  int* seq, const int* args );
  bool   scProc_ACTOP_MegaEvo(  int* seq, const int* args );
  bool   scProc_ACT_SimpleHP(  int* seq, const int* args );
  bool   scProc_ACT_UseItem(  int* seq, const int* args );
  bool   scProc_ACT_KinomiPrevWaza(  int* seq, const int* args );
  bool   scProc_ACT_Kill(  int* seq, const int* args );
  bool   scProc_ACTOP_Move(  int* seq, const int* args );
  bool   scProc_ACT_MigawariCreate(  int* seq, const int* args );
  bool   scProc_ACT_MigawariDelete(  int* seq, const int* args );
  bool   scProc_ACT_Hensin(  int* seq, const int* args );
  bool   scProc_ACT_MigawariDamage(  int* seq, const int* args );
  bool   scProc_ACT_PlayWinBGM(  int* seq, const int* args );
  bool   scProc_ACT_MsgWinHide(  int* seq, const int* args );
  bool   scProc_ACT_FriendshipEffect( int* seq, const int* args );
  bool   scProc_ACT_FriendshipEffectMsg( int* seq, const int* args );
  bool   scProc_ACT_DeadPlural( int* seq, const int* args );
  bool   scProc_ACT_Exp(  int* seq, const int* args );
  bool   scProc_ACT_Naderu(  int* seq, const int* args );
  bool   scProc_ACT_MemberOutMsg( int* seq, const int* args );
  bool   scProc_ACT_MemberOut(  int* seq, const int* args );
  bool   scProc_ACT_MemberIn(  int* seq, const int* args );
  bool   scProc_ACT_ZenryokuWazaBegin( int* seq, const int* args );
  bool   scProc_ACT_ZenryokuWazaEnd( int* seq, const int* args );
  bool   scProc_ACT_OteireButtonDisplay( int* seq, const int* args );
  bool   scProc_ACT_IntrudeCall( int* seq, const int* args );
  bool   scProc_ACT_IntrudeAppear( int* seq, const int* args );
  bool   scProc_ACT_IntrudeNotAppeared( int* seq, const int* args );
  bool   scProc_ACT_TurnStart( int* seq, const int* args );
  bool   scProc_MSG_Set(  int* seq, const int* args );
  bool   scProc_MSG_SetSE(  int* seq, const int* args );
  bool   scProc_MSG_Waza(  int* seq, const int* args );
  bool   scProc_ExArg(  int* seq, const int* args );


  void   sortChangePos( BtlPokePos* posAry, u32 posCnt );
  void   selActSubProc_Set( ClientSubProc proc );
  bool   selActSubProc_Call( void );
  bool   scproc_msgStdCore( int* seq, u16 strID, const int* args );
  bool   scproc_msgSetCore( int* seq, u16 strID, const int* args );
  WazaEffectCmdProcResult  scproc_wazaEffProc_Start( const int* args );
  bool   wazaEff_IsOmitFriendAttackEffect( WazaNo waza, WazaTarget wazaRange, BtlPokePos atPokePos, BtlPokePos defPokePos );


  void          RecPlayer_Init( RECPLAYER_CONTROL* ctrl );
  void          RecPlayer_Setup( RECPLAYER_CONTROL* ctrl, u32 turnCnt );
  bool          RecPlayer_CheckBlackOut( const RECPLAYER_CONTROL* ctrl );
  void          RecPlayer_TurnIncReq( RECPLAYER_CONTROL* ctrl );
  RecCtrlCode   RecPlayer_GetCtrlCode( const RECPLAYER_CONTROL* ctrl );
  void          RecPlayer_ChapterSkipOn( RECPLAYER_CONTROL* ctrl, u32 nextTurnNum );
  void          RecPlayer_ChapterSkipOff( RECPLAYER_CONTROL* ctrl );
  bool          RecPlayer_CheckChapterSkipEnd( const RECPLAYER_CONTROL* ctrl );
  u32           RecPlayer_GetNextTurn( const RECPLAYER_CONTROL* ctrl );
  bool          RecPlayerCtrl_Lock( RECPLAYER_CONTROL* ctrl );
  void          RecPlayer_Quit( RECPLAYER_CONTROL* ctrl );
  void          RecPlayerCtrl_Main( RECPLAYER_CONTROL* ctrl );
  void          RecPlayerCtrl_Unlock( RECPLAYER_CONTROL* ctrl );
  bool          RecPlayer_IsActive( const RECPLAYER_CONTROL* ctrl ) const;

  void          AIItem_Setup( void );
  u16           AIItem_CheckUse( const BTL_POKEPARAM* bpp, const BTL_PARTY* party );

  bool          AI_CheckZWazaEnable( void ) const;

  void          changeAI_InitWork( void );
  void          changeAI_SetReserve( u8 outPokeIdx, u8 inPokeIdx );
  BTL_POKEPARAM*  changeAI_SupposeEnemy( BtlPokePos basePos );
  bool           checkForbitEscapeEffective_Kagefumi( const BTL_POKEPARAM* procPoke ) const;
  bool           checkForbitEscapeEffective_Arijigoku( const BTL_POKEPARAM* procPoke ) const;
  bool           checkForbitEscapeEffective_Jiryoku( const BTL_POKEPARAM* procPoke ) const;
  bool          changeAI_IsOmitMegaEvolvable(const BTL_POKEPARAM* bpp);
  // momiji追加
  bool          changeAI_IsSpecialMons(const BTL_POKEPARAM* bpp);


  BTL_ACTION_PARAM* buildActionParamFromAiResult( BTL_ACTION_PARAM* actionParam, bool* isMegaEvoThisTurn, u8 actPokeIndex, const BTL_POKEPARAM* actPoke, const BTL_PARTY* actPokeParty, const BattleAi::Result& aiResult);
  bool checkAiMegaEvoEnable(const BTL_POKEPARAM& poke, bool isMegaEvoThisTurn) const;
  bool checkAiUltraBurstEnable(const BTL_POKEPARAM& poke) const;


  bool          check_status_up_item( u16 itemID, const BTL_POKEPARAM* bpp );
  bool          check_cure_sick_item( u16 itemID, const BTL_POKEPARAM* bpp );

  u8            getMyCoverPosNum( void ) const;
  void          setDummyReturnData( void );
  void          enemyPokeHPBase_Update( void );
  u32           enemyPokeHPBase_CheckRatio( void );
  const BTL_POKEPARAM* enemyPokeHPBase_GetTargetPoke( void );
  void          cmdLimit_Start( void );
  bool          cmdLimit_CheckOver( void );
  void          cmdLimit_End( void );
  bool          checkSelactForceQuit( ClientSubProc nextProc );
  void          setNullActionRecplay( void );
  bool          checkActionForceSet( const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
  bool          checkWazaForceSet( const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
  bool          is_unselectable_waza( const BTL_POKEPARAM* bpp, WazaNo waza, bool isZenryokuWaza, BTLV_STRPARAM* strParam ) const;
  bool          is_unselectable_target( const BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos, bool isZenryokuWaza, BTLV_STRPARAM* strParam ) const;
  void          setWaruagakiAction( BTL_ACTION_PARAM* dst, const BTL_POKEPARAM* bpp );
  u8            storeSelectableWazaFlag( const BTL_POKEPARAM* bpp, u8* dst ) const;
  CantEscapeCode checkForbidChangeEscapeCommon( const BTL_POKEPARAM* procPoke, u8* pokeID, u16* tokuseiID ) const;
  u8            countPuttablePokemons_sub( u8* list, u8 startCount, u8 numFrontPos, bool bSkipMegaEvolvable ) const;
  void          sortPuttablePokemonList(u8* list, u8 numPoke, const BTL_POKEPARAM* target , BtlPokePos checkPosEffect);
  bool          isExistHPRecoverEffect( BtlPokePos pos ) const;
  void          setupPokeSelParam( u8 numSelect, PokeSelParam* param, PokeSelResult* result );
  void          storePokeSelResult( const PokeSelResult* res );
  void          storePokeSelResult_ForceQuit( void );
  bool          selectPokemonUI_Core( int* seq, bool bForceChangeMode );
  u8            storeMyChangePokePos( BtlPokePos* myCoverPos );
  BtlResult     expandServerResult( ResultCause* resultCause ) const;
  bool          getVsCommTrainerExitMessage( BTLV_STRPARAM* strParam, BtlResult battleResult, bool isMulti ) const;
  void          msgWinningTrainerStart( void );
  bool          isEnemyClientDouble( void );
  void          trainerGraphicIn( int client_idx );
  void          setupSubwayTrainerMsg( BtlResult result, u8 client_idx );
  WazaTarget    checkWazaRange( WazaID waza, BtlPokePos atPokePos );
  u16           checkMemberOutStrID( u8 clientID, u8 pokeID, bool* fClientArg );
  u16           checkMemberPutStrID( const BTL_POKEPARAM* putPoke );
  u16           getMemberPutStrID( const BTL_POKEPARAM* putPoke, const BTL_POKEPARAM* opponentPoke );
  bool          wazaOboeSeq( int* seq, BTL_POKEPARAM* bpp );
  void          setupSelectStartStr( const BTL_POKEPARAM* procPoke, BTLV_STRPARAM* strParam );
  bool          checkFriendshipSpecialMessage( const BTL_POKEPARAM* procPoke, BTLV_STRPARAM* strParam );
  bool          isOteireExecuteEffectEnable( void ) const;
  void          getTimeUpMessage( u32* strID, BtlStrType* strType ) const;
  void          getTimeUpMessage_ClientLimitTime( u32* strID, BtlStrType* strType ) const;
  void          updateClientPublicInformation_FormNo( const BTL_POKEPARAM& poke );

  void  selItemWork_Init( void );
  void  selItemWork_Reserve( u8 pokeIdx, u16 itemID, bool bFromPokeSelect );
  bool  selItemWork_IsFromPokeSelect( u8 pokeIdx ) const;
  void  selItemWork_Restore( u8 pokeIdx );
  void  selItemWork_Quit( void );
  void  selact_startMsg( const BTLV_STRPARAM* strParam );
  bool  checkBallTargetHide( void );
  CantEscapeCode isForbidEscape( u8* pokeID, u16* tokuseiID );
  void   notifyTokuseiToAI( u8 pokeID );
  void   setBonusMoneyGettingStr( BTLV_STRPARAM* strParam, u32 bonus ) const;
  BTL_ACTION_PARAM*   insertActMegaEvoParam( BTL_ACTION_PARAM* ary, u8 pokeIdx );
  BTL_ACTION_PARAM*   deleteActMegaEvoParam( BTL_ACTION_PARAM* ary );

  const VariableArgs* PushTmpVariableArgsWork( const VariableArgs* newArgs );
  void                PopTmpVariableArgsWork( const VariableArgs* oldArgs );
  u8                  GetVariableArgsCount( void ) const;
  int                 GetVariableArgs( u8 idx ) const;

  u8  calcShortPokeID( u8 partyIdx ) const;

  void setupSelActReturnData( void );
  void storeActionSelectSendData( const BTL_ACTION_PARAM* actionParamArray, u32 actionNum );

  
  bool IsTrainerMessageEnable_OnLastPokeIn( u8 clientID ) const;
  bool DecideTrainerMessage_OnSelectAction( s8* clientID, TrainerMessage::MessageID* messageID );
  s8 DecideTrainerMessage_OnSelectAction_LastPokeHpHalf( void ) const;
  bool IsTrainerMessageEnable_OnSelectAction_LastPokeHpHalf( u8 clientID ) const;
  s8 DecideTrainerMessage_OnSelectAction_FirstAttackDone( void ) const;
  bool IsTrainerMessageEnable_OnSelectAction_FirstAttackDone( u8 clientID ) const;
  void StartTrainerMessage( u8 clientID, TrainerMessage::MessageID messageID );
  bool UpdateTrainerMessage( void );


  /**
   * かわいがり関連エフェクト（入場時）を並列処理させるための仕組み
   */
  class FriendshipEffectProc {
  public:
    FriendshipEffectProc(void)
    : m_mainModule(NULL)
    , m_pokeCon(NULL)
    , m_viewCore(NULL)
    , m_myID(BTL_CLIENTID_NULL)
    , m_searchIdx(0)
    , m_step(-1)
    {};
    ~FriendshipEffectProc(){};

    void Start( const MainModule* mainModule, const POKECON* pokeCon, BattleViewSystem* viewCore, u8 myID, u8 numCoverPos );
    bool Wait( void );

    /** エフェクト動作させて良い状態のポケモンデータかどうか判定 */
    static bool S_IsEnable( const BTL_POKEPARAM* bpp );

  private:
    const MainModule* m_mainModule;
    const POKECON*    m_pokeCon;
    BattleViewSystem* m_viewCore;
    u8                m_myID;
    u8                m_numCoverPos;
    u8                m_searchIdx;
    int               m_step;

    int checkEffectNo( const BTL_POKEPARAM* bpp ) const;
  };

  FriendshipEffectProc  m_frEffectProc;




};  // class BTL_CLIENT
GFL_NAMESPACE_END(btl)




#endif // __BTL_CLIENT_H__
