// ============================================================================
/*
 * @file TeamSelectValidationRequestSequence.cpp
 * @brief ポケパーティのレギュレーションチェックと不正チェックを行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetApp/TeamSelect/source/RequestSequence/TeamSelectValidationRequestSequence.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "battle/Regulation/include/Regulation.h"
#include "battle/Regulation/include/PokeRegulation.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

//------------------------------------------------------------------
/**
* @brief  コンストラクタ/デストラクタ
*/
//------------------------------------------------------------------
TeamSelectValidationRequestSequence::TeamSelectValidationRequestSequence()
: m_pListener( NULL )
, m_dummyWaitCount( 0 )
{
  memset( &m_requestParam , 0 , sizeof(m_requestParam) );

#if defined(GF_PLATFORM_CTR)
  m_pValidator = NULL;
#endif
}
TeamSelectValidationRequestSequence::~TeamSelectValidationRequestSequence()
{
#if defined(GF_PLATFORM_CTR)
  if( m_pValidator )
  {
    m_pValidator->Finalize();
  }
  GFL_SAFE_DELETE( m_pValidator );
#endif
}


//------------------------------------------------------------------
/**
* @brief  更新処理。
*
* @note   	メインループ毎にポーリングされる
*           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
*           UpdateCancelが呼ばれるようになります。
*/
//------------------------------------------------------------------
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE TeamSelectValidationRequestSequence::Update( void )
{
#if defined(GF_PLATFORM_CTR)

  if( m_dummyWaitCount++ < DUMMY_WAIT )
  {//ダミーウェイト
    return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
  }

  switch( GetSequenceNo() )
  {
  case SEQ_INIT:
    {
      gfl2::heap::HeapBase* pHeap = m_requestParam.pHeap;

      // レギュレーション検査クラス 
      PokeRegulation pokeRegulation( pHeap ); 

      // レギュレーションを設定する 
      pokeRegulation.SetRegulation( m_requestParam.pRegulation ); 


      //事前タマゴチェック
      bool isEggErrorExists = false;
      switch( m_requestParam.pRegulation->GetBattleCategory() )
      {
      case Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND:     //! ランダムマッチ[フリー]伝説あり
      case Regulation::BATTLE_CATEGORY_SPOT_FREE:            //! ランダムマッチ[フリー]伝説無し
      case Regulation::BATTLE_CATEGORY_SPOT_RATE:            //! ランダムマッチ[レーティング]
      case Regulation::BATTLE_CATEGORY_SPOT_SP:              //! ランダムマッチ[スペシャル]
      case Regulation::BATTLE_CATEGORY_LIVE_CUP:             //! ライブ大会
      case Regulation::BATTLE_CATEGORY_NET_CUP:              //! インターネット大会
      case Regulation::BATTLE_CATEGORY_GROUP_CUP:            //! 仲間大会
      case Regulation::BATTLE_CATEGORY_WCS:                  //! ランダムマッチ[レーティング]WCSルール
        {//これらはタマゴチェックする
         
          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            MonsNo monsNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetMonsNo(); 
            if( monsNo == MONSNO_NULL )
              continue;

            bool isEgg = m_requestParam.pPokeParty->GetMemberPointerConst( index )->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
            if( isEgg == true )
            {
              isEggErrorExists = true;
              break;
            }
          }
        }
        break;
      default:
        break;
      }

      //=================================================
      // momiji追加：P2P対戦時の互換対応チェック
      //=================================================
      {
        NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager );

        // P2P通信対戦でのチェック(=必ずP2P接続されている）
        if( pP2pConnectionManager->IsConnectSuccess() == true )
        {
          // フェスのP2P通信対戦のみチェック
          switch( pP2pConnectionManager->GetP2PID() ){
          case NetLib::P2P::NIJI_P2P_JF_QUICK_2:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
          case NetLib::P2P::NIJI_P2P_JF_QUICK_4:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
          case NetLib::P2P::NIJI_P2P_BATTLE_2_L:    ///< バトル（2人接続）（ローカル）
          case NetLib::P2P::NIJI_P2P_BATTLE_2_W:    ///< バトル（2人接続）（WiFi）
          case NetLib::P2P::NIJI_P2P_BATTLE_4_L:    ///< バトル（4人接続）（ローカル）
          case NetLib::P2P::NIJI_P2P_BATTLE_4_W:    ///< バトル（4人接続）（WiFi）
            {
              // 相手にnijiが混ざっている時
              if( pP2pConnectionManager->IsNijiConnect() )
              {
                // 互換エラーポケモンがいるかのみチェック
                bool isNijiCompatibleError = false;
                for( u8 i = 0; i < m_requestParam.pPokeParty->GetMemberCount(); i++ )
                {
                  pml::pokepara::PokemonParam *pPokePara = m_requestParam.pPokeParty->GetMemberPointer( i );
                  if( PokeRegulation::CheckNijiCompatible( pPokePara ) )
                  {
                    isNijiCompatibleError = true;
                    break;
                  }
                }

                // 互換エラーポケモンがいる場合、エラー詳細を決めて通知
                if( isNijiCompatibleError == true )
                {
                  REGULATION_CHECK_RESULT checkResultArray[REGULATION_NG_TYPE_MAX];
                  ::std::memset( checkResultArray, 0, sizeof( checkResultArray ) );
                  u32 NGTypeNum = 0;

                  for( u8 i = 0; i < m_requestParam.pPokeParty->GetMemberCount(); i++ )
                  {
                    pml::pokepara::PokemonParam *pPokePara = m_requestParam.pPokeParty->GetMemberPointer( i );
                    if( PokeRegulation::CheckNijiCompatible( pPokePara ) )
                    {
                      // @fix MMCat[88]：対象のNGポケモンにマークを付ける
                      checkResultArray[NGTypeNum].isPokeNG[i] = true;
                    }
                  }

                  // 通信互換違反のエラーを渡して終了。
                  checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_COMPATIBLE;
                  NGTypeNum++; // 必要

                  if( m_pListener )
                  {
                    m_pListener->OnRegulationNG( NGTypeNum, checkResultArray );
                  }

                  return gflnet2::ServerClient::REQUEST_STATE_FINISH;

                } // if

              }
            }
          }
        }

        
      }

      // 検査1 
      PokeRegulation::ViolationDetails violationDetails; // エラー詳細データ 
      if( pokeRegulation.CheckPokePartyForBtlTeam( m_requestParam.pPokeParty , violationDetails ) == false || isEggErrorExists )
      {//異常

        u32 NGTypeNum =0;
        REGULATION_CHECK_RESULT checkResultArray[REGULATION_NG_TYPE_MAX];
        ::std::memset( checkResultArray , 0 , sizeof(checkResultArray) );

        if( violationDetails.isViolationNumLo )//! 数が足りてない
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_NUM_LOW;
          NGTypeNum++;
        }
        if( violationDetails.isViolationNumHi )//! 数がオーバー
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_NUM_HI;
          NGTypeNum++;
        }
        if( violationDetails.isViolationMustPoke )//! 必須ポケモンがいない
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_MAST_POKE;
          NGTypeNum++;
        }
        if( violationDetails.isViolationVetoWaza )//! 禁止わざ
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_VETO_WAZA;

          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            MonsNo monsNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetMonsNo(); 
            if( monsNo == MONSNO_NULL )
              continue;

            // @fix NMCat[1235]：対象のNGポケモンにマークを付ける
            if( violationDetails.vetoWazaIndex[index] == true )
            {//NGに該当する
              checkResultArray[NGTypeNum].isPokeNG[index] = true;
            }
          }

          NGTypeNum++;
        }
        if( violationDetails.isViolationVetoPoke )//! 禁止ポケモン
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_VETO_POKE;

          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            MonsNo monsNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetMonsNo();
            if( monsNo == MONSNO_NULL )
              continue;

            // @fix NMCat[2004]：対象のNGポケモンにマークを付ける
            if( violationDetails.vetoPokeIndex[index] == true )
            {//NGに該当する
              checkResultArray[NGTypeNum].isPokeNG[index] = true;
            }
          }

          NGTypeNum++;
        }
        if( violationDetails.isViolationVetoItem )//! 禁止アイテム
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_VETO_ITEM;

          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            u16 itemNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetItem(); 
            if( itemNo == ITEM_DUMMY_DATA )
              continue;

            // @fix NMCat[1235]：対象のNGポケモンにマークを付ける
            if( violationDetails.vetoItemIndex[index] == true )
            {//NGに該当する
              checkResultArray[NGTypeNum].isPokeNG[index] = true;
            }
          }

          NGTypeNum++;
        }
        if( violationDetails.isViolationLevel )//! レベル違反のポケモンがいる
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_LEVEL;

          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            MonsNo monsNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetMonsNo(); 
            if( monsNo == MONSNO_NULL )
              continue;

            // @fix NMCat[1235]：対象のNGポケモンにマークを付ける
            if( violationDetails.levelIndex[index] == true )
            {//NGに該当する
              checkResultArray[NGTypeNum].isPokeNG[index] = true;
            }
          }

          NGTypeNum++;
        }
        if( violationDetails.isViolationBothPoke )//! 同じポケモン
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_BOTHPOKE;

          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            MonsNo monsNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetMonsNo(); 
            if( monsNo == MONSNO_NULL )
              continue;

            for( u32 vetoIndex = 0 ; vetoIndex < pml::PokeParty::MAX_MEMBERS ; ++vetoIndex )
            {
              if( monsNo == violationDetails.bothPoke[vetoIndex] )
              {//NGに該当する
                checkResultArray[NGTypeNum].isPokeNG[index] = true;
              }
            }
          }

          NGTypeNum++;
        }

        if( violationDetails.isViolationBothItem )//! 同じアイテム
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_BOTHITEM;

          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
          {
            u16 itemNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetItem(); 
            if( itemNo == ITEM_DUMMY_DATA )
              continue;

            for( u32 vetoIndex = 0 ; vetoIndex < pml::PokeParty::MAX_MEMBERS ; ++vetoIndex )
            {
              if( itemNo == violationDetails.bothItem[vetoIndex] )
              {//NGに該当する
                checkResultArray[NGTypeNum].isPokeNG[index] = true;
              }
            }
          }

          NGTypeNum++;
        }

        if( violationDetails.isViolationNumLegend )//! 伝説の数がオーバー
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_NUM_LEGEND;
          NGTypeNum++;
        }
        if( violationDetails.isViolationQR )//! QRバトルチームは使えない
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_QR;
          NGTypeNum++;
        }
        if( violationDetails.isViolationExScript )//! 拡張レギュレーション違反
        {
          checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_SPECIAL;
          NGTypeNum++;
        }

        //タマゴチェック
        switch( m_requestParam.pRegulation->GetBattleCategory() )
        {
        case Regulation::BATTLE_CATEGORY_SPOT_FREE_LEGEND:     //! ランダムマッチ[フリー]伝説あり
        case Regulation::BATTLE_CATEGORY_SPOT_FREE:            //! ランダムマッチ[フリー]伝説無し
        case Regulation::BATTLE_CATEGORY_SPOT_RATE:            //! ランダムマッチ[レーティング]
        case Regulation::BATTLE_CATEGORY_SPOT_SP:              //! ランダムマッチ[スペシャル]
        case Regulation::BATTLE_CATEGORY_LIVE_CUP:             //! ライブ大会
        case Regulation::BATTLE_CATEGORY_NET_CUP:              //! インターネット大会
        case Regulation::BATTLE_CATEGORY_GROUP_CUP:            //! 仲間大会
        case Regulation::BATTLE_CATEGORY_WCS:                  //! ランダムマッチ[レーティング]WCSルール
          {//これらはタマゴチェックする
            bool isEggError = false;
            for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; ++index )
            {
              MonsNo monsNo = m_requestParam.pPokeParty->GetMemberPointerConst( index )->GetMonsNo(); 
              if( monsNo == MONSNO_NULL )
                continue;

              bool isEgg = m_requestParam.pPokeParty->GetMemberPointerConst( index )->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
              if( isEgg == true )
              {
                checkResultArray[NGTypeNum].isPokeNG[index] = true;
                isEggError = true;
              }
            }

            if( isEggError == true )
            {
              checkResultArray[NGTypeNum].NGType = REGULATION_NG_TYPE_EGG;
              NGTypeNum++;
            }
          }
        break;
        case Regulation::BATTLE_CATEGORY_TREE_NORMAL:          //! バトルツリーノーマル
        case Regulation::BATTLE_CATEGORY_TREE_SUPER:           //! バトルツリースーパー
        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL://! バトルロイヤル施設ノーマル
        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_SUPER: //! バトルロイヤル施設スーパー
        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_HYPER: //! バトルロイヤル施設ハイパー
        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_MASTER://! バトルロイヤル施設マスター
        case Regulation::BATTLE_CATEGORY_NET_BTL:              //! 通信対戦(P2P)
        case Regulation::BATTLE_CATEGORY_DELIVERY:             //! 配信レギュレーション
        default:
        break;
        }

        if( m_pListener )
        {
          m_pListener->OnRegulationNG( NGTypeNum , checkResultArray );
        }

        return gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }
      else
      {//正常
        if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
        {//不正チェックする

          NetLib::Validation::PokemonValidation::GameMode mode = NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M;

          //=================================================
          // momiji追加：不正チェックサーバmomiji対応
          //=================================================
          {
            NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager );

            // P2P通信対戦でのチェック(=必ずP2P接続されている）
            if( pP2pConnectionManager->IsConnectSuccess() == true )
            {
              // フェスのP2P通信対戦のみチェック
              switch( pP2pConnectionManager->GetP2PID() ){
              case NetLib::P2P::NIJI_P2P_JF_QUICK_2:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
              case NetLib::P2P::NIJI_P2P_JF_QUICK_4:    ///< ジョインフェスタ  JF,battle,tradeは同じ番号である必要がある
              case NetLib::P2P::NIJI_P2P_BATTLE_2_L:    ///< バトル（2人接続）（ローカル）
              case NetLib::P2P::NIJI_P2P_BATTLE_2_W:    ///< バトル（2人接続）（WiFi）
              case NetLib::P2P::NIJI_P2P_BATTLE_4_L:    ///< バトル（4人接続）（ローカル）
              case NetLib::P2P::NIJI_P2P_BATTLE_4_W:    ///< バトル（4人接続）（WiFi）
                {
                  // 相手にnijiが混ざっている時
                  if( pP2pConnectionManager->IsNijiConnect() )
                  {
                    mode = NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE;
                  }
                }
              }
              // その他のバトルスポットや大会はmomijiモードになる。
            }
          }

          
          if( m_requestParam.isQRTeam == true )
          {
            mode = ( mode == NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M ) ? NetLib::Validation::PokemonValidation::E_GAME_MODE_QR_TEAM_M : NetLib::Validation::PokemonValidation::E_GAME_MODE_QR_TEAM;
          }

          GFL_PRINT("ValidationMode[%d]\n",mode);

          m_pValidator = GFL_NEW( m_requestParam.pHeap ) NetLib::Validation::PokemonValidation();
          m_pValidator->Initialize( m_requestParam.pHeap , m_requestParam.pHeap , 
            GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken() ,
            mode
            );
          m_pValidator->SetPokemonValidationListener( this );
          NetApp::GTS::GtsSerializePokemonParam tmpBuffer;
          for( u32 index = 0 ; index < m_requestParam.pPokeParty->GetMemberCount() ; ++index )
          {
            m_requestParam.pPokeParty->GetMemberPointer( index )->Serialize_Core( tmpBuffer.data );
            m_pValidator->SetPokemon( index , NULL , &tmpBuffer );
          }
          
          m_pValidator->CheckPokemonASync( m_requestParam.pHeap , m_requestParam.pPokeParty->GetMemberCount() );

          SetSequenceNo( SEQ_VALIDATION_WAIT );
        }
        else
        {//不正チェックしない
          SetSequenceNo( SEQ_VALIDATION_VALID );
        }
      }
    }
  break;
  case SEQ_VALIDATION_WAIT:
  break;
  case SEQ_VALIDATION_VALID:
    {
      if( m_pListener )
      {
        m_pListener->OnCheckSuccess();
      }

      return gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
  //break;
  case SEQ_VALIDATION_INVALID:
    {
      if( m_pListener )
      {
        m_pListener->OnValidationNG();
      }

      return gflnet2::ServerClient::REQUEST_STATE_FINISH;
    }
  //break;
  }

