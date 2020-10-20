#if PM_DEBUG

//======================================================================
/**
 * @file PokeModelTestModel.cpp
 * @date 2015/09/07 11:23:04
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestModel.h>
#include <GameSys/include/GameManager.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeModel::PokeModel( AppHeap* heap, AppRenderMan* renderMan )
  : m_pHeap( heap )
  , m_pRenderMan( renderMan )
  , m_pPokeModelSystem( NULL )
  , m_pSimpleModel( NULL )
  , m_Seq( 0 )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeModel::~PokeModel( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
bool PokeModel::Initialize( void )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  switch( m_Seq )
  {
  case 0:
    {
      m_pPokeModelSystem = GFL_NEW(devHeap) PokeTool::PokeModelSystem();
      GFL_ASSERT( m_pPokeModelSystem );

      m_pPokeModelSystem->InitSystemAsync(
        devHeap,
        devHeap,
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),
        1 );

      m_Seq++;
    } break;

  case 1:
    {
      if( !m_pPokeModelSystem->IsFinishInitSystemAsync() ) break;

      m_pPokeModelSystem->CreateSystem( false, NULL );

      // ポケモン用ヒープ作成
      PokeTool::PokeModelSystem::HeapOption opt;
      opt.animeType       = PokeTool::MODEL_ANIMETYPE_BATTLE;
      opt.useIdModel      = false;
      opt.useShadow       = false;
      opt.useColorShader  = false;
      m_pPokeModelSystem->CreatePokeModelHeap( devHeap, &opt );

      m_Seq++;
    } break;

  case 2:
    {
      PokeModelTool::INIT_PARAM initParam;

      {
        PokeTool::PokeModel::SetupOption* pOpt = &initParam.setupOpt;
        pOpt->dataHeap = devHeap;
        pOpt->workHeap = devHeap->GetLowerHandle();
      }

      initParam.pPokeModelSys       = m_pPokeModelSystem;
      initParam.pRenderingManager   = m_pRenderMan;
      initParam.renderPlace         = app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY;

      m_pSimpleModel = GFL_NEW(devHeap) PokeModelTool( initParam );

      //  位置は仮!
      gfl2::math::Vector3 pos( -175, -130, 0 );
      m_pSimpleModel->SetPos( pos );

      //  アニメ設定は仮
      m_pSimpleModel->SetAnime( PokeTool::MODEL_ANIME_BT_WAIT_A, true );
      m_pSimpleModel->SetVisible( false );

      m_Seq++;
    } break;

  case 3:
    {
      m_Seq = 0;

      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
bool PokeModel::Terminate( void )
{
  switch( m_Seq )
  {
  case 0:
    {
      m_pSimpleModel->Terminate();
      m_Seq++;
    } break;

  case 1:
    {
      if( !m_pSimpleModel->IsDelete() ) break;

      m_Seq++;
    } break;

  case 2:
    {
      m_pPokeModelSystem->DeletePokeModelHeap();
      m_pPokeModelSystem->TermSystemAsync();

      m_Seq++;
    } break;

  case 3:
    {
      if( !m_pPokeModelSystem->IsFinishTermSystemAsync() ) break;

      m_Seq++;
    } break;

  case 4:
    {
      GFL_SAFE_DELETE( m_pSimpleModel );
      GFL_SAFE_DELETE( m_pPokeModelSystem );

      m_Seq++;
    } break;

  case 5:
    {
      m_Seq = 0;
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void PokeModel::Update( void )
{
  if( m_pSimpleModel )
  {
    m_pSimpleModel->Update();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルのセット
 * @param   monsNo
 */
//------------------------------------------------------------------------------
void PokeModel::SetPokeModel( MonsNo monsNo )
{
  if( m_pSimpleModel )
  {
    m_pSimpleModel->StartLoad( monsNo );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルの準備チェック
 */
//------------------------------------------------------------------------------
bool PokeModel::IsModelReady( void ) const
{
  if( m_pSimpleModel )
  {
    return m_pSimpleModel->IsReady();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルの表示
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void PokeModel::SetVisibleModel( bool visible )
{
  if( m_pSimpleModel )
  {
    m_pSimpleModel->SetVisible( visible );
  }
}

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG