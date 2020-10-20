//============================================================================================
/**
 * @file    app_util_2d.cpp
 * @brief   2D関連
 * @author  Hiroyuki Nakamura
 * @date    11.10.04
 */
//============================================================================================

// module
#include "AppLib/include/Util/app_util_2d.h"
// gflib2
#include <Layout/include/gfl2_LayoutRenderPath.h>
#include <math/include/gfl2_MathTriangular.h>
// niji
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Print/include/SystemFont.h"
#include "GameSys/include/GameManager.h"


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::util
 * @brief			アプリケーションユーティリティ処理
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace util {

//! レイアウトセットアップデータ
const gfl2::lyt::LytWkSetUp G2DUtil::LytWkDefaultSetup[] =
{
  { // 上画面
    gfl2::math::Vector3(0,0,0),   // pos
    gfl2::math::Vector2(1,1),     // scale
    gfl2::math::Vector3(0,0,0),   // rotate
    1,                        // priority
    0,												// groupID
  },
  { // 下画面
    gfl2::math::Vector3(0,-290,0),  // pos
    gfl2::math::Vector2(1,1),       // scale
    gfl2::math::Vector3(0,0,0),     // rotate
    1,                          // priority
    0,												// groupID
  }
};


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------------------
G2DUtil::G2DUtil( Heap * heap ) :
  pLayoutSystem( NULL ),
  pUpperLayoutEnv( NULL ),
  pLowerLayoutEnv( NULL ),
  lwkMax( 0 ),            // レイアウトワーク管理データ数
  resIdMax( 0 ),          // リソースID最大数
  pMsgWin( NULL ),
  pMsgCursor( NULL ),
  m_bIsSetLayoutResourceID(false)
{
  CreateStrBuf( heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
G2DUtil::~G2DUtil()
{
  DeleteStrBuf();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アップデート
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::Update2D(void)
{
  for( u32 i=0; i<lwkMax; i++ ){
    if( pLayoutWork[i].pLytWk != NULL ){
      pLayoutWork[i].pLytWk->Update();
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク描画
 *
 * @param   AppRM       描画マネージャ
 * @param   displayNo   描画面
 * @param   id          描画するレイアウトのID
 * @param   order       描画順(デフォルト：ORDER_NORMAL(128))
 * @param   layerNo     レイヤーNo
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::AddDrawableLytWkForOneFrame( app::util::AppRenderingManager* AppRM, gfl2::gfx::CtrDisplayNo displayNo, u32 id, u8 order, u32 layerNo )
{
  if( !pLayoutWork || pLayoutWork[id].enable == false )
  {
    return;
  }

  if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
  {
    if( pLayoutWork[id].setupDisp == SETUP_UPPER )
    {
      AppRM->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pLayoutWork[id].pLytWk, layerNo, order );
    }
  }
  else if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    if( pLayoutWork[id].setupDisp == SETUP_LOWER )
    {
      AppRM->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, pLayoutWork[id].pLytWk, layerNo, order );
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトシステム作成
 *
 * @param   heap    ヒープ
 * @param   resMax  管理リソース最大数
 * @param   wkMax   管理レイアウトワーク最大数
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateLayoutSystem( Heap * heap, u32 resMax, u32 wkMax )
{
  gfl2::lyt::LytSysSetUp setup = {
    GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
    resMax,     // 管理リソース最大数
    wkMax       // 管理レイアウトワーク最大数
  };
  pLayoutSystem = GFL_NEW( heap->GetDeviceHeap() )  gfl2::lyt::LytSys( &setup, heap->GetSystemAllocator(), heap->GetDeviceAllocator(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトシステム削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteLayoutSystem(void)
{
  if( pLayoutSystem != NULL )
  {
    GFL_DELETE  pLayoutSystem;
    pLayoutSystem = NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  レイアウトシステムを設定（外部レイアウトシステムを使用する）
 *
 * @param   lytSys 外部レイアウトシステムへのポインタ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetLayoutSystem( gfl2::lyt::LytSys* lytSys )
{
  pLayoutSystem = lytSys;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトシステム取得
 *
 * @param   none
 *
 * @return  レイアウトシステム
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytSys * G2DUtil::GetLayoutSystem(void)
{
  return pLayoutSystem;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウト環境作成
 *
 * @param   heap					ヒープ
 * @param   isStereoView	3D表示を有効にするか
 * @param		projection		プロジェクションパラメータ
 * @param		view					ビューパラメータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateLayoutEnv(
			Heap * heap, bool isStereoView,
			const gfl2::lyt::ProjectionParam * projection,
			const gfl2::lyt::ViewParam * view )
{
	const gfl2::lyt::ProjectionParam * proj_param;
	const gfl2::lyt::ViewParam * view_param;

	if( projection == NULL ){
		// 射影設定（正射影）
		static const gfl2::lyt::ProjectionParam def_proj[] =
		{
			{ // 上画面
				gfl2::lyt::PROJECTION_ORTHO,
				{ -200.0f, 200.0f, -120.0f, 120.0f },
				1.0f,
				1000.0f,
				gfl2::math::PIVOT_UPSIDE_TO_TOP,
			},
			{ // 下画面
				gfl2::lyt::PROJECTION_ORTHO,
				{ -160.0f, 160.0f, -120.0f, 120.0f },
				1.0f,
				1000.0f,
				gfl2::math::PIVOT_UPSIDE_TO_TOP,
			}
		};
		proj_param = def_proj;
	}else{
		proj_param = projection;
	}

	if( view == NULL ){
		// ビュー設定
		static const gfl2::lyt::ViewParam def_view[] =
		{
			{ // 上画面（左目）
				gfl2::math::Vector3(0,0,1),   // カメラ座標
				gfl2::math::Vector3(0,1,0),   // 上方向
				gfl2::math::Vector3(0,0,0),   // ターゲット座標
			},
			{ // 上画面（右目）
				gfl2::math::Vector3(10,0,1),  // カメラ座標
				gfl2::math::Vector3(0,1,0),   // 上方向
				gfl2::math::Vector3(10,0,0),  // ターゲット座標
			},
			{ // 下画面
				gfl2::math::Vector3(0,-290,1),  // カメラ座標
				gfl2::math::Vector3(0,1,0),     // 上方向
				gfl2::math::Vector3(0,-290,0),  // ターゲット座標
			},
		};
		view_param = def_view;
	}else{
		view_param = view;
	}

	/* 立体視の場合、同じカメラから左右用の行列を生成する必要がある。
	   時期を考えて最小限の修正のみ。
		 ここを有効にし、viewの要素数を２に、isStereoViewはいらない。
		 2013/02/22 by nakahiro */
#if 0
  // 上画面
  pUpperLayoutEnv = GFL_NEW( heap->pHeap )  gfl2::lyt::Env();
  pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, proj_param[0], view_param[0] );
  pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, proj_param[0], view_param[0] );
  // 下画面
  pLowerLayoutEnv = GFL_NEW( heap->pHeap )  gfl2::lyt::Env();
  pLowerLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, proj_param[1], view_param[1] );
  pLowerLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, proj_param[1], view_param[1] );
#else
  // 上画面
  pUpperLayoutEnv = GFL_NEW( heap->GetDeviceHeap() )  gfl2::lyt::Env();
  if( isStereoView )
  {
    pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, proj_param[0], view_param[0] );
    pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, proj_param[0], view_param[1] );
  }
  else
  {
    pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, proj_param[0], view_param[0] );
    pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, proj_param[0], view_param[0] );
  }

  // 下画面
  pLowerLayoutEnv = GFL_NEW( heap->GetDeviceHeap() )  gfl2::lyt::Env();
  pLowerLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, proj_param[1], view_param[2] );
  pLowerLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, proj_param[1], view_param[2] );
#endif

  // Z座標による表示プライオリティを有効にするための設定
/*  アルファ処理で不具合が出るため、使用禁止にする
  if( z_priority == true ){
    gfl::grp::FragmentOperationParam param = gfl::grp::FragmentOperation::DefaultParam;
    param.depth_test = true;
    param.depth_func = gfl::grp::TEST_FUNC_LEQUAL;
    param.frame_depth_read = true;
    param.frame_depth_write = true;
    pUpperLayoutEnv->GetFragmentOperationPtr()->SetParam( param );
    pLowerLayoutEnv->GetFragmentOperationPtr()->SetParam( param );
  }
*/
}

void G2DUtil::CreateLayoutEnvPerspective( Heap * heap, bool isStereoView )
{
  // 射影設定
  static const gfl2::lyt::ProjectionParam projection[] =
  {
    { // 上画面
      gfl2::lyt::PROJECTION_PERSPECTIVE,
      {
        gfl2::math::AtanRad( 120.0f / 240.0f ) * 2,   // z値0が等倍で表示されるように設定
        400.0f / 240.0f
      },
      0.10f,
      480.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP,
    },
    { // 下画面
      gfl2::lyt::PROJECTION_PERSPECTIVE,
      {
        gfl2::math::AtanRad( 120.0f / 240.0f ) * 2,   // z値0が等倍で表示されるように設定
        320.0f / 240.0f
      },
      0.10f,
      480.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP,
    }
  };

  // ビュー設定
  static const gfl2::lyt::ViewParam view[] =
  {
    { // 上画面（左目）
      gfl2::math::Vector3(0,0,240),   // カメラ座標
      gfl2::math::Vector3(0,1,0),   // 上方向
      gfl2::math::Vector3(0,0,0),   // ターゲット座標
    },
    { // 上画面（右目）
      gfl2::math::Vector3(10,0,240),  // カメラ座標
      gfl2::math::Vector3(0,1,0),   // 上方向
      gfl2::math::Vector3(10,0,0),  // ターゲット座標
    },
    { // 下画面
      gfl2::math::Vector3(0,-290,240),  // カメラ座標
      gfl2::math::Vector3(0,1,0),     // 上方向
      gfl2::math::Vector3(0,-290,0),  // ターゲット座標
    },
  };

	CreateLayoutEnv( heap, isStereoView, projection, view );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウト環境削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteLayoutEnv(void)
{
  if( pUpperLayoutEnv != NULL )
  {
    GFL_DELETE  pUpperLayoutEnv;
    pUpperLayoutEnv = NULL;
  }
  if( pLowerLayoutEnv != NULL )
  {
    GFL_DELETE  pLowerLayoutEnv;
    pLowerLayoutEnv = NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク作成（リソースのバイナリの読み込みは外部で済んでいるとき）
 *
 * @param   resTbl        レイアウトリソースデータ
 * @param   tblMax        上記配列の数
 * @param   lytMax        作成数
 * @param   heap          ヒープ
 * @param		area			    転送エリア
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateLayoutWork( const LytResData * resTbl, u32 tblMax, u32 lytMax, Heap * heap, gfl2::lyt::MemoryArea area )
{
  CreateLayoutResource( tblMax, heap );
  
  u32 resID = 0;
  for( u32 i=0; i<tblMax; i++ )
  {
    app::util::AppLytAccessor appLytAccessor;
    appLytAccessor.SetAppLytBuf(resTbl[i].buf);
    u32 nowResIdIdx = resID + i;
    LoadLayoutResourceBinary( nowResIdIdx, appLytAccessor.GetLytDat(), resTbl[i].addTexNum, heap );
  }

  TransferLayoutTexture();
  CreateLayoutWork( lytMax, heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク削除（リソース削除込み）
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteLayoutWork(void)
{
  DeleteLayoutWorkCore();
  DeleteLayoutResource();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク作成
 *
 * @param   lytMax    作成数
 * @param   heap      ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateLayoutWork( u32 lytMax, Heap * heap )
{
  pLayoutWork = GFL_NEW_ARRAY( heap->GetDeviceHeap() )  LAYOUT_WORK[lytMax];
  lwkMax = lytMax;

  for( u32 i=0; i<lwkMax; i++ ){
    pLayoutWork[i].pLytWk = NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteLayoutWorkCore(void)
{
  if( pLayoutWork != NULL ){
    for( u32 i=0; i<lwkMax; i++ ){
      if( pLayoutWork[i].pLytWk != NULL ){
        pLayoutSystem->RemoveLytWk( pLayoutWork[i].pLytWk );
      }
    }
    GFL_SAFE_DELETE_ARRAY( pLayoutWork );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトリソース作成
 *
 * @param   lytMax    作成数
 * @param   heap      ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateLayoutResource( u32 max, Heap * heap )
{
  pLayoutResID = GFL_NEW_ARRAY( heap->GetDeviceHeap() ) gfl2::lyt::LytMultiResID[max];
  resIdMax = max;

  // エラーコードで初期化
  for( u32 i=0; i<resIdMax; i++ ){
    pLayoutResID->Init();
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトリソース削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteLayoutResource(void)
{
  if( pLayoutResID != NULL ){
    for( u32 i=0; i<resIdMax; i++ ){
      if( pLayoutResID[i].GetMultiID() != gfl2::lyt::LYTSYS_RESID_ERROR ){
        pLayoutSystem->ReleaseLytRes( &pLayoutResID[i] );
      }
    }
    GFL_SAFE_DELETE_ARRAY( pLayoutResID );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief レイアウトリソース　バイナリ直接渡し版
 *
 * @param resID     リソースID
 * @param buff      アーカイブバイナリ
 * @param addTextureNum テクスチャ追加数
 * @param heap          ヒープ
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::LoadLayoutResourceBinary( u32 resID, const void *buff, u32 addTextureNum, Heap * heap )
{
  pLayoutResID[ resID ] = pLayoutSystem->InitMultiResource( heap->GetDeviceHeap(), 1 );
  pLayoutSystem->LoadLytRes( &pLayoutResID[resID], heap->GetDeviceAllocator(), buff, true, addTextureNum );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトリソース追加登録
 *
 * @param   srcID   追加するリソースID
 * @param   dstID   追加先のリソースID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::RegistLayoutResource( u32 srcID, u32 dstID )
{
  pLayoutSystem->RegistTextureRes( &pLayoutResID[srcID], &pLayoutResID[dstID] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SetLayoutWorkを呼び出す際に対象になるLytResDataを参照して連結する
 *          (内部的にはRegisterLayoutResourceをRES_COMMON定義している
 *           LytResDataに対して呼び出します）
 *
 * @param   resTbl        レイアウトリソースデータ
 * @param   tblMax        上記配列の数
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::AttachLayoutResourceReference( const LytResData * resTbl, u32 tblMax )
{
  int tempTbl[APP_UTIL_LYRES_MAX];
  int count=0;

  GFL_ASSERT( tblMax < APP_UTIL_LYRES_MAX ); // resTblが20以上指定されていた場合は @check
                                             // APP_UTIL_LYRES_MAXの定義を増やす
  // 連結先になるリソースを探す
  for(u32 i=0;i<tblMax;i++){
    if(resTbl[i].addRegisterFlag==G2DUtil::ATTACH_ROOT){
      tempTbl[count] = i;
      count++;
    }
  }

  for(u32 i=1;i<tblMax;i++){
    // 初期化時に参照してほしいフラグが立っていたら
    if( resTbl[i].addRegisterFlag==G2DUtil::ATTACH_RESOURCE ){
      // 連結先リソースに連結
      for(int j=0;j<count;j++){
        RegistLayoutResource( i, tempTbl[j] );
      }
    }
  }

}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースID取得
 *
 * @param   index		登録インデックス
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytMultiResID& G2DUtil::GetLayoutResourceID( u32 index )
{
  if( index >= resIdMax && m_bIsSetLayoutResourceID == false )
  {
    GFL_ASSERT( 0 );
    index = 0;
  }
  return pLayoutResID[index];
}
gfl2::lyt::LytMultiResID* G2DUtil::GetLayoutResourceID()
{
  return pLayoutResID;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースID設定
 *
 * @param   pLayoutResouceID		登録インデックス
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetLayoutResourceID( gfl2::lyt::LytMultiResID* pLayoutResouceID )
{
  pLayoutResID = pLayoutResouceID;
  m_bIsSetLayoutResourceID = true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テクスチャ転送
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::TransferLayoutTexture(void)
{
  for( u32 i=0; i<resIdMax; i++ )
  {
    if( pLayoutResID[i].GetParentID() != gfl2::lyt::LytMultiResID::NULL_ID )
    {
      pLayoutSystem->TransferAllTexture( &pLayoutResID[i] );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   外部レイアウトを設定
 *          （※解放しないように！）
 *
 * @param   lytMax  レイアウト最大数
 * @param   lytWork レイアウトワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetLayoutWork( LAYOUT_WORK *lytWork, const u32 lytMax )
{
  pLayoutWork = lytWork;
  lwkMax      = lytMax;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク取得
 *
 * @param   id      レイアウトのID
 *
 * @return  レイアウトワークのポインタ
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytWk * G2DUtil::GetLayoutWork( u32 id ) const
{
  return pLayoutWork[id].pLytWk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウト表示設定
 *
 * @param   id      レイアウトのID
 * @param   flg     true = 表示, false = 非表示
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetLayoutDrawEnable( u32 id, bool flg )
{
  pLayoutWork[id].enable = flg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウト表示状態取得
 *
 * @param   id      レイアウトのID
 *
 * @retval  "true = 表示"
 * @retval  "false = 非表示"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::GetLayoutDrawEnable( u32 id ) const
{
  return pLayoutWork[id].enable;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワークのセットアップデータ取得
 *
 * @param   disp	ディスプレイ
 *
 * @return	セットアップデータ
 */
//--------------------------------------------------------------------------------------------
const gfl2::lyt::LytWkSetUp * G2DUtil::GetLayoutWorkSetup( gfl2::lyt::DisplayType disp )
{
  if( disp == gfl2::lyt::DISPLAY_UPPER ){
    return &LytWkDefaultSetup[0];
  }
  return &LytWkDefaultSetup[1];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペイン表示設定
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   flg     true = 表示, false = 非表示
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetPaneVisible( u32 lytID, gfl2::lyt::LytPaneIndex index, bool flg )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    pane->SetVisible( flg );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   ペイン表示設定(ペインポインタ版)
  *
  * @param   lytID   レイアウトのID
  * @param   index   ペインのインデックス
  * @param   flg     true = 表示, false = 非表示
  *
  * @return  none
  */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetPaneVisible( gfl2::lyt::LytPane* pane, bool flg )
{
  GFL_ASSERT( pane );
  pane->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペイン表示状態取得
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 *
 * @retval  "true = 表示"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::GetPaneVisible( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    return pane->IsVisible();
  }
  GFL_ASSERT( 0 );
  return false;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   ペイン表示状態取得(ペインポインタ版)
  *
  * @param   lytID   レイアウトのID
  * @param   index   ペインのインデックス
  *
  * @retval  "true = 表示"
  * @retval  "false = それ以外"
  */
//--------------------------------------------------------------------------------------------
bool G2DUtil::GetPaneVisible( gfl2::lyt::LytPane* pane ) const
{
  GFL_ASSERT( pane );

  return  pane->IsVisible();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ペインの矩形を取得する
 * @param layout_work   ペインが所属するレイアウトワーク
 * @param pane_index    対象ペインのインデックス
 * @return ペインの矩形サイズ
 */
//--------------------------------------------------------------------------------------------
const nw::ut::Rect G2DUtil::GetPaneRect( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    return pane->GetPaneRect();
  }
  GFL_ASSERT( 0 );
  {
    nw::ut::Rect rect;
    return rect;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのアルファ値設定
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   alpha   アルファ値
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetPaneAlpha( u32 lytID, gfl2::lyt::LytPaneIndex index, u8 alpha )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    pane->SetAlpha( alpha );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのアルファ値設定(ペイン直接指定版)
 *
 * @param		pane		ペイン
 * @param   alpha   アルファ値
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetPaneAlpha( gfl2::lyt::LytPane* pane, u8 alpha )
{
  if( pane != NULL )
  {
    pane->SetAlpha( alpha );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのアルファ値を取得
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 *
 * @return  アルファ値
 */
//--------------------------------------------------------------------------------------------
u8 G2DUtil::GetPaneAlpha( u32 lytID, gfl2::lyt::LytPaneIndex index )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    return pane->GetAlpha();
  }
  GFL_ASSERT( 0 );
  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのアルファ値を取得(ペイン直接指定版)
 *
 * @param		pane		ペイン
 *
 * @return  アルファ値
 */
//--------------------------------------------------------------------------------------------
u8 G2DUtil::GetPaneAlpha( gfl2::lyt::LytPane* pane )
{
  if( pane != NULL )
  {
    return pane->GetAlpha();
  }
  GFL_ASSERT( 0 );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief ピクチャーペインの頂点カラー成分を設定する
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   color   カラー値
 */
//-----------------------------------------------------------------------------
void G2DUtil::SetVtxColorElementOfPicturePane( u32 lytID, gfl2::lyt::LytPaneIndex index, nw::ut::Color8 color )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    pane->SetVtxColor( nw::lyt::VERTEXCOLOR_LT, color );
    pane->SetVtxColor( nw::lyt::VERTEXCOLOR_RT, color );
    pane->SetVtxColor( nw::lyt::VERTEXCOLOR_LB, color );
    pane->SetVtxColor( nw::lyt::VERTEXCOLOR_RB, color );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		マテリアルカラーを設定
 *
 * @param		lytID		レイアウトID
 * @param		pane		ペイン
 * @param		index		設定するカラーのインデックス
 * @param		color		カラー
 *
 * @return  none
 *
 * @li	LayoutEditor上のカラー補間の変更
 * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
 * @li	index = COLOR_INDEX_WHITE : 白カラー補間
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMaterialColor( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 index, nw::ut::Color8 color )
{
  gfl2::lyt::LytMaterial * material = pLayoutWork[lytID].pLytWk->GetMaterial( pane );
  material->SetColor( index, color );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		マテリアルカラーを設定
 *
 * @param		pane		ペイン
 * @param		index		設定するカラーのインデックス
 * @param		color		カラー
 *
 * @return  none
 *
 * @li	LayoutEditor上のカラー補間の変更
 * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
 * @li	index = COLOR_INDEX_WHITE : 白カラー補間
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMaterialColor( gfl2::lyt::LytPane* pane, u32 index, nw::ut::Color8 color )
{
  GFL_ASSERT( pane );
  
  gfl2::lyt::LytMaterial * material = pane->GetMaterial();
  GFL_ASSERT( material );

  material->SetColor( index, color );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		マテリアルカラーを取得
 *
 * @param		lytID		レイアウトID
 * @param		pane		ペイン
 * @param		index		設定するカラーのインデックス
 *
 * @return  カラー
 *
 * @li	LayoutEditor上のカラー補間の変更
 * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
 * @li	index = COLOR_INDEX_WHITE : 白カラー補間
 */
//--------------------------------------------------------------------------------------------
const nw::ut::Color8 & G2DUtil::GetMaterialColor( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 index )
{
  gfl2::lyt::LytMaterial * material = pLayoutWork[lytID].pLytWk->GetMaterial( pane );
  return material->GetColor( index );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		マテリアルカラーを取得
 *
 * @param		pane		ペインのアドレス
 * @param		index		設定するカラーのインデックス
 *
 * @return  カラー
 *
 * @li	LayoutEditor上のカラー補間の変更
 * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
 * @li	index = COLOR_INDEX_WHITE : 白カラー補間
 */
//--------------------------------------------------------------------------------------------
const nw::ut::Color8 & G2DUtil::GetMaterialColor( gfl2::lyt::LytPane* pane, u32 index )
{
  GFL_ASSERT( pane );
  
  gfl2::lyt::LytMaterial * material = pane->GetMaterial();
  GFL_ASSERT( material );

  return material->GetColor( index );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインの座標取得
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 *
 * @return  ペインの座標
 */
//--------------------------------------------------------------------------------------------
gfl2::math::VEC3 G2DUtil::GetPanePos( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  // app_utilの処理に置き換え 2015/02/26 by nakahiro
  gfl2::math::VEC3 pos;
  const gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    pos = GetPanePos( pane );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
  return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインの座標取得（ポインタ指定版）
 *
 * @param   pane  ペイン
 *
 * @return  ペインの座標
 *
 * @note  親がいないペインは取得できない
 */
//--------------------------------------------------------------------------------------------
gfl2::math::VEC3 G2DUtil::GetPanePos( const gfl2::lyt::LytPane * pane ) const
{
#if 0
  // このやり方だとZ座標に意図しない値が入るので変更 2015/10/15 by nakahiro
  gfl2::math::VEC3 pos;
  const gfl2::math::MTX34 & global_matrix = pane->GetGlobalMtx();
  pos.x = global_matrix._03;
  pos.y = global_matrix._13;
  pos.z = global_matrix._23;
  return pos;
#else
  gfl2::math::VEC3 pos( 0.0f, 0.0f, 0.0f );

  if( pane == NULL )
  {
    GFL_ASSERT( 0 );
  }
  else
  {
    for ever{
      // 親がいない（ルートペインも弾かれる）
      const gfl2::lyt::LytPane * parent = pane->GetParent();
      if( parent == NULL )
      {
        break;
      }
      pos += pane->GetTranslate();
      pane = parent;
    }
  }

  return pos;
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのローカル座標を取得
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 *
 * @return  ペインの座標
 */
//--------------------------------------------------------------------------------------------
gfl2::math::VEC3 G2DUtil::GetPaneLocalPos( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  gfl2::math::VEC3 pos;
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    pos = pane->GetTranslate();
  }
  else
  {
    GFL_ASSERT( 0 );
  }
  return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのローカル座標を取得（ポインタ指定版）
 *
 * @param   pane  ペイン
 *
 * @return  ペインの座標
 */
//--------------------------------------------------------------------------------------------
gfl2::math::VEC3 G2DUtil::GetPaneLocalPos( const gfl2::lyt::LytPane * pane ) const
{
  return pane->GetTranslate();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインの座標設定
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   pos     座標
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetPanePos( u32 lytID, gfl2::lyt::LytPaneIndex index, const gfl2::math::VEC3 * pos )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  if( pane != NULL )
  {
    pane->SetTranslate( *pos );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチ座標にペインを移動
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   x       タッチX座標
 * @param   y       タッチY座標
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetPaneTouchPos( u32 lytID, gfl2::lyt::LytPaneIndex index, u16 x, u16 y )
{
  gfl2::math::VEC3 pos = GetPanePos( lytID, index );

  if( pLayoutWork[lytID].setupDisp == SETUP_UPPER ){
    pos.x = static_cast<f32>( x-200 );
    pos.y = static_cast<f32>( 120-y );
  }else{
    pos.x = static_cast<f32>( x-160 );
    pos.y = static_cast<f32>( 120-y );
  }
  SetPanePos( lytID, index, &pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのサイズ取得
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 *
 * @return  サイズ
 */
//--------------------------------------------------------------------------------------------
const nw::lyt::Size & G2DUtil::GetSize( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  return pane->GetSize();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのサイズ設定
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   size		サイズ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetSize( u32 lytID, gfl2::lyt::LytPaneIndex index, const nw::lyt::Size & size )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  pane->SetSize( size );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのスケール設定
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   scale   スケール値
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetScale( u32 lytID, gfl2::lyt::LytPaneIndex index, gfl2::math::VEC2 & scale )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  pane->SetScale( scale );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのスケール設定(ペイン指定版)
 *
 * @param   pPane   ペイン
 * @param   scale   スケール値
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetScale( gfl2::lyt::LytPane* pPane, gfl2::math::VEC2 & scale )
{
  GFL_ASSERT( pPane );
  pPane->SetScale( scale );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのスケール取得
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   scale   取得場所
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::GetScale( u32 lytID, gfl2::lyt::LytPaneIndex index, gfl2::math::VEC2 * scale ) const
{
	gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
	*scale = pane->GetScale();
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetRotate
 * @brief   ペインの回転を取得
 * @date    2015.02.26
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   rotate  取得場所
 *
 * @note  親ペインからのオフセット回転を返します
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::GetRotate( u32 lytID, gfl2::lyt::LytPaneIndex index, gfl2::math::VEC3 * rotate ) const
{
  const gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  *rotate = pane->GetRotate();
}

//--------------------------------------------------------------------------------------------
/**
 * @func    SetRotate
 * @brief   ペインの回転を変更
 * @date    2015.02.26
 *
 * @param   lytID   レイアウトのID
 * @param   index   ペインのインデックス
 * @param   rotate  変更する値
 *
 * @note  親ペインからのオフセット回転を変更します
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRotate( u32 lytID, gfl2::lyt::LytPaneIndex index, const gfl2::math::VEC3 & rotate )
{
  gfl2::lyt::LytPane * pane = pLayoutWork[lytID].pLytWk->GetPane( index );
  pane->SetRotate( rotate );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アニメ開始
 *
 * @param   lytID         レイアウトのID
 * @param   anm           アニメ番号
 * @param   auto_anm_flag オートアニメーションするかどうか（デフォルト：true）
 * @param   auto_unbind   終了時に自動でアンバインドするか（デフォルト：true）
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::StartAnime( u32 lytID, u32 anm, bool auto_anm_flag, bool auto_unbind )
{
  StartAnimeFrame( lytID, anm, 0.0f, auto_anm_flag, auto_unbind );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   フレーム指定してアニメ開始
 *
 * @param   lytID         レイアウトのID
 * @param   anm           アニメ番号
 * @param   frame         フレーム
 * @param   auto_anm_flag オートアニメーションするかどうか（デフォルト：true）
 * @param   auto_unbind   終了時に自動でアンバインドするか（デフォルト：true）
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::StartAnimeFrame( u32 lytID, u32 anm, f32 frame, bool auto_anm_flag, bool auto_unbind )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) == false )
  {
    pLayoutWork[lytID].pLytWk->BindAnime( anm, auto_unbind );
  }
  pLayoutWork[lytID].pLytWk->SetAutoAnime( anm, auto_anm_flag );
  pLayoutWork[lytID].pLytWk->SetAnimeFrame( anm, frame );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アニメ停止
 *
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 * @param   clear   アニメを初期化するか true = 初期化 (デフォルト: false)
 *
 * @return  none
 *
 * @li  clear = true の場合、Unbindされません
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::StopAnime( u32 lytID, u32 anm, bool clear )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) == false )
  {
    return;
  }
  if( clear == true )
  {
    pLayoutWork[lytID].pLytWk->SetAutoAnime( anm, false );
    pLayoutWork[lytID].pLytWk->SetAnimeFrame( anm, 0.0f );
  }
  else
  {
    pLayoutWork[lytID].pLytWk->UnBindAnime( anm );
  }
}

//--------------------------------------------------------------------------------------------
/**
* @brief   アニメの再生速度を指定
*
* @param   lytID         レイアウトのID
* @param   speed         再生速度
*
* @return  none
*/
//--------------------------------------------------------------------------------------------
void G2DUtil::SetAnimeSpeed( u32 lytID, f32 speed )
{
  pLayoutWork[lytID].pLytWk->SetAutoAnimeSpeed( speed );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   アニメ終了チェック
 *
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 *
 * @retval  "false = 再生中"
 * @retval  "true = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsAnimeEnd( u32 lytID, u32 anm )
{
  if( pLayoutWork[lytID].pLytWk->IsAnime( anm ) != false &&
      pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) != false &&
      pLayoutWork[lytID].pLytWk->IsAnimeEnd( anm ) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief アニメーションが一時停止中か
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 * @retval  true  = 一時停止中
 *          false = 再生中かバインドされていない
 */
//-----------------------------------------------------------------------------
bool G2DUtil::IsAnimePause( u32 lytID, u32 anm )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) != false )
  {
    return !( pLayoutWork[lytID].pLytWk->IsAutoAnime(anm) );
  }
  return false;
}
//-----------------------------------------------------------------------------
/**
 * @brief アニメーションを一時停止する
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 * @param notPause            trueなら再開, falseなら停止
 */
//-----------------------------------------------------------------------------
void G2DUtil::PauseAnime( u32 lytID, u32 anm, bool notPause )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) != false )
  {
    pLayoutWork[lytID].pLytWk->SetAutoAnime( anm, notPause );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アニメーションフレームセット
 *
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 * @param   frame   セットするフレーム
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetAnimeFrame( u32 lytID, u32 anm, f32 frame )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) != false )
  {
    pLayoutWork[lytID].pLytWk->SetAutoAnime( anm, false );
    pLayoutWork[lytID].pLytWk->SetAnimeFrame( anm, frame );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アニメーションフレーム取得
 *
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 *
 * @return	アニメフレーム
 */
//--------------------------------------------------------------------------------------------
f32 G2DUtil::GetAnimeFrame( u32 lytID, u32 anm )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) != false )
  {
    return pLayoutWork[lytID].pLytWk->GetAnimeFrame( anm );
  }
  return 0.0f;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   最大アニメーションフレーム取得
 *
 * @param   lytID   レイアウトのID
 * @param   anm     アニメ番号
 *
 * @return	最大アニメフレーム
 */
//--------------------------------------------------------------------------------------------
f32 G2DUtil::GetAnimeMaxFrame( u32 lytID, u32 anm )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) != false )
  {
    return pLayoutWork[lytID].pLytWk->GetAnimeMaxFrame( anm );
  }
  return 0.0f;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief バインドアニメする
 *
 * @param LytID レイアウト
 * @param anm   アニメ
 * @param auto_unbind オートアンバインド設定
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::BindAnime( u32 lytID, u32 anm, bool auto_unbind )
{
  if( pLayoutWork[lytID].pLytWk->IsBindAnime( anm ) == false )
  {
    pLayoutWork[lytID].pLytWk->BindAnime( anm, auto_unbind );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   子ペインを追加
 *
 * @param   lytID   レイアウトのID
 * @param   p       親ペインのインデックス
 * @param   c       子ペインのインデックス
 * @param   mode    追加モード
 * @param   n       追加する位置の前のペインインデックス ( mode = INSERT のときのみ )
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::AppendChild(
        u32 lytID, gfl2::lyt::LytPaneIndex p,
        gfl2::lyt::LytPaneIndex c, u32 mode, gfl2::lyt::LytPaneIndex n )
{
  gfl2::lyt::LytPane * child  = pLayoutWork[lytID].pLytWk->GetPane( c );
  gfl2::lyt::LytPane * parent = child->GetParent();

  // 現在の親から削除
  if( parent != NULL ){
    parent->RemoveChild( child );
  }
  // 親設定
  if( p == APPEND_CHILD_INSERT_ROOT ){
    parent = pLayoutWork[lytID].pLytWk->GetLayoutCore()->GetRootPane();
  }else{
    parent = pLayoutWork[lytID].pLytWk->GetPane( p );
  }

  switch( mode ){
  case APPEND_CHILD_MODE_BOTTOM:
    parent->AppendChild( child );
    break;
  case APPEND_CHILD_MODE_TOP:
    parent->PrependChild( child );
    break;
  case APPEND_CHILD_MODE_INSERT:
    {
      gfl2::lyt::LytPane * next  = pLayoutWork[lytID].pLytWk->GetPane( n );
      parent->InsertChild( next, child );
    }
    break;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   子ペインを親子リストから削除
 *
 * @param   lytID   レイアウトのID
 * @param   p       親ペインのインデックス
 * @param   c       子ペインのインデックス
 *
 * @return  none
 *
 * @li  親がなくなるので注意。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::RemoveChild( u32 lytID, gfl2::lyt::LytPaneIndex p, gfl2::lyt::LytPaneIndex c )
{
  gfl2::lyt::LytPane * parent;
  if( p == APPEND_CHILD_INSERT_ROOT ){
    parent = pLayoutWork[lytID].pLytWk->GetLayoutCore()->GetRootPane();
  }else{
    parent = pLayoutWork[lytID].pLytWk->GetPane( p );
  }
  gfl2::lyt::LytPane * child = pLayoutWork[lytID].pLytWk->GetPane( c );
  parent->RemoveChild( child );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetTouchPanel
 * @brief   タッチ中パネルを取得
 * @date    2015.02.26
 *
 * @return  タッチパネル
 */
//--------------------------------------------------------------------------------------------
gfl2::ui::TouchPanel * G2DUtil::GetTouchPanel(void) const
{
  gfl2::ui::DeviceManager * man =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  return man->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsTouchPane
 * @brief   タッチ座標にペインがあるか
 * @date    2015.02.26
 *
 * @param   tp      タッチパネル
 * @param   lytID   レイアウトのID
 * @param   pane    ペイン
 *
 * @retval  true  = ある
 * @retval  false = ない
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsTouchPane( gfl2::ui::TouchPanel * tp, u32 lytID, gfl2::lyt::LytPaneIndex pane ) const
{
  u16 tpx = tp->GetX();
  u16 tpy = tp->GetY();
  return pLayoutWork[lytID].pLytWk->GetHitPane( tpx, tpy, pane );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsTouchPane
 * @brief   タッチ座標にペインがあるか（ポインタ指定版）
 * @date    2015.03.31
 *
 * @param   tp      タッチパネル
 * @param   lytID   レイアウトのID
 * @param   pane    ペイン
 *
 * @retval  true  = ある
 * @retval  false = ない
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsTouchPane( gfl2::ui::TouchPanel * tp, u32 lytID, gfl2::lyt::LytPane * pane ) const
{
  u16 tpx = tp->GetX();
  u16 tpy = tp->GetY();
  return pLayoutWork[lytID].pLytWk->GetHitPane( tpx, tpy, pane );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsPaneTouchTrigger
 * @brief   ペインがタッチされたかどうか
 * @date    2015.02.26
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペイン
 *
 * @retval  true  = タッチされた
 * @retval  false = それ以外
 *
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsPaneTouchTrigger( u32 lytID, gfl2::lyt::LytPaneIndex pane ) const
{
  // 一度描画しないとチェック不可
  if( pLayoutWork[lytID].pLytWk->IsHitCheckOK() == false )
  {
    return false;
  }
  gfl2::ui::TouchPanel * tp = GetTouchPanel();
  if( tp->IsTouchTrigger() != false )
  {
    return IsTouchPane( tp, lytID, pane );
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインがタッチされたかどうか（ポインタ指定版）
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペイン
 *
 * @retval  true  = タッチされた
 * @retval  false = それ以外
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsPaneTouchTrigger( u32 lytID, gfl2::lyt::LytPane * pane ) const
{
  // 一度描画しないとチェック不可
  if( pLayoutWork[lytID].pLytWk->IsHitCheckOK() == false )
  {
    return false;
  }
  gfl2::ui::TouchPanel * tp = GetTouchPanel();
  if( tp->IsTouchTrigger() != false )
  {
    return IsTouchPane( tp, lytID, pane );
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインをタッチ中かどうか
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペイン
 *
 * @retval  "true = タッチ中"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsPaneTouchHold( u32 lytID, gfl2::lyt::LytPaneIndex pane ) const
{
  // 一度描画しないとチェック不可
  if( pLayoutWork[lytID].pLytWk->IsHitCheckOK() == false )
  {
    return false;
  }
  gfl2::ui::TouchPanel * tp = GetTouchPanel();
  if( tp->IsTouch() != false )
  {
    return IsTouchPane( tp, lytID, pane );
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインをタッチ中かどうか（ポインタ指定版）
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペイン
 *
 * @retval  "true = タッチ中"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::IsPaneTouchHold( u32 lytID, gfl2::lyt::LytPane * pane ) const
{
  // 一度描画しないとチェック不可
  if( pLayoutWork[lytID].pLytWk->IsHitCheckOK() == false )
  {
    return false;
  }
  gfl2::ui::TouchPanel * tp = GetTouchPanel();
  if( tp->IsTouch() != false )
  {
    return IsTouchPane( tp, lytID, pane );
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetTouchPaneIndex
 * @brief   タッチ座標のペインを取得
 * @date    2015.02.26
 *
 * @param   tp          タッチパネル
 * @param   lytID       レイアウトのID
 * @param   group       グループ
 * @param   bound_only  true = 境界ペインのみ
 *
 * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR != 取得したペイン
 * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR = 取得できなかった
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytPaneIndex G2DUtil::GetTouchPaneIndex(
    gfl2::ui::TouchPanel * tp, u32 lytID, gfl2::lyt::LytPaneIndex group, bool bound_only ) const
{
  u16 tpx = tp->GetX();
  u16 tpy = tp->GetY();
  if( group == gfl2::lyt::LYTSYS_PANEINDEX_ERROR )
  {
    return pLayoutWork[lytID].pLytWk->GetHitPaneIndexAll( tpx, tpy, bound_only );
  }
  else
  {
    return pLayoutWork[lytID].pLytWk->GetHitPaneIndexGroup( group, tpx, tpy, bound_only );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetTouchTriggerPaneIndex
 * @brief   タッチされたペインを取得
 * @date    2015.02.26
 *
 * @param   lytID       レイアウトのID
 * @param   group       グループ
 * @param   bound_only  true = 境界ペインのみ
 *
 * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR != 取得したペイン
 * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR = 取得できなかった
 *
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytPaneIndex G2DUtil::GetTouchTriggerPaneIndex(
    u32 lytID, gfl2::lyt::LytPaneIndex group, bool bound_only ) const
{
  // 一度描画しないとチェック不可
  if( pLayoutWork[lytID].pLytWk->IsHitCheckOK() == false )
  {
    return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
  }
  gfl2::ui::TouchPanel * tp = GetTouchPanel();
  if( tp->IsTouchTrigger() != false )
  {
    return GetTouchPaneIndex( tp, lytID, group, bound_only );
  }
  return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetTouchHoldPaneIndex
 * @brief   タッチされているペインを取得
 * @date    2015.02.26
 *
 * @param   lytID       レイアウトのID
 * @param   group       グループ
 * @param   bound_only  true = 境界ペインのみ
 *
 * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR != 取得したペイン
 * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR = 取得できなかった
 *
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytPaneIndex G2DUtil::GetTouchHoldPaneIndex(
    u32 lytID, gfl2::lyt::LytPaneIndex group, bool bound_only ) const
{
  // 一度描画しないとチェック不可
  if( pLayoutWork[lytID].pLytWk->IsHitCheckOK() == false )
  {
    return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
  }
  gfl2::ui::TouchPanel * tp = GetTouchPanel();
  if( tp->IsTouch() != false )
  {
    return GetTouchPaneIndex( tp, lytID, group, bound_only );
  }
  return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定座標がペインにＨＩＴするかどうか
 *
 * @param   lytID     レイアウトのID
 * @param   pane      ペイン
 * @param   x         X座標
 * @param   y         Y座標
 * @param   visible   非表示ペインをチェックするか true = チェック ※デフォルト = true
 *
 * @retval  "true = ヒット中"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::GetHitPane( u32 lytID, gfl2::lyt::LytPaneIndex pane, u16 x, u16 y, bool visible ) const
{
  return GetHitPane( lytID, pLayoutWork[lytID].pLytWk->GetPane(pane), x, y, visible );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定座標がペインにＨＩＴするかどうか
 *
 * @param   lytID     レイアウトのID
 * @param   pane      ペイン
 * @param   x         X座標
 * @param   y         Y座標
 * @param   visible   非表示ペインをチェックするか true = チェック ※デフォルト = true
 *
 * @retval  "true = ヒット中"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::GetHitPane( u32 lytID, gfl2::lyt::LytPane * pane, u16 x, u16 y, bool visible ) const
{
  if( pane == NULL )
  {
    return false;
  }

  gfl2::lyt::LytWk * layout_work = pLayoutWork[lytID].pLytWk;

  // 一度描画しないとチェック不可
  if( !layout_work->IsHitCheckOK() )
  {
    return false;
  }
  if( visible == false )
  {
    if( GetPaneVisible( pane ) == false )
    {
      return false;
    }
  }
  return layout_work->GetHitPane( x, y, pane );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャを変更 ( 登録したインデックスを指定 )
 *
 * @param   lytID     レイアウトID
 * @param   pane      ペイン
 * @param   resID     テクスチャのリソースID
 * @param   resIndex  リソース内のインデックス
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTexMap( u32 lytID, u32 pane, u32 resID, u32 resIndex )
{
	SetTexMapEx( lytID, pane, pLayoutResID[resID], resIndex );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャを変更 ( 登録したインデックスを指定 )(ペインポインタ指定版)
 *
 * @param   pane      ペインポインタ
 * @param   resID     テクスチャのリソースID
 * @param   resIndex  リソース内のインデックス
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTexMap( gfl2::lyt::LytPane* pane, u32 resID, u32 resIndex )
{
	SetTexMapEx( pane, pLayoutResID[resID], resIndex );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャを変更 ( LytResID指定版 )
 *
 * @param   lytID     レイアウトID
 * @param   pane      ペイン
 * @param   resID     テクスチャのリソースID
 * @param   resIndex  リソース内のインデックス
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTexMapEx( u32 lytID, u32 pane, gfl2::lyt::LytMultiResID resID, u32 resIndex )
{
	gfl2::lyt::LytMaterial * material = pLayoutWork[lytID].pLytWk->GetMaterial( pane );
	gfl2::lyt::LytRes * res = pLayoutSystem->GetResource( &resID );
  const gfl2::lyt::LytResTextureInfo* info = res->GetTexture( resIndex, resID.GetMultiID() );
  material->SetTexMap( 0, info );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャを変更 ( LytResID指定版 )(ペインポインタ指定版)
 *
 * @param   pane      ペインポインタ
 * @param   resID     テクスチャのリソースID
 * @param   resIndex  リソース内のインデックス
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTexMapEx( gfl2::lyt::LytPane* pane, gfl2::lyt::LytMultiResID resID, u32 resIndex )
{
	gfl2::lyt::LytMaterial * material = pane->GetMaterial();
	gfl2::lyt::LytRes * res = pLayoutSystem->GetResource( &resID );
  const gfl2::lyt::LytResTextureInfo* info = res->GetTexture( resIndex, resID.GetMultiID() );
  material->SetTexMap( 0, info );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    CreateStrBuf
 * @brief   汎用文字列バッファ作成
 * @date    2015.02.26
 *
 * @param   heap  ヒープ
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateStrBuf( Heap * heap )
{
  for( u32 i=0; i<TEMP_STRBUF_NUM; i++ )
  {
    m_strbuf[i] = GFL_NEW( heap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, heap->GetDeviceHeap() );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @func    DeleteStrBuf
 * @brief   汎用文字列バッファ削除
 * @date    2015.02.26
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteStrBuf(void)
{
  for( u32 i=0; i<TEMP_STRBUF_NUM; i++ )
  {
    GFL_DELETE m_strbuf[i];
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージ関連作成
 *
 * @param   datID         アーカイブデータID
 * @param   loadType      読み込みタイプ
 * @param   heap    ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageData( u32 datID, gfl2::str::MsgData::LoadType loadType, Heap * heap , s32 wordsetBufferNum )
{
  GFL_ASSERT(pMsgData==NULL);
  pMsgData = GFL_NEW( heap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId(), datID, heap->GetSystemHeap(), loadType );
  pWordSet = GFL_NEW( heap->GetDeviceHeap() ) print::WordSet( wordsetBufferNum, print::WordSet::DEFAULT_BUF_LEN , heap->GetSystemHeap() );
  m_bMessageCreate = true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   外部メッセージデータ＆ワードセットを設定
 *
 * @param   messageData メッセージデータインスタンス
 * @param   wordset     ワードセットインスタンス
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMessageData( gfl2::str::MsgData *messageData, print::WordSet *wordset )
{
  //  途中でメッセージを切り替えことがある
//  GFL_ASSERT(pMsgData==NULL);
  m_bMessageCreate = false;
  pMsgData = messageData;
  pWordSet = wordset;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メッセージデータ取得
 *
 * @param		none
 *
 * @return	メッセージデータ
 */
//--------------------------------------------------------------------------------------------
gfl2::str::MsgData * G2DUtil::GetMsgData(void)
{
  return pMsgData;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ワードセット取得
 *
 * @param		none
 *
 * @return	ワードセット
 */
//--------------------------------------------------------------------------------------------
print::WordSet * G2DUtil::GetWordSet(void)
{
  return pWordSet;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージ関連削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteMessageData(void)
{
  if(pWordSetLoader){
    GFL_SAFE_DELETE( pWordSetLoader );
  }
  if(m_bMessageCreate){
    GFL_SAFE_DELETE( pWordSet );
    GFL_SAFE_DELETE( pMsgData );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ワードセットの事前バッファ読み込み
 *
 * @param		heap		ヒープ
 * @param		readID	読み込むデータのID（ビット指定）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::WordSetPreLoad( Heap * heap, u32 readID )
{
  pWordSetLoader = GFL_NEW( heap->GetDeviceHeap() ) WordSetLoader( heap->GetSystemHeap(), readID );
  pWordSetLoader->StartLoad( heap->GetSystemHeap() );
  pWordSet->SetWordSetLoader( pWordSetLoader );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ワードセットの事前バッファ読み込み終了待ち
 *
 * @param		none
 *
 * @retval	"true = 終了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::WordSetWaitLoad(void)
{
  if(pWordSetLoader){
    return pWordSetLoader->WaitLoad();
  }
  return true;  //無い時は終了扱い
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		ワードセットローダークラスの直接指定
 * @param		WordSetLoader
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetWordSetLoader(WordSetLoader* ploader)
{
  GFL_ASSERT(pWordSet);
  if( ploader == NULL )
  {
    //  NULLなら何も設定しない
    //  すでに使う側でWordSetLoaderされている可能性があるので、
    return;
  }

  pWordSet->SetWordSetLoader( ploader );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   文字列取得
 *
 * @param   buf     取得場所
 * @param   id      文字列ID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::GetString( gfl2::str::StrBuf * buf, u32 id )
{
  pMsgData->GetString( id, *buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   文字列を展開して取得
 *
 * @param   buf     取得場所
 * @param   id      文字列ID
 *
 * @return  none
 *
 * @li  展開領域にテンポラリバッファの[0]を使用しているため、取得場所には使えないので注意！
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::GetStringExpand( gfl2::str::StrBuf * buf, u32 id )
{
  pMsgData->GetString( id, *m_strbuf[0] );
  pWordSet->Expand( buf, m_strbuf[0] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに数字を描画
 *
 * @param   lytID       レイアウトのID
 * @param   pane        ペインのインデックス
 * @param   strID				文字列ID
 * @param   value       数値
 * @param   keta        桁
 * @param		bufID				バッファID　※デフォルト = 0
 * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
 * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneNumber(
      u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 strID,
      u32 value, u32 keta, u32 bufID, print::NumberDispType dispType, print::NumberCodeType codeType )
{
  SetRegisterNumber( bufID, value, keta, dispType, codeType );
  SetTextBoxPaneStringExpand( lytID, pane, strID );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに数字を描画（StrBuf版）
 *
 * @param   lytID       レイアウトのID
 * @param   pane        ペインのインデックス
 * @param   str					文字列
 * @param   value       数値
 * @param   keta        桁
 * @param		bufID				バッファID
 * @param   dispType    整形タイプ
 * @param   codeType    文字コード
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneNumber(
      u32 lytID, gfl2::lyt::LytPaneIndex pane, gfl2::str::StrBuf* str,
      u32 value, u32 keta, u32 bufID, print::NumberDispType dispType, print::NumberCodeType codeType )
{
  SetRegisterNumber( bufID, value, keta, dispType, codeType );
  SetTextBoxPaneStringExpand( lytID, pane, str );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   テキストボックスペインに数字を描画(ペインポインタ指定版)
  *
  * @param   lytID       レイアウトのID
  * @param   pane        ペインのインデックス
  * @param   strID				文字列ID
  * @param   value       数値
  * @param   keta        桁
  * @param		bufID				バッファID　※デフォルト = 0
  * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
  * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
  *
  * @return  none
  */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneNumber(
      gfl2::lyt::LytTextBox* pane, u32 strID,
      u32 value, u32 keta, u32 bufID, print::NumberDispType dispType, print::NumberCodeType codeType )
{
  GFL_ASSERT( pane );

  //  文字列作成
  {
    SetRegisterNumber( bufID, value, keta, dispType, codeType );

    pMsgData->GetString( strID, *m_strbuf[0] );
    pWordSet->Expand( m_strbuf[1], m_strbuf[0] );
  }

  print::SetStringForLytTextBox( pane, m_strbuf[1] );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   テキストボックスペインに数字を描画（ペインポインタ指定 ＋ StrBuf版）
  *
  * @param   lytID       レイアウトのID
  * @param   pane        ペインのインデックス
  * @param   str					文字列
  * @param   value       数値
  * @param   keta        桁
  * @param		bufID				バッファID
  * @param   dispType    整形タイプ
  * @param   codeType    文字コード
  *
  * @return  none
  */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneNumber(
    gfl2::lyt::LytTextBox* pane, gfl2::str::StrBuf* str,
    u32 value, u32 keta, u32 bufID, print::NumberDispType dispType, print::NumberCodeType codeType )
{
  GFL_ASSERT( pane );

  //  文字列作成
  {
    SetRegisterNumber( bufID, value, keta, dispType, codeType );
    pWordSet->Expand( m_strbuf[1], str );
  }

  print::SetStringForLytTextBox( pane, m_strbuf[1] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( STRCODE直指定 )
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペインのインデックス
 * @param   str     文字列
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, const gfl2::str::STRCODE * str )
{
  gfl2::lyt::LytTextBox * text_box = pLayoutWork[lytID].pLytWk->GetTextBoxPane( pane );
  if( text_box != NULL )
  {
    print::SetStringForLytTextBox( text_box, str );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( StrBuf指定 )
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペインのインデックス
 * @param   buf     文字列
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, gfl2::str::StrBuf * buf )
{
  gfl2::lyt::LytTextBox * text_box = pLayoutWork[lytID].pLytWk->GetTextBoxPane( pane );
  if( text_box != NULL )
  {
    print::SetStringForLytTextBox( text_box, buf );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( STRCODE直指定 )
 *
 * @param   pane    ペインのポインタ
 * @param   str     文字列
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneString( gfl2::lyt::LytTextBox * pane, const gfl2::str::STRCODE * str )
{
  GFL_ASSERT( pane );
  GFL_ASSERT( str );

  print::SetStringForLytTextBox( pane, str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( ポインタ/StrBuf指定 )
 *
 * @param   pane    ペインのポインタ
 * @param   buf     文字列
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneString( gfl2::lyt::LytTextBox * pane, gfl2::str::StrBuf * buf )
{
  print::SetStringForLytTextBox( pane, buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( 文字列ID指定 )
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペインのインデックス
 * @param   strID   文字列ID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 strID )
{
  pMsgData->GetString( strID, *m_strbuf[0] );
  SetTextBoxPaneString( lytID, pane, m_strbuf[0] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( ポインタ/文字列ID指定 )
 *
 * @param   pane    ペインのポインタ
 * @param   strID   文字列ID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneString( gfl2::lyt::LytTextBox * pane, u32 strID )
{
  pMsgData->GetString( strID, *m_strbuf[0] );
  SetTextBoxPaneString( pane, m_strbuf[0] );
}

#if PM_DEBUG
//--------------------------------------------------------------------------------------------
/**
 * @func    SetTextBoxPaneString
 * @brief   テキストボックスペインに文字列を描画 ( char型文字列指定 )
 * @date    2015.02.26
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペインのインデックス
 * @param   string  文字列
 *
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextboxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, const char * string )
{
  // char >> wchar_t 変換
  size_t length = strlen( string );
  wchar_t wc[ 256 + 1 ];
  if( length > 256 )
  {
    length = 256;
  }
  gfl2::std::MemClear( wc, sizeof(wchar_t)*(length+1) );
#if defined(GF_PLATFORM_WIN32)
  mbstowcs_s( &length, wc, 256+1, string, length+1 ); // wc にワイド文字列が入る
#elif defined(GF_PLATFORM_CTR)
  length = mbstowcs( wc, string, length+1 ); // wc にワイド文字列が入る
#endif
  SetTextBoxPaneString( lytID, pane, wc );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    SetTextBoxPaneString
 * @brief   テキストボックスペインに文字列を描画 ( char型文字列指定 / ペインポインタ指定版 )
 * @date    2015.04.02
 *
 * @param   pTextPane     ペインポインタ
 * @param   string        文字列
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextboxPaneStringByDebug( gfl2::lyt::LytTextBox* pTextPane, const char * string )
{
  // char >> wchar_t 変換
  size_t length = strlen( string );
  wchar_t wc[ 256 + 1 ];
  if( length > 256 )
  {
    length = 256;
  }
  gfl2::std::MemClear( wc, sizeof(wchar_t)*(length+1) );
#if defined(GF_PLATFORM_WIN32)
  mbstowcs_s( &length, wc, 256+1, string, length+1 ); // wc にワイド文字列が入る
#elif defined(GF_PLATFORM_CTR)
  length = mbstowcs( wc, string, length+1 ); // wc にワイド文字列が入る
#endif
  SetTextBoxPaneString( pTextPane, wc );
}

#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( 展開版 )
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペインのインデックス
 * @param   str     文字列ID
 *
 * @return  none
 *
 * @li  WORDSETを使用した場合はこちら。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneStringExpand( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 str )
{
  pMsgData->GetString( str, *m_strbuf[0] );
  pWordSet->Expand( m_strbuf[1], m_strbuf[0] );

  SetTextBoxPaneString( lytID, pane, m_strbuf[1] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( StrBuf指定,展開版 )
 *
 * @param   lytID   レイアウトのID
 * @param   pane    ペインのインデックス
 * @param   str     文字列
 *
 * @return  none
 *
 * @li  WORDSETを使用した場合はこちら。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneStringExpand( u32 lytID, gfl2::lyt::LytPaneIndex pane, gfl2::str::StrBuf* str )
{
  pWordSet->Expand( m_strbuf[1], str );
  SetTextBoxPaneString( lytID, pane, m_strbuf[1] );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( StrBuf指定,展開版,ペインポインタ指定 )
 *
 * @param   pane    ペインのポインタ
 * @param   rStr    文字列
 * @note    暗黙の形変換による u32 str 版との曖昧さ回避（#defineによるマクロ定数を与えたときなど）のため、参照で受け取る
 *
 * @return  none
 *
 * @li  WORDSETを使用した場合はこちら。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneStringExpand( gfl2::lyt::LytTextBox * pane, gfl2::str::StrBuf& rStr )
{
  pWordSet->Expand( m_strbuf[1], &rStr );
  SetTextBoxPaneString( pane, m_strbuf[1] );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画 ( 展開版,ペインポインタ指定 )
 *
 * @param   pane    ペインのポインタ
 * @param   str     文字列ID
 *
 * @return  none
 *
 * @li  WORDSETを使用した場合はこちら。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneStringExpand( gfl2::lyt::LytTextBox * pane, u32 str )
{
  pMsgData->GetString( str, *m_strbuf[0] );
  pWordSet->Expand( m_strbuf[1], m_strbuf[0] );
  print::SetStringForLytTextBox( pane, m_strbuf[1] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファに数値をセット
 *
 * @param   bufID       バッファID
 * @param   value       数値
 * @param   keta        桁
 * @param   dispType    整形タイプ
 * @param   codeType    文字コード
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterNumber( u32 bufID, u32 value, u32 keta, print::NumberDispType dispType, print::NumberCodeType codeType )
{
  pWordSet->RegisterNumber( bufID, value, keta, dispType, codeType );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファに単語をセット
 *
 * @param   bufID       バッファID
 * @param   word        単語
 * @param   grm         文法性
 * @param   form        単複
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterWord( u32 bufID, const gfl2::str::StrBuf & word, print::WordSet::Grammer grm, print::WordSet::Form form )
{
  pWordSet->RegisterWord( bufID, word, grm, form );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファに単語をセット
 *
 * @param   bufID       バッファID
 * @param   strID       文字列ID
 * @param   form        単複
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterWord( u32 bufID, u32 strID, print::WordSet::Form form )
{
  pWordSet->RegisterWord( bufID, pMsgData, strID, form );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにプレイヤー名をセット
 *
 * @param   bufID   バッファID
 * @param   myst    プレイヤーデータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterPlayerName( u32 bufID, const Savedata::MyStatus * myst )
{
  pWordSet->RegisterPlayerName( bufID, myst );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにアイテム名をセット
 *
 * @param   bufID   バッファID
 * @param   item    アイテム番号
 * @param		count		個数　※デフォルト = 1
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterItemName( u32 bufID, u32 item, u32 count )
{
  pWordSet->RegisterItemName( bufID, item, count );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにポケモン名をセット
 *
 * @param   bufID   バッファID
 * @param   pp      CoreParam
 *
 * @return  none
 *
 * @note ppのタマゴフラグが立っていてもポケモン種族名を代入します
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterMonsName( u32 bufID, const pml::pokepara::CoreParam * pp )
{
  pWordSet->RegisterPokeMonsName( bufID, pp );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにポケモン名をセット
 *
 * @param   bufID   バッファID
 * @param   pp      CoreParam
 *
 * @return  none
 *
 * @note ppのタマゴフラグが立っているときはタマゴを代入し、立っていないときはポケモン種族名を代入します。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterMonsNameOrTamago( u32 bufID, const pml::pokepara::CoreParam * pp )
{
  pWordSet->RegisterPokeMonsNameOrTamago( bufID, pp );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにポケモン名をセット（MonsNo指定版）
 *
 * @param   bufID   バッファID
 * @param   monsno	ポケモン番号
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterMonsNameNo( u32 bufID, MonsNo monsno )
{
  pWordSet->RegisterPokeMonsNameNo( bufID, monsno );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにポケモン名をセット（MsgData）
 *
 * @param[in]  bufID
 * @param[in]  msgData  ポケモン種族名が書かれたmstxtを読み込んだもの
 * @param[in]  strID    msgDataの何番目のラベルに書かれているポケモン種族名かを指定
 *                      mstxtがMonsNo順に並んでいるならstrIDとMonsNoは同じになる
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterMonsNameMsgData( u32 bufID, gfl2::str::MsgData* msgData, u32 strID )
{
  pWordSet->RegisterPokeMonsNameMsgData( bufID, msgData, strID );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにニックネームをセット
 *
 * @param   bufID   バッファID
 * @param   pp      CoreParam
 *
 * @return  none
 *
 * @note ppのタマゴフラグが立っていてもニックネームを代入しますが、
 *       タマゴフラグ立っているときはニックネームにタマゴと入っているので
 *       そのまま代入すればOKです。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterPokeNickName( u32 bufID, const pml::pokepara::CoreParam * pp )
{
  pWordSet->RegisterPokeNickName( bufID, pp );
}
void G2DUtil::SetRegisterPokeNickName( u32 bufID, const gfl2::str::StrBuf& str, pml::Sex sex )
{
  pWordSet->RegisterPokeNickName( bufID, str, sex );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファに技名をセット
 *
 * @param   bufID   バッファID
 * @param   waza    技番号
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterWazaName( u32 bufID, u32 waza )
{
  pWordSet->RegisterWazaName( bufID, waza );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファにタイプ名をセット
 *
 * @param   bufID   バッファID
 * @param   type		タイプ名
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterPokeTypeName( u32 bufID, u32 type )
{
  pWordSet->RegisterPokeTypeName( bufID, type );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファに特性名をセット
 *
 * @param   bufID     バッファID
 * @param   tokusei   特性番号
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterTokuseiName( u32 bufID, u32 tokusei )
{
  pWordSet->RegisterTokuseiName( bufID, tokusei );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定バッファに性格名をセット
 *
 * @param   bufID     バッファID
 * @param   seikaku   性格番号
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetRegisterSeikakuName( u32 bufID, u32 seikaku )
{
  pWordSet->RegisterSeikakuName( bufID, seikaku );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テンポラリ文字列バッファ取得
 *
 * @param   id    バッファID
 *
 * @return  文字列バッファ
 */
//--------------------------------------------------------------------------------------------
gfl2::str::StrBuf * G2DUtil::GetTempStrBuf( u32 id )
{
  return m_strbuf[id];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		文字色変更
 *
 * @param   lytID     レイアウトのID
 * @param   pane      ペイン
 * @param		top				上端カラー
 * @param		bottom		下端カラー
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneColor(
      u32 lytID, gfl2::lyt::LytPaneIndex pane, const gfl2::lyt::ColorU8 & top, const gfl2::lyt::ColorU8 & bottom )
{
  SetTextBoxPaneColor( pLayoutWork[lytID].pLytWk->GetTextBoxPane( pane ), top, bottom );
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   文字色変更
  *
  * @param   pTextBoxPane  テキストボックスペイン
  * @param   top           上端カラー
  * @param   bottom        下端カラー
  *
  * @return  none
  */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextBoxPaneColor( gfl2::lyt::LytTextBox* pTextBoxPane, const gfl2::lyt::ColorU8 & top, const gfl2::lyt::ColorU8 & bottom )
{
  if( pTextBoxPane != NULL )
  {
    pTextBoxPane->SetTextColor( top, bottom );
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   文字色取得
  *
  * @param   pTextBoxPane  テキストボックスペイン
  * @param   pOutTop       上端カラー取得用
  * @param   pOutBottom    下端カラー取得用
  *
  * @return  none
  */
//--------------------------------------------------------------------------------------------
void G2DUtil::GetTextBoxPaneColor( gfl2::lyt::LytTextBox* pTextBoxPane, gfl2::lyt::ColorU8* pOutTop, gfl2::lyt::ColorU8* pOutBottom )
{
  GFL_ASSERT( pOutTop );
  GFL_ASSERT( pOutBottom );

  if( pTextBoxPane != NULL )
  {
    if( pOutTop )
    {
      *pOutTop    = pTextBoxPane->GetTextColor( nw::lyt::TEXTCOLOR_TOP );
    }

    if( pOutBottom )
    {
      *pOutBottom = pTextBoxPane->GetTextColor( nw::lyt::TEXTCOLOR_BOTTOM );
    }
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ作成
 *
 * @param   lytID     レイアウトID
 * @param   paneTbl   文字描画に使用するペインのテーブル（２つ必要）
 * @param   wait      メッセージ表示ウェイト
 * @param   heap      ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageWindow(  u32 lytID, const gfl2::lyt::LytPaneIndex * paneTbl, int wait, Heap * heap )
{
  gfl2::ui::DeviceManager * man =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  pMsgWin = GFL_NEW( heap->GetDeviceHeap() ) print::MessageWindow(
                                    man,
                                    pLayoutWork[lytID].pLytWk,
                                    2,
                                    wait,
                                    heap->GetSystemHeap() );
  pMsgWin->AddLinePaneIdx( paneTbl[0] );
  pMsgWin->AddLinePaneIdx( paneTbl[1] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ作成
 *
 * @param   devMan    デバイスマネージャ
 * @param   lytID     レイアウトID
 * @param   paneTbl   文字描画に使用するペインのテーブル（２つ必要）
 * @param   wait      メッセージ表示ウェイト
 * @param   heap      ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageWindow(
      const gfl2::ui::DeviceManager * devMan,
      u32 lytID, const gfl2::lyt::LytPaneIndex * paneTbl, int wait, Heap * heap )
{
  pMsgWin = GFL_NEW( heap->GetDeviceHeap() ) print::MessageWindow(
                                    devMan,
                                    pLayoutWork[lytID].pLytWk,
                                    2,
                                    wait,
                                    heap->GetSystemHeap() );
  pMsgWin->AddLinePaneIdx( paneTbl[0] );
  pMsgWin->AddLinePaneIdx( paneTbl[1] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ作成  LayoutTextBoxポインタを渡す版
 *
 * @param   devMan    デバイスマネージャ
 * @param   lytID     レイアウトID
 * @param   paneTbl   文字描画に使用するペインのポインタテーブル（２つ必要）
 * @param   wait      メッセージ表示ウェイト
 * @param   heap      ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageWindow(
      const gfl2::ui::DeviceManager * devMan,
      u32 lytID,  gfl2::lyt::LytTextBox* pPane1, gfl2::lyt::LytTextBox *pPane2, int wait, Heap * heap )
{
  pMsgWin = GFL_NEW( heap->GetDeviceHeap() ) print::MessageWindow(
                                    devMan,
                                    pLayoutWork[lytID].pLytWk,
                                    2,
                                    wait,
                                    heap->GetSystemHeap() );
  pMsgWin->AddLinePaneIdxPtr( pPane1 );
  pMsgWin->AddLinePaneIdxPtr( pPane2 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ作成（ウェイト自動指定版）
 *
 * @param   devMan    デバイスマネージャ
 * @param   lytID     レイアウトID
 * @param   paneTbl   文字描画に使用するペインのテーブル（２つ必要）
 * @param   heap      ヒープ
 * @param   is60Fps   60FPSかどうか
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageWindow(
      const gfl2::ui::DeviceManager * devMan,
      u32 lytID, const gfl2::lyt::LytPaneIndex * paneTbl, Heap * heap, bool is60Fps)
{
  pMsgWin = GFL_NEW( heap->GetDeviceHeap() ) print::MessageWindow(
                                    devMan,
                                    pLayoutWork[lytID].pLytWk,
                                    2,
                                    heap->GetSystemHeap(),
                                    is60Fps );
  pMsgWin->AddLinePaneIdx( paneTbl[0] );
  pMsgWin->AddLinePaneIdx( paneTbl[1] );
}



//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ作成（ウェイト自動指定版）
 *
 * @param   devMan    デバイスマネージャ
 * @param   lytID     レイアウトID
 * @param   paneTbl   文字描画に使用するペインのテーブル
 * @param   paneNum   文字描画に使用するペインの数
 * @param   heap      ヒープ
 * @param   is60Fps   60FPSかどうか
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageWindow(
      const gfl2::ui::DeviceManager * devMan,
      u32 lytID, int painnum,const gfl2::lyt::LytPaneIndex * paneTbl, Heap * heap, bool is60Fps)
{
  pMsgWin = GFL_NEW( heap->GetDeviceHeap() ) print::MessageWindow(
                                    devMan, 
                                    pLayoutWork[lytID].pLytWk,painnum,
                                    heap->GetSystemHeap(),
                                    is60Fps );
  for(int a=0;a<painnum;a++){
    pMsgWin->AddLinePaneIdx( paneTbl[a] );
  }
}




//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ作成（ウェイト自動指定版）
 *
 * @param   devMan    デバイスマネージャ
 * @param   lytID     レイアウトID
 * @param   pPane1    テキストボックスペイン１
 * @param   pPane2    テキストボックスペイン２
 * @param   heap      ヒープ
 * @param   is60Fps   60FPSかどうか
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMessageWindow(
      const gfl2::ui::DeviceManager * devMan,
      u32 lytID,
      gfl2::lyt::LytTextBox * pPane1,
      gfl2::lyt::LytTextBox * pPane2,
      Heap * heap,
      bool is60Fps )
{
  pMsgWin = GFL_NEW( heap->GetDeviceHeap() ) print::MessageWindow(
                                    devMan,
                                    pLayoutWork[lytID].pLytWk,
                                    2,
                                    heap->GetSystemHeap(),
                                    is60Fps );
  pMsgWin->AddLinePaneIdxPtr( pPane1 );
  pMsgWin->AddLinePaneIdxPtr( pPane2 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteMessageWindow(void)
{
  if( pMsgWin != NULL )
  {
    pMsgWin->Clear();

    GFL_SAFE_DELETE( pMsgWin );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウ取得
 *
 * @param   none
 *
 * @return  メッセージウィンドウ取得
 */
//--------------------------------------------------------------------------------------------
print::MessageWindow * G2DUtil::GetMsgWin(void)
{
  return pMsgWin;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウにコールバック関数を設定
 *
 * @param   cb	コールバック関数
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMessageCallback( print::MsgCallback * cb )
{
  pMsgWin->SetMessageCallback( cb );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージカーソル作成
 *
 * @param   heap    ヒープ
 * @param   lytID   レイアウトID
 * @param   pane    カーソルペイン
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMsgCursor( Heap * heap, u32 lytID, gfl2::lyt::LytPaneIndex pane )
{
  gfl2::lyt::LytPane * pPane = pLayoutWork[lytID].pLytWk->GetPane( pane );
  this->CreateMsgCursor( heap, pPane );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージカーソル作成（部品指定版）
 *
 * @param   heap    ヒープ
 * @param   lytID   レイアウトID
 * @param   parts   親部品ペイン
 * @param   pane    カーソルペイン
 * @param   resIndex  リソースの登録インデックス(GetLayoutResourceIDに渡すインデックス)
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMsgCursor( Heap * heap, u32 lytID, gfl2::lyt::LytPaneIndex parts, gfl2::lyt::LytPaneIndex pane, u32 resIndex )
{
  gfl2::lyt::LytMultiResID & res_id = GetLayoutResourceID( resIndex );

  gfl2::lyt::LytPane * pPane = pLayoutWork[lytID].pLytWk->GetPane(
                                pLayoutWork[lytID].pLytWk->GetPartsPane(parts),
                                pane,
                                &res_id );

  this->CreateMsgCursor( heap, pPane );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージカーソル作成（ペインポインタ版）
 *
 * @param   heap    ヒープ
 * @param   pane    カーソルのペインポインタ。カーソルのペインは部品ペインになっているはずです。
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateMsgCursor( Heap* heap, gfl2::lyt::LytPane* pPane )
{
  pMsgCursor = GFL_NEW( heap->GetDeviceHeap() ) App::Tool::MsgCursor( pPane );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージカーソル削除
 *
 * @param   none
 *
 * @retval	"true = 削除完了"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool G2DUtil::DeleteMsgCursor(void)
{
  GFL_SAFE_DELETE( pMsgCursor );
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージカーソル表示非表示
 *
 * @param   bUser 自分でやる場合 true
 * @param   flg  onoff
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMsgCursorUserVisible(bool bUser, bool flg)
{
  GFL_ASSERT(pMsgCursor);
  pMsgCursor->SetVisibleUser( bUser );
  pMsgCursor->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージセット
 *
 * @param   str   文字列
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMessage( const gfl2::str::StrBuf * str )
{
  pMsgWin->SetString( *str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージ描画
 *
 * @param   none
 *
 * @return  処理結果
 */
//--------------------------------------------------------------------------------------------
gfl2::str::StrWin::Result G2DUtil::PrintMessage(void)
{
  gfl2::str::StrWin::Result result = gfl2::str::StrWin::RES_FINISH;

	if(pMsgWin)
	{
	  result = pMsgWin->Proc();
	  if( pMsgCursor != NULL && !pMsgCursor->GetVisibleUser() )
		{
			if( result == gfl2::str::StrWin::RES_PAUSE )
			{
        pMsgCursor->SetVisible( true );
			}
			else
			{
				pMsgCursor->SetVisible( false );
	    }
	  }
	}

  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージ終了モードを設定
 *
 * @param   type    終了タイプ
 * @param   frame   時間用フレーム
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetMessageFinishMode( gfl2::str::StrWin::FinishType type, u8 frame )
{
  pMsgWin->SetFinishMode( type, frame );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    SetTextboxPaneTagProcessor
 * @brief   タグプロセッサーの登録
 * @date    2015.02.26
 *
 * @param   lytID     レイアウトID
 * @param   pane      対象ペインのインデックス
 * @param   tag_proc  タグプロセッサへのポインタ
 *
 * @note  app_utilから移植
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetTextboxPaneTagProcessor(
      u32 lytID, gfl2::lyt::LytPaneIndex pane, nw::lyt::TextBox::TagProcessor * tag_proc )
{
  gfl2::lyt::LytTextBox * text_box = pLayoutWork[lytID].pLytWk->GetTextBoxPane( pane );
  if( text_box != NULL )
  {
    text_box->SetTagProcessor( tag_proc );
  }
  else
  {
    GFL_ASSERT( text_box );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テクスチャーリソースデータのアドレスを取得する
 *
 * @param   lytID     レイアウトID
 * @param   textureID テクスチャID
 *
 * @return  指定したテクスチャーのリソースアドレス
 */
//--------------------------------------------------------------------------------------------
#if 0   // TexResourceが解決できないためコメントアウト。必要になったら改めて考える。 2015.07.06 by nakahiro
void* G2DUtil::GetTextureResourceAddress( u32 lytID, u32 textureID )
{
#if 1
  // niji
  const gfl2::lyt::LytRes* lyt_res = pLayoutWork[lytID].pLytWk->GetResource();
  u32 size;
  return lyt_res->GetTextureResource(textureID, &size, pLayoutResID->GetMultiID());
  /*ビルドを通すため
  nw::lyt::TexResource         tex_resource(tex_res, size);

  return tex_resource.GetImageAddress();
  ビルドを通すため*/
#else
  // sango
  const gfl::grp::g2d::LytRes* lyt_res = pLayoutWork[lytID].pLytWk->GetResource();
  u32                          size;
  void*                        tex_res = lyt_res->GetTextureRes(textureID, &size);
  nw::lyt::TexResource         tex_resource(tex_res, size);
  return tex_resource.GetImageAddress();
#endif
}
#endif


//--------------------------------------------------------------------------------------------
/**
 * @func    CreateAppLytAccessor
 * @brief   applytファイルのアクセサを作成
 * @date    2015.02.25
 *
 * @param   heap  ヒープ
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreateAppLytAccessor( Heap * heap )
{
  m_pLytAccessor = GFL_NEW( heap->GetDeviceHeap() ) app::util::AppLytAccessor();
}

//--------------------------------------------------------------------------------------------
/**
 * @func    DeleteAppLytAccessor
 * @brief   applytファイルのアクセサを削除
 * @date    2015.02.25
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::DeleteAppLytAccessor(void)
{
  GFL_SAFE_DELETE( m_pLytAccessor );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetAppLytAccessor
 * @brief   applytファイルのアクセサを取得
 * @date    2015.02.25
 *
 * @return  applytファイルアクセサ
 */
//--------------------------------------------------------------------------------------------
app::util::AppLytAccessor * G2DUtil::GetAppLytAccessor(void)
{
  return m_pLytAccessor;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    SetLayoutWork
 * @brief   レイアウトをapplytファイルから自動生成
 * @date    2015.02.25
 *
 * @param   id                    生成するレイアウトの開始ID
 * @param   dat                   生成するレイアウトデータ
 * @param   num                   生成するレイアウト数
 * @param   allocator             アロケータ ( NULL = レイアウトシステムから確保 )
 * @param   dev_allocator         デバイスアロケータ ( NULL = レイアウトシステムから確保 )
 * @param   createPaneTbl         ペインテーブルを生成し検索時間を短縮するか
 * @param   createPaneTblWithHash ペインテーブル生成時にハッシュテーブルを作り生成時間を短縮するか
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::SetLayoutWork(
    u32 id,
    const LytwkData * dat,
    u32 num,
    gfl2::heap::NwAllocator * allocator,
    gfl2::heap::NwAllocator * dev_allocator,
    b32 createPaneTbl,
    b32 createPaneTblWithHash )
{
  for( u32 i=id; i<id+num; i++ )
  {
    GFL_ASSERT_STOP( i < lwkMax );

    const LytwkData * pld = &dat[i-id];
    m_pLytAccessor->SetAppLytBuf( pld->buf );

    u32 def_anm_max = m_pLytAccessor->GetAnimeMax( pld->lytres_id );

    // レイアウトワーク生成 ( + 親レイアウトのアニメ )
    if( allocator == NULL || dev_allocator == NULL )
    {
      pLayoutWork[i].pLytWk = pLayoutSystem->CreateLytWk(
                                &pLayoutResID[pld->res_index],
                                pld->lytres_id,
                                m_pLytAccessor->GetAnimeTable(pld->lytres_id),
                                def_anm_max,
                                pld->anm_max,
                                &LytWkDefaultSetup[pld->setupDisp],
                                false, createPaneTbl, createPaneTblWithHash );
    }
    else
    {
      pLayoutWork[i].pLytWk = pLayoutSystem->CreateLytWk(
                                allocator,
                                dev_allocator,
                                &pLayoutResID[pld->res_index],
                                pld->lytres_id,
                                m_pLytAccessor->GetAnimeTable(pld->lytres_id),
                                def_anm_max,
                                pld->anm_max,
                                &LytWkDefaultSetup[pld->setupDisp],
                                false, createPaneTbl, createPaneTblWithHash );
    }

    // 部品アニメを生成
    CreatePartsAnime( pLayoutWork[i].pLytWk, &pLayoutResID[pld->res_index], pld->lytres_id, &def_anm_max, NULL, 0 );

    pLayoutWork[i].pLytWk->SetUserEnv( pUpperLayoutEnv, pLowerLayoutEnv );  // ワークに関連付け
    pLayoutWork[i].pLytWk->SetAutoDrawFlag( false );                        // 個別表示へ

    pLayoutWork[i].pLytWk->SetInvisiblePaneCalculateMtx( pld->calculateMtxFlag );

    pLayoutWork[i].setupDisp = pld->setupDisp;
    pLayoutWork[i].enable    = pld->enable;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  描画中か
 *  @return  G2DUtil内で生成されたLytWkのうち１つでも描画中のものがあればtrue(LytWk１つずつ確認したい場合はGetLayoutWorkで取得して個別にLytWk::IsDrawingで確認して下さい)
 */
//----------------------------------------------------------------------------
bool G2DUtil::IsDrawing(void) const
{
  bool isDrawing = false;
  if(pLayoutWork)
  {
    for(u32 i=0; i<lwkMax; ++i)
    {
      if(pLayoutWork[i].pLytWk)
      {
        bool l_isDrawing = pLayoutWork[i].pLytWk->IsDrawing();
        if(l_isDrawing)
        {
          isDrawing = true;
          break;
        }
      }
    }
  }
  return isDrawing;
}


//--------------------------------------------------------------------------------------------
/**
 * @func       CTRCalcLytTextureInfoTexObject
 * @brief      CTR版レイアウトのTextureInfoに渡すtexObjectを計算する
 * @date       2015.08.03
 *
 * @param[in]  virtualAddr  テクスチャイメージの仮想アドレス
 * @return     texObjectの値
 * @attention  Win32版ではASSERTになります。ASSERTを抜けるとnw::lyt::TextureInfo::INVALID(無効値)を返します。
 */
//--------------------------------------------------------------------------------------------
u32 G2DUtil::CTRCalcLytTextureInfoTexObject(uptr virtualAddr)
{
  u32 texObject = gfl2::lyt::LytResTextureInfo::INVALID;

#if defined(GF_PLATFORM_CTR)
  {
    // NW_CTR/lyt2/demo/TexVRAM/src/TexVRAM.cpp
    // bool DemoApp::LoadTextureToVRAM(const void* pImgRes, u32 size, nw::lyt::TextureInfo* oTextureInfo)
    //
    // NW_CTR/lyt2/src/lyt/lyt_Util.cpp
    // bool
    // LoadTexture(TextureInfo* textureInfo, const void* pImgRes, u32 size)
    //
    // を参考にした。

    enum MemArea
    {
      MemArea_VRAMA = 1,
      MemArea_VRAMB = 2,
      MemArea_MASK = MemArea_VRAMA | MemArea_VRAMB
    };
 
    uptr vramAStartAddr = nn::gx::CTR::GetVramStartAddr(nn::gx::CTR::MEM_VRAMA);
    uptr vramAEndAddr   = nn::gx::CTR::GetVramEndAddr(nn::gx::CTR::MEM_VRAMA);
    
    uptr vramBStartAddr = nn::gx::CTR::GetVramStartAddr(nn::gx::CTR::MEM_VRAMB);
    uptr vramBEndAddr   = nn::gx::CTR::GetVramEndAddr(nn::gx::CTR::MEM_VRAMB);
  
    GFL_ASSERT((reinterpret_cast<u32>(virtualAddr) & MemArea_MASK) == 0);
 
    if(vramAStartAddr<=virtualAddr && virtualAddr<vramAEndAddr)
    {
      texObject = reinterpret_cast<u32>(virtualAddr) | (MemArea_VRAMA);
    }
    else if(vramBStartAddr<=virtualAddr && virtualAddr<vramBEndAddr)
    {
      texObject = reinterpret_cast<u32>(virtualAddr) | (MemArea_VRAMB);
    }
    // elseのときはFCRAM
  }
#elif defined(GF_PLATFORM_WIN32)
  {
    GFL_ASSERT(0);  // CTR版にしか対応していない関数
  }
#endif  // GF_PLATFORM_

  return texObject;
}


//--------------------------------------------------------------------------------------------
/**
 * @func       MakeTextureInfo
 * @brief      「gfxのテクスチャ」の内容を「レイアウトのテクスチャ情報」に反映する
 * @date       2015.08.19
 *
 * @param[out]  textureInfo  「レイアウトのテクスチャ情報」を受け取るオブジェクト。NULLを渡さないようにして下さい。
 * @parame[in]  texture      「gfxのテクスチャ」
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::MakeTextureInfo(gfl2::lyt::LytResTextureInfo* textureInfo, const gfl2::gfx::Texture* texture)
{
  GFL_ASSERT(textureInfo);  // 開発中に気付かせるためのASSERT。NULLはダメ。

#if defined(GF_PLATFORM_CTR)

  const gfl2::gfx::ctr::CTRTexture* tex = static_cast<const gfl2::gfx::ctr::CTRTexture*>(texture);

  // texObject
  uptr virtualAddr = tex->GetVirtualAddr();
  u32 texObject = app::util::G2DUtil::CTRCalcLytTextureInfoTexObject(virtualAddr);

  nw::lyt::TexSize texSize(tex->GetSurface()->GetSurfaceDesc().width, tex->GetSurface()->GetSurfaceDesc().height);
  nw::lyt::TexSize realTexSize(tex->GetSurface()->GetSurfaceDesc().bufferHeight, tex->GetSurface()->GetSurfaceDesc().bufferWidth);

  PicaDataTextureFormat picaFormat = tex->GetFormat();
  nw::lyt::TexFormat nwFormat = nw::lyt::TEXFORMAT_RGBA8;
  //nw::lyt::TexelRotation nwTexelRotation = nw::lyt::TEXELROTATION_NONE;  // 固定にせず、外から引数で渡してもらうほうがいいかなあ。
  nw::lyt::TexelRotation nwTexelRotation = nw::lyt::TEXELROTATION_ROTATE;
  switch(picaFormat)
  {
  case PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE                   : nwFormat = nw::lyt::TEXFORMAT_RGBA8;   break;
  case PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE                    : nwFormat = nw::lyt::TEXFORMAT_RGB8;    break;
  case PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_5_5_5_1          : nwFormat = nw::lyt::TEXFORMAT_RGB5A1;  break;
  case PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_SHORT_5_6_5             : nwFormat = nw::lyt::TEXFORMAT_RGB565;  break;
  case PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_4_4_4_4          : nwFormat = nw::lyt::TEXFORMAT_RGBA4;   break;
  case PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE        : nwFormat = nw::lyt::TEXFORMAT_LA8;     break;
  case PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_BYTE              : nwFormat = nw::lyt::TEXFORMAT_L8;      break;
  case PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_BYTE                  : nwFormat = nw::lyt::TEXFORMAT_A8;      break;
  case PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE_4_4_DMP: nwFormat = nw::lyt::TEXFORMAT_LA4;     break;
  case PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_4BITS_DMP         : nwFormat = nw::lyt::TEXFORMAT_L4;      break;
  case PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_4BITS_DMP             : nwFormat = nw::lyt::TEXFORMAT_A4;      break;
  case PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP                 : nwFormat = nw::lyt::TEXFORMAT_ETC1;    break;
  case PICA_DATA_TEXTURE_FORMAT_ETC1_ALPHA_RGB8_A4_NATIVE_DMP        : nwFormat = nw::lyt::TEXFORMAT_ETC1A4;  break;
  default: GFL_ASSERT(0); break;  // 開発中に気付かせるためのASSERT。対応していないフォーマット。
  }

  textureInfo->Set(
    texObject,
    tex->GetPhysicalAddr(),
    texSize,
    realTexSize,
    nwFormat,
    nwTexelRotation
  );

#elif defined(GF_PLATFORM_WIN32)
  
  const gfl2::gfx::winOpenGL::WinOpenGLTexture* tex = static_cast<const gfl2::gfx::winOpenGL::WinOpenGLTexture*>(texture);
  
  u32 texObject = tex->GetHandle();
  nw::lyt::TexSize texSize(tex->GetSurface()->GetSurfaceDesc().width, tex->GetSurface()->GetSurfaceDesc().height);
  
  gfl2::gfx::Format gfxFormat = tex->GetTextureDesc().format;
  nw::lyt::TexFormat nwFormat = nw::lyt::TEXFORMAT_RGBA8;
  switch(gfxFormat)
  {
  case gfl2::gfx::Format::R5G6B5  : nwFormat = nw::lyt::TEXFORMAT_RGB565; break;
  case gfl2::gfx::Format::X8R8G8B8: nwFormat = nw::lyt::TEXFORMAT_RGB8;   break;
  case gfl2::gfx::Format::A8R8G8B8: nwFormat = nw::lyt::TEXFORMAT_RGBA8;  break;
  case gfl2::gfx::Format::A1R5G5B5: nwFormat = nw::lyt::TEXFORMAT_RGB5A1; break;
  case gfl2::gfx::Format::A4R4G4B4: nwFormat = nw::lyt::TEXFORMAT_RGBA4;  break;
  default: GFL_ASSERT(0); break;  // 開発中に気付かせるためのASSERT。対応していないフォーマット。
  }
  
  textureInfo->Set(
      texObject,
      texSize,
      nwFormat
  );

#endif  // GF_PLATFORM_
}


//--------------------------------------------------------------------------------------------
/**
 * @func    CreatePartsAnime
 * @brief   部品アニメを生成
 * @date    2015.02.25
 *
 * @param   lytwk             レイアウトワーク
 * @param   res_id            リソースID
 * @param   lytres_id         レイアウトデータID ( *_BFLYT )
 * @param   start_anm_no      追加開始アニメ番号
 * @param   parent_parts      部品ペイン ( NULL = 親レイアウトからの呼び出し時 = NULL )
 * @param   recursive_count   再帰呼び出し回数 ( 最大回数 = CREATE_ANIME_RECURSIVE_LIMIT )
 *
 * @note  再帰が最大回数を超えるとアニメが生成されません
 */
//--------------------------------------------------------------------------------------------
void G2DUtil::CreatePartsAnime(
      gfl2::lyt::LytWk * lytwk,
      const gfl2::lyt::LytMultiResID * res_id,
      u32 lytres_id,
      u32 * start_anm_no,
      gfl2::lyt::LytParts * parent_parts,
      u32 recursive_count )
{
  // 再帰の限界をチェック
  if( recursive_count >= CREATE_ANIME_RECURSIVE_LIMIT )
  {
    GFL_ASSERT( 0 );
    return;
  }

  u32 parts_max = m_pLytAccessor->GetPartsMax( lytres_id );
  if( parts_max == 0 )
  {
    return;
  }

  const u16 * pane_list   = m_pLytAccessor->GetPartsPaneTable( lytres_id );
  const u16 * layout_list = m_pLytAccessor->GetPartsLayoutTable( lytres_id );

  for( u32 i=0; i<parts_max; i++ )
  {
    gfl2::lyt::LytParts * set_parts;
    if( parent_parts == NULL )
    {
      set_parts = lytwk->GetPartsPane( pane_list[i] );
    }
    else
    {
      set_parts = lytwk->GetPartsPane( parent_parts, pane_list[i], res_id );
    }

    u32 anime_max = m_pLytAccessor->GetAnimeMax( layout_list[i] );

    if( anime_max != 0 )
    {
      *start_anm_no = pLayoutSystem->CreatePartsLayoutAnime(
                        lytwk,
                        res_id,
                        *start_anm_no,
                        m_pLytAccessor->GetAnimeTable(layout_list[i]),
                        anime_max,
                        set_parts,
                        false,
                        true );
    }

    // 再帰
    CreatePartsAnime(
      lytwk,
      res_id,
      layout_list[i],
      start_anm_no,
      set_parts,
      recursive_count+1 );
  }
}


} // util
} // app
