// ============================================================================
/*
 * @file NetAppTrainerIconUtility.cpp
 * @brief app::tool::AppToolTrainerIconRendering の生成、削除のシーケンス制御などを行います。
 * @date 2015.09.04
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


NetAppTrainerIconUtility::NetAppTrainerIconUtility() :
  m_pAppHeap( NULL ),
  m_pAppPlayerIconRendering( NULL ),
  m_CreateParams(),
  m_eSequence( SEQUENCE_IDLE ),
  m_bCreateRequest( false ),
  m_bDeleteRequest( false ),
  m_bNPCOnly( false )
{
}


NetAppTrainerIconUtility::~NetAppTrainerIconUtility()
{
}


void NetAppTrainerIconUtility::Create( app::util::AppRenderingManager* pAppRenderingManager, u32 iconNum, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType, bool bNPCOnly )
{
  m_CreateParams.pAppRenderingManager = pAppRenderingManager;
  m_CreateParams.iconNum              = iconNum;
  m_CreateParams.displayType          = displayType;

  m_bCreateRequest  = true;
  m_bNPCOnly        = bNPCOnly;
}


void NetAppTrainerIconUtility::Delete()
{
  m_bDeleteRequest = true;
}


bool NetAppTrainerIconUtility::IsCreate()
{
  bool result = false;
  if( m_pAppPlayerIconRendering )
  {
    result = m_pAppPlayerIconRendering->IsReady();
  }
  return result;
}


bool NetAppTrainerIconUtility::IsDelete()
{
  bool result = false;
  if( !m_pAppPlayerIconRendering )
  {
    result = true;
  }
  return result;
}


void NetAppTrainerIconUtility::Update()
{
  updateSequence();

  if( m_pAppPlayerIconRendering )
  {
    m_pAppPlayerIconRendering->Update();
  }
}


void NetAppTrainerIconUtility::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pAppPlayerIconRendering )
  {
    m_pAppPlayerIconRendering->Draw( displayNo );
  }
}


void NetAppTrainerIconUtility::SetIconObjectData( const u32 idx, const app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon )
{
  if( IsCreate() )
  {
    m_pAppPlayerIconRendering->SetIconObjectData( idx, rObjectData, bVisibleTimerIcon );
  }
}


void NetAppTrainerIconUtility::updateSequence()
{
  switch( m_eSequence )
  {
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_CREATE_START:
    {
      gfl2::heap::HeapBase* pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      m_pAppHeap = GFL_NEW( pDeviceHeap ) app::util::Heap();


      u32 systemHeapSize = app::tool::AppToolTrainerIconRendering::GetUseSystemHeapSize( m_CreateParams.iconNum, m_bNPCOnly );
      u32 deviceHeapSize = app::tool::AppToolTrainerIconRendering::GetUseDeviceHeapSize( m_CreateParams.iconNum, m_bNPCOnly );

      u32 workSize = 1024 * 2;
      u32 AppToolTrainerIconRenderingSize = sizeof( app::tool::AppToolTrainerIconRendering );

      systemHeapSize += ( AppToolTrainerIconRenderingSize + workSize ); // @note : sizeof( app::tool::AppToolTrainerIconRendering ) だけでは
      deviceHeapSize += ( AppToolTrainerIconRenderingSize + workSize ); //         メモリが足りなかったので workSize 分加算しています。workSizeは手動で決めてます。
                                                                        //         以前は 1024 * 8 を直指定してた。合計＋α分が 16384 から 2276 に減ったので、
      GFL_PRINT( "systemHeapSize = %d\n", systemHeapSize );             //         NetAppTrainerIconUtilityまわりで動かなくなったらここを疑う。
      GFL_PRINT( "deviceHeapSize = %d\n", deviceHeapSize );             //         GTS(アイコン4個分)は動いた。2016/01/22
      GFL_PRINT( "workSize = %d\n", workSize );
      GFL_PRINT( "AppToolTrainerIconRenderingSize = %d\n", AppToolTrainerIconRenderingSize );

      m_pAppHeap->LocalHeapCreate( pDeviceHeap, pDeviceHeap, systemHeapSize, deviceHeapSize );

      m_pAppPlayerIconRendering = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::tool::AppToolTrainerIconRendering();

      if( m_bNPCOnly == false )
      {
        m_pAppPlayerIconRendering->Initialize( m_CreateParams.pAppRenderingManager, m_pAppHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), m_CreateParams.iconNum, m_CreateParams.displayType );
      }
      else
      {
        m_pAppPlayerIconRendering->InitializeByNPCOnly( m_CreateParams.pAppRenderingManager, m_pAppHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), m_CreateParams.iconNum, m_CreateParams.displayType );
      }

      m_eSequence = SEQUENCE_CREATE_WAIT;
    }
    break;

    // --------------------------------------------------------------------------------------------
    case SEQUENCE_CREATE_WAIT:
    {
      if( m_pAppPlayerIconRendering->IsReady() )
      {
        m_bCreateRequest = false;
        m_eSequence = SEQUENCE_CREATE_COMPLETE;
      }
    }
    break;
    
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_CREATE_COMPLETE:
    {
      if( m_bDeleteRequest )
      {
        m_eSequence = SEQUENCE_DELETE_START;
      }
    }
    break;
    
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_DELETE_START:
    {
      m_pAppPlayerIconRendering->Terminate();

      m_eSequence = SEQUENCE_DELETE_WAIT;
    }
    break;
    
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_DELETE_WAIT:
    {
      if( m_pAppPlayerIconRendering->IsDelete() )
      {
        GFL_SAFE_DELETE( m_pAppPlayerIconRendering );
        GFL_SAFE_DELETE( m_pAppHeap );

        m_eSequence = SEQUENCE_DELETE_COMPLETE;
      }
    }
    break;
    
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_DELETE_COMPLETE:
    case SEQUENCE_IDLE:
    {
      if( m_bCreateRequest )
      {
        m_eSequence = SEQUENCE_CREATE_START;
      }
    }
    break;
  }
}

//------------------------------------------------------------------
/**
  *  @brief    SaveData::MyStatus::ICON_DATAからICON_OBJECT_DATAを構築する
  */
