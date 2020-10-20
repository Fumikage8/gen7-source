//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   Fieldmap.h
 *  @brief  フィールドマップ
 *  @author GAME FREAK.inc
 *  @date   2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELDMAP_H__ )
#define __FIELDMAP_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_DrawUtilText.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Quaternion.h>
#include <ro/include/gfl2_RoManager.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include "System/include/nijiAllocator.h"

// Field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/Gimmick/FieldGimmickBuilder.h"
#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldStatic/include/FieldArea.h"
#include "Field/FieldStatic/include/FieldLocation.h"

// RTTIデバッグシステム
#include <system/include/InputDevice/gfl2_Pad.h>
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

//レイアウト関連
#include "AppLib/include/Util/AppRenderingManager.h"

// プラグラムによるプロファイラ制御の有無
#if defined(GFX_CTR_PROFILING_ENABLED)
   //#define FIELD_CTR_PROFILING_ENABLED
#endif

// プラグラムによるプロファイラ制御の有り
#if defined(FIELD_CTR_PROFILING_ENABLED)
  #include <nn/prof.h>
  // プロファイル開始
  #define FIELD_CTR_PROFILING_START nnprofRuntimeControlInitialize(); \
                                    nnprofRuntimeControlSetFlags            (NN_PROF_FLAGS_CALLSTACKS); \
                                    nnprofRuntimeControlSetSampleMethod     (NN_PROF_SAMPLEMETHOD_TIME); \
                                    nnprofRuntimeControlSetSampleTime       (NN_PROF_SETTINGS_100X); \
                                    nnprofRuntimeControlSetThreadIDToProfile(nn::os::Thread::GetCurrentId()); \
                                    nnprofRuntimeControlSetCoreToProfile    (NN_PROF_CORE_APPLICATION); \
                                    nnprofRuntimeControlStart();
  // プロファイル終了
  #define FIELD_CTR_PROFILING_END   nnprofRuntimeControlStop();
  // プロファイル計測をする処理
  #define FIELD_CTR_PROFILING_INIT           0
  #define FIELD_CTR_PROFILING_UPDATE         1
  #define FIELD_CTR_PROFILING_TERM           0
  #define FIELD_CTR_PROFILING_ZONECHANGE     0
  #define FIELD_CTR_PROFILING_MAPBLOCKCHANGE 0
// プラグラムによるプロファイラ制御の無し
#else
  // プロファイル開始
  #define FIELD_CTR_PROFILING_START
  // プロファイル終了
  #define FIELD_CTR_PROFILING_END
  // プロファイル計測をする処理
  #define FIELD_CTR_PROFILING_INIT           0
  #define FIELD_CTR_PROFILING_UPDATE         0
  #define FIELD_CTR_PROFILING_TERM           0
  #define FIELD_CTR_PROFILING_ZONECHANGE     0
  #define FIELD_CTR_PROFILING_MAPBLOCKCHANGE 0
#endif

#if PM_DEBUG
  #define FIELD_TICK_PROFILING_UPDATE_NORMAL 0
#else
  #define FIELD_TICK_PROFILING_UPDATE_NORMAL 0
#endif

//-----------------------------------------------------------------------------
/**
 *          前方宣言
*/
//-----------------------------------------------------------------------------

class BaseCollisionScene;

namespace Field {

  class FieldRenderingPipeLineManager;
  class MyRenderingPipeLine;
  class EnvManager;
  class FieldEventCheck;
  class FieldResident;
  class TrafficManager;

  class DebugInfoWin;

  namespace StaticModel {
    class StaticModelManager;
  }

  namespace Debug {
    class FieldDebugMenu;
  }

  namespace GimmickEncount {
    class Manager;
  }

  namespace SubScreen {
    class FieldSubScreenProcManager;
    class FieldSubScreen;
  }

  namespace Terrain {
    class TerrainManager;
  }

  namespace MoveModel {
    class FieldMoveModelShadowManager;
  }
  namespace Camera {
    class CameraManager;
    class UiDeviceTranslator;
    namespace Target {
      class TargetBase;
    }
  }

  namespace Dowsing {
    class Tool;
  }

  namespace FieldWindow {
    class FieldWindowManager;
  }

  namespace Effect {
    class EffectManager;
  }

  namespace Encount {
    class Encount;
  }

  namespace ActionButton {
    class ActionButtonManager;
  }

