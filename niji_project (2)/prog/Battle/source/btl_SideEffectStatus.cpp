//-----------------------------------------------------------------------------
/**
 * @file   btl_SideEffectStatus.cpp
 * @brief  １種類のサイドエフェクトの状態
 * @author obata_toshihiro
 * @date   2015.06.16
 */
//-----------------------------------------------------------------------------
#include <util/include/gfl2_std_string.h>

#include "./btl_SideEffect.h"
#include "./btl_SideEffectStatus.h"


namespace btl {


  //-------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param sideEffect  管理対象のサイドエフェクト
   */
  //-------------------------------------------------------------------------
  SideEffectStatus::SideEffectStatus( BtlSideEffect sideEffect ) :
    m_sideEffect( sideEffect ),
    m_contParam(),
    m_turn_counter( 0 ),
    m_add_counter( 0 )
  {
    gfl2::std::MemClear( &m_contParam, sizeof( BTL_SICKCONT ) );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------
  SideEffectStatus::~SideEffectStatus()
  {
  }

  




  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトを追加する
   *        すでに効果を発揮している場合、重ねがけする
   *
   * @param contParam  サイドエフェクトの継続パラメータ
   *
   * @retval true  サイドエフェクトを追加( 重ねがけ )した
   * @retval false サイドエフェクトを追加( 重ねがけ )できなかった
   */
  //-------------------------------------------------------------------------
  bool SideEffectStatus::AddEffect( const BTL_SICKCONT& contParam )
  {
    if( m_add_counter == 0 )
    {
      m_add_counter = 1;
      m_turn_counter = 0;
      m_contParam = contParam;
      return true;
    }
    else if( m_add_counter < SideEffect::GetMaxAddCount( m_sideEffect ) )
    {
      m_add_counter++;
      return true;
    }
    return false;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトを解除する
   *
   * @retval true  サイドエフェクトを解除した
   * @retval false サイドエフェクトを解除できなかった( もともと有効でなかった )
   */
  //-------------------------------------------------------------------------
  bool SideEffectStatus::RemoveEffect( void )
  {
    if( m_add_counter <= 0 ) {
      return false;
    }
    m_add_counter = 0;
    m_turn_counter = 0;
    m_contParam.type = pml::wazadata::SICKCONT_NONE;
    return true;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトが有効か？
   * @retval true  有効
   * @retval false 有効でない
   */
  //-------------------------------------------------------------------------
  bool SideEffectStatus::IsEffective( void ) const
  {
    return ( 0 < m_add_counter );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 重ねがけ数を取得する
   */
  //-------------------------------------------------------------------------
  u32 SideEffectStatus::GetAddCount( void ) const
  {
    return m_add_counter;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトの効果ターン数を取得する
   */
  //-------------------------------------------------------------------------
  u32 SideEffectStatus::GetMaxTurnCount( void ) const
  {
    return m_contParam.turn.count;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief ターン経過数を取得する
   */
  //-------------------------------------------------------------------------
  u32 SideEffectStatus::GetCurrentTurnCount( void ) const
  {
    return m_turn_counter;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 残りターン数を取得する
   */
  //-------------------------------------------------------------------------
  u32 SideEffectStatus::GetRemainingTurn( void ) const
  {
    const u32 totalTurn = this->GetMaxTurnCount();
    const u32 passedTurn = this->GetCurrentTurnCount();
    const u32 remainTurn = ( totalTurn - passedTurn );
    return remainTurn;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 継続ターン数の増加分を取得する
   */
  //-------------------------------------------------------------------------
  u32 SideEffectStatus::GetTurnUpCount( void ) const
  {
    const BTL_SICKCONT& sickcont = this->GetContParam();
    return SICKCONT_GetParam( sickcont ); // ターン数の増加分は汎用パラメータにセットされている
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 効果を引き起こしたポケモンのIDを取得する
   */
  //-------------------------------------------------------------------------
  u8 SideEffectStatus::GetCausePokeID( void ) const
  {
    const BTL_SICKCONT& sickcont = this->GetContParam();
    return SICKCONT_GetCausePokeID( sickcont );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief ターン経過数をインクリメントする
   */
  //-------------------------------------------------------------------------
  void SideEffectStatus::IncTurnCount( void )
  {
    ++m_turn_counter;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 効果ターン数を経過したか？
   * @retval true   効果ターン数を経過した
   * @retval false  効果ターン数を経過していない
   */
  //-------------------------------------------------------------------------
  bool SideEffectStatus::IsTurnPassed( void ) const
  {
    u32 max = this->GetMaxTurnCount();
    u32 current = this->GetCurrentTurnCount();
    return ( max <= current );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトの継続パラメータを取得する
   */
  //-------------------------------------------------------------------------
  const BTL_SICKCONT& SideEffectStatus::GetContParam( void ) const
  {
    return m_contParam;
  }




} // namespace btl