//------------------------------------------------------------------
void  NetAppTrainerIconUtility::SetIconObjectData( const u32 iconIndex, const Savedata::MyStatus::ICON_DATA& rIconData, gfl2::lyt::LytPicture* pIconPicture, gfl2::lyt::LytPane* pVisibilityControlPane, bool bVisibleTimerIcon )
{
  if(!m_pAppPlayerIconRendering)
  {
    return;
  }

  app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconObjectData;

  iconObjectData.pReplacePane       = pIconPicture;
  iconObjectData.pSyncVisiblePane   = pVisibilityControlPane;
  iconObjectData.ConvFromMyStatusIconData(rIconData);

  m_pAppPlayerIconRendering->SetIconObjectData(iconIndex, iconObjectData, bVisibleTimerIcon );
}

//------------------------------------------------------------------
/**
  *  @brief    SaveData::MyStatus::ICON_DATAからICON_OBJECT_DATAを構築する
  */
//------------------------------------------------------------------
void  NetAppTrainerIconUtility::SetIconObjectDataByOutputTexture( const u32 iconIndex, const Savedata::MyStatus::ICON_DATA& rIconData, gfl2::lyt::LytPicture* pIconPicture, gfl2::lyt::LytPane* pVisibilityControlPane, bool bVisibleTimerIcon , app::tool::AppToolTrainerIconTexture* pOutputTexture )
{
  if(!m_pAppPlayerIconRendering)
  {
    return;
  }

  app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconObjectData;

  iconObjectData.pReplacePane       = pIconPicture;
  iconObjectData.pSyncVisiblePane   = pVisibilityControlPane;
  iconObjectData.ConvFromMyStatusIconData(rIconData);

  m_pAppPlayerIconRendering->SetIconObjectDataByOutputTexture(iconIndex, iconObjectData, bVisibleTimerIcon , pOutputTexture );
}


//------------------------------------------------------------------
/**
  *  @brief    SetIconObjectData()関数のアイコン貼り付けが終了しているかチェック
  */
//------------------------------------------------------------------
bool  NetAppTrainerIconUtility::IsEndIconObjectData( const u32 iconIndex )
{
  if( IsCreate() == false )
  {
    // @fix NMCat[4033] 通信切断時にタイミングによってはこのフローにくることを考えていなかった
    //  ここにきてもバトルロイヤル結果画面では安全が保障されている
    //  警告に格下げ
    GFL_RELEASE_PRINT( "NetAppTrainerIconUtility::IsEndIconObjectData::IsCreate() == false\n" );

    //GFL_ASSERT( 0 );
    return  true;
  }

  return  m_pAppPlayerIconRendering->IsLoadedObject( iconIndex );
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)
