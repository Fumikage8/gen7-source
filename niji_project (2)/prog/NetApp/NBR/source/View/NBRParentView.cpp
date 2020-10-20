// ============================================================================
/*
 * @file NBRParentView.cpp
 * @brief 通信対戦受付アプリ 親ビュー
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/View/NBRParentView.h"
#include "NetApp/NBR/source/NBRResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

// コンストラクタ
NBRParentView::NBRParentView( NetAppLib::System::ApplicationWorkBase* pWorkBase ) :
  app::ui::UIView( pWorkBase->GetAppHeap(), pWorkBase->GetAppHeap()->GetSystemAllocator(), pWorkBase->GetAppHeap()->GetDeviceAllocator() ),
  m_pWork( pWorkBase ),
  m_pHeap( pWorkBase->GetAppHeap() ),
  m_layoutWork(),
  m_pAppRenderingManager( NULL ),
  m_pUpperViewVs( NULL ),
  m_pUpperViewPlayerInfoSimple( NULL ),
  m_pUpperViewPlayerInfoDefault( NULL ),
  m_pUpperViewRegulation( NULL ),
  m_pLowerViewReturnButton( NULL ),
  m_pLowerView2Button( NULL ),
  m_pLowerViewTeamSelect( NULL ),
  m_pLowerViewMultiSelect( NULL ),
  m_pLowerViewMessageMenu( NULL ),
  m_pNowUpperSubView( NULL ),
  m_pNowLowerSubView( NULL )
{
}

// デストラクタ
NBRParentView::~NBRParentView()
{
  deleteSubView();
  Delete2D();
  DeleteGraphicsSystem();
}

//--------------------------------------------------------------------------
/**
 * @brief 初期化
 * @param pResouceManager   リソースマネージャ
 */
//--------------------------------------------------------------------------
void NBRParentView::Initialize( NetAppLib::System::ResourceManager*        pResourceManager,
                                app::util::AppRenderingManager*            pAppRenderingManager,
                                NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility )
{
  m_pAppRenderingManager = pAppRenderingManager;
  setupLayoutSystem( pResourceManager );
  createSubView( netAppTrainerIconUtility );

  // BGアニメ再生
  GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_NBR_NBR_MIN_UPP_000__BG_00_BG_LOOP_00 );
  GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_LOWER, LA_NBR_NBR_SLC_LOW_000__BG_00_BG_LOOP_00 );
}

//--------------------------------------------------------------------------
/**
 * @brief 初期化待ち
 * @return trueで終了
 */
//--------------------------------------------------------------------------
bool NBRParentView::IsInitialize()
{
  if( m_pLowerViewMessageMenu->IsInitialized() )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 更新
 */
//--------------------------------------------------------------------------
void NBRParentView::Update( void )
{
  app::ui::UIView::Update();
}

//--------------------------------------------------------------------------
/**
 * @brief 描画
 */
//--------------------------------------------------------------------------
void NBRParentView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}

//--------------------------------------------------------------------------
/**
 *  @brief  描画中か
 *  @return  UIViewのメンバのG2DUtil内で生成されたLytWkのうち１つでも描画中のものがあればtrue(LytWk１つずつ確認したい場合はGetLayoutWorkで取得して個別にLytWk::IsDrawingで確認して下さい)
 */
