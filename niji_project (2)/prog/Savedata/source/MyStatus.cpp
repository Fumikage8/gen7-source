//===================================================================
/**
 * @file    MyStatus.cpp
 * @brief   プレイヤーステータスデータ管理
 * @author  taya
 * @date    2011.4.6
*/
//===================================================================

#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>

#include "../include/MyStatus.h"
#include <System/include/PokemonVersion.h>
#include "System/include/CTR/CTR_Config.h" 

#include "../include/MiscSave.h"  //BALL_THROW_TYPE_NUM

#include "AppLib/include/tool/AppToolTrainerIconRendering.h"

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>

#if defined(GF_PLATFORM_CTR)
#include <nex.h>
#endif // defined(GF_PLATFORM_CTR)

#include <ui/include/gfl2_UI.h>
#include <system/include/HeapDefine.h>
#include "System/include/HeapSize.h"


GFL_NAMESPACE_BEGIN(Savedata)

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
static wchar_t s_countryName[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NUM_LANGUAGES][nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH];
static wchar_t s_regionName[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NUM_LANGUAGES][nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH];
#endif // defined(GF_PLATFORM_CTR)
#endif // PM_DEBUG

MyStatus::MyStatus( void )
{
  // 型のサイズが変わってしまっていないかチェックする
  STATIC_ASSERT(sizeof(m_coreData.name)==26);

  this->Clear( NULL );
}

MyStatus::~MyStatus()
{
}


//=============================================================================================
/**
 * @brief 初期化処理
 *
 * @note  およそ4KBのヒープを一時的に確保する
 *
 * @param heap ヒープ
 */
//=============================================================================================
void MyStatus::Initialize( gfl2::heap::HeapBase* heap )
{
}


//=============================================================================================
/**
 * @brief １日毎に更新を行う処理
 */
//=============================================================================================
void MyStatus::ResetWithDaytime( void )
{
}
/**
 *  コピー
 */
void MyStatus::CopyFrom( const MyStatus& src )
{
  gfl2::std::MemCopy( reinterpret_cast<const void*>(&src.m_coreData), reinterpret_cast<void*>(&m_coreData), sizeof(m_coreData) );
}

/**
 *  名前文字列設定
 */
void MyStatus::SetNameString( gfl2::str::StrBuf* src )
{
  src->PutStr( m_coreData.name, System::STRLEN_PLAYER_NAME + System::EOM_LEN );
}

/**
 *  名前文字列取得
 */
void MyStatus::GetNameString( gfl2::str::StrBuf* dst ) const
{
  dst->SetStr( m_coreData.name );
}

/**
 *  名前文字列取得
 */
void MyStatus::GetNameString( gfl2::str::STRCODE* dst ) const
{
  gfl2::str::StrNCopy(dst, m_coreData.name , System::STRLEN_PLAYER_NAME+System::EOM_LEN );
}

/**
 * @brief ROMコードを取得
 *
 * @retval        u8    ロムコード
 */
u8 MyStatus::GetRomCode( void ) const
{
  return m_coreData.rom_code;
}

/**
 * @brief 国を取得
 *
 * @retval        u8    国コード
 */
u8 MyStatus::GetCountry( void ) const
{
#if defined(GF_PLATFORM_CTR)
  nn::cfg::SimpleAddressId addressId;
  addressId.id = m_coreData.addressId;
  return addressId.GetCountryCode();
#else
#if 0 //@attention DEPEND_ON_OLD_MODULE
  //  コード未実装.
#else
  return 0;
#endif
#endif // defined(GF_PLATFORM_CTR)
}

/**
 * @brief 地域を取得
 *
 * @retval        u8    地域コード
 */
u8 MyStatus::GetArea( void ) const
{
#if defined(GF_PLATFORM_CTR)
  nn::cfg::SimpleAddressId addressId;
  addressId.id = m_coreData.addressId;
  return addressId.GetRegionCode();
#else
#if 0 //@attention DEPEND_ON_OLD_MODULE
  //  コード未実装.
#else
  return 0;
#endif
#endif // defined(GF_PLATFORM_CTR)
}


