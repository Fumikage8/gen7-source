//=============================================================================
/**
 *
 *  @file   FieldScriptSystem.h
 *  @brief  フィールドスクリプトシステム
 *  @author hosaka genya
 *  @date   2011.08.12
 *
 */
//=============================================================================
#ifndef __FIELDSCRIPTSYSTEM_H__
#define __FIELDSCRIPTSYSTEM_H__

#include <pawn/include/pawn.h> // for AMX
#include <pawn/include/gfl2_Pawn.h> // for AMX
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include "FieldScript/include/ScriptTypes.h"  //SCRIPT_TYPE
#include "FieldScript/include/ScriptAssert.h"  //EventScriptのmake対象者限定アサートマクロ定義
#include "FieldScript/include/ScriptDebug.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"        //ZoneID
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "ScriptEffectManager.h"
#include "ScriptMoveModelManager.h"

#include <Fade/include/gfl2_FadeTypes.h>

//--------------------------------------------------------------
///  クラス参照宣言
//--------------------------------------------------------------
namespace gfl2{
  namespace str {
    class MsgData;
  }
}

namespace GameSys {
  class GameManager;
  class GameEvent;
}

namespace Field{
  class IFieldActorBase;

  namespace MoveModel{
    class FieldMoveModel;
  }
  namespace Camera{
    namespace Controller{
      class ControllerTypeEventScript;
    }
  }
  namespace Effect{
    class IEffectBase;
  }
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )


//=============================================================================
// @brief フェードの状態を復帰するために保持する情報
//=============================================================================
struct FadeStateSaveData
{
  gfl2::Fade::FADE_RESULT result;
  gfl2::math::Vector4     start_col;
  gfl2::math::Vector4     end_col;
};

//=============================================================================
// @brief スクリプト関連の常駐データ、GameDataが保持する
//=============================================================================
class GameData
{
public:

  GameData( void )
  {
    for( u32 i = 0; i < gfl2::Fade::DISP_BUFF_MAX; i++ )
    {
      FadeStateSaveData* pData = &m_FadeStateSaveData[ i ];
      pData->result = gfl2::Fade::FADE_RESULT_IN;
      pData->start_col = gfl2::math::Vector4(0.0f,0.0f,0.0f,0.0f);
      pData->end_col = gfl2::math::Vector4(0.0f,0.0f,0.0f,255.0f);
    }
  }

  ~GameData()
  {

  }

  /* ------------------------------------------------------------------------- */
  /**
  * @brief 指定画面のフェード状態保持情報を取得
  *
  * @param disp       画面
  */
  /* ------------------------------------------------------------------------- */
  FadeStateSaveData* GetFadeStateSaveData( gfl2::Fade::DISP disp )
  {
    return &m_FadeStateSaveData[ disp ];
  }

private:

  FadeStateSaveData m_FadeStateSaveData[ gfl2::Fade::DISP_BUFF_MAX ];   // フェード状態復元用情報。FadeStateSaveコマンドで使用。

};

//--------------------------------------------------------------
///  クラス参照宣言
//--------------------------------------------------------------
class ScriptObject;
class Work;
class SystemWork;

class RegularObject;
class SceneChangeControl;
class ScriptBufferManager;
class ObjectContainer;
class ScriptMoveModelController;
class ScriptTrialModel;

class DebugNameManager;

//--------------------------------------------------------------
/// 初期化スクリプトのタイプ指定
//--------------------------------------------------------------
enum GAME_INIT_TYPE
{ 
  GAME_INIT_TYPE_INIT,      ///< ゲーム開始時
  GAME_INIT_TYPE_DEBUG,     ///< デバッグ開始時
  GAME_INIT_TYPE_GAMECLEAR, ///< ゲームクリア時
};

struct SCRIPT_ARC_TABLE;

//=============================================================================
/**
 * @class System
 * @brief フィールドスクリプトシステム（シングルトン）
 */
//=============================================================================
class FieldScriptSystem
{ 
  GFL_FORBID_COPY_AND_ASSIGN( FieldScriptSystem ); ///< コンストラクタ + 代入禁止

public:
  void  ResetLastKeyXMenuOpen(){ m_bLastKeyXMenuOpen = false; }
  void  SetLastKeyXMenuOpen( const bool _bLastKeyXMenuOpen ){ m_bLastKeyXMenuOpen = _bLastKeyXMenuOpen; }
  bool  GetLastKeyXMenuOpen(){ return m_bLastKeyXMenuOpen; }

  static const u32 BG_PARTS_MAX = 5;
  struct BG_PARTS_PARAM
  {
    u8  use : 1;
    u8  visible : 1;
    u8  useEventHeap : 1;
    u16 partsNo;
  };

