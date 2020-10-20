//===================================================================
/**
 * @file    SaveData.cpp
 * @brief   セーブデータのメモリ確保クラス
 * @author  k.ohno
 * @date    11.12.3
 * @note    セーブデータをメモリに常駐するためのクラス
 */
//===================================================================

//#include <gflib.h>
//#include "util/include/gfl2_std_string.h"
#include "debug/include/gfl2_DebugPrint.h"
#include "math/include/gfl2_math_crc.h"

#include "Savedata/include/SaveData.h"

#include <NetStatic/NetAppLib/include/QR/QRUtility.h>
#include <crypto/include/gfl2_ISHA256Calculator.h>
#include "System/include/ThreadPriority.h"
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"


GFL_NAMESPACE_BEGIN(Savedata)

#if PM_DEBUG
//#define SECURE_VALUE_TEST
//#define SV_MODULE_CRC_CHECK //これを有効にすると各セーブオブジェクトのＣＲＣ不一致をチェックできます
#endif

//-----------------------------------------------------------------------------
/**
 * @brief 整合性管理領域
 */
//-----------------------------------------------------------------------------
class Footer : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(Footer);

  private:
  //----------------------------------------------------------
  //----------------------------------------------------------
  static const u32 MAGIC_NUMBER = 'BEEF';

  enum {
    /// 0の場合、本体側がどんな値でもOKになる
    SECURE_VALUE_ALL_OK = 0,

    /// 初期値は何でもいいが0を避ける
    SECURE_VALUE_INITIALIZER = 1,
  };
  //----------------------------------------------------------
  //----------------------------------------------------------
  struct CHECK_DATA{
      u32 size;
      u16 id;
      u16 crc;
  };

  //----------------------------------------------------------
  //----------------------------------------------------------
  struct TOTAL_CHECK_DATA {
    bit64 last_secure_value;  ///<現在のSecureValue
    bit64 prev_secure_value;  ///<ひとつ前のSecureValue
    u32 magic_number;
    CHECK_DATA mCheckData[ SaveData::SAVEDATA_CLASS_NUM ];
  };

  TOTAL_CHECK_DATA mRawData;
  bit64 trash_secure_value;  ///<捨てる予定のSecureValueを保持しておく領域

  public:
  Footer()
    : trash_secure_value(0)
  { 
    Clear(NULL); 
  }
  virtual ~Footer(){}

  virtual void SetData(void* pData)
  {
    gfl2::std::MemCopy( pData, &mRawData, sizeof(TOTAL_CHECK_DATA) );
  }

  virtual void* GetData(void) { return &mRawData; }
  virtual size_t GetDataSize(void) { return sizeof(TOTAL_CHECK_DATA); }

  virtual void Clear( gfl2::heap::HeapBase * heap )
  {
    gfl2::std::MemClear( &mRawData, sizeof(TOTAL_CHECK_DATA) );
    mRawData.last_secure_value = SECURE_VALUE_INITIALIZER;
    mRawData.prev_secure_value = SECURE_VALUE_INITIALIZER;
  }

  //----------------------------------------------------------
  //----------------------------------------------------------
  ///空っぽのデータかどうか
  bool IsNullData( void ) const
  {
    if ( mRawData.last_secure_value != SECURE_VALUE_INITIALIZER ) return false;
    if ( mRawData.prev_secure_value != SECURE_VALUE_INITIALIZER ) return false;
    if ( IsNullMagicNumber() == false ) return false;

    for (int index = 0; index < SaveData::SAVEDATA_CLASS_NUM; ++ index )
    {
      if ( mRawData.mCheckData[index].size != 0 ) return false;
      if ( mRawData.mCheckData[index].id   != 0 ) return false;
      if ( mRawData.mCheckData[index].crc  != 0 ) return false;
    }
    return true;
  }

  //----------------------------------------------------------
  //----------------------------------------------------------
  ///正しいデータかどうか
  bool CompareCheckData( u32 index, u32 size, u16 id, u16 crc ) const
  {
    GFL_ASSERT_STOP( index < SaveData::SAVEDATA_CLASS_NUM );
    GFL_PRINT("%02d:calc:rec =%04x:%04x\n",
        index, mRawData.mCheckData[index].crc, crc );
    if ( mRawData.mCheckData[index].size != size ) return false;
    if ( mRawData.mCheckData[index].id   != id ) return false;
    if ( mRawData.mCheckData[index].crc  != crc ) return false;
    return true;
  }

  void SetCheckData( u32 index, u32 size, u16 id, u16 crc )
  {
    GFL_ASSERT_STOP( index < SaveData::SAVEDATA_CLASS_NUM );
    mRawData.mCheckData[index].size = size;
    mRawData.mCheckData[index].id   = id;
    mRawData.mCheckData[index].crc  = crc;
  }

  void SetMagicNumber( void )
  {
    mRawData.magic_number = MAGIC_NUMBER;
  }

  bool IsValidMagicNumber( void ) const
  {
    GFL_PRINT("MAGIC_NUMBER = %08x\n", mRawData.magic_number);
    return ( mRawData.magic_number == MAGIC_NUMBER );
  }

  bool IsNullMagicNumber( void ) const
  {
    return ( mRawData.magic_number == 0 );
  }

  bit64 GetSecureValue( void ) const
  {
    return mRawData.last_secure_value;
  }

  bit64 GetPreviousSecureValue( void ) const
  {
    return mRawData.prev_secure_value;
  }

  void dumpSecureValue( void ) const
  {
    GFL_PRINT("SecureValue:now    %lx\n", mRawData.last_secure_value );
    GFL_PRINT("SecureValue:prev   %lx\n", mRawData.prev_secure_value );
    GFL_PRINT("SecureValue:trash  %lx\n", trash_secure_value );
#if 0 //@attention DEPEND_ON_OLD_MODULE
    xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_POKE_SAVE,
        L"SecureValue:now    %08x %08x\n",
        mRawData.last_secure_value >> 32,
        static_cast<u32>(mRawData.last_secure_value & 0xffffffff) );
    xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_POKE_SAVE,
        L"SecureValue:prev   %08x %08x\n",
        mRawData.prev_secure_value >> 32,
        static_cast<u32>(mRawData.prev_secure_value & 0xffffffff) );
    xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_POKE_SAVE,
        L"SecureValue:trash  %08x %08x\n",
        trash_secure_value >> 32,
        static_cast<u32>(trash_secure_value & 0xffffffff) );