//--------------------------------------------------------------------------
bool NBRParentView::IsDrawing() const
{
  /*
   *  全てのサブビューが破棄できる状態にあるかチェック
   */
  if( m_pUpperViewVs->IsDrawing() ||
      m_pUpperViewPlayerInfoSimple->IsDrawing() ||
      m_pUpperViewPlayerInfoDefault->IsDrawing() ||
      m_pUpperViewRegulation->IsDrawing() ||
      m_pLowerViewReturnButton->IsDrawing() ||
      m_pLowerView2Button->IsDrawing() ||
      m_pLowerViewTeamSelect->IsDrawing() ||
      m_pLowerViewMultiSelect->IsDrawing()  ||
      UIView::IsDrawing()
    )
  {
    ICHI_PRINT("ビューの破棄チェック待ち\n");
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 上画面サブビューの変更（=AddSubView)
 * @param viewId   画面ID
 */
//--------------------------------------------------------------------------
void NBRParentView::ChangeUpperSubView( UpperViewID viewId )
{
  // 既に登録されているSubViewがあったら交換する
  if( m_pNowUpperSubView != NULL )
  {
    m_pNowUpperSubView->SetVisible( false );
    m_pNowUpperSubView->RemoveFromSuperView();
  }

  NBRUpperViewBase* pSubView = NULL;
  switch( viewId ){
  case UPPER_VIEW_PLAYER_INFO_DEFAULT: ///< プレイヤー情報（デフォルト）
    pSubView = static_cast<NBRUpperViewBase*>(m_pUpperViewPlayerInfoDefault);
    ICHI_PRINT("NBR上画面変更 PLAYER_INFO_DEFAULT\n");
    break;
  case UPPER_VIEW_PLAYER_INFO_SIMPLE:  ///< プレイヤー情報（ルール未定）
    pSubView = static_cast<NBRUpperViewBase*>(m_pUpperViewPlayerInfoSimple);
    ICHI_PRINT("NBR上画面変更 PLAYER_INFO_SIMPLE\n");
    break;
  case UPPER_VIEW_VS:                  ///< VS画面
    pSubView = static_cast<NBRUpperViewBase*>(m_pUpperViewVs);
    ICHI_PRINT("NBR上画面変更 VS\n");
    break;
  case UPPER_VIEW_REGULATION:
    pSubView = static_cast<NBRUpperViewBase*>(m_pUpperViewRegulation);
    ICHI_PRINT("NBR上画面変更 REGULATION\n");
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

  m_pNowUpperSubView = pSubView;
  AddSubView( pSubView );
  m_pNowUpperSubView->SetVisible( true );
}
//--------------------------------------------------------------------------
/**
 * @brief 下画面サブビューの変更（=AddSubView)
 * @param viewId   画面ID
 */
//--------------------------------------------------------------------------
void NBRParentView::ChangeLowerSubView( LowerViewID viewId )
{
  // 既に登録されているSubViewがあったら交換する
  if( m_pNowLowerSubView != NULL )
  {
    m_pNowLowerSubView->SetVisible( false );
    m_pNowLowerSubView->RemoveFromSuperView();
  }

  NBRLowerViewBase* pSubView = NULL;
  switch( viewId ){
  case LOWER_VIEW_RETURN_BUTTON:       ///< 戻るボタンのみ
    pSubView = static_cast<NBRLowerViewBase*>(m_pLowerViewReturnButton);
    ICHI_PRINT("NBR下画面変更 RETURN_BUTTON\n");
    break;
  case LOWER_VIEW_2BUTTON:             ///< 2ボタン
    pSubView = static_cast<NBRLowerViewBase*>(m_pLowerView2Button);
    ICHI_PRINT("NBR下画面変更 2BUTTON\n");
    break;
  case LOWER_VIEW_TEAM_SELECT:         ///< バトルチーム選択
    pSubView = static_cast<NBRLowerViewBase*>(m_pLowerViewTeamSelect);
    ICHI_PRINT("NBR下画面変更 TEAM_SELECT\n");
    break;
  case LOWER_VIEW_MULTI_SELECT:        ///< マルチ・プレイヤー選択
    pSubView = static_cast<NBRLowerViewBase*>(m_pLowerViewMultiSelect);
    ICHI_PRINT("NBR下画面変更 MULTI_SELECT\n");
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

  m_pNowLowerSubView = pSubView;
  AddSubView( pSubView );
  m_pNowLowerSubView->SetVisible( true );

  // メッセージメニューを繋ぎ直す。
  // メッセージメニューを繋ぎ直す事で、最初に実行されるUIViewになる。でモーダルにする。
  m_pLowerViewMessageMenu->RemoveFromSuperView();
  AddSubView( m_pLowerViewMessageMenu );
}

//--------------------------------------------------------------------------
/**
 * @brief 下画面共通ビューを取得
 */
//--------------------------------------------------------------------------
NBRLowerViewBase* NBRParentView::GetLowerViewBase()
{
  GFL_ASSERT(m_pNowLowerSubView);
  return m_pNowLowerSubView;
}

//--------------------------------------------------------------------------
/**
 * @brief レイアウト作成
 * @param pResouceManager   リソースマネージャ
 */
//--------------------------------------------------------------------------
void NBRParentView::setupLayoutSystem( NetAppLib::System::ResourceManager* pResourceManager )
{
  void* pLayoutBuffer = pResourceManager->GetResourceBuffer( NetApp::NBR::NBR_RESOURCE_ID_NBR );

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  const app::util::G2DUtil::LytResData layoutResourceTable[] =
  {
    { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
  };
  
  const app::util::G2DUtil::LytwkData layoutWorkTable[] =
  {
    // 上画面
    {
      0,
      LYTRES_NBR_NBR_MIN_UPP_000_BFLYT,
      LA_NBR_NBR_MIN_UPP_000___NUM,
      pLayoutBuffer,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    // 下画面
    {
      0,
      LYTRES_NBR_NBR_SLC_LOW_000_BFLYT,
      LA_NBR_NBR_SLC_LOW_000___NUM,
      pLayoutBuffer,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  Create2D( m_pHeap, NULL, LAYOUT_WORK_ID_MAX,
            layoutResourceTable, GFL_NELEMS(layoutResourceTable),
            layoutWorkTable, GFL_NELEMS(layoutWorkTable),
            GARC_message_net_battle_reception_DAT );

  // 子ビューに渡すようにレイアウトワークを作る
  for( int i=0; i<LAYOUT_WORK_ID_MAX; i++ )
  {
    m_layoutWork[i].pLytWk = m_g2dUtil->GetLayoutWork(i);
    m_layoutWork[i].setupDisp = ( i==LAYOUT_WORK_ID_UPPER ? app::util::G2DUtil::SETUP_UPPER : app::util::G2DUtil::SETUP_LOWER);
    m_layoutWork[i].enable = true;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief サブビュー作成
 */
//--------------------------------------------------------------------------
void NBRParentView::createSubView( NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility )
{
  // 上画面
  m_pUpperViewVs                = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRUpperViewVs(               m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_UPPER ], netAppTrainerIconUtility );
  m_pUpperViewPlayerInfoSimple  = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRUpperViewPlayerInfoSimple( m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_UPPER ], netAppTrainerIconUtility ); 
  m_pUpperViewPlayerInfoDefault = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRUpperViewPlayerInfoDefault(m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_UPPER ], netAppTrainerIconUtility ); 
  m_pUpperViewRegulation        = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRUpperViewRegulation(       m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_UPPER ], netAppTrainerIconUtility ); 
  // 下画面
  m_pLowerViewReturnButton      = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRLowerViewReturnButton(     m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_LOWER ] );
  m_pLowerView2Button           = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRLowerView2Button(          m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_LOWER ] );
  m_pLowerViewTeamSelect        = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRLowerViewTeamSelect(       m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_LOWER ] );
  m_pLowerViewMultiSelect       = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRLowerViewMultiSelect(      m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_LOWER ] );
  m_pLowerViewMessageMenu       = GFL_NEW( m_pHeap->GetDeviceHeap() ) NBRLowerViewMessageMenu(      m_pWork, GetG2DUtil(), &m_layoutWork[ LAYOUT_WORK_ID_LOWER ] );

  // メッセージのビューは常に存在する
  AddSubView( m_pLowerViewMessageMenu );
}

//--------------------------------------------------------------------------
/**
 * @brief サブビュー削除
 */
//--------------------------------------------------------------------------
void NBRParentView::deleteSubView()
{
  //RemoveAllSubView();
  if( m_pNowUpperSubView != NULL ) { m_pNowUpperSubView->RemoveFromSuperView(); }
  if( m_pNowLowerSubView != NULL ) { m_pNowLowerSubView->RemoveFromSuperView(); }
  m_pLowerViewMessageMenu->RemoveFromSuperView();

  GFL_SAFE_DELETE( m_pLowerViewReturnButton );
  GFL_SAFE_DELETE( m_pLowerView2Button );
  GFL_SAFE_DELETE( m_pLowerViewTeamSelect );
  GFL_SAFE_DELETE( m_pLowerViewMultiSelect );
  GFL_SAFE_DELETE( m_pLowerViewMessageMenu );

  GFL_SAFE_DELETE( m_pUpperViewVs );
  GFL_SAFE_DELETE( m_pUpperViewPlayerInfoSimple );
  GFL_SAFE_DELETE( m_pUpperViewPlayerInfoDefault );
  GFL_SAFE_DELETE( m_pUpperViewRegulation );
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
