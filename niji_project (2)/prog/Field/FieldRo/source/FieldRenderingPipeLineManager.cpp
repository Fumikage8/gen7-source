//======================================================================
/**
 * @file FieldRenderingPipeLineManager.cpp
 * @date 2015/09/11 12:37:38
 * @author saita_kazuki
 * @brief 描画パイプライン管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

// gflib
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <System/include/Timer/gfl2_PerformanceCounter.h>

#if defined(GF_PLATFORM_CTR)
#include <RenderingEngine/include/Renderer/gfl2_CombinerCtrShaderDriver.h>
#endif

// field
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Fieldmap.h"

// system
#include "System/include/GflUse.h"
#include "System/include/nijiAllocator.h"

// gamesys
#include "GameSys/include/GameManager.h"

// arc
#include <arc_def_index/arc_def.h>
#include "arc_index/NijiRenderPath.gaix"

// debug
#include "../include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// DemoLib
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief コンストラクタ
 */
FieldRenderingPipeLineManager::FieldRenderingPipeLineManager()
  : m_nowExecuteType( EXECUTE_TYPE_NORMAL)
  , m_pDepthStencilTexture( NULL)
  , m_pOutLineResBuf( NULL)
  , m_pEdgeMapResBuf( NULL)
  , m_pTextureBuffer( NULL)
  , m_pColorBuffer( NULL)
  , m_Desc()
  , m_pSequenceViewSystem( NULL)
  , m_InitializeSeq(0)
{
  gfl2::std::MemClear( m_pExecutePipeLineList, sizeof( m_pExecutePipeLineList));
  gfl2::std::MemClear( &m_effectRenderPathConfig, sizeof( gfl2::Effect::EffectRenderPath::Config));
}

/**
 * @brief デストラクタ
 */
FieldRenderingPipeLineManager::~FieldRenderingPipeLineManager()
{
}

/**
 * @brief 初期化
 */
void FieldRenderingPipeLineManager::Initialize( const InitializeDescription& desc)
{
  m_Desc = desc;
  this->asyncLoadResource( m_Desc);
}

/**
 * @brief 初期化完了待ち
 */
bool FieldRenderingPipeLineManager::InitializeWait( void )
{
  enum
  {
    SEQ_WAIT_LOAD,
    SEQ_CREATE_NORMAL_PIPELINE,
    SEQ_BLOOM_INIT,
    SEQ_BLOOM_INIT_WAIT,
    SEQ_END
  };

  switch( m_InitializeSeq )
  {
  case SEQ_WAIT_LOAD:
    {
      if( !waitLoadResource(m_Desc) )
      {
        break;
      }
    }
    m_InitializeSeq++;
    //break; through

  case SEQ_CREATE_NORMAL_PIPELINE:
    this->createStencilTexture(m_Desc);
    this->createNormalPipeLine(m_Desc);
    m_InitializeSeq++;
    //break; through

  case SEQ_BLOOM_INIT:
    this->initializeBloomRenderPath(m_Desc);
    m_InitializeSeq++;
    //break; through

  case SEQ_BLOOM_INIT_WAIT:
    if( !this->waitInitializeBloomRenderPath(m_Desc) )
    {
      break;
    }
    m_InitializeSeq++;
    //break; through

  case SEQ_END:
    return true;
  }

  return false;
}

/**
 * @brief 終了
 */
void FieldRenderingPipeLineManager::Terminate()
{
  for( u32 i = 0; i < EXECUTE_TYPE_MAX; ++i)
  {
    GFL_SAFE_DELETE(m_pExecutePipeLineList[i]);
  }

  GflHeapSafeFreeMemory( m_pOutLineResBuf );
  GflHeapSafeFreeMemory( m_pEdgeMapResBuf );

  GFL_SAFE_DELETE(m_pDepthStencilTexture);

  m_InitializeSeq = 0; 
}

/**
 * @brief 更新
 */
void FieldRenderingPipeLineManager::Update()
{
  if( m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->Update();
  }
}

/**
 * @brief 前描画
 */
void FieldRenderingPipeLineManager::PreDraw()
{
  if( m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->PreDraw();
  }
}

/**
 * @brief 描画
 * @param displayNo  描画対象画面
 */
