//======================================================================
/**
 * @file FieldEventFieldDemo.cpp
 * @brief デモシーケンス再生イベント
 * @author saita_kazuki
 * @data 2015.11.10
 */
//======================================================================

#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Event/FieldEventFieldDemo.h"

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>
#include <Debug/include/gfl2_Assert.h>
#include <gfx/include/gfl2_GFGL.h>
#include <util/include/gfl2_std_string.h>
#include <System/include/Timer/gfl2_PerformanceCounter.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDummyPolygon.h"
#include "Field/FieldRo/include/FieldEnvManager.h"

// demo
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

// system
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// for debug
#ifdef DEBUG_ONLY_FOR_saita_kazuki
#if defined(GF_PLATFORM_WIN32)
#include  <gfx/include/gfl2_PCDebugUtil.h>
#endif // GF_PLATFORM_WIN32
#endif // DEBUG_ONLY_FOR_saita_kazuki


GFL_NAMESPACE_BEGIN( Field )


// Roモジュール
const char* const EventFieldDemo::RO_MODULES[ EventFieldDemo::RO_MODULE_NUM ] =
{
  //"SeqEditor.cro",  //Object,Background,SeqEditorはmomijiで統合されました。
  "SequencePlayer.cro",
  "Object.cro",
};



/**
 * @brief コンストラクタ
 * @param pHeap GameEventに渡すヒープ
 */ 
EventFieldDemo::EventFieldDemo( gfl2::heap::HeapBase* pHeap)
  : GameSys::GameEvent( pHeap)
  , m_desc()
  , m_isBoot( false)
  , m_pHeap( NULL)
  , m_pAllocator( NULL)
  , m_pFieldRenderingPipeLineManager( NULL)
  , m_pEffectManager( NULL)
  , m_initializeFuncList()
  , m_terminateFuncList()
  , m_pSequenceViewSystem( NULL)
  , m_pCapturePolygon( NULL)
  , m_pTextureResourceNode( NULL)
{
  gfl2::std::MemClear( m_pRoModules, sizeof( m_pRoModules));
}

/**
 * @brief デストラクタ
 */ 
EventFieldDemo::~EventFieldDemo()
{
}

/**
 * @brief 初期化処理(インスタンス生成直後に実行する)
 * @param desc 初期化設定
 */ 
void EventFieldDemo::Initialize( const Description& desc)
{
  m_desc = desc;
  m_isBoot = true;
}

/**
 * @brief イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 * @retval true 起動してよい
 * @retval false 起動しない
 */
bool EventFieldDemo::BootChk(GameSys::GameManager* gmgr)
{
  return m_isBoot;
}

/**
 * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 */
