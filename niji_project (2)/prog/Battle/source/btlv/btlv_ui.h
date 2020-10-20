//======================================================================
/**
 * @file  btlv_ui.h
 * @brief バトル描画 ユーザーインターフェース管理
 * @author  ariizumi
 * @data  10/11/15
 */
//======================================================================
#ifndef __BTLV_UI_H__
#define __BTLV_UI_H__

#include "../btl_common.h"

#include "btlv_types.h"
#include "btlv_core.h"
#include "btlv_gauge_status.h"
#include "btlv_gauge_ball.h"
#include "btlv_gauge_levelup.h"
#include "btlv_gauge_tokusei.h"
#include "btlv_convention_win.h"
#include "btlv_gauge_timer.h"
#include "btlv_gauge_shotdown.h"

#include "applib/include/Frame/AppFrameListener.h"

#include "Battle/source/btlv/BtlvUIActSelectFrame.h"
#include "Battle/source/btlv/BtlvUIYesNoFrame.h"
#include "Battle/source/btlv/BtlvUISkillSelectFrame.h"
#include "Battle/source/btlv/BtlvUIBGFrame.h"
#include "Battle/source/btlv/BtlvUITargetSelectFrame.h"
#include "Battle/source/btlv/BtlvUIItemConfirmFrame.h"
#include "Battle/source/btlv/BtlvUIPokeListFrame.h"
#include "Battle/source/btlv/BtlvUIIntroEventFrame.h"

#include "App/PokeList/include/PokeListAppParam.h"
#include "App/PokeList/source/PokeListSimpleFrame.h"

#include "App/Bag/include/BagFrame.h"
#include "App/Bag/include/BagAppParam.h"

#include "App/WazaOshie/include/WazaOshieFrame.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)

  class Manager;

GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;
GFL_NAMESPACE_BEGIN(app)
class BtlvUiMsgWin;
class BtlvUiRotom;
GFL_NAMESPACE_END(app)

//  アイテムでボールを使用する時の使用状態
enum ItemBallUseStatus
{
  //  使用可能
  BBAG_BALLUSE_NORMAL,

  //  先頭のポケモン行動時しか投げられない
  BBAG_BALLUSE_NOT_FIRST,
  
  // 手持ち・ボックスが満杯で投げられない
  BBAG_BALLUSE_POKEMAX,

  // 場に２体以上いるなら投げられない
  BBAG_BALLUSE_DOUBLE,

  // 場のポケモンが（そらをとぶなどで）消えてるなら投げられない
  BBAG_BALLUSE_POKE_NONE,

  // 相手が"ぬし"なので投げられない
  BBAG_BALLUSE_VS_NUSI,

  // 試練中なので投げられない
  BBAG_BALLUSE_SIREN,
  
  // 保護区なので投げられない
  BBAG_BALLUSE_SANCTUARY,

  // 捕獲不可戦闘なので投げられない
  BBAG_BALLUSE_NO_CAPTURE,
};

//  下画面の導入演出のON
//#define BTL_UI_LOWER_INTRO_EVENT_ON

