// ============================================================================
/*
 * @file GTSWork.h
 * @brief GTSアプリを動かす為のワーククラスです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( GTSWORK_H_INCLUDE )
#define GTSWORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_std_string.h>
#include <pml/include/pml_PokePara.h>
#include <pml/include/pml_Const.h>
#include <str/include/gfl2_StrBuf.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/message/msg_gts.h>
#include "NetApp/GTS/source/GTSTypes.h"
#include "NetApp/GTS/source/GTSDefine.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include "System/include/HeapDefine.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

typedef gfl2::util::List<NetApp::GTS::GtsSearchResultInfo>            GtsSearchResultList;
typedef gfl2::util::List<NetApp::GTS::GtsSearchResultInfo>::Iterator  GtsSearchResultIterator;

class PokemonSelectInfo
{
public:
  u32  tray;
  u32  pos;
  bool inBox;
  PokemonSelectInfo() : tray( 0 ), pos( 0 ), inBox( false ){}
};

class GTSWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::GTS::GTS_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::GTS::GTS_APP_DEVICE_HEAP_SIZE;

public:

  static const u32  GTS_SEARCH_POKEMON_RESULT_MAX   = NetApp::GTS::GTS_SEARCH_POKEMON_RESULT_MAX;

  enum BootProcID
  {
    BOOT_PROC_ID_NONE,
    BOOT_PROC_ID_BOX,
    BOOT_PROC_ID_DEMO_UPLOAD,
    BOOT_PROC_ID_DEMO_TRADE,
    BOOT_PROC_ID_DEMO_DOWNLOAD_TRADE,
    BOOT_PROC_ID_DEMO_DOWNLOAD,
    BOOT_PROC_ID_NAME_INPUT
  };

  enum NameInputBootFrame
  {
    NAME_INPUT_BOOT_FRAME_SEARCH_POKEMON,
    NAME_INPUT_BOOT_FRAME_UPLOAD_POKEMON
  };

public:
  GTSWork( gfl2::heap::HeapBase* pWorkHeap ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
    m_PokemonSelectInfo(),
    m_SelectPokemonCoreParam( pWorkHeap ),
    m_bCallZukanRegister( false ),
    m_ZukanRegisterEventParam(),
    m_bCallEvolution( false ),
    m_EvolutionEventParam(),
    m_pPokeModelUtility( NULL ),
    m_pRootUpperView( NULL ),
    m_pRootLowerView( NULL ),
    m_pTrainerIconUtility( NULL ),
    m_RequestPokemonMonsNo( MONSNO_NULL ),
    m_RequestPokemonGender( NetApp::GTS::GENDER_UNKNOWN ),
    m_RequestPokemonLevelBand( 0 ), // 0 : 気にしない
    m_RequestMessageID( msg_gts_introtext_01 ),
    m_SearchPokemonMonsNo( MONSNO_NULL ),
    m_SearchPokemonGender( NetApp::GTS::GENDER_UNKNOWN ),
    m_SearchPokemonLevelBand( 0 ), // 0 : 気にしない
    m_SearchPokemonLegend( 0 ),
    m_SearchRangeStartOffset( 0 ),
    m_SearchPokemonResultList( pWorkHeap, GTS_SEARCH_POKEMON_RESULT_MAX ),
    m_RefineSearchPokemonResultList( pWorkHeap, GTS_SEARCH_POKEMON_RESULT_MAX ),
    m_eBootProcID( BOOT_PROC_ID_NONE ),
    m_DemoSendPokemon( pWorkHeap ),
    m_DemoRecvPokemon( pWorkHeap ),
    m_ReceivedPokemonParam( pWorkHeap, m_DemoRecvPokemon ),
    m_TradePartnerName( pml::PERSON_NAME_BUFFER_SIZE, pWorkHeap ),
    m_InputName( NetApp::GTS::GTS_NAME_INPUT_LENGTH, pWorkHeap ),
    m_eNameInputBootFrame( NAME_INPUT_BOOT_FRAME_SEARCH_POKEMON ),
    m_bFadeInRequest( false ),
    m_bNameInputFlag( false ),
    m_pJapanMonsNameMessageUtility( NULL ),
    m_pEnglishMonsNameMessageUtility( NULL )
  {
    gfl2::std::MemClear( &m_TradePartnerInfo, sizeof( NetApp::GTS::GtsSearchResultInfo ) );
    gfl2::std::MemClear( &m_UploadGTSIndexData, sizeof( NetApp::GTS::GtsIndexData ) );
    gfl2::std::MemClear( &m_Signature, sizeof( NetApp::GTS::GtsSignature ) );
    gfl2::std::MemClear( &m_PrepareTradeKey, sizeof( NetApp::GTS::GtsSerializeRecordKey ) );
  }

  virtual ~GTSWork()
  {
    m_RefineSearchPokemonResultList.Clear();
    m_SearchPokemonResultList.Clear();
  }

  // ----------------------------------------------------------------------------------------------
  NetApp::GTS::ApplicationSystem::PokemonSelectInfo& GetPokemonSelectInfo() { return m_PokemonSelectInfo; }
  pml::pokepara::CoreParam& GetSelectPokemonCoreParam() { return m_SelectPokemonCoreParam; }
  bool& GetCallZukanRegister() { return m_bCallZukanRegister; }
  app::event::ZukanRegisterEventParam& GetZukanRegisterEventParam() { return m_ZukanRegisterEventParam; }
  bool& GetCallEvolution() { return m_bCallEvolution; }
  NetEvent::Evolution::EvolutionEventParam& GetEvolutionEventParam() { return m_EvolutionEventParam; }


  // ----------------------------------------------------------------------------------------------
  // @note : GetPokeModelUtility で取得して使用する際に NULL チェックをする必要があります
  NetAppLib::Util::NetAppPokeModelUtility* GetPokeModelUtility() { return m_pPokeModelUtility; }
  void SetPokeModelUtility( NetAppLib::Util::NetAppPokeModelUtility* pPokeModelUtility ){ m_pPokeModelUtility = pPokeModelUtility; }

  NetAppLib::UI::NetAppEmptyView* GetRootUpperView() const { return m_pRootUpperView; }
  void SetRootUpperView( NetAppLib::UI::NetAppEmptyView* pRootUpperView ) { m_pRootUpperView = pRootUpperView; }

  NetAppLib::UI::NetAppEmptyView* GetRootLowerView() const { return m_pRootLowerView; }
  void SetRootLowerView( NetAppLib::UI::NetAppEmptyView* pRootLowerView ) { m_pRootLowerView = pRootLowerView; }

  NetAppLib::Util::NetAppTrainerIconUtility* GetTrainerIconUtility() { return m_pTrainerIconUtility; }
  void SetTrainerIconUtility( NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility ){ m_pTrainerIconUtility = pTrainerIconUtility; }

  // ----------------------------------------------------------------------------------------------
  u32 GetRequestPokemonMonsNo() const { return m_RequestPokemonMonsNo; }
  void SetRequestPokemonMonsNo( u32 monsno ) { m_RequestPokemonMonsNo = monsno; }

  u32 GetRequestPokemonGender() const { return m_RequestPokemonGender; }
  void SetRequestPokemonGender( u32 gender ) { m_RequestPokemonGender = gender; }

  u32 GetRequestPokemonLevelBand() const { return m_RequestPokemonLevelBand; }
  void SetRequestPokemonLevelBand( u32 levelBand ) { m_RequestPokemonLevelBand = levelBand; }

  u32 GetRequestMessageID() const { return m_RequestMessageID; }
  void SetRequestMessageID( u32 messageID ) { m_RequestMessageID = messageID; }

  // ----------------------------------------------------------------------------------------------
  u32 GetSearchPokemonMonsNo() const { return m_SearchPokemonMonsNo; }
  void SetSearchPokemonMonsNo( u32 monsno ) { m_SearchPokemonMonsNo = monsno; }

  u32 GetSearchPokemonGender() const { return m_SearchPokemonGender; }
  void SetSearchPokemonGender( u32 gender ) { m_SearchPokemonGender = gender; }

  u32 GetSearchPokemonLevelBand() const { return m_SearchPokemonLevelBand; }
  void SetSearchPokemonLevelBand( u32 levelBand ) { m_SearchPokemonLevelBand = levelBand; }

  u32 GetSearchPokemonLegend() const { return m_SearchPokemonLegend; }
  void SetSearchPokemonLegend( u32 legend ) { m_SearchPokemonLegend = legend; }

  u32 GetSearchRangeStartOffset() const { return m_SearchRangeStartOffset; }
  void SetSearchRangeStartOffset( u32 startOffset ) { m_SearchRangeStartOffset = startOffset; }

  GtsSearchResultList& GetSearchPokemonResultList() { return m_SearchPokemonResultList; }
  GtsSearchResultList& GetRefineSearchPokemonResultList() { return m_RefineSearchPokemonResultList; }

  // ----------------------------------------------------------------------------------------------
  BootProcID GetBootProcID() const { return m_eBootProcID; }
  void SetBootProcID( BootProcID eBootProcID ) { m_eBootProcID = eBootProcID; }

  // ----------------------------------------------------------------------------------------------
  pml::pokepara::CoreParam& GetDemoSendPokemon() { return m_DemoSendPokemon; }
  pml::pokepara::CoreParam& GetDemoRecvPokemon() { return m_DemoRecvPokemon; }
  pml::pokepara::PokemonParam& GetReceivedPokemonParam() { return m_ReceivedPokemonParam; }
  gfl2::str::StrBuf& GetTradePartnerName() { return m_TradePartnerName; }

  // ----------------------------------------------------------------------------------------------
  const NetApp::GTS::GtsSearchResultInfo& GetTradePartnerInfo() const { return m_TradePartnerInfo; }
  void SetTradePartnerInfo( const NetApp::GTS::GtsSearchResultInfo& tradePartnerInfo ) { m_TradePartnerInfo = tradePartnerInfo; }

  // ----------------------------------------------------------------------------------------------
  NetApp::GTS::GtsIndexData& GetUploadGTSIndexData() { return m_UploadGTSIndexData; }
  const NetApp::GTS::GtsIndexData& GetUploadGTSIndexData() const { return m_UploadGTSIndexData; }
  void SetUploadGTSIndexData( const NetApp::GTS::GtsIndexData& gtsIndexData ) { m_UploadGTSIndexData = gtsIndexData; }

  const NetApp::GTS::GtsSignature& GetSignature() const { return m_Signature; }
  void SetSignature( const NetApp::GTS::GtsSignature& signature ) { m_Signature = signature; }

  const NetApp::GTS::GtsSerializeRecordKey& GetPrepareTradeKey() const { return m_PrepareTradeKey; }
  void SetPrepareTradeKey( const NetApp::GTS::GtsSerializeRecordKey& prepareTradeKey ) { m_PrepareTradeKey = prepareTradeKey; }

  // ----------------------------------------------------------------------------------------------
  gfl2::str::StrBuf& GetInputName() { return m_InputName; }
  void SetInputName( const gfl2::str::STRCODE* pInputName ) { m_InputName.SetStr( pInputName ); }

  NetApp::GTS::ApplicationSystem::GTSWork::NameInputBootFrame GetNameInputBootFrame() { return m_eNameInputBootFrame; }
  void SetNameInputBootFrame( NetApp::GTS::ApplicationSystem::GTSWork::NameInputBootFrame eBootFrame ) { m_eNameInputBootFrame = eBootFrame; }

  bool IsFadeInRequest() const { return m_bFadeInRequest; }
  void FadeInRequest() { m_bFadeInRequest = true; }
  void ResetFadeInRequest() { m_bFadeInRequest = false; }
  bool GetNameInputFlag() const { return m_bNameInputFlag; }
  void SetNameInputFlag( bool bFlag ) { m_bNameInputFlag = bFlag; }

  NetAppLib::Message::MessageUtility* GetJapanMonsNameMessageUtility() { return m_pJapanMonsNameMessageUtility; }
  void SetJapanMonsNameMessageUtility( NetAppLib::Message::MessageUtility* pMessageUtility ) { m_pJapanMonsNameMessageUtility = pMessageUtility; }
  NetAppLib::Message::MessageUtility* GetEnglishMonsNameMessageUtility() { return m_pEnglishMonsNameMessageUtility; }
  void SetEnglishMonsNameMessageUtility( NetAppLib::Message::MessageUtility* pMessageUtility ) { m_pEnglishMonsNameMessageUtility = pMessageUtility; }

private:

  // ----------------------------------------------------------------------------------------------
  NetApp::GTS::ApplicationSystem::PokemonSelectInfo               m_PokemonSelectInfo;        //! 選択情報
  pml::pokepara::CoreParam                                        m_SelectPokemonCoreParam;   //! Boxアプリで選択したポケモン
  bool                                                            m_bCallZukanRegister;
  app::event::ZukanRegisterEventParam                             m_ZukanRegisterEventParam;
  bool                                                            m_bCallEvolution;
  NetEvent::Evolution::EvolutionEventParam                        m_EvolutionEventParam;


  // ----------------------------------------------------------------------------------------------
  NetAppLib::Util::NetAppPokeModelUtility*                        m_pPokeModelUtility;
  NetAppLib::UI::NetAppEmptyView*                                 m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                                 m_pRootLowerView;
  NetAppLib::Util::NetAppTrainerIconUtility*                      m_pTrainerIconUtility;
  
  // ----------------------------------------------------------------------------------------------
  u32                                                             m_RequestPokemonMonsNo;
  u32                                                             m_RequestPokemonGender;
  u32                                                             m_RequestPokemonLevelBand;
  u32                                                             m_RequestMessageID;

  // ----------------------------------------------------------------------------------------------
  u32                                                             m_SearchPokemonMonsNo;
  u32                                                             m_SearchPokemonGender;
  u32                                                             m_SearchPokemonLevelBand;
  u32                                                             m_SearchPokemonLegend;
  u32                                                             m_SearchRangeStartOffset;
  GtsSearchResultList                                             m_SearchPokemonResultList;
  GtsSearchResultList                                             m_RefineSearchPokemonResultList;

  // ----------------------------------------------------------------------------------------------
  BootProcID                                                      m_eBootProcID;

  // ----------------------------------------------------------------------------------------------
  pml::pokepara::CoreParam                                        m_DemoSendPokemon;
  pml::pokepara::CoreParam                                        m_DemoRecvPokemon;
  pml::pokepara::PokemonParam                                     m_ReceivedPokemonParam;     //! 受信したポケモン
  gfl2::str::StrBuf                                               m_TradePartnerName;

  // ----------------------------------------------------------------------------------------------
  NetApp::GTS::GtsSearchResultInfo                                m_TradePartnerInfo;

  // ----------------------------------------------------------------------------------------------
  NetApp::GTS::GtsIndexData                                       m_UploadGTSIndexData;
  NetApp::GTS::GtsSignature                                       m_Signature;

  NetApp::GTS::GtsSerializeRecordKey                              m_PrepareTradeKey;

  // ----------------------------------------------------------------------------------------------
  gfl2::str::StrBuf                                               m_InputName;
  NetApp::GTS::ApplicationSystem::GTSWork::NameInputBootFrame     m_eNameInputBootFrame;
  bool                                                            m_bFadeInRequest;
  bool                                                            m_bNameInputFlag;

  // ----------------------------------------------------------------------------------------------
  NetAppLib::Message::MessageUtility*                             m_pJapanMonsNameMessageUtility;
  NetAppLib::Message::MessageUtility*                             m_pEnglishMonsNameMessageUtility;


};



GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSWORK_H_INCLUDE