  namespace TrialModel {
    class TrialModelFactory;
  }

  namespace DropItem {
    class Creator;
  }

  namespace weather {
    class WeatherManager;
    class WeatherControl;
  }

  namespace Skybox {
    class DataManager;
  }

  namespace Shade {
    class FieldPlayerShade;
  }
}

namespace System {
  namespace Skybox {
    class Skybox;
  }
}

namespace App {
  namespace Event {
    class ShopBuyEvent;
  }
  namespace StaffList {
    class FieldStaffList;
  }
}


GFL_NAMESPACE_BEGIN( Field );

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// 復帰モード
enum FieldmapOpenMode
{
  FM_OPEN_MODE_INIT,        //< 初期化
  FM_OPEN_MODE_APP_RECOVER, //< アプリ復帰
};
//// フィールドマップ初期化パラメータ
struct FieldmapDesc
{
  FieldmapOpenMode  openMode;
  Location          playerLocation;
};

enum FieldmapCloseMode
{
  FM_CLOSE_MODE_APP,          //< アプリケーションジャンプ
  FM_CLOSE_MODE_APP_KEEPRES,  //< アプリケーションジャンプ(エリアリソースを保持したまま）
  FM_CLOSE_MODE_MAPJUMP,      //< マップジャンプ
  FM_CLOSE_MODE_JFOPEN,       //< JFオープン
  FM_CLOSE_MODE_JFCLOSE,      //< JFクローズ

  FM_CLOSE_MODE_MAX
};

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------
class Fieldmap
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( Fieldmap );


  
public:
  // ギミック配列用定義
  enum GimmickIndex
  {
    GIMMICK_IDX_ZONE, //!< ゾーン用

    GIMMICK_IDX_MAX,  //!< 最大値
  }; // enum GimmickIndex

public:
  /**------------------------------------------------------------------
   *      公開定義
   ------------------------------------------------------------------*/

  /**------------------------------------------------------------------
   *      公開関数
   ------------------------------------------------------------------*/
  inline FieldRootNode* GetFieldRootNode(){ return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldRootNode(); }

  inline Field::FieldWindow::FieldWindowManager* GetFieldWindowManager( void ) const { return m_pFieldWindowManager; }

  /// 準備完了チェック
  inline bool IsReady() const{ return (0 != m_ReadyFlag); }

  /// 終了リクエスト
  inline void FieldmapCloseReq( FieldmapCloseMode mode ) { m_CloseFlag = true; m_CloseMode = mode; }

  inline GameSys::GameManager* GetGameManager() const {  return m_pGameManager;  }

  /// ゾーンID
  inline u32 GetZoneID() const { return m_ZoneId; }

  /// ワールドID
  inline u32 GetWorldID() const { return m_Desc.playerLocation.worldId; }
  
  /// エリアID 
  inline u32 GetAreaID() const { return m_pArea->GetAreaID(); }

  /// ヒープ取得
  inline gfl2::heap::HeapBase* GetHeap() { return m_pFieldHeap; }
  /// デバイスヒープ取得
  inline gfl2::heap::HeapBase* GetDeviceHeap() { return m_pFieldDeviceHeap; }
  /// デバイスヒープ　アロケーター取得
  inline System::nijiAllocator * GetDeviceHeapAllocator() { return m_pFieldDeviceHeapAllocator; }

  /// ゾーンチェンジ中かチェック
  inline bool IsZoneChange() const { return (m_seqZoneChange != 0); }

  /// イベントチェック
  inline FieldEventCheck *GetEventCheck( void ){ return m_pEventCheck; }

  /// ワールドID
  inline FieldmapOpenMode GetOpenMode() const { return m_Desc.openMode; }

