//======================================================================
/**
 * @file DendouDemoView.cpp
 * @date 2016/03/01 14:58:14
 * @author  nagihashi_toru(移植), Hiroyuki Nakamura
 * @brief グラフィックス関連 基本移植だが、uiViewに置き換えた箇所もある
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include <GameSys/include/GameManager.h>
#include <Savedata/include/MyStatus.h> 
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/EffectUtil.h>
#include <model/include/gfl2_BaseCamera.h>
#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>

//mine
#include "DendouDemoView.h"
#include "DendouDemoPokeModel.h"
#include "DendouDemoPlayerModel.h"
#include "DendouDemoFileLoader.h"
#include "DendouDemoDebug.h"

//resource
#include <niji_conv_header/app/dendou_demo/DendouDemo.h>
#include <niji_conv_header/app/dendou_demo/DendouDemo_pane.h>
#include <niji_conv_header/app/dendou_demo/DendouDemo_anim_list.h>

#include <Debug/include/gfl2_DebugTick.h>

namespace app {
namespace dendou_demo {

namespace
{
//! 2DリソースID
enum {
	RES_ID_2D_DENDOUDEMO = 0,			//!< 殿堂入りデモ
};

static const MonsNo MIN_BASE_HEIGHT_POKEMON = MONSNO_BATYURU;	//!< 高さ取得用：最小ポケモン（バチュル）
static const MonsNo MAX_BASE_HEIGHT_POKEMON = MONSNO_RUGIA;		//!< 高さ取得用：最大ポケモン（ルギア）

GFL_DEBUG_TICK_SETUP(dendou_demo, 10 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//--------------------------------------------------------------------------------------------
View::View( app::util::Heap * heap, gfl2::fs::AsyncFileManager * man, app::util::AppRenderingManager * renderingManager ) :
  app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator()),
  pHeap(heap),
  pRenderingManager(renderingManager),
  pFileManager(man),
  pPokeModel(NULL),
  pPlayerModel(NULL),
	endFlag( false ),
	playerModelSetup( false ),
  pEffectUtil(NULL),
  pCamera(NULL),
  pFloorModel(NULL),
  loadSeq(0),
  setupSeq(0),
  finalizeSeq(0),
  sex(0),
  pCameraMotNode(NULL),
  pFloorMotNode(NULL),
  pFloorMdlNode(NULL),
  pEnvResNode(NULL),
  pEnvNode(NULL)
{
  pFileLoader = GFL_NEW(heap->GetSystemHeap()) FileLoader( heap->GetDeviceHeap(), pFileManager );
  GFL_DEBUG_TICK_SET_FLAG(dendou_demo,true);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
View::~View()
{
  GFL_SAFE_DELETE( pFileLoader );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		初期化
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Initialize()
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( pHeap, false );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		終了処理
 *
 * @param		none
 *
 * @retval	"true = 完了"
 * @retval	"false = 処理中"
 */
