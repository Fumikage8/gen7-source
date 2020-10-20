//===================================================================
/**
 * @file    FieldEnvManager.h
 * @brief   フィールドEnv管理クラス
 * @author  saita_kazuki
 * @date    2015.05.12
 */
//===================================================================

#if !defined( FIELD_ENV_MANAGER_H_INCLUDED )
#define FIELD_ENV_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include "System/include/EnvAccessor.h"

// 前方参照
namespace System {
  class nijiAllocator;
  namespace Skybox {
    class Skybox;
  }
}
namespace Field {
class MyRenderingPipeLine;

  namespace Skybox {
    class DataManager;
  }
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
   */
  void Initialize( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pGLAllocator, MyRenderingPipeLine* pRenderingPipeLine, void* pEnvPackData, System::Skybox::Skybox* pSkybox, Skybox::DataManager* pSkyboxDataManager);

  /**
   * @brief 画面表示前に行う処理
   * @param マップ管理座標
   * @param hour 時間
   * @param minute 分
   * @param second 秒
   * @param projMatrix プロジェクションマトリクス
   * @param pRenderingPipeLine 描画パイプライン
   */
  void Ready( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix, MyRenderingPipeLine* pRenderingPipeLine);

  /**
   * @brief アニメーションのセット
   * @param　アニメーションリソースノードポインタ
   */
  void SetAnime(gfl2::renderingengine::scenegraph::resource::ResourceNode* anime_node);

  /**
   * @brief アニメーションのセット
   * @param　アニメーションリソースノードポインタ
   */
  void SetAnimeSmooth(gfl2::renderingengine::scenegraph::resource::ResourceNode* anime_node);

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
   * @brief ブルーム(グレア)の輝度を指定の割合に変化させる
   * @param rate 輝度の割合
   * @param frame 変化させるフレーム
   */
  void ChangeBloom( f32 rate, s32 frame);

  /**
   * @brief ライトセットのアンビエントカラーから輝度を受け取る
   */
  b32 GetLuminanceFromLightSetAmbient( u32 lightSetIndex, f32* pOutLuminance ) const;

  /**
   * @brief アニメーショの変更が終了したか
   * @return true で終了
   */
  bool IsAnimeChangeFinished(void);

private:

  /**
   * @brief リソースパックインデックス
   */
  enum PackIndex
  {
    PACK_INDEX_ENV,             ///< gfbenv
//    PACK_INDEX_MOTION_LIGHT,    ///< ライトモーション
//    PACK_INDEX_MOTION_FOG,      ///< フォグモーション
//    PACK_INDEX_MOTION_BLOOM,    ///< ブルームモーション
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

  System::Skybox::Skybox*     m_pSkybox;        ///< 天球
  Skybox::DataManager*        m_pSkyboxDataManager;   ///< 天球データ管理

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;                                         ///< Envノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceRootNode;                             ///< Envリソースノード
  ///gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_MAX];  ///< Envモーションリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceAlphaMask;

  /**
   * @brief リクエストを受けて行う更新の状態管理
   */
  class RequestUpdateState
  {
  public:

    /**
     * @enum 状態
     */
    enum State
    {
      STATE_BLOOM_STRENGTH          = 1 << 0,    ///< ブルーム輝度
      STATE_BLOOM_STRENGTH_RETURN   = 1 << 1,    ///< ブルーム輝度元に戻す
    };

    /**
     * @brief コンストラクタ
     */
    RequestUpdateState()
      : m_state( 0)
    {}

    /**
     * @brief デストラクタ
     */
    virtual ~RequestUpdateState(){}

    /**
     * @brief 状態追加
     * @param state 追加する状態
     */
    void AddState( u32 state) { m_state |= state; }

    /**
     * @brief 状態削除
     * @param state 削除する状態
     */
    void DeleteState( u32 state) { m_state &= (~state); }

    /**
     * @brief 指定した状態か？
     * @param state チェックする状態
     * @retval true 指定した状態である
     * @retval false 指定した状態でない
     */
    bool IsState( u32 state) { return (m_state & state) ? true : false ; }

    /**
     * @brief 状態リセット
     */
    void ResetState() { m_state = 0; }

  private:

    u32 m_state;    ///< 状態ビット
  };

  /**
   * @brief パラメータワーク
   */
  struct ParameterWork
  {
    ParameterWork()
      : now( 0.0f)
      , goal( 0.0f)
      , add( 0.0f)
      , save( 0.0f)
    {}

    f32   now;    ///< 現在
    f32   goal;   ///< 目標
    f32   add;    ///< 加算値
    f32   save;   ///< 保存用
  };

  RequestUpdateState          m_reqUpdateState;
  ParameterWork               m_bloomStrength;
  ParameterWork               m_bloomIntensity;
  s32                         m_changeFrameCnt;
  s32                         m_changeFrame;


  void UpdateRequest();

  void UpdateBloom();
};

GFL_NAMESPACE_END( Field )

#endif // FIELD_ENV_MANAGER_H_INCLUDED