#endif
  }

  bit64 RenewSecureValue( void )
  {
#if 0 //@attention DEPEND_ON_OLD_MODULE
    TAMADA_PRINT(":::SecureValue::Renew\n");
    xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_POKE_SAVE,
        L"SecureValue::Renew\n");
#endif
    dumpSecureValue();
    bit64 value = 0;
    //次のSecureValueを乱数で生成する
    do {
#ifdef  SECURE_VALUE_TEST
      nn::crypto::GenerateRandomBytes( &value, sizeof(value) );
      switch ( xy::GetLang() )
      {
      case LANG_JAPAN:
        // 上位：クリア　下位：乱数
        value &= 0xffffffff;
        break;
      case LANG_ENGLISH:
        // 上位：インクリメント　下位：乱数
        value &= 0xffffffff;
        value |= ((mRawData.last_secure_value >> 32) + 1 ) << 32;
        break;
      case LANG_FRANCE:
        // 上位：乱数   下位：クリア
        value = ( ((value >> 32) & 0xffffffff) << 32 );
        break;
      case LANG_GERMANY:
        // 上位：乱数   下位：インクリメント
        value = ( ((value >> 32) & 0xffffffff) << 32 );
        value |= (mRawData.last_secure_value ) + 1 ;
        break;
      case LANG_ITALY:
        //上位＆下位　同じ乱数
        value &= 0xffffffff;
        value |= (value << 32);
        break;
      case LANG_SPAIN:
        //上位：乱数　下位：前回と一緒
        value = ( ((value >> 32) & 0xffffffff) << 32 );
        value |= (mRawData.last_secure_value >> 32 );
        break;
      default:
        /* do nothing */
        break;
      }
#else
#if defined(GF_PLATFORM_CTR)
      nn::crypto::GenerateRandomBytes( &value, sizeof(value) );
#endif
      // 上位：インクリメント　下位：乱数
      value &= 0xffffffff;
      value |= ((mRawData.last_secure_value >> 32) + 1 ) << 32;
#endif
    }while ( value == SECURE_VALUE_ALL_OK
        || value == mRawData.last_secure_value
        || value == mRawData.prev_secure_value );

    trash_secure_value          = mRawData.prev_secure_value;
    mRawData.prev_secure_value  = mRawData.last_secure_value;
    mRawData.last_secure_value  = value;

    dumpSecureValue();

    return value;
  }

  void CancelSecureValue( void )
  {
#if 0 //@attention DEPEND_ON_OLD_MODULE
    TAMADA_PRINT(":::SecureValue::Cancel\n");
    xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_POKE_SAVE,
        L"SecureValue::Cancel\n");
