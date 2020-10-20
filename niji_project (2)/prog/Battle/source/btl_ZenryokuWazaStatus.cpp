//=============================================================================
/**
 * @file    btl_ZenryokuWazaStatus.cpp
 * @brief   全力技の使用状況を管理するクラス
 * @author  obata_toshihiro
 * @date    2015.05.27
 */
//=============================================================================
#include <Savedata/include/MyStatus.h>
#include "./btl_common.h"
#include "./btl_pokeparam.h"
#include "./btl_ZenryokuWaza.h"
#include "./btl_ZenryokuWazaStatus.h"


namespace btl {

 
  //-------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param setupParam  セットアップパラメータ
   */
  //-------------------------------------------------------------------------
  ZenryokuWazaStatus::ZenryokuWazaStatus( const SetupParam& setupParam ) : 
    m_haveZenryokuRing( setupParam.haveZenryokuRing ),
    m_isZenryokuWazaUsed( false ),
    m_isSecondZenryokuWazaUsed( false ),
    m_isCanSecondZenryokuWaza( false )
  {
  }
  
  //-------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------
  ZenryokuWazaStatus::~ZenryokuWazaStatus()
  {
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 初期状態に戻す
   */
  //-------------------------------------------------------------------------
  void ZenryokuWazaStatus::Clear( void )
  {
    m_isZenryokuWazaUsed = false;
    m_isSecondZenryokuWazaUsed = false;
    m_isCanSecondZenryokuWaza = false;
  }
  
  //-------------------------------------------------------------------------
  /**
   * @brief 全力技を使用したか？
   * @retval true   すでに全力技を使用した
   * @retval false  まだ全力技を使用していない
   */
  //-------------------------------------------------------------------------
  bool ZenryokuWazaStatus::IsZenryokuWazaUsed( void ) const
  {
    return m_isZenryokuWazaUsed && IsSecondZenryokuWazaUsed();
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 全力技を使用済みであることを登録する
   */
  //-------------------------------------------------------------------------
  void ZenryokuWazaStatus::SetZenryokuWazaUsed( void )
  {
    if (!IsSecondZenryokuWazaUsed())
    {
      SetSecondZenryokuWazaUsed();
      return;
    }
    m_isZenryokuWazaUsed = true;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 全力技として使用できる技が存在するか？
   * @param pokeParam  技を使用するポケモン
   * @retval true   全力技を使用できる
   * @retval false  全力技を使用できない
   */
  //-------------------------------------------------------------------------
  bool ZenryokuWazaStatus::CanUseZenryokuWaza( const BTL_POKEPARAM& pokeParam ) const
  {
    u8 wazaCount = pokeParam.WAZA_GetCount();
    for( u8 wazaIndex=0; wazaIndex<wazaCount; ++wazaIndex )
    {
      if( this->CanUseZenryokuWaza( pokeParam, wazaIndex ) ) {
        return true;
      }
    }
    return false;
  }
  
  //-------------------------------------------------------------------------
  /**
   * @brief 全力技を使用できるか？
   * @param pokeParam  技を使用するポケモン
   * @param wazaIndex  使用する技のインデックス
   * @retval true   全力技を使用できる
   * @retval false  全力技を使用できない
   */
  //-------------------------------------------------------------------------
  bool ZenryokuWazaStatus::CanUseZenryokuWaza( const BTL_POKEPARAM& pokeParam, u8 wazaIndex ) const
  {
    WazaNo zenryokuWazaNo = this->GetZenryokuWazaNo( pokeParam, wazaIndex );
    return ( zenryokuWazaNo != WAZANO_NULL );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 全力技に変換後の技No.を取得する
   * @param pokeParam  技を使用するポケモン
   * @param wazaIndex  使用する技のインデックス
   * @return 全力技に変換後の技No.
   * @retval WAZANO_NULL  全力技に変換できない場合
   */
  //-------------------------------------------------------------------------
  WazaNo ZenryokuWazaStatus::GetZenryokuWazaNo( const BTL_POKEPARAM& pokeParam, u8 wazaIndex ) const
  {
    // ぜんりょくリングを持っていない
    if( !m_haveZenryokuRing ) {
      return WAZANO_NULL;
    }

    // すでに使用済み
    if( this->IsZenryokuWazaUsed() ) {
      return WAZANO_NULL;
    }

    // メガシンカ or ゲンシカイキ or ウルトラバーストしたポケモンに「へんしん」している場合は、Z技を使用できない
    if( pokeParam.HENSIN_Check_Mega() ||
        pokeParam.HENSIN_Check_GensiKaiki() ||
        pokeParam.HENSIN_Check_UltraBurst() )
    {
      return WAZANO_NULL;
    }

    WazaNo wazaNo = pokeParam.WAZA_GetID( wazaIndex );
    return ZenryokuWaza::GetZenryokuWazaNo( pokeParam, wazaNo );
  }

  //-------------------------------------------------------------------------
  /**
  * @brief 二度目の全力技を使用したか？
  * @retval true   すでに全力技を使用した（又は二度目の技使用不可）
  * @retval false  まだ全力技を使用していない
  */
  //-------------------------------------------------------------------------
  bool ZenryokuWazaStatus::IsSecondZenryokuWazaUsed(void) const
  {
    if (m_isCanSecondZenryokuWaza)
    {
      return m_isSecondZenryokuWazaUsed;
    }
    return true;
  }

  //-------------------------------------------------------------------------
  /**
  * @brief 二度目の全力技を使用済みであることを登録する
  */
  //-------------------------------------------------------------------------
  void ZenryokuWazaStatus::SetSecondZenryokuWazaUsed(void)
  {
    m_isSecondZenryokuWazaUsed = true;
  }

  //-------------------------------------------------------------------------
  /**
  * @brief 二度目の全力技を使用可能であることを登録する
  */
  //-------------------------------------------------------------------------
  void ZenryokuWazaStatus::SetCanSecondZenryokuWaza(void)
  {
    m_isCanSecondZenryokuWaza = true;
  }

} // namespace btl