//--------------------------------------------------------------------------------------------
bool View::Finalize(void)
{
	endFlag = true;

	if( Delete3D() == false ){
		return false;
	}
	if( Delete2D() == false ){
		return false;
	}

  loadSeq = 0;
  setupSeq = 0;

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Update(void)
{
GFL_DEBUG_TICK_START(dendou_demo, 0, "UIView" );
  app::ui::UIView::Update();
GFL_DEBUG_TICK_END(dendou_demo,0);
	if( endFlag == true ){
		return;
	}

#ifndef DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE
	if( playerModelSetup == false ){
		playerModelSetup = pPlayerModel->Setup();
	}
#endif	// DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE

	this->Update3D();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		描画
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Draw( gfl2::gfx::CtrDisplayNo no )
{
	if( endFlag == true ){
		return;
	}
	this->Draw3D();

  if( GetLayoutWork(0) != NULL )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( pRenderingManager, no, 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		2D関連作成
 *
 * @param		heap		ヒープ
 * @param		man			ファイルリードマネージャ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Create2D( app::util::Heap * heap, gfl2::fs::AsyncFileManager * man )
{
  // アーカイブIDテーブル
  const ::app::util::G2DUtil::LytResData aResTbl[] =
  {
    {  pFileLoader->GetLayoutFileBuffer(), 0, ::app::util::G2DUtil::ATTACH_ROOT },
  };

  //  作成するレイアウトワーク
  const ::app::util::G2DUtil::LytwkData aSetupData[] =
  {
    {
      0,
      LYTRES_DENDOUDEMO_DENDOUDEMO_BPART_UP_00_BFLYT,
      LA_DENDOUDEMO_DENDOUDEMO_BPART_UP_00___NUM,
      pFileLoader->GetLayoutFileBuffer(),
      true,
      ::app::util::G2DUtil::SETUP_UPPER,
      true
      }
  };

  ::app::ui::UIView::Create2D( heap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), app::ui::UIView::UNUSE_MESSAGE_DATA );


}

//--------------------------------------------------------------------------------------------
/**
 * @brief		2D関連削除
 *
 * @param		none
 *
 * @retval	"true = 完了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::Delete2D(void)
{
  app::ui::UIView::Delete2D();

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3D関連作成
 *
 * @param		heap		ヒープ
 * @param		man			ファイルリードマネージャ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Create3D( app::util::Heap * heap, gfl2::fs::AsyncFileManager * man )
{
	CreateEnv( heap );
	CreateFloorModel( heap );
	CreateParticle( heap );

  {
    PokeModel::Description desc;
    desc.heap = heap;
    desc.asyncFileManager = man;
    desc.renderingManager = pRenderingManager;
    for( int i = 0; i < 6; ++i )
    {
      desc.spotModelFileBuffer[i] = pFileLoader->GetSpotModelFileBuffer(i);
    }
	  pPokeModel   = GFL_NEW( heap->GetSystemHeap() ) PokeModel( desc );
    pPokeModel->SetCamera( pCamera );
  }


  {
    PlayerModel::Description desc;
    desc.heap = heap;
    desc.asyncFileManager = man;
    desc.renderingManager = pRenderingManager;
	  pPlayerModel = GFL_NEW( heap->GetSystemHeap() ) PlayerModel( desc );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3D関連削除
 *
 * @param		none
 *
 * @retval	"true = 完了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::Delete3D(void)
{
	if( pPlayerModel != NULL ){
		if( DeletePlayerModel() == false ){
			return false;
		}
		GFL_DELETE pPlayerModel;
		pPlayerModel = NULL;
	}

	if( pPokeModel != NULL ){
    if( pPokeModel->Finalize() == false ) {
      return false;
    }

		GFL_DELETE pPokeModel;
		pPokeModel = NULL;
	}
	DeleteParticle();
	DeleteFloorModel();

	DeleteEnv();

	return true;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		ファイル読み込みチェック
 *
 * @param		none
 *
 * @retval	"true = 読み込み中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::CheckFileRead(const Savedata::MyStatus * myst)
{
  enum
  {
    SEQ_START,
    SEQ_END,
  };

  switch( loadSeq )
  {
  case SEQ_START:
    sex = myst->GetSex();
    pFileLoader->StartLoadAsync( sex, pHeap->GetDeviceHeap(), pHeap->GetDeviceHeap()->GetLowerHandle(), pHeap->GetDeviceHeap()->GetLowerHandle() );
    loadSeq++;
    //fall
  case SEQ_END:
    pFileLoader->Update();
    if( pFileLoader->IsComplete()){
      return false;
    }
    return true;  //読込中
  default:
    GFL_ASSERT(0);
    return false;
  }

}


//--------------------------------------------------------------------------------------------
/**
 * @brief		システム初期化セットアップ中
 *
 * @param		none
 *
 * @retval	"true = セットアップ中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::Setup(void)
{
  switch( setupSeq )
  {
  case 0:
    Create2D( pHeap, pFileManager );
    Create3D( pHeap, pFileManager );
    setupSeq++;
    //fall
  case 1:
    if( pPokeModel->Initialize() )
    {
      return false;
    }
    else
    {
      return true;
    }
  default:
    GFL_ASSERT(0);
    return false;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		レイアウトアニメ再生
 *
 * @param		anm		アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::SetLytAnime( u32 anm )
{
  if( GetLayoutWork(0) )
  {
    GetG2DUtil()->StartAnime( LYTID_TOP, anm );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		レイアウトアニメ再生チェック
 *
 * @param		anm		アニメ番号
 *
 * @retval	"true = 再生中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::CheckLytAnime( u32 anm )
{
  if( GetLayoutWork(0) )
  {
    if( GetG2DUtil()->IsAnimeEnd( LYTID_TOP, anm ) == true ){
      return false;
    }
  }
	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3Dアップデート
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Update3D(void)
{
  if( pCamera )
  {
GFL_DEBUG_TICK_START(dendou_demo, 1, "UIView" );
    pCamera->UpdateAnimation();
    NAGI_PRINT("frame %d\n", (int)pCamera->GetAnimationFrame());
GFL_DEBUG_TICK_END(dendou_demo,1);
  }

  if( pFloorModel )
  {
GFL_DEBUG_TICK_START(dendou_demo, 2, "FloorModel" );
    pFloorModel->UpdateAnimation();
GFL_DEBUG_TICK_END(dendou_demo,2);
  }

  if( pPlayerModel )
  {
GFL_DEBUG_TICK_START(dendou_demo, 3, "PlayerModel" );
    pPlayerModel->Update();
GFL_DEBUG_TICK_END(dendou_demo,3);
  }

  if( pPokeModel )
  {
GFL_DEBUG_TICK_START(dendou_demo, 4, "PokeModel" );
    pPokeModel->Update();
GFL_DEBUG_TICK_END(dendou_demo,4);
  }

  if( pPokeModel )
  {
GFL_DEBUG_TICK_START(dendou_demo, 5, "PokeModelSys" );
	  pPokeModel->UpdateSystem();
GFL_DEBUG_TICK_END(dendou_demo,5);
  }

  if( pEffectUtil )
  {
GFL_DEBUG_TICK_START(dendou_demo, 6, "Effect" );
    pEffectUtil->UpdateForMode1Res1EmitterSet1Group();
GFL_DEBUG_TICK_END(dendou_demo,6);
  }

GFL_DEBUG_TICK_DUMP_ALL(dendou_demo);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3D描画
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::Draw3D(void)
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3Dカメラアニメ開始
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::StartCameraAnime(void)
{
  pCamera->SetAnimationStepFrame(1.0f);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3Dカメラアニメ再生フレーム取得
 *
 * @param		none
 *
 * @return	再生フレーム
 */
//--------------------------------------------------------------------------------------------
f32 View::GetCameraFrame(void) const
{
	return pCamera->GetAnimationFrame();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		3Dカメラアニメの最終フレームかどうか
 *
 * @param		none
 *
 * @return	trueで最終フレーム
 */
//--------------------------------------------------------------------------------------------
b32 View::IsCameraLastFrame(void) const
{
	return pCamera->IsAnimationLastFrame();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カメラアニメ設定
 *
 * @param		flg		true = 再生、false = 停止
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::SetPlayCameraAnime( bool flg )
{
	if( flg == false ){
		pCamera->SetAnimationStepFrame( 0.0f );
	}else{
		pCamera->SetAnimationStepFrame( 1.0f );
	}

}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ライト作成
 *
 * @param		heap	ヒープ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreateEnv( app::util::Heap * heap )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envData;
  envData.SetBinaryEnvData(static_cast<c8*>(pFileLoader->GetDrawEnvFileBuffer()));
  pEnvResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(heap->GetDevGLHeapAllocator(), &envData);
  pEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(heap->GetDevGLHeapAllocator(), pEnvResNode);

  pRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pEnvNode, 0);
  pRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pEnvNode, 1);
  pRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pEnvNode, 2);

  // BaseCamera
  pCamera = GFL_NEW(heap->GetSystemHeap()) poke_3d::model::BaseCamera;
  {
    pCamera->Create(pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), pEnvNode, 0);
    pRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pCamera, 0);
    pRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pCamera, 1);
    pRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pCamera, 2);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
    anmRes.SetMotionData((c8*)pFileLoader->GetCameraMotionFileBuffer());
    pCameraMotNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( heap->GetDevGLHeapAllocator(), &anmRes );
    //pCamera->ChangeAnimationLocalByResourceNode( pCameraMotNode );
    pCamera->ChangeAnimationGlobalByResourceNode( pCameraMotNode );
    pCamera->SetAnimationStepFrame(0.0f);
    pCamera->SetAnimationLoop(true);

