//-----------------------------------------------------------------------------
/**
 * @file   btl_SideEffectManager.cpp
 * @brief  全陣営のサイドエフェクトの管理
 * @author obata_toshihiro
 * @date   2015.06.16
 */
//-----------------------------------------------------------------------------
#include "./btl_SideEffectStatus.h"
#include "./btl_SideEffectManager.h"


namespace btl {


  //-------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap  使用するヒープ
   */
  //-------------------------------------------------------------------------
  SideEffectManager::SideEffectManager( HeapHandle heap )
  {
    this->CreateEffectStatusArray( heap );
  }

  /**
   * @brief EffectStatus 配列を生成する
   * @param heap  使用するヒープ
   */
  void SideEffectManager::CreateEffectStatusArray( HeapHandle heap )
  {
    for( u32 side=0; side<BTL_SIDE_NUM; ++side )
    {
      for( u32 effect=0; effect<BTL_SIDEEFF_MAX; ++effect )
      {
        BtlSideEffect sideEffect = static_cast<BtlSideEffect>( effect );
        m_sideEffects[ side ][ effect ] = GFL_NEW( heap ) SideEffectStatus( sideEffect );
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------
  SideEffectManager::~SideEffectManager()
  {
    this->DeleteEffectStatusArray();
  }

  /**
   * @brief EffectStatus 配列を破棄する
   */
  void SideEffectManager::DeleteEffectStatusArray( void )
  {
    for( u32 side=0; side<BTL_SIDE_NUM; ++side )
    {
      for( u32 effect=0; effect<BTL_SIDEEFF_MAX; ++effect )
      {
        GFL_SAFE_DELETE( m_sideEffects[ side ][ effect ] );
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 全てのサイドエフェクトを解除する
   */
  //-------------------------------------------------------------------------
  void SideEffectManager::RemoveAllSideEffect( void )
  {
    for( u32 side=0; side<BTL_SIDE_NUM; ++side )
    {
      for( u32 effect=0; effect<BTL_SIDEEFF_MAX; ++effect )
      {
        SideEffectStatus* status = this->GetSideEffectStatus( static_cast<BtlSide>( side ), static_cast<BtlSideEffect>( effect ) );
        status->RemoveEffect();
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトの状態を取得する
   * @param side    取得したい陣営
   * @param effect  取得したいサイドエフェクト
   * @retval NULL  指定したサイドエフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  SideEffectStatus* SideEffectManager::GetSideEffectStatus( BtlSide side, BtlSideEffect effect )
  {
    return this->GetSideEffectStatusCommon( side, effect );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトの状態を取得する
   * @param side    取得したい陣営
   * @param effect  取得したいサイドエフェクト
   * @retval NULL  指定したサイドエフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  const SideEffectStatus* SideEffectManager::GetSideEffectStatusConst( BtlSide side, BtlSideEffect effect ) const
  {
    return this->GetSideEffectStatusCommon( side, effect );
  }

  /**
   * @brief サイドエフェクトの状態を取得する
   * @param side    取得したい陣営
   * @param effect  取得したいサイドエフェクト
   * @retval NULL  指定したサイドエフェクトが存在しない場合
   */
  SideEffectStatus* SideEffectManager::GetSideEffectStatusCommon( BtlSide side, BtlSideEffect effect ) const
  {
    if( !PokemonPosition::IsUnitSide( side ) ||
        ( BTL_SIDEEFF_MAX <= effect ) ) {
      GFL_ASSERT(0);
      return NULL;
    }
    return m_sideEffects[ side ][ effect ];
  }



} // namespace btl