/**
 * 性別コードを設定
 */
void   MyStatus::SetSex( u8 sex )
{
  m_coreData.sex = sex;
}

/**
 * 性別コードを取得
 */
u8   MyStatus::GetSex( void ) const
{
  return m_coreData.sex;
}

#if defined(GF_PLATFORM_CTR)
//=============================================================================================
/**
 * @brief   簡易アドレス情報IDを取得する
 *
 * @retval  簡易アドレスID
 */
//=============================================================================================
nn::cfg::SimpleAddressId MyStatus::GetSimpleAddressId( void ) const
{
  nn::cfg::SimpleAddressId addressId;
  addressId.id = m_coreData.addressId;
  return addressId;
}
#endif // defined(GF_PLATFORM_CTR)

//=============================================================================================
/**
* @brief アプリ起動時の経度を取得
*
* @note 値の範囲は0x0000-0x7FFF(0°～+179.995°)、0x8000-0xFFFF(-180°～-0.005°)。
* @note 「+(プラス)」は東経、「-(マイナス)」は西経。
*
* @retval        u16    経度
*/
//=============================================================================================
u16 MyStatus::GetLongitude( void ) const
{
  return m_coreData.longitude;
}

//=============================================================================================
/**
* @brief アプリ起動時の緯度を取得
*
* @note 値の範囲は0x0000-0x4000(0°～+90°)、0xC000-0xFFFF(-90°～-0.005°)。
* @note 「+(プラス)」は北緯、「-(マイナス)」は南緯。
*
* @retval        u16    緯度
*/
//=============================================================================================
u16 MyStatus::GetLatitude( void ) const
{
  return m_coreData.latitude;
}

/**
 * リージョンコードを取得
 */
u8   MyStatus::GetRegionCode( void ) const
{
  return m_coreData.regionCode;
}

/**
 * 着せ替え情報をセット
 */
void MyStatus::SetDressup( poke_3d::model::DressUpParam dressUpParam )
{
  System::DressUpParamSaveData::ConvertFromDressUpParam(&m_coreData.icon.dressUpParam, dressUpParam);
}

/**
 * 着せ替え情報を取得
 */
poke_3d::model::DressUpParam MyStatus::GetDressup( void ) const
{
  poke_3d::model::DressUpParam dressUpParam;
  System::DressUpParamSaveData::ConvertToDressUpParam(&dressUpParam, m_coreData.icon.dressUpParam);

  return dressUpParam;
}

//=============================================================================================
/**
  * @brief アイコン情報を設定
  *
  * @retval        ICON_DATA アイコン情報
  */
//=============================================================================================
void MyStatus::SetIcon( const ICON_DATA& rIconData )
{
  m_coreData.icon = rIconData;
}

//=============================================================================================
/**
  * @brief アイコン情報を取得
  *
  * @retval        ICON_DATA アイコン情報
  */
//=============================================================================================
const MyStatus::ICON_DATA&  MyStatus::GetIcon( void ) const
{
  return  m_coreData.icon;
}

/**
 * 言語コードを取得
 */
u8   MyStatus::GetPokeLanguageId( void ) const
{
  return m_coreData.pokeLanguageId;
}

void MyStatus::CreateOwnerInfo( pml::pokepara::OwnerInfo& owner_info ) const
{
  owner_info.trainer_id = m_coreData.ID;
  owner_info.sex = m_coreData.sex;
  gfl2::str::StrNCopy( owner_info.name, m_coreData.name, System::STRLEN_PLAYER_NAME + System::EOM_LEN );
}

/**
 *  自分が親かどうか判定
 */
bool MyStatus::IsMyPokemon( const pml::pokepara::CoreParam& poke ) const
{
  bool result = false;

  bool bFastMode = poke.StartFastMode();

  if( m_coreData.ID != poke.GetID() )
  {
    result = false;
  }
  else
  if( (pml::Sex)(m_coreData.sex) != poke.GetParentSex() )
  {
    result = false;
  }
  else
  {
    gfl2::str::STRCODE   compareBuf[ System::STRLEN_PLAYER_NAME + System::EOM_LEN ];

    poke.GetParentName( (compareBuf), GFL_NELEMS(compareBuf) );
    if( !gfl2::str::StrComp(compareBuf, m_coreData.name) )
    {
      result = false;
    }
    else
    {
      result = true;
    }
  }

  poke.EndFastMode( bFastMode );

  return result;
}

