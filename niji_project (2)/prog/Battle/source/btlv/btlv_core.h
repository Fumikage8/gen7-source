#if !defined( __BTLV_CORE_H__ )
#define __BTLV_CORE_H__
//======================================================================
/**
 * @file	btlv_core.h
 * @brief	BattleVIewSystem
 * @author	ariizumi
 * @data	15/00/00
 */
//======================================================================

#pragma once

#include <gfx/include/gfl2_GFGL.h>
#include <ui/include/gfl2_UI.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <model/include/gfl2_DressUpParam.h>

#include <DemoLib/Object/include/ObjectManager.h>
#include <DemoLib/Object/include/TaskSystem.h>
#include <DemoLib/Object/include/ResourceManager.h>
#include <DemoLib/Object/include/Object_Cluster.h>

#include "GameSys/include/TimeZone.h"
#include <Savedata/include/ConfigSave.h>
#include <Battle/Background/include/BgSystem.h>
#include "sound/include/Sound.h"

#include "System/include/DeviceTurnWatcher.h"
#include "Battle/include/battle_SetupParam.h"


#include "../btl_common.h"
#include "../btl_action.h"
#include "../btl_calc.h"
#include "../btl_str.h"
#include "../btl_pokeparam.h"
#include "../btl_pokeselect.h"
#include "../btl_sound.h"
#include "../btl_SideEff.h"
#include "../btl_PosEff.h"

#include "btlv_object.h"
#include "btlv_object_poke.h"
#include "btlv_object_chara.h"
#include "btlv_effect_def.h"
#include "btlv_types.h"
#include "render/btlv_RenderSystem.h"

#include "niji_conv_header/battle/poke_extra_data_struct.h"
#include "niji_conv_header/battle/trainer_extra_data_struct.h"

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
    class CharaModelFactory;
    class DressUpModelResourceManager;
  GFL_NAMESPACE_END( mdoel )
GFL_NAMESPACE_END( poke_3d )

GFL_NAMESPACE_BEGIN( app )
  GFL_NAMESPACE_BEGIN( util )
    class Heap;
  GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( PokeTool )
  class PokeModelSystem;
GFL_NAMESPACE_END( PokeTool )
  
GFL_NAMESPACE_BEGIN( GameSys )
class GameProc;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN(btl)

class BTL_CLIENT;
class ZenryokuWazaStatus;
class SideEffectStatus;
class PosEffectStatus;
class ClientPublicInformation;

class BtlvUi;
class BtlvSeqSystem;
class BtlvCameraSystem;
class BtlvLoaderBase;

struct BTLV_DEBUG_PARAM;

typedef BattleObjectManager<BtlvObject_Sound> SoundObjectManager;
typedef BattleObjectManager<BtlvObject_Gfmdl> GfmdlObjectManager;
typedef BattleObjectManager<BtlvObject_Effect> EffectObjectManager;
typedef BattleObjectManager<BtlvObject_Cluster> ClusterObjectManager;

//移植タイプ定義↓
enum {
///HPゲージのドット数
  BTLV_GAUGE_HP_DOTTOMAX = 48,

  BTLV_INPUT_BR_SEL_REW = 0,  //巻き戻し
  BTLV_INPUT_BR_SEL_FF,       //早送り
  BTLV_INPUT_BR_SEL_STOP,     //停止
};
//----------------------------------------------------------------------
/**
 *  戻り値チェック
 */
//----------------------------------------------------------------------
typedef enum {

  BTLV_RESULT_NONE = 0,
  BTLV_RESULT_DONE,
  BTLV_RESULT_CANCEL,

}BtlvResult;

//動作モード
typedef enum
{
  BTLV_MODE_BATTLE,       //通常バトル
  BTLV_MODE_CAPTURE,      //捕獲チュートリアル
  BTLV_MODE_EFFECT_VIEWER,  //エフェクトビュワー
}BtlvMode;

//--------------------------------------------------------------
/**
 *  メッセージキー待ちウェイト指定
 */
//--------------------------------------------------------------
enum {

  BTLV_MSGWAIT_NONE= 0,      ///< キーを待たず、即時終了
  //BTLV_MSGWAIT_STD = 80,      ///< 標準ウェイトフレーム数（この分待ったら、キー押下なくても終了）
  //BTLV_MSGWAIT_QUICK = 0xff,  ///< 改行は標準ウェイトフレーム数だが、最後（EOM）はゼロになる

  BTLV_MSGWAIT_AUTO_HIDE = 24,///< メッセージ表示後フレーム数待ったら自動でメッセージを消す

};

//----------------------------------------------------------------------
/**
 *  文字出力パラメータ
 */
//----------------------------------------------------------------------
typedef struct {
  u16   strID;          ///< 基本文字列ID
  u8    wait;           ///< 改行・表示終了時ウェイト
  u8    strType : 4;    ///< BtlStrType
  u8    argCnt  : 4;    ///< 引数の数
  int   args[BTL_STR_ARG_MAX];  ///< 引数
}BTLV_STRPARAM;

extern void BTLV_STRPARAM_Setup( BTLV_STRPARAM* sp, BtlStrType strType, u16 strID );
extern void BTLV_STRPARAM_AddArg( BTLV_STRPARAM* sp, int arg );
extern void BTLV_STRPARAM_SetWait( BTLV_STRPARAM* sp, u8 wait );


//----------------------------------------------------------------------
/**
 *  ローテーション用ワザ選択時パラメータ
 */
//----------------------------------------------------------------------
typedef struct {

  const BTL_POKEPARAM* bpp;             ///< ポケモンパラメータ
  u8    fWazaUsable[ PTL_WAZA_MAX ];    ///< ワザ使えるフラグ（使える状態なら該当Index部分がTRUEである）

}BTLV_WAZASEL_POKEMON_PARAM;

typedef struct {

  BTLV_WAZASEL_POKEMON_PARAM    poke[ BTL_ROTATE_NUM ];
  BTL_ACTION_PARAM   actRotation;
  BTL_ACTION_PARAM   actWaza;

}BTLV_ROTATION_WAZASEL_PARAM;


typedef enum {
  BTLV_INPUT_BR_STOP_NONE = 0,
  BTLV_INPUT_BR_STOP_KEY,         //キーによる中断
  BTLV_INPUT_BR_STOP_BREAK,       //データ破壊による中断
  BTLV_INPUT_BR_STOP_SKIP,        //チャプタースキップによる中断
}BTLV_INPUT_BR_STOP_FLAG;


typedef enum
{
  BTLV_VANISH_ON   = 0,//MCSS_VANISH_ON,
  BTLV_VANISH_OFF  = 1,//MCSS_VANISH_OFF,
  BTLV_VANISH_FLIP = 2,//MCSS_VANISH_FLIP,
  BTLV_EFFECT_VANISH_ON,
  BTLV_EFFECT_VANISH_OFF,
  BTLV_EFFECT_VANISH_POP,
}BTLV_VANISH_FLAG;


typedef struct
{
  BtlPokePos mAtkPos;
  BtlPokePos mDefPos;
  WazaID     mId;
  WazaTarget mRange;
  BtlvWazaEffect_TurnType mTurnType;
  u8         mContinueCount ;
  bool       mSyncDamageEffect;
  BtlTypeAffAbout mDmageAbout;

  bool       mSetData;
  bool       mIsBreakObj;

  //管理用
  int mFrame; //そのLOOPで何フレーム目が再生されたか。

  //モーションKeyframe対応
  bool      mEnableKeyAdjust;
  bool      mIsPlayMotion;
  int       mKeyDiffFrame;
  int       mAttackMotion;

  int       mStartFrame;  //連続技でずれる
}BTLV_WAZA_EFF_PARAM;

//移植タイプ定義↑

//全力技用の管理
typedef struct
{
  bool isZenryoku;  //全力技シーケンス
  bool isAttack;     //攻撃技の場合
  bool isUnique;    //固有技の場合

  int seq;
  int wazaType;

  //開始演出用
  u16 aulaMsgId;
  int aulaMsgParam[3];

  //スキップ時処理用(リセット対象を保持しておく
  BtlvPos targetPoke;
}WAZA_ZENRYOKU_PARAM;

typedef struct
{
  //初期化パラメータ
  MainModule *mMainModule;
  const BTL_CLIENT *mClient;
  POKECON *mPokeCon;
  BtlBagMode mBagMode;
  Savedata::ConfigSave::MSG_SPEED mMsgSpeed;
#if PM_DEBUG
  BTLV_DEBUG_PARAM *mDebugParam;
#endif
}BTLV_INIT_PARAM;

//シーケンスの一時停止待ち関数
typedef bool(BattleViewSystem::*BTLV_SEQ_WAIF_FUNC)(void);

