#if PM_DEBUG
//=============================================================================
/**
 * @file    LauncherSub.cpp
 * @brief   デバッグ用ランチャー処理サブルーチン
 * @author  Hiroyuki Nakamura
 * @date    2015.02.05
 */
//=============================================================================
#include "LauncherSub.h"

#include <niji_conv_header/app/debug/launcher/DebugLauncher.h>
#include <niji_conv_header/app/debug/launcher/DebugLauncher_pane.h>
#include <niji_conv_header/app/debug/launcher/DebugLauncher_anim_list.h>

#include <base/include/gfl2_Singleton.h>

#include <GameSys/include/GameManager.h>
#include <System/include/PokemonVersion.h>
#include <System/include/GflUse.h>
#include <Print/include/PrintSystem.h>

#include <arc_def_index/arc_def.h>



GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(launcher)

// 項目縦ブロックのベースペインテーブル
static const gfl2::lyt::LytPaneIndex ListPane[] =
{
  PANENAME_LAUNCHER_LOWER_PANE_LIST_01,
  PANENAME_LAUNCHER_LOWER_PANE_LIST_02,
  PANENAME_LAUNCHER_LOWER_PANE_LIST_03,
  PANENAME_LAUNCHER_LOWER_PANE_LIST_04,
};

// 項目部品ペインテーブル
static const gfl2::lyt::LytPaneIndex PartsPaneTbl[] =
{
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_00,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_01,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_02,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_03,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_04,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_05,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_06,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_07,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_08,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_09,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_10,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_11,

  PANENAME_LAUNCHER_LOWER_PANE_PARTS_12,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_13,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_14,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_15,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_16,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_17,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_18,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_19,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_20,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_21,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_22,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_23,

  PANENAME_LAUNCHER_LOWER_PANE_PARTS_24,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_25,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_26,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_27,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_28,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_29,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_30,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_31,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_32,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_33,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_34,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_35,

  PANENAME_LAUNCHER_LOWER_PANE_PARTS_36,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_37,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_38,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_39,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_40,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_41,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_42,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_43,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_44,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_45,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_46,
  PANENAME_LAUNCHER_LOWER_PANE_PARTS_47,
};



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap            ヒープ
 * @param   dev_heap        デバイスヒープ
 * @param   allocator       アロケータ
 * @param   dev_allocator   デバイスアロケータ
 */