/**
 * IDをセット
 *
 * @param  id セットするID
 */
void   MyStatus::SetID( u32 id )
{
  m_coreData.ID = id;
}

/**
 * IDを取得
 *
 * @retval        u32    ID
 */
u32 MyStatus::GetID( void ) const
{
  return m_coreData.ID;
}

/**
 * IDの下位2バイトを取得
 *
 * @retval        u16    IDの下位2バイト
 */
u16 MyStatus::GetIDLow( void ) const
{
  return m_coreData.ID & 0xFFFF;
}

/**
 * 表示用IDのを取得
 *
 * @retval        u32   IDの下6桁
 */
u32 MyStatus::GetDrawID( void ) const
{
  return ( m_coreData.ID % 1000000 );
}

//=============================================================================================
/**
 * NEXユニークIDをセット
 *
 * @param   NEXユニークid
 */
//=============================================================================================
void MyStatus::SetNexUniqueID( u64 id )
{
  m_coreData.nexUniqueId = id;
}

//=============================================================================================
/**
 * NEXユニークIDを取得
 *
 * @retval        u32    NEXユニークID
 */
//=============================================================================================
u64 MyStatus::GetNexUniqueID( void ) const
{
  return m_coreData.nexUniqueId;
}

//=============================================================================================
/**
 * NEXユニークIDが設定されているか判定
 *
 * @return NEXユニークIDが発行された値がセットされているならtrueを返却
 */
//=============================================================================================
bool MyStatus::IsValidNexUniqueID( void ) const
{
  return (GetNexUniqueID()!=0);
}

//=============================================================================================
/**
 * @brief PssPersonalData格納先データストアIDの設定
 *
 * @return PssPersonalData格納先データストアID
 */
//=============================================================================================
void MyStatus::SetPssPersonalDataStoreId( u64 id )
{
  m_coreData.pssDataStoreId = id;
}

//=============================================================================================
/**
 * @brief PssPersonalData格納先データストアIDの取得
 *
 * @return PssPersonalData格納先データストアID。未設定なら0を返却
 */
//=============================================================================================
u64  MyStatus::GetPssPersonalDataStoreId( void ) const
{
  return m_coreData.pssDataStoreId;
}

//=============================================================================================
/**
* @brief プリンシパルIDを取得
*
* @retval プリンシパルID
*/
//=============================================================================================
u32   MyStatus::GetPrincipalId( void ) const
{
  return m_coreData.principalId;
}

//=============================================================================================
/**
* @brief TransferdIDを取得
*
* @retval TransferdID
*/
//=============================================================================================
u64 MyStatus::GetTransferdID( void ) const
{
  return m_coreData.transferdId;
}

//-----------------------------------------------------------------------------
/**
 *    CTR本体が変更されたかチェックする
 */
//-----------------------------------------------------------------------------
void  MyStatus::CheckChangeCTR( void )
{
#if defined(GF_PLATFORM_CTR)

  u64 transferdId = nn::cfg::CTR::GetTransferableId( GameSys::GameManager::APLICATION_UNIQUE_ID );
  m_isChangedCTR = (transferdId != m_coreData.transferdId);  // 本体の変更が発生！
  m_coreData.transferdId = transferdId;                      // 現在のtransferdIdを書き込み
  m_coreData.principalId = nn::friends::GetMyPrincipalId();  // 現在のプリンシパルIDを書き込み

  // nijiでは必要ないのでコメントアウト
  // 本体が変更された際はCOPPACSの再設定を行う
  //InitCoppacs( m_isChangedCTR );

#endif // defined(GF_PLATFORM_CTR)
}