public:
  /**------------------------------------------------------------------
   ------------------------------------------------------------------*/
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  Fieldmap( const FieldmapDesc& desc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  ~Fieldmap( void );

  /**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
  bool Initialize( GameSys::GameManager* pGameManager );

  /**
  * @brief    PROCのメイン更新を行う関数
  * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  bool Update( void );

  /**
  * @brief    PROCのPre描画処理
  */
  void PreDraw( void );

  /**
  * @brief  PROCの描画処理
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  /**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
  bool Terminate(void);

  /**
  * @brief   イベントチェックの実行
  */
  void EventCheck();

  /**
  * @brief   天球リソースを保持するリクエストを発行
  */
  void RequestKeepSkyboxResource( void );

  //
  // 取得系
  //

  /**
  * @brief フィールド描画パイプラインマネージャーを取得

  * @return フィールド描画パイプラインマネージャー
  */
  FieldRenderingPipeLineManager* GetFieldRenderingPipeLineManager( void ) const { return m_pFieldRenderingPipeLineManager; }
  
  /**
  * @brief 上画面表示用描画パイプラインを取得
  * @note  描画パイプラインのインターフェイスはα以降、変更が入る可能性が高いです、複雑な依存は追加しないでください
  *
  * @return 描画パイプライン
  */
  MyRenderingPipeLine* GetUpperRenderingPipeLine( void ){ return m_pRenderingPipeLine; }

  /**
  * @brief 地形マネージャ取得
  *
  * @return 地形マネージャ
  */
  Terrain::TerrainManager* GetTerrainManager( void ) { return m_pTerrainManager; }


  /**
   *  @brief MoveModelManagerの取得
   */
  MoveModel::FieldMoveModelManager* GetMoveModelManager( void );

  /**
  * @brief カメラマネージャ取得
  *
  * @return カメラマネージャ
  */
  Camera::CameraManager* GetCameraManager( void ) { return m_pCameraManager; }
  const Camera::CameraManager* GetCameraManager( void ) const { return m_pCameraManager; }

  /**
  * @brief カメラを考慮した入力マネージャ取得
  *
  * @return カメラを考慮した入力マネージャ
  */
  Camera::UiDeviceTranslator* GetCameraUiDeviceTranslator( void ) { return m_pCameraUiDeviceTranslator; }
  const Camera::UiDeviceTranslator* GetCameraUiDeviceTranslator( void ) const { return m_pCameraUiDeviceTranslator; }


#if PM_DEBUG
  /**
  * @brief カメラターゲット取得
  *
  * @return カメラターゲット
  */
  Camera::Target::TargetBase* GetCameraTarget( void ) { return m_pCameraTarget; }
#endif

  // ダウジング管理取得
  const Dowsing::Tool* GetDowsingTool( void ) const { return m_pDowsingTool; }

  /**
   * @brief エフェクト取得
   */
  gfl2::Effect::GFHeap* GetEffectHeap( void ){ return m_pEffectHeap; }
  gfl2::Effect::System* GetEffectSystem( void ){ return m_pEffectSystem; }
  Effect::EffectManager* GetEffectManager( void ) { return m_pEffectManager; }
  
  /**
   *  @brief  TrialModelFactory取得
   */
  TrialModel::TrialModelFactory *GetTrialModelFactory( void ){ return m_pTrialModelFactory; }

  /**
  * @brief 主人公　MoveModelの取得
  */
  MoveModel::FieldMoveModelPlayer* GetPlayerCharacter( void ) { return static_cast<MoveModel::FieldMoveModelPlayer*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER) ); }
  const MoveModel::FieldMoveModelPlayer* GetPlayerCharacter( void ) const { return static_cast<const MoveModel::FieldMoveModelPlayer*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER) ); }

  /**
  * @brief 主人公のワールド座標を取得
  */
  gfl2::math::Vector3 GetPlayerPosition( void ) { return m_playerPosition; }

  /**
  * @brief 主人公のワールド姿勢を取得
  */
  gfl2::math::Quaternion GetPlayerQuaternion( void ) { return m_playerQuaternion; }

  /**
  * @brief 主人公のロケーションを取得する
  */
  void GetPlayerLocation( Field::Location* p_location );

  /**
  * @brief エンカウント管理システムの取得
  */
  Encount::Encount* GetEncount( void ) { return m_pEncount; }
  
  /**
  * @brief フィールドの下画面管理システムの取得
  */
  Field::SubScreen::FieldSubScreenProcManager* GetFieldSubScreenManager( void ){ return m_pSubScreen; }
  /**
  * @brief フィールドの下画面の取得
  */
  Field::SubScreen::FieldSubScreen* GetSubScreen( void );

  /**
   *  @brief  ギミック取得
   *  @param  idx enum Field::GimmickIndex
   */
  FieldGimmick *GetFieldGimmick( GimmickIndex idx ){ return m_pGimmick[ idx ]; }

  /**
   *  @brief  ギミックID取得
   *  @param  idx enum Field::GimmickIndex
   */
  u32 GetFieldGimmickID( GimmickIndex idx );

  /**
   *  @brief  StaticModelManagerの取得
   */
  StaticModel::StaticModelManager * GetStaticModelManager(){ return m_pStaticModelManager; }

  /**
   *  @brief アクションボタンマネージャーの取得
   */
  Field::ActionButton::ActionButtonManager * GetActionButtonManager(){ return m_pActionButtonManager; }

  /**
   *  @brief  往来Managerの取得
   */
  Field::TrafficManager *GetTrafficManager( void );

  /**
  * @brief Field::EnvManagerの取得
  */
  EnvManager* GetFieldEnvManager( void ) const { return m_pFieldEnvManager; }

  /**
  * @brief 主人公ドレスアップパラメータのフィールドワーク取得
  */
  poke_3d::model::DressUpParam* GetPlayerDressUpParamWork(void) { return &m_DressUpParam; }

  /**
  * @brief ドロップアイテム生成モジュール取得
  */
  DropItem::Creator* GetDropItemCreator(void) { return m_pDropItemCreator; }

  /**
  * @brief 天候管理クラス取得
  */
  weather::WeatherControl* GetWeatherControl(void) const { return m_pWeatherControl; }
 
  /**
  * @brief 育て屋システムの取得
  */
  inline FieldSodateya* GetSodateya( void  ) const { return m_pSodateya; }

  /**
  * @brief スタッフロールモジュールの取得
  */
  App::StaffList::FieldStaffList* GetFieldStaffroll(void) { return m_pFieldStaffList; }


  /**
   *  @brief フィールドイベント用ヒープが存在しているかチェック
   */
  bool IsFieldExtHeapExist()const { return m_pFieldExtHeap != NULL; }

  /**
   *  @brief フィールド拡張したヒープの取得（バトルポケモンモデルやJF）
   */
  gfl2::heap::HeapBase * GetFieldExtHeap(){ return m_pFieldExtHeap; }

  /**
   *  @brief 動作モデル影管理クラスの取得
   */
  MoveModel::FieldMoveModelShadowManager* GetMoveModelShadowManager() const { return m_pFieldMoveModelShadowManger; }

  /**
   *  @brief 天球データ管理クラスの取得
   */
  Skybox::DataManager* GetSkyboxDataManager() const { return m_pSkyboxDataManager; }

  /**
  *  @brief プレイヤー陰影モジュール取得
  */
  Shade::FieldPlayerShade* GetFieldPlayerShade() const { return m_pFieldPlayerShade; }
  
  //エンディング用スタッフロールモジュール準備公開関数
  bool InitializeFieldStaffrollReady();
  bool InitializeFieldStaffrollReadyWait();
  // エンディング用スタッフロールモジュール破棄準備公開関数
  void TerminateFieldStaffrollReady();
  bool TerminateFieldStaffrollReadyWait();