void FieldRenderingPipeLineManager::Draw( gfl2::gfx::CtrDisplayNo displayNo, const Camera::CameraUnit* pViewCamera)
{
  switch( m_nowExecuteType)
  {
  case EXECUTE_TYPE_NORMAL:
    this->drawNormal( displayNo, pViewCamera);
    break;
  case EXECUTE_TYPE_CAPTURE:
    this->drawCapture( displayNo, pViewCamera);
    break;
  default:
    GFL_ASSERT( 0);
    break;
  };

  if( m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->Draw( displayNo);
  }

  this->transferColorBuffer( displayNo);
}

/**
 * @brief 実行するパイプラインの変更
 */
void FieldRenderingPipeLineManager::ChangeExecuteType( ExecuteType type)
{
  if( m_pExecutePipeLineList[ type ] == NULL)
  {
    GFL_ASSERT_MSG( 0, "FieldRenderingPipeLineManager::ChangeExecuteType : 指定パイプラインがNULLです\n");
    return; // 変更しない
  }
  m_nowExecuteType = type;
}

/**
 * @brief 通常パイプライン取得
 */
MyRenderingPipeLine* FieldRenderingPipeLineManager::GetMyRenderingPipeLine() const
{
  GFL_ASSERT( m_pExecutePipeLineList[ EXECUTE_TYPE_NORMAL ]);
  return reinterpret_cast<MyRenderingPipeLine*>( m_pExecutePipeLineList[ EXECUTE_TYPE_NORMAL ]);
}

/**
 * @brief キャプチャー用パイプライン取得
 */
CaptureRenderingPipeLine* FieldRenderingPipeLineManager::GetCaptureRenderingPipeLine() const
{
  GFL_ASSERT( m_pExecutePipeLineList[ EXECUTE_TYPE_CAPTURE ]);
  return reinterpret_cast<CaptureRenderingPipeLine*>( m_pExecutePipeLineList[ EXECUTE_TYPE_CAPTURE ]);
}

/**
* @brief 現在の実行タイプ取得
*/
FieldRenderingPipeLineManager::ExecuteType FieldRenderingPipeLineManager::GetNowExecuteType() const
{
  return m_nowExecuteType;
}

/**
 *  @brief  カラーバッファ取得 リクエスト
 */
void FieldRenderingPipeLineManager::RequestTransferColorBuffer(  gfl2::gfx::Texture* pOutBuffer)
{
  m_pTextureBuffer = pOutBuffer;
}

/**
 *  @brief  カラーバッファ取得 リクエスト
 */
void FieldRenderingPipeLineManager::RequestTransferColorBuffer( void* pOutBuffer)
{
  m_pColorBuffer = pOutBuffer;
}

/**
 *  @brief キャプチャー用パイプラインの生成
 *  @param desc キャプチャーパイプライン生成用の初期化情報
 */
void FieldRenderingPipeLineManager::CreateCaptureRenderingPipeLine( const CapturePipeLineCreateDescription& desc)
{
  this->createCapturePipeLine( desc);
  this->SetupEffectRenderPathConfig( EXECUTE_TYPE_CAPTURE);
  this->AddDrawEnv( EXECUTE_TYPE_CAPTURE, desc.pDrawEnvNode);
}

/**
 *  @brief キャプチャー用パイプラインの破棄
 */
void FieldRenderingPipeLineManager::DeleteCaptureRenderingPipeLine()
{
  GFL_SAFE_DELETE( m_pExecutePipeLineList[ EXECUTE_TYPE_CAPTURE ]);
}

/**
 *  @brief エフェクトレンダーパスの設定ファイルをこのクラスに渡す
 */
void FieldRenderingPipeLineManager::SetEffectRenderPathConfig( const gfl2::Effect::EffectRenderPath::Config& config)
{
  m_effectRenderPathConfig = config;
}

/**
 *  @brief エフェクトレンダーパスに設定ファイルをセット
 */
