//============================================================================================
/**
 * @file    JoinFestaPersonalData.cpp

 * @brief    JoinFestaパーソナルデータ
 *          プレイヤーデータをWireless/NEX両方で設定取得できる
 *
 * namespace  NetAppLib::JoinFesta::
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"

// ライブラリ
#if defined(GF_PLATFORM_CTR)
#include <nn.h>
#include <nn/cfg.h>
#endif // GF_PLATFORM_CTR
#include <gflnet2/include/base/gflnet2_NetConfig.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <util/include/gfl2_std_string.h>
#include <system/include/Date/gfl2_Date.h>
#include <model/include/gfl2_DressUpParam.h>
#include <util/include/gfl2_FixedSizeContainer.h>

// システム
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/GflUse.h"
#include "GameSys/include/NijiTime.h"

// セーブ
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/PokeDiarySave.h"
#include "Savedata/include/FashionSave.h"
#include "Savedata/include/BattleFesSave.h"
#include "Savedata/include/MiscSave.h"  // ボール投げ定義用
#include "Savedata/include/FinderStudioSave.h" // ファインダースタジオ
#include "Savedata/include/ZukanSave.h"

// バトルフェス
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"

// ファインダースタジオ
#include "App/FinderStudioStatic/include/FinderStudioUtil.h"

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
// ジョインフェスタ施設ユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"
// ジョインフェスタパーソナルデータユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"
// Excelのデータ
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"
// validation
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

// msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>

#include <NetStatic/NetLib/include/Wifi/SubscriptionManager.h>
#include "System/include/GflUse.h"

#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_bg_def.h>

/*
  debug時のみ
*/
#if PM_DEBUG
  #include  <App/FInderStudioStatic/include/FinderStudioDefine.h>
#endif