#if PM_DEBUG
  /**
  * @brief フィールドデバッグメニュー管理を取得
  */
  Field::Debug::FieldDebugMenu* GetFieldDebugMenu( void ){ return m_pFieldDebugMenu; };
  Field::DebugInfoWin* GetFieldDebugInfoWin( void ) { return m_pDebugInfoWin; }
#endif

  //
  // 設定系
  //
  

  /**
  * @brief プレイヤーを作成
  */
  void CreatePlayer(b32 setSRT = true);

  void SetShopBuyEvent( App::Event::ShopBuyEvent * ev ){ m_pShopBuyEvent = ev; };


#if PM_DEBUG 
  // FileRead内容の監視
  void UpdateCheckFileRead_InitializeTime();
  void UpdateCheckFileRead_NormalTime();
#endif

private:
  /**------------------------------------------------------------------
   *      private定義
   ------------------------------------------------------------------*/
  typedef bool (Fieldmap::*StateFunc)( int* p_seq );
  /**
   *  @class Fieldmapの各処理を行うクラス
   *   初期化処理や破棄処理に新しい処理を追加したい場合は、
   *   InitializeState関数内で、
   *   m_InitializeStateList  m_TerminateStateListに処理を追加する。
   *
   */
  class State
  {
  public:
    State() : m_Seq(0), m_isFinish(false), m_pFunc(NULL), m_pInstance(NULL){}
    State( Fieldmap* instance, Fieldmap::StateFunc func ) : m_Seq(0), m_isFinish(false), m_pFunc(func), m_pInstance(instance){}
    virtual ~State(){}

    inline bool Execute( void ){
      if(m_pFunc){
        if( (m_pInstance->*m_pFunc)(&m_Seq) ){
          m_isFinish = true;
          return true;
        }
        return false;
      }
      return true;
    }
    inline bool IsFinish() const { return m_isFinish; }

  private:
    int m_Seq;
    bool m_isFinish;
    Fieldmap::StateFunc m_pFunc;
    Fieldmap* m_pInstance;
  };

  //! ステートインデックス 
  enum
  {
    INITIALIZE_STATE_MANAGER_INITIALIZE = 0,
    INITIALIZE_STATE_SCRIPT_INITIALIZE,
    INITIALIZE_STATE_READY,
    INITIALIZE_STATE_NUM,

    TERMINATE_STATE_RENDER_SKIP = 0,
    TERMINATE_STATE_SCRIPT_TERMINATE,
    TERMINATE_STATE_MANAGER_TERMINATE,
    TERMINATE_STATE_NUM,
  };

  //! レイアウトレンダリング用ローカルヒープサイズ
  static const u32 APP_RENDERING_MANAGER_HEAP_SIZE;
  static const u32 APP_RENDERING_MANAGER_HEAP_DEVICE_SIZE;

  /**------------------------------------------------------------------
   *      private関数
   ------------------------------------------------------------------*/
  //! ヒープ生成・破棄
  void InitializeHeap( void );
  void TerminateHeap( void );

  //! 初期化/破棄ステータス　生成・破棄
  void InitializeState( void );
  void TerminateState( void );

  //! レンダリングパイプライン　生成・破棄・生成待ち
  void InitializeRenderingPipeLine(void);
  void TerminateRenderingPipeLine(void);
  bool WaitInitializeRenderingPipeLine(void);
  bool CanDestroyRenderingPipeLine(void);

  // クリアレンダーターゲット設定を変更
  void TempOldClearRenderTargetSetting();
  void SetClearRenderTarget();
  void ResetClearRenderTarget();

  // 更新処理
  void UpdateNormal( void );

  // ゾーン更新処理
  void UpdateZone( Field::MoveModel::FieldMoveModel* pPlayerCharacter );
  void updateZone_ScriptTop( ZoneID zoneId );
  void updateZone_ScriptBottom( ZoneID zoneId );
  void updateZone_PlacementDataDelete();
  void updateZone_PlacementDataCreate( ZoneID zoneId );
  void updateZone_TrafficDelete();
  void updateZone_TrafficCreate();

  // 環境音の更新
  void updateEnvSoundManager();

  // 3DModelInstanceのインスタンス破棄処理
  void Field3DObjectNodeExecuteTerminate();

  // ギミック
  void InitializeGimmick();
  b32 UpdateCreateGimmick();
  void ReadyGimmick();
  void ReadyGimmickAfter();
  b32 FinishGimmick();
  void TerminateGimmick();
  void UpdateFirstGimmick();
  void UpdateLastGimmick();
  void UpdateTraverseBeforeGimmick();
  void UpdateCalculateAfterGimmick();
  bool IsBattleFesGimmick();
  bool IsJoinFestaGimmick();
  bool IsBattleInstGimmick();
  bool IsNpcHeapLargeZone();

  // 育て屋
  void CreateSodateya();
  void DeleteSodateya();

  // エフェクトシステム
  void InitializeEffectSystem();
  bool InitializeWaitEffectSystem();
  void UpdateEffectSystem();
  void TerminateEffectSystem();

  // 往来
  void CreateTraffic();
  void TerminateTraffic();

  // PlacementData
  void CreatePlacementData();
  void TerminateReqPlacementData();
  void TerminatePlacementData();

  // MoveModel
  void InitializeMoveModel();
  void StartLoadMoveModel();
  b32 WaitLoadMoveModel();
  void SetupMoveModel();
  void UpdateCollision();
  void TerminateMoveModel();
  u32 GetPlayerCharacterID() const;

  /**
   * @brief ライド状態に復帰
   */
  void StartRecoverPlayerRide();
  bool WaitRecoverPlayerRide();

  /**
   * @brief サブオブジェクトの復帰
   */
  void StartRecoverSubobject();
  bool WaitRecoverSubobject();

  // Camera
  void InitializeCamera();

  //@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　Mcatコメント内有るaの案件対処
  void SetSkyboxDarkFlag();

  // Skybox
  void StartCreateSkybox();
  b32 UpdateCreateSkybox();
  void CreateSkyboxAndEnvManager();
  void ReadyEnvManager();
  void UpdateEnvManager();
  void TerminateSkyboxAndEnvManager();

  // StaticModel
  void InitializeStaticModel();
  void ReadyStaticModel();
  void UpdateStaticModel();
  void TerminateStaticModel();

  // 地形
  void InitializeTerrain();
  b32 WaitTerrainSystemInitialize();
  void SetupBlockControlPositionTerrain();
  b32 UpdateSetUpTerrain();
  void UpdateTerrain();
  void TerminateTerrain();

  // ドロップアイテム生成モジュール
  void InitializeDropItemCreator();
  void TerminateDropItemCreator();

  // プレイヤー陰影モジュール
  void InitializePlayerShade();
  void TerminatePlayerShade();

  // エフェクトモジュール
  void InitializeEffectDll();
  void TerminateEffectDll();

  // アローラ時間の取得
  void GetAlolaRealTime( s32* hour, s32* minute, s32* second ) const ;

  // エンディング用スタッフロールモジュール
  bool InitializeFieldStaffroll();
  void TerminateFieldStaffroll();