void EventFieldDemo::InitFunc(GameSys::GameManager* gmgr)
{
  SAITA_PRINT( "EventFieldDemo Start\n");

  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE);

  gfl2::heap::HeapBase* pHeapDLL = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD);
  this->LoadDllModules( pHeapDLL);

  m_initializeFuncList.CreateBuffer( m_pHeap, 8);
  m_terminateFuncList.CreateBuffer( m_pHeap, 8);

  m_pAllocator = GFL_NEW( m_pHeap) System::nijiAllocator( m_pHeap);

  m_pFieldRenderingPipeLineManager = m_desc.pFieldmap->GetFieldRenderingPipeLineManager();
  m_pEffectManager = m_desc.pFieldmap->GetEffectManager();

  this->InitializeOptionFunction();
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 * @return GameSys::GMEVENT_RESULT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventFieldDemo::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    INITIALIZE_OPTION,
    INITIALIZE_DEMO,
    START_DEMO,
    CHECK_DEMO_FINISH,
    TEMINATE_DEMO,
    TERMINATE_OPTION,
    END,
  };

  switch( this->GetSeqNo())
  {
  // オプション用初期化
  case INITIALIZE_OPTION:
    {
      if( this->UpdateOptionFunction( &m_initializeFuncList))
      {
        this->AddSeqNo();
      }
    }
    break;

  // デモシステム初期化
  case INITIALIZE_DEMO:
    {
      this->InitializeSequenceViewSystem();

      // 更新、描画呼び出しはパイプラインマネージャーに任せる
      m_pFieldRenderingPipeLineManager->SetSequenceViewSystem( m_pSequenceViewSystem);

      this->AddSeqNo();
    }
    break;

  // デモ再生
  case START_DEMO:
    {
      m_pSequenceViewSystem->SeqComFunc_PlaySequenceFile( m_desc.demoID);

      this->AddSeqNo();
    }
    break;

  // デモ再生終了チェック
  case CHECK_DEMO_FINISH:
    {
      if( m_pSequenceViewSystem->SeqComFunc_IsFinishSequenceFile())
      {
        this->AddSeqNo();
      }
    }
    break;

  // デモシステム破棄
  case TEMINATE_DEMO:
    {
      if( this->TerminateSequenceViewSystem())
      {
        // パイプラインマネージャーからremove
        m_pFieldRenderingPipeLineManager->SetSequenceViewSystem( NULL);

        this->AddSeqNo();
      }
    }
    break;

  // オプション用破棄
  case TERMINATE_OPTION:
    {
      if( this->UpdateOptionFunction( &m_terminateFuncList))
      {
        this->AddSeqNo();
      }
    }
    break;

  // 終了
  case END:
    return GameSys::GMEVENT_RES_FINISH;
    //break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 */
void EventFieldDemo::EndFunc(GameSys::GameManager* gmgr)
{
  m_pFieldRenderingPipeLineManager = NULL;
  m_pEffectManager = NULL;

  GFL_SAFE_DELETE( m_pAllocator);

  m_initializeFuncList.Clear();
  m_terminateFuncList.Clear();

  this->DisposeDllModules();

  m_pHeap = NULL;

  SAITA_PRINT( "EventFieldDemo End\n");
}

//------------------------------------------------------------------------------
// private
//------------------------------------------------------------------------------

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 */
bool EventFieldDemo::ConvertUseOption( u32 check, u32 bit)
{
  return !(check & bit);
}

/**
 * @brief キャプチャーリクエスト
 */
void EventFieldDemo::RequestCapture()
{
  gfl2::gfx::Texture* pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->GetColorBufferTexture( gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER);
  GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetFieldmap()->GetFieldRenderingPipeLineManager()->RequestTransferColorBuffer( pTexture);
}

/**
 * @brief DllLoad
 * @param pHeap ヒープ
 */
void EventFieldDemo::LoadDllModules( gfl2::heap::HeapBase* pHeap)
{
  gfl2::ro::RoManager*        pRoManager   = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager);
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();

  for( u32 i = 0; i < RO_MODULE_NUM; ++i)
  {
    m_pRoModules[i] = pRoManager->LoadModule( pFileManager, RO_MODULES[i], pHeap);
    pRoManager->StartModule( m_pRoModules[i]);
  }
}

/**
 * @brief DLLUnLoad
 */
void EventFieldDemo::DisposeDllModules()
{
  gfl2::ro::RoManager* pRoManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager);

  for( u32 i = 0; i < RO_MODULE_NUM; ++ i)
  {
    if( m_pRoModules[i])
    {
      pRoManager->DisposeModule( m_pRoModules[i]);
    }
  }
}

/**
 * @brief シーケンス初期化
 */
void EventFieldDemo::InitializeSequenceViewSystem()
{
  // 生成
  m_pSequenceViewSystem = GFL_NEW( m_pHeap) DemoLib::Sequence::SequenceViewSystem();

  // 未使用オプション適用
  m_pSequenceViewSystem->SetUseEffectSystem( false);
  m_pSequenceViewSystem->SetUseCharaModel( this->ConvertUseOption( m_desc.unUseOption, UN_USE_OPTION_CHARA_MODEL));
  m_pSequenceViewSystem->SetUseCommonTextWindow( this->ConvertUseOption( m_desc.unUseOption, UN_USE_OPTION_TEXT_WINDOW));
  m_pSequenceViewSystem->SetUsePokemonSystem( this->ConvertUseOption( m_desc.unUseOption, UN_USE_OPTION_POKEMON));

  // 描画使用オプション適用
  m_pSequenceViewSystem->SetUseOption( m_desc.renderUseOption);

  // その他
  m_pSequenceViewSystem->SetForceOneStep( m_desc.isForceOneStep);

  // 初期化
  const u32 HEAP_SIZE_DEMO_SYSTEM = 0x80000;
  const u32 HEAP_SIZE_DEMO_DEVICE = 0x300000;
  m_pSequenceViewSystem->Initialize( m_pHeap, m_pHeap, HEAP_SIZE_DEMO_SYSTEM, HEAP_SIZE_DEMO_DEVICE);

  // テクスチャ差し替え準備
  if( m_desc.pTextureNode)
  {
    m_pSequenceViewSystem->SetReplaceTexture( m_desc.textureIndex, m_desc.pTextureNode);
  }

  // リアルタイムモードは無効
  m_pSequenceViewSystem->SetRealTimeMode( false);
}

/**
 * @brief シーケンス破棄
 */
