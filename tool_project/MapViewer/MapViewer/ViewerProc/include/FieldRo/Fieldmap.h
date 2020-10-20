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
///#if !defined( __FIELDMAP_H__ )
///#define __FIELDMAP_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_DrawUtilText.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

// niji
#include "GameSystem/include/GameProc.h"
#include "System/include/GflvUse.h"
#include "System/include/nijiAllocator.h"

// Field
#include "ViewerProc/include/FieldRo/FieldRootNode.h"
///#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"
///#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
///#include "Field/FieldStatic/include/FieldWorld.h"
///#include "Field/FieldStatic/include/FieldArea.h"
///#include "Field/FieldStatic/include/FieldLocation.h"

// RTTIデバッグシステム
#include <system/include/InputDevice/gfl2_Pad.h>
///#include "Field/FieldStatic/include/RttiDebug/RttiEditor.h"
///#include "Field/FieldStatic/include/RttiDebug/RttiManager.h"

//レイアウト関連
///#include "AppLib/include/Util/AppRenderingManager.h"
#include "GameSystem/include/GameData.h"

#include "ViewerProc/include/ReadViewerData.h";

//----------------------------------------------------------------------------
///クラスの前方宣言
//----------------------------------------------------------------------------

// コリジョンシステム
class BaseCollisionScene;
/*
namespace System {
  namespace Skybox {
    class Skybox;
  }
}
*///@todo_saito
GFL_NAMESPACE_BEGIN( Field );

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          前方宣言
*/
//-----------------------------------------------------------------------------
/**
GFL_NAMESPACE_BEGIN( SubScreen );
class FieldSubWindowProcManager;
GFL_NAMESPACE_END( SubScreen );
*/ //@saito_del
class GLAllocatorFromHeap;
class MyRenderingPipeLine;
class EnvManager;
///class FieldEventCheck;
class FieldResident;
/**
namespace StaticModel {
  class StaticModelManager;
}

namespace Debug
{
  class FieldDebugMenu;
}
*///@saito_del
GFL_NAMESPACE_BEGIN( Terrain );
class TerrainManager;
GFL_NAMESPACE_END( Terrain );

GFL_NAMESPACE_BEGIN( Camera );
class CameraManager;
GFL_NAMESPACE_BEGIN( Target );
class TargetBase;
GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );

GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModelManager;
GFL_NAMESPACE_END( MoveModel );
/**
namespace FieldWindow{
  class FieldWindowManager;
}

GFL_NAMESPACE_BEGIN( Effect );
class EffectManager;
GFL_NAMESPACE_END( Effect );

GFL_NAMESPACE_BEGIN( Encount );
class Encount;
GFL_NAMESPACE_END( Encount );

GFL_NAMESPACE_BEGIN( ActionButton );
class ActionButtonManager;
GFL_NAMESPACE_END( ActionButton );
*///@saito_del


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// 復帰モード
enum
{
  FM_OPEN_MODE_INIT,        //< 初期化
  FM_OPEN_MODE_APP_RECOVER, //< アプリ復帰
};

/**
//// フィールドマップ初期化パラメータ
struct FieldmapDesc
{
  u32       openMode;
  Location playerLocation;
};
*///@saito_del
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
  /**------------------------------------------------------------------
   *      公開定義
   ------------------------------------------------------------------*/

  /**------------------------------------------------------------------
   *      公開関数
   ------------------------------------------------------------------*/
  inline FieldRootNode* GetFieldRootNode(){ return GFL_SINGLETON_INSTANCE( GameSystem::GameManager )->GetGameData()->GetFieldRootNode(); }

  ///inline Field::FieldWindow::FieldWindowManager* GetFieldWindowManager( void ) const { return m_pFieldWindowManager; }

  /// 準備完了チェック
  inline bool IsReady() const{ return m_ReadyFlag; }

  /// 終了リクエスト
  inline void CloseReq() { m_CloseFlag = true; }

  inline GameSystem::GameManager* GetGameManager() const {  return m_pGameManager;  }

  /// ゾーンID
  ///inline u32 GetZoneID() const { return m_ZoneId; }

  /// ヒープ取得
  inline gfl2::heap::HeapBase* GetHeap() { return m_pFieldHeap; }
  /// デバイスヒープ取得
  inline gfl2::heap::HeapBase* GetDeviceHeap() { return m_pFieldDeviceHeap; }
  /// デバイスヒープ　アロケーター取得
  inline System::nijiAllocator * GetDeviceHeapAllocator() { return m_pFieldDeviceHeapAllocator; }

  /// ロケーション取得
  ///inline const Location& GetLocatioin() { return m_Desc.playerLocation; }

