// ============================================================================
/*
 * @file NetAppMenuCursor.cpp
 * @brief 通信アプリ用メニューカーソルです。
 * @date 2015.06.05
 */
// ================================================
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"
#include "AppLib/include/Util/app_util_GetPaneHelper.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppMenuCursor::NetAppMenuCursor( NetAppLib::System::ApplicationWorkBase* pWork ) :
  m_pWork( pWork ),
  m_pMenuCursor( NULL ),
  m_LayoutResourceID(),
  m_pLayoutWork( NULL ),
  m_pCursorPosPanes(NULL),
  m_CursorPositionMax( 0 ),
  m_CursorPosition( 0 )
{
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppMenuCursor::NetAppMenuCursor(
  NetAppLib::System::ApplicationWorkBase*  pWork,
  app::util::G2DUtil*                      pG2DUtil,
  NetAppLib::System::ResourceID            menuCursorResourceID,
  u32                                      layoutResourceIndex,
  u32                                      layoutWorkID ) :
  m_pWork( pWork ),
  m_pMenuCursor( NULL ),
  m_LayoutResourceID(),
  m_pLayoutWork( NULL ),
  m_pCursorPosPanes(NULL),
  m_CursorPositionMax( 0 ),
  m_CursorPosition( 0 )
{
  Create( pG2DUtil, layoutResourceIndex, layoutWorkID, menuCursorResourceID );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppMenuCursor::NetAppMenuCursor(
  NetAppLib::System::ApplicationWorkBase*  pWork,
  app::util::G2DUtil*                      pG2DUtil,
  NetAppLib::System::ResourceID            menuCursorResourceID,
  u32                                      layoutResourceIndex,
  u32                                      layoutWorkID,
  const u32*                               pButtonPartsPaneIDTable,
  const u32*                               pCursorPaneIDTable,
  s32                                      cursorPositionMax ) :
  m_pWork( pWork ),
  m_pMenuCursor( NULL ),
  m_LayoutResourceID(),
  m_pLayoutWork( NULL ),
  m_pCursorPosPanes(NULL),
  m_CursorPositionMax( cursorPositionMax ),
  m_CursorPosition( 0 )
{
  Create( pG2DUtil, layoutResourceIndex, layoutWorkID, pButtonPartsPaneIDTable, pCursorPaneIDTable, cursorPositionMax, menuCursorResourceID );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppMenuCursor::NetAppMenuCursor(
  NetAppLib::System::ApplicationWorkBase* pWork,
  app::util::G2DUtil*                     pG2DUtil,
  NetAppLib::System::ResourceID           menuCursorResourceID,
  u32                                     layoutResourceIndex,
  u32                                     layoutWorkID,
  gfl2::lyt::LytPane*                     pCursorPosPanes[],
  s32                                     cursorPositionMax ) :
  m_pWork( pWork ),
  m_pMenuCursor( NULL ),
  m_LayoutResourceID(),
  m_pLayoutWork( NULL ),
  m_pCursorPosPanes(NULL),
  m_CursorPositionMax( cursorPositionMax ),
  m_CursorPosition( 0 )
{
  Create( pG2DUtil, layoutResourceIndex, layoutWorkID, pCursorPosPanes, cursorPositionMax, menuCursorResourceID );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppMenuCursor::~NetAppMenuCursor()
{
  Destroy();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool NetAppMenuCursor::CanDestroy() const
{
  bool result = true;

  if( m_pMenuCursor )
  {
    result = m_pMenuCursor->DeleteLayoutWork();
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::Destroy()
{
  GFL_SAFE_DELETE_ARRAY( m_pCursorPosPanes );

  if( m_pMenuCursor )
  {
    m_pMenuCursor->DeleteLayoutWork();
  }
  GFL_SAFE_DELETE( m_pMenuCursor );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::Create(
  app::util::G2DUtil*                      pG2DUtil,
  u32                                      layoutResourceIndex,
  u32                                      layoutWorkID,
  NetAppLib::System::ResourceID            menuCursorResourceID )
{
  setupMenuCursor( m_pWork, pG2DUtil, menuCursorResourceID, layoutResourceIndex, layoutWorkID );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::Create(
  app::util::G2DUtil*                      pG2DUtil,
  u32                                      layoutResourceIndex,
  u32                                      layoutWorkID,
  const u32*                               pButtonPartsPaneIDTable,
  const u32*                               pCursorPaneIDTable,
  s32                                      cursorPositionMax,
  NetAppLib::System::ResourceID            menuCursorResourceID )
{
  m_CursorPositionMax = cursorPositionMax;

  setupMenuCursor( m_pWork, pG2DUtil, menuCursorResourceID, layoutResourceIndex, layoutWorkID );

  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();


  m_pCursorPosPanes = GFL_NEW_ARRAY( pAppHeap->GetDeviceHeap() ) gfl2::lyt::LytPane* [ cursorPositionMax ];
  for(u32 posIndex = 0; posIndex < cursorPositionMax; ++posIndex)
  {
    app::util::GetPaneHelper helper(m_pLayoutWork, &m_LayoutResourceID);

    m_pCursorPosPanes[posIndex] = helper.Clear().Push(pButtonPartsPaneIDTable[ posIndex ]).GetPane(pCursorPaneIDTable[ posIndex ]);
  }

  setMenuCursorPosition( m_CursorPosition );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::Create(
  app::util::G2DUtil*                      pG2DUtil,
  u32                                      layoutResourceIndex,
  u32                                      layoutWorkID,
  gfl2::lyt::LytPane*                      pCursorPosPanes[],
  s32                                      cursorPositionMax,
  NetAppLib::System::ResourceID            menuCursorResourceID )
{
  m_CursorPositionMax = cursorPositionMax;

  setupMenuCursor( m_pWork, pG2DUtil, menuCursorResourceID, layoutResourceIndex, layoutWorkID );

  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  m_pCursorPosPanes = GFL_NEW_ARRAY( pAppHeap->GetDeviceHeap() ) gfl2::lyt::LytPane* [ cursorPositionMax ];
  for(u32 posIndex = 0; posIndex < cursorPositionMax; ++posIndex)
  {
    m_pCursorPosPanes[posIndex] = pCursorPosPanes[posIndex];
  }

  setMenuCursorPosition( m_CursorPosition );
}




//------------------------------------------------------------------
/**
  *  @brief    カーソル位置Pane列を設定
  *  @note     CursorPositionは0にされる
  */
//------------------------------------------------------------------
void  NetAppMenuCursor::SetCusorPosPanes(gfl2::lyt::LytPane* pCursorPosPanes[], const s32 cursorPositionMax)
{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();

  /*  delete  */
  {
    GFL_SAFE_DELETE_ARRAY( m_pCursorPosPanes );
    m_CursorPositionMax = cursorPositionMax;
    m_CursorPosition    = 0;
  }

  /*  create  */
  {
    m_pCursorPosPanes = GFL_NEW_ARRAY( pHeapBase ) gfl2::lyt::LytPane* [ cursorPositionMax ];
    for(u32 posIndex = 0; posIndex < cursorPositionMax; ++posIndex)
    {
      m_pCursorPosPanes[posIndex] = pCursorPosPanes[posIndex];
    }
  }

  /*  setup  */
  {
    setMenuCursorPosition( m_CursorPosition );
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppMenuCursor::SetCusorPosPanes(const u32*  pButtonPartsPaneIDTable, const u32* pCursorPaneIDTable, const s32 cursorPositionMax)
{
  gfl2::lyt::LytPane**      ppCursorPosPanes = GFL_NEW_LOW(m_pWork->GetDeviceHeap()) gfl2::lyt::LytPane*[cursorPositionMax];
  app::util::GetPaneHelper  helper(m_pLayoutWork, &m_LayoutResourceID);


  for(u32 posIndex = 0; posIndex < cursorPositionMax; ++posIndex)
  {
    const u32  partsID  = pButtonPartsPaneIDTable[ posIndex ];
    
    ppCursorPosPanes[posIndex] = helper.Clear().Push(partsID).GetPane(pCursorPaneIDTable[ posIndex ]);
  }

  SetCusorPosPanes(ppCursorPosPanes, cursorPositionMax);

  GFL_SAFE_DELETE_ARRAY(ppCursorPosPanes);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::setupMenuCursor(
  NetAppLib::System::ApplicationWorkBase*  pWork,
  app::util::G2DUtil*                      pG2DUtil,
  NetAppLib::System::ResourceID            menuCursorResourceID,
  u32                                      layoutResourceIndex,
  u32                                      layoutWorkID )
{
  NetAppLib::System::ResourceManager* pResourceManager = pWork->GetResourceManager();
  void* pMenuCursorBuffer = pResourceManager->GetResourceBuffer( menuCursorResourceID );

  app::util::Heap* pAppHeap = pWork->GetAppHeap();
  m_pMenuCursor = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::MenuCursor( pAppHeap, &pMenuCursorBuffer );

  m_pMenuCursor->CreateLayoutWork(
    pG2DUtil->GetLayoutSystem(),
    pG2DUtil->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ), // @note : UPPER も指定したくなる場合があるかも(その時に対応する)
    pG2DUtil->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL, // @note : SIZE_LARGE を指定したくなる場合があるかも(その時に対応する)
    false );
  m_pMenuCursor->SetVisible( true );
  m_pMenuCursor->StartAnime();

  m_LayoutResourceID = pG2DUtil->GetLayoutResourceID( layoutResourceIndex );
  m_pLayoutWork = pG2DUtil->GetLayoutWork( layoutWorkID );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::setMenuCursorPosition( s32 cursorPosition )
{
  m_pMenuCursor->Put( m_pLayoutWork, m_pCursorPosPanes[cursorPosition] );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::Update()
{
  m_pMenuCursor->Update();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
  m_pMenuCursor->Draw( pAppRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::MoveUp( SoundMiddleID ePlaySEID )
{
  if( m_CursorPositionMax > 1 )
  {
    if( ePlaySEID != SMID_NULL )
    {
      Sound::PlaySE( static_cast<u32>( ePlaySEID ) );
    }
  }

  if( m_CursorPositionMax > 0 )
  {
    --m_CursorPosition;
    
    if( m_CursorPosition < 0 )
    {
      m_CursorPosition = m_CursorPositionMax - 1;
    }
    
    setMenuCursorPosition( m_CursorPosition );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppMenuCursor::MoveDown( SoundMiddleID ePlaySEID )
{
  if( m_CursorPositionMax > 1 )
  {
    if( ePlaySEID != SMID_NULL )
    {
      Sound::PlaySE( static_cast<u32>( ePlaySEID ) );
    }
  }

  if( m_CursorPositionMax > 0 )
  {
    ++m_CursorPosition;
    
    if( m_CursorPosition > m_CursorPositionMax - 1 )
    {
      m_CursorPosition = 0;
    }
    
    setMenuCursorPosition( m_CursorPosition );
  }
}


GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)