void FieldRenderingPipeLineManager::SetupEffectRenderPathConfig( ExecuteType type)
{
  gfl2::Effect::EffectRenderPath *pEffRenderPath = NULL;
  switch( type)
  {
  case EXECUTE_TYPE_NORMAL:
    {
      MyRenderingPipeLine* pPipeLine = this->GetMyRenderingPipeLine();
      pEffRenderPath = pPipeLine->GetEffectRenderPath();
    }
    break;

  case EXECUTE_TYPE_CAPTURE:
    {
      CaptureRenderingPipeLine* pPipeLine = this->GetCaptureRenderingPipeLine();
      pEffRenderPath = pPipeLine->GetEffectRenderPath();
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
  pEffRenderPath->SetConfig( m_effectRenderPathConfig);
}

/**
 *  @brief EnvNodeをパイプラインに追加
 */
void FieldRenderingPipeLineManager::AddDrawEnv( ExecuteType type, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
{
  switch( type)
  {
  case EXECUTE_TYPE_NORMAL:
    {
      MyRenderingPipeLine* pPipeLine = this->GetMyRenderingPipeLine();
      pPipeLine->AddDrawEnv( pNode);
    }
    break;

  case EXECUTE_TYPE_CAPTURE:
    {
      CaptureRenderingPipeLine* pPipeLine = this->GetCaptureRenderingPipeLine();
      pPipeLine->AddDrawEnv( pNode);
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
}

/**
 *  @brief シーケンスデモシステムセット
 */
void FieldRenderingPipeLineManager::SetSequenceViewSystem( DemoLib::Sequence::SequenceViewSystem* pSequenceViewSystem)
{
  m_pSequenceViewSystem = pSequenceViewSystem;
}

/**
 * @brief 描画パスリソースの読み込み
 */
void FieldRenderingPipeLineManager::asyncLoadResource( const InitializeDescription& desc)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = ARCID_RENDERPATH;
  openReq.heapForFile = desc.pTemporaryHeapForFileRead;
  openReq.heapForReq = desc.pTemporaryHeapForFileRead;
  desc.pAsyncFileManager->AddArcFileOpenReq( openReq );

  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_RENDERPATH;
    loadReq.align = 128;
    loadReq.heapForBuf = desc.pHeap;
    loadReq.heapForReq = desc.pTemporaryHeapForFileRead;
   
    loadReq.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
    loadReq.ppBuf = &( m_pEdgeMapResBuf );
    desc.pAsyncFileManager->AddArcFileLoadDataReq(loadReq);
   
    loadReq.datId = GARC_NijiRenderPath_OutLineResource_BIN;
    loadReq.ppBuf = &( m_pOutLineResBuf );
    desc.pAsyncFileManager->AddArcFileLoadDataReq(loadReq);
  }
}

/**
 * @brief 描画パスリソースの読み込み
 */
bool FieldRenderingPipeLineManager::waitLoadResource( const InitializeDescription& desc)
{
  if( !desc.pAsyncFileManager->IsArcFileOpenFinished(ARCID_RENDERPATH) )
  {
    return false;
  }

  if( !desc.pAsyncFileManager->IsArcFileLoadDataFinished( &m_pEdgeMapResBuf ) )
  {
    return false;
  }

  if( !desc.pAsyncFileManager->IsArcFileLoadDataFinished( &m_pOutLineResBuf ) )
  {
    return false;
  }

  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = ARCID_RENDERPATH;
  closeReq.heapForReq = desc.pTemporaryHeapForFileRead;
  desc.pAsyncFileManager->SyncArcFileClose(closeReq);
  return true;
}

/**
 * @brief ステンシルテクスチャーの生成
 */
void FieldRenderingPipeLineManager::createStencilTexture(const InitializeDescription& desc)
{
  m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(desc.pAllocator, gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT ] ));
}

/**
 * @brief 通常描画パイプラインの生成
 */
void FieldRenderingPipeLineManager::createNormalPipeLine( const InitializeDescription& desc)
{
  MyRenderingPipeLine* pPipeLine;
  pPipeLine = GFL_NEW( desc.pHeap ) MyRenderingPipeLine( 
    desc.pHeap,
    desc.pAllocator,
    m_pEdgeMapResBuf,
    m_pOutLineResBuf,
    512, 
    256
    );

  pPipeLine->SetDepthTexture( m_pDepthStencilTexture );

  m_pExecutePipeLineList[ EXECUTE_TYPE_NORMAL ] = pPipeLine;

  // GPU演算精度を上げるためViewSpaceで演算
  pPipeLine->SetViewSpaceRenderEnable(true);
}

/**
 * @brief キャプチャー用描画パイプラインの生成
 */
void FieldRenderingPipeLineManager::createCapturePipeLine( const CapturePipeLineCreateDescription& desc)
{
  // キャプチャー用描画パイプラインは必要な箇所で生成する
  // 以下のリソースはFieldRenderingPipeLineManager初期化時に作成されている
  GFL_ASSERT( m_pEdgeMapResBuf);
  GFL_ASSERT( m_pOutLineResBuf);
#if defined(GF_PLATFORM_CTR)
  GFL_ASSERT( m_pDepthStencilTexture);
#endif // GF_PLATFORM_CTR

  CaptureRenderingPipeLine* pPipeLine;
  pPipeLine = GFL_NEW( desc.pHeap ) CaptureRenderingPipeLine( 
    desc.pHeap,
    desc.pAllocator,
    m_pEdgeMapResBuf,
    m_pOutLineResBuf,
    512, 
    256
    );

  pPipeLine->SetDepthTexture( m_pDepthStencilTexture );

  m_pExecutePipeLineList[ EXECUTE_TYPE_CAPTURE ] = pPipeLine;

}

/**
 * @brief ブルーム描画パスの初期化
 */
void FieldRenderingPipeLineManager::initializeBloomRenderPath( const InitializeDescription& desc)
{
  poke_3d::renderer::BloomRenderPath::Description bloomDesc;
  {
    bloomDesc.sourceType                               = poke_3d::renderer::BloomRenderPath::SOURCE_TYPE_FRAME_BUFFER;
    bloomDesc.bloomType                                = poke_3d::renderer::BloomRenderPath::BLOOM_TYPE_GAUSSIAN_FILTER_3X3;
    bloomDesc.highLuminanceRenderTargetReciprocalScale = 2;
    bloomDesc.highLuminanceCopyNumMax                  = 1;
    bloomDesc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::LEFT ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT ] );
    bloomDesc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::RIGHT] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::RIGHT] );
    bloomDesc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::DOWN ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::DOWN ] );
  }

  {
    MyRenderingPipeLine* pPipeLine = this->GetMyRenderingPipeLine();
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pPipeLine->GetBloomRenderPath();
    bloomRenderPath->StartAsyncCreate(desc.pHeap,
                                desc.pHeap->GetLowerHandle(),
                                desc.pAllocator,
                                desc.pAllocator,
                                desc.pAsyncFileManager,
                                bloomDesc);
  }
}