class BtlvUi : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN( BtlvUi );
public:
  //  UIフレームの状態一覧
  enum UIFrameState
  {
    //  フレームが存在しない
    UI_FRAME_STAET_NOT_EXIST  = 0,
    //  演出中
    UI_FRAME_STAET_EVENT_RUN,

    //  待機状態(この状態で外部から設定可能)
    UI_FRAME_STAET_IDLE
  };

  BtlvUi(BattleViewSystem* core);
  ~BtlvUi();

  //  バッグ動作のリザルトデータ構造体
  typedef struct _tag_bag_result_data_
  {
    //  使用アイテムID
    s32 useItemId;

    //  使用アイテムを反映させるポケモンidx
    s32 targetPokeIndex;

    //  使用アイテムを反映させるわざidx
    s32 targetWazaIndex;

    _tag_bag_result_data_()
    {
      Clear();
    }

    void Clear()
    {
      useItemId       = ITEM_DUMMY_DATA;
      targetPokeIndex = -1;
      targetWazaIndex = -1;

    }

  } ITEM_PROC_RESULT_DATA;


  //初期化(バトルProc開始時)
  bool Initialize( Savedata::ConfigSave::MSG_SPEED msgSpeed );

  //解放して良いか？
  bool CanFinalize(void);
  //解放(バトルProc終了時)
  bool Finalize(void);
  //開始時
  bool Setup(void);
  //終了時
  bool CleanUp(void);
  //更新
  void Update(void);
  //更新
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);
  
  //メッセージ読み込み
  bool LoadMessage(void);

  //行動選択一式の開始
  bool InitializeActSelect(void);

  //------------------------------------------------------------------
  /**
    * @brief   行動選択終了
    * @return  true -> 選択終了 / false -> 選択が終了していない
    * @note
    *          選択終了時には行動選択と背景のフレームは死んでいないとだめ
    *          ここできちんと画面の後始末ができているかチェックする
    *
    *          現在 BtlUIDataManaagerクラスで管理している全ファイルロードデータのチェックおよび開放をしている。
    *          理由としてはこのファイルデータをつかうケースが行動選択中で使うフレームしか使わない想定で作っているから
    *          もしそれ以外で使ってしまうと例外エラーのハングになる可能性があるので注意
    */
  //------------------------------------------------------------------
  bool FinalizeActSelect();

  //取得系

  //  行動選択のフレーム状態を取得
  UIFrameState GetStateActSelectFrame();
  //  わざ選択のフレーム状態を取得
  UIFrameState GetStateSkillSelectFrame();
  //  bagフレーム状態を取得
  UIFrameState GetStateBagFrame();

  //動作系

  //コマンド選択
  void Start_SelectCommand( u8 poke_index, bool returnable );
  void StartAnime_SelectCommand();
  BTLV_COMMAND_RESULT WaitAnime_SelectCommand(void);
  void EndAnime_SelectCommand(void);

  //------------------------------------------------------------------
  /**
    * @brief   行動選択のコマンド通知
    * @return  true -> 通知成功 / false -> 通知失敗
    * @note
    *   行動選択フレームが待機中のみ有効
    */
  //------------------------------------------------------------------
  bool PushCmdActSelectCommnad( const BattleUIActSelectFrame::CmdType type );

  void ForceQuit_SelectCommand(void);
  void ForceSelect_Attack();
  void ForceSelect_Bag();

  //技選択
  void Start_SelectSkill( u8 poke_index );
  void StartAnime_SelectSkill( u8 poke_index );
  bool WaitAnime_SelectSkill(void);
  void GetResult_SelectSkill(BTLV_COMMAND_RESULT* pOutResult, bool* pOutUseMega, bool* pOutUseAuraWaza, bool *pOutUseUltraBurst);
	bool GetResult_SelectInfo(void);
  bool EndAnime_SelectSkill(void);
  void ForceQuit_SelectSkill(void);
  void Reset_SelectSkill(void); //選びなおし
  void ForceSelect_Skill();

  //------------------------------------------------------------------
  /**
    * @brief   わざ選択のコマンド通知
    * @return  true -> 通知成功 / false -> 通知失敗
    * @note
    *   わざ選択フレームが待機中のみ有効
    */
  //------------------------------------------------------------------
  bool PushCmdSkillSelectCommnad( const BattleUISkillSelectFrame::CmdType type );

  void Start_WazaWasure( u8 pos, WazaID waza );
  BTLV_COMMAND_RESULT WaitAnime_WazaWasure(void);
  void WaitAnime_EndWasure(void);

  //下画面２択
  void StartAnime_SelectYesNo( bool b_cancel, YesNoMode yesno_mode );
  BTLV_COMMAND_RESULT WaitAnime_SelectYesNo(void);
  void ForceQuit_SelectYestNo(void);
  void EndAnime_SelectYesNo(void);

  //  ポケモンリスト( トップ画面からのポケモンボタンを押した時の入れ替え時に呼ぶ「単体でのポケモン入れ替え」)
  void StartAnime_PokeList( const PokeSelParam* param, bool canEsc, const BTL_POKEPARAM* outMemberPokeParam, u8 poke_index, bool* sashiosae );
  BTLV_COMMAND_RESULT WaitAnime_PokeList( PokeSelResult* select_result );
  void EndAnime_PokeList(void);
  void ForceQuit_PokeList(void);

  void StartAnime_WazaInfoView(u8 pokeIdx, u8 wazaIdx);

  //  ポケモン選択( ポケモン気絶やとんば返りなどで入れ替えが発生した時に呼ばれる )
  void StartAnime_SelectPoke(const PokeSelParam* param, bool canEsc, int outMemberIndex, PokeSelResult* pResult );
  BTLV_COMMAND_RESULT WaitAnime_SelectPoke( PokeSelResult* result );
  void EndAnime_SelectPoke(void);
  void ForceQuit_SelectPoke(void);

  //バッグ
  void StartAnime_Bag( const ItemBallUseStatus ballusemode, bool* sashiosae);
  BTLV_COMMAND_RESULT WaitAnime_Bag(void);
  void EndAnime_Bag(void);
  void ForceQuit_Bag(void);
  void ForceSelect_BallCategory();
  void ForceSelect_ItemTopLeft();
  void ForceSelect_ItemUse();

  //  バッグ処理リザルト取得
  const ITEM_PROC_RESULT_DATA GetResultDataBag() const { return m_itemResultData; }

  //ターゲット選択
  void StartAnime_Target( u8 poke_index, const BTL_POKEPARAM* poke_param, WazaNo wazano );
  BTLV_COMMAND_RESULT WaitAnime_Target(void);
  void EndAnime_Target(void);
  void ForceQuit_Target(void);
  void UpdateHighLight_Target( void );

  //レベアルップ
  void Start_LevelUp( const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo );
  bool IsDispLevelUp(void);
  void Step_LevelUp( void );
  void End_LevelUp( void );

  //-----------------------------------------------------
  //パーツ制御
  //-----------------------------------------------------

  //6末用
  void AllInGauge(void);
  bool WaitAllInGauge(void);

  //ゲージ全体位置の制御
  void SetGaugePosStart(void);
  void SetGaugePosDispSelf(void); //全部出す
  void SetGaugePosDispEnemy(void);
  void SetGaugePosBaseSelf(void); //ボールだけ出す
  void SetGaugePosBaseEnemy(void);

  //ゲージ単体制御
  void DispGauge( BtlvPos viewPos , bool isUpdate = false );
  void HideGauge( BtlvPos viewPos );
  void DispGaugeAll(void);
  void HideGaugeAll(void);
  bool IsDispGauge( BtlvPos viewPos );

  //メッセージウィンドウの表示と合わせて制御するよう
  void SetDispGauge_MsgWin(bool flg );

  void StrtBallBarAnime(BtlvPos viewPos );

  void DispBallBarAll(void);
  void HideBallBarAll(void);
  void UpdateBallBarAll(void);

  void DispShotDownBarAll(void);
  void HideShotDownBarAll(void);

  //bool WaitHpGaugeAnime( const bool isEnemy );
  //ゲージ更新リクエスト 基本こっち
  void GaugeUpdateReq( void );
  //ゲージ強制更新 特殊な場合のみ(バトルビデオスキップ終わりのフェード中。
  void GaugeUpdateForce( void );
  void GaugeUpdate_Anime( void );

  void ResetGaugeParam( BtlvPos viewPos );
  void StartGaugeDamage( BtlvPos viewPos , int cnt );
  bool IsGaugeDamage(void);
  void PlayBatugunAnime( BtlvPos viewPos );

  void GaugeExpAnime( BtlvPos viewPos , u32 value );
  void GaugeExpAnime_Lvup( BtlvPos viewPos );
  void GaugeSetSick( BtlvPos viewPos , PokeSick sick);
  
  bool IsAnimeGauge(void);
  bool IsPinch( BtlvPos viewPos );

  //特性Win
  void DispTokWin( const BtlvPos viewPos );
  bool WaitDispTokWin(void);
  void HideTokWin( const BtlvPos viewPos );
  bool WaitHideTokWin(void);
  bool IsDispTokWin( const BtlvPos viewPos );
  BtlvPos GetTokWinDispPos(const BtlvPos viewPos );

  //-----------------------------------------------------
  //メッセージ
  //-----------------------------------------------------
  gfl2::str::MsgData * GetWazaInfoMessage(void);
  //メッセージの表示
  void StartDispMessage( const BTLV_STRPARAM *param, bool isKeyWait = false );
  void StartDispMessage( const gfl2::str::StrBuf *str, bool isKeyWait = false );
  void StartDispMessageEx( const gfl2::str::StrBuf *str );
  void StartDispMessageDummy( wchar_t *msg );
  void SetMessageCallBack( BTL_CLIENT* callbackTarget );
  //メッセージ自動送り　StartDispMessageの次に呼ぶ！
  void SetAutoStepMessage( bool flg );
  //今表示しているメッセージだけに有効なメッセージ速度変更
  //トレーナーの繰り出しメッセージの時に、遅いの時だけ普通にする。
  void SetMessageSpeedForce_Normal(void);

  // メッセージカーソルの表示・非表示
  void SetVisibleMsgCursor( bool isVisible );

  void HideMessage(void);
  bool IsUpdateMessage(void);
  bool IsVisibleMessage(void)const;
  bool IsOneLineMessage(void)const;
  void UpdateMessage(void);
  void DrawMessage(gfl2::gfx::CtrDisplayNo displayNo);

  // ステータス用メッセージデータの取得 
  gfl2::str::MsgData* GetStatusMessageData() const 
    {
      return( m_pBtlParamAccesser->GetStatusMessageData() ); 
    }

  gfl2::str::MsgData* GetAppMessageData() const
  {
    return(m_pBtlParamAccesser->GetAppMesssageData());
  }

  //  カーソルリセット
  void ResetCursorPos( const u32 pokeIdx );

  //敵側か味方側か？
  bool BtlvPosIsEnemySide( BtlvPos viewPos );

  //-----------------------------------------------------
  //タイマー
  //-----------------------------------------------------
  void SetTimerTime( int game_time, int client_time, int command_time );
  void DispTimer( BTLV_TIMER_TYPE type , bool isReset, bool isCountDownStart );
  void HideTimer( BTLV_TIMER_TYPE type );
  bool IsFinishTimer( BTLV_TIMER_TYPE type );
  u32 GetClientTime( u8 clientID );
  void SetClientTime( u8 clientID, u32 time );

  //-----------------------------------------------------
  //特殊
  //-----------------------------------------------------
  void StartNaderuCheck(void);
  bool EndNaderuCheck(void);
  bool IsNaderuCheck(void);

  void SetPassiveAnime( u32 anm );
  void DispWaitCommStr( bool flg);

  //-----------------------------------------------------
  //おていれ
  //-----------------------------------------------------
  void DispOteire();

  //-----------------------------------------------------
  //ロトム演出
  //-----------------------------------------------------
  void Rotom_StartLoad(void);
  bool Rotom_IsFinishLoad(void);
  void Rotom_RemoveLayout(void);
  void Rotom_Delete(void);

  void Rotom_StartAnimation(int type);
  void Rotom_SetData(const BTL_POKEPARAM *bpp);

  //-----------------------------------------------------
  //バトル開始導入イベント
  //-----------------------------------------------------
  //------------------------------------------------------------------
  /**
    * @brief   導入イベント開始
    */
  //------------------------------------------------------------------
  void StartFirstIntroEvent();

  //------------------------------------------------------------------
  /**
    * @brief   導入イベント待ち
    * @return  true -> 待ち継続 / false -> 待ち終了
    */
  //------------------------------------------------------------------
  bool WaitFirstIntroEvent();

  //-----------------------------------------------------
  //大会演出
  //-----------------------------------------------------
  void StartConventionDirection( void );
  void EndConventionDirection( void );

  //-----------------------------------------------------
  //ロイヤル用順位表示
  //-----------------------------------------------------
  void StartDispRoyalRanking(void);
  bool IsFinishDispRoyalRanking(void);
  void StartHideRoyalRanking(void);
  bool IsFinishHideRoyalRanking(void);

  //------------------------------------------------------------------
  /**
   * @brief  上画面のメッセージの文字列バッファ取得
   */
  //------------------------------------------------------------------
  gfl2::str::StrBuf*  GetDispMessageStrBuf( const u8 i )
  {
    GFL_ASSERT( i < GFL_NELEMS( m_workStr ) );

    return  m_workStr[ i ];
  }

  //------------------------------------------------------------------
  /**
   * @brief  FrameMangerに登録して実行中のFrameのEndFuncが完了した直後に呼び出される関数
   */
  //------------------------------------------------------------------
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );

  //  バトル再生下画面Viewを取得
  app::BattleUIRecPlayLowerView*  GetUIRecPlayLowerView();

  //  下画面の暗転表示設定
  void SetEnableBGFilter( const bool bFlg );