#endif
    dumpSecureValue();
    mRawData.last_secure_value  = mRawData.prev_secure_value;
    mRawData.prev_secure_value  = trash_secure_value;
    trash_secure_value          = SECURE_VALUE_ALL_OK;
    dumpSecureValue();
  }
};


//-----------------------------------------------------------------------------
/**
 *  @brief    コンストラクタ
 */
//-----------------------------------------------------------------------------
SaveData::SaveData( gfl2::heap::HeapBase * heap ) :
  mFooter(NULL)
{
  mFooter = GFL_NEW( heap ) Footer();

  /// @caution ここに追加したセーブデータを下に追加する形で記述
  /// セーブデータはこの順序で記録されることになる。メンバの定義順ではない。
  int i = 0;
  /// 並びにも意味があるという事なのでオリジナルの番号を残す.
  mSaveObject[i++] = &mMyItem;                ///< 1
  mSaveObject[i++] = &mSituation;             ///< 2
  mSaveObject[i++] = &mRandomGroup;           ///< 3
  mSaveObject[i++] = &mMyStatus;              ///< 4
  mSaveObject[i++] = &mPokePartySave;         ///< 5
  mSaveObject[i++] = &mEventWork;             ///< 6
  mSaveObject[i++] = &mZukanData;             ///< 7
  mSaveObject[i++] = &mGtsData;               ///< 8
  mSaveObject[i++] = &mUnionPokemon;          ///< 9
  mSaveObject[i++] = &mMisc;                  ///< 10
  mSaveObject[i++] = &mFieldMenu;             ///< 11
  mSaveObject[i++] = &mConfig;                ///< 12
  mSaveObject[i++] = &mGameTime;              ///< 13
  mSaveObject[i++] = &mBox;                   ///< 14
  mSaveObject[i++] = &mBoxPokemon;            ///< 15
  mSaveObject[i++] = &mResort;                ///< 16
  mSaveObject[i++] = &mPlayTime;              ///< 17
  mSaveObject[i++] = &mFieldMoveModelSave;    ///< 18
  mSaveObject[i++] = &mFashion;               ///< 19
  mSaveObject[i++] = &mJoinFestaVip;          ///< 20
  mSaveObject[i++] = &mJoinFestaGuest;        ///< 21
  mSaveObject[i++] = &mJoinFestaData;         ///< 22
  mSaveObject[i++] = &mBerrySpot;             ///< 23
  mSaveObject[i++] = &mFishingSpot;           ///< 24
  mSaveObject[i++] = &mLiveMatchData;         ///< 25
  mSaveObject[i++] = &mBattleSpotData;        ///< 26
  mSaveObject[i++] = &mPokeFinder;            ///< 27
  mSaveObject[i++] = &mMysteryGiftSave;       ///< 28
  mSaveObject[i++] = &mRecord;                ///< 29
  mSaveObject[i++] = &mValidation;            ///< 30
  mSaveObject[i++] = &mGameSync;              ///< 31
  mSaveObject[i++] = &mPokeDiarySaveData;     ///< 32
  mSaveObject[i++] = &mBattleInstSave;        ///< 33
  mSaveObject[i++] = &mSodateya;              ///< 34
  mSaveObject[i++] = &mWeather;               ///< 35
  mSaveObject[i++] = &mQRReader;              ///< 36
  mSaveObject[i++] = &mTurtleSalmonSave;      ///< 37
  mSaveObject[i++] = &mBattleFesSave;         ///< 38
  mSaveObject[i++] = &mFinderStudioSave;      ///< 39

  // MyStatusの初期化
  mMyStatus.Initialize( heap );

  GFL_ASSERT_STOP( i == SAVEDATA_CLASS_NUM );

  this->Clear(heap);
/*
  for ( int i = 0; i < SAVEDATA_CLASS_NUM; i ++ )
  {
    GFL_PRINT( "savedatasizecheck[%02d][%d]\n" ,i, mSaveObject[i]->GetDataSize() );
  }
  */
}

//-----------------------------------------------------------------------------
/**
 */