#if PM_DEBUG
    GFL_SINGLETON_INSTANCE(Debug)->SetupCamera(pCamera);
#endif
  }

  gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f)); 
  pEnvNode->SetAABB(envSpace); 
 

  gfl2::math::Vector3 pos = gfl2::math::Vector3(0.0f, 0.0f, 0.0f); 
  gfl2::math::Vector3 direction = gfl2::math::Vector3( 1.8f, -1.8f, -1.5f);
  // ポケモン用ライト 
  { 
    // ライトセット 
    gfl2::renderingengine::scenegraph::instance::LightSetNode* lightSetNode; 
    lightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(pHeap->GetDevGLHeapAllocator()); 
    lightSetNode->SetSetNo( 0 ); 
 
    // ライト 
    gfl2::renderingengine::scenegraph::instance::LightNode* directionalLightNode; 
    directionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pHeap->GetDevGLHeapAllocator()); 
    directionalLightNode->SetNodeName( "DirectionalLight" ); 
    gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(directionalLightNode->GetNodeName()) ); 
    directionalLightNode->SetNodeNameHash( nameHash.value ); 
    directionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional ); 
    //gfl2::math::Vector3 pos = gfl2::math::Vector3(00000000.0f, 10000000.0f, 00000000.0f);
    directionalLightNode->SetPosition( pos );
    directionalLightNode->SetDirection( direction ); 
    directionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) ); 
 