bool FieldRenderingPipeLineManager::waitInitializeBloomRenderPath( const InitializeDescription& desc)
{
  {
    MyRenderingPipeLine* pPipeLine = this->GetMyRenderingPipeLine();
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pPipeLine->GetBloomRenderPath();

    bloomRenderPath->Update();
    if( bloomRenderPath->IsCreated() )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief 通常描画
 * @param pViewCamera 描画に使用するカメラ
 */
void FieldRenderingPipeLineManager::drawNormal( gfl2::gfx::CtrDisplayNo displayNo, const Camera::CameraUnit* pViewCamera)
{
  // デフォルトのビュー・プロジェクション行列
  gfl2::math::Matrix44 projMat[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMat[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 view; 
  gfl2::math::Matrix44 projection;
#if PM_DEBUG
  b32 isOrtho = pViewCamera->IsOrtho();
#endif
  pViewCamera->GetViewMatrix( &view );
  pViewCamera->GetProjectionMatrix( &projection );

  // 天球のビュー・プロジェクション行列
  gfl2::math::Matrix44 projMatForSkyBox[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMatForSkyBox[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewForSkyBox( view );
  pViewCamera->GetViewMatrix( &viewForSkyBox );
  gfl2::math::Matrix44 projectionForSkyBox( gfl2::math::Matrix44::GetPerspective( pViewCamera->GetBaseCamera()->GetFovy(),
                                                                                  pViewCamera->GetBaseCamera()->GetAspectRatio(),
                                                                                  pViewCamera->GetBaseCamera()->GetNear(),
                                                                                  100000.0f ) );

  // 天候ボードビュー・プロジェクション行列
  gfl2::math::Matrix44 projMatForWeatherBoard[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMatForWeatherBoard[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewForWeatherBoard = this->getWeatherBoardViewMatrix();
  gfl2::math::Matrix44 projectionForWeatherBoard = this->getWeatherBoardProjectionMatrix();

  // 正射影のビュー・プロジェクション行列
  gfl2::math::Matrix44 projMatForOrtho[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMatForOrtho[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewForOrtho = this->getOrthoViewMatrix();
  gfl2::math::Matrix44 projectionForOrtho = this->getOrthoProjectionMatrix();

  // 被写界深度用のビュー・プロジェクション行列
  gfl2::math::Matrix44 projMatForDOF[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMatForDOF[ gfl2::gfx::CtrDisplayNo::NumberOf ];
#if defined(GF_PLATFORM_CTR)

  static f32 depthLevel = 30.0f;
  static f32 factor     = 0.15f;

  if( nn::gx::CTR::IsStereoVisionAllowed() == false || gfl2::gfx::ctr::CTRGL::IsStereoModeEnable() == false )
  {
    factor = 0.0f;
  }
  else
  {
    // @fix GFNMCat[4537]Finalビルド対応
    factor = 0.15f;
  }

#if PM_DEBUG
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu )
  {
    depthLevel = pFieldDebugMenu->GetDepthLevel();
    factor = pFieldDebugMenu->GetFactor();
  }
#endif // PM_DEBUG

  gfl2::gfx::GFGL::GetStereoMatrix( projection,
                                    view,
                                    depthLevel,
                                    factor, 
                                    &projMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &viewMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &projMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT],
                                    &viewMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);

  // ビュースペースのベース用行列
  gfl2::gfx::GFGL::GetStereoMatrix( projection,
                                    // @fix GFNMcat[1919]他、フィールドカメラが小刻みに震える、対処。ViewSpace描画ではGPUに単位行列×立体視差分が含まれた行列が渡されるが、その行列の生成計算式に問題があり、カメラ座標が大きい際に計算誤差が発生していた。
                                    // 立体視視差をとるだけなので、単位行列から単位行列×立体視差分行列を作成するように修正
                                    gfl2::math::Matrix34::GetIdentity(),
                                    depthLevel,
                                    factor, 
                                    &projMat[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &viewMat[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &projMat[gfl2::gfx::CtrDisplayNo::RIGHT],
                                    &viewMat[gfl2::gfx::CtrDisplayNo::RIGHT]);

  // @fix GFMMCat[13]:位置座標が大きい場合に天球の表示がカクつく、対処。ViewSpace描画を天球に適用。
  gfl2::gfx::GFGL::GetStereoMatrix( projectionForSkyBox,
                                    gfl2::math::Matrix34::GetIdentity(),
                                    depthLevel,
                                    factor, 
                                    &projMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &projMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT],
                                    &viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT]);
  
#elif defined(GF_PLATFORM_WIN32)

  projMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT] = projection;
  viewMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT] = view;
  projMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]  = projection;
  viewMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]  = view;

  projMat[gfl2::gfx::CtrDisplayNo::RIGHT] = projection;
  viewMat[gfl2::gfx::CtrDisplayNo::RIGHT] = view;
  projMat[gfl2::gfx::CtrDisplayNo::LEFT]  = projection;
  viewMat[gfl2::gfx::CtrDisplayNo::LEFT]  = view;

  projMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT] = projectionForSkyBox;
  viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT] = viewForSkyBox;
  projMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT]  = projectionForSkyBox;
  viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT]  = viewForSkyBox;

#endif // GF_PLATFORM_WIN32

#if PM_DEBUG
  // @fix CONNMCat[582]:マップ撮影機能実装、フィールドカメラが返す行列が正射影行列だった場合、前述の加工でGPUハングが発生するため、行列を強制上書きする
  if( isOrtho )
  {
    projMat[gfl2::gfx::CtrDisplayNo::RIGHT] = projection;
    viewMat[gfl2::gfx::CtrDisplayNo::RIGHT] = gfl2::math::Matrix34::GetIdentity();
    projMat[gfl2::gfx::CtrDisplayNo::LEFT]  = projection;
    viewMat[gfl2::gfx::CtrDisplayNo::LEFT]  = gfl2::math::Matrix34::GetIdentity();
  }
#endif

  projMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::RIGHT] = projectionForWeatherBoard;
  viewMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::RIGHT] = viewForWeatherBoard;
  projMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::LEFT]  = projectionForWeatherBoard;
  viewMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::LEFT]  = viewForWeatherBoard;
  
  projMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT] = projectionForOrtho;
  viewMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT] = viewForOrtho;
  projMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT]  = projectionForOrtho;
  viewMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT]  = viewForOrtho;

  MyRenderingPipeLine* pPipeLine = this->GetMyRenderingPipeLine();

  switch( displayNo )
  {
    // 左画面描画
    case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::GFGL::SetActiveDisplay( System::GflUse::m_HandleList[ displayNo ] );

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer        ( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth      ( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight     ( System::GflUse::m_HandleList[ displayNo ] );

      // 天球パスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetSkyBoxRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT];
#if defined(GF_PLATFORM_WIN32)
        pCameraParam->m_View = viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT];
#endif
#if defined(GF_PLATFORM_CTR)
        pCameraParam->m_View       = viewForSkyBox;
#endif
      }

      // 天候ボードパスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetWeatherBoardRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::LEFT];
        pCameraParam->m_View       = viewMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::LEFT];
      }

      // 正射影パスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetOrthoRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT];
        pCameraParam->m_View       = viewMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT];
      }

      // 被写界深度パスのビュー・プロジェクション行列をカスタム
      if (pPipeLine->GetMakeBlurImagePath_Front() != NULL)
      {
        pPipeLine->GetMakeBlurImagePath_Front()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
        pPipeLine->GetMakeBlurImagePath_Back()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
        pPipeLine->GetDefRenderPath_Front()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
        pPipeLine->GetDefRenderPath_Back()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);

        pPipeLine->GetMakeBlurImagePath_Front()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
        pPipeLine->GetMakeBlurImagePath_Back()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
        pPipeLine->GetDefRenderPath_Front()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
        pPipeLine->GetDefRenderPath_Back()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::LEFT]);
      }

      // デフォルトのビュー・プロジェクション行列
