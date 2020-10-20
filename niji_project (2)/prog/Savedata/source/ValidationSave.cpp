//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file    ValidationSave.cpp
 *  @brief   不正チェックサーバ公開鍵用セーブデータ
 *  @author  hisanso suzuki
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "Savedata/include/ValidationSave.h"

#include "System/include/HeapDefine.h"

#include <debug/include/gfl2_DebugPrint.h>
#include  <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(Savedata)

// 公開キー
const char* ValidationSave::PUBLIC_CER = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAx1w+Sk7S7BNGT/9Pj2n9RHStI9orP7Y+FDiX/KkbdDjFdvT2HCTw9XnW0DHj9uQTDHQz0tX0F9n+hsdynFcz0bvwi0oJcFcDhIO62dCtTjq6jkvU/xb8egrBUByjjBrSp5ZWFLSGt8OLgqUZQt4tEBUPXTpbYkCCtJgV0MDuNZOe0J0y9lPhbtbRryjSkWCIZw/GADlTsP3YLTiVnbarSjxaI1RSGzbPNky7rLeeBU6GQuACKpYvqyxnJci9pFbSOcY5pdKfl1MHcBOrxvwwdq5E5w+Rm/umfADv8fpXYbUNOigCVuU8nV4amPqZdP7jRyA+T7e6tymSy/KKZTs/awIDAQAB";


//=============================================================================
//@brief コンストラクタ・デストラクタ
//=============================================================================
/**
 * @brief コンストラクタ
 */
ValidationSave::ValidationSave() : SaveDataInterface()
{
  this->Clear(NULL);
}

/**
 * @brief デストラクタ
 */
ValidationSave::~ValidationSave()
{

}



//--------------------------------------------------------------------------------------------
/**
 * @brief 公開鍵のバージョンを取得
 *
 * @return 公開鍵のバージョン番号
 */
//--------------------------------------------------------------------------------------------
u16 ValidationSave::GetVersion(void) const
{
  return m_data.m_version;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 公開鍵のサイズを取得
 *
 * @return 公開鍵のサイズ
 */
//--------------------------------------------------------------------------------------------
size_t ValidationSave::GetPublicKeySize(void) const
{
  return m_data.m_publicKeySize;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 公開鍵を取得
 *
 * @return 公開鍵
 */
//--------------------------------------------------------------------------------------------
const void* ValidationSave::GetPublicKey(void) const
{
  return m_data.m_publicKey;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 公開鍵をサーバから取得した情報で更新する
 *
 * @param version       バージョン情報
 * @param publicKey     公開鍵
 * @param publicKeySize 公開鍵のサイズ
 */
//--------------------------------------------------------------------------------------------
void ValidationSave::UpdatePublicKey( u16 version, const u8* publicKey, u32 publicKeySize )
{
#if defined(GF_PLATFORM_CTR)
  GFL_ASSERT( publicKeySize <= PUBLIC_KEY_MAX_SIZE ); // @fix

  if( publicKeySize <= PUBLIC_KEY_MAX_SIZE )
  {
    m_data.m_version       = version;
    m_data.m_publicKeySize = publicKeySize;

    gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );

    char* base64CER = (char*)GFL_NEW_ARRAY( heap ) char[publicKeySize+1];

    ::std::memcpy( base64CER, publicKey, publicKeySize );
    base64CER[publicKeySize] = '\0';

    // 複合化用公開鍵を取得
    nn::Result cerRet = nn::util::Base64::FromBase64String( base64CER, m_data.m_publicKey, sizeof(m_data.m_publicKey), &m_data.m_publicKeySize );
    GFL_ASSERT( cerRet.IsSuccess() ); // @check

    GFL_DELETE_ARRAY( base64CER );
  }
#endif
}


//=============================================================================
//@brief SaveDataInterface実装
//=============================================================================
/**
 *  @brief    読み込んだデータをセットする関数
  * @param    pData   先頭のポインタ
 */
void ValidationSave::SetData(void* data)
{
  gfl2::std::MemCopy(data, &m_data, this->GetDataSize() );
}

/**
 *  @brief    データの先頭ポインタを得る関数
  * @return   先頭のポインタ
 */
void* ValidationSave::GetData(void)
{
  return reinterpret_cast<void*>(&m_data);
}

/**
 *  @brief    データサイズ
 *  @retval   バイト
 */
size_t ValidationSave::GetDataSize(void)
{
  return sizeof( CoreData );
}

/**
 * @brief セーブデータのクリア
 * @param heap　クリアに使用するテンポラリヒープ
 */
void ValidationSave::Clear( gfl2::heap::HeapBase * /*heap*/ )
{
  gfl2::std::MemClear( &m_data, this->GetDataSize() );
#if defined(GF_PLATFORM_CTR)
  // 複合化用公開鍵を取得
  nn::Result cerRet = nn::util::Base64::FromBase64String( PUBLIC_CER, m_data.m_publicKey, sizeof(m_data.m_publicKey), &m_data.m_publicKeySize );
  GFL_ASSERT( cerRet.IsSuccess() ); // @check
#endif
}


GFL_NAMESPACE_END(savedata)