//-----------------------------------------------------------------------------
//  メガリングを所持しているかチェック
//-----------------------------------------------------------------------------
bool MyStatus::HasMegaRing( void ) const
{
  return m_coreData.has_mega_ring;
}

//-----------------------------------------------------------------------------
//  メガリングの所持状態をチェック
//-----------------------------------------------------------------------------
void MyStatus::SetMegaRing( bool flag )
{
  m_coreData.has_mega_ring = flag;
}

//-----------------------------------------------------------------------------
//  ぜんりょくリングを所持しているかチェック
//-----------------------------------------------------------------------------
bool MyStatus::HasZenryokuRing( void ) const
{
  return m_coreData.has_zenryoku_ring;
}

//-----------------------------------------------------------------------------
//  ぜんりょくリングの所持状態を変更
//-----------------------------------------------------------------------------
void MyStatus::SetZenryokuRing( bool flag )
{
  m_coreData.icon.dressUpParam.hasZRing = flag;
  m_coreData.has_zenryoku_ring = flag;
}

//=============================================================================================
/**
 *    しまめぐりのしるしを所持しているかチェック
 */
//=============================================================================================
bool MyStatus::HasShimameguriNoShirushi( void ) const
{
  return m_coreData.has_shimameguri_no_akashi;
}

//=============================================================================================
/**
 *    しまめぐりのしるしの所持状態を変更
 */
//=============================================================================================
void MyStatus::SetShimameguriNoShirushi( bool flag )
{
  m_coreData.icon.dressUpParam.hasBagCharm = flag;
  m_coreData.has_shimameguri_no_akashi = flag;
}

//=============================================================================================
/**
  *    着せ替えパラメータのROMバージョンNIJIフラグをセット
  *     MMCat[337]対処用に作成
  */
//=============================================================================================
void MyStatus::SetDressUpParamRomVersionNiji()
{
  if( GetRomCode() == VERSION_SUN || GetRomCode() == VERSION_MOON )
  {
    m_coreData.icon.dressUpParam.isRomVersionNiji = true;
  }
}

//=============================================================================================
/**
 *    着せ替えパラメータのROMバージョンNIJIフラグを取得
 *     MMCat[337]対処用に作成
 */
//=============================================================================================
bool MyStatus::IsDressUpParamRomVersionNiji() const
{
  return m_coreData.icon.dressUpParam.isRomVersionNiji;
}

//=============================================================================================
/**
  *    クイックマッチ用情報取得
  */
//=============================================================================================
void MyStatus::GetQuickMatchInfo( bool* isPenalty , bool* isTradeOK , bool* isDoubleOK ) const
{
  if( isPenalty && isTradeOK && isDoubleOK )
  {
    *isPenalty = m_coreData.quick_is_penalty;
    *isTradeOK = m_coreData.quick_trade_ok;
    *isDoubleOK = m_coreData.quick_doubule_ok;
  }
}

//=============================================================================================
/**
  *    クイックマッチ用情報設定
  */
//=============================================================================================
void MyStatus::SetQuickMatchInfo( bool isPenalty , bool isTradeOK , bool isDoubleOK )
{
  m_coreData.quick_is_penalty = isPenalty;
  m_coreData.quick_trade_ok = isTradeOK;
  m_coreData.quick_doubule_ok = isDoubleOK;
}

//=============================================================================================
/**
  *    ボール投げの種類 設定・取得
  */
//=============================================================================================
void MyStatus::SetBallThrowType( u8 type )
{
  if( type<Misc::BALL_THROW_TYPE_NUM )
  {
    m_coreData.ballThrowType = type;
  }
  else
  {
    GFL_ASSERT_MSG(type<Misc::BALL_THROW_TYPE_NUM,"MyStatsu BallThrowType is over![%d]\n",type);
  }
}

u8 MyStatus::GetBallThrowType( void )const
{
  if( m_coreData.ballThrowType<Misc::BALL_THROW_TYPE_NUM )
  {
    return m_coreData.ballThrowType;
  }
  else
  {
    //アッパーを考慮してASSERTは入れない
    //GFL_ASSERT_MSG(m_coreData.ballThrowType<Misc::BALL_THROW_TYPE_NUM,"MyStatsu BallThrowType is over![%d]\n",type);
    return 0;
  }
}

