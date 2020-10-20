//============================================================================================
/**
 * @file		dendou_demo_PokeModel.cpp
 * @brief		殿堂入りデモ ポケモンモデル
 * @author  nagihashi_toru(移植), Hiroyuki Nakamura
 * @date		16.02.29
 */
//============================================================================================
#include "./DendouDemoPokeModel.h"

#include <PokeTool/include/PokeModelSystem.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <model/include/gfl2_BaseModel.h>
#include <util/include/gfl2_std_string.h>
#include <System/include/GflUse.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::dendou_demo
 * @brief			殿堂入りデモ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace dendou_demo {

//! モデル座標データ
static const gfl2::math::Vector3 Scene1_PosData[] = {
	gfl2::math::Vector3(    0.0f, 0.5f,   0.0f-180.f ),//-180は過去作より0,0,0中心で回転するため（過去作は0,0,180で回転していた)
	gfl2::math::Vector3(  180.0f, 0.5f, 180.0f-180.f ),
	gfl2::math::Vector3(    0.0f, 0.5f, 360.0f-180.f ),
	gfl2::math::Vector3( -180.0f, 0.5f, 180.0f-180.f ),
	gfl2::math::Vector3(    0.0f, 0.5f, 360.0f-180.f ),
	gfl2::math::Vector3(  180.0f, 0.5f, 180.0f-180.f ),
	// ここから登録数３匹の時の回転シーンのデータ
	gfl2::math::Vector3(  -90.0f, 0.5f,  27.0f-180.f ),
	gfl2::math::Vector3(  -90.0f, 0.5f, 333.0f-180.f ),
};

//! 回転データ
static const gfl2::math::Vector3 Scene1_RotateData[] = {
	gfl2::math::Vector3( 0.0f,  16.0f, 0.0f ),
	gfl2::math::Vector3( 0.0f, -16.0f, 0.0f ),
	gfl2::math::Vector3( 0.0f,  16.0f, 0.0f ),
	gfl2::math::Vector3( 0.0f, -16.0f, 0.0f ),
	gfl2::math::Vector3( 0.0f,  16.0f, 0.0f ),
	gfl2::math::Vector3( 0.0f, -16.0f, 0.0f ),
	// ここから登録数３匹の時の回転シーンのデータ
	gfl2::math::Vector3( 0.0f,   0.0f, 0.0f ),
	gfl2::math::Vector3( 0.0f,   0.0f, 0.0f ),
};

//! 位置ごとの回転角度
static const f32 PosRotateY[] = {
	0.0f, -90.0f, -180.0f, -270.0f, -180.0f, -90.0f,
	// ここから登録数３匹の時の回転シーンのデータ
	30.0f, 150.0f,
};

static const f32 OFFS_MIN_PZ = -120.0f;			//!< 最小サイズのZ座標オフセット値（バチュル）
static const f32 OFFS_MAX_PZ = -190.0f;		//!< 最大サイズのZ座標オフセット値（ルギア）


//------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		description 設定
 */
//------------------------------------------------------------------------------------------
PokeModel::PokeModel( const Description & description ) :
	pHeap( description.heap ),
  renderingManager( description.renderingManager ),
  asyncFileManager( description.asyncFileManager ),
  minBaseHeight(0),
  maxBaseHeight(0),
  minBaseLength(0),
  maxBaseLength(0),
  baseLength(0),
  initialze( 0 ),
  finalize( 0 ),
  spotInvisibleId(SPOT_INVALID),
  spotVisibleId(SPOT_INVALID),
  spotPos(SPOT_INVALID),
  pCamera(NULL)
{
	pPokeModelSystem = GFL_NEW( pHeap->GetSystemHeap() ) PokeTool::PokeModelSystem();

  PokeTool::PokeModel::SetupOption option;
  option.dataHeap = pHeap->GetDeviceHeap();
  option.workHeap = pHeap->GetDeviceHeap()->GetLowerHandle();
  option.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
  option.useShadow = false;
  option.useIdModel = false;

  app::tool::PokeSimpleModel::INIT_PARAM initParam;
  initParam.pPokeModelSys = pPokeModelSystem;
  initParam.pRenderingManager = renderingManager;
  initParam.layerNo = 2;
  initParam.renderPlace = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
  initParam.setupOpt = option;

	gfl2::std::MemClear( modelData, sizeof(MODEL_DATA)*MODEL_MAX );
	for( int i=0; i<MODEL_MAX; i++ ){
    modelData[i].model = GFL_NEW( pHeap->GetSystemHeap() ) app::tool::PokeSimpleModel( initParam );
		modelData[i].id = i;
    modelData[i].spotModelFileBuffer = description.spotModelFileBuffer[i];
    CreateSpotModel( &modelData[i] );
	}
}

//------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//------------------------------------------------------------------------------------------
PokeModel::~PokeModel()
{
	for( int i=0; i<MODEL_MAX; i++ ){
    DeleteSpotModel( &modelData[i] );
    GFL_SAFE_DELETE( modelData[i].model );
  }
  GFL_SAFE_DELETE( pPokeModelSystem );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		アップデート
 *
 * @param		none
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::Update(void)
{
	for( int i=0; i<MODEL_MAX; i++ ){
		UpdateCore( &modelData[i] );
	}

  SortPokeLayerId();
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデル別アップデート
 *
 * @param		dat		モデルデータ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::UpdateCore( MODEL_DATA * dat )
{
	switch( dat->seq ){
	case SEQ_WAIT:
    if( dat->createFlag )
    {
      if( dat->model->IsEndAnime() )
      {
        if( dat->isWaitB )
        {
          this->SetAnime( dat, PokeTool::MODEL_ANIME_BT_WAIT_A );
          GFL_PRINT("Anime[%d] force WaitA\n", dat->id);
        }
        else
        {
          s16 waitBrate = dat->model->GetSettingData()->waitBRate;
          s32 rand = System::GflUse::GetPublicRand(1000);
          if( rand < waitBrate &&
            dat->model->IsAvailableAnimation(PokeTool::MODEL_ANIME_BT_WAIT_B) )
          {
            this->SetAnime( dat, PokeTool::MODEL_ANIME_BT_WAIT_B );
            GFL_PRINT("Anime[%d] WaitB\n", dat->id);
          }
          else
          {
            this->SetAnime( dat, PokeTool::MODEL_ANIME_BT_WAIT_A );
            GFL_PRINT("Anime[%d] WaitA\n", dat->id);
          }
        }
      }
    }
		break;

	case SEQ_CREATE:
		if( Create( dat ) == true ){
			dat->seq = SEQ_SETTING;
		}
		break;

	case SEQ_SETTING:
		if( Setting( dat ) == true ){
			dat->createFlag = true;
			dat->seq = SEQ_WAIT;
		}
		break;
	}
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデルシステムのアップデート
 *
 * @param		none
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::UpdateSystem(void)
{
  pPokeModelSystem->Update();

	for( int i=0; i<MODEL_MAX; i++ ){
    if( modelData[i].model )
    {
      modelData[i].model->Update();
    }

    if(modelData[i].spotModel)
    {
      modelData[i].spotModel->UpdateAnimation();
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデル作成リクエスト
 *
 * @param	  id		データID
 * @param		sp		ポケモンデータ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::Create( u32 id, const PokeTool::SimpleParam & sp )
{
	MODEL_DATA * dat = &modelData[id];
	dat->simpleParam = sp;
	dat->createFlag  = false;
	dat->seq         = SEQ_CREATE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデル作成
 *
 * @param	  dat		モデルデータ
 *
 * @retval	"true = 作成完了"
 * @retval	"false = それ以外"
 */
//------------------------------------------------------------------------------------------
bool PokeModel::Create( MODEL_DATA * dat )
{
  dat->model->StartLoad(dat->simpleParam);

	return true;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデルの読み込みを待って各データをセット
 *
 * @param		dat		モデルデータ
 *
 * @retval	"true = セット完了"
 * @retval	"false = それ以外"
 */
//------------------------------------------------------------------------------------------
bool PokeModel::Setting( MODEL_DATA * dat )
{
  if( dat->model->IsReady() == false ) {
		return false;
	}

	SetModelPosition( dat );
  this->SetAnime( dat, PokeTool::MODEL_ANIME_BT_WAIT_A);
  dat->model->SetStencilReferenceOffset( dat->id );
	const f32 MODEL_COMMON_SCALE = 1.0f;
	dat->model->SetScale( MODEL_COMMON_SCALE );
	dat->model->SetVisible( dat->visibleFlag );

	return true;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデル座標設定
 *
 * @param		dat		モデルデータ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::SetModelPosition( MODEL_DATA * dat )
{
  gfl2::math::Vector3 pos = CalcPosition( dat );
	dat->model->SetPos(pos);

	dat->model->SetRotate(
		gfl2::math::ConvDegToRad( dat->rotate.x ),
		gfl2::math::ConvDegToRad( dat->rotate.y ),
		gfl2::math::ConvDegToRad( dat->rotate.z ) );
}
//------------------------------------------------------------------------------------------
/**
 * @brief		スポットライト座標設定
 *
 * @param		dat		モデルデータ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::SetSpotPosition( MODEL_DATA * dat, u32 pos )
{
  //早めにモデル用のデータを更新してしまうが、先読みなので問題なし
	const gfl2::math::Vector3 pos_tbl = Scene1_PosData[pos];
	const gfl2::math::Vector3 rot_tbl = Scene1_RotateData[pos];
	const f32 pos_rot_y_tbl = PosRotateY[pos];

	dat->pos = pos_tbl;

	dat->rotate.x = 0.0f;
	dat->rotate.y = pos_rot_y_tbl;
	dat->rotate.z = 0.0f;
	dat->posRotate = pos_rot_y_tbl;

/*
	const gfl2::math::Vector3 pos_tbl = Scene1_PosData[pos];
	const gfl2::math::Vector3 rot_tbl = Scene1_RotateData[pos];
	const f32 pos_rot_y_tbl = PosRotateY[pos];

	modelData[id].pos = pos_tbl;

	modelData[id].rotate.x = 0.0f;
	modelData[id].rotate.y = pos_rot_y_tbl;
	modelData[id].rotate.z = 0.0f;

*/

  gfl2::math::Vector3 spotpos = CalcPosition(dat);
  spotpos.y = 0.0f;
  dat->spotModel->SetPosition(spotpos);
}
//------------------------------------------------------------------------------------------
/**
 * @brief		モデル座標計算
 *
 * @param		dat		モデルデータ
 *
 * @return	none　計算
 */
//------------------------------------------------------------------------------------------
gfl2::math::Vector3 PokeModel::CalcPosition( MODEL_DATA * dat )
{
	f32 height = GetHeight( dat );
	f32	offs_z;

	if( height == minBaseHeight ){
		offs_z = OFFS_MIN_PZ;
	}else if( height == maxBaseHeight ){
		offs_z = OFFS_MAX_PZ;
	}else{
		offs_z = OFFS_MIN_PZ + minBaseLength + baseLength / (maxBaseHeight-minBaseHeight) * (height-minBaseHeight);
		offs_z -= GetLength( dat );
	}

	f32	px = 0.0f;
	f32	pz = offs_z;
	GetXZRotateY( &px, &pz, gfl2::math::ConvDegToRad(dat->posRotate) );

  NAGI_PRINT( "px%f, pz%f\n", px, pz );

  return gfl2::math::Vector3( dat->pos.x+px, dat->pos.y, dat->pos.z+pz );
}
//------------------------------------------------------------------------------------------
/**
 * @brief		表示切り替え
 *
 * @param		id		データID
 * @param		flg		true = 表示, false = 非表示
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::SetVisible( u32 id, bool flg )
{
	if( modelData[id].model != NULL ){
		modelData[id].model->SetVisible( flg );
	}
	modelData[id].visibleFlag = flg;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		表示取得
 *
 * @param		id		データID
 *
 * @retval	"true = 表示中"
 * @retval	"false = それ以外"
 */
//------------------------------------------------------------------------------------------
bool PokeModel::GetVisible( u32 id ) const
{
	return modelData[id].visibleFlag;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデルの作成が完了したか
 *
 * @param		none
 *
 * @retval	"true = 完了"
 * @retval	"false = それ以外"
 */
//------------------------------------------------------------------------------------------
bool PokeModel::CheckCreate( u32 id ) const
{
	return modelData[id].createFlag;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデル座標初期化
 *
 * @param		max		モデル登録数
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::InitPosition( u32 max )
{
	const gfl2::math::Vector3 * pos_tbl = &Scene1_PosData[pml::PokeParty::MAX_MEMBERS-max];
	const gfl2::math::Vector3 * rot_tbl = &Scene1_RotateData[pml::PokeParty::MAX_MEMBERS-max];
	const f32 * pos_rot_y_tbl = &PosRotateY[pml::PokeParty::MAX_MEMBERS-max];

	for( u32 i=0; i<max; i++ ){
		modelData[i].pos = pos_tbl[i];

		modelData[i].rotate.x = rot_tbl[i].x;
		modelData[i].rotate.y = rot_tbl[i].y + pos_rot_y_tbl[i];
		modelData[i].rotate.z = rot_tbl[i].z;

		modelData[i].posRotate = pos_rot_y_tbl[i];
	}
}

//------------------------------------------------------------------------------------------
/**
 * @brief		モデル座標設定
 *
 * @param		id		データID
 * @param		pos		配置位置
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void PokeModel::SetPosition( u32 id, u32 pos )
{
	const gfl2::math::Vector3 pos_tbl = Scene1_PosData[pos];
	const gfl2::math::Vector3 rot_tbl = Scene1_RotateData[pos];
	const f32 pos_rot_y_tbl = PosRotateY[pos];

	modelData[id].pos = pos_tbl;

	modelData[id].rotate.x = 0.0f;
	modelData[id].rotate.y = pos_rot_y_tbl;
	modelData[id].rotate.z = 0.0f;

	modelData[id].posRotate = pos_rot_y_tbl;

	SetModelPosition( &modelData[id] );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		高さ取得
 *
 * @param		dat		モデルデータ
 *
 * @return	モデルの高さ
 */
//------------------------------------------------------------------------------------------
f32 PokeModel::GetHeight( MODEL_DATA * dat )
{
	gfl2::math::AABB box;
	bool ret = dat->model->GetMotionCollision( &box );
  GFL_ASSERT( ret );
	const gfl2::math::Vector4 & max_v4 = box.GetMax();
	const gfl2::math::Vector4 & min_v4 = box.GetMin();
	return ( max_v4.y + min_v4.y );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		前後の幅取得
 *
 * @param		dat		モデルデータ
 *
 * @return	モデルの前後の幅
 */
//------------------------------------------------------------------------------------------
f32 PokeModel::GetLength( MODEL_DATA * dat )
{
	gfl2::math::AABB box;
	bool ret = dat->model->GetMotionCollision( &box );
  GFL_ASSERT( ret );
	const gfl2::math::Vector4 & max_v4 = box.GetMax();
	const gfl2::math::Vector4 & min_v4 = box.GetMin();
//	return ( max_v4.z - min_v4.z );
	return max_v4.z;
}

//------------------------------------------------------------------------------------------
/**
* @brief		Y回転時のXYを取得する
*
* @param		xtang[in/out]		X
* @param		ztang[in/out]		Y
* @param		yang		        ラジアン
*/
//------------------------------------------------------------------------------------------
void PokeModel::GetXZRotateY( f32* xtag, f32* ztag, f32 yang )
{
  f32 x = *xtag;
  f32 z = *ztag;

  f32 s, c;
  gfl2::math::SinCosRad( &s, &c, yang );

  //*xpos = GetX_RotateY_SinCosXZ( s, c, x, z );
  *xtag = c * x + s * z;
  //*zpos = GetZ_RotateY_SinCosXZ( s, c, x, z );
  *ztag = c * z - s * x;
}

//------------------------------------------------------------------------------------------
/**
* @brief		基準データ作成
*
* @param		min_index		最小サイズのモデルインデックス
* @param		min_index		最大サイズのモデルインデックス
*
* @return	none
*/
//------------------------------------------------------------------------------------------
void PokeModel::InitBaseData( u32 min_index, u32 max_index )
{
  gfl2::math::AABB box;
  {
    bool ret = modelData[min_index].model->GetMotionCollision( &box );
    GFL_ASSERT( ret );
    const gfl2::math::Vector4 & max_v4 = box.GetMax();
    const gfl2::math::Vector4 & min_v4 = box.GetMin();
    minBaseHeight = max_v4.y + min_v4.y;
    minBaseLength = max_v4.z;
  }
  {
    bool ret = modelData[max_index].model->GetMotionCollision( &box );
    GFL_ASSERT( ret );
    const gfl2::math::Vector4 & max_v4 = box.GetMax();
    const gfl2::math::Vector4 & min_v4 = box.GetMin();
    maxBaseHeight = max_v4.y + min_v4.y;
    maxBaseLength = max_v4.z;
  }

  baseLength = ( OFFS_MAX_PZ + maxBaseLength ) - ( OFFS_MIN_PZ + minBaseLength );
}

//------------------------------------------------------------------------------------------
/**
* @brief		モデル座標取得
*
* @param		id	モデルID
*
* @return	座標
*/
//------------------------------------------------------------------------------------------
gfl2::math::Vector3 PokeModel::GetPosition( u32 id )
{
  gfl2::math::AABB box;
  bool ret = modelData[id].model->GetMotionCollision( &box );
  GFL_ASSERT( ret );
  const gfl2::math::Vector4 & max_v4 = box.GetMax();
  const gfl2::math::Vector4 & min_v4 = box.GetMin();
  gfl2::math::Vector3 pos = modelData[id].model->GetPos();
  pos.y += ( (max_v4.y-min_v4.y) / 2.0f );

  return pos;
}


//------------------------------------------------------------------------------------------
/**
* @brief		システム初期化待ち
*/
//------------------------------------------------------------------------------------------
bool PokeModel::Initialize()
{
  switch( initialze )
  {
  case 0:
    pPokeModelSystem->InitSystemAsync( 
      pHeap->GetDeviceHeap(),
      pHeap->GetDeviceHeap(),
      asyncFileManager,
      MODEL_MAX );
    initialze++;
    //fall
  case 1:
    if( !pPokeModelSystem->IsFinishInitSystemAsync() )
    {
      return false;
    } else {

      pPokeModelSystem->CreateSystem(pHeap->GetDevGLHeapAllocator(),false,false);
      {
        PokeTool::PokeModelSystem::HeapOption opt;
        opt.animeType       = PokeTool::MODEL_ANIMETYPE_BATTLE;
        opt.useIdModel      = false;
        opt.useShadow       = false;
        opt.useColorShader  = false;
        pPokeModelSystem->CreatePokeModelHeap( pHeap->GetDeviceHeap(), &opt );
        initialze++;
      }
    }
    //fall
  case 2:
    initialze = 0;
    return true;
  default:
    GFL_ASSERT(0);
    return true;
  }
}
//------------------------------------------------------------------------------------------
/**
* @brief		システム破棄待ち
*/
//------------------------------------------------------------------------------------------
bool PokeModel::Finalize()
{
  switch( finalize )
  {
  case 0:
    {
      for( int i=0; i<MODEL_MAX; i++ ){
        modelData[i].model->Terminate();
      }
      finalize++;
    }
    //fall
  case 1:
    {
      bool isAllEnd = true;
      for( int i=0; i<MODEL_MAX; i++ ){
        isAllEnd &= modelData[i].model->IsDelete();
      }
      if( !isAllEnd )
      {
        return false;
      }
      finalize++;
    }
    //fall
  case 2:
    pPokeModelSystem->DeletePokeModelHeap();
    pPokeModelSystem->TermSystemAsync();
    finalize++;
    //fall
  case 3:
    if( pPokeModelSystem->IsFinishTermSystemAsync() )
    {
      finalize = 0;
      return true;
    }
    else
    {
      return false;
    }
  default:
    GFL_ASSERT(0);
    return true;
  }
}

//------------------------------------------------------------------------------------------
/**
* @brief		待機モーションBをする
*/
//------------------------------------------------------------------------------------------
void PokeModel::StartMotionB(u32 id)
{
  //デバッグ中なので、変更を抑えるために空関数にして使用箇所を消さない
}

//--------------------------------------------------------------------------------------------
/**
* @brief		スポットライトモデルセットアップ
*/
//--------------------------------------------------------------------------------------------
void PokeModel::SetupSpot( u32 id, u32 pos )
{
  SetSpotPosition( &modelData[id], pos );
  modelData[id].spotModel->SetVisible( true );
}
//--------------------------------------------------------------------------------------------
/**
* @brief		スポットライトモデルの切り替え
*/
//--------------------------------------------------------------------------------------------
void PokeModel::ChangeSpot( void )
{
  if( spotInvisibleId == SPOT_INVALID )
  {
    return;
  }

  modelData[spotInvisibleId].spotModel->SetVisible( false );
  modelData[spotVisibleId].spotModel->SetVisible( true );
  SetSpotPosition( &modelData[spotVisibleId], spotPos );
}
//--------------------------------------------------------------------------------------------
/**
* @brief		スポットライトモデルの切り替え予約
*/
//--------------------------------------------------------------------------------------------
void PokeModel::ReserveChangeSpot( u32 pos, u32 previd, u32 nextid )
{
  spotInvisibleId = previd;
  spotVisibleId = nextid;
  spotPos = pos;
}

//--------------------------------------------------------------------------------------------
/**
* @brief		カメラセット
*/
//--------------------------------------------------------------------------------------------
void PokeModel::SetCamera( poke_3d::model::BaseCamera* camera )
{
  pCamera = camera;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		Spotモデル作成
 *
 * @param		dat		モデルデータ
 */
//------------------------------------------------------------------------------------------
void PokeModel::CreateSpotModel( MODEL_DATA * dat )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
  modelDataPack.SetModelData(static_cast<c8*>(dat->spotModelFileBuffer));
  dat->spotMdlNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(pHeap->GetDevGLHeapAllocator(), &modelDataPack);

  dat->spotModel = GFL_NEW(pHeap->GetSystemHeap()) poke_3d::model::BaseModel;
  dat->spotModel->Create(pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), dat->spotMdlNode);
  renderingManager->AddModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, dat->spotModel, 1, false );
  dat->spotModel->SetVisible(false);
}
//------------------------------------------------------------------------------------------
/**
 * @brief		Spotモデル破棄
 *
 * @param		dat		モデルデータ
 */
//------------------------------------------------------------------------------------------
void PokeModel::DeleteSpotModel( MODEL_DATA * dat )
{
  renderingManager->RemoveModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, dat->spotModel, 1 );
  dat->spotModel->Destroy();
  GFL_SAFE_DELETE( dat->spotModel );
  GFL_SAFE_DELETE( dat->spotMdlNode );
}	
//------------------------------------------------------------------------------------------
/**
 * @brief		アニメを再生する
 *
 * @param		dat		モデルデータ
 * @param		animeId アニメID
 */
//------------------------------------------------------------------------------------------
void PokeModel::SetAnime( MODEL_DATA * dat, PokeTool::MODEL_ANIME animeId )
{
  dat->model->SetAnime( animeId, false, true );
  dat->isWaitB = (PokeTool::MODEL_ANIME_BT_WAIT_B == animeId);
}
//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンソート
 */
//------------------------------------------------------------------------------------------
void PokeModel::SortPokeLayerId()
{
  if( pCamera == NULL ) { return; }

  MODEL_DATA *pokeArr[MODEL_MAX];
  f32 lenArr[MODEL_MAX];

  gfl2::math::Vector3 camPos = pCamera->GetPosition();

  //距離計測
  for(int i=0;i<MODEL_MAX;i++ )
  {
    pokeArr[i] = &this->modelData[i];
    if( pokeArr[i]->createFlag )
    {
      gfl2::math::Vector3 pos = pokeArr[i]->model->GetPos();

      pos -= camPos;

      lenArr[i] = pos.LengthSq();
    }
    else
    {
      lenArr[i]  = 0;
    }
  }

  //ソート
  for( int i=0;i<MODEL_MAX-1;i++ )
  {
    for( int ii=i;ii<MODEL_MAX;ii++ )
    {
      if( lenArr[i] < lenArr[ii] )
      {
        f32 temp = lenArr[i];
        lenArr[i] = lenArr[ii];
        lenArr[ii] = temp;

        MODEL_DATA *tempIdx = pokeArr[i];
        pokeArr[i] = pokeArr[ii];
        pokeArr[ii] = tempIdx;
      }
    }
  }

  //ID指定
  static const u8 defaultPriority = gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::s_DefaultPriority;
  for(int i=0;i<MODEL_MAX;i++ )
  {
    if( pokeArr[i]->createFlag )
    {
      pokeArr[i]->model->SetSortPriority(defaultPriority + i + 1);
    }
  }

}

}	// dendou_demo
}	// app
