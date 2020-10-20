// ============================================================================
/*
 * @file NBRUpperViewBase.cpp
 * @brief 通信対戦受付アプリ　上画面ビュー
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
// mine
#include "NetApp/NBR/source/View/NBRUpperViewBase.h"

#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

// コンストラクタ
NBRUpperViewBase::NBRUpperViewBase( NetAppLib::System::ApplicationWorkBase* pWork, NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility 
) :
  app::ui::UIView( pWork->GetAppHeap(), pWork->GetAppHeap()->GetSystemAllocator(), pWork->GetAppHeap()->GetDeviceAllocator() )
  ,m_pWork( pWork )
  ,m_pHeap( pWork->GetAppHeap() )
  ,m_NetAppTrainerIconUtility( netAppTrainerIconUtility )
{
}

// デストラクタ
NBRUpperViewBase::~NBRUpperViewBase()
{
}

//--------------------------------------------------------------------------
/**
 * @brief タイトル文字（１行）
 * @param msg_title   タイトル
 */
//--------------------------------------------------------------------------
void NBRUpperViewBase::SetTitleTextOneLine( u32 msg_title )
{
  // テキスト設定
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTONELINE, msg_title );

  // ペイン表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TITLEONELINE, true );
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TITLETWOLINE, false );
}

//--------------------------------------------------------------------------
/**
 * @brief タイトル文字（１行）
 * @param msg_title   タイトル
 */
//--------------------------------------------------------------------------
void NBRUpperViewBase::SetTitleTextTwoLine( u32 msg_title1, u32 msg_title2 )
{
  // テキスト設定
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTTWOLINE_00, msg_title1 );
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTTWOLINE_01, msg_title2 );

  // ペイン表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TITLEONELINE, false );
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TITLETWOLINE, true );
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
