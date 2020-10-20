//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldResident.h
 *  @brief  フィールドresident
 *  @author tomoya takahashi
 *  @date   2015.05.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_RESIDENT_H__ )
#define __FIELD_RESIDENT_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <ro/include/gfl2_RoManager.h>

#include <Field/FieldStatic/include/FieldWarpData.h>

#include "GameSys/include/GameData.h"


//----------------------------------------------------------------------------
///クラスの前方宣言
//----------------------------------------------------------------------------
// コリジョンシステム
class BaseCollisionScene;
class WordSetLoader;

namespace System{
  class nijiAllocator;
}

namespace Field {
  namespace StaticModel {
    class StaticModelManager;
  }
  namespace SubScreen {
    class FieldSubScreenProcManager;
  }
  namespace GimmickEncount {
    class Manager;
  }
}


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );
class FieldSubScreen;
GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );

GFL_NAMESPACE_BEGIN(Field);

#include "niji_conv_header/field/field_resident/field_resident_pack.h"

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
class PlacementDataManager;
class EnvSoundManager;

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

class FieldResident
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldResident );

public:
  enum CollisionSceneType
  {
    COLLISION_SCENE_TERRAIN,                  /// 地形
    COLLISION_SCENE_STATIC_MODEL,             /// 配置されているモデル
    COLLISION_SCENE_TRAFFIC_MODEL,            /// 往来モデル
    COLLISION_SCENE_EVENT_POS,                /// POSイベント
    COLLISION_SCENE_EVENT_TALK,               /// 会話イベント
    COLLISION_SCENE_EVENT_PUSH,               /// 押し込みイベント
    COLLISION_SCENE_ENCOUNT,                  /// エンカウント
    COLLISION_SCENE_TRAINER_EYE,              /// トレーナー視線
    COLLISION_SCENE_OSHIKAESHI_NAMINORI,      /// 波乗り(押しかえし)
    COLLISION_SCENE_EVENT_NAMINORI,           /// 波乗り(イベント)
    COLLISION_SCENE_GIMMICK_ENCOUNT,          /// ギミックエンカウントのエンカウント判定用
    COLLISION_SCENE_GIMMICK_ENCOUNT_ACTION,   /// ギミックエンカウントの実行判定用
    COLLISION_SCENE_EVENT_INTRPOS,            /// IntrPosイベント
    COLLISION_SCENE_EVENT_FINDER,             /// ファインダーイベント
    COLLISION_SCENE_OSHIKAESHI_ROBA2,         /// 地面馬(押しかえし)
    COLLISION_SCENE_CAMERA_EYE,               /// カメラ視界
    COLLISION_SCENE_OSHIKAESHI_RIDE,          /// ライド進入禁止用
    COLLISION_SCENE_FISHING_POS,              /// 釣り警戒範囲用
    COLLISION_SCENE_MAX,
  };

public:

  FieldResident();
  ~FieldResident();

  // 初期化済みチェック
  inline bool IsInitialized() const { return m_IsInitialized; }

  //! FieldDLLが読み込み済みか？
  bool IsInitializedFieldDLL() const { return (m_pRoModule!=NULL); }

  //! DLL（読み込んだDLLは、FieldResidentのTerminateで破棄される）
  bool InitializeFieldDLL( gfl2::fs::AsyncFileManager * pAsyncFileMan );


  //! DLL破棄
  bool TerminateFieldDLL();

  // 初期化
  void Initialize( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* p_fileManager, const GameSys::GameData& gameData );

  // 初期化完了待ち
  bool InitializeWait();
  
  // 破棄
  void Terminate( void );

  // ヒープ取得
  inline gfl2::heap::HeapBase* GetFieldHeap(void) const { return m_pFieldHeap; }   ///< フィールドヒープ
  inline gfl2::heap::HeapBase* GetFieldDeviceHeap(void) const { return m_pFieldDeviceHeap; } ///< フィールドデバイスヒープ
  inline System::nijiAllocator* GetFieldDeviceHeapAllocator(void) const { return m_pFieldDeviceHeapAllocator; } ///< フィールドデバイスヒープアロケーター

  // ヒープ取得(下画面メニュー用)
  // @note 下記ヒープ内部の構成はメニュー班の管轄になります
  inline gfl2::heap::HeapBase* GetFieldSubScreenHeap(void) const { return m_pFieldSubScreenHeap; }                                ///< フィールド下画面メニュー用ヒープ
  inline gfl2::heap::HeapBase* GetFieldSubScreenDeviceHeap(void) const { return m_pFieldSubScreenDeviceHeap; }                    ///< フィールド下画面メニュー用デバイスヒープ
  inline System::nijiAllocator* GetFieldSubScreenDeviceHeapAllocator(void) const { return m_pFieldSubScreenDeviceHeapAllocator; } ///< フィールド下画面メニュー用デバイスヒープアロケーター
  
  // ヒープ取得(ファイル読み込み、解凍用テンポラリ)
  // @note 一旦、フィールドデバイスの後方参照ヒープを返しているが、今後変更される可能性が高い
  inline gfl2::heap::HeapBase* GetFieldTemporaryHeapForFileRead(void) { return m_pFieldDeviceHeap->GetLowerHandle(); } ///< ファイル読み込み、解凍用テンポラリ用ヒープ
  
  // バイナリ取得
  inline void * GetResource( u32 idx ) { return m_BinAccessor.GetData(idx); }
  inline u32 GetResourceSize( u32 idx ) { return m_BinAccessor.GetDataSize(idx); }
  
  // コリジョンシーンの取得
  inline BaseCollisionScene* GetCollisionScene( CollisionSceneType type ){ return m_pCollisionScene[type]; }

  // 配置データマネージャ取得
  inline PlacementDataManager* GetPlacementDataManager() { return m_pPlacementDataManager; }

  // 配置モデル管理クラスの取得
  inline StaticModel::StaticModelManager* GetStaticModelManager() { return m_pStaticModelManager; }

  // 環境音クラスの取得
  inline EnvSoundManager* GetEnvSoundManager() { return m_pEnvSoundManager; }

  // WordSetLoaderの取得
  inline WordSetLoader* GetWordSetLoader(){ return m_pWordSetLoader; }

  // ギミックエンカウントマネージャーの取得
  inline GimmickEncount::Manager* GetGimmickEncountManager() { return !m_pGameData->IsNowJoinFestaFieldData() ?  m_pGimmickEncountManager : NULL; }

  //フィールドサブスクリーンManager
  inline ::Field::SubScreen::FieldSubScreenProcManager* GetFieldSubScreenProcManager(void){ return m_pFieldSubScreenManager; };