namespace NetAppLib {
namespace JoinFesta {

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コンストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData::JoinFestaPersonalData( const JoinFestaPacketData& packet )
{
  Reset();
  SetPacketData( packet );
}

JoinFestaPersonalData::JoinFestaPersonalData( const CoreData& core )
{
  Reset();
  Copy( core );
}

JoinFestaPersonalData::JoinFestaPersonalData( void )
{
  STATIC_ASSERT( (sizeof(CoreData) == 512) );
  STATIC_ASSERT( (sizeof(SubData) == 64) );
  Reset();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    デストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData::~JoinFestaPersonalData(void)
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パーソナルデータのコピー
 *
 * @param  src コピー元パーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::Copy( const JoinFestaPersonalData& src )
{
  gfl2::std::MemCopy( &src.m_sub, &this->m_sub, sizeof(JoinFestaPersonalData::SubData) );
  Copy( src.m_core );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パーソナルコアデータのコピー
 *
 * @param  src コピー元パーソナルコアデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::Copy( const JoinFestaPersonalData::CoreData& src )
{
  gfl2::std::MemCopy( &src, &this->m_core, sizeof(JoinFestaPersonalData::CoreData) );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パーソナルコアデータを取得
 *
 * @param  dst パーソナルコアデータ格納先アドレス
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::GetCoreData( JoinFestaPersonalData::CoreData* dst ) const
{
  if( dst != NULL ){
    gfl2::std::MemCopy( &this->m_core, dst, sizeof(JoinFestaPersonalData::CoreData) );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  シリアライズ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SerializeCoreData( void* dst ) const
{
  gfl2::std::MemCopy( reinterpret_cast<const void*>(&m_core), dst, sizeof(JoinFestaPersonalData::CoreData) );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  デシリアライズ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::DeserializeCoreData( const void* data )
{
  gfl2::std::MemCopy( data, reinterpret_cast<void*>(&m_core), sizeof(JoinFestaPersonalData::CoreData) );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パケットデータを更新する(for Wireless)
 *
 * @param  packet 更新元パケットデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetPacketData( const JoinFestaPacketData& packet )
{
  bool isAttractionUpdate = false;
  if( m_core.m_header.requestCommand == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION &&
      packet.header.requestCommand == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION )
  {//アトラクション情報の更新である
    isAttractionUpdate = true;
  }

  // ヘッダーコピー
  m_core.m_header = packet.header;
  // 個人情報コピー
  m_core.m_personalInfo = packet.personalInfo;

  // リクエストコマンド
  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command = GetRequestCommand();

  // ゲーム情報内包パケットの場合はセーブに保存する必要があるので、
  // 個別にコピー
  if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsBasicCommand( command ) ){
    switch( GetUnionPacketType() ){
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1:      //!<  7:拡張情報1
      m_core.m_gameStatus1 = packet.unionInfo.status1;
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_2:      //!<  8:拡張情報2
      m_core.m_gameStatus2 = packet.unionInfo.status2;
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_3:      //!<  9:拡張情報3
      m_core.m_gameStatus3 = packet.unionInfo.status3;
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_4:      //!<  10:拡張情報4
      m_core.m_gameStatus4 = packet.unionInfo.status4;
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_5:      //!<  11:拡張情報5
      m_core.m_gameStatus5 = packet.unionInfo.status5;
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_6:      //!<  12:拡張情報6
      {
        // 違うデータの場合は、Newフラグを立てる。（拡張情報7とペアかどうかはcrc一致で行うので無視）
        if( gfl2::std::MemComp( &m_core.m_gameStatus6, &packet.unionInfo.status6, sizeof( UnionPacket ) ) != 0 )
        {
          m_core.m_newPhoto = true;
        }
        m_core.m_gameStatus6 = packet.unionInfo.status6;
      }
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_7:      //!<  13:拡張情報7
      {
        // 違うデータの場合は、Newフラグを立てる。（拡張情報6とペアかどうかはcrc一致で行うので無視）
        if( gfl2::std::MemComp( &m_core.m_gameStatus7, &packet.unionInfo.status7, sizeof( UnionPacket ) ) != 0 )
        {
          m_core.m_newPhoto = true;
        }
        m_core.m_gameStatus7 = packet.unionInfo.status7;
      }
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_8:      //!<  14:拡張情報8
      m_core.m_gameStatus8 = packet.unionInfo.status8;
      break;
    default:
      ICHI_PRINT(">ICHI リクエストコマンドと共有情報が一致していない！\n");
      break;
    }
  }



  // それ以外は一時的なものなのでメモリ保存
  if( isAttractionUpdate == false )
  {
    m_sub.m_unionPacket = packet.unionInfo;
  }
  else
  {
#if defined(GF_PLATFORM_CTR)
    u32 oldTime = m_sub.m_unionPacket.attraction.time / FRAME_PER_SECOND;
    u32 newTime = packet.unionInfo.attraction.time / FRAME_PER_SECOND;

    m_sub.m_unionPacket = packet.unionInfo;

    if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
    {//ローカルのみ
      if( oldTime < newTime )
      {//見た目の秒数が変わらないように補正する
        m_sub.m_unionPacket.attraction.time = ( oldTime * FRAME_PER_SECOND ) + ( FRAME_PER_SECOND - 1 );
      }
    }
#endif
  }

  m_sub.m_unionPakcetType = static_cast<u8>( GetUnionPacketType() );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パケットデータのオンラインフラグを設定する
 *
 * @param  isOnline オンライン状態に設定するならtrue、オフラインならfalseを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetOnline( bool isOnline )
{
  m_sub.m_isOnline = isOnline;

  // 更新日時更新
  // オンラインになった時のみ更新する（オフラインはセーブデータ展開とかある）
  if( isOnline )
  {
    SetUpdateDate();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パケットデータのオンラインフラグを取得する
 *
 * @return isOnline オンライン状態ならtrue、オフラインならfalseを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsOnline( void ) const
{
  return m_sub.m_isOnline;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  ローカルフレンドコードを設定する
 *
 * @param[in]   localFriendCode  ローカルフレンドコード（ビーコンのみ）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetLocalFriendCode( const u64 localFriendCode )
{
  m_core.m_personalInfo.localFriendCode = localFriendCode;
  //ICHI_PRINT(">ICHI SetLocalFriendCode %llu \n", localFriendCode );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  3DS本体フレンドか判定
 *
 * @return フレンドであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsCtrFriend( void ) const
{
#if defined(GF_PLATFORM_CTR)
  gflnet2::friends::FriendManager* friendMgr = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
  const NetAppLib::JoinFesta::PacketPersonalInfo* pPersonalInfo = &m_core.m_personalInfo;

  bool isFriend;

  // memo:ワイヤレスの場合はローカルフレンド込み。WiFiはPIDのみ。
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    // PIDのみ使用
    gflnet2::friends::FriendKey friendKey( static_cast<gflnet2::friends::PrincipalId>(pPersonalInfo->myPrincipalId) );

    isFriend = friendMgr->IsExistFriendKey( friendKey, gflnet2::friends::FriendManager::ATTRIBUTE_FLAG_ESTABLISHED, true );
  }
  else
  {
    // ローカルフレンド込み使用
    gflnet2::friends::FriendKey friendKey( pPersonalInfo->myPrincipalId, pPersonalInfo->localFriendCode );

    isFriend = friendMgr->IsExistFriendKey( friendKey, gflnet2::friends::FriendManager::ATTRIBUTE_FLAG_NONE, false );
  }

  return isFriend;

#else
  return false;
#endif
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  VIP関係にあるか判定
 *
 * @return VIP関係にあるのであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsRelatonVip( void ) const
{
  return ( GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  プレイヤー自身のデータか判定
 *
 * @return プレイヤー自身のデータであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsRelatonMe( void ) const
{
  return ( GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_MY );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  下位バージョンからの対戦を拒否するか判定
 *
 * @return 下位バージョンからの対戦を拒否するならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsLowerBattleReject( void ) const
{
  return m_core.m_personalInfo.isLowerBattleReject;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パケットデータを更新する(for WIFI)
 *
 * @param  packet 更新元パケットデータ
 */
/* ----------------------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_CTR)
bool JoinFestaPersonalData::SetPacketData( nn::nex::SubscriptionData& packet )
{
  nn::nex::qVector<qByte> buffer;
  size_t                  packetSize = sizeof(JoinFestaPacketData);
  size_t                  unionTotalSize = sizeof(UnionPacket) * 6; // nijiでは拡張情報6つ。momijiでは7つ。

  // パケット+拡張情報×7をいれる

  packet.GetApplicationBuffer( &buffer );

  if( (packetSize+unionTotalSize) != buffer.size() ){
    GFL_ASSERT(0);
    return false;
  }

  // まずはパケットを入れる
  {
    JoinFestaPacketData JFPacket;
    gfl2::std::MemClear( &JFPacket, sizeof(JoinFestaPacketData) );
    if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::GetPacketDataFromSubscriptionData( buffer, &JFPacket ) ){
      return false;
    }
  }

  // 拡張情報を入れる
  {
    u8* statusPtr  = reinterpret_cast<u8*>(&m_core.m_gameStatus1);
    u32 index      = packetSize;

    for( size_t i = 0; i < unionTotalSize; i++ )
    {
      *statusPtr = buffer.at(index);
      statusPtr++;
      index++;
    }
  }

  return true;
}
#endif // GF_PLATFORM_CTR


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   簡易アドレス情報IDを取得する
 *
 * @return  簡易アドレス情報ID
 */
/* ----------------------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_CTR)
nn::cfg::SimpleAddressId JoinFestaPersonalData::GetSimpleAddressId( void ) const
{
  nn::cfg::SimpleAddressId addressId;

  addressId.id = m_core.m_personalInfo.addressId;

  return addressId;
}
#endif // GF_PLATFORM_CTR


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief ROMバージョンを取得する
 *
 * @return ROMバージョン
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetRomVersion( void ) const
{
  return m_core.m_personalInfo.romCode;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 着せ替えパラメータのROMバージョンNIJIフラグを取得
 *         MMCat[337]対処用に作成
 *
 * @return trueでniji
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsDressUpParamRomVersionNiji( void ) const
{
  return m_core.m_personalInfo.icon.dressUpParam.isRomVersionNiji;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief リージョンコードを取得する
 *
 * @return リージョンコード
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetRegionCode( void ) const
{
  return m_core.m_personalInfo.regionCode;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 言語コードを取得する
 *
 * @return 言語コード
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetLanguageId( void ) const
{
  return m_core.m_personalInfo.languageId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 性別を取得する
 *
 * @return PM_FEMALE, PM_MALE
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetSex( void ) const
{
  return m_core.m_personalInfo.sex;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 通信経路を設定する
 *
 * @param 通信経路定義
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetCommRoute( JoinFestaScript::CommRouteType commRouteType )
{
  if( commRouteType >= JoinFestaScript::COMM_ROUTE_MAX )
  {
    return;
  }

  bool isUpdate = false;
  bool isP2pPerson = false;

  // ビーコン、サブスクリプション以外の経路は条件なしで変更可能
  // ビーコン、サブスクリプションは、ビーコン、サブスクリプション同士でのみ変更可能
  switch( commRouteType ){
  case JoinFestaScript::COMM_ROUTE_BEACON:          //!< すれ違い（ビーコン）
  case JoinFestaScript::COMM_ROUTE_SUBSCRIPTION:    //!< すれ違い（サブスクリプション）
    {
      if( GetCommRoute() == JoinFestaScript::COMM_ROUTE_BEACON ||
          GetCommRoute() == JoinFestaScript::COMM_ROUTE_SUBSCRIPTION )
      {
        isUpdate = true;
      }
    }
    break;

  case JoinFestaScript::COMM_ROUTE_GTS:             //!< GTS
  case JoinFestaScript::COMM_ROUTE_MIRACLE_TRADE:   //!< ミラクル交換
  case JoinFestaScript::COMM_ROUTE_RANDOM_BATTLE:   //!< ランダムマッチ
  case JoinFestaScript::COMM_ROUTE_P2P_TRADE:       //!< 通信交換
  case JoinFestaScript::COMM_ROUTE_P2P_BATTLE:      //!< 通信対戦
  case JoinFestaScript::COMM_ROUTE_QUICK:           //!< クイック通信（GFNMCat[1628]対処）
    {
      ICHI_PRINT(">ICHI 通信経路 %d に設定\n", commRouteType);
      isUpdate = true;
      isP2pPerson = true;
    }
    break;
  }

  if( isUpdate == true )
  {
    m_core.m_commRoute = static_cast<u8>(commRouteType);
  }

  // @fix[1447] 通信経路を設定した後で、コピーをとらないと反映されていなかった
  if( isP2pPerson == true )
  {
    // ※城内に出す為に、P2P人物セット
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->SetLastP2pPersonal( *this );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 強制で通信経路を設定する
 *
 * @param 通信経路定義
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetCommRouteForce( JoinFestaScript::CommRouteType commRouteType )
{
  if( commRouteType < JoinFestaScript::COMM_ROUTE_MAX )
  {
    m_core.m_commRoute = static_cast<u8>(commRouteType);
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 通信経路を取得する
 *
 * @return 通信経路定義
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::CommRouteType JoinFestaPersonalData::GetCommRoute() const
{
  return static_cast<JoinFestaScript::CommRouteType>( m_core.m_commRoute );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 最終更新日時を現在日時で更新する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetUpdateDate()
{
  GameSys::DeviceDate date;
  gfl2::system::DateParameters  parameters = date.GetDateConst()->GetParameters();

  m_core.m_date.year      = parameters.year - 2000;
  m_core.m_date.month     = parameters.month;
  m_core.m_date.day       = parameters.day;
  m_core.m_date.hour      = parameters.hour;
  m_core.m_date.minute    = parameters.minute;
  m_core.m_date.half_sec  = parameters.second / 2;

  // @fix GFNMCat[2524]：RTC改ざん時はパケット受信するまでペナルティ中にする
  m_core.m_penaltyTime = 0;
  /*
  {
  u8 year = m_core.m_date.year;
  u8 month = m_core.m_date.month;
  u8 day = m_core.m_date.day;
  u8 hour = m_core.m_date.hour;
  u8 minute = m_core.m_date.minute;
  u8 half_sec = m_core.m_date.half_sec;
  ICHI_PRINT( "更新日時 %2d/%2d/%2d %2d:%2d:%2d\n",
  year, month, day, hour, minute, half_sec );
  }*/
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 最終更新日時を取得する
 *
 * @return 最終更新日時
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData::Date JoinFestaPersonalData::GetUpdateDate() const
{
  return m_core.m_date;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 最終更新日時からの経過時間を取得する
 *
 * @return 経過時間(hour) MAX 99
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetUpdateElapsedDate() const
{
  GameSys::DeviceDate now;
  gfl2::system::Date updateDate( (m_core.m_date.year+2000), m_core.m_date.month, m_core.m_date.day, m_core.m_date.hour, m_core.m_date.minute, m_core.m_date.half_sec );

  s64 elapsedTime = now.GetDateConst()->GetDateTimeToSecond() - updateDate.GetDateTimeToSecond(); 
  ICHI_PRINT(">ICHI 経過時間 秒(%d) ",elapsedTime );
  // 秒→時
  elapsedTime /= 3600;

  // 最大99時間
  if( elapsedTime > 99 ){ elapsedTime = 99; }

  // @fix GFNMCat[2524]：RTC改ざん時はパケット受信するまでペナルティ中としてMAX経過時間にする
  if( m_core.m_penaltyTime == 1 )
  {
    elapsedTime = 99;
  }

  ICHI_PRINT("時(%d) \n",elapsedTime );
  return static_cast<u32>(elapsedTime);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief RTC改ざん時のペナルティフラグを立てる
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetPenaltyTime()
{
  m_core.m_penaltyTime = 1;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   着せ替え情報の取得
 *
 * @return  着せ替え情報
 */
/* ----------------------------------------------------------------------------------------- */
const ::System::DressUpParamSaveData* JoinFestaPersonalData::GetDressUpParam( void ) const
{
  return &m_core.m_personalInfo.icon.dressUpParam;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   着せ替えアイコン情報の取得
 *
 * @return  着せ替えアイコン情報
 */
/* ----------------------------------------------------------------------------------------- */
const Savedata::MyStatus::ICON_DATA* JoinFestaPersonalData::GetDressUpIcon( void ) const
{
  return &m_core.m_personalInfo.icon;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief パケットデータサイズを取得する
 *
 * @return パケットデータサイズ
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetPacketDataSize( void ) const
{
  return sizeof(JoinFestaPacketData);
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  JoinFestaフレンドキーを取得する
 *
 * @return JoinFestaフレンドキー
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaFriendKey JoinFestaPersonalData::GetJoinFestaFriendKey( void ) const
{
  return m_core.m_personalInfo.GetJoinFestaFriendKey();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プレイヤーユーザーとの関係性を取得
 *
 * @return  プレイヤーとの関係
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION JoinFestaPersonalData::GetRelation( void ) const
{
  return static_cast<NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION>(m_sub.m_relation);
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プレイヤーユーザーとの関係性を設定する
 *
 * @param   relation プレイヤーとの関係
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation )
{
  m_sub.m_relation = static_cast<u8>(relation);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プレイ状況を取得
 *
 * @return  プレイ状況定義
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION JoinFestaPersonalData::GetPlaySituation( void ) const
{
  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command = GetRequestCommand();

  switch( command ){
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS_FIELD:   //!<  3:ゲーム情報更新（フィールド版）
    return JoinFestaDefine::E_PLAY_SITUATION_FIELD;                 //!< フィールドで遊び中（ジョインフェスタ外）
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SINGLE:            //!<  4:シングルバトル中
    return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SINGLE;
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DOUBLE:            //!<  5:ダブルバトル中
    return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_DOUBLE;
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_MULTI:             //!<  6:マルチバトル中
    return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_MULTI;
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_ROYAL:             //!<  7:ロイヤルバトル中
    return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_ROYAL;
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE:                    //!<  8:通信交換中
    return JoinFestaDefine::E_PLAY_SITUATION_TRADE;
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_GTS:                      //!<  4:GTSプレイ中
    return JoinFestaDefine::E_PLAY_SITUATION_GTS;                   //!< GTS中
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SPOT:              //!<  5:バトルスポットプレイ中
    return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SPOT;           //!< バトルスポット中
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_MIRACLE_TRADE:            //!<  6:ミラクルトレードプレイ中
    return JoinFestaDefine::E_PLAY_SITUATION_MIRACLE_TRADE;         //!< ミラクル交換中
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_SHOP:                  //!<  7:ジョインフェスタのお店で買い物中
    return JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_SHOP;       //!< ジョインフェスタ：お店で買い物中
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_PERSONAL:              //!<  8:ジョインフェスタの人に話しかけ中
    return JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_PERSONAL;   //!< ジョインフェスタ：人に話しかけ中
  case JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION:                  //!< 27:アトラクション情報
    return JoinFestaDefine::E_PLAY_SITUATION_ATTRACTION;            //!< アトラクション中 
  }

  // バトル
  if( (JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BATTLE_TOP < command) && (command < JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BATTLE_BOTTOM) )
  {
    Regulation::BATTLE_RULE battleRule = Regulation::BATTLE_RULE_SINGLE;
    if( IsEnablePacketBattleInvite() )
    {
      battleRule = static_cast<Regulation::BATTLE_RULE>(GetPacketBattleInvite().battle_rule);
    }
    if( IsEnablePacketBattleRecieve() )
    {
      battleRule = static_cast<Regulation::BATTLE_RULE>(GetPacketBattleRecieve().battle_rule);
    }
    switch( battleRule ){
    case Regulation::BATTLE_RULE_SINGLE:  return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SINGLE;
    case Regulation::BATTLE_RULE_DOUBLE:  return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_DOUBLE;
    case Regulation::BATTLE_RULE_ROYAL:   return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_ROYAL;
    case Regulation::BATTLE_RULE_MULTI:   return JoinFestaDefine::E_PLAY_SITUATION_BATTLE_MULTI;
    }
  }

  // 交換
  if( (JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_TRADE_TOP < command) && (command < JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_TRADE_BOTTOM) )
  {
    return JoinFestaDefine::E_PLAY_SITUATION_TRADE;                 //!< 通信交換中
  }

  return JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_NORAML;     //!< ジョインフェスタ：フィールド遊び中
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パケットデータのコピーを出力する
 *
 * @param   dst コピー先パケットデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::OutputPacketData( JoinFestaPacketData *dst ) const
{
  ::std::memset( dst , 0 , sizeof(JoinFestaPacketData) );

  dst->header = m_core.m_header;
  dst->personalInfo = m_core.m_personalInfo;
  dst->unionInfo = m_sub.m_unionPacket;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定パケットが登録済みパケットと同一か判定
 *
 * @param   packet 判定対象パケット
 *
 * @return  同一パケットであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsSamePacket( const JoinFestaPacketData& packet )
{
  bool isSameHeader = ::std::memcmp( &packet.header , &m_core.m_header , sizeof(packet.header) ) == 0;
  bool isSamePersonalInfo = ::std::memcmp( &packet.personalInfo , &m_core.m_personalInfo , sizeof(packet.personalInfo) ) == 0;
  bool isSameUnionInfo = ::std::memcmp( &packet.unionInfo , &m_sub.m_unionPacket , sizeof(packet.unionInfo) ) == 0;

  return isSameHeader && isSamePersonalInfo && isSameUnionInfo;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パケットデータを指定のサブスクリプションデータにコピーを出力する
 *          NEXの機能を使用するため、事前にNEXを初期化しておく必要がある
 *
 * @param   dst コピー先サブスクリプションデータ
 */
/* ----------------------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_CTR)
void JoinFestaPersonalData::OutputPacketData( nn::nex::SubscriptionData *dst )
{
  JoinFestaPacketData packet;
  gfl2::std::MemClear( &packet, sizeof(JoinFestaPacketData) );

  // momijiでは拡張情報の1～7を送りたいが、サイズに制限がある為、union部分も使用して送る。
  // 上書きするのは、通常コマンドのみ。対戦・交換等は上書きしない。
  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command = GetRequestCommand();
  if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsBasicCommand( command ) )
  {
    m_sub.m_unionPacket.status7 = m_core.m_gameStatus7;
    SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_7 );
  }

  // パケットをそのままコピー
  packet.header = m_core.m_header;
  packet.personalInfo = m_core.m_personalInfo;
  packet.unionInfo = m_sub.m_unionPacket;

  if( IsOpenAttraction() )
  {
    u32 id = GFL_SINGLETON_INSTANCE( NetLib::Wifi::SubscriptionManager )->GetGatheringID();

    u32* pID = (u32*)(&packet.personalInfo.localFriendCode);
    *pID = id;
  }


  u8*    packetPtr  = reinterpret_cast<u8*>(&packet);
  size_t packetSize = sizeof(JoinFestaPacketData);
  u8*    statusPtr  = reinterpret_cast<u8*>(&m_core.m_gameStatus1); //
  size_t unionTotalSize = sizeof(UnionPacket) * 6; // nijiでは拡張情報6つ momijiでは7つ。


  nn::nex::qVector<qByte> buffer;

  for( size_t i = 0; i < packetSize; i++ )
  {
    buffer.push_back( qByte(*packetPtr) );
    packetPtr++;
  }
  for( size_t i = 0; i < unionTotalSize; i++ )
  {
    buffer.push_back( qByte(*statusPtr) );
    statusPtr++;
  }

  dst->SetApplicationBuffer(buffer);
}
#endif // GF_PLATFORM_CTR


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   WIFI通信時、パーソナルデータの更新を友達以外にも通知するために
 *          MessageClient経由でメッセージ送信する対象者PIDを取得する
 *
 * @param   pidList メッセージ送信先PIDを格納するための配列
 *
 * @return  メッセージを通知するPID件数
 */
/* ----------------------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_CTR)
u32 JoinFestaPersonalData::GetSendingMessageTarget( nn::nex::PrincipalID* pidList ) const
{
  u32 num = 0;

  // 通信対戦：参加者へ
  if( IsEnablePacketBattleInvite() )
  {
    const PacketBattleInvite* battleInvite = &m_sub.m_unionPacket.battleInvite;

    for( u32 i = 0; i < battleInvite->guestUserNum; i++ )
    {
      if( battleInvite->guestFriendId[i] != INVALID_PRINCIPALID )
      {
        pidList[num] = static_cast<nn::nex::PrincipalID>(battleInvite->guestFriendId[i]);
        num++;
      }
    }
  }
  // 通信対戦：招待者へ
  else if( IsEnablePacketBattleRecieve() )
  {
    const PacketBattleRecieve* battleRecive = &m_sub.m_unionPacket.battleReceive;
    pidList[0] = battleRecive->hostPrincipalId;
    num = 1;

    JoinFestaFriendKey friendKey;
    friendKey.initialize( static_cast<gflnet2::friends::PrincipalId>( battleRecive->hostPrincipalId ) );
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalData( friendKey );

    if( personal )
    {
      const PacketBattleInvite& battleInvite = personal->GetPacketBattleInvite();
      if( battleInvite.guestUserNum != 1 )
      {
        u32 count = 0;
        for( u32 index = 0 ; index < battleInvite.guestUserNum ; ++index )
        {
          u32 pid = battleInvite.guestFriendId[index];
          if( pid != m_core.m_personalInfo.myPrincipalId )
          {
            count++;
            pidList[count] = pid;
          }
        }
        // @fix NMCat[4166]：裏でP2Pに繋がっていてP2pCommonDataクラスによるJoinFestaPersonalDataの上書きがあると、subデータが空になる。
        //                   その為、guestUserNumが0なので、アサートに引っかかる。
        //                   その後の挙動は、アサート出た端末以外でタイムアウトになる。
        if( battleInvite.guestUserNum != 0 )
        {
          GFL_ASSERT( count == 2 );
        }
        num += count;
      }
    }
  }
  // 通信交換：招待者へ
  else if( IsEnablePacketTradeInvite() )
  {
    const PacketTradeInvite* tradeInvite = &m_sub.m_unionPacket.tradeInvite;
    if( tradeInvite->guestUserNum != 0 )
    {
      pidList[0] = tradeInvite->guestPrincipalId;
      num = 1;
    }
  }
  // 通信交換：主催者へ
  else if( IsEnablePacketTradeRecieve() )
  {
    const PacketTradeRecieve* tradeRecive = &m_sub.m_unionPacket.tradeReceive;
    pidList[0] = tradeRecive->hostPrincipalId;
    num = 1;
  }
  else if( IsOpenAttraction() )
  {//アトラクション招待

#if PM_DEBUG
    u8 debugNumCount = 0; // 送った人数計
    u8 debugOkCount = 0; // ○になった人数計
#endif

    for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
    {
      JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

      GFL_ASSERT( list ); //@fix

      if( list != NULL )
      {
        if( list->Size() > 0)
        {

          for( JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
          {
            if( (*it)->GetJoinFestaFriendKey().principalId == 0 )
            {//Wifi未接続のプレイヤー
              continue;
            }

#if PM_DEBUG
            debugNumCount++;
#endif

            u16 inviteParam = GFL_SINGLETON_INSTANCE( NetLib::Wifi::SubscriptionManager )->GetInviteParam();
            u32 rand = System::GflUse::GetPublicRand( inviteParam );
            if( inviteParam - 1 != rand )
            {//抽選外れ
#if PM_DEBUG
              gfl2::str::PrintCode( (*it)->m_core.m_personalInfo.name, "×：" );
#endif
              continue;
            }
            else
            {
#if PM_DEBUG
              gfl2::str::PrintCode( (*it)->m_core.m_personalInfo.name, "○：" );
              debugOkCount++;
#endif
            }

            if( (*it) )
            {
              pidList[num] = (*it)->GetJoinFestaFriendKey().principalId;
              num++;
              if( num == 100 )
              {
                return num;
              }
            }
          }
        }
      }
    }
    GFL_RELEASE_PRINT("招待割合:[%d] ○の人数:%d/%d \n", GFL_SINGLETON_INSTANCE( NetLib::Wifi::SubscriptionManager )->GetInviteParam(), debugOkCount, debugNumCount );
  }

  return num;
}
#endif // GF_PLATFORM_CTR


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のJoinFestaフレンドキーが自身のJoinFestaフレンドキーと一致するか判定
 *
 * @param   friendKey 比較対象となるJoinFestaフレンドキー
 *
 * @return  指定フレンドキーが自身のJoinFestaフレンドキーと一致するならtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsSameFriendKey( const JoinFestaFriendKey& friendKey ) const
{
  return NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( this->GetJoinFestaFriendKey(), friendKey );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   名前を取得する
 *
 * @return  名前
 */
/* ----------------------------------------------------------------------------------------- */
const gfl2::str::STRCODE* JoinFestaPersonalData::GetName( void ) const
{
  return m_core.m_personalInfo.name;
}
void JoinFestaPersonalData::GetName( gfl2::str::STRCODE* dst ) const 
{
  gfl2::str::StrNCopy( dst, m_core.m_personalInfo.name, ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ); 
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   名前の不正チェック＆置換
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ReplaceName()
{
#if defined(GF_PLATFORM_CTR)
  NetLib::Validation::PokemonValidation::ReplacePlayerName( m_core.m_personalInfo.name, m_core.m_personalInfo.languageId, m_core.m_personalInfo.romCode ); //不正名置換
#endif
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   PrincipalIDを取得する
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetPrincipalID( void ) const
{
  return m_core.m_personalInfo.myPrincipalId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   Beacon通信用識別IDを取得する
 *
 * @return  パケットの送信元を特定するための本体固有ID
 */
/* ----------------------------------------------------------------------------------------- */
bit64 JoinFestaPersonalData::GetTransferedID( void ) const
{
  return m_core.m_personalInfo.transferdId;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   削除対象から除外するようにロックする
 *          ロックを解除しないとメモリリークの危険性があるので管理に注意
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::Lock( void )
{
  m_sub.m_isLock = true;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   削除禁止ロックの解除
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::UnLock( void )
{
  m_sub.m_isLock = false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   削除禁止ロックの取得
 *
 * @return  ロック状態にあるならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsLock( void ) const
{
  return m_sub.m_isLock;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaフレンドキーをもとに、データの有効性を判断
 *
 * @return  設定されているJoinFestaフレンドキーが有効であればtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsEnable( void ) const
{
  return (m_core.m_personalInfo.myPrincipalId != NN_FRIENDS_INVALID_PRINCIPAL_ID)||
         (m_core.m_personalInfo.transferdId   != NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE);
}


/* ----------------------------------------------------------------------------------------- */
/** 
 * @brief   登録をリセットする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::Reset( void )
{
  gfl2::std::MemClear( &m_core, sizeof(CoreData) );
  gfl2::std::MemClear( &m_sub, sizeof(SubData) );
  // 
  m_core.m_header = PacketHeader();
  m_core.m_personalInfo = PacketPersonalInfo();
  m_core.m_date = Date();
  //
  SetRelation( JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
  //
  ResetPersonalEvent();
  //
  m_sub.m_isLock            = false;
  m_sub.m_isOnline          = false;
  //
  ResetAttractionData();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 日付更新におけるリセット処理
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ResetWithDaytime( void )
{
  // momiji追加:バトルフェスパケットデータの補間
  // @fix[620]仕様変更。選べないのにポケモンが変わるのに違和感がある為、日付の更新は行わない
  //this->BattleFesPacketInterpolation();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パケットのタイムスタンプを更新する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::UpdatePacketTimestamp( void )
{
  m_core.m_header.timestamp = GetTimeStamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   現在のタイムスタンプを取得する
 *
 * @return  現在時間(秒）を取得
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetTimeStamp( void )
{
  GameSys::DeviceDate date;
  return (date.GetDateConst()->GetDateTimeToSecond() & 0xFFFFFFFF);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   バトル招待用リクエストを設定する
 *
 * @param   battleType     バトルタイプ -> app::JoinFesta::JoinFesta_rule_select::E_JOIN_FESTA_BATTLE_TYPE
 * @param   isWifi         WIFI接続ならtrue、Wireless接続ならfalseを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestInvitingBattle( Regulation::BATTLE_RULE battleRule, bool isWifi )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_INVITE );
  //
  PacketBattleInvite *inviteInfo = &m_sub.m_unionPacket.battleInvite;

  inviteInfo->battle_rule          = static_cast<u8>(battleRule);
  inviteInfo->ticketID             = GetTimeStamp();
  inviteInfo->isWIFI               = isWifi;

  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   バトル招待キャンセル用リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestCancelingBattle( void )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CANCEL;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_INVITE );
  // PacketBattleInvite情報はそのまま流用
  UpdatePacketTimestamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [親]バトル開始用リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestStartingBattle( void )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CONNECT;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_INVITE );
  // PacketBattleInvite情報はそのまま流用
  UpdatePacketTimestamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   バトル招待に対する返答を設定する
 *
 * @param   hostFriendKey  招待者のJoinFestaフレンドキー
 * @param   ticketId       招待者発行のチケットＩＤ
 * @param   isJoin         参加意思。trueなら参加、falseなら不参加
 * @param   isRequlation   レギュレーションを満たしているならtrueを指定する。
 * @param]  battleRule     親からもらったバトルルール
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestRecieveBattle( const JoinFestaFriendKey& hostFriendKey, u32 ticketId, bool isJoin, bool isRequlation, u8 battleRule )
{
  m_core.m_header.requestCommand = isJoin ? JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_JOIN : JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_RECIEVE );
  //
  PacketBattleRecieve *recieveInfo = &m_sub.m_unionPacket.battleReceive;

  recieveInfo->ticketID            = ticketId;
  recieveInfo->hostPrincipalId     = hostFriendKey.principalId;
  recieveInfo->hostLocalFriendCode = hostFriendKey.localFriendCode;
  recieveInfo->isRegulation        = isRequlation;
  recieveInfo->battle_rule         = battleRule;
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [親]トレード招待用リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestInvitingTrade( void )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_INVITE );
  //
  PacketTradeInvite *inviteInfo = &m_sub.m_unionPacket.tradeInvite;

  inviteInfo->ticketID    = GetTimeStamp();
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   トレード招待キャンセル用リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestCancelingTrade( void )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CANCEL;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_INVITE );
  // PacketTradeInvite情報はそのまま流用
  UpdatePacketTimestamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [親]トレード開始用リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestStartingTrade( void )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CONNECT;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_INVITE );
  // PacketTradeInvite情報はそのまま流用
  UpdatePacketTimestamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   トレード招待に対する返答を設定する
 *
 * @param   hostFriendKey  招待者のJoinFestaフレンドキー
 * @param   ticketId       招待者発行のチケットＩＤ
 * @param   isJoin         参加意思。trueなら参加、falseなら不参加
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestRecieveTrade( const JoinFestaFriendKey& hostFriendKey, u32 ticketId, bool isJoin )
{
  m_core.m_header.requestCommand = isJoin ? JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_JOIN : JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_RECIEVE );
  //
  PacketTradeRecieve *recieveInfo = &m_sub.m_unionPacket.tradeReceive;

  recieveInfo->ticketID            = ticketId;
  recieveInfo->hostPrincipalId     = hostFriendKey.principalId;
  recieveInfo->hostLocalFriendCode = hostFriendKey.localFriendCode;
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]P2P通信対戦プレイ中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestPlayingP2pBattle( Regulation::BATTLE_RULE battleRule )
{
  switch( battleRule ){
  case Regulation::BATTLE_RULE_SINGLE:
    SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SINGLE );
    break;
  case Regulation::BATTLE_RULE_DOUBLE:
    SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DOUBLE );
    break;
  case Regulation::BATTLE_RULE_ROYAL:
    SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_ROYAL );
    break;
  case Regulation::BATTLE_RULE_MULTI:
    SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_MULTI );
    break;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]P2P通信交換プレイ中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestPlayingP2pTrade( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ＧＴＳプレイ中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestPlayingGTS( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_GTS );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   バトルマッチスポット中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestPlayingBattleSpot( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_SPOT );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ミラクル交換プレイ中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestPlayingMiracleTrade( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_MIRACLE_TRADE );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ゲーム情報更新リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestUpadateGameStatus( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ゲーム情報更新リクエスト(ジョインフェスタ外)を設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestUpadateGameStatusField( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS_FIELD );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JFのお店買い物中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestJoinFestaShop( void ) 
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_SHOP );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JFの人に話しかけ中リクエストを設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestJoinFestaPersonal( void )
{
  SetBasicPacketCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_JF_PERSONAL );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション情報を設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestAttraction( PacketAttraction& attractionInfo )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION;
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_ATTRACTION );
  //
  m_sub.m_unionPacket.attraction = attractionInfo;
  //
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パケットのアッパー対応処理
 * @memo    パケット受信後のパーソナル更新後、P2Pでパーソナルデータデシリアライズ後に呼び出す
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::UpperPacketCope()
{
  // 範囲外の定型文
  {
    // 定型文の最大値
    const u32 patternMax = (GetSex()==PM_MALE) ? msg_jf_phrase_max/2 : msg_jf_phrase_max;

    // 定型あいさつ
    if( m_core.m_personalInfo.patternHello >= patternMax )
    {
      if( GetTransferedID()%2 == 0 )
      { // おはよう
        m_core.m_personalInfo.patternHello = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_316 : msg_jf_phrase_f_316;
      }
      else
      { // こんばんは
        m_core.m_personalInfo.patternHello = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_317 : msg_jf_phrase_f_317;
      }
    }

    // 定型わかれ
    if( m_core.m_personalInfo.patternParting >= patternMax )
    {
      if( GetTransferedID()%2 == 0 )
      { // バイバイ
        m_core.m_personalInfo.patternParting = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_470 : msg_jf_phrase_f_470;
      }
      else
      { // あばよ
        m_core.m_personalInfo.patternParting = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_469 : msg_jf_phrase_f_469;
      }
    }

    // 定型かんどう
    if( m_core.m_personalInfo.patternEmotion >= patternMax )
    {
      if( GetTransferedID()%2 == 0 )
      { // アンビリーバブル
        m_core.m_personalInfo.patternEmotion = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_267 : msg_jf_phrase_f_267;
      }
      else
      { // さいこうです
        m_core.m_personalInfo.patternEmotion = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_268 : msg_jf_phrase_f_268;
      }
    }

    // 定型ざんねん
    if( m_core.m_personalInfo.patternRegret >= patternMax )
    {
      if( GetTransferedID()%2 == 0 )
      { // なんだってー
        m_core.m_personalInfo.patternRegret = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_304 : msg_jf_phrase_f_304;
      }
      else
      { // え～
        m_core.m_personalInfo.patternRegret = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_504 : msg_jf_phrase_f_504;
      }
    }

    // 定型ひみつ
    if( m_core.m_personalInfo.patternSecret >= patternMax )
    {
      m_core.m_personalInfo.patternSecret = ( GetSex() == PM_MALE ) ? msg_jf_phrase_m_518 : msg_jf_phrase_f_518 ;
    }
    
  }

  // 範囲外の一押し施設
  {
    // その日のその人の乱数
    gfl2::math::Random random;
    u32 seed = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetRandomGroup()->GetTodayRandSeed();

    // TIDの下1桁を引く
    seed -= (GetTransferedID()%10);
    random.Initialize( seed );

    // 施設ID
    if( !NetApp::JoinFesta::ShopItemInfoTable::IsValidIndex( m_core.m_personalInfo.recommendFacilitiesId ) )
    {
      ICHI_PRINT(">UPPER 施設ID範囲外 %d \n", m_core.m_personalInfo.recommendFacilitiesId);

      const u8 facilitiesIdTbl[7] = {
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_LOTTERY_00,
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_HAPPENING_00,
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_GIFT_A_00,
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_KITCHEN_A_00,
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_AIR_A_00,
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_FORTUNETELLING_RGB_00,
        NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_RED_00,
      };

      m_core.m_personalInfo.recommendFacilitiesId = facilitiesIdTbl[ random.Next(7) ];

      // 施設に合わせた色にする
      // ※赤の染め物屋は、赤色だが数値は同じ。
      m_core.m_personalInfo.recommendFacilitiesColor = JoinFestaScript::FACILITIES_COLOR_1;
    }

    // 施設カラー
    {
      JoinFestaScript::FacilitiesType facilitiesType = NetApp::JoinFesta::ShopItemInfoTable::GetFacilitiesType( m_core.m_personalInfo.recommendFacilitiesId );

      if( facilitiesType == JoinFestaScript::FACILITIES_TYPE_DYEING )
      {
        // 染物屋は施設に合わせた色に決め打ち
        m_core.m_personalInfo.recommendFacilitiesColor = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::ConvertDyeingColor( m_core.m_personalInfo.recommendFacilitiesId );
      }
      else
      {
        if( m_core.m_personalInfo.recommendFacilitiesColor > JoinFestaScript::FACILITIES_COLOR_4 )
        {
          ICHI_PRINT(">UPPER 施設カラー範囲外 %d \n", m_core.m_personalInfo.recommendFacilitiesColor);
          m_core.m_personalInfo.recommendFacilitiesColor = JoinFestaScript::FACILITIES_COLOR_1;
        }
      }
    }

    // 施設店員
    {
      if( m_core.m_personalInfo.recommendFacilitiesCharacterId >= NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX )
      {
        ICHI_PRINT(">UPPER 施設店員範囲外 %d \n", m_core.m_personalInfo.recommendFacilitiesCharacterId);
        m_core.m_personalInfo.recommendFacilitiesCharacterId = random.Next( NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX );
      }
    }
  }

  // 範囲外のレコード
  {
    Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetRecordSaveData();

    if( m_core.m_gameStatus2.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      // 拡張情報2
      if( m_core.m_gameStatus2.record1 > pRecord->GetMax( Savedata::Record::RECID_COMM_BATTLE ) ){m_core.m_gameStatus2.record1=0;}
      if( m_core.m_gameStatus2.record2 > pRecord->GetMax( Savedata::Record::RECID_WIFI_BATTLE ) ){m_core.m_gameStatus2.record2=0;}
      if( m_core.m_gameStatus2.record3 > pRecord->GetMax( Savedata::Record::RECID_BTREE_CHALLENGE ) ){m_core.m_gameStatus2.record3=0;}
      if( m_core.m_gameStatus2.record4 > pRecord->GetMax( Savedata::Record::RECID_BTL_ENCOUNT ) ){m_core.m_gameStatus2.record4=0;}
      if( m_core.m_gameStatus2.record5 > pRecord->GetMax( Savedata::Record::RECID_WEAK_ATTACK ) ){m_core.m_gameStatus2.record5=0;}
      if( m_core.m_gameStatus2.record6 > pRecord->GetMax( Savedata::Record::RECID_COMM_TRADE ) ){m_core.m_gameStatus2.record6=0;}
      if( m_core.m_gameStatus2.record7 > pRecord->GetMax( Savedata::Record::RECID_MIRACLE_TRADE_COUNT ) ){m_core.m_gameStatus2.record7=0;}
      if( m_core.m_gameStatus2.record8 > pRecord->GetMax( Savedata::Record::RECID_GTS_TRADE_COUNT ) ){m_core.m_gameStatus2.record8=0;}
      if( m_core.m_gameStatus2.record9 > pRecord->GetMax( Savedata::Record::RECID_GTS_PUT ) ){m_core.m_gameStatus2.record9=0;}
      if( m_core.m_gameStatus2.record10 > pRecord->GetMax( Savedata::Record::RECID_DAYMAX_EXP ) ){m_core.m_gameStatus2.record10=0;}
      if( m_core.m_gameStatus2.record11 > pRecord->GetMax( Savedata::Record::RECID_MAME_GET_CNT ) ){m_core.m_gameStatus2.record11=0;}
    }

    if( m_core.m_gameStatus3.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      // 拡張情報3
      if( m_core.m_gameStatus3.record1 > pRecord->GetMax( Savedata::Record::RECID_BERRY_HARVEST ) ){m_core.m_gameStatus3.record1=0;}
      if( m_core.m_gameStatus3.record2 > pRecord->GetMax( Savedata::Record::RECID_SHOPPING_MONEY ) ){m_core.m_gameStatus3.record2=0;}
      if( m_core.m_gameStatus3.record3 > pRecord->GetMax( Savedata::Record::RECID_USE_BP ) ){m_core.m_gameStatus3.record3=0;}
      if( m_core.m_gameStatus3.record4 > pRecord->GetMax( Savedata::Record::RECID_CAPTURE_POKE ) ){m_core.m_gameStatus3.record4=0;}
      if( m_core.m_gameStatus3.record5 > pRecord->GetMax( Savedata::Record::RECID_TAMAGO_HATCHING ) ){m_core.m_gameStatus3.record5=0;}
      if( m_core.m_gameStatus3.record6 > pRecord->GetMax( Savedata::Record::RECID_MAME_PRESENT_CNT) ){m_core.m_gameStatus3.record6=0;}
      if( m_core.m_gameStatus3.record7 > pRecord->GetMax( Savedata::Record::RECID_NICKNAME ) ){m_core.m_gameStatus3.record7=0;}
      if( m_core.m_gameStatus3.record8 > pRecord->GetMax( Savedata::Record::RECID_RIDE_POKE ) ){m_core.m_gameStatus3.record8=0;}
      if( m_core.m_gameStatus3.record9 > pRecord->GetMax( Savedata::Record::RECID_JF_USE_JCOIN ) ){m_core.m_gameStatus3.record9=0;}
      if( m_core.m_gameStatus3.record10 > pRecord->GetMax( Savedata::Record::RECID_ZWAZA_CNT ) ){m_core.m_gameStatus3.record10=0;}
      if( m_core.m_gameStatus3.record11 > pRecord->GetMax( Savedata::Record::RECID_JF_GEST_TALK_CNT ) ){m_core.m_gameStatus3.record11=0;}
    }

    if( m_core.m_gameStatus4.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      // 拡張情報4
      if( m_core.m_gameStatus4.record1 > pRecord->GetMax( Savedata::Record::RECID_DENDOU_CNT ) ){m_core.m_gameStatus4.record1=0;}
      if( m_core.m_gameStatus4.record2 > pRecord->GetMax( Savedata::Record::RECID_KAWAIGARI_CNT ) ){m_core.m_gameStatus4.record2=0;}
      if( m_core.m_gameStatus4.record3 > pRecord->GetMax( Savedata::Record::RECID_DAYMAX_CAPTURE ) ){m_core.m_gameStatus4.record3=0;}
      if( m_core.m_gameStatus4.record4 > pRecord->GetMax( Savedata::Record::RECID_DAYMAX_EVOLUTION ) ){m_core.m_gameStatus4.record4=0;}
      if( m_core.m_gameStatus4.record5 > pRecord->GetMax( Savedata::Record::RECID_DAYMAX_KILL ) ){m_core.m_gameStatus4.record5=0;}
      if( m_core.m_gameStatus4.record6 > pRecord->GetMax( Savedata::Record::RECID_DAYMAX_TRAINER_BATTLE ) ){m_core.m_gameStatus4.record6=0;}
      if( m_core.m_gameStatus4.record7 > pRecord->GetMax( Savedata::Record::RECID_JF_ATT_CNT ) ){m_core.m_gameStatus4.record7=0;}
      if( m_core.m_gameStatus4.record8 > pRecord->GetMax( Savedata::Record::RECID_JF_SHOP_GET_CNT ) ){m_core.m_gameStatus4.record8=0;}
      if( m_core.m_gameStatus4.record9 > pRecord->GetMax( Savedata::Record::RECID_RARE_COLOUR_ENC_COUNT ) ){m_core.m_gameStatus4.record9=0;}
      if( m_core.m_gameStatus4.record10 > pRecord->GetMax( Savedata::Record::RECID_IDKUJI_ATARI_CNT ) ){m_core.m_gameStatus4.record10=0;}
      if( m_core.m_gameStatus4.record11 > pRecord->GetMax( Savedata::Record::RECID_POINTUP_USE ) ){m_core.m_gameStatus4.record11=0;}
      if( m_core.m_gameStatus4.record12 > pRecord->GetMax( Savedata::Record::RECID_KISEKAE_CNT ) ){m_core.m_gameStatus4.record12=0;}
      if( m_core.m_gameStatus4.record13 > pRecord->GetMax( Savedata::Record::RECID_QR_SCAN ) ){m_core.m_gameStatus4.record13=0;}
      if( m_core.m_gameStatus4.record14 > pRecord->GetMax( Savedata::Record::RECID_CAMERA_CNT ) ){m_core.m_gameStatus4.record14=0;}
      if( m_core.m_gameStatus4.record15 > pRecord->GetMax( Savedata::Record::RECID_LUNASOL_BESTSCORE ) ){ m_core.m_gameStatus4.record15 = 0; }
      if( m_core.m_gameStatus4.record16 > pRecord->GetMax( Savedata::Record::RECID_MANTAIN_SURF_BEST_BP ) ){ m_core.m_gameStatus4.record16 = 0; }
    }
  }

  // 範囲外のフェスランク
  {
    if( m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      if( m_core.m_gameStatus1.jfRank > NetApp::JoinFesta::JoinFestaDefine::RANK_MAX ||
          m_core.m_gameStatus1.jfRank == 0 )
      {
        ICHI_PRINT(">UPPER 範囲外フェスランク\n");
        m_core.m_gameStatus1.jfRank = 2;
      }
    }
  }

  // 範囲外の着せ替えアイテム総数
  {
    if( m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      const u32 haveDressUpItemNumMax = ( GetSex() == PM_MALE ) ? 679 : 1229;
      if( m_core.m_gameStatus1.haveDressUpItemNum > haveDressUpItemNumMax )
      {
        ICHI_PRINT(">UPPER 範囲外着せ替えアイテム総数\n");
        m_core.m_gameStatus1.haveDressUpItemNum = 6;
      }
    }
  }

  // 範囲外の着せ替え
  {
    // ScriptFuncSetJoinFesta.cppのThatKisekaeNiceで、着せ替えをコンバートしているのでそこでデフォルトに直る
  }

  // 範囲外のアンケート回答
  {
    if( m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      for( u32 i=0; i<16; i++ )
      { 
        // アンケート回答数
        const u8 answerMax = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetAnswerMaxFromQuestionnaireId( i );
        // @fix NMCat[4129]：アンケート回答の最大を含んでいた為、含まないように修正
        if( m_core.m_gameStatus1.questionnaireAnswer[i] > answerMax )
        {
          ICHI_PRINT(">UPPER 範囲外アンケート回答%d \n", i);
          // @fix GFNMCat[4432]：無回答はイベントに影響するのでナイショにしておく
          m_core.m_gameStatus1.questionnaireAnswer[i] = 1;
        }
      }
    }
  }

  // 範囲外の建てているお店
  {
    if( m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      for( u32 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
      {
        if( !NetApp::JoinFesta::ShopItemInfoTable::IsValidIndex( m_core.m_gameStatus1.facilitiesId[ i ] ) )
        {
          ICHI_PRINT(">UPPER 範囲外施設ID index[]\n", i);
          m_core.m_gameStatus1.facilitiesId[ i ] = 0;
        }
      }
    }
  }

  // バトルフェスの最大値をチェックする
  {
    if (m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL)
    {
      if (m_core.m_gameStatus1.battleFesPokeId > BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX)
      {
        MUNAKATA_PRINT(">UPPER バトルツリーID\n");
        m_core.m_gameStatus1.battleFesPokeId = 0;
      }
      if (m_core.m_gameStatus1.battleFesPokeSex >= BattleFes::BattleFesDefine::POKEMON_SEX_END)
      {
        MUNAKATA_PRINT(">UPPER バトルツリー性別\n");
        m_core.m_gameStatus1.battleFesPokeSex = 0;
      }
      if (m_core.m_gameStatus1.battleFesPokeTokuseiIndex >= BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END)
      {
        MUNAKATA_PRINT(">UPPER バトルツリー特性\n");
        m_core.m_gameStatus1.battleFesPokeTokuseiIndex = 0;
      }
      if (m_core.m_gameStatus1.battleFesRank > BattleFes::BattleFesDefine::BFES_RANK_MAX)
      {
        MUNAKATA_PRINT(">UPPER バトルフェスランク\n");
        m_core.m_gameStatus1.battleFesRank = 0;
      }
      if (m_core.m_gameStatus1.ballThrowType >= Savedata::Misc::BALL_THROW_TYPE_NUM)
      {
        MUNAKATA_PRINT(">UPPER バトルフェスボール投げ\n");
        m_core.m_gameStatus1.ballThrowType = 0;
      }
      if (m_core.m_gameStatus1.has_mega_ring > true)
      {
        MUNAKATA_PRINT(">UPPER バトルフェスメガリング\n");
        m_core.m_gameStatus1.has_mega_ring = false;
      }
      if (m_core.m_gameStatus1.has_zenryoku_ring > true)
      {
        MUNAKATA_PRINT(">UPPER バトルフェス全力フラグ\n");
        m_core.m_gameStatus1.has_zenryoku_ring = false;
      }
    }
  }

  // 範囲外のダイアリー
  {
    // ScriptFuncSetJoinFesta.cpp のPokeDiaryTalkで行っている
  }

  // 不正なファインダースタジオの写真
  {
    if( m_core.m_gameStatus6.photoParam.packetHeaderSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      App::FinderStudio::Viewer::PhotoParam photoParam = m_core.m_gameStatus6.photoParam;

      bool isNgData = false; // 不正なデータだったか

      // ポケモンNo.
      if( photoParam.monsNo == MONSNO_NULL || photoParam.monsNo > MONSNO_END ) // タマゴは含めない
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：ポケモンNo %d \n", photoParam.monsNo);
      }
      // ネタばれ対策
      // momijiの幻ポケモンのゼラオラは秘匿性が高いので、初めて写真で目にするのを防ぐ
      if( photoParam.monsNo == MONSNO_MABOROSI18 )
      {
        // ゼラオラを捕獲している場合は、見えても問題無い。
        // 捕獲していない場合、写真を受け取っていない事にする。
        Savedata::ZukanData* pZukanData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetZukanData();

        if( pZukanData->GetPokeGetFlag( MONSNO_MABOROSI18 ) == false )
        {
          isNgData = true;
          ICHI_PRINT( ">ICHI ゼラオラネタばれNG \n");
        }
      }


      // 性別
      if( photoParam.sexPoke >= pml::SEX_NUM )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：ポケモン性別 %d \n", photoParam.sexPoke);
      }

      // 背景：インデックス
      if( photoParam.bgSetIndex >= BG_ID_NUM )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：背景%d \n", photoParam.bgSetIndex);
      }
      // 背景：戦闘背景の時間アニメーション種
      if( photoParam.bgTimeAnmType > 11 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：背景アニメ種%d \n", photoParam.bgTimeAnmType);
      }
      // 背景：時間帯
      if( photoParam.timeZone > 2 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：時間帯%d \n", photoParam.timeZone);
      }
      // トレーナー：モーション種
      if( photoParam.trainerMotionType > 40 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：トレーナーモーション%d \n", photoParam.trainerMotionType);
      }
      // 座標：ポケモンX座標
      if( photoParam.pokePosX < -25000 || photoParam.pokePosX > 25000  )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：ポケモンX%d \n", photoParam.pokePosX);
      }
      // 座標：ポケモンZ座標
      if( photoParam.pokePosZ < -25000 || photoParam.pokePosZ > 25000  )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：ポケモンZ%d \n", photoParam.pokePosZ);
      }
      // 座標：ポケモン角度
      if( photoParam.pokeDegY > 359 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：ポケモン角度%d \n", photoParam.pokeDegY);
      }
      // 座標：トレーナー角度
      if( photoParam.trainerDegY > 359 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：トレーナー角度%d \n", photoParam.trainerDegY);
      }
      // カメラ：カメラ高さ
      if( photoParam.cameraHeight > 100 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：カメラ高さ%d \n", photoParam.cameraHeight);
      }
      // カメラ：カメラY軸角度
      if( photoParam.cameraDegY > 359 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：カメラY角度%d \n", photoParam.cameraDegY);
      }
      // カメラ：カメラX軸角度
      // @fix MMCat[670] カメラの上限角度を狭めたので不正チェックも修正
      //                 一楽さん了承済み
      if( photoParam.cameraDegX > 73 || photoParam.cameraDegX < 1 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：カメラX角度%d \n", photoParam.cameraDegX);
      }
      // カメラ：カメラ画角
      if( photoParam.cameraFov > 100 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：カメラ画角%d \n", photoParam.cameraFov);
      }
      // カメラ：カメラ係数
      if( photoParam.cameraFocusFactor > 100 )
      {
        isNgData = true;
        ICHI_PRINT(">ICHI FS不正：カメラ係数%d \n", photoParam.cameraFocusFactor);
      }


      if( isNgData == true )
      {
        // データを無効にする。（受け取ってないことになる。）
        gfl2::std::MemClear( &m_core.m_gameStatus6, sizeof( PacketGameStatus6 ) );
      }
    }


    if( m_core.m_gameStatus7.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      App::FinderStudio::Static::DecorationData decorationData = m_core.m_gameStatus7.decorationData;

      bool isNgData = false; // 不正なデータだったか

      for( u8 i = 0; i < App::FinderStudio::Static::DECO_ITEM_MAX; i++ )
      {
        App::FinderStudio::Static::StampData stampData = decorationData.deco_param[i];

        // スタンプID最大値チェック
        if( stampData.id != App::FinderStudio::Static::DECO_NULL &&
            stampData.id >= App::FinderStudio::Static::DecoDataAccessor::GetDecoDataListIndex( DECO_TYPE_FRAME, 0 ) )
        {
          isNgData = true;
          ICHI_PRINT( ">ICHI FS不正：スタンプID%d \n", stampData.id );
        }
        // 座標チェック
        if( stampData.x > 400 || stampData.y > 240 )
        {
          isNgData = true;
          ICHI_PRINT( ">ICHI FS不正：スタンプ座標%d:%d \n", stampData.x, stampData.y );
        }
        // フレームIDチェック
        // ※要素0のみフレームIDが参照されるが全部チェックしても問題無いので回す
        if( stampData.frame_id > App::FinderStudio::Static::DecoDataAccessor::GetDecoTypeMax( DECO_TYPE_FRAME ) )
        {
          isNgData = true;
          ICHI_PRINT( ">ICHI FS不正：フレームID%d \n", stampData.frame_id );
        }
      }

      if( isNgData == true )
      {
        // データを無効にする。（受け取ってないことになる。）
        gfl2::std::MemClear( &m_core.m_gameStatus7, sizeof( PacketGameStatus7 ) );
      }
    }

  }

  // @fix MMCat[337]：相手がnijiの場合フラグをたてる。
  {
    if( GetRomVersion() == VERSION_SUN || GetRomVersion() == VERSION_MOON )
    {
      m_core.m_personalInfo.icon.dressUpParam.isRomVersionNiji = true;
    }
  }

}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   バトルフェスパケットデータの補間対応処理
 * @memo    nijiは存在しないデータなのでmomijiで補間する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::BattleFesPacketInterpolation()
{
  // nijiの人は、momijjでレンタルポケモンを補間する
  if( m_core.m_personalInfo.romCode == VERSION_SUN ||
      m_core.m_personalInfo.romCode == VERSION_MOON )
  {
    if( m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
    {
      // その日のその人の乱数
      gfl2::math::Random random;
      u32 seed = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetSaveData()->GetRandomGroup()->GetTodayRandSeed();

      // TIDの下1桁を引く
      seed -= (GetTransferedID() % 10);
      random.Initialize( seed );

      // バトルフェス用データ
      m_core.m_gameStatus1.battleFesPokeId            = random.Next( BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX );        // BATTLE_TREE_ID_MAXは無効値
      m_core.m_gameStatus1.battleFesPokeSex           = random.Next( BattleFes::BattleFesDefine::POKEMON_SEX_END );           // バトルフェスで選んだポケモンの性別
      m_core.m_gameStatus1.battleFesPokeTokuseiIndex  = random.Next( BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END );       // バトルフェスで選んだポケモンの特性Index
      m_core.m_gameStatus1.battleFesRank              = 0;  // バトルフェスのランク
      m_core.m_gameStatus1.ballThrowType              = 0;  // バトルのボール投げの種類(0～7)

      // 着せ替えのデータから判別する
      m_core.m_gameStatus1.has_mega_ring      = m_core.m_personalInfo.icon.dressUpParam.hasZRing;  // メガリング入手フラグ
      m_core.m_gameStatus1.has_zenryoku_ring  = m_core.m_personalInfo.icon.dressUpParam.hasZRing;  // ぜんりょくリング入手フラグ
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ファインダースタジオの写真Newフラグ取得
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsNewPhoto() const
{
  // 写真のパケット受信チェック
  App::FinderStudio::Static::PhotoData photoData;
  if( GetPacketPhotoData( &photoData ) )
  {
    return m_core.m_newPhoto;
  }

  return false; // 未受信はNewフラグOFF
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ファインダースタジオの写真NewフラグをOFFにする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetNewPhotoOff()
{
  m_core.m_newPhoto = false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ファインダースタジオの写真NewフラグをOFFにする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetNewPhotoOffEx(const u32 otherCRC)
{
  App::FinderStudio::Static::PhotoData  photoData;

  if(GetPacketPhotoData(&photoData))
  {
    if(photoData.deco_param.crc == otherCRC)
    {
      SetNewPhotoOff();
    }
  }
}


#if PM_DEBUG
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来の遊び招待リクエストを設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureCommand( void )
{
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_UPPER_DUMMY;
  UpdatePacketTimestamp();
}
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来の共通パケットを設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureUnion( void )
{
  SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_UPPER_DUMMY );
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来の定型文を設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFuturePattern( JoinFestaScript::PatternType patternType ) 
{
  switch( patternType ){
  case JoinFestaScript::PATTERN_HELLO:    //!< 定型：あいさつ
    m_core.m_personalInfo.patternHello = 9999;
    break;
  case JoinFestaScript::PATTERN_PARTING:  //!< 定型：別れ
    m_core.m_personalInfo.patternParting = 9999;
    break;
  case JoinFestaScript::PATTERN_EMOTION:  //!< 定型：感動
    m_core.m_personalInfo.patternEmotion = 9999;
    break;
  case JoinFestaScript::PATTERN_REGRET:   //!< 定型：残念
    m_core.m_personalInfo.patternRegret = 9999;
    break;
  case JoinFestaScript::PATTERN_SECRET:   //!< 定型：ナイショ
    m_core.m_personalInfo.patternSecret = 9999;
    break;
  }
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来の一押し施設IDを設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureRecommendFacilitiesId( void )
{
  m_core.m_personalInfo.recommendFacilitiesId = 0xFF;
  UpdatePacketTimestamp();
}
void JoinFestaPersonalData::SetRequestFutureRecommendFacilitiesColor( void )
{
  m_core.m_personalInfo.recommendFacilitiesColor = 7; // 3bit
  UpdatePacketTimestamp();
}
void JoinFestaPersonalData::SetRequestFutureRecommendFacilitiesCharacterId( void )
{
  m_core.m_personalInfo.recommendFacilitiesCharacterId = 0xF; // 4bit
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来のフェスタランク設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureFesRank( void )
{
  m_core.m_gameStatus1.jfRank = 0xFFFF;
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来のきせかえアイテム総数設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureHaveDressUpItemNum( void )
{
  m_core.m_gameStatus1.haveDressUpItemNum = 9999;
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来のアンケート回答設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureQuestionnaireAnswer( void )
{
  for( u32 i=0; i<16; i++ )
  { // アンケート回答
    m_core.m_gameStatus1.questionnaireAnswer[i] = 99;
  }
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来の所持施設ID設定する（アッパーROM動作確認用）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFutureAllFacilitiesId( void )
{
  for( u32 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    m_core.m_gameStatus1.facilitiesId[ i ] = 0xFF;
  }
  UpdatePacketTimestamp();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]未来のポケダイアリーID設定する（アッパーROM動作確認用）
 * @param   sendPattern   送信パターン(0～6)
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestFuturePokeDiaryId( int sendPattern )
{
  SetPacketGameStatusWithProfile( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData() );

  switch( sendPattern ){
  case 0:
    m_core.m_gameStatus5.pokeDiary[0].diary_id  = PDID_COUNT_NUM;
    break;
  case 1:
    m_core.m_gameStatus5.pokeDiary[1].diary_id  = PDID_COUNT_NUM;
    break;
  case 2:
    m_core.m_gameStatus5.pokeDiary[2].diary_id  = PDID_COUNT_NUM;
    break;
  case 3:
    m_core.m_gameStatus5.pokeDiary[0].diary_id  = PDID_COUNT_NUM;
    m_core.m_gameStatus5.pokeDiary[1].diary_id  = PDID_COUNT_NUM;
    break;
  case 4:
    m_core.m_gameStatus5.pokeDiary[0].diary_id  = PDID_COUNT_NUM;
    m_core.m_gameStatus5.pokeDiary[2].diary_id  = PDID_COUNT_NUM;
    break;
  case 5:
    m_core.m_gameStatus5.pokeDiary[1].diary_id  = PDID_COUNT_NUM;
    m_core.m_gameStatus5.pokeDiary[2].diary_id  = PDID_COUNT_NUM;
    break;
  case 6:
    m_core.m_gameStatus5.pokeDiary[0].diary_id  = PDID_COUNT_NUM;
    m_core.m_gameStatus5.pokeDiary[1].diary_id  = PDID_COUNT_NUM;
    m_core.m_gameStatus5.pokeDiary[2].diary_id  = PDID_COUNT_NUM;
    break;
  }

  UpdatePacketTimestamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   [無]不正のファインダースタジオデータを設定する（動作確認用）
 * @param   pattern   送信パターン(0～19)
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetRequestNgFinderStudio( int pattern )
{
  SetPacketGameStatusWithProfile( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData() );

  switch( pattern ){
  case 0: m_core.m_gameStatus6.photoParam.monsNo = MONSNO_END + 1; break;
  case 1: m_core.m_gameStatus6.photoParam.sexPoke = pml::SEX_NUM ; break;
  case 2: m_core.m_gameStatus6.photoParam.bgSetIndex = BG_ID_NUM + 1; break;
  case 3: m_core.m_gameStatus6.photoParam.bgTimeAnmType = 15; break;
  case 4: m_core.m_gameStatus6.photoParam.timeZone = 3; break;
  case 5: m_core.m_gameStatus6.photoParam.trainerMotionType = 50; break;
  case 6: m_core.m_gameStatus6.photoParam.pokePosX = 30000; break;
  case 7: m_core.m_gameStatus6.photoParam.pokePosZ = 30000; break;
  case 8: m_core.m_gameStatus6.photoParam.pokeDegY = 400; break;
  case 9:m_core.m_gameStatus6.photoParam.trainerDegY = 400; break;
  case 10:m_core.m_gameStatus6.photoParam.cameraHeight = 101; break;
  case 11:m_core.m_gameStatus6.photoParam.cameraDegY = 400; break;
  case 12:m_core.m_gameStatus6.photoParam.cameraDegX = 0; break;
  case 13:m_core.m_gameStatus6.photoParam.cameraFov = 101; break;
  case 14:m_core.m_gameStatus6.photoParam.cameraFocusFactor = 101; break;
  case 15:m_core.m_gameStatus7.decorationData.deco_param[0].id = 200; break;
  case 16:m_core.m_gameStatus7.decorationData.deco_param[9].id = 200; break;
  case 17:m_core.m_gameStatus7.decorationData.deco_param[0].x = 401; break;
  case 18:m_core.m_gameStatus7.decorationData.deco_param[0].y = 241; break;
  case 19:m_core.m_gameStatus7.decorationData.deco_param[0].frame_id = 60; break;
  }

  UpdatePacketTimestamp();
}

#endif


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ゲームパケット情報内のプロフィール情報部を設定する
 *
 * @param   gameData  ゲームデータ情報
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetPacketGameStatusWithProfile( GameSys::GameData* gameData )
{


  // 拡張情報1
  {
    Savedata::MyStatus*          pMyStatus          = gameData->GetPlayerStatus();
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = gameData->GetJoinFestaData();
    Savedata::Fashion*           pFashion           = gameData->GetFashion();
    Savedata::BattleFesSave*     pBattleFesSave     = gameData->GetSaveData()->GetBattleFesSave();

    m_core.m_gameStatus1.jfCoinCount        = pJoinFestaDataSave->GetTotalCoin();
    m_core.m_gameStatus1.jfRank             = pJoinFestaDataSave->GetRank();

    // 着せ替えアイテム所持数
    m_core.m_gameStatus1.haveDressUpItemNum = pFashion->GetHaveItemTotalNum();
    ICHI_PRINT(">ICHI 着せ替えアイテム総所持数:%d \n", m_core.m_gameStatus1.haveDressUpItemNum );

    for( u32 i=0; i<16; i++ )
    { // アンケート回答
      m_core.m_gameStatus1.questionnaireAnswer[i] = pJoinFestaDataSave->GetQuestionnaireAnswer( i );
    }

    for( u32 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
    {
      m_core.m_gameStatus1.facilitiesId[ i ] = pJoinFestaDataSave->GetFacilitiesData( static_cast<JoinFestaScript::FacilitiesIndex>(i) )->facilitiesId;
    }

    // ★momiji新規
    // バトルフェス用のデータ
    // バトルフェスのセーブから取得
    m_core.m_gameStatus1.battleFesPokeId            = pBattleFesSave->GetBattleTreeID();              // バトルフェスで選んだポケモンのID
    m_core.m_gameStatus1.battleFesPokeSex           = pBattleFesSave->GetBattleTreeSex();             // バトルフェスで選んだポケモンの性別
    m_core.m_gameStatus1.battleFesPokeTokuseiIndex  = pBattleFesSave->GetBattleTreeTokusei();         // バトルフェスで選んだポケモンの特性Index
    m_core.m_gameStatus1.battleFesRank              = pBattleFesSave->GetRank();                      // バトルフェスのランク
    m_core.m_gameStatus1.ballThrowType              = pMyStatus->GetBallThrowType();                  // バトルのボール投げの種類(0～7)
    m_core.m_gameStatus1.has_mega_ring              = static_cast<u8>(pMyStatus->HasMegaRing());      // メガリング入手フラグ
    m_core.m_gameStatus1.has_zenryoku_ring          = static_cast<u8>(pMyStatus->HasZenryokuRing());  // ぜんりょくリング入手フラグ

    m_core.m_gameStatus1.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
  }

  // レコード系
  {
    Savedata::Record* pRecord = gameData->GetRecordSaveData();

    // 拡張情報2
    m_core.m_gameStatus2.record1    = pRecord->Get( Savedata::Record::RECID_COMM_BATTLE );
    m_core.m_gameStatus2.record2    = pRecord->Get( Savedata::Record::RECID_WIFI_BATTLE );
    m_core.m_gameStatus2.record3    = pRecord->Get( Savedata::Record::RECID_BTREE_CHALLENGE );
    m_core.m_gameStatus2.record4    = pRecord->Get( Savedata::Record::RECID_BTL_ENCOUNT );
    m_core.m_gameStatus2.record5    = pRecord->Get( Savedata::Record::RECID_WEAK_ATTACK );
    m_core.m_gameStatus2.record6    = pRecord->Get( Savedata::Record::RECID_COMM_TRADE );
    m_core.m_gameStatus2.record7    = pRecord->Get( Savedata::Record::RECID_MIRACLE_TRADE_COUNT );
    m_core.m_gameStatus2.record8    = pRecord->Get( Savedata::Record::RECID_GTS_TRADE_COUNT );
    m_core.m_gameStatus2.record9    = pRecord->Get( Savedata::Record::RECID_GTS_PUT );
    m_core.m_gameStatus2.record10   = pRecord->Get( Savedata::Record::RECID_DAYMAX_EXP );
    m_core.m_gameStatus2.record11   = pRecord->Get( Savedata::Record::RECID_MAME_GET_CNT );
    m_core.m_gameStatus2.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

    // 拡張情報3
    m_core.m_gameStatus3.record1    = pRecord->Get( Savedata::Record::RECID_BERRY_HARVEST );
    m_core.m_gameStatus3.record2    = pRecord->Get( Savedata::Record::RECID_SHOPPING_MONEY );
    m_core.m_gameStatus3.record3    = pRecord->Get( Savedata::Record::RECID_USE_BP );
    m_core.m_gameStatus3.record4    = pRecord->Get( Savedata::Record::RECID_CAPTURE_POKE );
    m_core.m_gameStatus3.record5    = pRecord->Get( Savedata::Record::RECID_TAMAGO_HATCHING );
    m_core.m_gameStatus3.record6    = pRecord->Get( Savedata::Record::RECID_MAME_PRESENT_CNT);
    m_core.m_gameStatus3.record7    = pRecord->Get( Savedata::Record::RECID_NICKNAME );
    m_core.m_gameStatus3.record8    = pRecord->Get( Savedata::Record::RECID_RIDE_POKE );
    m_core.m_gameStatus3.record9    = pRecord->Get( Savedata::Record::RECID_JF_USE_JCOIN );
    m_core.m_gameStatus3.record10   = pRecord->Get( Savedata::Record::RECID_ZWAZA_CNT );
    m_core.m_gameStatus3.record11   = pRecord->Get( Savedata::Record::RECID_JF_GEST_TALK_CNT );
    m_core.m_gameStatus3.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

    // 拡張情報4
    m_core.m_gameStatus4.record1    = pRecord->Get( Savedata::Record::RECID_DENDOU_CNT );
    // @fix NMCat[4123]：RECID_KAWAIGARI_CNTがnijiでは欠番だった為、変わりにRECID_PARLE_NADEを使用する。
    {
      // PARLE_NADEは４バイトレコード
      u32 recordValue = pRecord->Get( Savedata::Record::RECID_PARLE_NADE );
      // カンストはKAWAIGARI_CNTで行う(最大値9999）
      if( recordValue > pRecord->GetMax( Savedata::Record::RECID_KAWAIGARI_CNT ) )
      {
        recordValue = pRecord->GetMax( Savedata::Record::RECID_KAWAIGARI_CNT );
      }
      m_core.m_gameStatus4.record2 = static_cast<u16>(recordValue);
    }
    m_core.m_gameStatus4.record3    = pRecord->Get( Savedata::Record::RECID_DAYMAX_CAPTURE );
    m_core.m_gameStatus4.record4    = pRecord->Get( Savedata::Record::RECID_DAYMAX_EVOLUTION );
    m_core.m_gameStatus4.record5    = pRecord->Get( Savedata::Record::RECID_DAYMAX_KILL );
    m_core.m_gameStatus4.record6    = pRecord->Get( Savedata::Record::RECID_DAYMAX_TRAINER_BATTLE );
    m_core.m_gameStatus4.record7    = pRecord->Get( Savedata::Record::RECID_JF_ATT_CNT );
    m_core.m_gameStatus4.record8    = pRecord->Get( Savedata::Record::RECID_JF_SHOP_GET_CNT );
    m_core.m_gameStatus4.record9    = pRecord->Get( Savedata::Record::RECID_RARE_COLOUR_ENC_COUNT );
    m_core.m_gameStatus4.record10   = pRecord->Get( Savedata::Record::RECID_IDKUJI_ATARI_CNT );
    m_core.m_gameStatus4.record11   = pRecord->Get( Savedata::Record::RECID_POINTUP_USE );
    m_core.m_gameStatus4.record12   = pRecord->Get( Savedata::Record::RECID_KISEKAE_CNT );
    m_core.m_gameStatus4.record13   = pRecord->Get( Savedata::Record::RECID_QR_SCAN );
    m_core.m_gameStatus4.record14   = pRecord->Get( Savedata::Record::RECID_CAMERA_CNT );
    m_core.m_gameStatus4.record15   = pRecord->Get( Savedata::Record::RECID_LUNASOL_BESTSCORE );
    m_core.m_gameStatus4.record16   = pRecord->Get( Savedata::Record::RECID_MANTAIN_SURF_BEST_BP );
    m_core.m_gameStatus4.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
  }


  // ポケダイアリー系
  {
    Savedata::PokeDiarySave* pDiary = gameData->GetPokeDiarySave();
    // 最新3件のデータを取ってくる
    pDiary->CreatePlayLog( m_core.m_gameStatus5.pokeDiary );
    m_core.m_gameStatus5.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
  }


  // ファインダースタジオ写真
  {
    Savedata::FinderStudioSave* pFinderStudioSave = gameData->GetFinderStudioSave();

    // 公開写真を設定していた場合セット
    if( pFinderStudioSave->IsSendPhotoDataEnable() )
    {
      // 写真データ
      m_core.m_gameStatus6.photoParam = pFinderStudioSave->GetSendPhotoData().photo_data.photo_param;
      m_core.m_gameStatus6.photoParam.packetHeaderSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

      // デコデータ
      m_core.m_gameStatus7.decorationData = pFinderStudioSave->GetSendPhotoData().photo_data.deco_param;
      m_core.m_gameStatus7.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

      GFL_ASSERT( m_core.m_gameStatus7.decorationData.crc != 0 );
    }
    else
    {
      // 公開していない場合は、無効状態
      gfl2::std::MemClear( &m_core.m_gameStatus6, sizeof( PacketGameStatus6 ) );
      gfl2::std::MemClear( &m_core.m_gameStatus7, sizeof( PacketGameStatus7 ) );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   汎用通信コマンドの設定
 *
 * @param   commandId 設定するコマンド
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetBasicPacketCommand( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND commandId )
{
  GFL_ASSERT( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsBasicCommand( commandId ) );

  m_core.m_header.requestCommand   = commandId;

  UpdatePacketTimestamp();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクエストコマンドを取得する
 *
 * @return  設定されているリクエストコマンド（NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND）
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND JoinFestaPersonalData::GetRequestCommand( void ) const
{
  return static_cast<JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND>(m_core.m_header.requestCommand);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   送信するパケットの共有部分の種類の設定
 *
 * @param[in]   unionPakcetType E_JOIN_FESTA_UNION_PACKET_TYPE定義
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TYPE unionPakcetType )
{
  //ICHI_PRINT(">ICHI SetUnionPacketType:%d \n", unionPakcetType);
  m_core.m_header.unionPacketType = static_cast<u8>(unionPakcetType);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   送信するパケットの共有部分の種類を取得する
 *
 * @return  設定されているパケット共有種類（NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TYPE）
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TYPE JoinFestaPersonalData::GetUnionPacketType( void ) const
{
  return static_cast<JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TYPE>(m_core.m_header.unionPacketType);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信パケット内個人情報を取得する
 *
 * @return  通信パケット内個人情報(PacketPersonalInfo)
 */
/* ----------------------------------------------------------------------------------------- */
const PacketPersonalInfo& JoinFestaPersonalData::GetPacketPersonalInfo( void ) const
{
  return m_core.m_personalInfo;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信パケット内ゲームステータス情報を取得する
 *
 * @param   dst情報格納先へのポインタ
 *
 * @return  情報の取得に成功したならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::GetPacketGameStatus1( PacketGameStatus1 *dst ) const
{
  if( m_core.m_gameStatus1.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    gfl2::std::MemCopy( &m_core.m_gameStatus1, dst, sizeof(PacketGameStatus1) );
    return true;
  }
  return false;
}
bool JoinFestaPersonalData::GetPacketGameStatus2( PacketGameStatus2 *dst ) const
{
  if( m_core.m_gameStatus2.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    gfl2::std::MemCopy( &m_core.m_gameStatus2, dst, sizeof(PacketGameStatus2) );
    return true;
  }
  return false;
}
bool JoinFestaPersonalData::GetPacketGameStatus3( PacketGameStatus3 *dst ) const
{
  if( m_core.m_gameStatus3.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    gfl2::std::MemCopy( &m_core.m_gameStatus3, dst, sizeof(PacketGameStatus3) );
    return true;
  }
  return false;
}
bool JoinFestaPersonalData::GetPacketGameStatus4( PacketGameStatus4 *dst ) const
{
  if( m_core.m_gameStatus4.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    gfl2::std::MemCopy( &m_core.m_gameStatus4, dst, sizeof(PacketGameStatus4) );
    return true;
  }
  return false;
}
bool JoinFestaPersonalData::GetPacketGameStatus5( PacketGameStatus5 *dst ) const
{
  if( m_core.m_gameStatus5.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    gfl2::std::MemCopy( &m_core.m_gameStatus5, dst, sizeof(PacketGameStatus5) );
    return true;
  }
  return false;
}
bool JoinFestaPersonalData::GetPacketPhotoData( App::FinderStudio::Static::PhotoData* dst ) const
{
  // 拡張情報の6,7を受け取っている事が前提
  if( m_core.m_gameStatus6.photoParam.packetHeaderSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL 
      && m_core.m_gameStatus7.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    // 中身のペアチェック。crcが一致したらペアとする。
    App::FinderStudio::Static::PhotoData photoData;
    photoData.photo_param = m_core.m_gameStatus6.photoParam;
    photoData.photo_param.packetHeaderSymbol = 0;   // crc計算時には通信ヘッダはないので初期化
    //
    photoData.deco_param = m_core.m_gameStatus7.decorationData;

    if( App::FinderStudio::Static::FinderStudioUtil::CheckPhotoDataCrc( &photoData ) )
    {
      gfl2::std::MemCopy( &photoData, dst, sizeof( App::FinderStudio::Static::PhotoData ) );
      return true;
    }
    else
    {
      ICHI_PRINT("FS PhotoData受信でcrc不一致\n");
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信パケット内のレコード情報を取得する
 *
 * @param[in]  レコードカテゴリ
 * @param[in]  レコードカテゴリ内のインデックス
 *
 * @return  データ（-1でデータなし)
 */
/* ----------------------------------------------------------------------------------------- */
s32 JoinFestaPersonalData::GetRecordData( const u32 recordCategory, const u32 recordIndex ) const
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  const bool bClearFlag_ALL = pEventWork->CheckEventFlag( SYS_FLAG_GAME_CLEAR ); // ストーリー全クリアフラグ
  const bool bZWazaFlag = ( Savedata::GetRecord( Savedata::Record::RECID_ZWAZA_CNT ) == 0 ) ? false : true;

  switch( recordCategory ){
  case JoinFestaScript::RECORD_CATEGORY_BATTLE:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus2.record1; // RECID_COMM_BATTLE 通信対戦
      case 1: return m_core.m_gameStatus2.record2; // RECID_WIFI_BATTLE バトルスポット
      case 2: return bClearFlag_ALL ? static_cast<s32>(m_core.m_gameStatus2.record3) : -1; // RECID_BTREE_CHALLENGE バトルツリー
      case 3: return m_core.m_gameStatus2.record4; // RECID_BTL_ENCOUNT やせいポケモン
      case 4: return bClearFlag_ALL ? m_core.m_gameStatus4.record1 : -1; // RECID_DENDOU_CNT ぼうえいかいすう
      case 5: return m_core.m_gameStatus2.record5; // RECID_WEAK_ATTACK こうかばつぐん
      case 6: return bZWazaFlag ? static_cast<s32>(m_core.m_gameStatus3.record10) : -1; // RECID_ZWAZA_CNT つかったZワザ
      }
    }
    break;
  case JoinFestaScript::RECORD_CATEGORY_TRADE:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus2.record6; // RECID_COMM_TRADE 通信交換
      case 1: return m_core.m_gameStatus2.record7; // RECID_MIRACLE_TRADE_COUNT ミラクル交換
      case 2: return m_core.m_gameStatus2.record8; // RECID_GTS_TRADE_COUNT GTSで交換
      case 3: return m_core.m_gameStatus2.record9; // RECID_GTS_PUT GTSに預け換
      }
    }
    break;
  case JoinFestaScript::RECORD_CATEGORY_GREED:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus2.record11; // RECID_MAME_GET_CNT 集めたポケマメ
      case 1: return m_core.m_gameStatus3.record1; // RECID_BERRY_HARVEST 集めたきのみ
      case 2: return m_core.m_gameStatus3.record2; // RECID_SHOPPING_MONEY 使ったおこづかい
      case 3: return m_core.m_gameStatus3.record3; // RECID_USE_BP 使ったBP
      case 4: return m_core.m_gameStatus1.haveDressUpItemNum; // 集めた服
      }
    }
    break;
  case JoinFestaScript::RECORD_CATEGORY_POKEMON:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus3.record4; // RECID_CAPTURE_POKE つかまえたポケモン
      case 1: return m_core.m_gameStatus3.record5; // RECID_TAMAGO_HATCHING 孵化させたポケモン
      case 2: return m_core.m_gameStatus4.record2; // RECID_KAWAIGARI_CNT なでた回数
      case 3: return m_core.m_gameStatus3.record6; // RECID_MAME_PRESENT_CNT ポケマメをあげた
      case 4: return m_core.m_gameStatus3.record7; // RECID_NICKNAME つけたニックネーム
      case 5: return m_core.m_gameStatus3.record8; // RECID_RIDE_POKE ポケモンに乗った
      }
    }
    break;
  case JoinFestaScript::RECORD_CATEGORY_DAILY:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus2.record10; // RECID_DAYMAX_EXP 集めた経験値
      case 1: return m_core.m_gameStatus4.record3; // RECID_DAYMAX_CAPTURE 捕まえたポケモン
      case 2: return m_core.m_gameStatus4.record4; // RECID_DAYMAX_EVOLUTION 進化させたポケモン
      case 3: return m_core.m_gameStatus4.record5; // RECID_DAYMAX_KILL 倒したポケモン
      case 4: return m_core.m_gameStatus4.record6; // RECID_DAYMAX_TRAINER_BATTLE 戦ったトレーナー
      }
    }
    break;
  case JoinFestaScript::RECORD_CATEGORY_CIRCLE:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus1.jfRank; // フェスランク
      case 1: return m_core.m_gameStatus4.record7; // RECID_JF_ATT_CNT 参加アトラクション
      case 2: return m_core.m_gameStatus4.record8; // RECID_JF_SHOP_GET_CNT 招いたお店
      case 3: return m_core.m_gameStatus3.record11; // RECID_JF_GEST_TALK_CNT ゲストとの交流
      case 4: return m_core.m_gameStatus3.record9; // RECID_JF_USE_JCOIN 使ったコイン
      }
    }
    break;
  case JoinFestaScript::RECORD_CATEGORY_VARIETY:
    {
      switch( recordIndex ){
      case 0: return m_core.m_gameStatus4.record9; // RECID_RARE_COLOUR_ENC_COUNT 色違いにであった
      case 1: return m_core.m_gameStatus4.record10; // RECID_IDKUJI_ATARI_CNT IDくじに当たった
      case 2: return m_core.m_gameStatus4.record11; // RECID_POINTUP_USE PPをアップした
      case 3: return m_core.m_gameStatus4.record12; // RECID_KISEKAE_CNT 服をきがえた
      case 4: return m_core.m_gameStatus4.record13; // RECID_QR_SCAN QRスキャン
      case 5: return m_core.m_gameStatus4.record14; // RECID_CAMERA_CNT カメラで撮影した
      }
    }
    break;
  }

  GFL_ASSERT_MSG(0,"recordCategory:%d recordIndex:%d \n", recordCategory, recordIndex);
  return -1;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信対戦招待の返答情報がパケットに入っているか
 *
 * @return  trueで入っている（GetPacketBattleInviteで取得可能）
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsEnablePacketBattleInvite() const
{
  if( GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE ||
      GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CANCEL ||
      GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_CONNECT )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信対戦招待情報を取得する
 *
 * @return  対戦返答情報(PacketBattleInvite)
 */
/* ----------------------------------------------------------------------------------------- */
const PacketBattleInvite& JoinFestaPersonalData::GetPacketBattleInvite( void ) const
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_INVITE );
  return m_sub.m_unionPacket.battleInvite;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   メッセージ情報がパケットに入っているか
 *
 * @return  trueで入っている（GetPacketMessageで取得可能）
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsEnablePacketMessage() const
{
  // メッセージ使用するリクエストコマンドがない
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   メッセージ情報を取得する
 *
 * @return  メッセージ情報(PacketMessage)
 */
/* ----------------------------------------------------------------------------------------- */
const PacketMessage& JoinFestaPersonalData::GetPacketMessage( void ) const
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_MESSAGE );
  return m_sub.m_unionPacket.message;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信対戦申し込みの返答情報がパケットに入っているか
 *
 * @return  trueで入っている（GetPacketBattleRecieveで取得可能）
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsEnablePacketBattleRecieve() const
{
  if( GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_JOIN ||
      GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_DISJOIN )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信対戦申し込みの返答情報を取得する
 *
 * @return  対戦返答情報(PacketBattleRecieve)
 */
/* ----------------------------------------------------------------------------------------- */
const PacketBattleRecieve& JoinFestaPersonalData::GetPacketBattleRecieve( void ) const
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_BATTLE_RECIEVE );
  return m_sub.m_unionPacket.battleReceive;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信交換招待の返答情報がパケットに入っているか
 *
 * @return  trueで入っている（GetPacketTradeInviteで取得可能）
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsEnablePacketTradeInvite() const
{
  if( GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE ||
      GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CANCEL ||
      GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_CONNECT )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信交換招待情報を取得する
 *
 * @return  交換返答情報(PacketTradeInvite)
 */
/* ----------------------------------------------------------------------------------------- */
const PacketTradeInvite& JoinFestaPersonalData::GetPacketTradeInvite( void ) const
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_INVITE );
  return m_sub.m_unionPacket.tradeInvite;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信交換申し込みの返答情報がパケットに入っているか
 *
 * @return  trueで入っている（GetPacketTradeRecieveで取得可能）
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsEnablePacketTradeRecieve() const
{
  if( GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_JOIN ||
      GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_DISJOIN )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信交換申し込みの返答情報を取得する
 *
 * @return  交換返答情報(PacketTradeRecieve)
 */
/* ----------------------------------------------------------------------------------------- */
const PacketTradeRecieve& JoinFestaPersonalData::GetPacketTradeRecieve( void ) const
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TRADE_RECIEVE );
  return m_sub.m_unionPacket.tradeReceive;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション情報を取得する
 *
 * @return  アトラクション情報
 */
/* ----------------------------------------------------------------------------------------- */
const PacketAttraction& JoinFestaPersonalData::GetPacketAttraction( void ) const
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_ATTRACTION );
  return m_sub.m_unionPacket.attraction;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション情報を取得する
 *
 * @return  アトラクション情報
 */
/* ----------------------------------------------------------------------------------------- */
PacketAttraction* JoinFestaPersonalData::GetPacketAttractionPtr( void )
{
  //GFL_ASSERT( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_ATTRACTION );
  return &m_sub.m_unionPacket.attraction;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   バトル申請に絡むパケット情報を初期化
 *          (コマンドはJoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_NONEに設定）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ResetPacketRequest( void )
{
  gfl2::std::MemClear( &m_sub.m_unionPacket, sizeof(UnionPacket) );
  m_core.m_header.requestCommand = JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_NONE;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パケットの拡張情報を変更
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ChangePacketGameStatus( void )
{
#if PM_DEBUG
  // 拡張情報で変えられるとアッパー送信のデバッグが出来ないので処理しない
  if( GetUnionPacketType() == JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_UPPER_DUMMY )
  {
    return;
  }
#endif

  // 次の拡張情報に変更する

  // リクエストコマンド
  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command = GetRequestCommand();

  // 拡張情報がGameStatsuのコマンド時のみ変更適用
  if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsBasicCommand( command ) ){

    switch( GetUnionPacketType() ){
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_NONE:
      // 初期値
      m_sub.m_unionPacket.status1 = m_core.m_gameStatus1;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1 );
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1:      //!<  7:拡張情報1
      m_sub.m_unionPacket.status2 = m_core.m_gameStatus2;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_2 );
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_2:      //!<  8:拡張情報2
      m_sub.m_unionPacket.status3 = m_core.m_gameStatus3;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_3 );
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_3:      //!<  9:拡張情報3
      m_sub.m_unionPacket.status4 = m_core.m_gameStatus4;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_4 );
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_4:      //!<  10:拡張情報4
      m_sub.m_unionPacket.status5 = m_core.m_gameStatus5;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_5 );
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_5:      //!<  11:拡張情報5
      {
        // 無効状態ならばSkip
        if( m_core.m_gameStatus6.photoParam.packetHeaderSymbol != NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
        {
          m_sub.m_unionPacket.status1 = m_core.m_gameStatus1;
          SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1  );
        }
        else
        {
          m_sub.m_unionPacket.status6 = m_core.m_gameStatus6;
          SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_6 );
        }
      }
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_6:      //!<  12:拡張情報6
      {
        // 無効状態ならばSkip
        if( m_core.m_gameStatus7.headerSymbol != NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
        {
          m_sub.m_unionPacket.status1 = m_core.m_gameStatus1;
          SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1 );
        }
        else
        {
          m_sub.m_unionPacket.status7 = m_core.m_gameStatus7;
          SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_7 );
        }
      }
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_7:      //!<  13:拡張情報7
      m_sub.m_unionPacket.status1 = m_core.m_gameStatus1;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1 );
      break;
    case JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_8:      //!<  14:拡張情報8
    default:
      // 他のパケット情報だった場合は、初期化する（招待とか）
      m_sub.m_unionPacket.status1 = m_core.m_gameStatus1;
      SetUnionPacketType( JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_GAME_STATUS_1 );
      break;
    }

    UpdatePacketTimestamp();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信対戦リクエスト申請対象となるユーザーを追加
 *
 * @param   isWifi       ＷＩＦＩモードとして設定するならtrueを指定
 * @param   personal     追加ユーザーのJoinFestaフレンドキー配列
 * @param   relation     追加ユーザーとの関係性配列
 * @param   num          追加ユーザー数
 * @param   isCommitFlag 確定参加者フラグをセットするならtrueを指定
 *
 * @return  登録に成功したならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::SetBattleRequestTargetUser( bool isWifi, const JoinFestaFriendKey* personal,
                                                  JoinFestaDefine::E_JOIN_FESTA_RELATION* relation,  u32 num, bool isCommitFlag )
{
  if( JoinFestaDefine::JOIN_FESTA_BEACON_GUEST_USER_MAX < num ){
    return false;
  }
  PacketBattleInvite *inviteInfo = &m_sub.m_unionPacket.battleInvite;

  // 参加者情報を設定
  for( u32 i = 0; i < num; i++ )
  {
    if( isWifi ){
      inviteInfo->guestFriendId[i] = personal[i].principalId;
    }
    else{
      inviteInfo->guestFriendId[i] = personal[i].localFriendCode;
    }
    inviteInfo->relation[i] = relation[i];
  }
  inviteInfo->guestUserNum = num;

  // 確定参加者フラグを設定
  if( isCommitFlag )
  {
    inviteInfo->commitUserFlag = 0;
    for( u32 i = 0; i < num; i++ ){
      inviteInfo->commitUserFlag |= (1<<i);
    }
  }

  // ＷＩＦＩフラグを設定
  inviteInfo->isWIFI = isWifi;

  return true;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信交換リクエスト申請対象となるユーザーを追加
 *
 * @param   personal     追加ユーザーのJoinFestaフレンドキー
 * @param   relation     追加ユーザーとの関係
 * @param   num          追加ユーザー数。0の場合は確定招待者はいないものとする
 * @param   isCommitFlag 確定参加者フラグをセットするならtrueを指定
 *
 * @return  登録に成功したならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::SetTradeRequestTargetUser( const JoinFestaFriendKey& personal, JoinFestaDefine::E_JOIN_FESTA_RELATION relation, u32 num )
{
  PacketTradeInvite *inviteInfo = &m_sub.m_unionPacket.tradeInvite;

  // 参加者情報を設定
  inviteInfo->guestPrincipalId     = personal.principalId;
  inviteInfo->guestLocalFriendCode = personal.localFriendCode;
  inviteInfo->guestUserNum         = num;
  inviteInfo->relation             = relation;

  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定ユーザーがバトル招待対象リストに存在するか判定
 *
 * @param   personal 調査対象ユーザーのJoinFestaフレンドキー
 * @param   relation 対象者と主催者との関係
 *
 * @return  ターゲットリストに登録されているならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsBattleInviteTargetUser( const JoinFestaFriendKey& personal, JoinFestaDefine::E_JOIN_FESTA_RELATION* relation ) const
{
  const PacketBattleInvite& inviteInfo = this->GetPacketBattleInvite();

  for( u32 i = 0; i < inviteInfo.guestUserNum; i++ )
  {
    if( inviteInfo.isWIFI )
    {
      if( inviteInfo.guestFriendId[i] == personal.principalId )
      {
        if( relation != NULL ){
          *relation = static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(inviteInfo.relation[i]);
        }
        return true;
      }
    }
    else
    {
      if( inviteInfo.guestFriendId[i] == personal.localFriendCode )
      {
        if( relation != NULL ){
          *relation = static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(inviteInfo.relation[i]);
        }
        return true;
      }
    }
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定ユーザーがトレード招待対象リストに存在するか判定
 *
 * @param   personal 調査対象ユーザーのJoinFestaフレンドキー
 *
 * @return  ターゲットリストに登録されているならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsTradeInviteTargetUser( const JoinFestaFriendKey& personal ) const
{
  const PacketTradeInvite& inviteInfo = this->GetPacketTradeInvite();

  if( inviteInfo.guestUserNum == 0 ){
    return false;   // 登録なし
  }

  JoinFestaFriendKey guestFriendKey;
  guestFriendKey.initialize( inviteInfo.guestPrincipalId, inviteInfo.guestLocalFriendCode );

  return NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( personal, guestFriendKey );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   自身のパーソナルデータをセットアップする
 *
 * @param   gameData ゲームデータ情報
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupMyself( GameSys::GameData* gameData )
{
#if defined(GF_PLATFORM_CTR)
  gflnet2::friends::FriendManager* friendMgr = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
  NetAppLib::JoinFesta::PacketPersonalInfo* personalInf = &m_core.m_personalInfo;
  Savedata::MyStatus*              myStatus    = gameData->GetPlayerStatus();
  Savedata::JoinFestaDataSave*     pJoinFestaDataSave = gameData->GetJoinFestaData();
  //const field::EventWork*          eventWork   = gameData->GetEventWork();
  gfl2::str::STRCODE               name[ System::STRLEN_PLAYER_NAME+System::EOM_LEN ];

  // 名前
  myStatus->GetNameString( name );

  // 基本情報
  ::std::memcpy( personalInf->name , name , sizeof(personalInf->name) );
  personalInf->sex           = myStatus->GetSex();
  personalInf->addressId     = myStatus->GetSimpleAddressId().id;
  //personalInf->isGameClear   = eventWork->CheckEventFlag( SYS_FLAG_GAME_CLEAR );
  personalInf->regionCode    =  myStatus->GetRegionCode();
  personalInf->romCode       = myStatus->GetRomCode();
  personalInf->languageId    = myStatus->GetPokeLanguageId();

  personalInf->myPrincipalId = friendMgr->GetMyPrincipalId();
  personalInf->transferdId   = myStatus->GetTransferdID();

  // 定型分
  personalInf->patternHello     = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_HELLO ); // 定型：あいさつ
  personalInf->patternParting   = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_PARTING ); // 定型：別れ
  personalInf->patternEmotion   = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_EMOTION ); // 定型：感動
  personalInf->patternRegret    = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_REGRET ); // 定型：残念
  personalInf->patternSecret    = pJoinFestaDataSave->GetPattern( JoinFestaScript::PATTERN_SECRET ); // 定型：秘密

  // 着せ替え情報
  personalInf->icon = myStatus->GetIcon();

  // 通信拒否フラグ
  personalInf->isRejectFriend       = pJoinFestaDataSave->IsRejectFriend();
  personalInf->isRejectVip          = pJoinFestaDataSave->IsRejectVip();
  personalInf->isRejectGuest        = pJoinFestaDataSave->IsRejectGuest();
  personalInf->isAlwaysBattleOk     = pJoinFestaDataSave->IsAlwaysBattleOk();
  personalInf->isAlwaysTradeOk      = pJoinFestaDataSave->IsAlwaysTradeOk();
  personalInf->isAlwaysAttractionOk = pJoinFestaDataSave->IsAlwaysAttractionOk();

  {
    // イチオシ施設情報（0番がイチオシ）
    NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* pFacilitiesData = pJoinFestaDataSave->GetFacilitiesData( JoinFestaScript::FACILITIES_INDEX_0 );
    personalInf->recommendFacilitiesId = pFacilitiesData->facilitiesId;
    personalInf->recommendFacilitiesColor = pFacilitiesData->color;
    personalInf->recommendFacilitiesCharacterId = pFacilitiesData->characterId;
  }

  // 下位バージョン向け設定（@feature アッパーバージョン専用処理）
  if( (m_core.m_personalInfo.romCode > VERSION_SUN) && (m_core.m_personalInfo.romCode > VERSION_MOON) )
  {
    // @feature アッパーで対戦したくない時はtrueにする
    //personalInf->isLowerBattleReject = false;
  }

  // 通信情報
  SetPacketGameStatusWithProfile( gameData );
  SetRequestUpadateGameStatus();
  SetOnline( true );
  SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MY );
#else

#if PM_DEBUG
  // Winでもフレンドキーだけは入れておく
  m_core.m_personalInfo.myPrincipalId = System::GflUse::GetPublicRand();
  m_core.m_personalInfo.transferdId = System::GflUse::GetPublicRand();
#endif

#endif // GF_PLATFORM_CTR
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   チュートリアル中のダミーデータをセットアップする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupTutorialDummyData( const u32 index, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  ICHI_PRINT(">ICHI SetupTutorialDummyData\n");
  // ダミーデータフラグ
  m_sub.m_isDummyData = true;

  {
    NetAppLib::JoinFesta::PacketPersonalInfo* personalInf = &m_core.m_personalInfo;

    // 名前は無し
    personalInf->name[0] = L'\0';

    // 性別
    personalInf->sex = PM_MALE;

    // 定型分は固定
    personalInf->patternHello     = msg_jf_phrase_m_018; // 定型：あいさつ
    personalInf->patternParting   = msg_jf_phrase_m_320; // 定型：別れ
    personalInf->patternEmotion   = msg_jf_phrase_m_476; // 定型：感動
    personalInf->patternRegret    = msg_jf_phrase_m_304; // 定型：残念

    // 着せかえ 拡張
    u32 kisekaeId = 0;
    switch( index ){
    case 0: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_34; break;
    case 1: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_42; break;
    case 2: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_51; break;
    }

    // 着せ替えパターンで着せ替えデータ反映
    JoinFestaPersonalDataUtil::SetDressUpPattern( *this, kisekaeId, pDataTable );
    // ↑で性別も確定

    // フレンドキー
    // 何か入れていないとダメ。一言ウィンドウとカイリキーライドアトラクションで使っている。
    {
      m_core.m_personalInfo.myPrincipalId = System::GflUse::GetPublicRand();
      m_core.m_personalInfo.transferdId   = System::GflUse::GetPublicRand();
    }

    SetOnline( false );
    SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MY ); // Myじゃないとダメ。アトラクションイベントで使う
  }

}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   チュートリアル中のダミーイベント発動する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupTutorialDummyEvent()
{
  ICHI_PRINT(">ICHI SetupTutorialDummyEvent\n");

  if( m_sub.m_isDummyData == true )
  {
    // 一言ウィンドウの色を変えたいので、イベント発動状態にしておく
    m_core.m_eventId = JoinFestaScript::PERSONAL_EVENT_RECENT;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   最後のP2Pやった人用イベントを発動する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetLastP2PPersonalEvent()
{
  m_core.m_eventId = JoinFestaScript::PERSONAL_EVENT_THANKS;
  // メモ：パーソナルイベントのセーブは使わない
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定難易度のパーソナルイベントをセットする
 *
 * @param[in]  personalEventSaveIndex   セーブデータのパーソナルイベント配列のインデックス
 * @param[in]  personalEventType   発動するパーソナルイベント
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetPersonalEvent( const u32 personalEventSaveIndex, const JoinFestaScript::PersonalEventType personalEventType )
{
  //ICHI_PRINT(">ICHI JoinFestaPersonalData::SetPersonalEvent personalEventType %d \n", personalEventType);

  m_core.m_eventId        = static_cast<u8>(personalEventType);
  m_core.m_eventSaveIndex = personalEventSaveIndex;

  // レコード自慢なら、レコードを3つ選ぶ
  if( personalEventType == JoinFestaScript::PERSONAL_EVENT_RECORD_BOAST )
  {
    JoinFestaPersonalEvent::GetHighRecord( this,
      &m_core.m_eventRecordCategory[0], &m_core.m_eventRecordCategory[1], &m_core.m_eventRecordCategory[2],
      &m_core.m_eventRecordIndex[0], &m_core.m_eventRecordIndex[1], &m_core.m_eventRecordIndex[2] );

    //ICHI_PRINT(">ICHI レコード自慢のレコード [%d:%d] [%d:%d] [%d:%d] \n",
      //m_core.m_eventRecordCategory[0], m_core.m_eventRecordIndex[0],
      //m_core.m_eventRecordCategory[1], m_core.m_eventRecordIndex[1],
      //m_core.m_eventRecordCategory[2], m_core.m_eventRecordIndex[2] );
  }
  // レコード勝負なら、レコードを3つ選ぶ
  else if( personalEventType == JoinFestaScript::PERSONAL_EVENT_RECORD_GAME )
  {
    JoinFestaPersonalEvent::GetHighRecord( this,
      &m_core.m_eventRecordCategory[0], &m_core.m_eventRecordCategory[1], &m_core.m_eventRecordCategory[2],
      &m_core.m_eventRecordIndex[0], &m_core.m_eventRecordIndex[1], &m_core.m_eventRecordIndex[2] );

    //ICHI_PRINT(">ICHI レコード勝負のレコード [%d:%d] [%d:%d] [%d:%d] \n",
      //m_core.m_eventRecordCategory[0], m_core.m_eventRecordIndex[0],
      //m_core.m_eventRecordCategory[1], m_core.m_eventRecordIndex[1],
      //m_core.m_eventRecordCategory[2], m_core.m_eventRecordIndex[2] );
  }
  // レコード紹介なら、レコードカテゴリをランダムで決める
  else if( personalEventType == JoinFestaScript::PERSONAL_EVENT_RECORD_INTRODUCTION )
  {
    m_core.m_eventRecordCategory[0] = System::GflUse::GetPublicRand( JoinFestaScript::RECORD_CATEGORY_TOTAL );
    //ICHI_PRINT(">ICHI レコード紹介のレコードカテゴリ %d \n", m_core.m_eventRecordCategory[0] );
  }
  // ズバリ当てろレコードなら、レコードカテゴリをランダムで決める
  else if( personalEventType == JoinFestaScript::PERSONAL_EVENT_RECORD_GUESS )
  {
    m_core.m_eventRecordCategory[0] = System::GflUse::GetPublicRand( JoinFestaScript::RECORD_CATEGORY_TOTAL );
    //ICHI_PRINT(">ICHI ズバリ当てろレコードのレコードカテゴリ %d \n", m_core.m_eventRecordCategory[0] );
  }
  // ズバリ当てろアンケートなら、アンケート番号決める
  else if( personalEventType == JoinFestaScript::PERSONAL_EVENT_QUESTIONNAIRE_GUESS )
  {
    gfl2::util::FixedSizeContainer<u8, 16> questionnaireIdList;
    for( u8 questionId=0; questionId<16; questionId++ )
    {
      // アンケート回答が無回答(=0)、ナイショ(=1)以外であればOK
      if( m_core.m_gameStatus1.questionnaireAnswer[questionId] >= 2 )
      {
        questionnaireIdList.push_back( questionId );
      }
    }
    GFL_ASSERT( questionnaireIdList.size() );
    if( questionnaireIdList.size() == 1 )
    {
      // 1つしかないので固定
      m_core.m_eventQuestionId = questionnaireIdList[ 0 ];
    }
    else if( questionnaireIdList.size() > 1 )
    {
      u32 randomValue = System::GflUse::GetPublicRand( (questionnaireIdList.size()) );
      m_core.m_eventQuestionId = questionnaireIdList[ randomValue ];
    }
  }

  // セーブのパーソナルイベントもセット
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  pSave->SetPersonalEventLotteryOk( m_core.m_eventSaveIndex, this->GetJoinFestaFriendKey() );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントが発動しているかチェックする
 * @return  trueで発動している
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsPersonalEventEnable() const
{
  //ICHI_PRINT(">ICHI JoinFestaPersonalData::IsPersonalEventEnable\n");
  // イベントID有効値
  if( m_core.m_eventId != JoinFestaScript::PERSONAL_EVENT_NULL )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントIDを取得
 * @return  PersonalEventID  JoinFestaScript::PersonalEventType定義
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::PersonalEventType JoinFestaPersonalData::GetPersonalEventId() const
{
  //ICHI_PRINT(">ICHI JoinFestaPersonalData::GetPersonalEventId %d \n", m_core.m_eventId);
  return static_cast<JoinFestaScript::PersonalEventType>(m_core.m_eventId);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントの終了
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::FinishPersonalEvent()
{
  //ICHI_PRINT(">ICHI JoinFestaPersonalData::FinishPersonalEvent \n");
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // イベント発動時のみリセットする
  if( this->IsPersonalEventEnable() )
  {
    // P2P専用イベントは別枠
    if( m_core.m_eventId == JoinFestaScript::PERSONAL_EVENT_THANKS )
    {
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->FinishLastP2pPersonalEvent();
    }

    // セーブのパーソナルイベントも終了
    if( m_sub.m_isPresetData == false )
    {
      pSave->SetPersonalEventFinish( m_core.m_eventSaveIndex );
    }
    else
    {
      // プリセットの場合は別管理
      pSave->FnishPresetNpcEvent( m_sub.m_presetDataId );
    }

    ResetPersonalEvent();
  }

  // @fix MMCat[50]：セーブデータも更新する
  JoinFestaPacketUtil::UpdateJoinFestaPersonalSaveData();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントをリセットする（無効化）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ResetPersonalEvent()
{
  //ICHI_PRINT(">ICHI JoinFestaPersonalData::ResetPersonalEvent \n");
  m_core.m_eventId = JoinFestaScript::PERSONAL_EVENT_NULL;
  m_core.m_eventSaveIndex = 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   レコード勝負のパーソナルイベントのレコードID取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetPersonalEventRecordID1() const
{
  if( this->IsPersonalEventEnable() ) { return m_core.m_eventRecordIndex[0]; }
  return 0;
}
u8 JoinFestaPersonalData::GetPersonalEventRecordID2() const
{
  if( this->IsPersonalEventEnable() ) { return m_core.m_eventRecordIndex[1]; }
  return 0;
}
u8 JoinFestaPersonalData::GetPersonalEventRecordID3() const
{
  if( this->IsPersonalEventEnable() ) { return m_core.m_eventRecordIndex[2]; }
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   レコード紹介のパーソナルイベントのレコードカテゴリ取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetPersonalEventRecordCategory1() const
{
  if( this->IsPersonalEventEnable() ) { return m_core.m_eventRecordCategory[0]; }
  return 0;
}
u8 JoinFestaPersonalData::GetPersonalEventRecordCategory2() const
{
  if( this->IsPersonalEventEnable() ) { return m_core.m_eventRecordCategory[1]; }
  return 0;
}
u8 JoinFestaPersonalData::GetPersonalEventRecordCategory3() const
{
  if( this->IsPersonalEventEnable() ) { return m_core.m_eventRecordCategory[2]; }
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ズバリ当てろアンケートのパーソナルイベントのアンケートID取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetPersonalEventQuestionId() const
{
  if( this->IsPersonalEventEnable() )
  {
    return m_core.m_eventQuestionId;
  }
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   定型文取得
 * @param   patternType   定型文種類
 * @return  msg_jf_phraseのラベルインデックス値
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetPattern( const JoinFestaScript::PatternType patternType ) const
{
  switch( patternType ){
  case JoinFestaScript::PATTERN_HELLO:    return m_core.m_personalInfo.patternHello;
  case JoinFestaScript::PATTERN_PARTING:  return m_core.m_personalInfo.patternParting;
  case JoinFestaScript::PATTERN_EMOTION:  return m_core.m_personalInfo.patternEmotion;
  case JoinFestaScript::PATTERN_REGRET:   return m_core.m_personalInfo.patternRegret;
  case JoinFestaScript::PATTERN_SECRET:   return m_core.m_personalInfo.patternSecret;
  }
  GFL_ASSERT_MSG(0,"%d",patternType);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   一押し施設IDを取得
 * @return  施設ID（全施設のユニークID）
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaPersonalData::GetRecommendFacilitiesId() const
{
  return m_core.m_personalInfo.recommendFacilitiesId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   一押し施設カラーを取得
 * @return  色定義
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaPersonalData::GetRecommendFacilitiesColor() const
{
  return m_core.m_personalInfo.recommendFacilitiesColor;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   一押し施設店員IDを取得
 * @return  店員ID
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaPersonalData::GetRecommendFacilitiesCharacterId() const
{
  return m_core.m_personalInfo.recommendFacilitiesCharacterId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトからのプレイヤーリストで選択されたかどうか設定
 * @param   isSelected (true:選択された  false:選択されてない)
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetScriptListSelected( bool isSelected )
{
  m_core.m_isScriptListSeleted = isSelected;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトからのプレイヤーリストで既に選択されたかどうか
 * @retval  false:選択されてない
 * @retval  true:既に選択された
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsScriptListSelected() const
{
  return m_core.m_isScriptListSeleted;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション開催中か判定する
 * @return  trueで開催中
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsOpenAttraction() const
{
  if( GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
  * @brief   同一アトラクションに参加しているかチェック
  */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsSameAttraction( const PacketAttraction& attractionInfo ) const
{
  if( GetRequestCommand() == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION )
  {
    const PacketAttraction& myAttractionInfo = GetPacketAttraction();
    if( myAttractionInfo.attractionID == attractionInfo.attractionID &&
        myAttractionInfo.ownerTransferdID == attractionInfo.ownerTransferdID )
    {
      if( ::std::memcmp( myAttractionInfo.name , attractionInfo.name , sizeof(attractionInfo.name) ) == 0 )
      {
        if( myAttractionInfo.ticketID == attractionInfo.ticketID )
        {
          return true;
        }
      }
    }
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のデータをリセットする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ResetAttractionData()
{
  m_sub.m_isAttractionAddPoint = false;
  m_sub.m_attractionState   = static_cast<u8>(JoinFestaAttractionDefine::APPEARANCE_STATE_NOT);
  m_sub.m_isDummyData       = false;
  m_sub.m_attractionEventId = JoinFestaScript::PERSONAL_EVENT_NULL;
  m_sub.m_attractionPokeType1 = POKETYPE_NULL;
  m_sub.m_attractionPokeType2 = POKETYPE_NULL;
  m_sub.m_attractionMonsNo    = MONSNO_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中の出現状態をセット
 * @param[in]  attractionState
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetAttractionAppearanceState( JoinFestaAttractionDefine::AppearanceState attractionState )
{
  m_sub.m_attractionState = static_cast<u8>(attractionState);
  ICHI_PRINT(">ICHI SetAttractionAppearanceState %d \n", m_sub.m_attractionState);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中の出現状態を取得
 * @return  attractionState
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaAttractionDefine::AppearanceState JoinFestaPersonalData::GetAttractionAppearanceState() const
{
  ICHI_PRINT(">ICHI GetAttractionAppearanceState %d \n", m_sub.m_attractionState);
  return static_cast<JoinFestaAttractionDefine::AppearanceState>(m_sub.m_attractionState);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のダミーデータをセットアップする
 * @param[in] dummyCount   何体目のダミーデータか
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupAttractionDummyData( const u8 dummyCount )
{
  ICHI_PRINT(">ICHI SetupAttractionDummyData %d \n", dummyCount);
  // ダミーデータフラグ
  m_sub.m_isDummyData = true;

  {
    NetAppLib::JoinFesta::PacketPersonalInfo* personalInf = &m_core.m_personalInfo;

    // 名前は無し
    personalInf->name[0] = L'\0';

    // 性別
    personalInf->sex = PM_MALE;

    // 着せ替え情報（女主人公デフォルト）
    poke_3d::model::DressUpParam dressUpParam;
    ::System::DressUp::GetDressUpParamDefaultHero( &dressUpParam );
    ::System::DressUpParamSaveData::ConvertFromDressUpParam( &personalInf->icon.dressUpParam, dressUpParam ); 

    // 言語コード
    {
      static const u8 languageIdTbl[9] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };
      personalInf->languageId = languageIdTbl[ System::GflUse::GetPublicRand( 9 ) ];
    }

    // アンケート回答ID取得
    const JoinFestaScript::AttractionId attractionId = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetPlayAttractionId();
    // アンケートアトラクションだった場合、回答入れる
    if( JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1 <= attractionId && attractionId <= JoinFestaScript::ATTRACTION_QUESTIONNAIRE_16 )
    {
      // 3人に1人は正解を出す(dummyCountは0オリジンでくる)
      const bool isHit = ( (dummyCount+1)%3 == 0 ) ? true : false; 
      ICHI_PRINT(">ICHI SetupAttractionDummyData IsHit:%d \n", isHit );

      // アンケートアトラクションに対応したアンケートID取得
      const u8 questionnaireId = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetQuestionnaireIdFromAttractionId( attractionId );

      m_core.m_gameStatus1.questionnaireAnswer[questionnaireId] = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetQuestionnaireAnswwerFromDummyData( *this, questionnaireId, isHit );
      m_core.m_gameStatus1.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
    }

    // フレンドキー
    // 何か入れていないとダメ。一言ウィンドウとカイリキーライドアトラクションで使っている。
    {
      m_core.m_personalInfo.myPrincipalId = System::GflUse::GetPublicRand();
      m_core.m_personalInfo.transferdId   = System::GflUse::GetPublicRand();
    }


    // ※↑の情報使うので場所はココ
    // 一言ウィンドウに表示される挨拶は特殊
    personalInf->patternHello     = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternHelloFromDummyData( *this, true );
    // 他の定型分はランダム
    personalInf->patternParting   = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternNotHelloFromDummyData( *this ); // 定型：別れ
    personalInf->patternEmotion   = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternNotHelloFromDummyData( *this ); // 定型：感動
    personalInf->patternRegret    = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternNotHelloFromDummyData( *this ); // 定型：残念
    personalInf->patternSecret    = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternNotHelloFromDummyData( *this ); // 定型：秘密

    SetOnline( false );
    SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MY ); // Myじゃないとダメ。アトラクションイベントで使う
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のダミーデータかどうか
 * @return  trueでダミーデータ
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsAttractionDummyData() const
{
  ICHI_PRINT(">ICHI IsAttractionDummyData %d \n", m_sub.m_isDummyData);
  return m_sub.m_isDummyData;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションの加点対象の人物かどうか設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetAttractionAddPointFlag()
{
  ICHI_PRINT(">ICHI SetAttractionAddPointFlag \n");
  m_sub.m_isAttractionAddPoint = true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションの加点対象の人物かどうか取得
 * @return  trueで加点対象
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsAttractionAddPoint() const
{
  return m_sub.m_isAttractionAddPoint;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のダミーデータの着せ替えをセットアップ
 * @param   dressUpParam 着せ替え情報
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupAttractionDummyDataDressup( const poke_3d::model::DressUpParam& dressUpParam )
{
  // プリセットNPCも着せ替えする
  if( m_sub.m_isDummyData == true || m_sub.m_isPresetData == true )
  {
    // 性別
    m_core.m_personalInfo.sex = dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ];
    // 着せ替え情報
    ::System::DressUpParamSaveData::ConvertFromDressUpParam( &m_core.m_personalInfo.icon.dressUpParam, dressUpParam ); 
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のダミーデータの言語をセットアップ
 * @param   languageId 言語ID
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupAttractionDummyDataLanguageId( const u8 languageId )
{
  if( m_sub.m_isDummyData == true )
  {
    m_core.m_personalInfo.languageId = languageId;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のダミーデータ定型文あいさつを再設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::ResettingAttractionDummyDataPatternHello()
{
  if( m_sub.m_isDummyData == true )
  {
    // 定型文再抽選
    m_core.m_personalInfo.patternHello  = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternHelloFromDummyData( *this, true );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中のに発動したふりをするパーソナルイベントをセットアップする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupAttractionPersonalEventFake( const u8 fakePersonalEvent )
{
  ICHI_PRINT(">ICHI SetupAttractionPersonalEventFake %d \n", fakePersonalEvent);

  // 一応アトラクション以外は設定しないようにする
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
  {
    m_sub.m_attractionEventId = fakePersonalEvent;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション用に指定したパーソナルイベントを取得
 * @return  trueで発動している
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsAttractionPersonalEventEnable() const
{
  // イベントID有効値
  if( m_sub.m_attractionEventId != JoinFestaScript::PERSONAL_EVENT_NULL )
  {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション用に指定したパーソナルイベントを取得
 *
 * @return  personalEventId   パーソナルイベントのID
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetAttractionPersonalEventFake() const
{
  return m_sub.m_attractionEventId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中の一言ウィンドウに出るメッセージを取得
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalData::GetAttractionSingleWordMessage() const
{
  // ダミデータは定型文で設定したもので良い
  if( IsAttractionDummyData() )
  {
    return GetPattern( JoinFestaScript::PATTERN_HELLO );
  }
  // 通信の人の場合は、アトラクションによって変える
  else
  {
    u32 patternHello = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPatternHelloFromDummyData( *this, false );
    if( patternHello != NetAppLib::JoinFesta::JoinFestaAttractionUtil::MSG_ID_NULL )
    {
      return patternHello;
    }
    return GetPattern( JoinFestaScript::PATTERN_HELLO );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ相性アトラクションのポケモンタイプ設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetAttractionPokeType( u8 pokeType1, u8 pokeType2 )
{
  GFL_ASSERT( m_sub.m_isDummyData );
  if( m_sub.m_isDummyData == true )
  {
    m_sub.m_attractionPokeType1 = pokeType1;
    m_sub.m_attractionPokeType2 = pokeType2;
    ICHI_PRINT(">ICHI ポケモンタイプ1 %d タイプ2 %d \n", pokeType1, pokeType2 );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ相性アトラクションのポケモンタイプ取得
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::GetAttractionPokeType( u8& pokeType1, u8& pokeType2 ) const
{
  pokeType1 = m_sub.m_attractionPokeType1;
  pokeType2 = m_sub.m_attractionPokeType2;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ポケモンタイプサーチアトラクションのポケモン設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetAttractionMonsNo( u16 monsNo )
{
  GFL_ASSERT( m_sub.m_isDummyData );
  if( m_sub.m_isDummyData == true )
  {
    m_sub.m_attractionMonsNo = monsNo;
    ICHI_PRINT(">ICHI ポケモン番号 %d \n", monsNo );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ポケモンタイプサーチアトラクションのポケモン取得
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaPersonalData::GetAttractionMonsNo() const
{
  return m_sub.m_attractionMonsNo;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プリセットNPC用のダミーデータをセットアップする
 * @param[in] id    0～11
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupPresetNpcData( const u32 id )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  ICHI_PRINT(">ICHI SetupPresetNpcData %d \n", id );

  // プリセットNPC番号
  m_sub.m_presetDataId = id;

  // プリセットデータフラグ
  m_sub.m_isPresetData = true;

  // 名前はフェスファン固定
  pSave->GetPresetNpcNameString( this->GetSex(), m_core.m_personalInfo.name );


  // フレンドキー
  // 何か入れていないとダメ。一言ウィンドウとカイリキーライドアトラクションで使っている。
  {
    m_core.m_personalInfo.myPrincipalId = System::GflUse::GetPublicRand();
    m_core.m_personalInfo.transferdId   = System::GflUse::GetPublicRand();
  }

  // 定型文
  m_core.m_personalInfo.patternHello     = JoinFestaPersonalDataUtil::GetInitPatternMsg( id, 0 );
  m_core.m_personalInfo.patternParting   = JoinFestaPersonalDataUtil::GetInitPatternMsg( id, 1 );
  m_core.m_personalInfo.patternEmotion   = JoinFestaPersonalDataUtil::GetInitPatternMsg( id, 2 );
  m_core.m_personalInfo.patternRegret    = JoinFestaPersonalDataUtil::GetInitPatternMsg( id, 3 );

  // パーソナルイベント
  {
    JoinFestaPersonalEventDefine::State eventState = static_cast<JoinFestaPersonalEventDefine::State>( pSave->GetPresetNpcEventState( id ) );

    // 未抽選の場合は抽選する。それ以外はセーブの値をそのまま入れる

    if( eventState == JoinFestaPersonalEventDefine::STATE_DISABLE ||
        eventState == JoinFestaPersonalEventDefine::STATE_LOTTERY_NO )
    {
      // イベントはIDにより固定
      switch( id ){
      case 0:
      case 1:
      case 2:
      case 4:
      case 5:
      case 6:
      case 8:
      case 9:
      case 10:
        {
          // お店案内
          u32 randomOffset = System::GflUse::GetPublicRand(9); // お店案内イベントの数
          u8 eventId = static_cast<u8>(JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0) + randomOffset;
          pSave->SetPresetNpcEventId( id, eventId );
        }
        break;

      case 3:
      case 7:
      case 11:
        {
          // アンケート質問
          pSave->SetPresetNpcEventId( id, static_cast<u8>(JoinFestaScript::PERSONAL_EVENT_QUESTIONNAIRE_Q) );
        }
        break;
      }
    }


    // セーブのイベントIDをそのまま入れる
    if( eventState != JoinFestaPersonalEventDefine::STATE_FINISH )
    {
      m_core.m_eventId = pSave->GetPresetNpcEventId( id );
    }
  }


  SetOnline( false );
  SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MY );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プリセットNPC用の着せ替えをセットアップする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalData::SetupPresetNpcDressUp( const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  ICHI_PRINT(">ICHI SetupPresetNpcDressUp %d \n", m_sub.m_presetDataId );

  if( !IsPresetNpcData() )
  {
    return;
  }

  // 着せかえ
  u32 kisekaeId = 0;
  switch( m_sub.m_presetDataId ){
  case 0: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_31; break; // 男
  case 1: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_00; break; // 女
  case 2: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_32; break; // 男
  case 3: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_01; break; // 女
  case 4: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_33; break; // 男
  case 5: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_02; break; // 女
  case 6: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_34; break; // 男
  case 7: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_03; break; // 女
  case 8: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_35; break; // 男
  case 9: kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_04; break; // 女
  case 10:kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_36; break; // 男
  case 11:kisekaeId = NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_05; break; // 女
  }

  // 着せ替えパターンで着せ替えデータ反映
  JoinFestaPersonalDataUtil::SetDressUpPattern( *this, kisekaeId, pDataTable );
  // ↑で性別も確定
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プリセットNPC用データかどうか
 * @return  trueでプリセットNPC用データ
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalData::IsPresetNpcData() const
{
  return m_sub.m_isPresetData;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プリセットNPC用データID取得
 * @return  id
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalData::GetPresetNpcId() const
{
  return m_sub.m_presetDataId;
}

#if PM_DEBUG

/**
 * @brief 全情報を埋める
 */
void JoinFestaPersonalData::DEBUG_SetFullData()
{
  // 定型文ヒミツは入れる
  m_core.m_personalInfo.patternSecret = m_core.m_personalInfo.patternHello;

  // 拡張情報1
  {
    // ランクは3以上に
    m_core.m_gameStatus1.jfRank             = System::GflUse::GetPublicRand(800) + 3;

    // 着せ替えアイテム所持数
    m_core.m_gameStatus1.haveDressUpItemNum = System::GflUse::GetPublicRand(200) + 100;

    m_core.m_gameStatus1.jfCoinCount        = System::GflUse::GetPublicRand(NetApp::JoinFesta::JoinFestaDefine::COIN_MAX);

    // 施設ID
    for( int i=0; i<7; i++ )
    {
      m_core.m_gameStatus1.facilitiesId[i]  = System::GflUse::GetPublicRand(NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_MAX);
    }

    // アンケート回答（必ず答えている）
    m_core.m_gameStatus1.questionnaireAnswer[0]  = System::GflUse::GetPublicRand(4) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[1]  = System::GflUse::GetPublicRand(5) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[2]  = System::GflUse::GetPublicRand(9) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[3]  = System::GflUse::GetPublicRand(5) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[4]  = System::GflUse::GetPublicRand(12) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[5]  = System::GflUse::GetPublicRand(10) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[6]  = System::GflUse::GetPublicRand(4) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[7]  = System::GflUse::GetPublicRand(4) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[8]  = System::GflUse::GetPublicRand(3) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[9]  = System::GflUse::GetPublicRand(8) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[10] = System::GflUse::GetPublicRand(6) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[11] = System::GflUse::GetPublicRand(3) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[12] = System::GflUse::GetPublicRand(7) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[13] = System::GflUse::GetPublicRand(9) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[14] = System::GflUse::GetPublicRand(8) + 1;
    m_core.m_gameStatus1.questionnaireAnswer[15] = System::GflUse::GetPublicRand(7) + 1;

    // バトルフェス用データ
    if(System::GflUse::GetPublicRand(2))
    {
      m_core.m_gameStatus1.battleFesPokeId            = System::GflUse::GetPublicRand(BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX);    // バトルフェスで選んだポケモンのID
    }
    else
    {
      m_core.m_gameStatus1.battleFesPokeId            = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
    }
    m_core.m_gameStatus1.battleFesPokeSex           = System::GflUse::GetPublicRand(BattleFes::BattleFesDefine::POKEMON_SEX_END);           // バトルフェスで選んだポケモンの性別
    m_core.m_gameStatus1.battleFesPokeTokuseiIndex  = System::GflUse::GetPublicRand(BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END);       // バトルフェスで選んだポケモンの特性Index
    m_core.m_gameStatus1.battleFesRank              = System::GflUse::GetPublicRand(BattleFes::BattleFesDefine::BFES_RANK_MAX) + 1;         // バトルフェスのランク
    m_core.m_gameStatus1.ballThrowType              = System::GflUse::GetPublicRand(Savedata::Misc::BALL_THROW_TYPE_NUM);                   // バトルのボール投げの種類(0～7)
    m_core.m_gameStatus1.has_mega_ring              = 1;  // メガリング入手フラグ
    m_core.m_gameStatus1.has_zenryoku_ring          = 1;  // ぜんりょくリング入手フラグ

    m_core.m_gameStatus1.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
  }

  // レコード系
  {
    // 拡張情報2
    m_core.m_gameStatus2.record1    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record2    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record3    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record4    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record5    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record6    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record7    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record8    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record9    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record10   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.record11   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus2.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

    // 拡張情報3
    m_core.m_gameStatus3.record1    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record2    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record3    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record4    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record5    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record6    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record7    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record8    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record9    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record10   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.record11   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus3.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

    // 拡張情報4
    m_core.m_gameStatus4.record1    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record2    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record3    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record4    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record5    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record6    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record7    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record8    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record9    = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record10   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record11   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record12   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record13   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.record14   = System::GflUse::GetPublicRand(999) + 1;
    m_core.m_gameStatus4.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
  }

  // ポケダイアリー系
  {
    GameSys::DeviceDate now;

    Savedata::PokeDiarySave::DIARY_DATE nowDate;
    nowDate.year     = now.GetYear() - 2000;
    nowDate.month    = now.GetMonth();
    nowDate.day      = now.GetDay();
    nowDate.hour     = now.GetHour();
    nowDate.minute   = now.GetMinute();
    nowDate.half_sec = now.GetSecond() / 2;

    for( int i=0; i<3; i++ )
    {
      m_core.m_gameStatus5.pokeDiary[i].diary_id = System::GflUse::GetPublicRand(30) + PDID_COUNT_NULL + 1;
      m_core.m_gameStatus5.pokeDiary[i].data[0] = System::GflUse::GetPublicRand(3) + 1;
      m_core.m_gameStatus5.pokeDiary[i].data[1] = System::GflUse::GetPublicRand(3) + 1;
      m_core.m_gameStatus5.pokeDiary[i].data[2] = System::GflUse::GetPublicRand(3) + 1;
      m_core.m_gameStatus5.pokeDiary[i].date    = nowDate;
    }
    m_core.m_gameStatus5.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;
  }

  // ファインダースタジオ系
  {
    if(!System::GflUse::GetPublicRand(3))
    {
      m_core.m_gameStatus6.photoParam = App::FinderStudio::Static::c_DefPhotoParam;
      m_core.m_gameStatus6.photoParam.packetHeaderSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

      memset(&(m_core.m_gameStatus7.decorationData), 0, sizeof(m_core.m_gameStatus7.decorationData));    /*  all0でも多分大丈夫  */
      {
        m_core.m_gameStatus7.decorationData.deco_param[0].frame_id  = System::GflUse::GetPublicRand(10);
        for(u32 index = 0; index < GFL_NELEMS(m_core.m_gameStatus7.decorationData.deco_param); ++index)
        {
          m_core.m_gameStatus7.decorationData.deco_param[index].id  = System::GflUse::GetPublicRand(50);
          m_core.m_gameStatus7.decorationData.deco_param[index].x   = 32+System::GflUse::GetPublicRand(400-32);
          m_core.m_gameStatus7.decorationData.deco_param[index].y   = 32+System::GflUse::GetPublicRand(240-32);
        }
      }
      m_core.m_gameStatus7.headerSymbol = JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL;

      /*  crc */
      {
        App::FinderStudio::Static::PhotoData  photoData;
        
        photoData.photo_param = m_core.m_gameStatus6.photoParam;
        photoData.deco_param  = m_core.m_gameStatus7.decorationData;

        photoData.photo_param.packetHeaderSymbol = 0;   // crc計算時には通信ヘッダはないので初期化
        
        m_core.m_gameStatus7.decorationData.crc  = App::FinderStudio::Static::FinderStudioUtil::GetPhotoDataCrc( &photoData );
      }

    }
  }

  // 更新日時を現時間に。
  SetOnline( true );

  if(System::GflUse::GetPublicRand(2))
  {
    /*  過去の人にする  */
    --m_core.m_date.year;
  }
}


/**
 * @brief データダンプ
 */
void JoinFestaPersonalData::DEBUG_Dump()
{
  GFL_RELEASE_PRINT("---------------------------------------------\n");
  GFL_RELEASE_PRINT("PersonalDataDump\n");
  GFL_RELEASE_PRINT("-----\n");
  GFL_RELEASE_PRINT("1.パケットヘッダー\n");
  GFL_RELEASE_PRINT("ビーコンバージョン[%d]\n", m_core.m_header.version );
  GFL_RELEASE_PRINT("リクエストコマンド[%d]\n", m_core.m_header.requestCommand );
  GFL_RELEASE_PRINT("タイムスタンプ[%d]\n", m_core.m_header.timestamp );
  GFL_RELEASE_PRINT("-----\n");
  GFL_RELEASE_PRINT("2.個人情報\n");
  gfl2::str::PrintCode( m_core.m_personalInfo.name, "名前：" );
  GFL_RELEASE_PRINT("性別[%d]\n", m_core.m_personalInfo.sex );
  GFL_RELEASE_PRINT("通信拒否フラグ Friend[%d] VIP[%d] Guest[%d]\n", m_core.m_personalInfo.isRejectFriend, m_core.m_personalInfo.isRejectVip, m_core.m_personalInfo.isRejectGuest );
  GFL_RELEASE_PRINT("通信遊びフラグ 対戦[%d] 交換[%d] アトラクション[%d]\n", m_core.m_personalInfo.isAlwaysBattleOk, m_core.m_personalInfo.isAlwaysTradeOk, m_core.m_personalInfo.isAlwaysAttractionOk );
  GFL_RELEASE_PRINT("下位バージョン禁止フラグ[%d]\n", m_core.m_personalInfo.isLowerBattleReject );
  GFL_RELEASE_PRINT("プリンシパルID[%d]\n", m_core.m_personalInfo.myPrincipalId );
  GFL_RELEASE_PRINT("トランスファードID[%llu]\n", m_core.m_personalInfo.transferdId );
  GFL_RELEASE_PRINT("ローカルフレンドコード[%llu]\n", m_core.m_personalInfo.localFriendCode );
  GFL_RELEASE_PRINT("簡易アドレス[%d]\n", m_core.m_personalInfo.addressId );
  GFL_RELEASE_PRINT("定型文　挨拶[%d] 別れ[%d] 感動[%d] 残念[%d] 秘密[%d]\n", m_core.m_personalInfo.patternHello, m_core.m_personalInfo.patternParting, m_core.m_personalInfo.patternEmotion, m_core.m_personalInfo.patternRegret, m_core.m_personalInfo.patternSecret );
  GFL_RELEASE_PRINT("リージョン[%d] ROMコード[%d] 言語コード[%d]\n", m_core.m_personalInfo.regionCode, m_core.m_personalInfo.romCode, m_core.m_personalInfo.languageId );
  GFL_RELEASE_PRINT("一押し施設 ID[%d] カラー[%d] 店員[%d]\n", m_core.m_personalInfo.recommendFacilitiesId, m_core.m_personalInfo.recommendFacilitiesColor, m_core.m_personalInfo.recommendFacilitiesCharacterId );


  GFL_RELEASE_PRINT("-----\n");
  GFL_RELEASE_PRINT("3.拡張情報\n");

  PacketGameStatus1 packet1;
  if( GetPacketGameStatus1( &packet1 ) )
  {
    GFL_RELEASE_PRINT("▼拡張情報1\n");
    GFL_RELEASE_PRINT("フェスコイン累計[%d]\n", packet1.jfCoinCount );
    GFL_RELEASE_PRINT("所持着せ替えアイテム総数[%d]\n", packet1.haveDressUpItemNum );
    GFL_RELEASE_PRINT("フェスランク[%d]\n", packet1.jfRank );
    for( u8 i=0; i<16; i++ ) { GFL_RELEASE_PRINT("アンケート回答[%d:%d]\n", i, packet1.questionnaireAnswer[i] ); }
    for( u8 i=0; i<7; i++ ) { GFL_RELEASE_PRINT("施設ID[%d:%d]\n", i, packet1.facilitiesId[i] ); }
    GFL_RELEASE_PRINT("バトルフェスで選んだポケモンのID[%d]\n", packet1.battleFesPokeId );
    GFL_RELEASE_PRINT("バトルフェスで選んだポケモンの性別[%d]\n", packet1.battleFesPokeSex );
    GFL_RELEASE_PRINT("バトルフェスで選んだポケモンの特性Index[%d]\n", packet1.battleFesPokeTokuseiIndex );
    GFL_RELEASE_PRINT("バトルフェスのランク[%d]\n", packet1.battleFesRank );
    GFL_RELEASE_PRINT("バトルボール投げの種類[%d]\n", packet1.ballThrowType );
    GFL_RELEASE_PRINT("メガリング入手[%d] 全力リング入手[%d]\n", packet1.has_mega_ring, packet1.has_zenryoku_ring );
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報1は受信していない\n");
  }

  PacketGameStatus2 packet2;
  if( GetPacketGameStatus2( &packet2 ) )
  {
    GFL_RELEASE_PRINT("▼拡張情報2\n");
    GFL_RELEASE_PRINT("RECID_COMM_BATTLE[%d]\n", packet2.record1 );
    GFL_RELEASE_PRINT("RECID_WIFI_BATTLE[%d]\n", packet2.record2 );
    GFL_RELEASE_PRINT("RECID_BTREE_CHALLENGE[%d]\n", packet2.record3 );
    GFL_RELEASE_PRINT("RECID_BTL_ENCOUNT[%d]\n", packet2.record4 );
    GFL_RELEASE_PRINT("RECID_WEAK_ATTACK[%d]\n", packet2.record5 );
    GFL_RELEASE_PRINT("RECID_COMM_TRADE[%d]\n", packet2.record6 );
    GFL_RELEASE_PRINT("RECID_MIRACLE_TRADE_COUNT[%d]\n", packet2.record7 );
    GFL_RELEASE_PRINT("RECID_GTS_TRADE_COUNT[%d]\n", packet2.record8 );
    GFL_RELEASE_PRINT("RECID_GTS_PUT[%d]\n", packet2.record9 );
    GFL_RELEASE_PRINT("RECID_DAYMAX_EXP[%d]\n", packet2.record10 );
    GFL_RELEASE_PRINT("RECID_MAME_GET_CNT[%d]\n", packet2.record11 );
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報2は受信していない\n");
  }

  PacketGameStatus3 packet3;
  if( GetPacketGameStatus3( &packet3 ) )
  {
    GFL_RELEASE_PRINT("▼拡張情報3\n");
    GFL_RELEASE_PRINT("RECID_BERRY_HARVEST[%d]\n", packet3.record1 );
    GFL_RELEASE_PRINT("RECID_SHOPPING_MONEY[%d]\n", packet3.record2 );
    GFL_RELEASE_PRINT("RECID_USE_BP[%d]\n", packet3.record3 );
    GFL_RELEASE_PRINT("RECID_CAPTURE_POKE[%d]\n", packet3.record4 );
    GFL_RELEASE_PRINT("RECID_TAMAGO_HATCHING[%d]\n", packet3.record5 );
    GFL_RELEASE_PRINT("RECID_MAME_PRESENT_CNT[%d]\n", packet3.record6 );
    GFL_RELEASE_PRINT("RECID_NICKNAME[%d]\n", packet3.record7 );
    GFL_RELEASE_PRINT("RECID_RIDE_POKE[%d]\n", packet3.record8 );
    GFL_RELEASE_PRINT("RECID_JF_USE_JCOIN[%d]\n", packet3.record9 );
    GFL_RELEASE_PRINT("RECID_ZWAZA_CNT[%d]\n", packet3.record10 );
    GFL_RELEASE_PRINT("RECID_JF_GEST_TALK_CNT[%d]\n", packet3.record11 );
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報3は受信していない\n");
  }

  PacketGameStatus4 packet4;
  if( GetPacketGameStatus4( &packet4 ) )
  {
    GFL_RELEASE_PRINT("▼拡張情報4\n");
    GFL_RELEASE_PRINT("RECID_DENDOU_CNT[%d]\n", packet4.record1 );
    GFL_RELEASE_PRINT("RECID_KAWAIGARI_CNT[%d]\n", packet4.record2 );
    GFL_RELEASE_PRINT("RECID_DAYMAX_CAPTURE[%d]\n", packet4.record3 );
    GFL_RELEASE_PRINT("RECID_DAYMAX_EVOLUTION[%d]\n", packet4.record4 );
    GFL_RELEASE_PRINT("RECID_DAYMAX_KILL[%d]\n", packet4.record5 );
    GFL_RELEASE_PRINT("RECID_DAYMAX_TRAINER_BATTLE[%d]\n", packet4.record6 );
    GFL_RELEASE_PRINT("RECID_JF_ATT_CNT[%d]\n", packet4.record7 );
    GFL_RELEASE_PRINT("RECID_JF_SHOP_GET_CNT[%d]\n", packet4.record8 );
    GFL_RELEASE_PRINT("RECID_RARE_COLOUR_ENC_COUNT[%d]\n", packet4.record9 );
    GFL_RELEASE_PRINT("RECID_IDKUJI_ATARI_CNT[%d]\n", packet4.record10 );
    GFL_RELEASE_PRINT("RECID_POINTUP_USE[%d]\n", packet4.record11 );
    GFL_RELEASE_PRINT("RECID_KISEKAE_CNT[%d]\n", packet4.record12 );
    GFL_RELEASE_PRINT("RECID_QR_SCAN[%d]\n", packet4.record13 );
    GFL_RELEASE_PRINT("RECID_CAMERA_CNT[%d]\n", packet4.record14 );
    GFL_RELEASE_PRINT("RECID_LUNASOL_BESTSCORE[%d]\n", packet4.record15 );
    GFL_RELEASE_PRINT("RECID_MANTAIN_SURF_BEST_BP[%d]\n", packet4.record16 );
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報4は受信していない\n");
  }

  PacketGameStatus5 packet5;
  if( GetPacketGameStatus5( &packet5 ) )
  {
    GFL_RELEASE_PRINT("▼拡張情報5\n");
    for( u8 i=0; i<3; i++ )
    {
      Savedata::PokeDiarySave::DIARY_DATA * dd = &packet5.pokeDiary[i];

      u8 year = dd->date.year;
      u8 month = dd->date.month;
      u8 day = dd->date.day;
      u8 hour = dd->date.hour;
      u8 minute = dd->date.minute;
      u8 half_sec = dd->date.half_sec;
      GFL_RELEASE_PRINT( "%4d/%2d/%2d %2d:%2d:%2d",
        year, month, day, hour, minute, half_sec );
      GFL_RELEASE_PRINT(
        " ID(%3d)TYPE(%d):ROM(%2d):DATA(%3d,%3d,%3d)\n",
        dd->diary_id, dd->diary_type, dd->rom_code,
        dd->data[0], dd->data[1], dd->data[2] );
    }
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報5は受信していない\n");
  }

  if( m_core.m_gameStatus6.photoParam.packetHeaderSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    GFL_RELEASE_PRINT("▼拡張情報6受信済み\n");
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報6は受信していない\n");
  }

  if( m_core.m_gameStatus7.headerSymbol == NetAppLib::JoinFesta::JoinFestaDefine::PACKET_GAME_STATUS_INITIALIZED_SYMBOL )
  {
    GFL_RELEASE_PRINT("▼拡張情報7\n");
    GFL_RELEASE_PRINT("crc[%d]\n", m_core.m_gameStatus7.decorationData.crc );
  }
  else
  {
    GFL_RELEASE_PRINT("▼拡張情報7は受信していない\n");
  }

  {
    App::FinderStudio::Static::PhotoData photoData;
    if( GetPacketPhotoData( &photoData ) )
    {
      GFL_RELEASE_PRINT("FS写真受信済み\n" );
    }
    else
    {
      GFL_RELEASE_PRINT("FS写真未受信\n" );
    }
  }

  GFL_RELEASE_PRINT("-----\n");
  GFL_RELEASE_PRINT("4.その他\n");

  {
    u8 year = m_core.m_date.year;
    u8 month = m_core.m_date.month;
    u8 day = m_core.m_date.day;
    u8 hour = m_core.m_date.hour;
    u8 minute = m_core.m_date.minute;
    u8 half_sec = m_core.m_date.half_sec;
    GFL_RELEASE_PRINT("更新日時 %2d/%2d/%2d %2d:%2d:%2d\n",
    year, month, day, hour, minute, half_sec );
  }

  GFL_RELEASE_PRINT("パーソナルイベントID[%d]\n", m_core.m_eventId );
  GFL_RELEASE_PRINT("パーソナルイベントセーブIndex[%d]\n", m_core.m_eventSaveIndex );
  GFL_RELEASE_PRINT("通信経路[%d]\n", m_core.m_commRoute );
  GFL_RELEASE_PRINT("スクリプトから選ばれたフラグ[%d]\n", m_core.m_isScriptListSeleted );
  GFL_RELEASE_PRINT("関係性[%d]\n", m_sub.m_relation );

}

#endif // PM_DEBUG

}    // JoinFesta
}    // NetAppLib