private:
  void StrParamToString( const BTLV_STRPARAM* param, gfl2::str::StrBuf* dst );

  void UpdateNaderuCheck(void);

  // 入れ替えが起きたかどうかのチェック
  void UpdateExchangeInfo();
  bool IsExchangeOccurred(int pos);

  bool IsExchangeOccurredAll(int index);

  //  アクション用の全フレームを終了させる
  void _ForceQuitActFrame();

private:
  friend class PokeListProc;
  friend class ItemProc;

  //  終了時のステート
  enum CleanUpState
  {
    CLEANUP_STATE_FADE_OUT = 0,
    CLEANUP_STATE_FRONT_FRAME_END,
    CLEANUP_STATE_NOTICE_OTEIRE,
    CLEANUP_STATE_END_FRAME,
    CLEANUP_STATE_DEL_UI_DATA,
  };

  //  アイテム処理構造体
  class ItemProc
  {
  public:
    //  アイテムのフロータイプ
    enum ITEM_FLOW_TYPE
    {
      ITEM_FLOW_TYPE_NONE                 = -1,
      //  通常のバッグ
      ITEM_FLOW_TYPE_BAG                  = 0,
      //  行動選択のショートカットボール使用
      ITEM_FLOW_TYPE_USE_BALL_SHORT_CUT,
      //  ターゲット選択しているポケモン
      ITEM_FLOW_TYPE_TARGET_POKE,
      //  強制破棄(Die関数が呼ばれるとこのフロー)
      ITEM_FLOW_TYPE_DIE,
    };

    ItemProc( BtlvUi* pParentBtlvUI )
    {
      GFL_ASSERT_STOP( pParentBtlvUI );

      _Clear();

      m_pParentUI = pParentBtlvUI;
    }

    //  処理開始の事前処理

    //  すでにアイテム指定対象のポケモンが選択されてる用
    void PreStart( const ITEM_FLOW_TYPE floeType, const btl::BTL_PARTY* pSelectParty );

    //  通常用
    void PreStart( const btl::BTL_PARTY* pSelectParty, const u8 in_pokeListIdx );

    //------------------------------------------------------------------
    /**
     * @brief 処理開始
     * @param ballusemode : ボールアイテム使用状態
     * @param pRootFrameActSelect : フレームルートが行動選択の場合には設定
     * @note
          ItemProcクラスはPokeListProc内でも使われる。( ※ポケモンステータスの「回復する」選択したときに呼び出す )
          PokeListProc内で使用する場合は、フレームのルートは行動選択ではなく、ポケモンリストフレームになる。
          この場合は pUnderActSelectFrame = NULL を指定。

          ※なぜこういったことをしているか
            PokeListProc内で行動選択のマスク表示設定をしているが、ItemProc内でも行動選択のマスク表示設定をしている。
            しかしPokeListProc内でItemProcを呼び出すとItemProc::End()を呼ばれることがあるので、この時にPokeListProc::Start()で設定したマスクが解除されるのを防ぐため
     */
    //------------------------------------------------------------------ 
    void Start( const ItemBallUseStatus ballusemode, BattleUIActSelectFrame* pRootFrameActSelect );

    //  終了
    void End();

    //  処理強制終了
    void Die();

    //  更新
    BTLV_COMMAND_RESULT Update();

    //  アイテム処理のリザルト取得
    const ITEM_PROC_RESULT_DATA GetResult() const { return m_resutlData; }

    //  アイテム関連のフレームが存在しているか
    bool  IsExistFrames();

    //  アイテム関連のフレームで演出か
    bool  IsEventFrames();

  private:
    enum BagState
    {
      //  バッグの道具選択前処理(バッグ画面は全体表示なのでバトルのレイアウトはいらないのでバトルレイアウトを破棄する)
      BAG_STATE_START = 0,

      //  バッグフレーム処理(道具選択・対象選択を含める)
      BAG_STATE_FRAME,

      //  バッグで選んだ道具の使用説明
      BAG_STATE_ITEM_CONFOIFM,

      //  バッグ処理終了(バトルレイアウトを復帰させる)
      BAG_STATE_PROC_CANCEL,

      //  アイテム使用
      BAG_STATE_ITEM_USE,
    };

    void _Reset()
    {
      m_flowType              = ITEM_FLOW_TYPE_NONE;
      m_updateSubState        = -1;
      m_pocketMax             = 0;
      m_pRootFrameActSelect   = NULL;
      m_activePokeListIdx     = 0;

      m_resutlData.Clear();
    }

    void _Clear()
    {
      m_flowType              = ITEM_FLOW_TYPE_NONE;
      m_pParentUI             = NULL;
      m_pSelectParty          = NULL;
      m_pRootFrameActSelect   = NULL;
      m_updateSubState        = -1;
      m_activePokeListIdx     = 0;

      m_resutlData.Clear();
    }

    //  バッグフレーム作成
    void _CreateBagFrame();

    //  アイテムがバトル用か判定
    bool _IsBattleItem( const u32 itemId );

    //  指定したアイテムidにバッグ画面のカーソル位置を合わせるように更新
    void _UpdateBagItemCursorPos( u32 itemId, ::app::bag::APP_PARAM* pBagAppParam );

    //  バッグ更新用のサブステート
    void _SetUpdateSubStateByBag( const BagState newState );

    //  使用アイテム対象選択サブステート
    void _SetUpdateSubSteteByBagPokeTargetSelect();

    ITEM_FLOW_TYPE        m_flowType;
    ITEM_PROC_RESULT_DATA m_resutlData;
    BtlvUi*               m_pParentUI;

    s32                   m_updateSubState;
    u16                   m_pocketMax;
    //  アイテム設定対象のポケモンパーティー
    const btl::BTL_PARTY* m_pSelectParty;

    BattleUIActSelectFrame* m_pRootFrameActSelect;

    //  ※ m_floeType == ITEM_FLOW_TYPE_TARGET_POKE の時のみ有効
    u8                    m_activePokeListIdx;
  };

  //  ポケモンリスト処理制御
  //  クラスにしたのは行動選択からの入れ替えと気絶した時の入れ替えとでは呼ばれるコマンドが異なるので、共通処理にできなかったので
  //  こちらでクラスを作って共通処理にした。
  class PokeListProc
  {
  public:
    //  ポケモンリストのステート
    enum ProcListState
    {
      //  ポケモンリストへ
      POKE_LIST_STATE_SELECT  = 0,

      //  ステータスへ遷移する前の事前準備（バトルのレイアウトを終了させる。ポケステータス画面は全体表示なのでバトルのレイアウトはいらない）
      POKE_LIST_STATE_PRE_STATUS,

      POKE_LIST_STATE_STATUS,

      POKE_LIST_STATE_PROC_CANCEL,

      //  アイテム処理へ遷移
      POKE_LIST_STATE_PROC_ITEM
    };

    //  ProcMode
    enum procMode
    {
      //  ポケモンリスト(行動選択からの遷移の時に設定)
      PROC_MODE_LIST = 0,

      //  ポケモン入れ替え（気絶、技のとんぼ帰り、敵ポケを倒した後の入れ替えで指定）
      PROC_MODE_CHANGE,
    };

    PokeListProc( BtlvUi* pParentBtlvUI )
    : m_itemProc( pParentBtlvUI )
    {
      GFL_ASSERT_STOP( pParentBtlvUI );

      _Clear();

      m_pParentBtlvUI = pParentBtlvUI;
    }

    //  処理開始
    //  この関数は単体入れ替え用（トップ画面からのポケモン選択で使う）
    void StartBySelectChange( const u8 outPokeIdx, const BTL_POKEPARAM* pOutputPokeParam, const bool bPokeChange );

    //  この関数はきぜつやとんぼ返りなどの割り込みで起きた時用
    void StartByForceChange( const u8 outPokeIdx, const bool bProcCancel, const bool bPokeChange, PokeSelResult* pResult );

    //  処理終了
    void End();

    //  処理を殺す
    void Die();

    //  処理更新
    BTLV_COMMAND_RESULT Update( PokeSelResult* pPokeSelectResult );

  private:
    void _StartBase( PokeSelResult* pResult );

    //  処理開始時に必ず呼ぶ初期化(処理が開始するたびに呼ぶ)
    void _Reset()
    {
      BattleUIPokeListFrame::APP_PARAM::_tag_in_param* pPokeListInData  = &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.in_param;
      pPokeListInData->pFirstCurrentPokeParam = NULL;
      pPokeListInData->firstButtonCursorPos   = BattleUIPokeListFrame::FIRST_BUTTON_CURSOR_POS_NONE;

      m_state         = POKE_LIST_STATE_SELECT;
      m_procMode      = PROC_MODE_LIST;
      m_outPokeIdx    = 0;
      m_bProcCancel   = false;
      m_bPokeChange   = false;
      m_pFirstExChangeSourcePokeParam = NULL;
    }

    //  変数初期化(インスタンス生成時のしょっぱなだけ)
    void _Clear()
    {
      m_state         = POKE_LIST_STATE_SELECT;
      m_procMode      = PROC_MODE_LIST;
      m_pParentBtlvUI = NULL;
      m_outPokeIdx    = 0;
      m_bProcCancel   = false;
      m_bPokeChange   = false;
      m_pFirstExChangeSourcePokeParam = NULL;

      m_selectPokeParty.Initialize();
    }

    void _SetState( const ProcListState newState, PokeSelResult* pPokeSelectResult );

    ProcListState   m_state;
    procMode        m_procMode;

    BtlvUi*         m_pParentBtlvUI;
    u8              m_outPokeIdx;

    //  入れ替え元の先頭のポケモンパラメータ
    const BTL_POKEPARAM*  m_pFirstExChangeSourcePokeParam;

    //  アイテム処理
    ItemProc        m_itemProc;

    //  処理をキャンセルして、元の画面へ遷移するのを禁止
    bool            m_bProcCancel;

    //  ポケモン入れ替え可能か
    bool            m_bPokeChange;

    //  バッグ画面時に渡すポケモンパーティー
    btl::BTL_PARTY  m_selectPokeParty;
  };

  //  行動選択フレーム作成
  void _CreateSelectActFrame( const bool bRetFlg );

  BattleViewSystem* m_btlvCore;

  //  下画面UIフレーム生成用のローカルメモリ
  gfl2::heap::HeapBase *m_pUILowerManagerLocalHeap;

  //  下画面UIフレーム管理
  applib::frame::Manager*               m_pUILowerFrameManager;
  //  LowerFrameの手前に出すフレーム
  //  このフレームを使っている時はLowerFrameの更新と描画がOFF
  applib::frame::Manager*               m_pUILowerFrontFrameManager;

  //  行動選択Frameアプリ用のパラメータ(入力・設定・結果の出力を持つ)
  BattleUIActSelectFrame::APP_PARAM     m_uiActSelectFrameAppParam;

  //  2選択Frameアプリ用のパラメータ(入力・結果の出力を持つ)
  BattleUIYesNoFrame::APP_PARAM         m_uiYesNoFrameAppParam;

  //  技選択Frameアプリ用のパラメータ
  BattleUISkillSelectFrame::APP_PARAM   m_uiSkillSelectFrameAppFrame;

  //  背景Frameアプリ用のパラメータ
  BattleUIBGFrame::APP_PARAM            m_uiBGFrameAppParam;

  //  対象選択Frameアプリ用のパラメータ
  BattleUITargetSelectFrame::APP_PARAM  m_uiTargetSelectFrameAppParam;

  //  ポケモンリストFrameアプリ用のパラメータ
  App::PokeList::APP_PARAM              m_uiPokeListFrameAppParam;
  App::PokeList::CONTEXT_PARAM          m_uiPokeListFrameContextParam;

  //  バッグFrameアプリ用のパラメータ
  ::app::bag::APP_PARAM                 m_uiBagFrameAppParam;

  //  バトルポケモンリストFrameアプリ用のパラメータ
  BattleUIPokeListFrame::APP_PARAM      m_uiBattlePokeListFrameAppParam;

  //  アイテム確認Frameアプリ用のパラメータ
  BattleUIItemConfirmFrame::APP_PARAM   m_uiItemConfirmFrameAppParam;

  //  導入イベントFrameアプリ用のパラメータ
  BattleUIIntroEventFrame::APP_PARAM    m_uiIntroEventFrameAppParam;

  //  わざ忘れフレームのアプリ用のパラメータ
  App::WazaOshie::APP_PARAM             m_uiWazaOshieFrameAppParam;

  //  わざ忘れフレームのアプリ用のセットアップパラメータ
  App::WazaOshie::WazaOshieFrame::SetupParam   m_uiWazaOshieFrameSetupParam;

  //  行動選択フレーム
  BattleUIActSelectFrame*               m_pUIActSelectFrame;

  //  ２選択フレーム
  BattleUIYesNoFrame*                   m_pUIYesNoFrame;

  //  技選択フレーム
  BattleUISkillSelectFrame*             m_pUISkillSelectFrame;

  //  背景フレーム
  BattleUIBGFrame*                      m_pUIBGFrame;

  //  対象選択フレーム
  BattleUITargetSelectFrame*            m_pUITargetSelectFrame;

  //  ポケリストフレーム
  App::PokeList::PokeListSimpleFrame*   m_pUIPokeListFrame;

  //  バッグフレーム
  ::app::bag::BagFrame*                 m_pUIBagFrame;

  //  バトルポケリストフレーム
  BattleUIPokeListFrame*                m_pUIBattlePokeListFrame;

  //  アイテム確認フレーム
  BattleUIItemConfirmFrame*             m_pUIItemConfirmFrame;

  //  わざ忘れフレーム
  App::WazaOshie::WazaOshieFrame*       m_pWazaOshieFrame;

