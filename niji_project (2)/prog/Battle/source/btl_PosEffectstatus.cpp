//-----------------------------------------------------------------------------
/**
 * @file   btl_PosEffectStatus.cpp
 * @brief  １種類の位置エフェクトの状態
 * @author obata_toshihiro
 * @date   2015.06.22
 */
//-----------------------------------------------------------------------------
#include "./btl_PosEffectStatus.h"


namespace btl {


  //-------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param pos        管理対象の位置
   * @param posEffect  管理対象の位置エフェクト
   */
  //-------------------------------------------------------------------------
  PosEffectStatus::PosEffectStatus( BtlPokePos pos, BtlPosEffect posEffect ) : 
    m_pos( pos ),
    m_posEffect( posEffect ),
    m_isEffective( false ),
    m_effectParam()
  {
  }

  //-------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------
  PosEffectStatus::~PosEffectStatus()
  {
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトを登録する
   * @param effectParam  位置エフェクトのパラメータ
   * @retval true   位置エフェクトが有効になった
   * @retval false  位置エフェクトが有効にならなかった
   */
  //-------------------------------------------------------------------------
  bool PosEffectStatus::SetEffect( const PosEffect::EffectParam& effectParam )
  {
    // すでに登録済み
    if( this->IsEffective() ) {
      return false;
    }

    this->SetEffectParam( effectParam );
    m_isEffective = true;
    return true;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトを解除する
   */
  //-------------------------------------------------------------------------
  void PosEffectStatus::RemoveEffect( void )
  {
    m_isEffective = false;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトが有効か？
   * @retval true  有効
   * @retval false 有効でない
   */
  //-------------------------------------------------------------------------
  bool PosEffectStatus::IsEffective( void ) const
  {
    return m_isEffective;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトのパラメータを取得する
   */
  //-------------------------------------------------------------------------
  const PosEffect::EffectParam& PosEffectStatus::GetEffectParam( void ) const
  {
    return m_effectParam;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトのパラメータを設定する
   * @param effectParam  設定するパラメータ
   */
  //-------------------------------------------------------------------------
  void PosEffectStatus::SetEffectParam( const PosEffect::EffectParam& effectParam )
  {
    m_effectParam = effectParam;
  }



} // namespace btl