#if defined(GF_PLATFORM_CTR)
      // 左目の差分行列設定
      gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(viewMat[gfl2::gfx::CtrDisplayNo::LEFT]);
      pPipeLine->GetSkyBoxRenderPath()->SetViewSpaceDiffMatrix( viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::LEFT] ) ;
#endif
      pPipeLine->SetViewMatrix      ( view );
      pPipeLine->SetProjectionMatrix( projMat[gfl2::gfx::CtrDisplayNo::LEFT] );
      pPipeLine->SetRenderTarget    ( pBackBuffer  );
      pPipeLine->SetDepthStencil    ( pDepthBuffer );
      pPipeLine->SetDisplaySize     ( displayWidth, displayHeight );

      pPipeLine->Update();
      pPipeLine->Execute();
      break;
    }

    // 右画面描画(立体視時のみ)
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::GFGL::SetActiveDisplay( System::GflUse::m_HandleList[ displayNo ] );

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer        ( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth      ( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight     ( System::GflUse::m_HandleList[ displayNo ] );

      // 天球パスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetSkyBoxRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT];
#if defined(GF_PLATFORM_WIN32)
        pCameraParam->m_View = viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT];
#endif
#if defined(GF_PLATFORM_CTR)
        pCameraParam->m_View = viewForSkyBox;
#endif
      }

      // 天候ボードパスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetWeatherBoardRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::RIGHT];
        pCameraParam->m_View       = viewMatForWeatherBoard[gfl2::gfx::CtrDisplayNo::RIGHT];
      }

      // 正射影パスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetOrthoRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT];
        pCameraParam->m_View       = viewMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT];
      }

      // 被写界深度パスのビュー・プロジェクション行列をカスタム
      if (pPipeLine->GetMakeBlurImagePath_Front() != NULL)
      {
        pPipeLine->GetMakeBlurImagePath_Front()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
        pPipeLine->GetMakeBlurImagePath_Back()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
        pPipeLine->GetDefRenderPath_Front()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
        pPipeLine->GetDefRenderPath_Back()->SetViewMatrix(viewMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);

        pPipeLine->GetMakeBlurImagePath_Front()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
        pPipeLine->GetMakeBlurImagePath_Back()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
        pPipeLine->GetDefRenderPath_Front()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
        pPipeLine->GetDefRenderPath_Back()->SetProjMatrix(projMatForDOF[gfl2::gfx::CtrDisplayNo::RIGHT]);
      }

      // デフォルトのビュー・プロジェクション行列