class BattleViewSystem 
{
GFL_FORBID_COPY_AND_ASSIGN(BattleViewSystem);

public:

  static const f32 NEAR_PLANE;
  static const f32 FAR_PLANE;
  static const f32 CAMERA_FOVY;

  BattleViewSystem(void);
  BattleViewSystem(BTLV_INIT_PARAM* initParam);
  virtual ~BattleViewSystem();

  //システム初期化
  virtual bool Initialize(gfl2::heap::HeapBase* heap);
  //システム解放
  virtual bool Finalize(void);
  virtual bool FinalizeForce(void);

  virtual bool SetupGraphic(void);

  //更新(30フレーム前半用)
  virtual void Update(void);
  //更新(30フレーム後半用)
  void NextFrameUpdate(void);
  //描画更新
  void PreDraw(void);
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);
public:
  //初期化
  void CMD_StartSetup(void);
  bool CMD_WaitSetup(void);
  bool CMD_WaitSetup_RecReset(void);
  //開放
  void CMD_StartCleanUp(void);
  bool CMD_WaitCleanUp(void);

  bool CMD_InitStartWaitCameraEffect(void);
  bool CMD_WaitStartWaitCameraEffect(void);
  void CMD_InitFinishWaitCameraEffect(void);
  bool CMD_WaitFinishWaitCameraEffect(void);

  //コマンド系
  void       CMD_UI_SelectAction_Start( const BTL_POKEPARAM* bpp, u8 pokeIndex, bool fReturnable, BTL_ACTION_PARAM* dest );
  BtlAction  CMD_UI_SelectAction_Wait( void );
  void       CMD_UI_SelectAction_ForceQuit( void );

  void CMD_UI_SelectWaza_Start( const BTL_POKEPARAM* bpp, u8 pokeIndex, BTL_ACTION_PARAM* dest );
  void CMD_UI_SelectRotationWaza_Start( u8 pokeIndex, BTLV_ROTATION_WAZASEL_PARAM* selParam, BTL_ACTION_PARAM* dest );
  void CMD_UI_SelectWaza_Restart( u8 pokeIndex );
  bool CMD_UI_SelectWaza_Wait( void );
  bool CMD_UI_SelectWaza_End( void );
  void CMD_UI_SelectWaza_ForceQuit( void );

  void       CMD_UI_SelectTarget_Start( u8 poke_index, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
  BtlvResult CMD_UI_SelectTarget_Wait(void);
  void       CMD_UI_SelectTarget_ForceQuit(void);

  void CMD_UI_RestartIfNotStandBy( void );
  void CMD_UI_Restart( void );
  bool CMD_UI_WaitRestart( void );

  void CMD_StartMemberChangeAct( BtlPokePos pos, u8 clientID, u8 memberIdx );
  bool CMD_WaitMemberChangeAct(void);

  void CMD_CopyMsgToBufferExpanded( const STRBUF* src );
  void CMD_SetMsgToBuffer( const BTLV_STRPARAM* param );
  void CMD_StartMsgInBuffer( bool isKeyWait = false );

  //--------------------------------------------------------------------------
  //
  // このコマンドでメッセージ表示を開始した場合、開始直後から、
  //
  // callbackTarget->PrintCallback( arg ) を、毎フレーム呼び出し続けてください。
  // ※ arg については、btl_client.h の PrintCallback() メソッド近辺にコメントがありますので参照してください。
  //
  // PrintCallback が true を返してきたら、このコマンドを終了してください。
  //
  //--------------------------------------------------------------------------
  void CMD_StartMsgCallback( const BTLV_STRPARAM* param, BTL_CLIENT* callbackTarget );

  //=============================================================================================
  /**
   * ポケモンリスト処理を開始
   *
   * @param   core            [in]  描画メインモジュールハンドラ
   * @param   param           [in]  リスト処理パラメータポインタ
   * @param   outMemberParam  [in]  退出するメンバーのパラメータ
   * @param   outMemberIndex  [in]  「ポケモン」メニューからのメンバー交替時、退出するメンバーのIndex / それ以外-1
   * @param   fCantEsc        [in]
   * @param   result          [out] 選択結果格納構造体ポインタ
   */
  //=============================================================================================
  void CMD_StartPokeList( const PokeSelParam* param, const BTL_POKEPARAM* outMemberParam, int outMemberIndex, bool fCantEsc, PokeSelResult* result );

  //=============================================================================================
  /**
   * ポケモンリスト処理の終了待ち
   *
   * @param   core      [in]  描画メインモジュールハンドラ
   *
   * @retval  bool      終了したらTRUE
   */
  //=============================================================================================
  bool CMD_WaitPokeList(void);

  //=============================================================================================
  /**
   *  ポケモンリスト処理の強制終了（コマンド制限時間による）
   */
  //=============================================================================================
  void CMD_ForceQuitPokeList(void);
  
  //=============================================================================================
  /**
   *  ポケモンリスト処理の強制終了待ち
   * 
   * @retval true   強制終了完了
   * @retval false  強制終了処理中
   */
  //=============================================================================================
  bool CMD_WaitForceQuitPokeList(void);




  //=============================================================================================
  /**
   * ポケモン選択処理を開始
   *
   * @param   core            [in]  描画メインモジュールハンドラ
   * @param   param           [in]  選択処理パラメータポインタ
   * @param   outMemberIndex  [in]  「ポケモン」メニューからのメンバー交替時、退出するメンバーのIndex / それ以外-1
   * @param   fCantEsc        [in]
   * @param   result          [out] 選択結果格納構造体ポインタ
   */
  //=============================================================================================
  void CMD_StartPokeSelect( const PokeSelParam* param, int outMemberIndex, bool fCantEsc, PokeSelResult* result );

  //=============================================================================================
  /**
   * ポケモン選択処理の終了待ち
   *
   * @param   core      [in]  描画メインモジュールハンドラ
   *
   * @retval  bool      終了したらTRUE
   */
  //=============================================================================================
  bool CMD_WaitPokeSelect(void);

  //=============================================================================================
  /**
   *  ポケモン選択処理の強制終了（コマンド制限時間による）
   */
  //=============================================================================================
  void CMD_ForceQuitPokeSelect(void);
  
  //=============================================================================================
  /**
   *  ポケモン選択処理の強制終了（コマンド制限時間による）待ち
   */
  //=============================================================================================
  bool CMD_WaitForceQuitPokeSelect(void);



  /**
   *  ワザ説明（ショートカット）画面の呼び出し
   */
  void CMD_StartWazaInfoView( u8 pokeIdx, u8 wazaIdx );



  //=============================================================================================
  /**
   *
   *
   * @param   wk
   * @param   strID
   * @param   clientID
   *
   * @retval  void
   */
  //=============================================================================================
  void CMD_StartMsg( const BTLV_STRPARAM* param );
  void CMD_StartMsgWaza( u8 pokeID, u16 waza, bool isZenryokuWaza );
  void CMD_PrintMsgAtOnce( const BTLV_STRPARAM* param );


  void CMD_StartMsgStd( u16 strID, const int* args );
  void CMD_StartMsgSet( u16 strID, const int* args );
  bool CMD_StartMsgTrainer( u32 trainerID, int param, bool isKeyWait = false );
  bool CMD_StartMsgFesTrainer(u16 strId);

  bool CMD_WaitMsg(void);     // Update & Window消去まで
  bool CMD_WaitMsg_WithoutHide(void);   // Updateのみ
  void CMD_HideMsg(void);               // Window消去のみ


  void CMD_ACT_WazaEffect_Start( BtlPokePos atPokePos, BtlPokePos defPokePos, WazaID waza,
    WazaTarget wazaRange, BtlvWazaEffect_TurnType turnType, u8 continueCount , bool syncDamageEffect/*, BtlObjBreak* objBreakManager*/ );

  bool CMD_ACT_WazaEffect_Wait(void);
  //技エフェクトスキップ時の背景破壊フラグのセット
  void CMD_ACT_SetObjBreakFlg(void);
  
  void CMD_ACT_DamageEffectSingle_Start( WazaID wazaID, BtlPokePos defPokePos, BtlTypeAffAbout affAbout );
  bool CMD_ACT_DamageEffectSingle_Wait(void);


  //=============================================================================================
  /**
   * 複数体一斉ダメージ処理 開始
   *
   * @param   wk
   * @param   pokeCnt   ポケモン数
   * @param   affifnity 相性ID
   * @param   pokeID    対象となるポケモンID配列
   */
  //=============================================================================================
  void CMD_ACT_DamageEffectPlural_Start( u32 pokeCnt, BtlTypeAffAbout affAbout, const u8* pokeID, const BtlTypeAff* pokeAffinity, WazaID waza );
  bool CMD_ACT_DamageEffectPlural_Wait(void);

  void CMD_ACT_MigawariDamageEffect_Start( WazaID wazaID, BtlPokePos migawariPos, BtlTypeAffAbout affAbout );
  bool CMD_ACT_MigawariDamageEffect_Wait(void);
  
  //上3つのダメージ処理(Single・Plural・Migawari)の共通処理
  bool CMD_ACT_DamageEffect_WaitCore(void);
  
  void CMD_StartDeadAct( BtlPokePos pokePos, bool isKillCountEffectExist );
  bool CMD_WaitDeadAct(void);
  void CMD_StartDeadActPlural( u32 pokeCnt, const BtlPokePos *pokePos );
  bool CMD_WaitDeadActPlural(void);

  void CMD_StartReliveAct( BtlPokePos pos );
  bool CMD_WaitReliveAct(void);
  void CMD_ACT_MemberOut_Start( BtlvPos vpos, u16 effectNo );
  bool CMD_ACT_MemberOut_Wait(void);
  void CMD_ACT_TameWazaHide( BtlvPos vpos, BTLV_VANISH_FLAG vanishFlag );
  void CMD_ACT_SimpleHPEffect_Start( BtlPokePos pokePos );
  bool CMD_ACT_SimpleHPEffect_Wait(void);

  void CMD_AddEffect( u16 effectNo );
  bool CMD_WaitEffect(void);
  void CMD_AddEffectByPos( BtlvPos vpos, u16 effectNo );
  bool CMD_WaitEffectByPos(void);
  void CMD_AddEffectByDir( BtlvPos vpos_from, BtlvPos vpos_to, u16 effectNo );
  bool CMD_WaitEffectByDir(void);


  void CMD_TokWin_DispStart( BtlPokePos pos, bool fFlash );
  bool CMD_TokWin_DispWait( BtlPokePos pos );
  void CMD_QuitTokWin( BtlPokePos pos );
  bool CMD_QuitTokWinWait( BtlPokePos pos );
  void CMD_TokWin_Renew_Start( BtlPokePos pos );
  bool CMD_TokWin_Renew_Wait( BtlPokePos pos );
  void CMD_TokWin_SwapStart( BtlPokePos pos1, BtlPokePos pos2 );
  bool CMD_TokWin_SwapWait(void);


  void CMD_StartRankDownEffect( u8 vpos, u8 rankDownVolume );
  void CMD_StartRankUpEffect( u8 vpos, u8 rankUpVolume );
  bool CMD_WaitRankEffect( u8 vpos );


  void CMD_StartCommWait(void);
  bool CMD_WaitCommWait(void);
  void CMD_ResetCommWaitInfo(void);

  void CMD_ItemAct_Start( BtlPokePos pos );
  bool CMD_ItemAct_Wait( BtlPokePos pos );
  void CMD_KinomiAct_Start( BtlPokePos pos );
  bool CMD_KinomiAct_Wait( BtlPokePos pos );

  void CMD_FakeDisable_Start( BtlPokePos pos, bool fSkipMode );
  bool CMD_FakeDisable_Wait(void);
  void CMD_ChangeForm_Start( BtlvPos vpos );
  bool CMD_ChangeForm_Wait(void);
  void CMD_Hensin_Start( BtlvPos atkVpos, BtlvPos tgtVpos );
  bool CMD_Hensin_Wait(void);

  //上3つの読み替え処理ダメージ処理(イリュージョン・へんしん・フォルムチェンジ)の共通処理
  void CMD_ACT_PokeChangeEffect_StartCore(BtlvPos vpos,u16 effId ,bool isNoLoad);
  bool CMD_ACT_PokeChangeEffect_WaitCore(void);
  bool CMD_ACT_PokeChangeEffect_WaitCore_Skip(void);

  void CMD_ACT_MoveMember_Start( u8 clientID, u8 vpos1, u8 vpos2, u8 posIdx1, u8 posIdx2 );
  bool CMD_ACT_MoveMember_Wait(void);

  void CMD_ACT_ZenryokuWazaBegin_Start( BtlPokePos pokePos, BtlPokePos targetPos, WazaNo wazaNo, u16 strId, u8 strParam1, u8 strParam2, u8 strParam3 );
  bool CMD_ACT_ZenryokuWazaBegin_Wait( void );
  void CMD_ACT_ZenryokuWazaEnd_Start( BtlPokePos pokePos );
  bool CMD_ACT_ZenryokuWazaEnd_Wait( void );


  void CMD_ITEMSELECT_Start( u8 bagMode, u8 energy, u8 reserved_energy,
          u8 fFirstPokemon, u8 fBallTargetHide );
  bool CMD_ITEMSELECT_Wait(void);
  void CMD_ITEMSELECT_ForceQuit(void);

  u16 CMD_ITEMSELECT_GetItemID(void);
  u8  CMD_ITEMSELECT_GetCost(void);
  u8  CMD_ITEMSELECT_GetTargetIdx(void);
  u8 CMD_ITEMSELECT_GetWazaIdx(void);
  void CMD_ITEMSELECT_ReflectUsedItem(void);


  void CMD_YESNO_Start( bool b_cancel, YesNoMode yesno_mode );
  bool CMD_YESNO_Wait( BtlYesNo* result );
  void CMD_YESNO_ForceQuit( void );
  void CMD_YESNO_Delete(void);

  enum WazaWasureResult
  {
    WAZAWASURE_RESULT_NONE,       // 選択中
    WAZAWASURE_RESULT_CANCEL,     // 何も選ばなかった
    WAZAWASURE_RESULT_SELECT_OLD, // 習得済みの技を選んだ
    WAZAWASURE_RESULT_SELECT_NEW, // 新しく覚える技を選んだ
  };
  void CMD_WAZAWASURE_Start( u8 pos, WazaID waza );
  WazaWasureResult CMD_WAZAWASURE_Wait( u8* selectedWazaIndex );

  void CMD_StartSelectChangeOrEscape(void);
  bool CMD_WaitSelectChangeOrEscape( u8* fSelect );

  // レベルアップ情報処理
  void CMD_LvupWin_StartDisp( const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo );
  bool CMD_LvupWin_WaitDisp(void);
  void CMD_LvupWin_StepFwd(void);
  bool CMD_LvupWin_WaitFwd(void);
  void CMD_LvupWin_StartHide(void);
  bool CMD_LvupWin_WaitHide(void);

  // 録画再生時フェードアウト
  void CMD_RecPlayFadeOut_Start(void);
  bool CMD_RecPlayFadeOut_Wait(void);
  void CMD_RecPlayFadeIn_Start(void);
  bool CMD_RecPlayFadeIn_Wait(void);

  //録画再生のターンの時にゲージを表示する仕組み
  void CMD_RecDispTurnGauge_Start(void);
  bool CMD_RecDispTurnGauge_Wait(void);

  // かわいがり（なでる）処理
  void CMD_Naderu_Start( BtlvPos vpos );
  bool CMD_Naderu_Wait( BtlvPos vpos );

  // お手入れ
  void CMD_OteireButton_StartDisp( void );
  void CMD_OteireExecuteEffect_Start( BtlvPos vpos );  // お手入れ移行演出( 開始 )
  bool CMD_OteireExecuteEffect_Wait( void );           // お手入れ移行演出( 終了待ち )

  // ぬし戦
  void CMD_VsNusiWinEffect_Start( void );  // ぬし戦勝利演出( 開始 )
  bool CMD_VsNusiWinEffect_Wait( void );   // ぬし戦勝利演出( 終了待ち )

  // 乱入
  void CMD_Intrude_Call_Start( BtlPokePos callPokePos );                               // 助けを呼ぶ演出　開始
  bool CMD_Intrude_Call_Wait( void );                                                  // 助けを呼ぶ演出　終了待ち
  void CMD_Intrude_NotAppeared_Start( BtlPokePos callPokePos );                        // 助けが来なかった場合の演出　開始
  bool CMD_Intrude_NotAppeared_Wait( void );                                           // 助けが来なかった場合の演出　終了待ち
  void CMD_Intrude_Appear_Start( BtlPokePos callPokePos, BtlPokePos intrudePokePos );  // 乱入ポケモンの登場演出　開始
  bool CMD_Intrude_Appear_Wait( void );                                                // 乱入ポケモンの登場演出　終了待ち

  //=============================================================================================
  //  時間制限によるアクション選択の強制終了対応
  //=============================================================================================
  void CMD_MsgWinHide_Start(void);
  bool CMD_MsgWinHide_Wait(void);

  //=============================================================================================
  //  時間制限によるアクション選択の強制終了対応
  //=============================================================================================
  void CMD_ForceQuitInput_Notify(void);
  bool CMD_ForceQuitInput_Wait(void);

  //=============================================================================================
  //  録画再生モード
  //=============================================================================================
  void CMD_RecPlayer_Init(u16 max_chapter);
  int CMD_CheckRecPlayerInput(void);
  void CMD_UpdateRecPlayerInput( u16 currentChapter, u16 ctrlChapter );
  void CMD_RecPlayer_StartSkip( void );
  void CMD_RecPlayer_StartSkipDisplay( u16 nextChapter );
  void CMD_RecPlayer_StartQuit( u16 chapter, BTLV_INPUT_BR_STOP_FLAG stop_flag );

  //=============================================================================================
  //  XY追加コマンド
  //=============================================================================================
  void CMD_TrainerIn_Win( BtlvTrainerPos position );
  void CMD_TrainerIn_Win_AZ( BtlvTrainerPos position ); //AZ特殊勝利
  void CMD_TrainerIn_Lose( BtlvTrainerPos position );
  void CMD_TrainerIn_WinFes(BtlvTrainerPos position); //フェス用勝利演出
  //重要キャラの対戦中台詞
  void CMD_TrainerIn_Event( BtlvTrainerPos position );
  bool CMD_WaitTrainerIn();

  //BTLV_EFFECTから移植
  void  CMD_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvPos pos );
  bool  CMD_EFFECT_CheckExecuteGauge( void );
  void  CMD_EFFECT_CalcGaugeEXP( int position, int value );
  void  CMD_EFFECT_CalcGaugeEXPLevelUp( int position, const BTL_POKEPARAM* bpp );
  bool  CMD_EFFECT_CheckExistGauge( BtlvPos pos );
  void  CMD_EFFECT_BallThrow( int position, u16 item_no, u8 yure_cnt, bool f_success, bool f_critical );
  void  CMD_EFFECT_BallThrowTrainer( int vpos, u16 item_no );
  bool  CMD_EFFECT_WaitBallThrow( void );
  bool  CMD_EFFECT_WaitBallThrowTrainer( void );

  //BTLV_EFFECTから移植(タイマー
  void  CMD_EFFECT_CreateTimer( int game_time, int client_time, int command_time );
  void CMD_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE type, bool enable, bool init, bool isCountDownStart );
  bool  CMD_EFFECT_IsZero( BTLV_TIMER_TYPE type );
  u32   CMD_EFFECT_GetClientTime( u8 clientID );
  void  CMD_EFFECT_SetClientTime( u8 clientID, u32 time );

  // メガ進化
  void  CMD_StartMegaEvo( BtlvPos vpos , bool isLong = false );
  bool  CMD_WaitMegaEvo( BtlvPos vpos );

  // かわいがり系振り向き処理
  void CMD_TurnPoke( BtlPokePos pos );
  bool CMD_TurnPoke_Wait( BtlPokePos pos );

  // 天候変更処理
  void CMD_ChangeWheather( BtlWeather weather );
  bool CMD_ChangeWheather_Wait( void );

  // フィールド変更処理
  void CMD_ChangeGround( BtlGround type );
  bool CMD_ChangeGround_Wait( void );

  // カメラリセット処理
  void CMD_ResetCamera(void);
  bool CMD_ResetCamera_Wait( void );

  //3DSさかさま判定関連
  void CMD_Start3DSReverseWatcher(bool is_short);
  void CMD_Stop3DSReverseWatcher(void);
  bool CMD_Get3DSReverseWatcherResult(void);