  struct BG_PARTS_INFO
  {
    BG_PARTS_PARAM paramTable[ BG_PARTS_MAX ];
  };


#if PM_DEBUG
  void  SetDebugMsgLabel( wchar_t *_pStr ){
    if( _pStr == NULL ) m_sDebugMsgLabel[ 0 ] = '\0';
    else  memcpy( m_sDebugMsgLabel, _pStr, 64 * sizeof(wchar_t) );
  }
  wchar_t  *GetDebugMsgLabelPtr(){ return m_sDebugMsgLabel; }
  u32  *GetIsDebugMsgLabelPtr(){ return &m_uIsDebugMsgLabel; }
  u32  GetIsDebugMsgLabel(){ return m_uIsDebugMsgLabel; }
#endif // PM_DEBUG
  
  SCRIPT_ARC_TABLE* GetScriptArcTable() const { return m_pScriptArcTable; }
  size_t GetScriptArcTableSize() const { return m_iScriptArcTableSize; }
  Field::Effect::IEffectBase  *GetEffect(){ return m_pEffect; }
  void  SetEffect( Field::Effect::IEffectBase* _pEffect ){  m_pEffect = _pEffect; }
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_gman ゲームマネージャへのポインタ
   * @param p_heap ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  FieldScriptSystem( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~FieldScriptSystem( void );
  
  void MsgDataNameList_Initialize();
  bool MsgDataNameList_Update();
  void MsgDataNameList_Terminate();

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 生成処理：開始
   *
   * @param p_heap  ヒープへのポインタ
   * @param p_fileread ファイル読み込みクラスへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void Create( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* p_fileread );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 生成処理：メイン
   *
   * @return true:生成完了, false:生成中
   */
  /* ------------------------------------------------------------------------- */
  bool UpdateCreate( void );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 生成済みかどうかのチェック
   *
   * @return true:生成済み, false:生成済みでない
   */
  /* ------------------------------------------------------------------------- */
  bool IsCreated( void ) const;
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 削除処理
   */
  /* ------------------------------------------------------------------------- */
  void Delete( void );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief フィールドマップが削除される際の破棄処理
   */
  /* ------------------------------------------------------------------------- */
  void FinalizeFieldmapTerminateTop( void );
  void FinalizeFieldmapTerminateBottom( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ゾーンスクリプト初期化処理
   *
   * @param data ゾーンスクリプトのバッファ
   * @param size ゾーンスクリプトのバッファサイズ
   * @param zone_id ゾーンID
   *
   * @note  「つづきから」でも実行される
   */
  /* ------------------------------------------------------------------------- */
  void InitializeZoneScript( void* data, u32 size, ZoneID zone_id );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ゾーンスクリプト廃棄処理
   */
  /* ------------------------------------------------------------------------- */
  void FinalizeZoneScript( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ゾーンローカルワーク初期化
   * ゾーンチェンジを伴う初期化時に呼び出して、ゾーンローカル領域の初期化を行う
   */
  /* ------------------------------------------------------------------------- */
  void ClearZoneLocalWork( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプトのアップデート処理
   *
   * @return pawnのupdate実行結果
   */
  /* ------------------------------------------------------------------------- */
  s32 Update( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief インタラクティブエンジン/AIエンジンのアップデート処理
   */
  /* ------------------------------------------------------------------------- */
  void UpdateInterctive( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト実行リクエスト
   *
   * @param scrid 実行するスクリプトID
   * @param p_event 実行元イベントへのポインタ
   * @param p_target_actor 対象のアクターへのポインタ(話しかけ起動などの時に設定)
   * @param p_talk_model   対象の動作モデルへのポインタ(話しかけ起動などの時に設定)
   */
  /* ------------------------------------------------------------------------- */
  void RunRequest( SCRIPTID scrid, GameSys::GameEvent* p_event = NULL, IFieldActorBase* p_target_actor = NULL, MoveModel::FieldMoveModel* p_talk_model = NULL );
  void RunRequest( SCRIPTID scrid, GameSys::GameEvent* p_event, u32 tareget_event_id, FieldActorType target_actor_type, MoveModel::FieldMoveModel* p_talk_model );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 子スクリプトを起動
   *
   * @param parent 親スクリプトオブジェクトへのポインタ
   * @param scrid 実行するスクリプトID
   */
  /* ------------------------------------------------------------------------- */
  void CallChildScript( ScriptObject * parent, SCRIPTID scrid );

  /* ------------------------------------------------------------------------- */
  /**
  * @brief 並列スクリプトを起動
  *
  * @param parent 親スクリプトオブジェクトへのポインタ
  * @param scrid 実行するスクリプトID
  * @param pHeap 使用するヒープ
  * @param param0 引き数として渡すワーク値 0 SCWK_PARAM0 にはいる
  * @param param1 引き数として渡すワーク値 1 SCWK_PARAM1 にはいる
  * @param param2 引き数として渡すワーク値 2 SCWK_PARAM2 にはいる
  * @param param3 引き数として渡すワーク値 3 SCWK_PARAM3 にはいる
  *
  * @return 登録ハンドル
  */
  /* ------------------------------------------------------------------------- */
  s32 CallParallelScript( RegularObject * parent, SCRIPTID scrid, gfl2::heap::HeapBase* pHeap, u32 param0, u32 param1, u32 param2, u32 param3 );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト実行中判定
   *
   * @return ture:実行中, false:実行中でない
   */
  /* ------------------------------------------------------------------------- */
  bool IsRunning( void ) const;
  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプトロード中判定
   *
   * @return true:ロード中, false:ロード中でない
   */
  /* ------------------------------------------------------------------------- */
  bool IsLoading( void ) const;
  /* ------------------------------------------------------------------------- */
  /**
   * @brief  スクリプト実行を強制的に終了させる。
   */
  /* ------------------------------------------------------------------------- */
  void ForceEnd(void);

  //--------------------------------------------------------------
  //  初期化スクリプト
  //--------------------------------------------------------------
  /* ------------------------------------------------------------------------- */
  /**
   * @brief　ゾーンスクリプト単発呼び出し
   *
   * @param buffer スクリプトバッファ
   * @param size スクリプトバッファサイズ 
   * @param zone_id ゾーンID
   * @param script_id 実行するスクリプトID
   * @param p_heap ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void CallZoneScriptOnetime( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, ZoneID zone_id, SCRIPTID script_id, gfl2::heap::HeapBase* p_heap );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 初期化スクリプト呼び出し：ゾーンチェンジ
   *
   * @param buffer スクリプトバッファ
   * @param size スクリプトバッファサイズ 
   * @param zone_id ゾーンID
   * @param mapchange_type マップ遷移タイプ
   * @param p_heap ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void CallZoneChange( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, ZoneID zone_id, ScrZoneInitType type, gfl2::heap::HeapBase* p_heap );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 初期化スクリプト呼び出し：フィールド復帰（コンティニュー時も）
   *
   * @param pTempHeap 一時ヒープへのポインタ
   * @param buffer スクリプトバッファ
   * @param size スクリプトバッファサイズ 
   */
  /* ------------------------------------------------------------------------- */
  void CallFieldRecover( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, gfl2::heap::HeapBase * pTempHeap );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 初期化スクリプト呼び出し：フィールド初期化
   *
   * @param pTempHeap 一時ヒープへのポインタ
   * @param buffer スクリプトバッファ
   * @param size スクリプトバッファサイズ 
   */
  /* ------------------------------------------------------------------------- */
  void CallFieldInit( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, gfl2::heap::HeapBase * pTempHeap );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 初期化スクリプト呼び出し：ゲーム開始
   *
   * @param p_heap ヒープへのポインタ
   * @param type 初期化タイプ
   */
  /* ------------------------------------------------------------------------- */
  void CallGameInit( gfl2::heap::HeapBase* p_heap, GAME_INIT_TYPE type );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief INTRスクリプト呼び出し(即時実行スクリプト)
   *
   * @param scrid 実行するスクリプトID
   * @param pos_event_id 実行元POSイベントのID
   */
  /* ------------------------------------------------------------------------- */
  void CallIntrScript( SCRIPTID scrid, int pos_event_id = 0 );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 起動パラメータをスクリプトワークにセット
   *
   * @param p0 パラメータ0
   * @param p1 パラメータ1
   * @param p2 パラメータ2
   * @param p3 パラメータ3
   */
  /* ------------------------------------------------------------------------- */
  void SetParameters( u16 p0, u16 p1, u16 p2, u16 p3 );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief BGIDをセット
   *
   * @param bgid BGID
   */
  /* ------------------------------------------------------------------------- */
  void SetBGID( u32 bgid );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief POSIDをセット
   *
   * @param posid POSID
   */
  /* ------------------------------------------------------------------------- */
  void SetPosID( u32 posid );

  //-----------------------------------------------------------------------------
  /**
   * @brief ワークに通知するフェードフラグを設定
   *
   * @param is_fade フェードフラグ
   */
  //-----------------------------------------------------------------------------
  void SetWorkToFadeFlag( bool is_fade );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 現在のカレント スクリプトオブジェクトを取得
   *
   * @return スクリプトオブジェクト
   */
  /* ------------------------------------------------------------------------- */
  ScriptObject* GetScriptObject( void ) const;
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief ワーク生成
   *
   * @param p_event 実行元イベントへのポインタ
   * @param target_actor_id   対象のフィールドアクターのEventID(起動時に設定)
   * @param target_actor_type 対象のフィールドアクターのType(起動時に設定)
   * @param p_talk_model   対象の動作モデルへのポインタ(話しかけ起動などの時に設定)
   * @param scr_id ワーク生成元のスクリプトID
   * @param acmdlist_max アクションコマンドリストの最大数
   *
   * @return 生成したスクリプトワークへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  Work* CreateWork( GameSys::GameEvent* p_event, u32 tareget_event_id, FieldActorType target_actor_type, MoveModel::FieldMoveModel* p_talk_model, SCRIPTID scr_id = SCRID_DEFAULT, u16 acmdlist_max = 128 );

  //--------------------------------------------------------------
  //  アクセサー
  //--------------------------------------------------------------
  /* ------------------------------------------------------------------------- */
  /**
   * @brief シングルトンであるスクリプトシステムへのポインタを取得
   *
   * @return スクリプトシステムへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  static FieldScriptSystem* GetInstance( void );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief ゲームマネージャへのポインタを取得
   *
   * @return ゲームマネージャへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  GameSys::GameManager* GetGameManager( void ) const { return m_pGameManager; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト動作モデルコントローラーへのポインタを取得
   *
   * @return スクリプト動作モデルコントローラーへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptMoveModelController * GetSMMController( void ) const { return NULL; /*m_ScriptMoveModelController;*/ }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief バッファマネージャへのポインタを取得
   *
   * @return バッファマネーシャへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptBufferManager * GetBufferManager( void ) const { return m_ScriptBufferManager; }
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief エフェクトマネージャへのポインタを取得
   *
   * @return エフェクトマネージャへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptEffectManager*  GetScriptEffectManager() const {  return m_pScriptEffectManager; }

  //------------------------------------------------------------------------------
  /**
   * @brief       TrialModelの取得.
   * @return      ScriptTrialModel* 
   */
  //------------------------------------------------------------------------------
  ScriptTrialModel* GetScriptTrialModel() const { return m_pScriptTrialModel; }
 
  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト動作モデルマネージャへのポインタを取得
   *
   * @return スクリプト動作モデルマネージャへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptMoveModelManager* GetScriptMoveModelManager() const {  return m_pScriptMoveModelManager;  }


  ScriptBufferManager* GetScriptBufferManager() { return m_ScriptBufferManager; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief カメラコントローラーのポインタを取得
   *
   * @return バッファマネーシャへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  Field::Camera::Controller::ControllerTypeEventScript * GetCameraController( void ) const { return m_pCameraController; }
  void SetCameraController( Field::Camera::Controller::ControllerTypeEventScript *pCamera) { m_pCameraController = pCamera; }

  // 地形ブロック制御点情報の記憶
  void SetTerrainBlockControlPosition( const gfl2::math::Vector3& rPos );
  void ClearTerrainBlockControlPosition( void );
  b32 IsTerrainBlockControlSet( void ) const;
  void GetTerrainBlockControlPosition( gfl2::math::Vector3* pPos ) const;

  // BGパーツの記憶
  BG_PARTS_INFO* GetBgPartsInfo( void );
  void ClearBgPartsParam( void );
  void GetBgPartsInfo( BG_PARTS_INFO* pOut );
  void SetBgPartsLoad( u32 no, b32 useEventHeap );
  void SetBgPartsUnLoad( u32 no );
  void SetBgPartsVisible( u32 no, b32 flag );
  BG_PARTS_PARAM* GetBgPartsParam( u32 no );
  u32 GetBgPartsCount( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief イベントポジション(イベント相対座標システム基準点)
   * セットされていない時は(0,0,0)が返るので、有効無効で扱いを変える必要はない
   */
  /* ------------------------------------------------------------------------- */
  gfl2::math::Vector3 GetEventPosition();
  void SetEventPosition( const gfl2::math::Vector3& pos );
  void SetEventPosition( float x, float y, float z );
  void ResetEventPosition();
  bool CheckEventPositionEnable();

  /* ------------------------------------------------------------------------- */
  /**
   * @brief イベントポジション(イベント相対座標システム基準点) オフセット計算
   * 
   * ワールド座標を与えると、イベントポジションからの相対座標を返すユーティリティ
   */
  /* ------------------------------------------------------------------------- */
  gfl2::math::Vector3 CalcEventPositionOffset( const gfl2::math::Vector3& pos );
  gfl2::math::Vector3 CalcEventPositionOffset( float x, float y, float z );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief イベントポジション(イベント相対座標システム基準点) オフセット計算
   * 
   * 相対座標座標を与えると、イベントポジションからワールド座標を計算して返すユーティリティ
   */
  /* ------------------------------------------------------------------------- */
  gfl2::math::Vector3 CalcEventPositionWorld( const gfl2::math::Vector3& targetPos );
  gfl2::math::Vector3 CalcEventPositionWorld( float x, float y, float z );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief  スクリプト終了後に、遷移するイベントを設定
   *
   * @param next_event スクリプト終了後に、遷移するイベント
   */
  /* ------------------------------------------------------------------------- */
  void EntryNextEvent( GameSys::GameEvent* next_event );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief  エントリーされたイベントを登録
   */
  /* ------------------------------------------------------------------------- */
  void CallEntryNextEvent( void );
  void ChangeEntryNextEvent( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 予約スクリプトをセットする
   *
   * @param scrid スクリプトID
   * @param p0 パラメータ0
   * @param p1 パラメータ1
   * @param p2 パラメータ2
   * @param p3 パラメータ3
   */
  /* ------------------------------------------------------------------------- */
  void SetReserveScript( SCRIPTID scrid, u32 p0 = 0, u32 p1 = 0, u32 p2 = 0, u32 p3 = 0 );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 予約されたスクリプトIDをゲットする
   *
   * @return スクリプトID
   */
  /* ------------------------------------------------------------------------- */
  SCRIPTID GetReservedScrID() const{return m_ReservedScrID;};
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 予約スクリプトが既にセットされているか？
   *
   * @return true されている, false されていない
   */
  /* ------------------------------------------------------------------------- */
  bool IsReserveScriptSetting( void ) const;
  //-----------------------------------------------------------------------------
  /**
   * @brief 予約スクリプトの発動チェック＆発動
   *
   * @return true:発動, false:発動なし
   */
  //-----------------------------------------------------------------------------
  bool CheckRunReserveScript( void );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 弱い予約スクリプトの発動予約
   *
   * @param scrid スクリプトID
   * @param p0 パラメータ0
   * @param p1 パラメータ1
   * @param p2 パラメータ2
   * @param p3 パラメータ3
   */
  /* ------------------------------------------------------------------------- */
  void SetWeakReservedScript( SCRIPTID scrid, u32 p0 = 0, u32 p1 = 0, u32 p2 = 0, u32 p3 = 0 );
  //-----------------------------------------------------------------------------
  /**
   * @brief 弱い予約スクリプトの発動チェック＆発動
   *
   * @return true:発動した, false:発動しなかった
   */
  //-----------------------------------------------------------------------------
  bool CheckRunWeakReserveScript( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief 条件を満たすシーン遷移スクリプトがあるか調べる
   * @return  bool  trueのとき存在する
   */
  //-----------------------------------------------------------------------------
  bool IsEnableSceneScript( void );
  //-----------------------------------------------------------------------------
  /**
   * @brief 条件を満たすシーン遷移スクリプトを検索、実行させる
   * @return  bool  trueのとき実行する
   */
  //-----------------------------------------------------------------------------
  bool SearchRunSceneScript( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief SpChrLoaded
   * @return  bool  trueの読み込みが実行されてまだ破棄されていない。
   */
  //-----------------------------------------------------------------------------
  bool IsSpChrLoaded() const { return m_bSpChrLoaded; }
  void SetSpChrLoadedFlag( bool flag ) { m_bSpChrLoaded = flag; }

  //-----------------------------------------------------------------------------
  /**
   * @brief IsContinueRecover
   * @return  bool  セーブ復帰処理中かどうか？（一度FieldInitを通ったタイミングOff）
   */
  //-----------------------------------------------------------------------------
  bool IsContinueRecover() const { return m_bContinueRecover; }
  void SetContinueRecoverFlag( bool flag ) { m_bContinueRecover = flag; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 条件を満たすシーン遷移スクリプトを検索、フェードオプションを返す
   */
  //-----------------------------------------------------------------------------
  SceneChangeFadeMode CheckSceneScriptFadeMode( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief コールバックスクリプトを設定 
   *
   * @param type コールバックタイプ
   * @param scrid スクリプトID
   */
  /* ------------------------------------------------------------------------- */
  void RegisterCallBackScript( SCR_CALLBACK_TYPE type, SCRIPTID scrid );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コールバックスクリプトIDを取得
   *
   * @param type 取得するコールバックタイプ
   *
   * @return スクリプトID
   */
  /* ------------------------------------------------------------------------- */
  SCRIPTID GetCallBackScriptID( SCR_CALLBACK_TYPE type ) const;

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ウェイト関数
   *
   * @param p_sys スクリプトシステムへのポインタ
   * @param label ユーザーラベル
   * @param p_work ユーザーワーク
   *
   * @return true:Wait完了, false:Wait中
   */
  /* ------------------------------------------------------------------------- */
  typedef bool (*WAIT_FUNC)( FieldScriptSystem* p_sys, u32 label, void* p_work );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief ウェイト関数を設定
   *
   * @param amx 実行元のAMXへのポインタ
   * @param func ウェイト関数へのポインタ
   * @param label ユーザーラベル
   * @param p_work ユーザーワーク
   */
  /* ------------------------------------------------------------------------- */
  void SetWaitFunc( AMX* amx, WAIT_FUNC func, u32 label, void* p_work );


  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージUpdate関数
   *
   * @param amx   amx
   * @param label ユーザーラベル
   * @param p_work ユーザーワーク
   *
   * @return true:完了, false:処理中
   */
  /* ------------------------------------------------------------------------- */
  typedef bool (*MSG_UPDATE_FUNC)( u32 label, void* p_work );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージUpdate関数を設定
   *
   * @param func ウェイト関数へのポインタ
   * @param label ユーザーラベル
   * @param p_work ユーザーワーク
   */
  /* ------------------------------------------------------------------------- */
  void SetMsgUpdateFunc( MSG_UPDATE_FUNC func, u32 label, void* p_work );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージUpdate関数の完了をチェック
   *
   *  @retval true  完了
   *  @retval false 途中
   */
  /* ------------------------------------------------------------------------- */
  bool IsMsgUpdateFuncEnd()const;

  /* ------------------------------------------------------------------------- */
  /**
   * @brief キャラクターの名前リストMsgDataへのポインタを取得
   */
  /* ------------------------------------------------------------------------- */
  gfl2::str::MsgData* GetMsgDataNameList( void ) const { return m_pMsgDataNameList; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプトを一旦中断させる(次のUpdateで途中から実行される)
   */
  /* ------------------------------------------------------------------------- */
  void Suspend( void );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプトシステムワークへのポインタを取得
   *
   * @return スクリプトシステムワークへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  SystemWork* GetSystemWork( void ) const { return m_pSystemWork; }
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief AMXの名前を取得(デバッグ時のみ)
   *
   * @param type スクリプトタイプ
   * @param index スクリプトインデックス
   *
   * @return AMXの名前
   */
  /* ------------------------------------------------------------------------- */
  const char * GetAmxName( SCRIPT_TYPE type, u16 index );

  /* ------------------------------------------------------------------------- */
  /**
  * @brief initスクリプトタイミングか？
  */
  /* ------------------------------------------------------------------------- */
  bool IsInitScriptTiming(void){ return m_initScriptTiming; }

#if PM_DEBUG
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 子スクリプトを起動 Debug用Onetime実行 
   *
   * @param parent 親スクリプトオブジェクトへのポインタ
   * @param scrid 実行するスクリプトID
   * 
   * すべてを同期読み込みで処理するので処理負荷がん無視。ロック上等
  /* ------------------------------------------------------------------------- */
  void DebugCallChildScriptOnetime( ScriptObject * parent, SCRIPTID scrid );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 登録されているリザーブスクリプトをクリア(debug用)
   */
  /* ------------------------------------------------------------------------- */
  void DebugClearReserveScript( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief キャラエディットリストを取得
   */
  /* ------------------------------------------------------------------------- */
  const DebugChrEditList* DebugGetChrEditList( int* p_num );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief シーンエディットの有効無効をゲット
   */
  /* ------------------------------------------------------------------------- */
  bool DebugCheckSceneEditEnable( int scriptID );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief シーンエディットのスキップIDをセット
   */
  /* ------------------------------------------------------------------------- */
  void DebugSetSceneSkipCode( int start, int end,  int max );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief シーンエディットのパラメータを取得
   */
  /* ------------------------------------------------------------------------- */
  int DebugGetSceneEditParam( DebugSceneEditParam * param );

#endif // PM_DEBUG

//=============================================================================
//
//    デバッグ用公開フラグ
//
//=============================================================================
#if PM_DEBUG
public:
  static int DebugHioEnableFlag;  ///<HIO機能の有効無効フラグ
  static int DebugEndPositionCheckEnableFlag;
  static int DebugEndPositionOutPutEnableFlag;
  static int DebugSceneEditEnableFlag;  ///<シーンエディット有効無効フラグ
  static int DebugRegdentHeapCheckDisableFlag;  ///<これが有効だと一回だけ常駐ヒープチェックアサートを回避する
  static int DebugHeapCheckOffEnableFlag;  ///<これが有効だとスクリプト起動毎にDebugRegdentHeapCheckDisableFlagがtrueになる
  bool  GetFieldScriptLocalRead() const { return GFL_BOOL_CAST(DebugHioEnableFlag); }
#else
  bool  GetFieldScriptLocalRead() const { return false; }
#endif // PM_DEBUG
  

  /* ------------------------------------------------------------------------- */
  /**
   * @brief　キャラクターモーション終了チェック開始
   */
  /* ------------------------------------------------------------------------- */
  b32 ChrMotionEndWatchStart( s32 event_id, u32 anime_id );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief　キャラクターモーション終了チェック終了
   */
  /* ------------------------------------------------------------------------- */
  void ChrMotionEndWatchClear( s32 event_id );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief　アクション終了後の回転リクエスト
   */
  /* ------------------------------------------------------------------------- */
  b32 ChrReqActionAfterRot(s32 event_id, u32 rot_frame, f32 target_angle, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 is_enable_idle_wait = true );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief　アクション終了後の回転リクエスト
   */
  /* ------------------------------------------------------------------------- */
  b32 ChrReqActionAfterRotTarget(s32 event_id, u32 rot_frame, s32 target_event_id, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 is_enable_idle_wait = true );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief　アクション終了後の回転リクエスト、無効化
   */
  /* ------------------------------------------------------------------------- */
  void ChrClearActionAfterRot( s32 event_id );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief　アクション終了後の回転リクエスト、無効化(全部)
   */
  /* ------------------------------------------------------------------------- */
  void ChrClearActionAfterRotAll( void );



#if PM_DEBUG
  b32 GetDebugScriptEventAfterCheckFlag(void) const { return m_DebugScriptEventAfterCheckFlag; }
  void SetDebugScriptEventAfterCheckFlag(b32 flag){ m_DebugScriptEventAfterCheckFlag = flag; }
#endif 

//=============================================================================
//
//    以下、内部用
//
//=============================================================================
private:



  /* ------------------------------------------------------------------------- */
  /**
   * @brief　キャラクターアクション終了のチェック
   */
  /* ------------------------------------------------------------------------- */
  void CheckChrActionEnd(void);

  /* ------------------------------------------------------------------------- */
  /**
   * @brief　キャラクターモーション終了のチェック
   */
  /* ------------------------------------------------------------------------- */
  void CheckChrMotionEnd( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief　キャラクターモーション終了のチェックを全てクリア
   */
  /* ------------------------------------------------------------------------- */
  void ChrMotionEndWatchClearAll( void );


  static const int ACMDLIST_MAX = 512;  ///< 通常のスクリプトでのアクションコマンドリストの最大値
  
  /// 自分自身へのポインタ
  /// @note 静的な関数でないと呼び出せないから自分自身が必要
  /// @note ここでは自分を一つとして限定しているが、複数の場合にはここを増やして管理する必要がある
  static FieldScriptSystem* pThis;
  
  gfl2::heap::HeapBase* m_pHeap;                                ///< ヒープへのポインタ
  gfl2::heap::HeapBase* m_pHeapFileRead;                        ///< amxファイル読み込み用のヒープへのポインタ
  gfl2::heap::HeapBase* m_pHeapArcFile;                         ///< FieldScriptGarc高速化テーブル展開領域
  gfl2::heap::HeapBase* m_pHeapWork;                            ///< スクリプトワーク用ヒープへのポインタ
  gfl2::heap::HeapBase* m_pHeapPawnZone;                        ///< ZoneScript用Pawnオブジェクト展開領域
  gfl2::heap::HeapBase* m_pHeapPawnGeneral;                     ///< GeneralScript用Pawnオブジェクト展開領域
  gfl2::heap::HeapBase* m_pHeapNameList;                        ///< メッセージ名前リストヒープへのポインタ
  GameSys::GameManager* m_pGameManager;                         ///< ゲームマネージャへのポインタ

  SystemWork*             m_pSystemWork;                                    ///< スクリプトシステムワークへのポインタ
  SceneChangeControl*     m_SceneChangeControl;                    ///< シーンチェンジコントロールへのポインタ
  ScriptBufferManager*    m_ScriptBufferManager;                  ///< スクリプトバッファマネージャへのポインタ
  ObjectContainer*        m_ObjectContainer;                          ///< オブジェクトコンテナへのポインタ
  ScriptEffectManager*    m_pScriptEffectManager;                  ///< 
  ScriptTrialModel*       m_pScriptTrialModel;
  ScriptMoveModelManager* m_pScriptMoveModelManager;

  enum INIT_STATE{
    INIT_STATE_NONE,
    INIT_STATE_LOADWAIT,
    INIT_STATE_CLOSEWAIT,
    INIT_STATE_COMPLITE,
  };
  INIT_STATE              m_eMsgDataNameListInit;
  void                    *m_pMsgDataNameListBuffer;
  gfl2::str::MsgData*     m_pMsgDataNameList;   ///<キャラクターの名前リストメッセージデータ


  Field::Effect::IEffectBase   *m_pEffect;


  bool    m_bCreated;                                           ///< 初期化済みフラグ
  bool    m_bForceEnd;                                          ///< 強制終了フラグ
  bool    m_bZoneChangeFlag;                                    ///< ゾーンチェンジフラグ

  RegularObject* m_pNowObject;                                  ///<現在実行中のスクリプトOBJECT
  Field::Camera::Controller::ControllerTypeEventScript*    m_pCameraController;  // カメラ操作、作るのはこのイベントだが、寿命管理をカメラシステムに任せるので外部扱い

  ZoneID  m_ZoneID;                                             ///< ゾーンID
  void*   m_pZoneAmx;                                           ///< ゾーンAMXバッファ
  u32     m_ZoneAmxSize;                                        ///< ゾーンAMXバッファサイズ

  GameSys::GameEvent* m_pEntryNextEvent;                        ///< スクリプト実行後のイベント
  SCRIPTID m_ReservedScrID;                                     ///< リザーブスクリプトID
  SCRIPTID m_WeakReservedScrID;                                 ///< 弱いリザーブスクリプトID
  SCR_CALLBACK_TYPE m_CallBackType;                             ///< スクリプトコールバックタイプ
  SCRIPTID m_CallBackScriptID;                                  ///< スクリプトコールバックID
  
  static const u32 WEAK_RESERVED_SCR_PARAM_MAX = 4;             ///< 弱いリザーブのパラメータ数
  u32 m_WeakReservedScrParam[ WEAK_RESERVED_SCR_PARAM_MAX ];    ///< 弱いリザーブのパラメータ保持ワーク

  static const u32 RESERVED_SCR_PARAM_MAX = 4;                  ///< リザーブのパラメータ数
  u32 m_ReservedScrParam[ RESERVED_SCR_PARAM_MAX ];             ///< リザーブのパラメータ保持ワーク


  SCRIPT_ARC_TABLE  *m_pScriptArcTable;
  size_t            m_iScriptArcTableSize;

  b32                   m_IsTerrainBlockControlSet;             ///< true 地形ブロックの制御点指定をおこなっている
  gfl2::math::Vector3   m_TerrainBlockControlPositon;           ///< 地形ブロックの制御点

  BG_PARTS_INFO         m_BgPartsInfo;

  bool  m_bFade;               ///< スクリプトワークに渡すフェードフラグ
  bool  m_bLastKeyXMenuOpen;   ///< LastKey特殊フラグ.
  bool  m_bSpChrLoaded;        ///< SpChrLoadを実行した状態か。
  bool  m_bContinueRecover;    ///< スクリプト実行時に、セーブ復帰初期化時かどうかを取得するためのフラグ(一度FieldInitを通ったタイミングOff)

  FieldScriptSystem::MSG_UPDATE_FUNC  m_MsgUpdateFunc;  ///< メッセージアップデート処理
  u32                                 m_MsgUpdateLabel; ///< メッセージアップデートラベル
  void*                               m_pMsgUpdateWork; ///< メッセージアップデートワーク

  bool m_initScriptTiming; //initスクリプト実行中か？

#if PM_DEBUG
  DebugNameManager * m_DebugNameManager;                        ///< デバッグ名マネージャへのポインタ
  b32     m_DebugBufferFlag;                                    ///< ゾーンAMXバッファはHIOシステムで生成したものか？
  wchar_t     m_sDebugMsgLabel[64];
  u32         m_uIsDebugMsgLabel;                               ///< デバッグメニューで触りやすいように u32 に.
#endif // PM_DEBUG

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 新しいオブジェクトを生成&取得
   *
   * @param scrid スクリプトID
   * @param p_event 実行元イベントへのポインタ
   * @param target_actor_id   対象のフィールドアクターのEventID(起動時に設定)
   * @param target_actor_type 対象のフィールドアクターのType(起動時に設定)
   * @param p_talk_model   対象の動作モデルへのポインタ(話しかけ起動などの時に設定)
   *
   * @return 新しいオブジェクト
   */
  /* ------------------------------------------------------------------------- */
  RegularObject * GetNewObject( SCRIPTID scrid, GameSys::GameEvent* p_event, u32 tareget_event_id, FieldActorType target_actor_type, MoveModel::FieldMoveModel* p_talk_model );

  // モーション終了監視機能
  struct MOTION_END_WATCH_DATA
  {
    s32 event_id;     // 対象キャラクター
    u32 anime_id;     // 終了後のモーション
    u8  disable_count;// 0までカウントダウンする、0のときのみ、有効
  };

  static const int MOTION_END_WATCH_DATA_MAX = 4;  ///< 監視できる最大数
  MOTION_END_WATCH_DATA m_ChrMotionEndWatchData[ MOTION_END_WATCH_DATA_MAX ];

  // アクション終了監視機能
  struct ACTION_END_WATCH_DATA
  {
    s32 event_id;     // 対象キャラクター
    s32 rot_frame;
    b32 is_enable_idle_init;
    b32 is_compress_motion;
    b32 is_enable_turn_motion_init;
    b32 is_enable_idle_wait;

    b32 is_taget_mode;
    s32 target_event_id;
    f32 target_angle;
  };
  
  static const int ACTION_END_WATCH_DATA_MAX = 12;  ///< 監視できる最大数
  ACTION_END_WATCH_DATA m_ChrActionEndWatchData[ ACTION_END_WATCH_DATA_MAX ];


  /* ------------------------------------------------------------------------- */
  /**
   * @brief　キャラクターアクション終了監視用データ取得
   *
   * @return 空データがない場合はNULL
   */
  /* ------------------------------------------------------------------------- */
  ACTION_END_WATCH_DATA* GetActionEndWatchEmptyWork(void);

#if PM_DEBUG
  b32 m_DebugScriptEventAfterCheckFlag;     // trueのとき、フリームーブのイベント終了時チェックを行う。現状ではバミリ。
#endif 
};      // End of Class System
  
  
//=============================================================================
//=============================================================================
/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトからのイベントコール
 *
 * @tparam ManagerClass マネージャの型
 * @tparam callclass 生成＆コールするイベントの型
 * @param pManager マネージャへのポインタ
 *
 * @return 生成したイベントへのポインタ
 */
/* ------------------------------------------------------------------------- */
template <class ManagerClass , class CallClass> CallClass* SCRIPT_EVENT_CALL(ManagerClass* pManager)
{
  FieldScriptSystem* pSystem = FieldScriptSystem::GetInstance();
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pManager->CallEvent( pCC );
  
  // 中断させる
  pSystem->Suspend();

  return pCC;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトからのイベントコール DLL対応版
 *
 *
 * @tparam ManagerClass マネージャの型
 * @tparam callclass 生成＆コールするイベントの型
 * @param pManager マネージャへのポインタ
 * @param roName 読み込むRO名
 *
 * @return 生成したイベントへのポインタ
 */
/* ------------------------------------------------------------------------- */
template <class ManagerClass , class CallClass> CallClass* SCRIPT_EVENT_RO_CALL(ManagerClass* pManager, const char* roName)
{
  nn::ro::Module* pModule = gfl2::ro::StartModuleFunc( roName );
  FieldScriptSystem* pSystem = FieldScriptSystem::GetInstance();
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );

  pManager->CallEvent( pCC );
  
  // 中断させる
  pSystem->Suspend();

  return pCC;
}


GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );


#endif // __FIELDSCRIPTSYSTEM_H__