public:
  /**------------------------------------------------------------------
   ------------------------------------------------------------------*/
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  Fieldmap( /*const FieldmapDesc& desc*/ );

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
  bool Initialize( GameSystem::GameManager* pGameManager );

  /**
  * @brief    PROCのメイン更新を行う関数
  * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  bool Update( void );

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
  ///void EventCheck();

  //
  // 取得系
  //

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
  
  /**
  * @brief 主人公　MoveModelの取得
  */
  ///MoveModel::FieldMoveModel* GetPlayerCharacter( void ) { return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER); }

  /**
  * @brief エンカウント管理システムの取得
  */
  ///Encount::Encount* GetEncount( void ) { return m_pEncount; }


  //
  // 設定系
  //
  
  // @todo 仮
  ///void SetPokeFinderFlag( b32 flag ){ m_IsPokeFinder = flag; }

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

  //! レンダリングパイプライン　生成・破棄
  void InitializeRenderingPipeLine(void);
  void TerminateRenderingPipeLine(void);

  // 更新処理
  void UpdateNormal( void );


  // 3DModelInstanceのインスタンス破棄処理
  void Field3DObjectNodeExecuteTerminate();

  /**------------------------------------------------------------------
   *      State関数
   ------------------------------------------------------------------*/
  bool State_ManagerInitialize( int* p_seq );
  ///bool State_ScriptInitialize( int* p_seq );
  bool State_Ready( int* p_seq );

  bool State_RenderSkip( int* p_seq );
  ///bool State_ScriptTerminate( int* p_seq );
  bool State_ManagerTerminate( int* p_seq );

private:
  /**------------------------------------------------------------------
   *      メンバ変数
   ------------------------------------------------------------------*/
  s32   m_Seq;  // シーケンス管理
  ///FieldmapDesc m_Desc;  ///< 初期化パラメータ
  b32   m_CloseFlag;  ///< 終了フラグ
  b32   m_ReadyFlag;  ///< 準備完了

  // ステータス管理
  gfl2::util::List<State> m_InitializeStateList;
  gfl2::util::List<State> m_TerminateStateList;
  

  // ヒープ管理
  gfl2::heap::HeapBase *  m_pFieldHeap;       //! フィールドヒープ
  gfl2::heap::HeapBase *  m_pFieldDeviceHeap; //! フィールドデバイスヒープ
  System::nijiAllocator *   m_pFieldDeviceHeapAllocator;  //! フィールドデバイスヒープアロケーター
  gfl2::gfx::IGLAllocator* m_pFieldGfglHeapOld; //! 元設定されていたアロケータ保存用ワーク

  ///! ゲームマネージャー
  GameSystem::GameManager* m_pGameManager;

  ///! フィールド常駐
  FieldResident* m_pFieldResident;

  ///World*  m_pWorld; ///! フィールドワールドデータ
  ///Area*   m_pArea; ///! フィールドエリアデータ
  ///ZoneID m_ZoneId;  ///! 今いるゾーンID
  ///PlacementDataManager* m_pPlacementDataManager;  ///! 配置情報マネージャー
  ///FieldEventCheck * m_pEventCheck; ///! イベントチェック

  // PipeLine管理
  gfl2::gfx::Texture*     m_pDepthStencilTexture;
  MyRenderingPipeLine*    m_pRenderingPipeLine;
	void*                   m_pOutLineResBuf;
  void*                   m_pEdgeMapResBuf;

  ///app::util::AppRenderingManager* m_pAppRenderingManager;
  ///app::util::Heap*                m_pAppRenderingHeap;

  // コリジョンシステム
  BaseCollisionScene*     m_pCollsionSceneForTerrain;
  BaseCollisionScene*     m_pCollsionSceneForStaticModel;

  //サブウインドウ
  ///Field::SubScreen::FieldSubWindowProcManager* m_pSubWindow;
  
  // 地形システム
  Terrain::TerrainManager*               m_pTerrainManager;

  // カメラシステム
  Camera::Target::TargetBase*             m_pCameraTarget;
  Camera::CameraManager*                  m_pCameraManager;

  // エフェクトシステム
  ///Effect::EffectManager                   *m_pEffectManager;

  ///Field::FieldWindow::FieldWindowManager  *m_pFieldWindowManager;

  // RTTIデバッグシステム
#if USE_RTTI_DEBUG
  gfl2::util::DrawUtilText* m_pDrawUtilText;     // RTTIデバッグの描画に必要なDrawUtilText用フォント
  gfl2::str::ResFont*       m_pResFont;          // RTTIデバッグの描画に必要なDrawUtilText用フォントリソース
  gfl2::heap::NwAllocator*  m_pMemAllocator;     // RTTIデバッグの描画に必要なDrawUtilText用アロケータ
  gfl2::heap::NwAllocator*  m_pDevAllocator;     // RTTIデバッグの描画に必要なDrawUtilText用アロケータ
  b32                       m_isDrawRttiEditor;  // RTTIデバッグの描画オンorオフ
#endif

#if PM_DEBUG
  ///Field::Debug::FieldDebugMenu* m_pFieldDebugMenu;
#endif

  ///StaticModel::StaticModelManager*       m_pStaticModelManager;

  ///System::Skybox::Skybox*                m_pSkybox;@todo_saito
  EnvManager*                            m_pFieldEnvManager;

  ///Encount::Encount*                      m_pEncount;

  ///Field::ActionButton::ActionButtonManager *m_pActionButtonManager;

  //===========================================================================================
  // todo: システムの確認用コード : ここから
  //===========================================================================================

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;

	GLResourceAllocator m_ResourceAllocator;
	void* m_pBinData;

  // @todo 仮カメラ制御情報
  ///b32                       m_IsPokeFinder;

  // @todo 着せ替えパラメータ
  ///poke_3d::model::DressUpParam m_DressUpParam;

  //===========================================================================================
  // todo: システムの確認用コード : ここまで
  //===========================================================================================

};


GFL_NAMESPACE_END( Field );

///#endif  // __FIELDMAP_H__