#endif
  return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
}

//------------------------------------------------------------------
/**
* @brief  キャンセル処理の更新
*
* @note   	
*			      リクエストをキャンセルするとUpdateではなくこちらが呼ばれるようになります。
*           キャンセル処理が完了したらREQUEST_STATE_FINISHを返すとリクエストは終了します。
*           キャンセルが行われなかった場合、この関数は呼ばれません。
*/
//------------------------------------------------------------------
gflnet2::ServerClient::REQUEST_SEQUENCE_STATE TeamSelectValidationRequestSequence::UpdateCancel( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_pValidator )
  {
    m_pValidator->CancelConnecting();
    if( m_pValidator->IsConnecting() )
    {
      return gflnet2::ServerClient::REQUEST_STATE_CONTINUE;
    }

    m_pValidator->Finalize();
    GFL_SAFE_DELETE( m_pValidator );
  }

  if( m_pListener )
  {
    m_pListener->OnError();
  }

#endif
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}


//------------------------------------------------------------------
/**
* @brief  リスナー登録解除
*/
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  リスナー設定
*/
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::SetListener( TeamSelectResponseListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief  リクエストパラメータ設定
*/
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::SetRequestParam( TeamSelectRequestClient::CHECK_REQUEST_PARAM& pParam )
{
  m_requestParam = pParam;
}


#if defined(GF_PLATFORM_CTR)

//------------------------------------------------------------------
/**
  * @brief ポケモン不正チェック処理の完了イベントを検知
  *
  * @note  本イベント終了後、[checkResult]配列、および[certData]は解放される
  *
  * @param[in]  checkResult  個々のポケモンの検査結果が配列で格納される。
  * @param[in]  resultNum    検査結果配列の数
  * @param[in]  certData     署名データ
  * @param[in]  certDataSize 署名データサイズ。この値が0の場合は、不正なポケモンデータであることを指す。
  */
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum,
                                        const u8* certData, const u32 certDataSize )
{
  if( certDataSize == 0 )
  {
    SetSequenceNo( SEQ_VALIDATION_INVALID );
  }
  else
  {
    std::memcpy( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetSignature() , certData , NetLib::Validation::PokemonValidation::SIGNATURE_SIZE );
    SetSequenceNo( SEQ_VALIDATION_VALID );

    bool ret = m_pValidator->CheckRsaValidation( m_requestParam.pPokeParty->GetMemberCount(), certData, certDataSize );
    ICHI_PRINT(">ICHI 結果 %d \n", ret );

  }
}

//------------------------------------------------------------------
/**
* @brief ポケモン不正チェック処理のエラーイベントを検知
*
* @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
*
* @param[in] httpStatus サーバから返却されたHTTPステータスコード
*/
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::OnPokemonValidationError( const s32 httpStatus )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultValidation5xx( httpStatus );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
  Cancel();
}

//------------------------------------------------------------------
/**
* @brief ポケモン不正チェック処理のエラーイベントを検知
*
* @param[in] result    エラー内容
* @param[in] errorCode ネットワークエラーコード
*/
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
  Cancel();
}

//------------------------------------------------------------------
/**
* @brief ポケモン不正チェック処理のキャンセルイベントを検知
*/
//------------------------------------------------------------------
void TeamSelectValidationRequestSequence::OnPokemonValidationCanceled( void )
{
  Cancel();
}

#endif

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