#if defined(GF_PLATFORM_CTR)
      // 右目の差分行列設定
      gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(viewMat[gfl2::gfx::CtrDisplayNo::RIGHT]);
      pPipeLine->GetSkyBoxRenderPath()->SetViewSpaceDiffMatrix( viewMatForSkyBox[gfl2::gfx::CtrDisplayNo::RIGHT] ) ;
#endif
      pPipeLine->SetViewMatrix      ( view );
      pPipeLine->SetProjectionMatrix( projMat[gfl2::gfx::CtrDisplayNo::RIGHT] );
      pPipeLine->SetRenderTarget    ( pBackBuffer  );
      pPipeLine->SetDepthStencil    ( pDepthBuffer );
      pPipeLine->SetDisplaySize     ( displayWidth, displayHeight );

      pPipeLine->CallDisplayList();
      break;
    }
  }
}

/**
 * @brief キャプチャー描画
 */
void FieldRenderingPipeLineManager::drawCapture( gfl2::gfx::CtrDisplayNo displayNo, const Camera::CameraUnit* pViewCamera)
{
  // デフォルトのビュー・プロジェクション行列
  gfl2::math::Matrix44 projMat[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMat[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 view; 
  gfl2::math::Matrix44 projection;
  pViewCamera->GetViewMatrix( &view );
  pViewCamera->GetProjectionMatrix( &projection );

  // 正射影のビュー・プロジェクション行列
  gfl2::math::Matrix44 projMatForOrtho[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewMatForOrtho[ gfl2::gfx::CtrDisplayNo::NumberOf ];
  gfl2::math::Matrix34 viewForOrtho = this->getOrthoViewMatrix();
  gfl2::math::Matrix44 projectionForOrtho = this->getOrthoProjectionMatrix();

#if defined(GF_PLATFORM_CTR)
  
  static f32 depthLevel = 30.0f;
  static f32 factor     = 0.15f;
  
#if PM_DEBUG
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu )
  {
    depthLevel = pFieldDebugMenu->GetDepthLevel();
    factor = pFieldDebugMenu->GetFactor();
  }
#endif // PM_DEBUG

  if( nn::gx::CTR::IsStereoVisionAllowed() == false || gfl2::gfx::ctr::CTRGL::IsStereoModeEnable() == false )
  {
    factor = 0.0f;
  }
  gfl2::gfx::GFGL::GetStereoMatrix( projection,
                                    view,
                                    depthLevel,
                                    factor, 
                                    &projMat[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &viewMat[gfl2::gfx::CtrDisplayNo::LEFT],
                                    &projMat[gfl2::gfx::CtrDisplayNo::RIGHT],
                                    &viewMat[gfl2::gfx::CtrDisplayNo::RIGHT]);

#elif defined(GF_PLATFORM_WIN32)

  projMat[gfl2::gfx::CtrDisplayNo::RIGHT] = projection;
  viewMat[gfl2::gfx::CtrDisplayNo::RIGHT] = view;
  projMat[gfl2::gfx::CtrDisplayNo::LEFT]  = projection;
  viewMat[gfl2::gfx::CtrDisplayNo::LEFT]  = view;

#endif // GL_PLATFORM_WIN32
  
  projMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT] = projectionForOrtho;
  viewMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT] = viewForOrtho;
  projMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT]  = projectionForOrtho;
  viewMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT]  = viewForOrtho;

  CaptureRenderingPipeLine* pPipeLine = this->GetCaptureRenderingPipeLine();

  switch( displayNo )
  {
    // 左画面描画
    case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::GFGL::SetActiveDisplay( System::GflUse::m_HandleList[ displayNo ] );

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer        ( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth      ( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight     ( System::GflUse::m_HandleList[ displayNo ] );

      // 正射影パスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetOrthoRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT];
        pCameraParam->m_View       = viewMatForOrtho[gfl2::gfx::CtrDisplayNo::LEFT];
      }

      // デフォルトのビュー・プロジェクション行列
      pPipeLine->SetViewMatrix      ( viewMat[gfl2::gfx::CtrDisplayNo::LEFT] );
      pPipeLine->SetProjectionMatrix( projMat[gfl2::gfx::CtrDisplayNo::LEFT] );
      pPipeLine->SetRenderTarget    ( pBackBuffer  );
      pPipeLine->SetDepthStencil    ( pDepthBuffer );
      pPipeLine->SetDisplaySize     ( displayWidth, displayHeight );

      pPipeLine->Update();
      pPipeLine->Execute();
      break;
    }

    // 右画面描画(立体視時のみ)
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::GFGL::SetActiveDisplay( System::GflUse::m_HandleList[ displayNo ] );

      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer        ( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth      ( System::GflUse::m_HandleList[ displayNo ] );
      u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight     ( System::GflUse::m_HandleList[ displayNo ] );

      // 正射影パスのビュー・プロジェクション行列をカスタム
      {
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCameraParam = pPipeLine->GetOrthoRenderPath()->GetCameraParamAddress();
        pCameraParam->m_Projection = projMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT];
        pCameraParam->m_View       = viewMatForOrtho[gfl2::gfx::CtrDisplayNo::RIGHT];
      }

      // デフォルトのビュー・プロジェクション行列
      pPipeLine->SetViewMatrix      ( viewMat[gfl2::gfx::CtrDisplayNo::RIGHT] );
      pPipeLine->SetProjectionMatrix( projMat[gfl2::gfx::CtrDisplayNo::RIGHT] );
      pPipeLine->SetRenderTarget    ( pBackBuffer  );
      pPipeLine->SetDepthStencil    ( pDepthBuffer );
      pPipeLine->SetDisplaySize     ( displayWidth, displayHeight );

      pPipeLine->CallDisplayList();
      break;
    }
  }
}