#if PM_DEBUG 
  // RTTIの初期化・破棄
  void InitializeDeubgRtti();
  void UpdateDebugRtti();
  void DrawDebugRtti( gfl2::gfx::CtrDisplayNo displayNo);
  void FinalineDebugRtti();
#endif

#if PM_DEBUG
  void InitializeDebugDll();
  void TerminateDebugDll();
  void DebugRegistRender();
#endif // PM_DEBUG

  /**------------------------------------------------------------------
   *      State関数
   ------------------------------------------------------------------*/
  bool State_ManagerInitialize( int* p_seq );
  bool State_ScriptInitialize( int* p_seq );
  bool State_Ready( int* p_seq );

  bool State_RenderSkip( int* p_seq );
  bool State_ScriptTerminate( int* p_seq );
  bool State_ManagerTerminate( int* p_seq );

private:
  /**------------------------------------------------------------------
   *      メンバ変数
   ------------------------------------------------------------------*/
  s32   m_Seq;  // シーケンス管理
  FieldmapDesc m_Desc;  ///< 初期化パラメータ
  u16   m_CloseMode;  ///< 終了モード
  b8    m_CloseFlag;  ///< 終了フラグ
  u8    padding;
  b32   m_ReadyFlag;  ///< 準備完了

  // ステータス管理
  gfl2::util::List<State> m_InitializeStateList;
  gfl2::util::List<State> m_TerminateStateList;
  

  // ヒープ管理
  gfl2::heap::HeapBase *  m_pFieldHeap;       //! フィールドヒープ
  gfl2::heap::HeapBase *  m_pFieldDeviceHeap; //! フィールドデバイスヒープ
  System::nijiAllocator *   m_pFieldDeviceHeapAllocator;  //! フィールドデバイスヒープアロケーター
  gfl2::heap::HeapBase * m_pFieldExtHeap;   //! 拡張ヒープ

  ///! ゲームマネージャー
  GameSys::GameManager* m_pGameManager;

  ///! フィールド常駐
  FieldResident* m_pFieldResident;

  World*  m_pWorld; ///! フィールドワールドデータ
  Area*   m_pArea; ///! フィールドエリアデータ
  ZoneID m_ZoneId;  ///! 今いるゾーンID
  ZoneID m_reqZoneId; ///! ゾーンチェンジするID
  u32    m_seqZoneChange; ///! ゾーン変更のシーケンス
  PlacementDataManager* m_pPlacementDataManager;  ///! 配置情報マネージャー
  FieldEventCheck * m_pEventCheck; ///! イベントチェック

  // PipeLine管理
  FieldRenderingPipeLineManager*  m_pFieldRenderingPipeLineManager;
  MyRenderingPipeLine*            m_pRenderingPipeLine;
  app::util::AppRenderingManager* m_pAppRenderingManager;
  app::util::Heap*                m_pAppRenderingHeap;

  // コリジョンシステム
  BaseCollisionScene*     m_pCollsionSceneForTerrain;
  BaseCollisionScene*     m_pCollsionSceneForStaticModel;
  BaseCollisionScene*     m_pCollsionSceneForTrafficModel;
  BaseCollisionScene*     m_pCollsionSceneForOshikaeshiNaminori;
  BaseCollisionScene*     m_pCollsionSceneForOshikaeshiRoba2;
  BaseCollisionScene*     m_pCollsionSceneForOshikaeshiRide;

  //サブウインドウ
  Field::SubScreen::FieldSubScreenProcManager* m_pSubScreen;
  
  // 地形システム
  Terrain::TerrainManager*               m_pTerrainManager;

  // カメラシステム
  Camera::Target::TargetBase*             m_pCameraTarget;
  Camera::CameraManager*                  m_pCameraManager;
  Camera::UiDeviceTranslator*             m_pCameraUiDeviceTranslator;


  // ダウジング
  Dowsing::Tool*                          m_pDowsingTool;

  // エフェクトシステム
  gfl2::Effect::GFHeap                    *m_pEffectHeap;
  gfl2::Effect::System                    *m_pEffectSystem;
  gfl2::util::List<u32>                   *m_pEffectGroupIDList;
  Effect::EffectManager                   *m_pEffectManager;

  // ギミック
  FieldGimmick                            *m_pGimmick[ GIMMICK_IDX_MAX ];

  Field::FieldWindow::FieldWindowManager  *m_pFieldWindowManager;

  MoveModel::FieldMoveModelShadowManager  *m_pFieldMoveModelShadowManger;