#if PM_DEBUG
/**
 *  自分を指定ポケモンの親として設定する
 */
void  MyStatus::SetPokemonParent( pml::pokepara::CoreParam* poke, gfl2::heap::HeapBase* pTmpHeap ) const
{
  bool bFastMode = poke->StartFastMode();

  gfl2::str::StrBuf tmpBuf( 32, pTmpHeap->GetLowerHandle() );
  poke->SetID( m_coreData.ID );
  tmpBuf.SetStr( m_coreData.name );
  poke->SetParentName( &tmpBuf );
  poke->SetParentSex( static_cast<pml::Sex>(m_coreData.sex) );
  poke->EndFastMode( bFastMode );
}
#endif



#if PM_DEBUG
void MyStatus::SetDebugName( bool is_ctrname )
{


  wchar_t *tempName = L"ニジ\0";
  gfl2::str::StrNCopy( m_coreData.name, tempName, static_cast<size_t>(System::STRLEN_PLAYER_NAME) );

#if defined(GF_PLATFORM_CTR)

  //DS本体名を
  System::CTR::Config  config;
  config.Initialize();
  System::CTR::UserName name;
  config.GetUserName( &name );
  config.Finalize();

  //登録されてなかったらニジにする
  if( name.userName[0] == L'\0' || is_ctrname == false )
  {
    name.userName[0]  = L'ニ';
    name.userName[1]  = L'ジ';
    name.userName[2]  = L'\0';
  }

  ::std::wcsncpy( m_coreData.name, name.userName, static_cast<size_t>(System::STRLEN_PLAYER_NAME) );

  gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();

  if( GetRegionCode() == nn::cfg::CTR::CFG_REGION_JAPAN || GetRegionCode() == nn::cfg::CTR::CFG_REGION_KOREA ||
      GetRegionCode() == nn::cfg::CTR::CFG_REGION_CHINA || GetRegionCode() == nn::cfg::CTR::CFG_REGION_TAIWAN )
  {
    gfl2::ui::Button* pButton      = pDeviceManager->GetButton(0);
    if( pButton->IsHold( gfl2::ui::BUTTON_X ) == false )//X押してたらスキップ
    {//半角英数を全角にする
      for( u32 index = 0 ; index < System::STRLEN_PLAYER_NAME+System::EOM_LEN ; ++index )
      {
        if( m_coreData.name[index] >= L'0' && m_coreData.name[index] <= L'9' )
        {
          m_coreData.name[index] = m_coreData.name[index] - L'0' + L'０';
        }
        else if( m_coreData.name[index] >= L'A' && m_coreData.name[index] <= L'Z' )
        {
          m_coreData.name[index] = m_coreData.name[index] - L'A' + L'Ａ';
        }
        else if( m_coreData.name[index] >= L'a' && m_coreData.name[index] <= L'z' )
        {
          m_coreData.name[index] = m_coreData.name[index] - L'a' + L'ａ';
        }
      }
    }
  }

#endif // defined(GF_PLATFORM_CTR)
}

void MyStatus::RemoveNameEom( void )
{
  gfl2::str::STRCODE * code = m_coreData.name;
  for (int i = 0; i < System::STRLEN_PLAYER_NAME+System::EOM_LEN; ++ i )
  {
    if ( code[i] == gfl2::str::EOM_CODE ) code[i] = 0xff20; // '＠'
  }
}

void MyStatus::SetName13Chars( void )
{
  gfl2::str::STRCODE * code = m_coreData.name;
  for (int i = 0; i < System::STRLEN_PLAYER_NAME+System::EOM_LEN; ++ i )
  {
    code[i] = L'a' + i;
  }
}
#endif //PM_DEUBG

//======================================================================================================================
//======================================================================================================================

/**
 * シリアライズデータのサイズを取得
 *
 * @retval       u32    シリアライズデータのバイトサイズ
 */