#if PM_DEBUG
  const BTL_CLIENT* SetTmpClient( const BTL_CLIENT* tmpClient )
  {
    const BTL_CLIENT* oldClient = mBattleClient;
    mBattleClient = tmpClient;
    return oldClient;
  }
#endif

public:

  virtual BtlvMode GetBtlvMode(void)const{return BTLV_MODE_BATTLE;}
  virtual void SetIsZenryokuTutorial(bool flg){mIsZenryokuTutorial = flg;}
  virtual bool GetIsZenryokuTutorial(void){return mIsZenryokuTutorial;}

  //-------------------------------------------------------
  //バトル数値取得系
  //多用禁止(メッセージ初期化でUIが使う)
  MainModule* GetMainModule(void){return mMainModule;}

  virtual const BTL_FIELD_SITUATION* GetFieldSituation(void);
  void SetBgSetupParam(btl::BgSystem::SETUP_PARAM *param);

  POKECON*  GetBattleContainer(void)const{return mBattleContainer;}
  virtual BTL_POKEPARAM* GetBattlePokeParam( BtlvPos pos ) const;
  virtual BTL_PARTY* GetBattleParty_Self(void)const;
  virtual BTL_PARTY* GetBattleParty_Friend(void)const;
  virtual BTL_PARTY* GetBattleParty_Enemy(int idx = 0)const;
  virtual bool IsPlayerInLeftSide( void ) const;
  virtual BtlSound* GetBattleSound(void);
  virtual GameSys::GameProc* GetBattleProc(void)const;

  virtual const FieldStatus* GetFieldStatus( void ) const;
  virtual const SideEffectStatus* GetSideEffectStatus( BtlSide side, BtlSideEffect effect ) const;
  virtual const PosEffectStatus* GetPosEffectStatus( BtlPokePos pos, BtlPosEffect effect ) const;
  virtual const PartyParam* GetPartySetupParam( u8 clientID ) const;

  virtual Savedata::MyItem* GetMyItem(void)const;
  virtual const Savedata::MyStatus* GetClientPlayerData( u8 clientID , int btlFesIdx = -1 ) const;
  virtual Savedata::ConfigSave* GetConfigSaveData( void ) const;
  virtual const Savedata::ZukanData* GetZukanSaveData(void) const;
  virtual const Savedata::ZukanData* GetZukanSaveDataOnBattleStart(void) const;
  virtual GameSys::GameData* GetGameData(void) const;

  virtual bool IsMegaEvolved( u8 clientID ) const;
  virtual bool IsMegaEvolved_Self( void ) const;
  virtual bool IsUltraBursted(u8 clientID) const;
  virtual bool IsUltraBursted_Self(void) const;
  virtual const ZenryokuWazaStatus* GetZenryokuWazaStatus(u8 clientID) const;
  virtual const ClientPublicInformation* GetClientPublicInformation( u8 clientID ) const;
  virtual u8 GetBattleRoyalRanking( u8 clientID ) const;

  virtual BtlCompetitor GetBattleCompetitor(void)const; //対戦種類(野生・トレーナー・サブウェイ
  virtual BtlRule GetBattleRule(void)const; //ルール
  virtual BtlCommMode GetCommMode( void ) const;  //通信モード
  virtual bool IsCommMode(void)const;
  virtual bool IsCommChild(void)const;
  virtual bool IsMultiMode( void ) const;
  virtual bool IsBattleFesMode(void) const;
  virtual bool IsPokeExist(BtlvPos pos) const;
  virtual bool IsFriendExist( void ) const;
  virtual bool IsClientTrainerExist( u8 clientID ) const;
  virtual u8 GetClientID(void)const;        //クライアントID
  virtual u8 GetFriendCleintID( void ) const;
  virtual u8 GetEnemyClientID( u8 idx ) const;
  virtual u8 GetEnemyClientNum( u8 clientID ) const;
  virtual bool IsClientNpc( u8 idx ) const;
  virtual BtlvPos GetClientIdToBtlvPos( u8 clientId , u8 pokeIdx = 0) const;
  virtual u8 GetBtlvPosToClientId( BtlvPos vpos ) const;
  virtual u8 GetBtlvPosToPosIdx( BtlvPos vpos ) const;
  virtual u8 GetBtlvPosToTrainerIdx( BtlvPos vpos ) const;
  virtual u16 GetTrainerID( u8 clientID ) const;
  virtual u16 GetTrainerType( u8 clientID ) const;
  virtual u16 GetClientRating( u8 clientID ) const;
  virtual bool IsClientRatingDisplay( void ) const;
  virtual BtlBagMode GetBagMode(void)const{return mBagMode;}  //バッグモード
  virtual u8 GetFrontPosCount_Self() const;
  virtual u8 GetFrontPosCount_Friend() const;
  virtual u8 GetBenchPosIndex_Self() const;
  virtual u8 GetBenchPosIndex_Friend() const;
  virtual bool IsSkyBattle(void) const;
  virtual bool IsZukanRegistered(const BTL_POKEPARAM* bpp) const;
  virtual bool IsZukanLooked(const BTL_POKEPARAM* bpp) const;
  virtual bool IsEnableWazaEffect(void) const;
  virtual bool IsItemEnable(void) const;
  virtual bool GetSetupStatusFlag( BTL_STATUS_FLAG flag ) const;
  virtual bool GetEnableTimeStop(void) const;
  virtual bool GetEnableVoiceChat(void) const;
  virtual bool IsRecordPlayMode() const;
  virtual u32 GetTurnCount( void ) const;
  virtual bool IsWazaEffectEnable(void)const;

  virtual void GetSkyBoxTime(s32 *hour,s32 *min,s32 *sec);

  //  にげるモードを取得
  BtlEscapeMode GetEscapeMode();

  //  ui入力ができるかどうか
  //  主に捕獲チュートリアルなどでユーザーが入力できない場合は継承してoverrideをしてfalseを返す
  virtual bool IsControllable()  const { return true; }
  virtual bool IsViewerMode()  const { return false; }
  virtual bool IsQuickStart(void);
  //代表ビースト戦か？
  virtual bool IsBeastBattle(void);

  virtual bool IsRecordSkipMode() const{return mRec_IsSkip;}
  virtual bool IsRecordStartSkip() const{return mRec_IsStartSkip;}
  virtual bool IsUnselectableWaza( const BTL_POKEPARAM* bpp, WazaID waza ) const;
  virtual const BSP_ENCOUNT_EFFECT* GetEncountEffect( void );

  void EnableFrameSkip(bool flg);
  void SuspendFrameSkip(void);  //ポケモン読み替えのため数フレーム止める

  //陣営別のサイズ取得
  PokeTool::POKE_SIZE GetPokeSize(bool isEnemy);

  void GetDefaultPokePos( BtlvPos vPos , gfl2::math::Vector3 *pos , int *deg );
  void GetDefaultCamera( gfl2::math::Vector3 *pos , gfl2::math::Vector3 *trg , f32 *fov );
  void GetDefaultTrainerPos( int idx , gfl2::math::Vector3 *pos , int *deg , bool isOrigin = false );

  void ResetPokemon( BtlvPos vPos , int frame );
  void ResetTrainer(int idx);
  void ResetDefaultCamera(int frame);
  void ResetHitBack(void);
  void GetFollowCameraPos( f32 camLen , gfl2::math::Vector3 *ret );
  void GetFollowCameraRot( gfl2::math::Vector3 *ret );

  BTLV_LAND_TYPE CheckLandType(BtlvPos pos );

  //Rotom関係
  bool IsEnableRotom(void)const;
  Savedata::FieldMenu::RotomRank GetRotomRank(void)const;
  //Rotom導入を再生するか？(乱入時も)
  bool IsEnableRotomSearch(BTL_POKEPARAM *target);
  //図鑑セーブを見るとそのバトル内で見たものが記録されないので、バトルで管理
  void RegistLookedMonsNo(BTL_POKEPARAM *bpp);


    //-------------------------------------------------------
  //システム取得系
  gfl2::fs::AsyncFileManager* GetAsyncFileManager(void){return mAsyncFileManager;}

  const gfl2::ui::DeviceManager* GetUIDevice(void){ return mDeviceManager; }
  const gfl2::ui::Button*        GetButton(void){return mDeviceManager->GetButton(0);}
  const gfl2::ui::VectorDevice*  GetKey(void){return mDeviceManager->GetCrossKey(0);}
  const gfl2::ui::TouchPanel*    GetTP(void){return mDeviceManager->GetTouchPanel(0);}

  virtual gfl2::heap::HeapBase* GetMainHeap(void){return mHeap;}
  virtual gfl2::heap::HeapBase* GetResidentHeap(void){return mResidentHeap;}
  virtual gfl2::heap::HeapBase* GetPokeHeap(int i){return mPokeHeap[i];}
  virtual gfl2::heap::HeapBase* GetTrainerHeap(int i){return mTrainerHeap[i];}
  virtual gfl2::heap::HeapBase* GetTrainerCommonHeap(void){return mTrainerCommonHeap;}
  virtual gfl2::heap::HeapBase* GetTempHeap(void){return mTempHeap;}
  virtual gfl2::heap::HeapBase* GetWeatherHeap(int i){return mWeatherHeap[i];}
  virtual gfl2::heap::HeapBase* GetFullPowerHeap(void){return mFullPowerHeap;}

  //行動選択中のみ有効なヒープ
  virtual gfl2::heap::HeapBase* GetWaitCameraHeap(void){return mWaitCameraHeap;}
  virtual gfl2::heap::HeapBase* GetWaitSickHeap(void){return mWaitSickHeap;}
  gfl2::gfx::IGLAllocator* GetWaitCameraGLAllocator(void){return mWaitCameraGLAllocator;}

  virtual gfl2::heap::NwAllocator* GetAllocator(void){return mResidentAllocator;}

  gfl2::gfx::IGLAllocator* GetGLAllocator(void){return mResidentGLAllocator;}
  gfl2::gfx::IGLAllocator* GetPokeGLAllocator(int i){return mPokeGLAllocator[i];}
  gfl2::gfx::IGLAllocator* GetTrainerGLAllocator(int i){return mTrainerGLAllocator[i];}
  gfl2::gfx::IGLAllocator* GetTempGLAllocator(void){return mTempGLAllocator;}
  gfl2::gfx::IGLAllocator* GetFullPowerGLAllocator(void){return mFullPowerAllocator;}

  virtual ::app::util::Heap* GetAppHeap(void){return mAppHeap;}
  gfl2::Effect::GFHeap* GetEffectHeap(void){return mEffectHeap;}
  gfl2::Effect::System* GetEffectSystem(void){return mEffectSystem;}

  BtlvSeqSystem*  GetSeqSys(void){return mSeqSys;}

  DemoLib::Obj::ObjectManager* GetObjectManager(void){return mObjectManager;}
  DemoLib::Obj::TaskSystem* GetTaskSystem(void){return mTaskSystem;}
  DemoLib::Obj::TaskSystem* GetTaskSystemAfterCamera(void){return mTaskSystemAlways;}
  DemoLib::Obj::TaskSystem* GetTaskSystemAlways(void){return mTaskSystemAlways;}
  DemoLib::Obj::ResourceManager* GetResourceManager(void){return mResourceManager;}


  poke_3d::model::CharaModelFactory* GetCharaModelFactory(void){return mCharaFactory;}
  poke_3d::model::DressUpModelResourceManager* GetDressUpModelResourceManager(void){return mDressUpModelResourceManager;}

  BtlvObject_PokeModel* GetPokeModel(BtlvPos pos){return mPokeModel[pos];}

  //-------------------------------------------------------
  //内部システム取得系
  BtlvUi* GetUiSys(void){return mUiSys;}
  BtlvRenderSystem* GetRenderSys(void){return mRenderSys;}
  BgSystem* GetBgSystem(void){return mBgSystem;}
  BtlvCameraSystem* GetCameraSys(void){return mCameraSys;}
  PokeTool::PokeModelSystem* GetPokeModelSys(void){return mPokeModelSys;}

  //-------------------------------------------------------
  //文字列関係
  virtual void GetStringUi(gfl2::str::StrBuf* dst, u16 strID );
  virtual const gfl2::str::StrBuf* GetClientTrainerName( print::GrammaticalAttribute* grammer, u8 clientID );
  gfl2::str::StrBuf* GetWorkStr(int idx){return mWorkStr[idx];}
  print::WordSet* GetWordSet(void){return mWordSet;}

  //-------------------------------------------------------
  //数値変換
  virtual BtlvPos BtlPosToViewPos( BtlPokePos pos )const;
  virtual BtlPokePos ViewPosToBtlPos( BtlvPos pos )const;
  virtual BtlSide ViewPosToBtlSide( BtlvPos pos ) const;
  virtual BtlvPos PokeIDToViewPos( int pos )const;

  //自分側？
  virtual bool IsPlayerSide(BtlvPos pos );

  //------------------------------------------------------
  // UI用
  virtual void GetUiDisplay_PokeType( pml::PokeType* type1, pml::PokeType* type2, const BTL_POKEPARAM& poke ) const;
  virtual pml::PokeType GetUiDisplay_WazaType( WazaID orgWaza, WazaID actWaza ) const;
  virtual u16 GetUiDisplay_WazaPower( WazaID orgWaza, WazaID actWaza ) const;
  virtual u16 GetUiDisplay_WazaHitPer( WazaID orgWaza, WazaID actWaza ) const;
  virtual WazaDamageType GetUiDisplay_WazaDamageType( WazaID orgWaza, WazaID actWaza ) const;
  virtual bool IsWazaAffinityDisplayEnable( u8 defPokeID ) const;
  virtual BtlTypeAffAbout CalcWazaAffinityAbout( u8 atkPokeID, u8 defPokeID, WazaID waza, bool* isDisplayEnable );
  virtual void GetUiDisplay_Turn_Weather( u32* numerator, u32* denominator ) const;
  virtual void GetUiDisplay_Turn_Ground( u32* numerator, u32* denominator ) const;
  virtual void GetUiDisplay_Turn_Hikarinokabe( u32* numerator, u32* denominator, BtlSide side ) const;
  virtual void GetUiDisplay_Turn_Reflector( u32* numerator, u32* denominator, BtlSide side ) const;
  virtual void GetUiDisplay_Turn_AuroraVeil( u32* numerator, u32* denominator, BtlSide side ) const;
  virtual void NotifyOteireButtonPressed( void );
  virtual bool IsOteireEnable( void ) const;

  //------------------------------------------------------
  // 技エフェクト用
  BTLV_WAZA_EFF_PARAM* GetWazaParam(void){return &mWazaParam;}
  WAZA_ZENRYOKU_PARAM* GetZenryokuParam(void){return &mZenryokuParam;}

  void ResetZenryokuParam(void);
  void ResetZenryokuTrainerMotion(void);
  //上昇下降エフェクトの連続再生を抑制するため
  bool CheckCanPlayEffect( BtlvPos vpos, u16 effectNo );
  void ResetCheckCanPlayEffect(void);

  void StartWazaEffect(void);
  void StartWazaEffectCore(void);
  bool UpdateWazaEffect(void);

  int GetWazaSequenceFile(int mainNo , int subNo );

  //グループオプションの判断
  bool CheckCanPlayCommand(int grpOpt,u32 comNo);
  //プログラムで判定して分岐するリソースのチェック(ボールエフェクトとか)
  u32 CheckWazaDataId_Model( u32 datId , u16 idx = 0 );
  u32 CheckWazaDataId_Camera( u32 datId , u16 idx = 0 );
  u32 CheckWazaDataId_Env( u32 datId , u16 idx = 0 );
  u32 GetTrainerLightEnv( int type , u16 idx = 0);

  u32 CheckWazaDataId_Effect( u32 datId , u16 idx = 0 );
  u32 CheckWazaDataId_Texture( u32 datId , u16 idx );
  u32 CheckWazaDataId_TrainerTexture( int trgChara , int type );

  u32 CheckWazaDataId_Se( u32 datId , u16 idx = 0 );
  //------------------------------------------------------
  //シーケンス系

  //シーケンスの一時停止系の関数の設定
  void SetSequenceWaitFunc( int type );
  //シーケンスシステムの一時停止の更新とチェック関数
  void UpdateWaitSequenceSystem(void);
  bool IsWaitSequenceSystem(void);
  
  //シーケンス一時停止の待ち関数
  bool SeqComWaitFunc_KeyInput( void );
  bool SeqComWaitFunc_Message( void );
  bool SeqComWaitFunc_Load( void );
  bool SeqComWaitFunc_LoadPoke( void );
  bool SeqComWaitFunc_LoadBg( void );
  bool SeqComWaitFunc_PokeIntro( void );
  bool SeqComWaitFunc_WaitFade( void);
  bool SeqComWaitFunc_WaitDemoFadeSync( void);

  //非同期読みから待ちまでしてくれる再生
  void SeqComFunc_PlaySequenceFile( int no );
  void SeqComFunc_PlaySequenceFileEffect( int effNo ); //エフェクト定義を使った再生
  void SeqComFunc_PlaySequenceFileDebug(void);
  bool SeqComFunc_IsFinishSequenceFile( bool isNoRelease = false );

  void SeqComFunc_CrealWazaResource(void);
  void SeqComFunc_CleanAllWazaResource(void);

  //Target系のリセット
  void SetComTargetReset(void);
  //シーケンスの攻撃側・防御側からBtlvPosを返す
  BtlvPos SeqComFunc_GetTargetPoke(int trg,int idx=0);
  BtlvPos SeqComFunc_GetTargetPokeSub(int trg);
  //直接取得。基本は上の関数を使って下さい。
  BtlvPos SeqComFunc_GetTargetPoke_Org(int idx){return mComTargetPoke[idx];}

  //複数ターゲットの体数取得
  int SeqComFunc_GetTargetPokeNum(bool isCheck = true);

  //subTargetからTargetへ対してのエフェクト。
  bool SeqComFunc_IsFlipEffect(BtlvPos target,BtlvPos subTarget);
  void SeqComFunc_GetPokeRelativePos( const BTLV_RELARIVE_POKE_OPTION &opt , gfl2::math::Vector3 *retPos , gfl2::math::Vector3 *retRot ,gfl2::math::Vector3 *retScale , bool isCameraAdjust );
  void SeqComFunc_MoveRelativePoke( DemoLib::Obj::Object *obj , int frame , const BTLV_RELARIVE_POKE_OPTION &opt );
  void SeqComFunc_MoveSpecialPos( DemoLib::Obj::Object *obj , int frame , int trgType , gfl2::math::Vector3 &ofs , bool isRotate , bool isFlip );
  void SeqComFunc_GetSpecialPos( int trgType , gfl2::math::Vector3 *retPos , gfl2::math::Vector3 *retRot );
  void SeqComFunc_GetPokeFiledPos( gfl2::math::Vector3 *retPos , gfl2::math::Vector3 *retRot , BtlvPos player , bool isPlayer );

  //指定ポケモンへの回転を取得。縦はフラグで制御
  //retRotに現在の回転を入れておかないと、回転方向の補正がうまく行かない
  void SeqComFunc_CalcPokeDir( const gfl2::math::Vector3 &nowPos , BtlvPos trgPoke , int trgNode , gfl2::math::Vector3 *retRot , bool isVertical );
  void SeqComFunc_CalcPosDir( const gfl2::math::Vector3 &nowPos , const gfl2::math::Vector3 &trgPos  , gfl2::math::Vector3 *retRot , bool isVertical );

  //シーケンスの定義から対象のトレーナーを返す
  int SeqComFunc_GetTargetCharaIndex(int trg);
  BtlvObject_Chara* SeqComFunc_GetTargetChara(int trg);

  //演出対象ボールの取得(IDの0はBALLID_NULLなので注意)
  int SeqComFunc_GetTargetBall(int idx);

  //対象キャラによるパン位置の補正
  f32 SeqComFunc_GetPanOffset( int panType );

  //攻撃モーション分岐処理
  void InitWazaMotionTable(void);
  int SeqComFunc_GetWazaMotionType( int mosnNo , int formNo , int waza , int motionType);

  //バトルのメッセージがMainModuleに依存しているので、捕獲チュートリアルなどで出す場合の関数
  // @param msgId メッセージID(btl_std.h)
  // @param str   表示する文字列
  virtual void SeqComFunc_SetEffectMessage_Std(int msgId , gfl2::str::StrBuf* str);
  virtual void SeqComFunc_SetEffectMessage_Set(int msgId , gfl2::str::StrBuf* str);

  bool IsPrintSequence(void){return true;}
  void SetIsLoadPokeModel(bool flg){mIsLoadPokeModel = flg;}
  bool IsLoadPokeModel(void){return mIsLoadPokeModel;}

  //------------------------------------------------------
  //ポケモンの描画尾ソート
  void SortPokeLayerId(void);

  //------------------------------------------------------
  //サウンド系
  void UpdateSound(void);

  void PlaySe( int seNo , u32 player = Sound::DEFAULT_PLAYER , s32 ctrlId = Sound::NOT_CONTROL );
  bool IsFinishSe( int seNo , s32 ctrlId = Sound::NOT_CONTROL );
  void StopSe( int seNo );

  void PlayVoice( BtlvPos pos , bool isFadeBgm = true , Sound::VoiceType type = Sound::VOICE_TYPE_BATTLE );
  void PlayVoiceId( BtlvPos pos , int id ,bool isFadeBgm = true , Sound::VoiceType type = Sound::VOICE_TYPE_BATTLE );
  void PlayVoice3D( BtlvPos pos , bool isFadeBgm = true , Sound::VoiceType type = Sound::VOICE_TYPE_BATTLE );
  void PlayVoiceId3D( BtlvPos pos , int id ,bool isFadeBgm = true , Sound::VoiceType type = Sound::VOICE_TYPE_BATTLE );
  void PlayVoiceKw( BtlvPos pos , bool isFadeBgm = true );
  void PlayDeadVoice3D( BtlvPos pos , bool isFadeBgm = true , Sound::VoiceType type = Sound::VOICE_TYPE_BATTLE );
  bool IsPlayVoice(void)const{return mIsPlayVoice;}

  void CheckPinchBgmReq(void){mReqCheckPinchBgm = true;}
  void CheckPinchBgm(void);

  //------------------------------------------------------
  //時間更新処理
  void UpdateTime(void);

  //------------------------------------------------------
  //開始時読み込み
  void DataLoadStart(int idx);
  void DataLoadUpdateLoad(int idx);
  bool DataLoadWait(int idx);
  void DataLoadEnd(int idx);

  void DataLoadStartOne(int idx);
  void DataLoadUpdateLoadOne(int idx);

  //天候関連
  bool IsEnableSkyBox(void);

  int GetWeatehrLoadIndex(void);
  int GetWeatehrReverceLoadIndex(void); //逆のIdx
  int GetWeatherResourceIdx(int type , int idx);
  void LoadWeatherResource(int type);
  bool IsFinishLoadWeatherResource(void);
  void StartWeather(int type );
  void StopWeather(int idx);
  void DeleteWeather(int idx);

  void SetVisibleWeather(bool isVisible);
  BtlWeather GetNowWeather(void)const{return mNowWeather;}

  //定義のためにここ
  typedef struct 
  {
    void* mDataBuf[3];

    BtlvObject_Effect *mEffectObj[3];
    BtlvObject_Gfmdl *mModelObj;
  }WEATHER_EFFECT_DATA;
  
  void WeatherFunc_CreateEffect(WEATHER_EFFECT_DATA *data,int resIdx);
  void WeatherFunc_CreateScreenEffect(WEATHER_EFFECT_DATA *data,int resIdx);
  void WeatherFunc_CreateModel(WEATHER_EFFECT_DATA *data);

  //フィールド技関連関連
  void StartLoadFieldWaza(int type);
  bool IsFinishLoadFieldWaza(void);
  void UpdateLoadFieldWaza(void);

  //待機時状態異常
  void Start_WaitCamSickEffect(void);
  void Finish_WaitCamSickEffect(void);
  bool IsFinish_WaitCamSickEffect(void);

  void Update_WaitCamSickEffect(void);

  //------------------------------------------------------
  //取得系
  const BTL_POKEPARAM* DebugGetComBtlPP(void){return mComPokeParam;}
  BTL_ACTION_PARAM*    DebugGetActParam(void){return mComActParam;}
  
  BtlvObject_PokeModel* GetPokeModelObj(int idx){return mPokeModel[idx];}
  BtlvObject_Chara* GetTrainerModelObj(int idx){return mTrainerModel[idx];}
  BtlvObject_Sound* GetSoundObj(int idx){return mSoundObject[idx];}

  EffectObjectManager* GetEffectObjManager(void){return mEffectObjManager;}
  GfmdlObjectManager* GetModelObjManager(void){return mModelObjManager;}
  ClusterObjectManager* GetClusterObjManager(void){return mClusterObjManager;}

  BtlvObject_Gfmdl* GetEdgeMaskModelObj(void){return mEdgeMaskModel;}
  BtlvObject_Gfmdl* GetFloorMaskModelObj(void){return mFloorMaskModel;}
  BtlvObject_Gfmdl* GetBgFadeModelObj(void){return mBgFadeModel;}
  void* GetDigudaStoneResouce(void){return mDigudaStoneResource;}

  DemoLib::Obj::EffectIndexManager* GetParticleIndexManager(void){return mParticleIndexManager;}

  virtual BATTLE_CONVENTION_INFO* GetBattleConventionInfo( u8 id ) const;   // 大会演出情報

  //------------------------------------------------------
  // ポケモン、トレーナー追加情報関連
  PokeExtraInfoStruct* getPokeExtraInfo( int monsNo , int formNo );
  TrainerExtraInfoStruct* getTrainerExtraInfo( int idx );

  int getPokeExtraInfoVersion(void);
  int getTrainerExtraInfoVersion(void);


  // 大会演出開始を表示するモードかどうかの判定
  bool IsConventionDirection(void);

  //待機Bの一斉許可と解除
  void SetAllEnableWaitB(void);
  void ResetAllEnableWaitB(void);

  //フェスの控えトレーナーの表示制御
  void DispFesTrainer(bool flg);

  //デバッグ
  void DumpBattleHeap(void);


