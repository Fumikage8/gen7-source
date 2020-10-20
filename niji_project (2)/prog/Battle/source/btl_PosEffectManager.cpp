//-----------------------------------------------------------------------------
/**
 * @file   btl_PosEffectManager.cpp
 * @brief  全ての位置の位置エフェクトの管理
 * @author obata_toshihiro
 * @date   2015.06.22
 */
//-----------------------------------------------------------------------------
#include "./btl_PosEffectStatus.h"
#include "./btl_PosEffectManager.h"


namespace btl {


  //-------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap  使用するヒープ
   */
  //-------------------------------------------------------------------------
  PosEffectManager::PosEffectManager( HeapHandle heap )
  {
    this->CreateEffectStatusArray( heap );
  }

  /**
   * @brief EffectStatus 配列を生成する
   * @param heap  使用するヒープ
   */
  void PosEffectManager::CreateEffectStatusArray( HeapHandle heap )
  {
    for( u32 pos=0; pos<BTL_EXIST_POS_MAX; ++pos )
    {
      for( u32 effect=0; effect<BTL_POSEFF_MAX; ++effect )
      {
        BtlPokePos   pokePos   = static_cast<BtlPokePos>( pos );
        BtlPosEffect posEffect = static_cast<BtlPosEffect>( effect );
        m_posEffects[ pos ][ effect ] = GFL_NEW( heap ) PosEffectStatus( pokePos, posEffect );
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------
  PosEffectManager::~PosEffectManager()
  {
    this->DeleteEffectStatusArray();
  }

  /**
   * @brief EffectStatus 配列を破棄する
   */
  void PosEffectManager::DeleteEffectStatusArray( void )
  {
    for( u32 pos=0; pos<BTL_EXIST_POS_MAX; ++pos )
    {
      for( u32 effect=0; effect<BTL_POSEFF_MAX; ++effect )
      {
        GFL_SAFE_DELETE( m_posEffects[ pos ][ effect ] );
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 全ての位置エフェクトを解除する
   */
  //-------------------------------------------------------------------------
  void PosEffectManager::RemoveAllPosEffect( void )
  {
    for( u32 pos=0; pos<BTL_EXIST_POS_MAX; ++pos )
    {
      for( u32 effect=0; effect<BTL_POSEFF_MAX; ++effect )
      {
        PosEffectStatus* status = this->GetPosEffectStatus( static_cast<BtlPokePos>( pos ), static_cast<BtlPosEffect>( effect ) );
        status->RemoveEffect();
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトの状態を取得する
   * @param pos     取得したい陣営
   * @param effect  取得したい位置エフェクト
   * @retval NULL  指定した位置エフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  PosEffectStatus* PosEffectManager::GetPosEffectStatus( BtlPokePos pos, BtlPosEffect effect )
  {
    return this->GetPosEffectStatusCommon( pos, effect );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトの状態を取得する
   * @param pos     取得したい陣営
   * @param effect  取得したい位置エフェクト
   * @retval NULL  指定した位置エフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  const PosEffectStatus* PosEffectManager::GetPosEffectStatusConst( BtlPokePos pos, BtlPosEffect effect ) const
  {
    return this->GetPosEffectStatusCommon( pos, effect );
  }

  /**
   * @brief 位置エフェクトの状態を取得する
   * @param pos     取得したい陣営
   * @param effect  取得したい位置エフェクト
   * @retval NULL  指定した位置エフェクトが存在しない場合
   */
  PosEffectStatus* PosEffectManager::GetPosEffectStatusCommon( BtlPokePos pos, BtlPosEffect effect ) const
  {
    if( ( BTL_EXIST_POS_MAX <= pos ) ||
        ( BTL_POSEFF_MAX <= effect ) ) {
      GFL_ASSERT(0);
      return NULL;
    }
    return m_posEffects[ pos ][ effect ];
  }



} // namespace btl