#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
  //  バトル導入イベントフレーム
  BattleUIIntroEventFrame*              m_pUIIntroEventFrame;
#endif

  //  バトルで共通で使用するパラメータ
  app::BattleUIDataManager*             m_pBtlParamAccesser;

  bool            m_isInitialize;
  bool            m_bSetup;
  bool            m_bCleanUp;
  bool            m_bReadlBtlParamAccesser;
  int             m_forceFinalizeStep;

  u8              m_subStep;

  //btl_appのメッセージリソース
  void*           m_appMessageBuf;

  //  アイテム処理
  ItemProc        m_itemProc;

  //  ポケリスト処理
  PokeListProc     m_pokeListProc;

  //  終了時のステート
  CleanUpState    m_cleanUpState;

  //  アイテム使用結果(バッグとポケモンリストから更新される)
  ITEM_PROC_RESULT_DATA m_itemResultData;

  //バトルメッセージ
  app::BtlvUiMsgWin* m_pUiMsgWin;
  //バトルメッセージの引数に渡すHEAP
  ::app::util::Heap m_AppHeap;

  //  UI全般で使用する非常駐用のHeap
  ::app::util::Heap m_appTempHeap;
  //  非常駐のheapが参照するアロケータ
  gfl2::heap::NwAllocator*      m_pHeapTempAllocator;
  gfl2::heap::NwAllocator*      m_pDevHeapTempAllocator;

  gfl2::util::GLHeapAllocator*  m_pGLHeapTempAllocator;
  gfl2::util::GLHeapAllocator*  m_pDevGLHeapTempAllocator;

  //  バトルメッセージの文字列バッファ
  gfl2::str::StrBuf *m_workStr[1];

  //ゲージ類
  BtlvGaugeBase *m_gaugeStatus[BTL_EXIST_POS_MAX];
  //ボールバー
  BtlvGaugeBall *m_gaugeBall[4];
  // レベルアップ表示
  BtlvGaugeLevelUp *m_gaugeLevelUp;
  // 特性表示
  BtlvGaugeTokusei *m_gaugeTokusei;
  // 大会演出ウィンドウ表示
  BtlvConventionWin *m_conventionWin;
  // 対戦時間表示
  BtlvGaugeTimer *m_gaugeTimer;
  // ロイヤル用撃墜数表示
  BtlvGaugeShotDown *m_gaugeShotDown[4];



  //ロイヤル順位保存用
  u8 m_rolayRankingAnime[4];

  //  入れ替えチェック用
  BTL_POKEPARAM* m_pPosBackup[ pml::PokeParty::MAX_MEMBERS ];

  //  お手入れを通知しているか
  bool  m_bNotifyOteire;

  //  下画面フェードを常にリセットするか
  bool  m_bLowerFadeReset;

  //ロトム演出
  app::BtlvUiRotom* m_pUiRotom;

  //6末用
  ::app::util::G2DUtil* mGaugeG2dUtil;
  gfl2::lyt::LytWk* mGaugeLyt;

};

GFL_NAMESPACE_END(btl)



#endif //__BTLV_UI_H__