/**
 * @brief 天候ボードビュー行列取得
 */
gfl2::math::Matrix34 FieldRenderingPipeLineManager::getWeatherBoardViewMatrix()
{
  return gfl2::math::Matrix34(
    gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f),
    gfl2::math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f),
    gfl2::math::Vector4( 0.0f, 0.0f, 1.0f, -10.0f) );
}

/**
 * @brief 天候ボードプロジェクション行列取得
 */
gfl2::math::Matrix44 FieldRenderingPipeLineManager::getWeatherBoardProjectionMatrix()
{
  f32 orthoL =-20.0f;     // ニアクリッピング面の視錐台左辺のX座標
  f32 orthoR = 20.0f;     // ニアクリッピング面の視錐台右辺のX座標
  f32 orthoB =-12.0f;     // ニアクリッピング面の視錐台下辺のY座標
  f32 orthoT = 12.0f;     // ニアクリッピング面の視錐台上辺のY座標
  gfl2::math::Matrix44 projectionForWeatherBoard;
  nw::math::MTX44 proj = gfl2::math::ConvertGfMtx44ToNwMtx44( projectionForWeatherBoard );
  gfl2::math::MTX44OrthoPivot( 
    &proj,
    orthoL,
    orthoR,
    orthoB,
    orthoT,
    1.0f,
    1000.0f,
    gfl2::math::PIVOT_UPSIDE_TO_TOP );
  return gfl2::math::ConvertNwMtx44ToGfMtx44( proj );
}