u32   MyStatus::GetSerializeDataSize( void )
{
  return SERIALIZE_DATA_SIZE;
}

/**
 *  シリアライズ
 */
u32  MyStatus::Serialize( void* dst ) const
{
  gfl2::std::MemCopy( reinterpret_cast<const void*>(&m_coreData), dst, sizeof(m_coreData) );
  return sizeof(m_coreData);
}

/**
 *  デシリアライズ
 */
void MyStatus::Deserialize( const void* data )
{
  gfl2::std::MemCopy( data, reinterpret_cast<void*>(&m_coreData), sizeof(m_coreData) );
#if defined(GF_PLATFORM_CTR)
  NetLib::Validation::PokemonValidation::ReplacePlayerName( m_coreData.name , m_coreData.pokeLanguageId , m_coreData.rom_code );//不正名置換
#endif
}

//======================================================================================================================
// for SaveData
//======================================================================================================================

//=============================================================================================
/**
 * [for SaveData]  保存するデータバイナリへのポインタを返す
 *
 * @retval        void*
 */
//=============================================================================================
void*  MyStatus::GetData( void )
{
  return reinterpret_cast<void*>(&m_coreData);
}

//=============================================================================================
/**
 * [for SaveData]  保存するデータバイナリのサイズを返す
 *
 * @retval        u32
 */
//=============================================================================================
size_t MyStatus::GetDataSize( void )
{
  return sizeof(m_coreData);
}

//=============================================================================================
/**
 * [for SaveData]  セーブデータを展開する
 *
 * @retval        void*
 */
//=============================================================================================
void MyStatus::SetData(void* pData)
{
  gfl2::std::MemCopy(pData, &m_coreData, sizeof(m_coreData) );
}


//=============================================================================================
/**
 * @brief セーブデータ生成時＆ロード時に毎回情報をセットする
 *
 * @param heap 一時的に利用するヒープ
 */
//=============================================================================================
void MyStatus::SetupBasicInfo( gfl2::heap::HeapBase * heap )
{
  // 緯度経度を初期化
  InitializeGeoParam(heap);

#if defined(GF_PLATFORM_CTR)
  // リージョン
  m_coreData.regionCode = nn::cfg::CTR::GetRegion();
#endif // defined(GF_PLATFORM_CTR)
}

//=============================================================================================
/**
* @brief 緯度・経度・シンプルアドレス情報を初期化する
*
* @note  引数で渡したメモリは一時的なメモリエリアとして利用され、常駐使用はされない
*
* @param heap    ワークメモリ
*/
//=============================================================================================
void MyStatus::InitializeGeoParam( gfl2::heap::HeapBase * heap )
{
#if defined(GF_PLATFORM_CTR)

  if( heap == NULL )
  {
    m_coreData.longitude = 0;
    m_coreData.latitude  = 0;
    m_coreData.addressId = 0;
  }
  else
  {
    nn::cfg::CTR::SimpleAddress* simpleAddress = GFL_NEW_LOW(heap) nn::cfg::CTR::SimpleAddress();

    m_isChangedCTR = false;

    if( simpleAddress != NULL )
    {
      // 緯度/経度情報を取得
      nn::cfg::CTR::GetSimpleAddress( simpleAddress );
      m_coreData.longitude = simpleAddress->longitude;
      m_coreData.latitude  = simpleAddress->latitude;

      // 簡易アドレス(国・地域）
      m_coreData.addressId = simpleAddress->id;

  #if PM_DEBUG
      std::memcpy( s_countryName , simpleAddress->countryName , sizeof(s_countryName) );
      std::memcpy( s_regionName , simpleAddress->regionName , sizeof(s_regionName) );
  #endif // PM_DEBUG
    }
    GFL_SAFE_DELETE( simpleAddress );
  }

#endif // defined(GF_PLATFORM_CTR)
}

//=============================================================================================
/**
 * @brief COPPACSの初期化
 *
 * @param isForceChange 強制更新フラグ
 */
