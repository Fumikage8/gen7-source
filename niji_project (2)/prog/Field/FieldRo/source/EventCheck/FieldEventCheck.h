//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEventCheck.h
 *  @brief  イベントチェック処理
 *  @author tomoya takahashi
 *  @date   2015.05.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELDEVENTCHECK_H__
#define __FIELDEVENTCHECK_H__

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldAttribute.h"

// conv_header
#include <niji_conv_header/field/attribute/attribute_id.h>

namespace Field{

  class FieldPokeFinderActor;

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

// イベントリクエスト情報
// この情報を見て、イベントを起動するか判断する。
struct EventCheckRequest{
  gfl2::heap::HeapBase* p_heap;  ///<ヒープベース
  GameSys::GameManager * p_gameman;        ///<ゲームシステムへのポインタ
  GameSys::GameData * p_gamedata;        ///<ゲームデータへのポインタ
  GameSys::GameEventManager *p_evman;   ///<参照しているイベントデータ
  Fieldmap * p_fieldmap;        ///< フィールドマップインスタンス
  MoveModel::FieldMoveModel * p_player;        ///< 自機
  //↑変化がないパラメータ　

  //↓毎回変化する可能性があるパラメータ
  u16 zone_id;                 ///<現在のZoneID
  u16 reserved_scr_id;        ///<予約スクリプトのID
  const gfl2::ui::Button* cp_button;       ///<ボタンの情報
  const gfl2::ui::VectorDevice* cp_stick;  ///<スティックの情報  UiDeviceManagerそのまま
  const gfl2::ui::VectorDevice* cp_crossKey;///<十字キーの情報   UiDeviceManagerそのまま

  BaseActor * ptalkActor;       ///< 話しかけ対象のアクター
  FieldPokeFinderActor* pFinderActor; ///< ヒットしたファインダーアクター

  Attr now_attr;                ///<足元のアトリビュート情報

  u32 encountDataID;            ///< エンカウントデータID

  u32 talkRequest:1; ///<話しかけキー操作があったかどうか
  u32 menuRequest:1; ///<メニューオープン操作があったかどうか
  u32 moveRequest:1; ///<GRID_SIZEの距離移動した終了タイミングかどうか(アナログ移動の場合、グリッドが変わったとしてもtrueにならない）
  u32 stepRequest:1; ///<振り向きorGRID_SIZEの距離移動したタイミングかどうか？(アナログ移動の場合、グリッドが変わったとしてもtrueにならない）
  u32 subscreenRequest:1; ///<サブスクリーンイベントの起動操作があったか？

  u32 debugRequest:1;  ///<デバッグ操作があったかどうか(Rボタン)
  u32 exMoveRequest:1;  ///<ダニエル状態の動作終了時にエンカウント補正をアップさせる
  u32 isEncount:1;    ///<エンカウントを許すか
  u32 enableWeakReserve:1;  ///<WeakReserveの発動許可
  u32 isHitHiddenItem:1;    ///<隠しアイテムにHITしているか
  u32 moveStepRequest:1;    ///< 一歩移動したらtrue
  u32 encountRequest:1;   ///< エンカウントのイベント起動を許可
  u32 padding : 15;

};

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------
class FieldEventCheck
{
public:
  enum Mode{
    CHECKMODE_NORMAL,     ///< 通常時用イベントチェック
    CHECKMODE_JOIN_FESTA, ///< ジョインフェスタ時用イベントチェック

    CHECKMODE_MAX,    ///< システム内で使用
  };

  enum EventCheckRet{

    RET_EVENT_NONE,           ///< イベント起動なし
    RET_EVENT_NORMAL_EVENT,   ///< 通常イベント起動
    RET_EVENT_NOT_CLEAR_INTEREST_EVENT,///< アクションボタンのInterestをクリアしないイベント起動
    RET_EVENT_MENU_EVENT,     ///< フィールドメニューを動かす必要があるイベント
    RET_EVENT_NO_EVENT_ON,    ///< イベントは起動していないが、アクションを実行した。

#if PM_DEBUG
    RET_DEBUG_FORCE_END,     ///< イベントチェック強制終了

#endif // #if PM_DEBUG

  };

  // 設定
  struct Desc
  {
    Mode mode;
    GameSys::GameManager* p_gameman;
    Fieldmap* p_fieldmap;
    gfl2::heap::HeapBase* p_heap;
    gfl2::gfx::IGLAllocator* p_allocator;