/**
 * @brief 正射影ビュー行列取得
 */
gfl2::math::Matrix34 FieldRenderingPipeLineManager::getOrthoViewMatrix()
{
  return gfl2::math::Matrix34(
    gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f),
    gfl2::math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f),
    gfl2::math::Vector4( 0.0f, 0.0f, 1.0f, -10.0f) );
}

/**
 * @brief 正射影プロジェクション行列取得
 */
gfl2::math::Matrix44 FieldRenderingPipeLineManager::getOrthoProjectionMatrix()
{
  f32 orthoL =-200.0f;     // ニアクリッピング面の視錐台左辺のX座標
  f32 orthoR = 200.0f;     // ニアクリッピング面の視錐台右辺のX座標
  f32 orthoB =-120.0f;     // ニアクリッピング面の視錐台下辺のY座標
  f32 orthoT = 120.0f;     // ニアクリッピング面の視錐台上辺のY座標
  gfl2::math::Matrix44 projectionForOrtho;
  nw::math::MTX44 proj = gfl2::math::ConvertGfMtx44ToNwMtx44( projectionForOrtho );
  gfl2::math::MTX44OrthoPivot( 
    &proj,
    orthoL,
    orthoR,
    orthoB,
    orthoT,
    0.0f,
    1000.0f,
    gfl2::math::PIVOT_UPSIDE_TO_TOP );
  return gfl2::math::ConvertNwMtx44ToGfMtx44( proj );
}

/**
 *  @brief  カラーバッファ取得
 */
void FieldRenderingPipeLineManager::transferColorBuffer( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pTextureBuffer && no == gfl2::gfx::CtrDisplayNo::LEFT )
  {
    gfl2::gfx::GFGL::AddReadColorBufferToTextureCommand( m_pTextureBuffer );
    m_pTextureBuffer = NULL;
  }
  if( m_pColorBuffer && no == gfl2::gfx::CtrDisplayNo::LEFT )
  {
    gfl2::gfx::GFGL::AddTransferLinearColorBufferCommand( m_pColorBuffer, true );
    m_pColorBuffer = NULL;
  }
}

GFL_NAMESPACE_END( Field )

