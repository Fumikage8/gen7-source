//===================================================================
/**
 * @file    StaticModelManager.h
 * @brief   配置モデルの管理クラス
 * @author  saita_kazuki
 * @date    2015.04.08
 */
//===================================================================

#if !defined( STATIC_MODEL_MANAGER_H_INCLUDED )
#define STATIC_MODEL_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Math.h>
#include <math/include/gfl2_Quaternion.h>
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"
#include "Field/FieldStatic/include/FieldManagerNode.h"


// 前方参照
namespace gfl2 {

  namespace renderingengine { namespace scenegraph { namespace resource {
    class ResourceNode;
  }}}

  namespace fs {
    class BinLinkerAccessor;
    class AsyncFileManager;
  }
}

namespace Field {

  class FieldRootNode;

  namespace StaticModel {
    class StaticModel;
    class StaticModelSEPlayer;
    class StaticModelResourceNodeManager;
  }
}

namespace System {
  class nijiAllocator;
}

class CollisionSceneDefault;
class DynamicActor;


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @class 配置モデル管理クラス
 */
class StaticModelManager : public IFieldManagerNode
{
  GFL_FORBID_COPY_AND_ASSIGN(StaticModelManager); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @class 初期化用データアクセサー
   */
  class InitializeDataAccessor
  {
  public:
    InitializeDataAccessor() :
      pHeap( NULL),
      pResourcePackBinLinker( NULL),
      pFieldRootNode( NULL),
      pSETableData( NULL)
    {}
    virtual ~InitializeDataAccessor(){}

    gfl2::heap::HeapBase*           pHeap;                  ///< ヒープ。配置モデル専用ローカルヒープの親にもなる
    gfl2::fs::BinLinkerAccessor*    pResourcePackBinLinker; ///< 配置モデルリソースパックにアクセスできるBinLinker
    FieldRootNode*                  pFieldRootNode;         ///< フィールドルートノード
    void*                           pSETableData;           ///< SEテーブルバイナリデータ
  };


  static const u8   STATIC_MODEL_MAX = 24;          ///< 配置モデル最大数
  static const u8   STATIC_MODEL_DYNAMIC_MAX = 4;   ///< 動的配置モデル最大数

  static const u8   STATIC_MODEL_DYNAMIC_OFFSET = STATIC_MODEL_MAX; ///< 動的配置モデルのワーク番号オフセット

public:

  /**
   * @brief 配置モデルマネージャー生成
   * @param pAllocator 生成に使用するアロケーター
   * @return 配置モデルマネージャーのポインタ
   */
  static StaticModelManager* CreateStaticModelManager( gfl2::gfx::IGLAllocator* pAllocator );

  /**
   * @brief   コンストラクタ
   */
  StaticModelManager();

  /**
   * @brief   デストラクタ
   */
  virtual ~StaticModelManager();

  /**
   * @brief ファイル読み込み管理
   */
  virtual void StartFileRead( void );
  virtual bool WaitFileRead( void );

  /**
   * @brief 使用可能状態にセットアップ
   */
  virtual bool Setup( void );

  /**
   * @brief 破棄
   */
  virtual bool Finalize( void );

  /**
   * @brief 破棄の必要があるか。
   */
  virtual bool IsExists( void ) const ;

  /**
   * @brief 初回初期化
   * @param initData モデル生成などに必要なデータが格納されたアクセサ
   */
  void InitializeFirst( InitializeDataAccessor& initDataAccessor);

  /**
   * @brief Readyで1度だけ呼び出す
   * @param pDynamicActor 当たる側のアクター
   */
  void Ready( DynamicActor* pDynamicActor);

  /**
   * @brief バウンディングボックスアニメーションコリジョン更新
   * @param pDynamicActor 当たる側のアクター
   */
  void UpdateBBAnimationCollision( DynamicActor* pDynamicActor);

  /**
   * @brief 更新処理
   */
  void Update();

