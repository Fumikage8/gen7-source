// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionFrame.cpp
 * @brief ジョインフェスタ下画面のアトラクション中専用フレームです。
 * @date 2015.11.17
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAttractionFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h" // リクエストID

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenAttractionFrame::JoinFestaSubScreenAttractionFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenAttractionFrame::~JoinFestaSubScreenAttractionFrame()
{
}

//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   このフレームの起動処理を記述します。true で終了
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenAttractionFrame::startup()
{
  m_pParentView->DisableAllView();

  // 共通BG表示
  m_pParentView->GetBgLowerView()->SetVisible( true );

  // 選択肢ビューをリタイアで設定（表示はしない）
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_ATTRACTION_END );

  // アトラクション中画面に変更
  // @fix GFNMCat[784]：ここで入力許可を呼んではいけない
  m_pParentView->GetAttractionLowerView()->SetVisible( true );
  m_pParentView->GetSelectLowerView()->SetVisible( false );

  // 残り時間初期値
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
    View::JoinFestaSubScreenAttractionLowerView* pView = m_pParentView->GetAttractionLowerView();
#ifdef  GF_PLATFORM_CTR
    const int second = pAttractionManager->GetTimeLimitFrame() / FRAME_PER_SECOND; //秒数変換
#else
    const int second = pAttractionManager->GetTimeLimitFrame() / 30; //秒数変換
#endif
    pView->SetTimeLimit( second/60, second%60 );
    // 参加人数
    pView->SetJoinNum( pAttractionManager->GetMemberNum() );
    // 個人スコア
    pView->SetMyScore( 0 );
    // トータルスコア
    pView->SetTotalScore( 0 );
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionFrame::setListener()
{
  m_pParentView->GetAttractionLowerView()->SetListener( this );
  m_pParentView->GetSelectLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionFrame::removeListener()
{
  m_pParentView->GetAttractionLowerView()->SetInputEnabled( false ); // ダブルタッチ修正
  m_pParentView->GetSelectLowerView()->SetInputEnabled( false ); // ダブルタッチ修正
  m_pParentView->GetAttractionLowerView()->GetButtonManager()->SetInputEnable( false );
  m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false );
  m_pParentView->GetAttractionLowerView()->RemoveListener();
  m_pParentView->GetSelectLowerView()->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionFrame::updateSequence()
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  // リタイアリクエストが受理され、アトラクションリタイアイベントが呼ばれたら
  if( pAttractionManager->IsRetireRequest() )
  {
    // アトラクション終了フレームにいく
    exitFrame( ATTRACTION_RETIRE );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenAttractionFrame::cleanup()
{
  return true;
}

//-----------------------------------------------------------------------------
// JoinFestaSubScreenAttractionLowerInputListener
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   中止ボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionFrame::OnStopButtonAction()
{
  ICHI_PRINT("Attraction::OnStopButtonAction \n");

  // 中止画面に変更
  ChangeStopView();
}

//-----------------------------------------------------------------------------
// JoinFestaSubScreenMainLowerInputListener
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//--------------------------------------------------------------------------
// @override
void JoinFestaSubScreenAttractionFrame::OnButtonAction( int buttonId )
{
  ICHI_PRINT("Select::On2uttonAction buttonId %d\n", buttonId);

  switch( buttonId ){
  case View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_1BUTTON:
    // リタイアリクエスト
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_ATTRACTION_RETIRE );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
// @override
void JoinFestaSubScreenAttractionFrame::OnBackButtonAction()
{
  ICHI_PRINT("Select::OnBackButtonAction \n");
  // アトラクション中画面に変更
  ChangeAttractionView();
}


//--------------------------------------------------------------------------
/**
 * @brief   アトラクション中画面に変更
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionFrame::ChangeAttractionView()
{
  // アトラクション中ビューを有効化
  m_pParentView->GetAttractionLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetAttractionLowerView()->SetInputEnabled( true );
    m_pParentView->GetAttractionLowerView()->GetButtonManager()->SetInputEnable( true );
  }

  // 選択肢ビューを無効化
  m_pParentView->GetSelectLowerView()->SetVisible( false );
  m_pParentView->GetSelectLowerView()->SetInputEnabled( false );
  m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false );
}
//--------------------------------------------------------------------------
/**
 * @brief   中止画面に変更
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionFrame::ChangeStopView()
{
  // アトラクション中ビューを無効化
  m_pParentView->GetAttractionLowerView()->SetVisible( false );
  m_pParentView->GetAttractionLowerView()->SetInputEnabled( false );
  m_pParentView->GetAttractionLowerView()->GetButtonManager()->SetInputEnable( false );

  // 選択肢ビューを有効化
  m_pParentView->GetSelectLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetSelectLowerView()->SetInputEnabled( true );
    m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( true );
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
