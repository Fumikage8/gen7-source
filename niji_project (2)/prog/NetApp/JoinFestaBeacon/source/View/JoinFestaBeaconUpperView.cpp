//=============================================================================
/**
 * @file    JoinFestaBeaconUpperView.cpp
 * @brief   ビーコン上画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================

// module

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/MyStatus.h"

#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconResourceID.h"
#include <NetApp/JoinFestaBeacon/include/JoinFestaBeaconAppParam.h>
#include "JoinFestaBeaconUpperView.h"
#include "heap/include/gfl2_heap_manager.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include <System/include/DressUp.h>

// niji
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/join_festa_beacon.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/app/join_festa_beacon/join_festa_beacon.h"
#include "niji_conv_header/app/join_festa_beacon/join_festa_beacon_pane.h"
#include "niji_conv_header/app/join_festa_beacon/join_festa_beacon_anim_list.h"
#include <niji_conv_header/message/msg_jf_menu.h>



#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(View)



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
JoinFestaBeaconUpperView::JoinFestaBeaconUpperView( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork , app::util::Heap * pHeap )
  : app::ui::UIView( pHeap , pHeap->GetDeviceAllocator() , pHeap->GetDeviceAllocator() )
  , m_heap( pHeap )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pJoinFestaBeaconWork( pJoinFestaBeaconWork )
  //, m_isPlayerIconSetup
{
  ::std::memset( m_isPlayerIconSetup , 0 , sizeof( m_isPlayerIconSetup ) );

  CreateLayout();


  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_JFB_WIN_UPP_000_PANE_PARTS_02 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_JFB_WIN_UPP_000_PANE_PARTS_00 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_JFB_WIN_UPP_000_PANE_PARTS_01 , false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
JoinFestaBeaconUpperView::~JoinFestaBeaconUpperView()
{
  DeleteLayout();
}


//-----------------------------------------------------------------------------
/**
 * @brief   プレイヤーアイコンの表示
 */
//-----------------------------------------------------------------------------
void JoinFestaBeaconUpperView::SetUpPlayerIcon( void )
{
  APP_PARAM* pAppParam = m_pJoinFestaBeaconWork->GetAppParam();

  //ワークの状態を参照して表示
  if( pAppParam->connectNum == 1 || pAppParam->inviteType == TYPE_INVITED )
  {//1:1接続
    if( m_isPlayerIconSetup[0] )
      return;
    
    if( pAppParam->m_pSelectedPersonalDataArray[0]->IsEnable() )
    {
      //真ん中を使う
      CreatePlayerIcon( 1 , pAppParam->m_pSelectedPersonalDataArray[0] );
      m_isPlayerIconSetup[0] = true;
    }
  }
  else
  {//複数接続
    for( u32 index = 0 ; index < pAppParam->connectNum ; ++index )
    {
      if( m_isPlayerIconSetup[index] )
        continue;
    
      if( pAppParam->m_pSelectedPersonalDataArray[index] && pAppParam->m_pSelectedPersonalDataArray[index]->IsEnable() )
      {
        CreatePlayerIcon( index , pAppParam->m_pSelectedPersonalDataArray[index] );
        m_isPlayerIconSetup[index] = true;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   プレイヤーアイコンにデータを渡す
 */
//-----------------------------------------------------------------------------
void JoinFestaBeaconUpperView::CreatePlayerIcon( u32 index , NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData )
{
  if( index >= PLAYER_ICON_MAX )
    return;

  const u32 PHOTO_PLATE_PARTS[] =
  {
    PANENAME_JFB_WIN_UPP_000_PANE_PARTS_02,
    PANENAME_JFB_WIN_UPP_000_PANE_PARTS_00,
    PANENAME_JFB_WIN_UPP_000_PANE_PARTS_01
  };

  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PHOTO_PLATE_PARTS[index] , true );

  //プレイヤー名
  gfl2::str::StrBuf name( pPersonalData->GetName() , m_heap->GetDeviceHeap() );
  m_g2dUtil->SetRegisterWord( 0 , name , m_pWordSet->GrammerFromSex( pPersonalData->GetSex() ) );
  m_g2dUtil->SetTextBoxPaneStringExpand(
  lytwk->GetTextBoxPane(lytwk->GetPartsPane( PHOTO_PLATE_PARTS[index] ) , PANENAME_COMMON_PLT_001_PANE_TEXTBOX_00 , &res_id ),
  jf_menu_cap_05 ); 

  //プレイヤーアイコン
  gfl2::lyt::LytParts*  pIconPartsPane  =  lytwk->GetPartsPane( PHOTO_PLATE_PARTS[index] );
  gfl2::lyt::LytPicture* pPicturePane  = lytwk->GetPicturePane( pIconPartsPane , PANENAME_COMMON_PLT_001_PANE_PLAYERPHOTO , &res_id );
  if( pPicturePane )
  {
    NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility = m_pJoinFestaBeaconWork->GetTrainerIconUtility();

    app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;

    data.pReplacePane                  = pPicturePane;
    data.pTimerIconRootPane            = pIconPartsPane;
    data.pInLoadVisibleChangePane      = lytwk->GetPicturePane( pIconPartsPane , PANENAME_COMMON_PLT_001_PANE_WINDOW_PLAYER_FRAME , &res_id );

    data.ConvFromMyStatusIconData( *pPersonalData->GetDressUpIcon() );


    pTrainerIconUtility->SetIconObjectData( index , data ); 
  }
}


void JoinFestaBeaconUpperView::Update(void)
{
  app::ui::UIView::Update(); 
}

void JoinFestaBeaconUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaBeaconWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LYTID_MAIN );
}



//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult JoinFestaBeaconUpperView::OnLayoutPaneEvent( const u32 button_id )
{
  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ボタンタッチを検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void JoinFestaBeaconUpperView::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.05
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult JoinFestaBeaconUpperView::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  return ENABLE_ACCESS;
}



void JoinFestaBeaconUpperView::CreateLayout(void)
{
  NetAppLib::System::ResourceManager* pResourceManager = m_pJoinFestaBeaconWork->GetResourceManager();
  void* pLayoutBuffer = pResourceManager->GetResourceBuffer( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_LAYOUT );
  void* pMsgBuffer = pResourceManager->GetResourceBuffer( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_MSG );

  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_JOIN_FESTA_BEACON_JFB_WIN_UPP_000_BFLYT,
      LA_JOIN_FESTA_BEACON_JFB_WIN_UPP_000___NUM,
      pLayoutBuffer,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  m_pMsgData     = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( pMsgBuffer, m_heap->GetDeviceHeap() );
  m_pWordSet     = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

}

void JoinFestaBeaconUpperView::DeleteLayout(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}





//=============================================================================
//=============================================================================







GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)