//============================================================================================
/**
 * @file   JoinFestaBlackPlayerData.cpp
 * @brief  JoinFestaブラックリストのプレイヤーデータ用クラス
 * @author ichiraku_katsuhiko
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaBlackPlayerData.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"


namespace NetAppLib {
namespace JoinFestaUI {

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コアデータのコンストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaBlackPlayerData::CoreData::CoreData()
{
  gfl2::std::MemClear( this, sizeof(CoreData) );
  //
  myPrincipalId       = NN_FRIENDS_INVALID_PRINCIPAL_ID;
  isUse               = false;

  STATIC_ASSERT( (sizeof(CoreData) == 88) );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コアデータのコンストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaBlackPlayerData::CoreData::CoreData( const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData )
{
  gfl2::std::MemClear( this, sizeof(CoreData) );

  personalData.GetName( name ); // 名前
  myPrincipalId  = personalData.GetJoinFestaFriendKey().principalId;
  transferdId    = personalData.GetJoinFestaFriendKey().localFriendCode;
#if defined(GF_PLATFORM_CTR)
  addressId      = personalData.GetSimpleAddressId().id;
#endif // GF_PLATFORM_CTR
  icon           = *personalData.GetDressUpIcon();
  sex            = personalData.GetSex();
  regionCode     = personalData.GetRegionCode();
  romCode        = personalData.GetRomVersion();
  languageId     = personalData.GetLanguageId();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コンストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaBlackPlayerData::JoinFestaBlackPlayerData() : m_core()
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    デストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaBlackPlayerData::~JoinFestaBlackPlayerData()
{
}


}    // JoinFestaUI
}    // NetAppLib