    BaseCollisionScene* pCollisionSceneForEventPos;
    BaseCollisionScene* pCollisionSceneForEventIntrPos;
    BaseCollisionScene* pCollisionSceneForEventTalk;
    BaseCollisionScene* pCollisionSceneForEventPush;
    BaseCollisionScene* pCollisionSceneForEncount;
    BaseCollisionScene* pCollisionSceneForTrainerEye;
    BaseCollisionScene* pCollisionSceneForEventNaminori;
    BaseCollisionScene* pCollisionSceneForGimmickEncount;
    BaseCollisionScene* pCollisionSceneForGimmickEncountAction;
    BaseCollisionScene* pCollisionSceneForEventFinder;
    BaseCollisionScene* pCollisionSceneForFishingPos;
  };

  // 会話判定　Dist算出　開始距離
  static const f32 SELECT_TALK_START_DIST;


#if PM_DEBUG
  static f32 m_DebugTalkCheckCosLimit;  ///< 会話出来る角度のリミット値
#else
  static const f32 m_DebugTalkCheckCosLimit;  ///< 会話出来る角度のリミット値
#endif // PM_DEBUG

#if PM_DEBUG
  static u32 m_DebugStepCount;
  static u32 m_DebugMoveStepCount;
#endif

#if PM_DEBUG
  static b32 m_IsDebugPokemonHeapMemorySizeCheck;
  static u32 m_DebugPokemonHeapFreeMemorySize;
  static u32 m_DebugPokemonHeapAllocatableSize;
  static u32 m_DebugPokemonHeapCheckZoneID;
#endif

  /**
   *  @brief イベント起動していないかチェック
   **/
  static b32 IsEventCheckRetNoneEvent( EventCheckRet ret )
  {
    return (ret == RET_EVENT_NONE) || (ret == RET_EVENT_NO_EVENT_ON);
  }

public:

  /* コンストラクタ・デストラクタ */
  FieldEventCheck();
  ~FieldEventCheck();

  //----------------------------------------------------------------------------
  /**
   *  @brief    イベントチェック　初期化
   *
   *  @param    mode      チェックモード
   *  @param    p_gameman    ゲームマネージャ
   *  @param    p_fieldmap   フィールドマップ
   */
  //-----------------------------------------------------------------------------
  void Initialize( const Desc& desc );

  //----------------------------------------------------------------------------
  /**
   *  @brief    イベントチェック
   */
  //-----------------------------------------------------------------------------
  void EventCheck( void );


  //----------------------------------------------------------------------------
  /**
   *  @brief    現在の会話ターゲットを取得
   */
  //-----------------------------------------------------------------------------
  BaseActor * GetTalkTarget() { return m_Request.ptalkActor; }

  //----------------------------------------------------------------------------
  /**
   *  @brief    現在の会話ターゲットをクリア
   *  @fix GFNMCat[3161]：動的に会話ターゲットなっている配置を削除する事があるのでクリアする
   */
  //-----------------------------------------------------------------------------
  void ClearTalkTarget() { m_Request.ptalkActor = NULL; }

  //----------------------------------------------------------------------------
  /**
   *  @brief    現在のターゲットPokeFinderActorを取得
   */
  //-----------------------------------------------------------------------------
  FieldPokeFinderActor * GetPokeFinderActor() { return m_Request.pFinderActor; }


  //----------------------------------------------------------------------------
  /**
   *  @brief    アクションボタンの注視処理をクリアしない状況かチェック
   */
  //-----------------------------------------------------------------------------
  b32 IsActionButtonInterestNotClear() const { return m_ActionButtonInterestNotClear; }

  //----------------------------------------------------------------------------
  /**
   *  @brief    隠しアイテムにHitしているかを取得(優先度チェックは行っていない)
   */
  //-----------------------------------------------------------------------------
  b32 IsHitHiddenItem() const { return m_Request.isHitHiddenItem; }


  /**
   *  @brief エンカウントのコリジョンチェックを実行する。
   *  @note 次にCheckEncountを実行
   */
  static void CalcCollisionEncount( MoveModel::FieldMoveModelManager& moveModelMan, BaseCollisionScene& collisionSceneEncount );

  /**
   *  @brief エンカウントするかチェックする。
   *  @note 必ず先にCalcCollisionEncountを実行
   *  @retval Encount::Encount::ENCOUNT_DATA_ID_ERROR エンカウントしない
   *  @retval それ以外  エンカウントする。
   */
  static u32 CheckEncount( MoveModel::FieldMoveModel* p_player );

  //----------------------------------------------------------------------------
  /**
   *  @brief  "なみのり"のコリジョンの更新を行なう
   *  @fix GFNMCat[3882] 追加対処
   */
  //-----------------------------------------------------------------------------
  void UpdateCollisionNaminori(void);


private:

  Mode m_Mode;        ///< 動作モード
  EventCheckRequest m_Request;  ///< リクエスト情報
  BaseCollisionScene*  m_pCollsionSceneForEventPos;   /// 上に載ったら反応
  BaseCollisionScene*  m_pCollsionSceneForEventIntrPos;/// 上に載ったら反応
  BaseCollisionScene*  m_pCollsionSceneForEventTalk;  /// 話しかけで反応
  BaseCollisionScene*  m_pCollsionSceneForEventPush;  /// プッシュタイミングで反応
  BaseCollisionScene*  m_pCollsionSceneForEncount;    /// エンカウント
  BaseCollisionScene*  m_pCollsionSceneForTrainerEye; /// トレーナー視線
  BaseCollisionScene*  m_pCollsionSceneForEventNaminori; /// 波乗り(イベント)
  BaseCollisionScene*  m_pCollsionSceneForGimmickEncount;         /// ギミックエンカウントのエンカウント
  BaseCollisionScene*  m_pCollsionSceneForGimmickEncountAction;   /// ギミックエンカウントのアクション
  BaseCollisionScene*  m_pCollsionSceneForEventFinder;/// 上に載ったら反応
  BaseCollisionScene*  m_pCollisionSceneForFishingPos;

  b32 m_ActionButtonInterestNotClear; ///< アクションボタンの注目をクリアしないフラグ

private:

  //-----------------------------------------------------------------------------
  /**
   *      共通
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *  @brief  リクエストワークの初期化
   *
   *  @param  p_reqData     データ設定するリクエストワーク
   *  @param  p_gameman     ゲームマネージャ
   *  @param  p_fieldmap    フィールドマップ
   */
  //-----------------------------------------------------------------------------
  static void InitializeRequest( EventCheckRequest* p_reqData, GameSys::GameManager* p_gameman, Fieldmap* p_fieldmap );
  //----------------------------------------------------------------------------
  /**
   *  @brief  リクエストワークの更新
   *
   *  @param  p_reqData     データ設定するリクエストワーク
   */
  //-----------------------------------------------------------------------------
  static void UpdateRequest( EventCheckRequest* p_reqData );

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
   *  @brief    デバッグ　起動　イベントチェック
   *
   *  @param  reqData リクエストデータ
   *
   *  @return イベント起動結果
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckDebug( const EventCheckRequest& reqData );

  //----------------------------------------------------------------------------
  /**
   *  @brief    デバッグ　起動　イベントチェック(イベントチェックの一番最後に実行される)
   *
   *  @param  reqData リクエストデータ
   *
   *  @return イベント起動結果
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckDebugBottom( const EventCheckRequest& reqData );

#endif // PM_DEBUG


  //-----------------------------------------------------------------------------
  /**
   *      通常マップ用
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *  @brief  通常マップ用　イベントチェック
   *
   *  @param  リクエストデータ
   *
   *  @retval true  イベント起動
   *  @retval false イベント起動なし
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckNormal( const EventCheckRequest& reqData );

  //----------------------------------------------------------------------------
  /**
   *  @brief  一歩移動イベントチェック
   */
  //-----------------------------------------------------------------------------
  static bool CheckMoveEvent( const EventCheckRequest& reqData );