#if PM_DEBUG
  // DrawUtil
  gfl2::util::DrawUtilText* m_pDebugDrawUtilText;     // RTTIデバッグの描画に必要なDrawUtilText用フォント
  gfl2::str::ResFont*       m_pDebugResFont;          // RTTIデバッグの描画に必要なDrawUtilText用フォントリソース
  gfl2::heap::NwAllocator*  m_pDebugMemAllocator;     // RTTIデバッグの描画に必要なDrawUtilText用アロケータ

  DebugInfoWin * m_pDebugInfoWin;
#endif

#if PM_DEBUG
  gfl2::ro::Module* m_pDebugDllModule;
  Field::Debug::FieldDebugMenu* m_pFieldDebugMenu;
#endif

  StaticModel::StaticModelManager*       m_pStaticModelManager;

  System::Skybox::Skybox*                m_pSkybox;
  Skybox::DataManager*                   m_pSkyboxDataManager;
  EnvManager*                            m_pFieldEnvManager;

  Encount::Encount*                      m_pEncount;

  Field::ActionButton::ActionButtonManager *m_pActionButtonManager;

  bool    m_isSkyboxKeep;   // 天球リソース保持フラグ

  gfl2::math::Vector3                    m_playerPosition;
  gfl2::math::Quaternion                 m_playerQuaternion;

  GimmickEncount::Manager*               m_pGimmickEncountManager;

  TrialModel::TrialModelFactory         *m_pTrialModelFactory;

  DropItem::Creator*                     m_pDropItemCreator;

  // 着せ替えパラメータ
  poke_3d::model::DressUpParam m_DressUpParam;

  GameSys::GameManager::BUFFER_CLEAR_SETTING m_OldMainDispClearSetting;

  weather::WeatherManager*              m_pWeatherManager;
  weather::WeatherControl*              m_pWeatherControl;

  App::Event::ShopBuyEvent* m_pShopBuyEvent;

  // アクションコマンド
  Field::MoveModel::FieldActionCommandManager* m_pFieldActionComandManager;

  //! 育て屋管理システム
  FieldSodateya* m_pSodateya;

#if PM_DEBUG
  //! デバッグ描画OFF
  s32 m_nDebugVisibleStaticModel_Old;
  s32 m_nDebugVisibleMoveModel_Old;
  s32 m_nDebugVisiblePlayer_Old;
#endif


  // プレイヤー陰影モジュール
  Shade::FieldPlayerShade* m_pFieldPlayerShade;

  // エフェクトモジュール
  gfl2::ro::Module* m_pEffectCommonModule;
  gfl2::ro::Module* m_pEffectUniqueModule;
  gfl2::ro::Module* m_pEffectJoinFestaModule;

  //エンディング用　スタッフロールモジュール
  App::StaffList::FieldStaffList* m_pFieldStaffList;
  gfl2::ro::Module* m_pStaffrollModule;
};


GFL_NAMESPACE_END( Field );

#endif  // __FIELDMAP_H__

