//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldResident.h
 *	@brief  フィールドresident
 *	@author	tomoya takahashi
 *	@date		2015.05.26
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



//----------------------------------------------------------------------------
///クラスの前方宣言
//----------------------------------------------------------------------------
// コリジョンシステム
class BaseCollisionScene;

namespace System{
  class nijiAllocator;
}
/**
namespace Field { namespace StaticModel {
  class StaticModelManager;
}}
*/
GFL_NAMESPACE_BEGIN(Field);

#include "niji_conv_header/field/field_resident/field_resident_pack.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					前方宣言
*/
//-----------------------------------------------------------------------------
///class PlacementDataManager;

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

class FieldResident
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldResident );

public:
  enum CollisionSceneType
  {
    COLLISION_SCENE_TERRAIN,      /// 地形
    COLLISION_SCENE_STATIC_MODEL, /// 配置されているモデル
    COLLISION_SCENE_EVENT_POS,    /// POSイベント
    COLLISION_SCENE_EVENT_TALK,   /// 会話イベント
    COLLISION_SCENE_EVENT_PUSH,   /// 押し込みイベント
    COLLISION_SCENE_ENCOUNT,      /// エンカウント
    COLLISION_SCENE_MAX,          /// 
  };

public:

  FieldResident();
  ~FieldResident();

  // 初期化済みチェック
  inline bool IsInitialized() const { return m_IsInitialized; }

  // 初期化
  void Initialize( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* p_fileManager);

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
  inline gfl2::heap::HeapBase* GetFieldSubScreenHeap(void) const { return m_pFieldSubScreenHeap; }   ///< フィールド下画面メニュー用ヒープ
  inline gfl2::heap::HeapBase* GetFieldSubScreenDeviceHeap(void) const { return m_pFieldSubScreenDeviceHeap; } ///< フィールド下画面メニュー用デバイスヒープ
  inline System::nijiAllocator* GetFieldSubScreenDeviceHeapAllocator(void) const { return m_pFieldSubScreenDeviceHeapAllocator; } ///< フィールド下画面メニュー用デバイスヒープアロケーター

  // バイナリ取得
  inline void * GetResource( u32 idx ) { return m_BinAccessor.GetData(idx); }
  inline u32 GetResourceSize( u32 idx ) { return m_BinAccessor.GetDataSize(idx); }
  
  // コリジョンシーンの取得
  inline BaseCollisionScene* GetCollisionScene( CollisionSceneType type ){ return m_pCollisionScene[type]; }

  // 配置データマネージャ取得
  ///inline PlacementDataManager* GetPlacementDataManager() { return m_pPlacementDataManager; }

  // 配置モデル管理クラスの取得
  ///inline StaticModel::StaticModelManager* GetStaticModelManager() { return m_pStaticModelManager; }

private:

  //! ヒープ生成・破棄
  void InitializeHeap( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap );
  void TerminateHeap( void );

  //! 常駐リソース読み込み・破棄
  ///void InitializeResource();
  ///bool WaitInitializeResource();
  ///void TerminateResouce();

  //! DLL
  ///void InitializeFieldDLL();
  ///void TerminateFieldDLL();

  //! コリジョンシーン生成
  void InitializeCollisionScene();
  void TeminateCollisionScene();
	/**
  //! 配置データマネージャー
  void InitializePlacementDataManager();
  void TerminatePlacementDataManager();

  //! 配置モデル生成・破棄
  void InitializeStaticModelManager();
  void TerminateStaticModelManager();
	*/
private:

  bool m_IsInitialized;

  u32 m_InitializeSeq;

  gfl2::heap::HeapBase *  m_pFieldHeap;       //! フィールドヒープ
  gfl2::heap::HeapBase *  m_pFieldDeviceHeap; //! フィールドデバイスヒープ
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
  ///PlacementDataManager* m_pPlacementDataManager;

  // 配置モデル
  ///StaticModel::StaticModelManager*  m_pStaticModelManager;
};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_RESIDENT_H__