#if PM_DEBUG
    GFL_SINGLETON_INSTANCE(Debug)->SetupPokemonDirectionalLight( directionalLightNode );
#endif

    // 追加 
    lightSetNode->AddLight( directionalLightNode ); 
    pEnvNode->AddLightSet( lightSetNode ); 
  } 

  //トレーナー用ライト
  {
    // ライトセット 
    gfl2::renderingengine::scenegraph::instance::LightSetNode* lightSetNode; 
    {
      lightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(pHeap->GetDevGLHeapAllocator()); 
      lightSetNode->SetSetNo( 2 ); 
    }

    // ライト 
    gfl2::renderingengine::scenegraph::instance::LightNode* directionalLightNode; 
    {
      directionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pHeap->GetDevGLHeapAllocator()); 
      directionalLightNode->SetNodeName( "DirectionalLight" ); 
      gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(directionalLightNode->GetNodeName()) ); 
      directionalLightNode->SetNodeNameHash( nameHash.value ); 
      directionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional ); 
      directionalLightNode->SetPosition( pos ); 
      //gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.466435f, -0.171028f, -0.867834f); 
      directionalLightNode->SetDirection( direction ); 
      directionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) ); 
    }

    // ライト 
    gfl2::renderingengine::scenegraph::instance::LightNode* ambientLightNode; 
    {
      ambientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pHeap->GetDevGLHeapAllocator()); 
      ambientLightNode->SetNodeName( "AmbientLight" ); 
      gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(ambientLightNode->GetNodeName()) ); 
      ambientLightNode->SetNodeNameHash( nameHash.value ); 
      ambientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient ); 
      ambientLightNode->SetColor( gfl2::math::Vector3( 0.2f, 0.2f, 0.2f ) ); 
    } 

#if PM_DEBUG
    GFL_SINGLETON_INSTANCE(Debug)->SetupTrainerDirectionalLight( directionalLightNode );
    GFL_SINGLETON_INSTANCE(Debug)->SetupTrainerAmbiaentLight( ambientLightNode );