  //----------------------------------------------------------------------------
  /**
   *  @brief  なつきど上昇処理
   */
  //-----------------------------------------------------------------------------
  static void checkFriendlyStepCount( const EventCheckRequest& reqData );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 育て屋チェック
   *
   * @param reqData リクエストデータ
   */
  /* -------------------------------------------------------------------------*/
  static bool CheckSodateya( const EventCheckRequest& reqData );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 卵孵化　チェック
   *
   * @param reqData リクエストデータ
   */
  /* -------------------------------------------------------------------------*/
  static bool checkPartyEggHatching( pml::PokeParty* party, pml::pokepara::CoreParam** pp_param, u32 *pos );
  static void updatePartyEggHatching( GameSys::GameData * gdata, pml::PokeParty* party );  ///< ポケパーティ　孵化カウンタ更新
  static void updatePartyEggHatchingCount( pml::PokeParty* party ); ///< ポケパーティ　孵化カウンタ更新  実際の更新処理
  static u32 getPartyEggHatchingValue( const pml::PokeParty* party ); ///< パーティ内のポケモンを見て、負荷のカウントスピードを選ぶ
  
  
  /* -------------------------------------------------------------------------*/
  /**
   * @brief 虫除けスプレー使用チェック
   *
   * @param reqData リクエストデータ
   */
  /* -------------------------------------------------------------------------*/
  static bool CheckSpray( const EventCheckRequest& reqData );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ジョインフェスタマップ用　イベントチェック
   *
   *  @param  リクエストデータ
   *
   *  @retval true  イベント起動
   *  @retval false イベント起動なし
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckJoinFesta( const EventCheckRequest& reqData );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベント POSイベント発動チェック
   *
   *  @param  reqData  リクエストデータ
   *
   *  @retval true  イベント起動 / false イベント起動なし
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckPosEvent( const EventCheckRequest& reqData );
  static EventCheckRet checkPosEventCore( const EventCheckRequest& reqData, DynamicActor* dynamic_actor, int result_index, b32 isIntr = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベント INTRPOSイベント発動チェック
   *
   *  @param  reqData  リクエストデータ
   *
   *  @retval true  イベント起動 / false イベント起動なし
   */
  //-----------------------------------------------------------------------------
  static bool CheckIntrPosEvent( const EventCheckRequest& reqData );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベント つりPOSイベント発動チェック
   *
   *  @param  reqData  リクエストデータ
   *
   *  @retval true  イベント起動 / false イベント起動なし
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckFishingPosEvent( const EventCheckRequest& reqData );


  //-----------------------------------------------------------------------------
  /**
   * @brief   イベント 会話イベントチェック
   *
   * @param   reqData リクエストデータ
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckTalkEvent( const EventCheckRequest& reqData );

  //-----------------------------------------------------------------------------
  /**
   * @brief   イベント Pushイベントチェック
   *
   * @param   reqData リクエストデータ
   *
   * @return  true  イベント起動 / false イベント起動なし
   */
  //-----------------------------------------------------------------------------
  static EventCheckRet CheckPushEvent( const EventCheckRequest& reqData );

  //----------------------------------------------------------------------------
  /**
   *  @brief  通常エンカウントチェック
   *
   *  @param  reqData リクエストデータ
   *
   *  @retval true  エンカウント起動
   *  @retval false エンカウントしない
   */
  //-----------------------------------------------------------------------------
  static bool CheckNormalEncountEvent( const EventCheckRequest& reqData, Attr attr );

  //----------------------------------------------------------------------------
  /**
   *  @brief  下画面タッチリクエストチェック
   *
   *  @param  pManager FieldSubScreenProcManager
   *  @retval true  起動
   *  @retval false リクエストは無い
   */
  //-----------------------------------------------------------------------------
  static bool CheckSubScreenReq( Field::SubScreen::FieldSubScreenProcManager* pManager);

  //----------------------------------------------------------------------------
  /**
   *  @brief  波乗り解除リクエストチェック
   *
   *  @param  
   *  @retval true  起動
   *  @retval false リクエストは無い
   */
  //-----------------------------------------------------------------------------
  static bool CheckCancelNaminori( const EventCheckRequest& reqDat );

  //----------------------------------------------------------------------------
  /**
   *  @brief  波乗り開始リクエストチェック
   *
   *  @param  
   *  @retval true  起動
   *  @retval false リクエストは無い
   */
  //-----------------------------------------------------------------------------
  static bool CheckStartNaminori( const EventCheckRequest& reqDat );

  //----------------------------------------------------------------------------
  /**
   *  @brief  QRスキャン時間経過チェック
   *  @retval true  起動
   *  @retval false リクエストは無い
   */
  //-----------------------------------------------------------------------------
  static bool CheckQRScanTimeout( const EventCheckRequest& reqDat );

  //----------------------------------------------------------------------------
  /**
  *  @brief  ロトムパワーチェック
  *  @retval true  終了した
  *  @retval false 使用中または未使用
  */
  //-----------------------------------------------------------------------------
  static bool CheckRotomPowerTimeout( const EventCheckRequest& reqData );

  //-----------------------------------------------------------------------------
  /**
   *     〜用
   */
  //-----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   *  @brief  コリジョンの更新を行なう
   *  @param  reqData リクエストデータ
   */
  //-----------------------------------------------------------------------------
  void UpdateCollision( void );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  話しかけターゲットの決定
   *
   *  @param  reqData リクエストデータ
   */
  //-----------------------------------------------------------------------------
  void SelectTalkTarget( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウントテーブルデータIDの決定
   */
  //-----------------------------------------------------------------------------
  void SelectEncountDataID( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケファインダーの決定
   */
  //-----------------------------------------------------------------------------
  void SelectPokeFinder( void );

  //----------------------------------------------------------------------------
  /**
  *  @brief  NPCの注目を停止
  */
  //-----------------------------------------------------------------------------
  void StopNpcInterest( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief 会話イベントHookのチェック
   */
  //-----------------------------------------------------------------------------
  void HookTalkEvent(void);
  static b32 IsHitCollisionResult( DynamicActor* dynamic_actor, int result_index );

};


} // namespace field


#endif  // __FIELDEVENTCHECK_H__