protected:

  void GetUiDisplay_Turn( u32* numerator, u32* denominator, u8 myClientID, u8 causePokeID, u32 totalTurn, u32 upTurn, u32 remainTurn, u32 passedTurn ) const;
  void GetUiDisplay_Turn_SideEffect( u32* numerator, u32* denominator, BtlSide side, BtlSideEffect sideEffect ) const;



    //初期化でもらう
  MainModule* mMainModule;
  gfl2::ui::DeviceManager* mDeviceManager;
  gfl2::fs::AsyncFileManager *mAsyncFileManager;

  gfl2::heap::HeapBase* mHeap;
  gfl2::heap::HeapBase* mHeapKeep;  //へそくり
  gfl2::heap::HeapBase* mTempHeap;      //演出用一時ヒープ
  gfl2::heap::HeapBase* mResidentHeap;  //常駐
  gfl2::heap::HeapBase* mPokeHeap[POKE_MODEL_NUM];  //ポケモン用(小さい管理用)
  gfl2::heap::HeapBase* mTrainerHeap[TRAINER_MODEL_NUM];  //トレーナー用
  gfl2::heap::HeapBase* mTrainerCommonHeap;  //トレーナー用(共有ヒープ
  gfl2::heap::HeapBase* mFullPowerHeap;      //全力演出ヒープ
  gfl2::heap::HeapBase* mBgHeap;      //背景用ヒープ

  gfl2::heap::HeapBase* mWeatherHeap[2];  //天候用

  //行動選択中のみ有効なヒープ
  gfl2::heap::HeapBase* mWaitCameraHeap;  //待機カメラ用
  System::nijiAllocator *mWaitCameraGLAllocator;    //常駐
  gfl2::heap::HeapBase* mWaitSickHeap;    //状態異常エフェクト用

  gfl2::heap::NwAllocator *mResidentAllocator;

  System::nijiAllocator *mResidentGLAllocator;    //常駐
  System::nijiAllocator *mTempGLAllocator;  //演出一時用
  System::nijiAllocator *mFullPowerAllocator;  //全力用
  System::nijiAllocator *mPokeGLAllocator[POKE_MODEL_NUM];
  System::nijiAllocator *mTrainerGLAllocator[TRAINER_MODEL_NUM];

  gfl2::Effect::GFHeap  *mEffectHeap;
  gfl2::Effect::System  *mEffectSystem;
  enum
  {
    EFFGRP_NORMAL,
    EFFGRP_SCREEN,
    EFFGRP_SCREEN_BEFORE,
    EFFGRP_NORMAL_LOW,
    EFFGRP_NORMAL_HIGH,

    EFFGRP_MAX,
  };
  gfl2::util::List<u32> *mEffectGroupIDList[EFFGRP_MAX];     ///< グループIDリスト
  
  ::app::util::Heap                *mAppHeap;

  //-------------------------------------------------------
  //内部系システム
  BtlvRenderSystem *mRenderSys;
  BtlvCameraSystem *mCameraSys;
  BtlvSeqSystem       *mSeqSys;

  BgSystem *mBgSystem;
  PokeTool::PokeModelSystem *mPokeModelSys;
  poke_3d::model::CharaModelFactory *mCharaFactory;
  poke_3d::model::DressUpModelResourceManager *mDressUpModelResourceManager;

  const BTL_CLIENT *mBattleClient;
  POKECON *mBattleContainer;
  BtlBagMode mBagMode;
  Savedata::ConfigSave::MSG_SPEED mMsgSpeed;

  //描画系システム
  BtlvUi  *mUiSys;

  //Objectがらみ
  DemoLib::Obj::ObjectManager *mObjectManager;
  DemoLib::Obj::TaskSystem    *mTaskSystem;       //通常のタスクシステム
  DemoLib::Obj::TaskSystem    *mTaskSystemAfterCamera;       //カメラのあとのタスクシステム
  DemoLib::Obj::TaskSystem    *mTaskSystemAlways; //同期待ちとかでも止まらないタスクシステム
  DemoLib::Obj::ResourceManager *mResourceManager;

  static const int POKEMODEL_NUM = POKE_MODEL_NUM;
  static const int TRAINERMODEL_NUM = TRAINER_MODEL_NUM;
  BtlvObject_PokeModel *mPokeModel[POKEMODEL_NUM];  
  BtlvObject_Chara *mTrainerModel[TRAINER_MODEL_NUM];
  BtlvObject_Sound *mSoundObject[BTLV_EFF_SOUND_NUM];
  DemoLib::Obj::EffectIndexManager *mParticleIndexManager;

  poke_3d::model::DressUpParam mTempDressParam;

  GfmdlObjectManager* mModelObjManager;
  EffectObjectManager* mEffectObjManager;
  ClusterObjectManager* mClusterObjManager;

  //特殊処理用
  BtlvObject_Gfmdl *mBgFadeModel;
  BtlvObject_Gfmdl *mEdgeMaskModel;
  BtlvObject_Gfmdl *mFloorMaskModel;
  
  //天候用
  int mWeatehrLoadIdx;
  BtlWeather mNowWeather;

  WEATHER_EFFECT_DATA mWeatherData[2];

  void *mDigudaStoneResource;

  //待機時状態異常
  typedef struct 
  {
    void* mDataBuf;
    BtlvObject_Effect *mEffectObj;

    BtlvPos mOrderArr[POKE_MODEL_NUM];
    int mNowOrder;

    bool mEnable;
    int mSeq;
    int mCnt;
  }WAIT_CAM_SICK_DATA;
  WAIT_CAM_SICK_DATA mWaitCamSickData;



  //-------------------------------------------------------
  int mInitSeq;
  int mTermSeq;
  int mSetupSeq;
  int mSetupGraphicSeq;
  int mCleanupSeq;
  int mSequenceSeq;
  int mPlaySequenceNo;
  bool mIsLoadSequenceResource;
  int mLoadResourceNum; //ResourceManagerが一度に何個読み込むか？
  bool mIsFinishFade; //終了時フェードをしたか？
  bool mIsPlayNushiWin; //ヌシ勝利演出を再生したか？
  bool mIsEnableDraw; //描画可能

  bool mEnableFrameSkip;
  int  mFrameSkipSuspendCnt;

  //初期化処理用
  gfl2::util::List<BtlvLoaderBase*> *mPreLoadList[3];

  //コマンド用
  int mComSeq;
  BTLV_COMMAND_RESULT mComSelResult;

  GameSys::TimeZone::Zone mComTimeZone; //再生中に時間が変わるとまずい
  BtlvPos mComTargetPoke[2]; //複数ありそうなので配列化
  BtlvPos mComTargetDamage[BTL_EXIST_POS_MAX];  //ダメージを食らう相手
  BtlvPos mComTargetDamageBackup[BTL_EXIST_POS_MAX];  //ダメージを食らう相手(全力技スキップ時に退避用
  int mTargetTrainer[BTLV_TARGET_TRAINER_NUM];

  int mEffectBallId[2];  //ボールIDはダブルで2個？
  
  bool mReserveClearResource; //捕獲時にボールを残してるよフラグ
  bool mIsLoadPokeModel;      //ポケモン読み替え中フラグ

  BTL_POKEPARAM const *mComPokeParam;
  BTL_ACTION_PARAM    *mComActParam;
  bool                mMegaSelected;
  const PokeSelParam  *mPokeSelParam;
  PokeSelResult       *mPokeSelResult;

  int mComScene;
  int mComSceneSub;
  int mComCnt;
  int mComEffectId;
  int mComIsVisibleTame;

  bool  mIsMigawariReserve;   //連続技中なので、身代わりを戻す予約
  BtlvPos mComTargetMigawariReserve;  //連続技の身代わり位置退避

  BTLV_WAZA_EFF_PARAM mWazaParam;

  //シーケンスの一時停止状態と待ち関数
  bool mComWaitIsSusspend;
  BTLV_SEQ_WAIF_FUNC mComWaitFunc;

  //捕獲演出用
  int mComItemId;
  int mComRepeatNum;  //繰り返し回数・ボールゆれ回数
  bool mComIsSuccsess;
  bool mComIsCritical;

  bool mIsMigawariWaza;
  bool mIsCommIntro;  //通信対戦導入演出か？(模擬戦のプレイヤーで使うので、フラグか)
  bool mPlayingTrainerIntro;  //トレーナー導入再生中(導入演出中にキャラを暗くしないため)

  //メッセージ
  static const int STR_BUF_NUM = 2;
  gfl2::str::StrBuf *mWorkStr[STR_BUF_NUM];
  print::WordSet* mWordSet;

  //上昇下降エフェクトの連続再生を抑制するため
  BtlvPos mBefPlayEffPos;
  u16 mBefPlayEffNo;

  //サウンド
  static const int VOICE_PLAYER_NUM = 3;
  bool    mIsPlayVoice; //ボリューム戻しのチェック用。ボリュームいじらなければfalse(判定にも使用中
  bool    mIsPlayVoiceFlg[VOICE_PLAYER_NUM];
  bool    mIsPlayVoice3D[VOICE_PLAYER_NUM];
  bool    mIsPikachu5Fade;
  //こっちは技エフェクト用の待ち用のフラグ
  bool    mIsPlayVoiceFlg_Waza[VOICE_PLAYER_NUM];

  bool mReqCheckPinchBgm;
  bool mCanChangePinchBgm;
  bool mIsStartWaitCameraEffect;

  void *mBattleDataPackBuf;
  gfl2::fs::BinLinkerAccessor mBattleDataPack;

  typedef struct
  {
    //実際は13だけどパディングで14
    u8  motIdx[14];

    u16 spWaza;
    u8  spMotIdx;
    u8  padding;
  }WAZA_MOTION_TABLE;

  //技分岐テーブル
  void *mWazaMotionTableBuf;
  u16  *mWazaMotionTableIdx;

  WAZA_MOTION_TABLE *mWazaMotionTable;

  WAZA_ZENRYOKU_PARAM mZenryokuParam;
  bool mReserveResetZenryokuTrainer;  //トレーナーの全力技導入の外部モーションをリセットさせる予約
  int mReserveResetZenryokuTrainerTarget;

  //ポケモン用追加情報
  void *mPokeExtraInfoTableBuf;

  //トレーナー用追加情報
  void *mTrainerExtraInfoTableBuf;

  // バイナリテーブルアクセス関連
  bool checkBinaryTableMAGIC( void *buf )const;
  void* getBinaryTableRow( void *buf, int idx )const;
  int getBinaryTableVersion ( void *buf )const;
  int getBinaryTableNum( void *buf )const;

  // 3DS逆さ判定クラス
  System::DeviceTurnWatcher *mTurnWatcher;

  //フィールド技
  bool mIsEnableFieldWaza;
  bool mIsLoadingFiledWaza;
  int  mFieldWazaSeq;
  int  mFieldWazaType;

  //----------------------------------------
  //バトルレコーダー関係
  bool mRec_IsSkip;       //スキップ中
  bool mRec_IsStartSkip;  //スキップ開始の暗転中


  //----------------------------------------
  //捕獲チュートリアルと全力技チュートリアルの分岐
  bool mIsZenryokuTutorial;

  //----------------------------------------
  //ロトム図鑑演出用

  //そのバトルで見たポケモンのMonsNo 乱入が13枠なので合わせて14あれば良い。
  static const int LOOKED_MONSNO_NUM = 16;
  u16 mLookedMonsNo[LOOKED_MONSNO_NUM];

