// ============================================================================
/*
 * @file NetAppPokemonValidationUtility.h
 * @brief ポケモンの不正チェックユーティリティ
 * @date 2016.01.14
 * @author endo_akira
 */
// ============================================================================
#if defined( GF_PLATFORM_CTR )
#if !defined( NETAPP_POKEMON_VALIDATION_UTILITY_H_INCLUDE )
#define NETAPP_POKEMON_VALIDATION_UTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <pml/include/pml_PokePara.h>
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

/*
  使い方サンプル( ポケモン単数 )

  if( m_NetAppPokemonValidationUtility.Verify( NetLib::Validation::PokemonValidation::E_GAME_MODE_TRADE, pPokemon ) )
  {
    NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_NetAppPokemonValidationUtility.GetVerifyResult();

    switch( eVerifyResult )
    {
      case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
      break;

      case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
      break;

      case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
      break;
    }
  }
*/

/*
  使い方サンプル( ポケモン複数 )

  if( m_NetAppPokemonValidationUtility.Verify( NetLib::Validation::PokemonValidation::E_GAME_MODE_TRADE, pPokemons, 3 ) )
  {
    for( u32 i = 0; i < 3; ++i )
    {
      NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_NetAppPokemonValidationUtility.GetVerifyResult(i);

      switch( eVerifyResult )
      {
        case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
        break;

        case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
        break;

        case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
        break;
      }
    }
  }
*/


/*
  このクラスを破棄するとき

  Proc の EndFunc などで CanDestroy の返り値が true になるまで待ちます

  if( m_NetAppPokemonValidationUtility.CanDestroy() )
  {
    // 破棄できる
  }
*/

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

//@fix GFNMCat[#5699 P2P交換（インターネット）の不正チェック時にエラーアプレットを表示し続けると相手を擬似的にはまり状態にできる]
class ServerErrorListener
{
public:
  //サーバエラーが発生した瞬間に通知する為だけのイベント
  virtual void OnServerError( void ) = 0;
};

class NetAppPokemonValidationUtility : public NetLib::Validation::PokemonValidationListener
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppPokemonValidationUtility);

public:

  enum VerifyResult
  {
    VERIFY_RESULT_OK,
    VERIFY_RESULT_NG,
    VERIFY_RESULT_NET_ERROR
  };

private:

  enum State
  {
    STATE_INITIALIZE,
    STATE_VERIFY_WAIT,
    STATE_COMPLETE
  };

public:

  NetAppPokemonValidationUtility( gfl2::heap::HeapBase* pHeap );
  virtual~NetAppPokemonValidationUtility();

  //! @brief   不正チェック
  //! @retval  true  : 不正チェック完了
  //! @retval  false : 不正チェック中
  //! @note    不正チェックが完了するまで呼び出し続けます。
  //!          不正チェックが完了したら呼び出さないでください。
  //!          不正チェックが完了して呼び出すと不正チェック処理が再び走ります。
  //!          不正チェックが完了した瞬間(1回)だけ true が返ります。
  bool Verify(
    NetLib::Validation::PokemonValidation::GameMode eGameMode,
    const pml::pokepara::CoreParam* pPokemons,
    u32 pokemonCount = 1 );

  bool Verify(
    NetLib::Validation::PokemonValidation::GameMode eGameMode,
    const NetApp::GTS::GtsIndexData* pGTSIndexData,
    const pml::pokepara::CoreParam* pPokemon );
#if PM_DEBUG
  bool Verify(
    NetLib::Validation::PokemonValidation::GameMode eGameMode,
    pml::PokeParty* pParty );

  int GetResultCode(const u32 index) { return m_resultCode[index]; }
#endif

  //! @brief   不正チェックの結果を取得
  //! @note    Verify 関数が true になった瞬間に取得して参照してください。
  VerifyResult GetVerifyResult( u32 index = 0 ) const;

  bool VerifySignature(
    bool& bVerifyResult,
    NetLib::Validation::PokemonValidation::GameMode eGameMode,
    u8* pSignature,
    u32 signatureSize,
    const pml::pokepara::CoreParam* pPokemons,
    u32 pokemonCount = 1 );

  //! @brief   破棄できるか
  //! @retval  true  : 破棄できる
  //! @retval  false : 破棄できない
  bool CanDestroy();


  u8* GetSignature();
  u32 GetSignatureSize();

  void SetServerErrorListener( ServerErrorListener* pListener ){ m_pServerErrorListener = pListener; }

  // NetLib::Validation::PokemonValidationListener
  virtual void OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum, const u8* certData, const u32 certDataSize );
  virtual void OnPokemonValidationError( const s32 httpStatus );
  virtual void OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode );
  virtual void OnPokemonValidationCanceled();

private:

  bool verify(
    NetLib::Validation::PokemonValidation::GameMode eGameMode,
    const NetApp::GTS::GtsIndexData* pGTSIndexData,
    const pml::pokepara::CoreParam* pPokemons,
    u32 pokemonCount );

  void setVerifyResult( u32 index, VerifyResult eVerifyResult );
  void setVerifyResult( VerifyResult eVerifyResult );
  void setState( State eState );

private:

  gfl2::heap::HeapBase*                               m_pHeap;
  NetLib::Validation::PokemonValidation*              m_pPokemonValidation;
  VerifyResult                                        m_aeVerifyResult[ NetLib::Validation::PokemonValidation::POKEMON_NUM ];
  State                                               m_eState;
  u8                                                  m_Signature[ NetLib::Validation::PokemonValidation::SIGNATURE_SIZE ];
  u32                                                 m_SignatureSize;
  bool                                                m_bVerifyResultSignature;
  ServerErrorListener*                                m_pServerErrorListener;
#if PM_DEBUG
  int                                                 m_resultCode[ NetLib::Validation::PokemonValidation::POKEMON_NUM ];
#endif
};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_POKEMON_VALIDATION_UTILITY_H_INCLUDE
#endif // defined( GF_PLATFORM_CTR )