bool EventFieldDemo::TerminateSequenceViewSystem()
{
  if( m_pSequenceViewSystem->Finalize())
  {
    GFL_SAFE_DELETE( m_pSequenceViewSystem);
    return true;
  }
  return false;
}

/**
 * @brief オプション関数リスト初期化
 */
void EventFieldDemo::InitializeOptionFunction()
{
  // フィールド描画をキャプチャーにする
  if( m_desc.option & OPTION_USE_FIELD_DRAW_CAPTURE)
  {
    m_initializeFuncList.PushBack( OptionFunction( this, &EventFieldDemo::InitializeCapturePipeLine) );
    m_initializeFuncList.PushBack( OptionFunction( this, &EventFieldDemo::InitializeCaptureTexture) );
    m_initializeFuncList.PushBack( OptionFunction( this, &EventFieldDemo::InitializeCapturePolygon) );
    m_initializeFuncList.PushBack( OptionFunction( this, &EventFieldDemo::ChangeExecuteTypeCapture) );

    m_terminateFuncList.PushBack( OptionFunction( this, &EventFieldDemo::ChangeExecuteTypeNormal) );
    m_terminateFuncList.PushBack( OptionFunction( this, &EventFieldDemo::TerminateCapturePolygon) );
    m_terminateFuncList.PushBack( OptionFunction( this, &EventFieldDemo::TerminateCaptureTexture) );
    m_terminateFuncList.PushBack( OptionFunction( this, &EventFieldDemo::TerminateCapturePipeLine) );
  }
}

/**
 * @brief オプション関数リスト更新
 */
bool EventFieldDemo::UpdateOptionFunction( gfl2::util::List<OptionFunction>* pList)
{
  gfl2::util::List<OptionFunction>::Iterator iter = pList->Begin();
  u32 finishCnt = 0;
  for( ; iter != pList->End(); ++iter, ++finishCnt )
  {
    if( (*iter).IsFinish() )
    {
      continue;
    }

    if( (*iter).Execute() == false )
    {
      break;
    }
  }

  if( finishCnt == pList->Size())
  {
    return true;
  }
  return false;
}

/**
 * @brief キャプチャー用パイプライン初期化
 */
bool EventFieldDemo::InitializeCapturePipeLine( s32* pSeq)
{
  FieldRenderingPipeLineManager::CapturePipeLineCreateDescription desc;
  desc.pHeap        = m_pHeap;
  desc.pAllocator   = m_pAllocator;
  desc.pDrawEnvNode = m_desc.pFieldmap->GetFieldEnvManager()->GetDrawEnvNode();
  m_pFieldRenderingPipeLineManager->CreateCaptureRenderingPipeLine( desc);

  return true;
}

/**
 * @brief キャプチャー用パイプライン破棄
 */
bool EventFieldDemo::TerminateCapturePipeLine( s32* pSeq)
{
  m_pFieldRenderingPipeLineManager->DeleteCaptureRenderingPipeLine();

  return true;
}

/**
 * @brief キャプチャーテクスチャ初期化
 */
bool EventFieldDemo::InitializeCaptureTexture( s32* pSeq)
{
  switch( *pSeq)
  {
  case 0:
    {
      this->RequestCapture();
      (*pSeq)++;
    }
    break;

  case 1:
    {
      gfl2::gfx::Texture* pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->GetColorBufferTexture( gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER);
  
      const c8* pFactoryName;
      pFactoryName = gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName();
      gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory;
      pTextureResourceNodeFactory = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>( gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory( gfl2::renderingengine::scenegraph::resource::IResourceData::Type( pFactoryName)));
  
      m_pTextureResourceNode = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>( pTextureResourceNodeFactory->CreateResourceNode( m_pAllocator, "capture", pTexture, false));

#ifdef DEBUG_ONLY_FOR_saita_kazuki
#if defined(GF_PLATFORM_WIN32)
      // Win32のみキャプチャを画像として保存しておく(確認用)
      void * addr = pTexture->Lock();
      gfl2::gfx::PCDebugUtil::PrintBlockImage( addr, "screenshot/colorbuffer_block.bmp", 512, 256, 4, m_pHeap );
      gfl2::gfx::PCDebugUtil::PrintImage( addr, "screenshot/colorbuffer_liner.bmp", 512, 256, 4, m_pHeap );
      pTexture->UnLock();
#endif // GF_PLATFORM_WIN32
#endif // DEBUG_ONLY_FOR_saita_kazuki
    }
    return true;
    //break;
  }

  return false;
}

/**
 * @brief キャプチャーテクスチャ破棄
 */