#if PM_DEBUG
  BTLV_DEBUG_PARAM *mDebugParam;
  //デバッグ用
public:
  bool mDebugDispLyt;
  bool mDebugPrintTick;
  int  mDebugPrintTickCnt;
  int  mDebugPrintTickInterval;
  bool mDebugTickCheck;
  bool mDebugTickCheckSe;
  int mDebugTickLimitCPU;
  int mDebugTickLimitGPU;

  int  mDebugObjPreset;
  int  mDebugEdgeMode;
  bool mIsEnableWaitCamera;
  bool mDebugDispBg;
  bool mDebugDispWeather;
  bool mDebugDispMask;

  int mDebugRenderPathCheck;

  bool mDebugUseLight;
  gfl2::math::Vector3 mDebugModelLightVec;
  gfl2::math::Vector3 mDebugCharaLightVec[4];
  gfl2::math::Vector3 mDebugPokeLightVec[4];

  gfl2::math::Vector4 mDebugFPRimCol;

  // For Wait camera
  int mDebugWaitCamDataIdx;
  int mDebugWaitCamHeatLevel;
  int mDebugWaitCamTargetPokemon;
  int mDebugWaitCamTargetTrainer;
  int mDebugWaitCamTargetPokemon_end;
  int mDebugWaitCamTargetTrainer_end;
  int mDebugWaitCamReverse;
  int mDebugWaitCamStartWaitFrame;
  int mDebugWaitCamFirstPlayDataIdx;

  //オーラ上書き
  bool mDebugFPEnableSetting;
  f32  mDebugFPRimColorR;
  f32  mDebugFPRimColorG;
  f32  mDebugFPRimColorB;
  f32  mDebugFPRimPow;
  f32  mDebugFPRimWidth;
  u32  mDebugFPRimMoveSpeed;
  f32  mDebugFPRimMoveWidth;

  f32 mDebugFPAuraColorR;
  f32 mDebugFPAuraColorG;
  f32 mDebugFPAuraColorB;
  f32 mDebugFPAuraScaleX;
  f32 mDebugFPAuraScaleY;
  u32 mDebugFPAuraDensity;
  f32 mDebugFPAuraBaseScaleX;
  f32 mDebugFPAuraBaseScaleY;

  BTLV_DEBUG_PARAM* GetDebugParam(void){return mDebugParam;}
  void SetDebugParam(BTLV_DEBUG_PARAM* param){mDebugParam = param;}

#endif


};

GFL_NAMESPACE_END(btl)


#endif // __BTLV_CORE_H__
