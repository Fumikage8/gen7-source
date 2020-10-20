// ============================================================================
/*
 * @file NetApplicationViewBase.cpp
 * @brief 
 * @date 2015.06.15
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


NetApplicationViewBase::NetApplicationViewBase( app::util::Heap* pHeap ) :
  app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() ),
  m_LayoutResourceData(),
  m_LayoutWorkData()
{
}


NetApplicationViewBase::NetApplicationViewBase(
    NetAppLib::System::ApplicationWorkBase* pWork,
    NetAppLib::System::ResourceID layoutResourceID,
    gfl2::lyt::LytArcIndex layoutArcIndex,
    u32 animationMax,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE setupDisplayType,
    NetAppLib::Message::MessageUtility* pMessageUtility,
    gfl2::fs::ArcFile::ARCDATID garcID ) :
    app::ui::UIView( pWork->GetAppHeap(), pWork->GetAppHeap()->GetDeviceAllocator(), pWork->GetAppHeap()->GetDeviceAllocator() ),
    m_LayoutResourceData(),
    m_LayoutWorkData()
{
  Create2DOneLayout(
    pWork,
    layoutResourceID,
    layoutArcIndex,
    animationMax,
    setupDisplayType,
    pMessageUtility,
    garcID
  );
}


//------------------------------------------------------------------
/**
  *  @func     Create2DOneLayout
  *  @brief    ひとつのLayoutリソースに特化するかわりに必要なパラメータを減らしたCreate2D簡易版
  */
//------------------------------------------------------------------
void  NetApplicationViewBase::Create2DOneLayout(
  NetAppLib::System::ApplicationWorkBase* pWork,
  NetAppLib::System::ResourceID           layoutResourceID,
  gfl2::lyt::LytArcIndex                  layoutArcIndex,
  u32                                     animationMax,
  app::util::G2DUtil::SETUP_DISPLAY_TYPE  setupDisplayType,
  NetAppLib::Message::MessageUtility*     pMessageUtility,
  gfl2::fs::ArcFile::ARCDATID             messageArcDatID
)
{
  NetAppLib::System::ResourceManager* pResourceManager = pWork->GetResourceManager();
  void* pLayoutBuffer = pResourceManager->GetResourceBuffer( layoutResourceID );

  app::util::Heap* pAppHeap = pWork->GetAppHeap();

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( pAppHeap, false );

  m_LayoutResourceData.buf              = pLayoutBuffer;
  m_LayoutResourceData.addTexNum        = 0;
  m_LayoutResourceData.addRegisterFlag  = app::util::G2DUtil::ATTACH_NONE;
  
  m_LayoutWorkData.res_index            = 0;
  m_LayoutWorkData.lytres_id            = layoutArcIndex;
  m_LayoutWorkData.anm_max              = animationMax;
  m_LayoutWorkData.buf                  = pLayoutBuffer;
  m_LayoutWorkData.calculateMtxFlag     = true;
  m_LayoutWorkData.setupDisp            = setupDisplayType;
  m_LayoutWorkData.enable               = true;

  Create2D( pAppHeap, NULL, 1,
    &m_LayoutResourceData, 1,
    &m_LayoutWorkData, 1,
    pMessageUtility->GetData( messageArcDatID ), pMessageUtility->GetWordSet() );
}



NetApplicationViewBase::~NetApplicationViewBase()
{
  Delete2D();
  DeleteGraphicsSystem();
}


void NetApplicationViewBase::setupLayout( u32 layoutID, u32 animationID )
{
  UIView::GetG2DUtil()->StartAnime( layoutID, animationID );

  bool bIsInputEnabled = IsInputEnabled();

  SetInputEnabled( false );

  app::ui::UIView::Update();

  SetInputEnabled( bIsInputEnabled );
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)
