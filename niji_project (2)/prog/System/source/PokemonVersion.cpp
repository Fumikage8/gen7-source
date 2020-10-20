//=============================================================================
/**
 * @file	 pm_version.cpp
 * @brief	 ポケモンバージョン
 * @author k.ohno
 * @date	 2012.5.10
 */
//=============================================================================
#include <System/include/PokemonVersion.h>

GFL_NAMESPACE_BEGIN(System)

#if PM_DEBUG
static u8 CasetteVersion = PM_VERSION;
void SetVersion( u8 version )
{
  CasetteVersion = version;
}
#endif
u8 GetVersion(void)
{
#if PM_DEBUG
  return CasetteVersion;
#else
  return PM_VERSION;
#endif
}


/**
 * @brief 指定のROMバージョンがアッパーバージョンか判定
 *
 * @param version     ROMバージョン
 *
 * @return アッパーバージョンROMならtrueを返却
 */
/* @note niji未対応 ( 2016/01/22の時点で使用してないのでコメントアウト )
bool IsUpperVersion( u8 version )
{
  return ((version>VERSION_SUN)&&(version>VERSION_MOON));
}
*/




GFL_NAMESPACE_END(System)
