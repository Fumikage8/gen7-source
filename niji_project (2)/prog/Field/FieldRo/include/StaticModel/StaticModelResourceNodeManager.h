//===================================================================
/**
 * @file    StaticModelResourceNodeManager.h
 * @brief   配置モデルリソースノード管理クラス
 * @author  saita_kazuki
 * @date    2016.01.26
 */
//===================================================================

#if !defined( STATIC_MODEL_RESOURCE_NODE_MANAGER_H_INCLUDED )
#define STATIC_MODEL_RESOURCE_NODE_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"
#include "Field/FieldStatic/include/FieldManagerNode.h"


// 前方参照
namespace gfl2 {
  namespace renderingengine { namespace scenegraph { namespace resource {
    class ResourceNode;
  }}}
  namespace gfx {
    class IGLAllocator;
  }
}

namespace Field {
  class FieldRootNode;
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @class 配置モデルリソースノード管理クラス
 */
class StaticModelResourceNodeManager
{
  GFL_FORBID_COPY_AND_ASSIGN(StaticModelResourceNodeManager); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @class 初期化情報
   */
  class InitializeDescription
  {
  public:
    InitializeDescription()
      : pHeap( NULL)
      , pAllocator( NULL)
      , pFieldRootNode( NULL)
      , areaResourceNum( 0)
    {}
    virtual ~InitializeDescription(){}

    gfl2::heap::HeapBase*           pHeap;                  ///< リソースノード生成に使用するヒープ
    gfl2::gfx::IGLAllocator*        pAllocator;             ///< リソースノード生成に使用するアロケーター
    FieldRootNode*                  pFieldRootNode;         ///< フィールドルートノード
    u32                             areaResourceNum;        ///< エリア内の配置モデルリソース数
  };

  static const s32        WORK_INDEX_ERROR = -1;            ///< ワーク番号エラー値

public:

  /**
   * @brief コンストラクタ
   */
  StaticModelResourceNodeManager();

  /**
   * @brief デストラクタ
   */
  virtual ~StaticModelResourceNodeManager();

  /**
   * @brief 初期化
   * @param desc 初期化情報
   */
  void Initialize( const InitializeDescription& desc);

  /**
   * @brief 破棄
   */
  void Terminate();

  /**
   * @brief リソースノード生成
   * @param modelID 配置モデルID
   * @param pStaticModelPackBinary 配置モデルパックバイナリ
   * @return 生成したワークが格納されている番号 (-1はエラー値)
   */
  s32 Create( u32 modelID, void* pStaticModelPackBinary);

  /**
   * @brief リソースノード破棄
   * @param modelID 配置モデルID
   */
  void Delete( u32 modelID);

  /**
   * @brief モデルリソースノード取得
   * @param workIndex ワーク番号。Create関数の戻り値
   * @param modelType モデルのLOD種類
   * @return 指定したモデルのリソースノード
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetModelResourceNode( s32 workIndex, ModelType modelType) const ;

  /**
   * @brief モーションリソースノード取得
   * @param workIndex ワーク番号。Create関数の戻り値
   * @param modelType モデルのLOD種類
   * @param animationIndex モーション番号
   * @return 指定したモーションのリソースノード
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionResourceNode( s32 workIndex,ModelType modelType, AnimationIndex animationIndex) const ;

private:

  /**
   * @class リソースワーク
   */
  class ResourceWork
  {
  public:

    /**
     * @brief コンストラクタ
     */
    ResourceWork();

    /**
     * @brief デストラクタ
     */
    virtual ~ResourceWork();

    /**
     * @brief リソースノード生成
     * @param pAllocator アロケーター
     * @param pFieldRootNode フィールドルートノード
     * @param pStaticModelPackBinary 配置モデルパックバイナリ
     */
    void Create( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode, void* pStaticModelPackBinary);

    /**
     * @brief リソースノード破棄
     * @param pFieldRootNode フィールドルートノード
     */
    void Delete( FieldRootNode* pFieldRootNode);

    /**
     * @brief モデルリソースノード取得
     * @param modelType モデルのLOD種類
     */
    gfl2::renderingengine::scenegraph::resource::ResourceNode* GetModelResourceNode( ModelType modelType) const ;

    /**
     * @brief モーションリソースノード取得
     * @param modelType モデルのLOD種類
     * @param animationIndex モーション番号
     */
    gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionResourceNode( ModelType modelType, AnimationIndex animationIndex) const ;

    /**
     * @brief 参照カウントの取得
     */
    u32 GetReferenceCount() const ;

    /**
     * @brief 指定したモデルIDとリソースのモデルIDが合っているか
     */
    bool IsMatchModelID( u32 modelID) const ;

  private:

    void CreateAllResourceNode( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode);
    void CreateModelResourceNode( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode, u32 modelType, void* pModelData);
    void CreateMotionResourceNode( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode, u32 modelType, u32 animationIndex, void* pMotionData);

    void DeleteAllResourceNode( FieldRootNode* pFieldRootNode);
    void DeleteModelResourceNode( FieldRootNode* pFieldRootNode, u32 modelType);
    void DeleteMotionResourceNode( FieldRootNode* pFieldRootNode, u32 modelType, u32 animationIndex);

  private:

    s32                                                         m_referenceCount;                                                     ///< 参照カウント
    void*                                                       m_pStaticModelPackBinary;                                             ///< パックバイナリ
    gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pModelResourceNode[ MODEL_TYPE_MAX ];;                              ///< モデルリソースノード
    gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pMotionResourceNode[ MODEL_TYPE_MAX ][ ANIMATION_INDEX_MAX ];       ///< モーションリソースノード
  };

private:

  s32 GetResourceWorkIndex( u32 modelID);
  s32 GetEmptyResourceWorkIndex();

private:

  gfl2::heap::HeapBase*         m_pHeap;                          ///< ワーク生成に使用するヒープ
  gfl2::gfx::IGLAllocator*      m_pAllocator;                     ///< リソースノード生成に使用するアロケーター
  FieldRootNode*                m_pFieldRootNode;                 ///< フィールドルートノード。ノードへの登録削除に使用
  u32                           m_resorceWorkNum;                 ///< リソースワーク数
  ResourceWork*                 m_pResourceWork;                  ///< リソースワーク(エリアに存在する配置モデルリソース分確保する)

};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // STATIC_MODEL_RESOURCE_NODE_MANAGER_H_INCLUDED