//=============================================================================================
void MyStatus::InitCoppacs( bool isForceChange )
{
#if defined(GF_PLATFORM_CTR)

  bool isRestrict = false;

  if( nn::cfg::CTR::IsCoppacsSupported() )
  {
    nn::cfg::CTR::CfgCoppacsRestriction config = nn::cfg::CTR::GetCoppacsRestriction();
    isRestrict = (config!=nn::cfg::CFG_COPPACS_RESTRICTION_NONE);

    // 前回の設定と異なる
    if( m_coreData.coppacsSetting != static_cast<u8>(config) )
    {
      isForceChange = true;
      m_coreData.coppacsSetting = static_cast<u8>(config);
    }
  }
  else
  {
    m_coreData.coppacsSetting = static_cast<u8>(nn::cfg::CFG_COPPACS_RESTRICTION_NONE);
  }

  // 強制更新、またはCoppacsの制限が無いならフラグ設定
  if( isForceChange || !isRestrict ){
    m_coreData.coppacsRestriction = isRestrict;
  }

#endif // defined(GF_PLATFORM_CTR)

}

//=============================================================================================
/**
 * @brief COPPACS制限有無の取得
 *
 * @return COPPACSの制限が有効ならtrueを返却する
 */
//=============================================================================================
bool MyStatus::IsCoppacsRestriction( void ) const
{
  return m_coreData.coppacsRestriction;
}


//=============================================================================================
/**
 * @brief COPPACSの制限を有効にする
 */
//=============================================================================================
void MyStatus::RestrictCoppacs( void )
{
  m_coreData.coppacsRestriction = true;
}


//=============================================================================================
/**
 * @brief COPPACSの制限を解除する
 */
//=============================================================================================
void MyStatus::PermitCoppacs( void )
{
  m_coreData.coppacsRestriction = false;
}

//=============================================================================================
/**
 * 前回セーブ時からCTR本体が変更されたかを判定
 *
 * @return 本体が変更されたならtrueを返却
 */
//=============================================================================================
bool MyStatus::IsChangedCTR( void ) const
{
  return m_isChangedCTR;
}

//=============================================================================================
/**
* [for SaveData]  セーブデータを初期化する
*
* @note  引数で渡したメモリは一時的なメモリエリアとして利用され、常駐使用はされない
*
* @param heap    ワークメモリ
*/
//=============================================================================================
void MyStatus::Clear( gfl2::heap::HeapBase * heap )
{
  ::std::memset( &m_coreData, 0, sizeof(m_coreData) );
  m_isChangedCTR = false;

  // 暫定値(新規ゲーム開始時にセットされる）
  m_coreData.ID                = 0;

  // 予約情報

#if defined(GF_PLATFORM_CTR)
  // 本体情報
  m_coreData.firstTransferdId = nn::cfg::CTR::GetTransferableId( GameSys::GameManager::APLICATION_UNIQUE_ID );
  m_coreData.transferdId      = m_coreData.firstTransferdId;
  m_coreData.principalId      = nn::friends::GetMyPrincipalId();
  m_coreData.nexUniqueId      = 0;
  m_coreData.pssDataStoreId   = nn::nex::DataStoreConstants::INVALID_DATAID;
#endif // defined(GF_PLATFORM_CTR)

  // 基本情報1
  SetupBasicInfo(heap);

  // ROMコード
  m_coreData.rom_code = GET_VERSION();

  // 言語コード
  m_coreData.pokeLanguageId = System::GetLang();

  // 性別
  m_coreData.sex = PM_FEMALE;

  // デフォルトは名前なしに設定
  ::std::memset( m_coreData.name,     0, sizeof(m_coreData.name) );

  //  アイコンデータのデフォルト作成
  app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( m_coreData.sex, &m_coreData.icon );

  // 着せ替えのデフォルト
  poke_3d::model::DressUpParam dressUpParam;
  System::DressUp::GetDressUpParamDefault(m_coreData.sex, &dressUpParam);
  m_coreData.icon.bDressDefault = 0;

  SetDressup(dressUpParam);

  // COPPA制限
  InitCoppacs(true);

}

GFL_NAMESPACE_END(Savedata)

