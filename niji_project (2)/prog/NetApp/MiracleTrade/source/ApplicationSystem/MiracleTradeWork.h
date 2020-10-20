// ============================================================================
/*
 * @file MiracleTradeWork.h
 * @brief ミラクル交換アプリを動かす為のワーククラスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEWORK_H_INCLUDE )
#define MIRACLETRADEWORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include "NetApp/MiracleTrade/source/MiracleTradeDefine.h"
#include "pml/include/pokepara/pml_PokemonParam.h"
#include "Savedata/include/MyStatus.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class PokemonSelectInfo
{
public:
  u32  tray;
  u32  pos;
  bool inBox;
  PokemonSelectInfo() : tray( 0 ), pos( 0 ), inBox( false ){}
};

class MiracleTradeWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::MiracleTrade::MIRACLETRADE_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::MiracleTrade::MIRACLETRADE_APP_DEVICE_HEAP_SIZE;

public:

  enum BootProcID
  {
    BOOT_PROC_ID_NONE,
    BOOT_PROC_ID_BOX,
    BOOT_PROC_ID_DEMO,
  };

public:
  MiracleTradeWork( gfl2::heap::HeapBase* pWorkHeap ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
    m_PokemonSelectInfo(),
    m_SelectPokemonCoreParam( pWorkHeap ),
    m_ReceivedPokemonCoreParam( pWorkHeap ),
    m_ReceivedPokemonParam( pWorkHeap, m_ReceivedPokemonCoreParam ),
    m_TradePersonStatus(),
    m_TradePersonJoinFestaPacketData(),
    m_Signature(),
    m_SignatureSize( 0 ),
    m_TradePersonSignature(),
    m_TradePersonSignatureSize( 0 ),
    m_TradePersonHelloPhraseMessageID( 0 ),
    m_TradePersonEmotionPhraseMessageID( 0 ),
    m_pPokeModelUtility( NULL ),
    m_pTrainerIconUtility( NULL ),
    m_pRootUpperView( NULL ),
    m_pRootLowerView( NULL ),
    m_eBootProcID( BOOT_PROC_ID_NONE ),
    m_bCallZukanRegister( false ),
    m_ZukanRegisterEventParam(),
    m_bCallEvolution( false ),
    m_EvolutionEventParam()
  {
  }

  virtual ~MiracleTradeWork()
  {
  }

  NetApp::MiracleTrade::ApplicationSystem::PokemonSelectInfo& GetPokemonSelectInfo() { return m_PokemonSelectInfo; }
  pml::pokepara::CoreParam& GetSelectPokemonCoreParam() { return m_SelectPokemonCoreParam; }
  pml::pokepara::CoreParam& GetReceivedPokemonCoreParam() { return m_ReceivedPokemonCoreParam; }
  pml::pokepara::PokemonParam& GetReceivedPokemonParam() { return m_ReceivedPokemonParam; }
  Savedata::MyStatus& GetTradePersonStatus() { return m_TradePersonStatus; }
  NetAppLib::JoinFesta::JoinFestaPacketData& GetTradePersonJoinFestaPacketData() { return m_TradePersonJoinFestaPacketData; }

  u8* GetSignature() { return m_Signature; }
  u32 GetSignatureSize() { return m_SignatureSize; }
  void SetSignatureSize( u32 size ){ m_SignatureSize = size; }

  u8* GetTradePersonSignature() { return m_TradePersonSignature; }
  u32 GetTradePersonSignatureSize() { return m_TradePersonSignatureSize; }
  void SetTradePersonSignatureSize( u32 size ){ m_TradePersonSignatureSize = size; }

  u32 GetHelloPhraseMessageID() { return m_TradePersonHelloPhraseMessageID; }
  void SetHelloPhraseMessageID( u32 messageID ){ m_TradePersonHelloPhraseMessageID = messageID; }

  u32 GetEmotionPhraseMessageID() { return m_TradePersonEmotionPhraseMessageID; }
  void SetEmotionPhraseMessageID( u32 messageID ){ m_TradePersonEmotionPhraseMessageID = messageID; }

  // @note : GetPokeModelUtility で取得して使用する際に NULL チェックをする必要があります
  NetAppLib::Util::NetAppPokeModelUtility* GetPokeModelUtility() { return m_pPokeModelUtility; }
  void SetPokeModelUtility( NetAppLib::Util::NetAppPokeModelUtility* pPokeModelUtility ){ m_pPokeModelUtility = pPokeModelUtility; }

  NetAppLib::Util::NetAppTrainerIconUtility* GetTrainerIconUtility() { return m_pTrainerIconUtility; }
  void SetTrainerIconUtility( NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility ){ m_pTrainerIconUtility = pTrainerIconUtility; }

  NetAppLib::UI::NetAppEmptyView* GetRootUpperView() const { return m_pRootUpperView; }
  void SetRootUpperView( NetAppLib::UI::NetAppEmptyView* pRootUpperView ) { m_pRootUpperView = pRootUpperView; }

  NetAppLib::UI::NetAppEmptyView* GetRootLowerView() const { return m_pRootLowerView; }
  void SetRootLowerView( NetAppLib::UI::NetAppEmptyView* pRootLowerView ) { m_pRootLowerView = pRootLowerView; }

  BootProcID GetBootProcID() const { return m_eBootProcID; }
  void SetBootProcID( BootProcID eBootProcID ) { m_eBootProcID = eBootProcID; }

  bool& GetCallZukanRegister() { return m_bCallZukanRegister; }
  app::event::ZukanRegisterEventParam& GetZukanRegisterEventParam() { return m_ZukanRegisterEventParam; }

  bool& GetCallEvolution() { return m_bCallEvolution; }
  NetEvent::Evolution::EvolutionEventParam& GetEvolutionEventParam() { return m_EvolutionEventParam; }

private:
  NetApp::MiracleTrade::ApplicationSystem::PokemonSelectInfo    m_PokemonSelectInfo;        //! 選択情報
  pml::pokepara::CoreParam                                      m_SelectPokemonCoreParam;   //! Boxアプリで選択したポケモン(送信するポケモン)
  pml::pokepara::CoreParam                                      m_ReceivedPokemonCoreParam; //! 受信したポケモン
  pml::pokepara::PokemonParam                                   m_ReceivedPokemonParam;     //! 受信したポケモン
  Savedata::MyStatus                                            m_TradePersonStatus;        //! 交換相手のステータス
  NetAppLib::JoinFesta::JoinFestaPacketData                     m_TradePersonJoinFestaPacketData;   //! 交換相手のパーソナルパケットデータ
  u8                                                            m_Signature[ NetApp::MiracleTrade::SIGNATRUE_SIZE ];
  u32                                                           m_SignatureSize;
  u8                                                            m_TradePersonSignature[ NetApp::MiracleTrade::SIGNATRUE_SIZE ];
  u32                                                           m_TradePersonSignatureSize;
  u32                                                           m_TradePersonHelloPhraseMessageID;
  u32                                                           m_TradePersonEmotionPhraseMessageID;

  NetAppLib::Util::NetAppPokeModelUtility*                      m_pPokeModelUtility;
  NetAppLib::Util::NetAppTrainerIconUtility*                    m_pTrainerIconUtility;

  NetAppLib::UI::NetAppEmptyView*                               m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                               m_pRootLowerView;

  BootProcID                                                    m_eBootProcID;
  bool                                                          m_bCallZukanRegister;
  app::event::ZukanRegisterEventParam                           m_ZukanRegisterEventParam;
  bool                                                          m_bCallEvolution;
  NetEvent::Evolution::EvolutionEventParam                      m_EvolutionEventParam;


};



GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEWORK_H_INCLUDE