  /**
  *  @brief  カリング
  *  @param  ビュープロジェクション行列
  */
  void UpdateCulling(gfl2::math::Matrix44 viewProj);

  /**
   * @brief 配置モデルの取得
   * @param index 配置モデルの番号
   * @return 配置モデルのポインタ。indexがオーバーしていたらNULL
   */
  StaticModel* GetStaticModel( u32 index) const ;

  /**
   * @brief イベントIDで配置モデルの取得
   * @param eventID イベントID
   * @return 配置モデルのポインタ。見つからなければNULL
   */
  StaticModel* GetEventIDStaticModel( u32 eventID) const ;

  /**
   * @brief 配置モデルIDで配置モデルの取得
   * @param staticModelID 配置モデルID
   * @return 配置モデルのポインタ。見つからなければNULL
   */
  StaticModel* GetStaticModelIDStaticModel( u32 staticModelID) const ;

  /**
   * @brief インスタンスの遅延破棄リクエスト発行
   */
  void RequestDelayTerminate();

  /**
   * @brief 配置モデルの生成
   * @param modelID 配置モデルID
   * @param eventID イベントID
   * @param pos 初期位置
   * @param qt 向き
   * @return 生成した配置モデルのポインタ
   */
  StaticModel* CreateModel( u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt);

  /**
   * @brief 配置モデルの生成
   * @param modelID 配置モデルID
   * @param eventID イベントID
   * @param pos 初期位置
   * @param qt 向き
   * @return 生成した配置モデルのポインタ
   */
  StaticModel* CreateModelDynamic( u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt);

  /**
   * @brief バウンディングボックス用コリジョンシーンの取得
   * @return バウンディングボックス用コリジョンシーン
   */
  CollisionSceneDefault* GetCollisionSceneForBBAnimation() const ;

  /**
   * @brief 配置モデルから管理クラスにTerminate処理をしたことを通知する関数
   * @param pStaticModel 配置モデルのポインタ
   */
  void TerminateNotification( StaticModel* pStaticModel);

#if PM_DEBUG

  /**
   * @brief 配置モデルの生成
   * @param pHeap 配置モデルが使用するヒープ
   * @param pAllocator 配置モデルが使用するヒープアロケーター
   * @param pData 配置モデルバイナリパックデータ
   * @param modelID 配置モデルID
   * @param pModelData 配置モデルパックバイナリデータ
   * @param pos 初期位置
   * @param qt 向き
   * @return 生成した配置モデルのポインタ
   */
  StaticModel* CreateModelDebug( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pAllocator, void* pModelData, u32 modelID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt);

  /**
   * @brief 配置モデルの生成
   * @param modelID 配置モデルID
   * @param pModelData 配置モデルパックバイナリデータ
   * @param pos 初期位置
   * @param qt 向き
   * @return 生成した配置モデルのポインタ
   */
   StaticModel* CreateModelDebug( void* pModelData, u32 modelID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt);

  /**
   * @brief 配置モデルの生成
   * @param modelID 配置モデルID
   * @param pModelData 配置モデルパックバイナリデータ
   * @param pos 初期位置
   * @param qt 向き
   * @return 生成した配置モデルのポインタ
   */
  StaticModel* CreateModelDynamicDebug( void* pModelData, u32 modelID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt);

  /**
   * @brief ゾーンIDセット
   */
  void SetZoneID( u32 zoneID) { m_zoneID = zoneID; }

  //
  // @fix CONNMCat[582]:攻略本用マップ撮影機能、ローカルアニメを特定のフレームで停止する機能
  //

  /**
  * @brief ローカルアニメを指定フレームで一時停止させる
  *
  * @param frame 指定フレーム
  *
  * @return 無し
  */
  void DebugSuspendLocalAnimation( f32 frame );

  /**
  * @brief ローカルアニメの一時停止を解除
  *
  * @return 無し
  */
  void DebugResumeLocalAnimation( void );


#endif // PM_DEBUG

private:

  void CreateHeap( gfl2::heap::HeapBase* pParent);
  void DeleteHeap();

  StaticModel* GetEmptyModelWork( s32* pOutIndex) const ;
  StaticModel* GetStaticModelIDToModelWork( u32 modelID) const ;
  void* GetStaticModelIDToResourcePackData( u32 modelID) const ;
  void* GetStaticModelIDToResourcePackDataEx( u32 modelID);

  StaticModel* CreateModelCore( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pAllocator, void* pModelData, u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt, bool isDynamic);
  StaticModel* CreateModelDynamicCore( void* pModelData, u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt);

  s32 GetEmptyDynamicModelIndex() const ;
  StaticModel* GetEmptyDynamicModelWork( s32* pOutIndex) const ;

private:

  StaticModel**                 m_ppStaticModelArray;           ///< 配置モデル配列
  gfl2::heap::HeapBase*         m_pHeap;                        ///< ヒープ
  gfl2::fs::BinLinkerAccessor*  m_pResourcePackBinLinker;       ///< 配置モデル情報アクセスできるBinLinker
  System::nijiAllocator*        m_pAllocator;                   ///< このクラスが使用するアロケーター
  FieldRootNode*                m_pFieldRootNode;               ///< フィールドルートノード
  StaticModelSEPlayer*          m_pSEPlayer;                    ///< SEテーブル再生クラス
  CollisionSceneDefault*        m_pCollisionSceneForBBAnimation;///< バウンディングボックスアニメーション用コリジョンシーン

  gfl2::heap::HeapBase*         m_pLocalHeapForStatic;                                      ///< それぞれの配置モデルインスタンス用ローカルヒープ。配置など決まったタイミングで確保されるものはこれを使用
  gfl2::heap::HeapBase*         m_pLocalHeapForDynamic[ STATIC_MODEL_DYNAMIC_MAX ];         ///< それぞれの配置モデルインスタンス用ローカルヒープ。ドロップアイテムなど動的に確保されるものはこれを使用
  System::nijiAllocator*        m_pModelAllocatorForStatic;                                 ///< 静的生成用配置モデル専用のアロケーター
  System::nijiAllocator*        m_pModelAllocatorForDynamic[ STATIC_MODEL_DYNAMIC_MAX ];    ///< 動的生成配置モデル個々専用のアロケーター

  StaticModel**                 m_ppStaticModelDynamicArray;                                ///< 動的生成配置モデルのポインタ。カウンタ減算のチェックに使用
  s8                            m_dynamicModelCreateNum;                                    ///< 動的生成した配置モデル数

  b32                           m_isUseCulling; ///< カリングを利用するかどうか

  StaticModelResourceNodeManager* m_pResourceNodeManager;           ///< リソースノード管理クラス

  gfl2::math::Vector3           m_playerOldPos;                     ///< プレイヤーの前回座標

  //-------------------------------------------------------------------
  // 以下デバッグ用途
  //-------------------------------------------------------------------
#if PM_DEBUG

  void SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID);
  void SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID);
  void SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize, u32 align);
  void** GetEmptyDebugModelPackDataBuffer();
  void* LoadDebugModelPackData( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 modelID);
  void TerminateDebugModelPackDataBuffer();
  char* GetModelName( u32 modelID);
  char* GetZoneName( u32 zoneID);
  void WriteTextStaticModel();

  void*                         m_pDebugModelPackDataBuffer[ STATIC_MODEL_MAX ];
  char*                         m_pDebugModelName;
  char*                         m_pDebugZoneName;
  u32                           m_zoneID;
  gfl2::heap::HeapBase*         m_pDebuggerHeap;
  gfl2::fs::AsyncFileManager*   m_pAsyncFileManager;

#endif // PM_DEBUG

};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // STATIC_MODEL_MANAGER_H_INCLUDED