bool EventFieldDemo::TerminateCaptureTexture( s32* pSeq)
{
  GFL_SAFE_DELETE( m_pTextureResourceNode);

  return true;
}

/**
 * @brief キャプチャーテクスチャ貼り付け用板ポリゴン初期化
 */
bool EventFieldDemo::InitializeCapturePolygon( s32* pSeq)
{
  m_pCapturePolygon = reinterpret_cast<Effect::EffectDummyPolygon*>( m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_DUMMY_POLYGON, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f)));
  if( m_pCapturePolygon)
  {
    m_pCapturePolygon->GetBaseModel()->SetVisible( true);

    // テクスチャ差し替え
    poke_3d::model::BaseModel* pCapturePolygonModel = m_pCapturePolygon->GetBaseModel();
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode = pCapturePolygonModel->GetModelInstanceNode();
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pModelNode->GetMaterialInstanceNode( 0);
    pMaterialNode->SetTextureResourceNode( 0, m_pTextureResourceNode);

#if defined(GF_PLATFORM_CTR)
    // CTRのみ、UV値の回転と平行移動
    pCapturePolygonModel->SetMaterialTextureCoordinatorRotateUV(0, 0, GFL_MATH_DEG_TO_RAD(-90));
    gfl2::math::Vector2 trans( 0.0f, 0.218750f );
    pCapturePolygonModel->SetMaterialTextureCoordinatorTranslateUV(0, 0, trans);
#endif // GF_PLATFORM_CTR

    m_pFieldRenderingPipeLineManager->GetCaptureRenderingPipeLine()->GetOrthoRenderPath()->AddRenderingTarget( pModelNode);
  }

  return true;
}

/**
 * @brief キャプチャーテクスチャ貼り付け用板ポリゴン破棄
 */
bool EventFieldDemo::TerminateCapturePolygon( s32* pSeq)
{
  if( m_pCapturePolygon == NULL)
  {
    return true;
  }

  switch( *pSeq)
  {
  case 0:
    {
      m_pCapturePolygon->GetBaseModel()->SetVisible( false);
      poke_3d::model::BaseModel* pCapturePolygonModel = m_pCapturePolygon->GetBaseModel();
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode = pCapturePolygonModel->GetModelInstanceNode();
      m_pFieldRenderingPipeLineManager->GetCaptureRenderingPipeLine()->GetOrthoRenderPath()->RemoveRenderingTarget( pModelNode);

      (*pSeq)++;
    }
    break;

  case 1:
    {
      m_pEffectManager->DeleteEffect( m_pCapturePolygon);
      m_pCapturePolygon = NULL;
    }
    return true;
    //break;
  }

  return false;
}

/**
 * @brief キャプチャーパイプラインに切り替え
 */
bool EventFieldDemo::ChangeExecuteTypeCapture( s32* pSeq)
{
  m_pFieldRenderingPipeLineManager->ChangeExecuteType( FieldRenderingPipeLineManager::EXECUTE_TYPE_CAPTURE);

  return true;
}

/**
 * @brief 通常パイプラインに切り替え
 */
bool EventFieldDemo::ChangeExecuteTypeNormal( s32* pSeq)
{
  m_pFieldRenderingPipeLineManager->ChangeExecuteType( FieldRenderingPipeLineManager::EXECUTE_TYPE_NORMAL);

  return true;
}


//------------------------------------------------------------------------------
// OptionFunction
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */
EventFieldDemo::OptionFunction::OptionFunction()
  : m_pOwner( NULL)
  , m_pFunc( NULL)
  , m_seq( 0)
  , m_isFinish( false)
{
}

/**
 * @brief コンストラクタ
 */
EventFieldDemo::OptionFunction::OptionFunction( EventFieldDemo* pOwner, EventFieldDemo::OptionFunc pFunc)
  : m_pOwner( pOwner)
  , m_pFunc( pFunc)
  , m_seq( 0)
  , m_isFinish( false)
{
}

/**
 * @brief デストラクタ
 */
EventFieldDemo::OptionFunction::~OptionFunction()
{
}

/**
 * @brief 実行
 */
bool EventFieldDemo::OptionFunction::Execute()
{
  if( m_pFunc)
  {
    if( (m_pOwner->*m_pFunc)( &m_seq) )
    {
      m_isFinish = true;
      return true;
    }
    return false;
  }
  return true;
}

/**
 * @brief 終了したか
 * @retval true 終了
 * @retval false 実行中
 */
bool EventFieldDemo::OptionFunction::IsFinish() const
{
  return m_isFinish;
}

GFL_NAMESPACE_END( Field )

