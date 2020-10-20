//======================================================================
/**
 * @file FieldRenderingPipeLineManager.h
 * @date 2015/09/11 12:37:38
 * @author saita_kazuki
 * @brief 描画パイプライン管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_RENDERING_PIPE_LINE_MANAGER_H_INCLUDED__
#define __FIELD_RENDERING_PIPE_LINE_MANAGER_H_INCLUDED__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <math/include/gfl2_Math.h>

#include <Effect/include/gfl2_EffectRenderPath.h>

// 前方参照
namespace gfl2 {
  namespace renderingengine
  {
    namespace renderer {
      class RenderingPipeLine;
    }
    namespace scenegraph { namespace instance {
      class DrawEnvNode;
    }}
  }
  namespace gfx {
    class Texture;
    class IGLAllocator;
  }
  namespace fs {
    class AsyncFileManager;
  }
}
namespace Field {
  class MyRenderingPipeLine;
  class CaptureRenderingPipeLine;
  namespace Camera {
    class CameraUnit;
  }
}
namespace System {
  class nijiAllocator;
}
namespace DemoLib { namespace Sequence {
  class SequenceViewSystem;
}}


GFL_NAMESPACE_BEGIN( Field )

/**
 * @class FieldRenderingPipeLineManager
 * @brief 描画パイプライン管理
 */
class FieldRenderingPipeLineManager
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldRenderingPipeLineManager );

public:

  /**
   * @brief 実行タイプ
   */
  enum ExecuteType
  {
    EXECUTE_TYPE_NORMAL,      ///< 通常
    EXECUTE_TYPE_CAPTURE,     ///< キャプチャー

    EXECUTE_TYPE_MAX
  };

  /**
   * @brief カラーバッファタイプ
   */
  enum CollorBufferType
  {
    COLLOR_BUFFER_TYPE_UPPER,   ///< 上画面
    COLLOR_BUFFER_TYPE_LOWER,   ///< 下画面

    COLLOR_BUFFER_TYPE_MAX,
  };

  /**
   * @brief 初期化情報
   */
  struct InitializeDescription
  {
    gfl2::heap::HeapBase*       pHeap;
    gfl2::heap::HeapBase*       pTemporaryHeapForFileRead;
    System::nijiAllocator*      pAllocator;
    gfl2::fs::AsyncFileManager* pAsyncFileManager;

    InitializeDescription()
      : pHeap( NULL )
      , pTemporaryHeapForFileRead( NULL )
      , pAllocator( NULL )
      , pAsyncFileManager( NULL )
    {}
  };

  /**
   * @brief キャプチャーパイプライン生成情報
   */
  struct CapturePipeLineCreateDescription
  {
    gfl2::heap::HeapBase*                                     pHeap;
    System::nijiAllocator*                                    pAllocator;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode;
  };

public:

  /**
   * @brief コンストラクタ
   */
  FieldRenderingPipeLineManager();

  /**
   * @brief デストラクタ
   */
  virtual ~FieldRenderingPipeLineManager();

  /**
   * @brief 初期化
   */
  void Initialize( const InitializeDescription& desc);
  bool InitializeWait( void );

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief 更新
   */
  void Update();

  /**
   * @brief 前描画
   */
  void PreDraw();

  /**
   * @brief 描画
   * @param displayNo  描画対象画面
   * @param pViewCamera 描画に使用するカメラ
   */
  void Draw( gfl2::gfx::CtrDisplayNo displayNo, const Camera::CameraUnit* pViewCamera);

  /**
   * @brief 実行するパイプラインの変更
   */
  void ChangeExecuteType( ExecuteType type);

  /**
   * @brief 通常パイプライン取得
   */
  MyRenderingPipeLine* GetMyRenderingPipeLine() const ;

  /**
   * @brief キャプチャー用パイプライン取得
   */
  CaptureRenderingPipeLine* GetCaptureRenderingPipeLine() const ;

  /**
   * @brief 現在の実行タイプ取得
   */
  ExecuteType GetNowExecuteType() const ;

  /**
   *  @brief カラーバッファ取得 リクエスト
   *  @param pOutBuffer 格納先バッファ
   *  @param target 取得対象
   */
  void RequestTransferColorBuffer( gfl2::gfx::Texture* pOutBuffer );
  void RequestTransferColorBuffer( void *pOutBuffer );

  /**
   *  @brief キャプチャー用パイプラインの生成
   *  @param desc キャプチャーパイプライン生成用の初期化情報
   */
  void CreateCaptureRenderingPipeLine( const CapturePipeLineCreateDescription& desc);

  /**
   *  @brief キャプチャー用パイプラインの破棄
   */
  void DeleteCaptureRenderingPipeLine();

  /**
   *  @brief エフェクトレンダーパスの設定ファイルをこのクラスに渡す
   */
  void SetEffectRenderPathConfig( const gfl2::Effect::EffectRenderPath::Config& config);

  /**
   *  @brief エフェクトレンダーパスに設定ファイルをセット
   */
  void SetupEffectRenderPathConfig( ExecuteType type);

  /**
   *  @brief EnvNodeをパイプラインに追加
   */
  void AddDrawEnv( ExecuteType type, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode);

  /**
   *  @brief シーケンスデモシステムセット
   */
  void SetSequenceViewSystem( DemoLib::Sequence::SequenceViewSystem* pSequenceViewSystem);

private:

  void asyncLoadResource( const InitializeDescription& desc);
  bool waitLoadResource( const InitializeDescription& desc);
  void createStencilTexture( const InitializeDescription& desc);
  void createNormalPipeLine( const InitializeDescription& desc);
  void createCapturePipeLine( const CapturePipeLineCreateDescription& desc);
  void initializeBloomRenderPath( const InitializeDescription& desc);
  bool waitInitializeBloomRenderPath( const InitializeDescription& desc);

  void drawNormal( gfl2::gfx::CtrDisplayNo displayNo, const Camera::CameraUnit* pViewCamera);
  void drawCapture( gfl2::gfx::CtrDisplayNo displayNo, const Camera::CameraUnit* pViewCamera);

  void transferColorBuffer( gfl2::gfx::CtrDisplayNo no );

  gfl2::math::Matrix34 getWeatherBoardViewMatrix();
  gfl2::math::Matrix44 getWeatherBoardProjectionMatrix();

  gfl2::math::Matrix34 getOrthoViewMatrix();
  gfl2::math::Matrix44 getOrthoProjectionMatrix();

private:

  gfl2::renderingengine::renderer::RenderingPipeLine*     m_pExecutePipeLineList[ EXECUTE_TYPE_MAX ];
  ExecuteType                                             m_nowExecuteType;

  gfl2::gfx::Texture*                                     m_pDepthStencilTexture;
  void*                                                   m_pOutLineResBuf;
  void*                                                   m_pEdgeMapResBuf;

  gfl2::gfx::Texture*                                     m_pTextureBuffer;
  void*                                                   m_pColorBuffer;

  gfl2::Effect::EffectRenderPath::Config                  m_effectRenderPathConfig;

  DemoLib::Sequence::SequenceViewSystem*                  m_pSequenceViewSystem;

  InitializeDescription                                   m_Desc;
  u32                                                     m_InitializeSeq;
};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_RENDERING_PIPE_LINE_MANAGER_H_INCLUDED__

