//=============================================================================
/**
 * @file   btl_UltraBeast.cpp
 * @date   2016/01/07 15:02:55
 * @author obata_toshihiro
 * @brief  ウルトラビースト
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_UltraBeast.h"
#include "./btl_mainmodule.h"
#include "./btl_pokeparam.h"
#include "./btl_str.h"

GFL_NAMESPACE_BEGIN( btl )



//----------------------------------------------------------------------------
/**
 * @brief ウルトラビーストかどうかをチェックする
 * @param monsno  チェック対象のモンスター
 * @retval true   指定したモンスターはウルトラビーストである
 * @retval false  指定したモンスターはウルトラビーストでない
 */
 //----------------------------------------------------------------------------
bool UltraBeast::IsUltraBeast( MonsNo monsno )
{
  static const MonsNo ULTRA_BEAST_LIST[] = 
  {
    MONSNO_BIISUTOr,
    MONSNO_BIISUTOy,
    MONSNO_BIISUTOb,
    MONSNO_BIISUTOg,
    MONSNO_BIISUTOp,
    MONSNO_BIISUTOi,
    MONSNO_BIISUTOo,
    // momiji追加分
    MONSNO_BIISUTOd1,     // 毒ビースト1
    MONSNO_BIISUTOd2,     // 毒ビースト2
    MONSNO_BIISUTOs,      // sun用ビースト：イシガキ
    MONSNO_BIISUTOh,      // moon用ビースト：ハナビ
  };

  for( u32 i=0; i<GFL_NELEMS(ULTRA_BEAST_LIST); ++i )
  {
    if( ULTRA_BEAST_LIST[i] == monsno )
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 * @brief ウルトラビーストかどうかをチェックする
 * @param poke  チェック対象のポケモン
 * @retval true   指定したモンスターはウルトラビーストである
 * @retval false  指定したモンスターはウルトラビーストでない
 */
//----------------------------------------------------------------------------
bool UltraBeast::IsUltraBeast( const BTL_POKEPARAM& poke )
{
  MonsNo monsno = static_cast<MonsNo>( poke.GetMonsNo() );
  return UltraBeast::IsUltraBeast( monsno );
}

//----------------------------------------------------------------------------
/**
 * @brief 名前を「？？？？？」と表示すべきポケモンかどうかをチェックする
 * @param monsno  チェック対象のモンスター
 * @retval true   名前を「？？？？？」と表示すべき
 * @retval false  名前を「？？？？？」と表示すべきでない
 */
//----------------------------------------------------------------------------
bool UltraBeast::IsUnknownNamePokemon( const MainModule& mainModule, MonsNo monsno )
{
  // momiji限定対応？
  // ウルトラビーストであっても？？？？？としない対応
  static const MonsNo NO_USE_UNKNOWN_BEAST_LIST[] =
  {
    MONSNO_BIISUTOi,      // カミツルギ
    MONSNO_BIISUTOd1,     // 毒ビースト1
    MONSNO_BIISUTOd2,     // 毒ビースト2
    MONSNO_BIISUTOs,      // sun用ビースト：イシガキ
    MONSNO_BIISUTOh,      // moon用ビースト：ハナビ
  };

  if (mainModule.GetSetupStatusFlag(BTL_STATUS_FLAG_VS_ULTRA_BEAST))
  {
    for (u32 i = 0; i < GFL_NELEMS(NO_USE_UNKNOWN_BEAST_LIST); ++i)
    {
      if (NO_USE_UNKNOWN_BEAST_LIST[i] == monsno)
      {
        return false;
      }
    }
  }

  if( mainModule.GetSetupStatusFlag( BTL_STATUS_FLAG_VS_ULTRA_BEAST ) &&
     !mainModule.IsZukanRegistered( monsno ) &&
      UltraBeast::IsUltraBeast( monsno ) )
  {
    return true;
  }

  return false;
}



GFL_NAMESPACE_END( btl )