#endif

    // 追加 
    lightSetNode->AddLight( directionalLightNode ); 
    lightSetNode->AddLight( ambientLightNode ); 
    pEnvNode->AddLightSet( lightSetNode ); 
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ライト削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::DeleteEnv(void)
{

  GFL_SAFE_DELETE( pCameraMotNode );
  GFL_SAFE_DELETE( pCamera );
  GFL_SAFE_DELETE(pEnvNode);  // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも破棄されるらしい
  GFL_SAFE_DELETE(pEnvResNode);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		床モデル作成
 *
 * @param		af		アークファイル
 * @param		heap	ヒープ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreateFloorModel( app::util::Heap * heap )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
  modelDataPack.SetModelData(static_cast<c8*>(pFileLoader->GetBGModelFileBuffer()));
  pFloorMdlNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(heap->GetDevGLHeapAllocator(), &modelDataPack);

  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
  anmRes.SetMotionData((c8*)pFileLoader->GetBGMotionFileBuffer());
  pFloorMotNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( heap->GetDevGLHeapAllocator(), &anmRes );


  pFloorModel = GFL_NEW(heap->GetSystemHeap()) poke_3d::model::BaseModel;
  pFloorModel->Create(heap->GetDevGLHeapAllocator(), heap->GetDeviceHeap(), pFloorMdlNode);
  pRenderingManager->AddModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pFloorModel, 0, false );

  pFloorModel->ChangeAnimationByResourceNode( pFloorMotNode );
  pFloorModel->SetAnimationStepFrame(1.0f);
  pFloorModel->SetAnimationLoop(true);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		床モデル削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::DeleteFloorModel(void)
{
  pRenderingManager->RemoveModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pFloorModel, 0 );
  pFloorModel->Destroy();
  GFL_SAFE_DELETE( pFloorModel );

  GFL_SAFE_DELETE( pFloorMotNode );
  GFL_SAFE_DELETE( pFloorMdlNode );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモデル作成
 *
 * @param		pp		PokemonParamのテーブル
 * @param		max		モデル数
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreatePokeModel( const pml::pokepara::PokemonParam ** pp, u32 max )
{
	pPokeModel->InitPosition( max );
	for( u32 i=0; i<max; i++ ){
    PokeTool::SimpleParam param;
#if PM_DEBUG
    if( GFL_SINGLETON_INSTANCE(Debug)->UseDebugPokemon() )
    {
      GFL_SINGLETON_INSTANCE(Debug)->GetSimpleParam( i, param );
    }
    else
#endif
    PokeTool::GetSimpleParam( &param, pp[i] );
    GFL_ASSERT( param.isEgg == false );
		CreatePokeModel( i, param );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモデル作成
 *
 * @param		id	モデルID
 * @param		pp	PokemonParam
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreatePokeModel( u32 id, const PokeTool::SimpleParam & param )
{
	pPokeModel->Create( id, param );
	pPokeModel->SetVisible( id, false );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモデルが生成されたか
 *
 * @param		id	モデルID
 *
 * @retval	"true = 生成済み"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::IsPokeModelCreate( u32 id )
{
	return pPokeModel->CheckCreate( id );
}
//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモデルが生成されたか（ぜんぶ）
 *
 * @param   num モデル数
 *
 * @retval	"true = 生成済み"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::IsPokeModelCreateAll( u32 max )
{
  bool isEnd = true;
	for( u32 i=0; i<max; i++ ){
	  isEnd &= pPokeModel->CheckCreate( i );
  }
  return isEnd;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモデル表示切り替え
 *
 * @param		id		モデルID
 * @param		flg		true = 表示、false = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::SetVisiblePokeMondel( u32 id, bool flg )
{
	pPokeModel->SetVisible( id, flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ループシーンのポケモン配置
 *
 * @param		id		モデルID
 * @param		pos		配置位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::PutLoopPokemon( u32 id, u32 pos )
{
	pPokeModel->SetVisible( id, true );
	pPokeModel->SetPosition( id, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		初期化用ダミーポケモデル作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreateDummyPokeModel(void)
{
	PokeTool::SimpleParam param;

	param.isRare  = false;
	param.isEgg   = false;
	param.perRand = 0;

	// 最小ポケモン
	param.monsNo  = MIN_BASE_HEIGHT_POKEMON;
	param.formNo  = 0;
	param.sex     = pml::SEX_MALE;

	pPokeModel->Create( 0, param );
	pPokeModel->SetVisible( 0, false );

	// 最大ポケモン
	param.monsNo  = MAX_BASE_HEIGHT_POKEMON;
	param.formNo  = 0;
	param.sex     = pml::SEX_UNKNOWN;

	pPokeModel->Create( 1, param );
	pPokeModel->SetVisible( 1, false );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		初期化用ダミーポケモデルからパラメータを設定
 *
 * @param		none
 *
 * @retval	"true = 完了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::SetDummyPokeModelParam(void)
{
	if( pPokeModel->CheckCreate( 0 ) == true &&
			pPokeModel->CheckCreate( 1 ) == true ){
		pPokeModel->InitBaseData( 0, 1 );
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデル作成
 *
 * @param		myst								プレイヤーデータ
 * @param		resident_resource		ゲーム内常駐リソース
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreatePlayerModel(
			const Savedata::MyStatus * myst )
{
	pPlayerModel->Create( myst );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデル削除
 *
 * @param		none
 *
 * @retval	"true = 削除完了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::DeletePlayerModel(void)
{
	return pPlayerModel->Delete();
}

#ifdef DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE
//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデルの設定が完了したか
 *
 * @param		none
 *
 * @retval	"true = 完了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::SetupPlayerModel(void)
{
	return pPlayerModel->Setup();
}
#endif	// DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE

//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデルが生成されているか
 *
 * @param		none
 *
 * @retval	"true = 生成済み"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool View::IsPlayerModelCreate(void)
{
	return pPlayerModel->IsCreated();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデル表示切り替え
 *
 * @param		flg		true = 表示、false = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::SetVisiblePlayerModel( bool flg )
{
	if( pPlayerModel->IsCreated() == false ){
		GFL_ASSERT( 0 );
		return;
	}
	pPlayerModel->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデルの終了モーションを再生
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::SetPlayerEndMotion(void)
{
	pPlayerModel->SetEndMotion();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プレイヤーモデルの終了モーション後のモーションを再生
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::SetPlayerEndMotionNext(void)
{
	pPlayerModel->SetEndMotionNext();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタン押し確認
 *
 * @param		none
 *
 * @return	押された時true それ以外false
 */
//--------------------------------------------------------------------------------------------
bool View::UpdateUI(void)
{
  if( m_button->IsTrigger( gfl2::ui::BUTTON_A ) ) {
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パーティクル作成
 *
 * @param		af		アークファイル
 * @param		heap	ヒープ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::CreateParticle( app::util::Heap * heap )
{
  u32 effectGroupIDNumMaxArray[1] = { 1 };
  app::util::EffectUtil::Description desc;
  {
    desc.layerNum = 1;
    desc.effectResNumMax = 1;
    desc.mode = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
    desc.effectGroupIDNumMaxArray = effectGroupIDNumMaxArray;
  }

  pEffectUtil = GFL_NEW(heap->GetSystemHeap()) app::util::EffectUtil(heap->GetDeviceHeap(), desc);

  pEffectUtil->EffectSystemEntryResource( pFileLoader->GetParticleFileBuffer(), 0 );

  pEffectUtil->GetData(0)->effectGroupIDList->PushFront( 0 );

  {
    pRenderingManager->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pEffectUtil->GetData(0)->effectRenderPathConfig);
    pRenderingManager->SetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pCamera);
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パーティクル削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void View::DeleteParticle(void)
{
  pEffectUtil->KillAllEmitterSetForMode1Res1EmitterSet1Group();
  pEffectUtil->EffectSystemClearResource(0);
  GFL_SAFE_DELETE(pEffectUtil);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パーティクル開始
 *
 */
//--------------------------------------------------------------------------------------------
void View::StartParticle(void)
{
  pEffectUtil->CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();
  gfl2::Effect::Handle* effectHandle = pEffectUtil->GetEffectHandleForMode1Res1EmitterSet1Group(0);
  nw::eft::EmitterSet* emitterSet = effectHandle->GetEmitterSet();
  if( emitterSet )
  {
    gfl2::math::Vector3 pos(0.0f, 0.0f, 0.0f);
    gfl2::Effect::EmitterSet::SetGFPos(emitterSet, pos);
  }
}
//--------------------------------------------------------------------------------------------
/**
* @brief		ポケモンモデルモーション開始
*/
//--------------------------------------------------------------------------------------------
void View::StartPokeMotion(u32 id)
{
  pPokeModel->StartMotionB(id);
}
//--------------------------------------------------------------------------------------------
/**
* @brief		スポットライトモデルセットアップ
*/
//--------------------------------------------------------------------------------------------
void View::SetupSpot( u32 id, u32 pos )
{
  pPokeModel->SetupSpot(id,pos);
}
//--------------------------------------------------------------------------------------------
/**
* @brief		スポットライトモデルの切り替え
*/
//--------------------------------------------------------------------------------------------
void View::ChangeSpot( void )
{
  pPokeModel->ChangeSpot();
}
//--------------------------------------------------------------------------------------------
/**
* @brief		スポットライトモデルの切り替え予約
*/
//--------------------------------------------------------------------------------------------
void View::ReserveChangeSpot( u32 pos, u32 previd, u32 nextid )
{
  pPokeModel->ReserveChangeSpot( pos, previd, nextid );
}
}	// dendou_demo
}	// app
