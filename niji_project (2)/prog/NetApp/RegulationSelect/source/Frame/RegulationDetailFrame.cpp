// ============================================================================
/*
 * @file RegulationDetailFrame.cpp
 * @brief レギュレーション詳細画面のフレームです。
 * @date 2015.07.15
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationDetailFrame.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationDetailLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_regulation.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/poke_lib/item/itemsym.h>
#include <niji_conv_header/poke_lib/wazano_def.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationDetailFrame::RegulationDetailFrame(
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  NetApp::RegulationSelect::RegulationSelectFrameBase( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects ),
  m_AppQRTextureCreateUtility( pRegulationSelectWork->GetDeviceHeap(), ENCODE_DATA_BUFFER_SIZE ),
  m_pInfoWindowBookTypeLowerView( NULL ),
  m_VetoPokeList( pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), MONSNO_MAX ),
  m_VetoItemList( pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), ITEM_DATA_MAX ),
  m_VetoWazaList( pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), WAZANO_MAX ),
  m_PageInfoList( pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), ( ( MONSNO_MAX / PAGE_MENU_MAX ) + ( ITEM_DATA_MAX / PAGE_MENU_MAX ) + ( WAZANO_MAX / PAGE_MENU_MAX ) + 3 ) ), // 1ページ PAGE_MENU_MAX 分の表示
  m_VetoPokeStartPageIndex( 0 ),
  m_VetoItemStartPageIndex( 0 ),
  m_VetoWazaStartPageIndex( 0 )
{
}


RegulationDetailFrame::~RegulationDetailFrame()
{
  if( m_pInfoWindowBookTypeLowerView )
  {
    m_pInfoWindowBookTypeLowerView->RemoveFromSuperView();
  }
  GFL_SAFE_DELETE( m_pInfoWindowBookTypeLowerView );
}


u32 RegulationDetailFrame::getDetailPageNum()
{
  u32 pageNum = 0;

  NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = m_pRegulationSelectWork->GetSelectRegulationDrawInfo();
  
  if( pRegulationDrawInfo )
  {
    if( pRegulationDrawInfo->IsPreset() )
    {
      NetApp::RegulationSelect::BootMode eBootMode = m_pRegulationSelectWork->GetBootMode();

      Savedata::ZukanData* pZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

      m_VetoPokeList.Erase();
      m_VetoItemList.Erase();
      m_VetoWazaList.Erase();
      m_PageInfoList.Erase();

      Regulation& regulation = pRegulationDrawInfo->GetRegulation();

      for( u16 monsNo = 1; monsNo <= MONSNO_END; ++monsNo )
      {
        MonsNo eMonsNo = static_cast<MonsNo>(monsNo);

        bool bPokeSeeFlag = pZukanData->GetPokeSeeFlag( eMonsNo );

        bool bAddFlag = false;

        if( bPokeSeeFlag )
        {
          if( regulation.CheckParamBit( Regulation::VETO_POKE_BIT, eMonsNo ) )
          {
            bAddFlag = true;
          }
        }

        if( eBootMode == NetApp::RegulationSelect::BOOT_MODE_DETAIL_BATTLE_TEAM )
        {
          if( m_pRegulationSelectWork->GetQRPartyFlag() )
          {
            pml::PokeParty* pQRPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRParty();

            u32 memberCount = pQRPokeParty->GetMemberCount();

            for( u32 i = 0; i < memberCount; ++i )
            {
              pml::pokepara::PokemonParam* pPokemonParam = pQRPokeParty->GetMemberPointer(i);
              
              if( pPokemonParam )
              {
                if( pPokemonParam->GetMonsNo() == eMonsNo )
                {
                  if( regulation.CheckParamBit( Regulation::VETO_POKE_BIT, eMonsNo ) )
                  {
                    bAddFlag = true;
                  }
                }
              }
            }
          }
        }

        if( bAddFlag )
        {
          m_VetoPokeList.PushBack( monsNo );
        }
      }

      for( u16 itemNo = 1; itemNo <= ITEM_DATA_MAX; ++itemNo )
      {
        if( regulation.CheckParamBit( Regulation::VETO_ITEM_BIT, static_cast<u32>(itemNo) ) )
        {
          m_VetoItemList.PushBack( itemNo );
        }
      }

      for( u16 wazaNo = 1; wazaNo < WAZANO_MAX; ++wazaNo )
      {
        if( regulation.CheckParamBit( Regulation::VETO_WAZA_BIT, static_cast<u32>(wazaNo) ) )
        {
          m_VetoWazaList.PushBack( wazaNo );
        }
      }

      u32 addPageNum = 0;

      // 禁止ポケモンページ
      {
        m_VetoPokeStartPageIndex = pageNum;
        addPageNum = calcPageNum( m_VetoPokeList.Size() );
        for( u32 i = 0; i < addPageNum; ++i )
        {
          m_PageInfoList.PushBack( PAGE_INFO_POKE );
        }
        pageNum += addPageNum;
      }

      // 禁止道具ページ
      {
        m_VetoItemStartPageIndex = pageNum;
        addPageNum = calcPageNum( m_VetoItemList.Size() );
        for( u32 i = 0; i < addPageNum; ++i )
        {
          m_PageInfoList.PushBack( PAGE_INFO_ITEM );
        }
        pageNum += addPageNum;
      }

      // 禁止技ページ
      {
        m_VetoWazaStartPageIndex = pageNum;
        addPageNum = calcPageNum( m_VetoWazaList.Size() );
        for( u32 i = 0; i < addPageNum; ++i )
        {
          m_PageInfoList.PushBack( PAGE_INFO_WAZA );
        }
        pageNum += addPageNum;
      }
    }
    else
    {
      pageNum = 2;
    }
  }
  
  return pageNum;
}


u32 RegulationDetailFrame::calcPageNum( u32 listSize )
{
  u32 pageNum = 0;

  const u32 pageMenuMax = static_cast<u32>(PAGE_MENU_MAX);

  pageNum = listSize / pageMenuMax;

  u32 value = listSize % pageMenuMax;

  if( value > 0 )
  {
    pageNum += 1;
  }

  if( pageNum == 0 )
  {
    pageNum = 1;
  }

  return pageNum;
}


//-----------------------------------------------------------------------------
// RegulationSelectFrameBase
//-----------------------------------------------------------------------------
bool RegulationDetailFrame::startup()
{
  m_pRegulationSelectViewObjects->InputDisableAll();
  m_pRegulationSelectWork->GetRootUpperView()->RemoveAllSubView();
  m_pRegulationSelectWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView() );
  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationDetailLowerView() );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( m_pRegulationSelectWork->GetSelectRegulationDrawInfo() );

  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  m_pInfoWindowBookTypeLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( pAppHeap );
  m_pInfoWindowBookTypeLowerView->SyncCreate( pAppHeap, m_pRegulationSelectWork->GetAppRenderingManager(), m_PageInfoList.MaxSize(), NULL );

  NetApp::RegulationSelect::Draw::RegulationDetailLowerView* pRegulationDetailLowerView = m_pRegulationSelectViewObjects->GetRegulationDetailLowerView();
  pRegulationDetailLowerView->AddSubView( m_pInfoWindowBookTypeLowerView );

  m_pInfoWindowBookTypeLowerView->ResetContents( getDetailPageNum() );
  OnChangePage(0);

  NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = m_pRegulationSelectWork->GetSelectRegulationDrawInfo();

  if( pRegulationDrawInfo )
  {
    char url[ ENCODE_DATA_BUFFER_SIZE ];
    gfl2::std::MemClear( url, sizeof( url ) );
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    sprintf( url, "https://dev2-3ds.pokemon-gl.com/dl-regulation/%d", pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::URL_DATA ) );
#elif defined(GF_PLATFORM_WIN32)
    sprintf_s( url, "https://dev2-3ds.pokemon-gl.com/dl-regulation/%d", pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::URL_DATA ) );
#endif
#else
#if defined(GF_PLATFORM_CTR)
    sprintf( url, "https://3ds.pokemon-gl.com/dl-regulation/%d", pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::URL_DATA ) );
#endif
#endif

    GFL_PRINT( "encode url = %s\n", url );
    m_AppQRTextureCreateUtility.CreateStart( reinterpret_cast<const u8*>( url ), strlen( url ) );
  }

  m_pInfoWindowBookTypeLowerView->StartOpen( this, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM );

  return true;
}


void RegulationDetailFrame::setListener()
{
}


void RegulationDetailFrame::removeListener()
{
  m_pRegulationSelectViewObjects->InputDisableAll();
}


bool RegulationDetailFrame::cleanup()
{
  // @fix GFNMCat #5733 QRテクスチャの生成が完了する前にフレームの終了処理が走ってしまうと無限ループになるのでUpdateを呼び出す
  m_AppQRTextureCreateUtility.Update();

  bool bIsBusy = m_AppQRTextureCreateUtility.IsBusy();
  bool bIsEnd  = m_pInfoWindowBookTypeLowerView->EndFunc();

  if( bIsBusy == false && bIsEnd == true )
  {
    return true;
  }

  return false;
}


void RegulationDetailFrame::updateSequence()
{
  m_AppQRTextureCreateUtility.Update();
}


//-----------------------------------------------------------------------------
// InfoWindowBookTypeListener
//-----------------------------------------------------------------------------
void RegulationDetailFrame::OnClose()
{
  NetAppLib::System::FrameResult frameResult = NetAppLib::System::FRAME_RESULT_INVALID;
  switch( m_pRegulationSelectWork->GetDetailBootFrame() )
  {
    case NetApp::RegulationSelect::DETAIL_BOOT_FRAME_SELECT_MAIN:
      frameResult = NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_SELECT_MAIN;
    break;

    case NetApp::RegulationSelect::DETAIL_BOOT_FRAME_DL_SELECT:
      frameResult = NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_DL_SELECT;
    break;

    case NetApp::RegulationSelect::DETAIL_BOOT_FRAME_DL_DELETE:
      frameResult = NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_DL_DELETE;
    break;

    case NetApp::RegulationSelect::DETAIL_BOOT_PROC_BATTLE_TEAM_SELECT:
      frameResult = NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_BATTLE_TEAM_SELECT;
    break;
  }
  exitFrame( frameResult );
}


void RegulationDetailFrame::OnChangePage( u32 pageNo )
{
  NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = m_pRegulationSelectWork->GetSelectRegulationDrawInfo();
  
  if( pRegulationDrawInfo )
  {
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();
  
    bool bIsRegister = false;
    u32 messageID = 0;
    u32 contentType = 0;
    
    // プリセットレギュレーションの場合
    if( pRegulationDrawInfo->IsPreset() )
    {
      u32 pageCount = 0;
      for( gfl2::util::List<u8>::Iterator it = m_PageInfoList.Begin(); it != m_PageInfoList.End(); ++it )
      {
        if( pageNo == pageCount )
        {
          switch( *it )
          {
            case PAGE_INFO_POKE:
              switch( m_VetoPokeList.Size() )
              {
                case 0:  contentType = 1; messageID = msg_reg_exp_09; break; // 禁止ポケモン無し
                default: contentType = 1; messageID = msg_reg_exp_06; break; // 禁止ポケモン有り
              }
              bIsRegister = setRegisterNumber( PAGE_INFO_POKE, m_VetoPokeList, m_VetoPokeStartPageIndex, pageNo );
            break;

            case PAGE_INFO_ITEM:
              switch( m_VetoItemList.Size() )
              {
                case 0:  contentType = 1; messageID = msg_reg_exp_10; break; // 禁止道具無し
                default: contentType = 1; messageID = msg_reg_exp_07; break; // 禁止道具有り
              }
              bIsRegister = setRegisterNumber( PAGE_INFO_ITEM, m_VetoItemList, m_VetoItemStartPageIndex, pageNo );
            break;

            case PAGE_INFO_WAZA:
              switch( m_VetoWazaList.Size() )
              {
                case 0:  contentType = 1; messageID = msg_reg_exp_11; break; // 禁止技無し
                default: contentType = 1; messageID = msg_reg_exp_08; break; // 禁止技有り
              }
              bIsRegister = setRegisterNumber( PAGE_INFO_WAZA, m_VetoWazaList, m_VetoWazaStartPageIndex, pageNo );
            break;
          }
          break;
        }
        ++pageCount;
      }
    }
    // 配信レギュレーションの場合
    else
    {
      switch( pageNo )
      {
        case 0: contentType = 1; messageID = msg_reg_exp_12; break;
        case 1: contentType = 2;                             break;
      }
    }
    
    if( contentType == 1 )
    {
      gfl2::str::StrBuf& strbuf = pMessageUtility->GetString( GARC_message_regulation_DAT, messageID );
      gfl2::str::StrBuf* pTempStrBuf = &strbuf;
      if( bIsRegister )
      {
        pTempStrBuf = m_pInfoWindowBookTypeLowerView->GetG2DUtil()->GetTempStrBuf(0);
        print::WordSet* pWordSet = pMessageUtility->GetWordSet();
        pWordSet->Expand( pTempStrBuf, &strbuf );
      }
      m_pInfoWindowBookTypeLowerView->SetTextContent( pageNo, pTempStrBuf, NULL );
    }
    else
    if( contentType == 2 )
    {
      m_pInfoWindowBookTypeLowerView->SetQRContent( pageNo, m_AppQRTextureCreateUtility.GetLytResTextureInfo(), NULL );
    }
  }
}


bool RegulationDetailFrame::setRegisterNumber( PageInfo ePageInfo, gfl2::util::List<u16>& vetoList, u32 startPageIndex, u32 drawPageIndex )
{
  bool bIsRegister = false;
  if( vetoList.Size() > 0 )
  {
    if( startPageIndex <= drawPageIndex )
    {
      NetAppLib::Message::MessageUtility* pMessageUtility = m_pRegulationSelectWork->GetMessageUtility();
      print::WordSet* pWordSet = pMessageUtility->GetWordSet();
      pWordSet->ClearAllBuffer();

      u32 index = drawPageIndex - startPageIndex;
      index *= PAGE_MENU_MAX;

      u32 count = 0;

      for( gfl2::util::List<u16>::Iterator it = vetoList.Begin(); it != vetoList.End(); ++it )
      {
        if( index == count )
        {
          for( u32 i = 0; i < PAGE_MENU_MAX; ++i )
          {
            switch( ePageInfo )
            {
              case PAGE_INFO_POKE:
                pWordSet->RegisterPokeMonsNameNo( i, static_cast<MonsNo>( *it ) );
                bIsRegister = true;
              break;

              case PAGE_INFO_ITEM:
                pWordSet->RegisterItemName( i, static_cast<u32>( *it ), 1 );
                bIsRegister = true;
              break;

              case PAGE_INFO_WAZA:
                pWordSet->RegisterWazaName( i, static_cast<u32>( *it ) );
                bIsRegister = true;
              break;
            }

            ++it;

            if( it == vetoList.End() )
            {
              break;
            }
          }
          break;
        }
        ++count;
      }
    }
  }
  return bIsRegister;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
