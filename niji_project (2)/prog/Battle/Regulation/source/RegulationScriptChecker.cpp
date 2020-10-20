// ============================================================================
/*
 * @file RegulationScriptChecker.cpp
 * @brief レギュレーションのスクリプトチェックを行います
 * @date 2015.12.11
 * @author endo_akira
 */
// ============================================================================
#include "Battle/Regulation/include/RegulationScriptChecker.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetInit.h"
#include "NetStatic/NetAppLib/include/QR/QRUtility.h"


pml::PokeParty* RegulationScriptChecker::s_pCheckPokeParty = NULL;

#if PM_DEBUG
bool RegulationScriptChecker::m_debugRegulationInvalidCert    = false; 
#endif

pml::PokeParty* RegulationScriptChecker::GetCheckPokeParty()
{
  return s_pCheckPokeParty;
}

RegulationScriptChecker::RegulationScriptChecker( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_pPawn( NULL ),
  m_pAmxBuffer( NULL ),
  m_eResultEntry( RegulationScriptChecker::RESULT_ENTRY_NG ),
  m_eResultLimit( RegulationScriptChecker::RESULT_LIMIT_NG ),
  m_aePoke(),
  m_aeItem(),
  m_CheckPokeParty( pHeap ),
  m_isVerifyNg(false)
{
  s_pCheckPokeParty = &m_CheckPokeParty;
}


RegulationScriptChecker::~RegulationScriptChecker()
{
  GFL_SAFE_DELETE( m_pPawn );

  GFL_SAFE_DELETE_ARRAY( m_pAmxBuffer );

  s_pCheckPokeParty = NULL;
}


void RegulationScriptChecker::CheckStart( Regulation* pRegulation, pml::PokeParty* pCheckPokeParty, Mode eMode )
{
  m_eResultEntry = RESULT_ENTRY_OK;
  m_eResultLimit = RESULT_LIMIT_OK;
  for( int i = 0; i < POKE_PARTY_MAX; ++i )
  {
    m_aePoke[i] = RESULT_POKEMON_OK;
    m_aeItem[i] = RESULT_ITEM_OK;
  }

  if( pRegulation )
  {
    void* pAmxData = pRegulation->GetAmxData();
    u32 amxSize = pRegulation->GetAmxDataSize();

    if( pAmxData && amxSize > 0 )
    {
      m_eResultEntry = RESULT_ENTRY_NG;
      m_eResultLimit = RESULT_LIMIT_NG;
      for( int i = 0; i < POKE_PARTY_MAX; ++i )
      {
        m_aePoke[i] = RESULT_POKEMON_NG;
        m_aeItem[i] = RESULT_ITEM_NG;
      }

      int nPokePartyCount = 0;
      int nMode = static_cast<int>( eMode );

      if( pCheckPokeParty )
      {
        m_CheckPokeParty.CopyFrom( *pCheckPokeParty );
        nPokePartyCount = static_cast<int>( m_CheckPokeParty.GetMemberCount() );
      }

      m_pAmxBuffer = GFL_NEW_ARRAY( m_pHeap ) u8[ amxSize - 8 ];

#if PM_DEBUG
      if( m_debugRegulationInvalidCert )
      {
        ((u8*)pAmxData)[0] = ~((u8*)pAmxData)[0];
      }
#endif

      bool bResult = NetApp::QR::QRUtility::VerifyBinary( pAmxData, amxSize, m_pAmxBuffer, m_pHeap, NetApp::QR::QRUtility::QR_CRYPTO_TYPE_REG );

#if PM_DEBUG
      if( m_debugRegulationInvalidCert )
      {
        ((u8*)pAmxData)[0] = ~((u8*)pAmxData)[0];
      }
#endif

      if( bResult )
      {
        m_pPawn = GFL_NEW( m_pHeap ) gfl2::pawn::Pawn( m_pHeap, m_pHeap );
        m_pPawn->Load( m_pAmxBuffer, amxSize - 8 );
        m_pPawn->RegisterPawnFunction( Field::FieldScript::FuncSetInit::GetTableForRegulation() );
        m_pPawn->SetGlobalParameterInteger( "g_mode", nMode );
        m_pPawn->SetGlobalParameterInteger( "g_pokePartyCount", nPokePartyCount );
        m_isVerifyNg = false;
      }
      else
      {
        GFL_SAFE_DELETE_ARRAY( m_pAmxBuffer );
        m_isVerifyNg = true;
      }
    }
  }
}


//! @retval true : チェック中
//! @retval false : チェック完了
bool RegulationScriptChecker::Update()
{
  bool bIsUpdate = false;

  if( m_pPawn )
  {
    bIsUpdate = true;

    s32 pawnResult = m_pPawn->Execute();

    if( pawnResult == gfl2::pawn::PawnBase::GFL_PAWN_FINISHED )
    {
      /*
      // リザルトコード
      public g_result = 1; // 0 : 参加条件を満たした, 1 : 参加条件を満たしていない

      // 指定したポケモンが指定したアイテムを所持しているかいないか調べる為のフラグ
      // g_pokeX == 0 && g_itemX == 0 ( 両方 0 で指定したポケモンが指定したアイテムを所持している )
      // g_pokeX == 0 && g_itemX == 1 ( g_pokeX が 0 で g_itemX が 1 で指定したポケモンは存在するが、指定したアイテムを所持していない )
      // g_pokeX == 1 && g_itemX == 1 ( 両方 1 で指定したポケモンが存在しないし、指定したアイテムも所持していない )
      public g_poke0 = 1;
      public g_item0 = 1;
      public g_poke1 = 1;
      public g_item1 = 1;
      public g_poke2 = 1;
      public g_item2 = 1;
      public g_poke3 = 1;
      public g_item3 = 1;
      public g_poke4 = 1;
      public g_item4 = 1;
      public g_poke5 = 1;
      public g_item5 = 1;

      // 上限下限のチェック結果
      // 0 : 上限下限のチェックに引っかからなかった
      // 1 : 上限下限の設定がされておらず、参加条件を満たす事ができなかった
      // 2 : 下限の設定がされており、下限の数を満たす事ができなかった
      // 3 : 上限の設定がされており、上限の数を超えてしまった
      // 4 : 上限下限の設定がされており、参加条件を満たす事ができなかった(上限下限の範囲外)
      public g_checkLimitResult = 1;
      */

      int aPoke[ POKE_PARTY_MAX ];
      int aItem[ POKE_PARTY_MAX ];

      int g_result              = m_pPawn->GetGlobalParameterInteger( "g_result" );
      int g_checkLimitResult    = m_pPawn->GetGlobalParameterInteger( "g_checkLimitResult" );
      aPoke[0]                  = m_pPawn->GetGlobalParameterInteger( "g_poke0" );
      aItem[0]                  = m_pPawn->GetGlobalParameterInteger( "g_item0" );
      aPoke[1]                  = m_pPawn->GetGlobalParameterInteger( "g_poke1" );
      aItem[1]                  = m_pPawn->GetGlobalParameterInteger( "g_item1" );
      aPoke[2]                  = m_pPawn->GetGlobalParameterInteger( "g_poke2" );
      aItem[2]                  = m_pPawn->GetGlobalParameterInteger( "g_item2" );
      aPoke[3]                  = m_pPawn->GetGlobalParameterInteger( "g_poke3" );
      aItem[3]                  = m_pPawn->GetGlobalParameterInteger( "g_item3" );
      aPoke[4]                  = m_pPawn->GetGlobalParameterInteger( "g_poke4" );
      aItem[4]                  = m_pPawn->GetGlobalParameterInteger( "g_item4" );
      aPoke[5]                  = m_pPawn->GetGlobalParameterInteger( "g_poke5" );
      aItem[5]                  = m_pPawn->GetGlobalParameterInteger( "g_item5" );

      m_eResultEntry = static_cast<RegulationScriptChecker::ResultEntry>( g_result );
      m_eResultLimit = static_cast<RegulationScriptChecker::ResultLimit>( g_checkLimitResult );

      for( int i = 0; i < POKE_PARTY_MAX; ++i )
      {
        m_aePoke[i] = static_cast<RegulationScriptChecker::ResultPokemon>( aPoke[i] );
        m_aeItem[i] = static_cast<RegulationScriptChecker::ResultItem>( aItem[i] );
      }

      bIsUpdate = false;

      GFL_SAFE_DELETE( m_pPawn );

      GFL_SAFE_DELETE_ARRAY( m_pAmxBuffer );
    }
  }

  return bIsUpdate;
}

