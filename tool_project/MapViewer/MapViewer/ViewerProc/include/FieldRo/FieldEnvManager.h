//===================================================================
/**
 * @file    FieldEnvManager.h
 * @brief   フィールドEnv管理クラス
 * @author  saita_kazuki
 * @date    2015.05.12
 */
//===================================================================

///#if !defined( FIELD_ENV_MANAGER_H_INCLUDED )
///#define FIELD_ENV_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include "System/include/EnvAccessor.h"

// 前方参照
namespace System {
  class nijiAllocator;
///  namespace Skybox {
///    class Skybox;
///  }
}
namespace Field {
class MyRenderingPipeLine;
}
namespace gfl2 {
  namespace fs {
    class BinLinkerAccessor;
  }
  namespace renderingengine { namespace scenegraph {
    namespace resource {
      class ResourceNode;
    }
    namespace instance {
      class DrawEnvNode;
    }
  }}
}


GFL_NAMESPACE_BEGIN( Field )

/**
 * @class フィールドEnv管理クラス
 */
class EnvManager
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( EnvManager );

public:

  /**
   * @brief コンストラクタ
   */
  EnvManager();

  /**
   * @brief デストラクタ
   */
  virtual ~EnvManager();

  /**
   * @brief 初期化
   * @param 天球インスタンスのポインタ
   * @todo 天候、スクリプトなど必要なものを渡す
   */
  void Initialize( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pGLAllocator, MyRenderingPipeLine* pRenderingPipeLine, void* pEnvPackData/*, System::Skybox::Skybox* pSkybox*/);//@todo_saito

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief 更新
   * @param マップ管理座標
   * @param hour 時間
   * @param minute 分
   * @param second 秒
   */
  void Update( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix,MyRenderingPipeLine* pRenderingPipeLine);

  /**
   * @brief DrawEnvNode取得
   */
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnvNode() const ;

  /**
   * @brief フォグデータの適応
   * @param pModelInstanceNode フォグを適応したいモデルインスタンス
   */
  void ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode);

private:

  /**
   * @brief リソースパックインデックス
   */
  enum PackIndex
  {
    PACK_INDEX_ENV,             ///< gfbenv
    PACK_INDEX_MOTION_LIGHT,    ///< ライトモーション
    PACK_INDEX_MOTION_FOG,      ///< フォグモーション
    PACK_INDEX_MOTION_BLOOM,    ///< ブルームモーション
    PACK_INDEX_TEXTURE_BLOOM,   ///< ブルーム用テクスチャ 
    PACK_INDEX_MAX
  };

  /**
   * @brief アニメーションインデックス
   */
  enum AnimationIndex
  {
    ANIMATION_INDEX_LIGHT,
    ANIMATION_INDEX_FOG,
    ANIMATION_INDEX_BLOOM,

    ANIMATION_INDEX_MAX,
  };

private:

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* CreateDrawEnvNode( void* pData);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* CreateDrawEnvResourceNode( void* pData);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* CreateDrawEnvMotionResourceNode( void* pData);

private:

  gfl2::heap::HeapBase*       m_pHeap;
  System::nijiAllocator*      m_pGLAllocator;

  System::EnvAccessor         m_EnvAccessor;    ///< DrawEnv共通処理アクセサー

  ///System::Skybox::Skybox*     m_pSkybox;        ///< 天球

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;                                         ///< Envノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceRootNode;                             ///< Envリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_MAX];  ///< Envモーションリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceAlphaMask;

};

GFL_NAMESPACE_END( Field )

///#endif // FIELD_ENV_MANAGER_H_INCLUDED