//-----------------------------------------------------------------------------
LauncherSub::LauncherSub( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * dev_heap, gfl2::heap::NwAllocator * allocator, gfl2::heap::NwAllocator * dev_allocator )
  : m_pUiDeviceManager( NULL )
  , m_pKey( NULL )
  , m_pButton( NULL )
  , m_pRenderingManager( NULL )
  , m_pLayoutSystem( NULL )
  , m_pUpperLayoutEnv( NULL )
  , m_pLowerLayoutEnv( NULL )
  , m_pLytMultiResID( NULL )
  , m_scrollCount( 0 )
  , m_eventListener( NULL )
{
  m_heap.Create( heap, dev_heap, allocator, dev_allocator, NULL, NULL );

  for( u32 i=0; i<LYTID_MAX; i++ )
  {
    m_pLytWk[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherSub::~LauncherSub(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupUI
 * @brief   UI関連作成
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherSub::SetupUI(void)
{
  m_pUiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

	m_pKey    = m_pUiDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
	m_pButton = m_pUiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsButtonTrigger
 * @brief   ボタンが押されたか
 * @date    2015.02.05
 *
 * @param   button  チェックするボタン
 *
 * @retval  true  = 押された
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool LauncherSub::IsButtonTrigger( u32 button )
{
  return m_pButton->IsTrigger( button );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateGraphicsSustem
 * @brief   グラフィックシステム作成
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherSub::CreateGraphicsSystem(void)
{
  m_pRenderingManager = GFL_NEW(m_heap.GetSystemHeap()) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }

  m_pRenderingManager->SyncCreate(
    desc,
    &m_heap,
    NULL,
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteGraphicsSustem
 * @brief   グラフィックシステム削除
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherSub::DeleteGraphicsSystem(void)
{
  GFL_SAFE_DELETE( m_pRenderingManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayout
 * @brief   レイアウト作成
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherSub::CreateLayout(void)
{
  { // システム生成
    gfl2::lyt::LytSysSetUp setup = {
					GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
      128,     // 管理リソース最大数
      10       // 管理レイアウトワーク最大数
    };
    m_pLayoutSystem = GFL_NEW(m_heap.GetSystemHeap())  gfl2::lyt::LytSys( &setup, m_heap.GetSystemAllocator(), m_heap.GetDeviceAllocator(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );
  }

  { // 環境設定
		// 射影設定（正射影）
		static const gfl2::lyt::ProjectionParam def_proj[] =
		{
			{ // 上画面
				gfl2::lyt::PROJECTION_ORTHO,
				{ -200.0f, 200.0f, -120.0f, 120.0f },
        -500.0f,
        500.0f,
				gfl2::math::PIVOT_UPSIDE_TO_TOP,
			},
			{ // 下画面
				gfl2::lyt::PROJECTION_ORTHO,
				{ -160.0f, 160.0f, -120.0f, 120.0f },
        -500.0f,
        500.0f,
				gfl2::math::PIVOT_UPSIDE_TO_TOP,
			}
		};
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
				gfl2::math::Vector3(0,0,1),  // カメラ座標
				gfl2::math::Vector3(0,1,0),     // 上方向
				gfl2::math::Vector3(0,0,0),  // ターゲット座標
			},
		};
    // 上画面
    m_pUpperLayoutEnv = GFL_NEW(m_heap.GetSystemHeap())  gfl2::lyt::Env();
//    m_pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, def_proj[0], def_view[0] );
//    m_pUpperLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, def_proj[0], def_view[1] );
    // 下画面
    m_pLowerLayoutEnv = GFL_NEW(m_heap.GetSystemHeap())  gfl2::lyt::Env();
    m_pLowerLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_NORMAL, def_proj[1], def_view[2] );
    m_pLowerLayoutEnv->SetCameraParam( gfl2::lyt::Env::CAMERA_RIGHT, def_proj[1], def_view[2] );
  }

  m_pLytMultiResID = GFL_NEW_ARRAY(m_heap.GetSystemHeap()) gfl2::lyt::LytMultiResID[RESID_MAX]();

  m_pLytMultiResID[RESID_BASE] = m_pLayoutSystem->InitMultiResource( m_heap.GetSystemHeap(), 1 );

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  { // オープン
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.fileName    = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    req.arcId       = ARCID_DEBUG_LAUNCHER;                         // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.heapForFile = m_heap.GetSystemHeap();                                      // ヒープ。ファイルインスタンスの作成と索引テーブルの作成に使用。Closeするまで左記のものが保持される。
    req.heapForReq  = m_heap.GetSystemHeap();                                      // リクエスト終了まで一時的に使用するヒープ。索引テーブルの作成のためにも使用する。ArcSrcのインスタンスやソーステーブルの作成のためにも使用する。
    req.result      = NULL;                                         // 結果。NULLでもよい。
    // ↓arcSrcCreateFlagがtrueのときのみ使われる
//    req->arcSrcFileName;  //!< [in]  ArcSrcのファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。heapForArcSrcがNULLのときは使われない。
//    req->heapForArcSrc;   //!< [in]  ヒープ。ArcSrcのインスタンスの作成とソーステーブルの作成に使用。Closeするまで左記のものが保持される。NULLのときはつくられない。
    // ↑arcSrcCreateFlagがtrueのときのみ使われる
    man->SyncArcFileOpen( req );
  }

  { // ロード
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.fileName = NULL;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    req.arcId = ARCID_DEBUG_LAUNCHER;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    req.datId = 0;              //!< [in]  アーカイブファイル内データID
    req.prio = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    req.ppBuf = &m_pBaseLayoutBuff;              //!< [out] heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
    req.pBufSize = NULL;           //!< [out] heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
    req.pRealReadSize = NULL;      //!< [out] 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
    req.heapForBuf = m_heap.GetDeviceHeap();         //!< [in]  ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
    req.align = 128;              //!< [in]  heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
    req.heapForReq = NULL;         //!< [in]  リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    req.heapForCompressed = m_heap.GetSystemHeap()->GetLowerHandle();  //!< [in]  解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
    req.result = NULL;             //!< [out] 結果。NULLでもよい。
    man->SyncArcFileLoadData( req );
  }

  { // クローズ
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.fileName   = NULL;                                        // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    req.arcId      = ARCID_DEBUG_LAUNCHER;                        // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    req.prio       = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL; // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.heapForReq = NULL;                                        // リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
    req.result     = NULL;                                        // 結果。NULLでもよい。
    man->SyncArcFileClose( req );
  }

  app::util::AppLytAccessor * accessor = GFL_NEW_LOW( m_heap.GetSystemHeap() ) app::util::AppLytAccessor();
  accessor->SetAppLytBuf( m_pBaseLayoutBuff );

  m_pLytMultiResID[RESID_BASE] = m_pLayoutSystem->LoadLytRes( &m_pLytMultiResID[RESID_BASE], m_heap.GetDeviceAllocator(), accessor->GetLytDat(), true, 0 );
  m_pLayoutSystem->TransferAllTexture( &m_pLytMultiResID[RESID_BASE] );

  { // レイアウトワーク生成
    // セットアップデータ
    static const gfl2::lyt::LytWkSetUp setup[] =
    {
      { // 上画面
        gfl2::math::Vector3(0,0,0),   // pos
        gfl2::math::Vector2(1,1),     // scale
        gfl2::math::Vector3(0,0,0),   // rotate
        1,                        // priority
        0,												// groupID
      },
      { // 下画面
        gfl2::math::Vector3(0,0,0),  // pos
        gfl2::math::Vector2(1,1),       // scale
        gfl2::math::Vector3(0,0,0),     // rotate
        1,                          // priority
        0,                          // groupID
      }
    };

    u32 def_anm_max = accessor->GetAnimeMax( LYTRES_DEBUGLAUNCHER_LAUNCHER_LOWER_BFLYT );
    m_pLytWk[LYTID_LOWER] = m_pLayoutSystem->CreateLytWk(
                              &m_pLytMultiResID[RESID_BASE],
                              LYTRES_DEBUGLAUNCHER_LAUNCHER_LOWER_BFLYT,
                              accessor->GetAnimeTable(LYTRES_DEBUGLAUNCHER_LAUNCHER_LOWER_BFLYT),
                              def_anm_max,
                              LA_DEBUGLAUNCHER_LAUNCHER_LOWER___NUM,
                              &setup[1],
                              false, true, true );
    // 部品アニメを生成
    CreatePartsAnime( accessor, m_pLytWk[LYTID_LOWER], &m_pLytMultiResID[RESID_BASE], LYTRES_DEBUGLAUNCHER_LAUNCHER_LOWER_BFLYT, &def_anm_max );

    for( u32 i=0; i<LYTID_MAX; i++ )
    {
      if( m_pLytWk[i] == NULL ){
        continue;
      }

      m_pLytWk[i]->SetUserEnv( m_pUpperLayoutEnv, m_pLowerLayoutEnv );  // ワークに関連付け
      m_pLytWk[i]->SetAutoDrawFlag( false );                        // 個別表示へ

      m_pLytWk[i]->SetInvisiblePaneCalculateMtx( false );
    }
  }

  GFL_DELETE accessor;
}

void LauncherSub::CreatePartsAnime(
      app::util::AppLytAccessor * accessor,
      gfl2::lyt::LytWk * lytwk,
      const gfl2::lyt::LytMultiResID * res_id,
      u32 lytres_id,
      u32 * start_anm_no )
{
  u32 parts_max = accessor->GetPartsMax( lytres_id );
  if( parts_max == 0 )
  {
    GFL_ASSERT( 0 );
    return;
  }

  const u16 * pane_list   = accessor->GetPartsPaneTable( lytres_id );
  const u16 * layout_list = accessor->GetPartsLayoutTable( lytres_id );

  for( u32 i=0; i<parts_max; i++ )
  {
    gfl2::lyt::LytParts * set_parts = lytwk->GetPartsPane( pane_list[i] );
    u32 anime_max = accessor->GetAnimeMax( layout_list[i] );

    if( anime_max != 0 )
    {
      *start_anm_no = m_pLayoutSystem->CreatePartsLayoutAnime(
                        lytwk,
                        res_id,
                        *start_anm_no,
                        accessor->GetAnimeTable(layout_list[i]),
                        anime_max,
                        set_parts,
                        false,
                        true );
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    DeleteLayout
 * @brief   レイアウト削除
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherSub::DeleteLayout(void)
{
  for( int i=0; i<LYTID_MAX; i++ ){
    if( m_pLytWk[i] != NULL ){
      m_pLayoutSystem->RemoveLytWk( m_pLytWk[i] );
      m_pLytWk[i] = NULL;
    }
  }

  for( u32 i=0; i<RESID_MAX; i++ ){
    if( m_pLytMultiResID[i].IsEnable() != false ){
       m_pLayoutSystem->ReleaseLytRes( &m_pLytMultiResID[i] );
    }
  }

  GflHeapFreeMemory( m_pBaseLayoutBuff );

  GFL_SAFE_DELETE_ARRAY( m_pLytMultiResID );
  GFL_SAFE_DELETE( m_pUpperLayoutEnv );
  GFL_SAFE_DELETE( m_pLowerLayoutEnv );
  GFL_SAFE_DELETE( m_pLayoutSystem );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateLayout
 * @brief   レイアウト更新
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherSub::UpdateLayout(void)
{
  if( m_pRenderingManager == NULL )
  {
    return;
  }

  m_pRenderingManager->Update();

  if( m_pLytWk[LYTID_UPPER] != NULL )
  {
    m_pLytWk[LYTID_UPPER]->Update();
  }
  if( m_pLytWk[LYTID_LOWER] != NULL )
  {
    m_pLytWk[LYTID_LOWER]->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDraw
 * @brief   描画下準備
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void LauncherSub::PreDraw(void)
{
  if( m_pRenderingManager == NULL )
  {
    return;
  }
  m_pRenderingManager->PreDraw();
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawLayout
 * @brief   レイアウト描画
 * @date    2015.02.05
 *
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void LauncherSub::DrawLayout( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pRenderingManager == NULL )
  {
    return;
  }

  if( m_pLytWk[LYTID_UPPER] != NULL )
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
    {
      m_pRenderingManager->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pLytWk[LYTID_UPPER] );
    }
  }
  if( m_pLytWk[LYTID_LOWER] != NULL )
  {
    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      m_pRenderingManager->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pLytWk[LYTID_LOWER] );
    }
  }
  m_pRenderingManager->Draw( displayNo );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsLayoutDrawing
 * @brief   レイアウト描画中か
 * @date    2015.08.21
 *
 * @retval  true  = 描画中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool LauncherSub::IsLayoutDrawing(void)
{
  for( int i=0; i<LYTID_MAX; i++ )
  {
    if( m_pLytWk[i] != NULL )
    {
      if( m_pLytWk[i]->IsDrawing() != false )
      {
        return true;
      }
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartAnime
 * @brief   アニメ開始
 * @date    2015.02.05
 *
 * @param   lyt   レイアウトID
 * @param   anm   アニメID
 */
//-----------------------------------------------------------------------------
void LauncherSub::StartAnime( LayoutID lyt, u32 anm )
{
  if( m_pLytWk[lyt]->IsBindAnime(anm) == false )
  {
    m_pLytWk[lyt]->BindAnime( anm, true );
  }
  m_pLytWk[lyt]->SetAutoAnime( anm, true );
  m_pLytWk[lyt]->SetAnimeFrame( anm, 0.0f );
}

//-----------------------------------------------------------------------------
/**
 * @func    StopAnime
 * @brief   アニメ停止
 * @date    2015.02.05
 *
 * @param   lyt   レイアウトID
 * @param   anm   アニメID
 */
//-----------------------------------------------------------------------------
void LauncherSub::StopAnime( LayoutID lyt, u32 anm )
{
  if( m_pLytWk[lyt]->IsBindAnime(anm) != false )
  {
    m_pLytWk[lyt]->UnBindAnime( anm );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsAnimeEnd
 * @brief   アニメ終了チェック
 * @date    2015.02.05
 *
 * @param   lyt   レイアウトID
 * @param   anm   アニメID
 */
//-----------------------------------------------------------------------------
bool LauncherSub::IsAnimeEnd( LayoutID lyt, u32 anm )
{
  return m_pLytWk[lyt]->IsAnimeEnd( anm );
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcCursorPos
 * @brief   全体のカーソル位置を計算して取得
 * @date    2015.02.05
 *
 * @param   page  ページ番号
 * @param   pos   カーソル表示位置
 *
 * @return  全体のカーソル位置
 */
//-----------------------------------------------------------------------------
u32 LauncherSub::CalcCursorPos( s16 page, s16 pos )
{
  return ( page * ITEM_V_MAX + pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveCursor
 * @brief   カーソル移動
 * @date    2015.02.05
 *
 * @param   page      ページ番号
 * @param   pos       カーソル表示位置
 * @param   item_max  項目最大数
 *
 * @retval  true  = ページスクロール
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool LauncherSub::MoveCursor( s16 * page, s16 * pos, u32 item_max )
{
  s16 old_page = *page;
  s16 old_pos  = *pos;

  if( m_pKey->IsRepeat( gfl2::ui::DIR_UP ) )
  {
    if( ( *pos % ITEM_V_MAX ) == 0 )
    {
      *pos += ( ITEM_V_MAX - 1 );
    }
    else
    {
      *pos -= 1;
    }
  }
  else if( m_pKey->IsRepeat( gfl2::ui::DIR_DOWN ) )
  {
    if( ( *pos % ITEM_V_MAX ) == ( ITEM_V_MAX - 1 ) )
    {
      *pos -= ( ITEM_V_MAX - 1 );
    }
    else
    {
      *pos += 1;
    }
  }
  else if( m_pKey->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    if( *pos >= ITEM_V_MAX )
    {
      *pos -= ITEM_V_MAX;
    }
    else if( *page != 0 )
    {
      *page -= 1;
    }
  }
  else if( m_pKey->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( *pos < ITEM_V_MAX )
    {
      *pos += ITEM_V_MAX;
    }
    else
    {
      s16 page_max;
      if( item_max <= ITEM_MAX )
      {
        page_max = 0;
      }
      else
      {
        page_max = ( item_max - ITEM_MAX ) / ITEM_V_MAX;
        if( ( item_max % ITEM_V_MAX ) != 0 )
        {
          page_max += 1;
        }
      }
      if( page_max != 0 && *page != page_max )
      {
        *page += 1;
      }
    }
  }

  if( *page > old_page )
  {
    InitScroll( *page, *pos, 1 );
    return true;
  }
  else if( *page < old_page )
  {
    InitScroll( *page, *pos, -1 );
    return true;
  }
  else if( *pos != old_pos )
  {
    // カーソル位置変更
    SetCursorAnime( old_pos, false );
    SetCursorAnime( *pos, true );
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCursorAnime
 * @brief   カーソルアニメ設定
 * @date    2015.02.05
 *
 * @param   pos   カーソル表示位置
 * @param   mode  ON = true, OFF = false
 *
 * @retval  true  = ページスクロール
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
void LauncherSub::SetCursorAnime( u32 pos, bool mode )
{
  if( pos >= ITEM_V_MAX )
  {
    pos = ( pos % ITEM_V_MAX + ( m_scrollCount + 1 ) % SCROLL_MAX * ITEM_V_MAX ) * LIST_ITEM_ANM_MAX;
  }
  else
  {
    pos = ( pos % ITEM_V_MAX + m_scrollCount % SCROLL_MAX * ITEM_V_MAX ) * LIST_ITEM_ANM_MAX;
  }

  u32 start, stop;
  if( mode != false )
  {
    start = LA_DEBUGLAUNCHER_LAUNCHER_LOWER__PARTS_00_ON + pos;
    stop  = LA_DEBUGLAUNCHER_LAUNCHER_LOWER__PARTS_00_OFF + pos;
  }
  else
  {
    start = LA_DEBUGLAUNCHER_LAUNCHER_LOWER__PARTS_00_OFF + pos;
    stop  = LA_DEBUGLAUNCHER_LAUNCHER_LOWER__PARTS_00_ON + pos;
  }
  StopAnime( LYTID_LOWER, stop );
  StartAnime( LYTID_LOWER, start );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEndScrollAnime
 * @brief   スクロールアニメが終了したか
 * @date    2015.02.05
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool LauncherSub::IsEndScrollAnime(void)
{
  if( IsAnimeEnd( LYTID_LOWER, LA_DEBUGLAUNCHER_LAUNCHER_LOWER_LEFT ) != false &&
      IsAnimeEnd( LYTID_LOWER, LA_DEBUGLAUNCHER_LAUNCHER_LOWER_RIGHT ) != false )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitScroll
 * @brief   ページスクロール初期設定
 * @date    2015.02.05
 *
 * @param   page        ページ
 * @param   cursor_pos  カーソル表示位置
 * @param   mv          スクロール値
 */
//-----------------------------------------------------------------------------
void LauncherSub::InitScroll( s16 page, s16 cursor_pos, s16 mv )
{
  SetCursorAnime( cursor_pos, false );

  static const gfl2::lyt::LytPaneIndex pos_pane[] =
  {
    PANENAME_LAUNCHER_LOWER_PANE_POS_01,
    PANENAME_LAUNCHER_LOWER_PANE_POS_02,
    PANENAME_LAUNCHER_LOWER_PANE_POS_03,
  };

  // 指定位置へ移動
  s32 put = m_scrollCount;
  for( int i=0; i<SCROLL_MAX-1; i++ )
  {
    gfl2::lyt::LytPane * pane = m_pLytWk[LYTID_LOWER]->GetPane( pos_pane[i] );
    gfl2::math::VEC3 pos = pane->GetTranslate();

    pane = m_pLytWk[LYTID_LOWER]->GetPane( ListPane[put] );
    pane->SetTranslate( pos );

    put++;
    if( put == SCROLL_MAX )
    {
      put = 0;
    }
  }

  { // スクロールで入る部分にペインを配置
    gfl2::lyt::LytPane * pane;
    if( mv >= 0 )
    {
      pane = m_pLytWk[LYTID_LOWER]->GetPane( PANENAME_LAUNCHER_LOWER_PANE_POS_04 );
      StartAnime( LYTID_LOWER, LA_DEBUGLAUNCHER_LAUNCHER_LOWER_LEFT );
      PrintItem( page+2, put );
    }
    else
    {
      pane = m_pLytWk[LYTID_LOWER]->GetPane( PANENAME_LAUNCHER_LOWER_PANE_POS_00 );
      StartAnime( LYTID_LOWER, LA_DEBUGLAUNCHER_LAUNCHER_LOWER_RIGHT );
      PrintItem( page, put );
    }
    gfl2::math::VEC3 pos = pane->GetTranslate();

    pane = m_pLytWk[LYTID_LOWER]->GetPane( ListPane[put] );
    pane->SetTranslate( pos );
  }

  m_scrollCount += mv;
  if( m_scrollCount < 0 )
  {
    m_scrollCount = SCROLL_MAX - 1;
  }
  else if( m_scrollCount >= SCROLL_MAX )
  {
    m_scrollCount = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetListPos
 * @brief   リスト表示位置を再設定
 * @date    2015.02.05
 *
 * @param   cursor_pos  カーソル表示位置
 */
//-----------------------------------------------------------------------------
void LauncherSub::ResetListPos( u32 cursor_pos )
{
  gfl2::math::VEC3 pos[SCROLL_MAX];
  for( u32 i=0; i<SCROLL_MAX; i++ )
  {
    const gfl2::lyt::LytPane * pane = m_pLytWk[LYTID_LOWER]->GetPane( ListPane[i] );
    const gfl2::math::MTX34 & mtx = pane->GetGlobalMtx();
    pos[i].x = mtx._03;
    pos[i].y = mtx._13;
    pos[i].z = mtx._23;
  }

  { // 親をデフォルト位置へ
    gfl2::math::VEC3 def_pos( 0.0f, 0.0f, 0.0f );
    gfl2::lyt::LytPane * pane = m_pLytWk[LYTID_LOWER]->GetPane( PANENAME_LAUNCHER_LOWER_PANE_BASE );
    pane->SetTranslate( def_pos );
  }

  for( u32 i=0; i<SCROLL_MAX; i++ )
  {
    gfl2::lyt::LytPane * pane = m_pLytWk[LYTID_LOWER]->GetPane( ListPane[i] );
    pane->SetTranslate( pos[i] );
  }

  SetCursorAnime( cursor_pos, true );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateMessage
 * @brief   メッセージデータ作成
 * @date    2015.02.18
 *
 * @param   id        アークデータID
 * @param   list      項目データ
 * @param   list_max  項目データサイズ
 */
//-----------------------------------------------------------------------------
void LauncherSub::CreateMessage( gfl2::fs::ArcFile::ARCDATID id, const u32 * list, u32 list_max )
{
  m_pMsgData = GFL_NEW(m_heap.GetSystemHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(), id, m_heap.GetSystemHeap(), gfl2::str::MsgData::LOAD_FULL );

  m_itemStr = list;
  m_itemStrMax = list_max;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMessage
 * @brief   メッセージデータ削除
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherSub::DeleteMessage(void)
{
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateStrBuf
 * @brief   文字列バッファ作成
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherSub::CreateStrBuf(void)
{
  m_pStrBuf = GFL_NEW(m_heap.GetSystemHeap()) gfl2::str::StrBuf( 256, m_heap.GetSystemHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteStrBuf
 * @brief   文字列バッファ削除
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherSub::DeleteStrBuf(void)
{
  GFL_SAFE_DELETE( m_pStrBuf );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintItem
 * @brief   項目書き込み
 * @date    2015.02.18
 *
 * @param   page    ページ番号
 * @param   group   書き込むペインのグループ
 */
//-----------------------------------------------------------------------------
void LauncherSub::PrintItem( u32 page, u32 group )
{
  const gfl2::lyt::LytPaneIndex * pane = &PartsPaneTbl[group*ITEM_V_MAX];

  for( u32 i=0; i<ITEM_V_MAX; i++ )
  {
    u32 pos = page * ITEM_V_MAX + i;
    if( pos < m_itemStrMax )
    {
      m_pMsgData->GetString( m_itemStr[pos], *m_pStrBuf );
    }
    else
    {
      static const gfl2::str::STRCODE * str = L"-----------";
      m_pStrBuf->SetStr( str );
    }

    if( m_eventListener )
    {
      m_eventListener->OnPrintString( pos, m_pStrBuf );
    }

    gfl2::lyt::LytParts * parts_pane = m_pLytWk[LYTID_LOWER]->GetPartsPane( pane[i] );
    gfl2::lyt::LytTextBox * text_box = m_pLytWk[LYTID_LOWER]->GetTextBoxPane( parts_pane, PANENAME_LAUNCHER_ITEM_PANE_TEXTBOX_00, &m_pLytMultiResID[RESID_BASE] );
    print::SetStringForLytTextBox( text_box, m_pStrBuf );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPrintItem
 * @brief   初期項目書き込み
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherSub::InitPrintItem(void)
{
  u32 group = m_scrollCount;
  for( u32 i=0; i<3; i++ )
  {
    if( group >= SCROLL_MAX )
    {
      group = 0;
    }
    PrintItem( i, group );
    group++;
  }
}


GFL_NAMESPACE_END(launcher)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