#if PM_DEBUG

  void DebugSetZone( ZoneID zoneID ) { m_debugZoneID = zoneID; }
  ZoneID DebugGetZone( void ) const { return m_debugZoneID; }

#endif

private:

  //! ヒープ生成・破棄
  void InitializeHeap( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap );
  void TerminateHeap( void );

  //! 常駐リソース読み込み・破棄
  void InitializeResource();
  bool WaitInitializeResource();
  void TerminateResouce();



  //! コリジョンシーン生成
  void InitializeCollisionScene();
  void TeminateCollisionScene();

  //! 配置データマネージャー
  void InitializePlacementDataManager();
  void TerminatePlacementDataManager();

  //! 配置モデル生成・破棄
  void InitializeStaticModelManager();
  void TerminateStaticModelManager();

  //! 環境音生成・破棄
  void InitializeEnvSoundManager();
  void TerminateEnvSoundManager();

  // WordSetLoader
  void InitializeWordSetLoader();
  bool WaitInitializeWordSetLoader();
  void TerminateWordSetLoader();

  /// 天球ヒープ生成・破棄
  void InitializeSkyboxHeap();
  void TerminateSkyboxHeap();

  /// ギミックエンカウントマネージャー生成・破棄
  void InitializeGimmickEncountManager();
  void TerminateGimmickEncountManager();

  // フィールドサブスクリーン
  void InitializeFieldSubScreenManager(void);
  void TerminateFieldSubScreenManager(void);

  
private:

  bool m_IsInitialized;

  u32 m_InitializeSeq;

  gfl2::heap::HeapBase *  m_pFieldHeap;       //! フィールドヒープ
  gfl2::heap::HeapBase *  m_pFieldDeviceHeap; //! フィールドデバイスヒープ
  gfl2::heap::HeapBase *  m_pRegidentBuffHeap;//! レジデント展開用
  System::nijiAllocator *   m_pFieldDeviceHeapAllocator;  //! フィールドデバイスヒープアロケーター

  // @note 下記ヒープ内部の構成はメニュー班の管轄になります
  gfl2::heap::HeapBase*  m_pFieldSubScreenHeap;                 //! フィールド下画面メニュー用ヒープ
  gfl2::heap::HeapBase*  m_pFieldSubScreenDeviceHeap;           //! フィールド下画面メニュー用デバイスヒープ
  System::nijiAllocator* m_pFieldSubScreenDeviceHeapAllocator;  //! フィールド下画面メニュー用デバイスヒープアロケーター


  // DLL
  gfl2::ro::Module* m_pRoModule;  

  gfl2::fs::AsyncFileManager * m_pAsyncFileMan; //! ファイル読み込みマネージャ

  // 常駐リソース
  void * m_pBuffer;
  gfl2::fs::BinLinkerAccessor m_BinAccessor;

  // コリジョンシーン
  BaseCollisionScene* m_pCollisionScene[COLLISION_SCENE_MAX];

  // PlacementDataManager
  PlacementDataManager* m_pPlacementDataManager;

  // 配置モデル
  StaticModel::StaticModelManager*  m_pStaticModelManager;

  // 環境音システム
  EnvSoundManager* m_pEnvSoundManager;
  
  WordSetLoader   *m_pWordSetLoader;

  // ギミックエンカウント
  GimmickEncount::Manager*          m_pGimmickEncountManager;

  ///フィールドサブスクリーン
  ::Field::SubScreen::FieldSubScreenProcManager* m_pFieldSubScreenManager;
  
  // ゲームデータ
  const GameSys::GameData* m_pGameData;

#if PM_DEBUG

  ZoneID  m_debugZoneID;

#endif
};

GFL_NAMESPACE_END(Field);

#endif  // __FIELD_RESIDENT_H__

