//===================================================================
/**
 * @file    FieldEffectManager.h
 * @brief   フィールドエフェクトシステム管理
 * @author  saita_kazuki
 * @date    2015.03.12
 */
//===================================================================

#if !defined( FIELD_EFFECT_MANAGER_H_INCLUDED )
#define FIELD_EFFECT_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// effect
#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

// util
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_ListIterator.h>


namespace gfl2 {
  namespace gfx {
    class IGLAllocator;
  }
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 * @brief   フィールドエフェクト管理クラス
 */
class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager); //コピーコンストラクタ＋代入禁止

public:

  ///< グループID
  enum GroupID
  {
    // @todo 名前は適当。とりあえず１つ
    GROUP_ID_0,

    GROUP_ID_MAX,
  };

  ///< リソースID
  enum ResourceID
  {
    RESOURCE_ID_WEATHER,        // 天候
    RESOURCE_ID_FOOT_SPRAY,     // 足元しぶき

    RESOURCE_ID_MAX,
  };

  /**
   * @brief   コンストラクタ
   */
  Manager();

  /**
   * @brief   デストラクタ
   */
  virtual ~Manager();

  /**
   * @brief   初期化
   * @ param  pHeap エフェクトヒープに使用するヒープメモリ
   * @ param  pGLAllocator リソース確保に使用するアロケータ
   */
  b8 Initialize( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator);

  /**
   * @brief   破棄
   */
  b8 Terminate();

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   リソース作成
   * @param   resId リソースID。このクラスで定義
   * @retval  true 完了
   * @retval  false 実行中
   * @note    今は1フレームで完了する
   */
  b8 CreateResource( ResourceID resId);

  /**
   * @brief   リソース破棄
   * @param   resId リソースID。このクラスで定義
   * @retval  true 完了
   * @retval  false 実行中
   * @note    リソース破棄→バイナリ破棄を数フレームかけて行う
   */
  b8 DestroyResource( ResourceID resId);

  /**
   * @brief   モデルリソース作成
   * @param   resId リソースID。このクラスで定義
   * @retval  true 完了
   * @retval  false 実行中
   * @note    今は1フレームで完了する
   */
  b8 CreateModelResource( ResourceID resId);

  /**
   * @brief   モデルリソース破棄
   * @param   resId リソースID。このクラスで定義
   * @retval  true 完了
   * @retval  false 実行中
   * @note    リソース破棄→バイナリ破棄を数フレームかけて行う
   */
  b8 DestroyModelResource( ResourceID resId);

  /**
   * @brief   リソースが生成されているか
   * @param   resId リソースID。このクラスで定義
   * @retval  true 生成済み
   * @retval  false 生成されていない
   */
  b8 IsCreateResource( ResourceID resId);

  /**
   * @brief   リソースが生成されているか
   * @param   resId リソースID。このクラスで定義
   * @retval  true 生成済み
   * @retval  false 生成されていない
   */
  b8 IsCreateModelResource( ResourceID resId);

  /**
   * @brief   エフェクト描画パスのコンフィグ情報取得
   * @param   エフェクト描画コンフィグ情報のポインタ
   */
  void GetEffectRenderPathConfig( gfl2::Effect::EffectRenderPath::Config* pConfig);

  /**
   * @brief   エフェクトシステム取得
   */
  gfl2::Effect::System* GetSystem() const { return m_pSystem; }

  /**
   * @brief   フレームレート係数取得
   */
  f32 GetFrameRate() const { return m_frameRate; }

  /**
   * @brief   フレームレート係数変更
   */
  void SetFrameRate( f32 frameRate) { m_frameRate = frameRate; }

  /**
   * @brief   モデルリソースノード取得
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetModelResourceRootNode( ResourceID resId);

  /**
   * @brief   モーションリソースノード取得
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionResourceRootNode( ResourceID resId);

private:

  void SafeMemoryFree( void* pAddress);


  static const c16* PTCL_RESOURCE_PATH_TABLE[ RESOURCE_ID_MAX ];    ///< ptclリソースパステーブル
  static const c16* MODEL_RESOURCE_PATH_TABLE[ RESOURCE_ID_MAX ];   ///< modelリソースパステーブル
  static const c16* MOTION_RESOURCE_PATH_TABLE[ RESOURCE_ID_MAX ];  ///< motionリソースパステーブル


  gfl2::Effect::System*                   m_pSystem;          ///< システム
  gfl2::Effect::GFHeap*                   m_pHeap;            ///< エフェクト汎用ヒープ
  gfl2::util::List<u32>*                  m_pGroupIDList;     ///< グループIDリスト
  f32                                     m_frameRate;        ///< 1.0で等倍

  u8                        m_seq;

  gfl2::gfx::IGLAllocator*  m_pGLAllocator;                   ///< リソース用アロケータ
  void*                     m_pResourceBin[ RESOURCE_ID_MAX ];///< リソースバイナリ


  c8*                                                               m_pModelPackBin[ RESOURCE_ID_MAX ];
  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pModelResourceRootNode[ RESOURCE_ID_MAX ];
  c8*                                                               m_pMotionBin[ RESOURCE_ID_MAX ];
  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pMotionResourceRootNode[ RESOURCE_ID_MAX ];
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // FIELD_EFFECT_MANAGER_H_INCLUDED