//-----------------------------------------------------------------------------
int SaveData::GetSaveObjectNum(void) const
{
  // 各セーブデータ ＋ 整合性データ（Footer)
  return SAVEDATA_CLASS_NUM + 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SaveDataInterface * SaveData::GetSaveObject( u32 class_id )
{
  if ( class_id == SAVEDATA_CLASS_NUM ) return mFooter;
  GFL_ASSERT_STOP( class_id < SAVEDATA_CLASS_NUM );
  return mSaveObject[class_id];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
size_t SaveData::GetAlignmentSize( void ) const
{
  return 512;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    バックアップデータのクリア
 *  @param heap  クリア処理に使用するテンポラリヒープ
 */
//-----------------------------------------------------------------------------
void SaveData::Clear( gfl2::heap::HeapBase * heap )
{

  for ( int i = 0; i < SAVEDATA_CLASS_NUM; i ++ )
  {
    mSaveObject[i]->Clear( heap );
#if PM_DEBUG
#ifdef SV_MODULE_CRC_CHECK
    u16 before, after;
    int size = mSaveObject[i]->GetDataSize();
    before = gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(mSaveObject[i]->GetData()), size );
    u8 * row_data = reinterpret_cast<u8*>(mSaveObject[i]->GetData());
    gfl2::std::MemFill( row_data, 0xff, size );
    mSaveObject[i]->Clear( heap );
    after = gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(mSaveObject[i]->GetData()), size );
    GFL_PRINT("SV CRC CHECK [%d] (before:%d, after:%d)", i, before, after);
    if ( before != after )
    {
      GFL_PRINT("   NOT_EQUAL\n");
    }
    else GFL_PRINT("\n");
#endif //SV_MODULE_CRC_CHECK
#endif
  }

  mFooter->Clear( heap );
}

//-----------------------------------------------------------------------------
/**
 * @brief   状態を取得する
 * @retval  STATE_NULL    データが存在しない
 * @retval  STATE_OK      正常
 * @retval  STATE_NG      異常、使用できない
 */
//-----------------------------------------------------------------------------
ISaveDataTotal::State SaveData::GetStatus( s32 * error_info ) const
{
  if (this->IsValidCheckData(error_info) == true )  return STATE_OK;
  if (this->IsNullData() == true )        return STATE_NULL;
  return STATE_NG;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    バックアップデータの整合性チェック
 */
//-----------------------------------------------------------------------------
bool SaveData::IsValidCheckData( s32 * error_info ) const
{
  if (error_info) *error_info = -1;

  if ( !mFooter->IsValidMagicNumber() ) return false;
  
  u8 signature[128];
  
  for ( int i = 0; i < SAVEDATA_CLASS_NUM; i ++ )
  {
    if (error_info) *error_info = i;
    u32 size = mSaveObject[i]->GetDataSize();

    if( i == TURTLESALMON_SAVE_INDEX )
    {
      u8* pSave = reinterpret_cast<u8*>( mSaveObject[TURTLESALMON_SAVE_INDEX]->GetData() );
      ::std::memcpy( signature , &pSave[256] , sizeof(signature) );
      ::std::memset( &pSave[256] , 0 , sizeof(signature) );
    }

    u16 crc = gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(mSaveObject[i]->GetData()), size );

    if( i == TURTLESALMON_SAVE_INDEX )
    {
      u8* pSave = reinterpret_cast<u8*>( mSaveObject[TURTLESALMON_SAVE_INDEX]->GetData() );
      ::std::memcpy( &pSave[256] , signature , sizeof(signature) );
    }

    if ( mFooter->CompareCheckData( i, size, i, crc ) == false ) return false;
  }

#if defined(GF_PLATFORM_WIN32)
  return true;
#endif

  gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  //niji署名
  u16 message[64];
  if( NetApp::QR::QRUtility::VerifyBinary( signature , sizeof(signature) , message , eventDeviceHeap ) == false )
  {
    GFL_ASSERT( 0 );//@fix
    return false;
  }

  u32 size = mFooter->GetDataSize();
  u8 value[32];

  gfl2::crypto::ISHA256Calculator* pSHA256Calculator = gfl2::crypto::ISHA256Calculator::CreateSHA256Calculator( eventDeviceHeap, 256, System::ThreadPriority::ASYNC_SHA256_CRYPTO_THREAD_PRIORITY );

  gfl2::crypto::SHA256Hash* pSHA256Hash = pSHA256Calculator->CalcHash( reinterpret_cast<const u8*>(mFooter->GetData()), size );
  memcpy( value, pSHA256Hash->GetData(), sizeof(value) );

  GFL_SAFE_DELETE( pSHA256Calculator );

  if( ::std::memcmp( value , message , sizeof(value) ) != 0 )
  {
    GFL_ASSERT( 0 );//@fix
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bit64 SaveData::GetSecureValue( void ) const
{
  return mFooter->GetSecureValue();
}

bit64 SaveData::RenewSecureValue( void )
{
  return mFooter->RenewSecureValue();
}

bit64 SaveData::GetPreviousSecureValue( void ) const
{
  return mFooter->GetPreviousSecureValue();
}

void SaveData::CancelSecureValue( void )
{
  mFooter->CancelSecureValue();
}

//-----------------------------------------------------------------------------
/**
 *  @brief    バックアップデータの存在チェック
 */
//-----------------------------------------------------------------------------
bool SaveData::IsNullData( void ) const
{
  //フッタがクリアされていればバックアップもクリアされているとみなす
  return mFooter->IsNullData();
}

//-----------------------------------------------------------------------------
/**
 *  @brief    バックアップデータの整合性情報埋め込み
 */
//-----------------------------------------------------------------------------
void SaveData::SetupCheckData( void )
{
  u8 signature[128];
  ::std::memset( signature , 0 , sizeof(signature) );

  for ( int i = 0; i < SAVEDATA_CLASS_NUM; i ++)
  {
    u32 size = mSaveObject[i]->GetDataSize();
    if( i == TURTLESALMON_SAVE_INDEX )
    {
      u8* pSave = reinterpret_cast<u8*>( mSaveObject[TURTLESALMON_SAVE_INDEX]->GetData() );
      ::std::memset( &pSave[256] , 0 , sizeof(signature) );
    }

    u16 crc = gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(mSaveObject[i]->GetData()), size );
    mFooter->SetCheckData( i, size, i, crc );
  }
  mFooter->SetMagicNumber();

#if defined(GF_PLATFORM_WIN32)
  return;
#endif

  //niij署名
  u32 size = mFooter->GetDataSize();


  u8 value[32];
  gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  gfl2::crypto::ISHA256Calculator* pSHA256Calculator = gfl2::crypto::ISHA256Calculator::CreateSHA256Calculator( eventDeviceHeap, 256, System::ThreadPriority::ASYNC_SHA256_CRYPTO_THREAD_PRIORITY );

  gfl2::crypto::SHA256Hash* pSHA256Hash = pSHA256Calculator->CalcHash( reinterpret_cast<const u8*>(mFooter->GetData()), size );
  memcpy( value, pSHA256Hash->GetData(), sizeof(value) );

  GFL_SAFE_DELETE( pSHA256Calculator );
  
  u16 message[64];
  ::std::memset( message , 0 , sizeof(message) );
  ::std::memcpy( message , value , sizeof( value ) );
  
  u8* pSave = reinterpret_cast<u8*>( mSaveObject[TURTLESALMON_SAVE_INDEX]->GetData() );
  NetApp::QR::QRUtility::SignBinary( &pSave[256] , message , 120 , eventDeviceHeap );
}
//-----------------------------------------------------------------------------
/**
 *  @brief    バックアップデータの無効情報埋め込み
 */
//-----------------------------------------------------------------------------
void SaveData::SetupNullData( void )
{
  mFooter->Clear( NULL );
}

//-----------------------------------------------------------------------------
/**
 * @brief     セーブデータ書き換え禁止
 * @attention 本来は各セーブデータの詳細は知るべきではないが各インターフェイスに追加する作業量と
 * @attention SaveLoadControl/GameDataレベルで対応するよりもましなのでここに置く
 */
//-----------------------------------------------------------------------------
void SaveData::LockData( void )
{
  this->GetRecordSaveData()->SetDisableRecord( true );
}

//-----------------------------------------------------------------------------
/**
 * @brief     セーブデータ書き換え禁止を解除
 * @attention 本来は各セーブデータの詳細は知るべきではないが各インターフェイスに追加する作業量と
 * @attention SaveLoadControl/GameDataレベルで対応するよりもましなのでここに置く
 */
//-----------------------------------------------------------------------------
void SaveData::UnlockData( void )
{
  this->GetRecordSaveData()->SetDisableRecord( false );
}




GFL_NAMESPACE_END(Savedata)
