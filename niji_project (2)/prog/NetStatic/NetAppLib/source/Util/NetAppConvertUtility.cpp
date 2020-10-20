//==============================================================================
/**
 * @file	ConvertUtility.cpp
 * @brief	汎用のコンバート系クラス
 * @author	ichiraku_katsuhiko -> yuto_uchida が移植
 * @data	2013/01/28, 18:21
 */
// =============================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppConvertUtility.h"

#include <nn/util/util_crc.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Util)

// コンストラクタ
ConvertUtility::ConvertUtility( void )
{
}

// デストラクタ
ConvertUtility::~ConvertUtility( void )
{
}

//--------------------------------------------------------------------------
/**
 * @brief  u64の数値を特定の規則に従って文字列に変換
 *
 * @param   [in]id        変換元数値
 * @param   [out]outStr   変換後文字列
 * @param   [in]addHyphenCode        ハイフンコードを4文字区切りでつけるかどうか
 */
//--------------------------------------------------------------------------
void ConvertUtility::U64ToStr( const u64 id, gfl2::str::StrBuf *outStr , bool addHyphenCode )
{
  // CRC
  u64 crc16 = 0;

#if defined(GF_PLATFORM_CTR)
  crc16 = nn::util::Crc16Ccitt::Calculate( &id , sizeof(id) );
#elif defined(GF_PLATFORM_WIN32)
#endif

  GFL_PRINT("crc16: %llu \n", crc16);

  u16 num = 0;
  u64 nexTemp = id;
  u64 crcTemp = crc16;

  u64 mask = 0x1F;  // 5bit分のマスク

  gfl2::str::STRCODE str[ STR_MAX+1 ];
  gfl2::str::STRCODE outBuf[ STR_MAX_WITH_HYPHEN+1 ];

  u32               outIndex = 0;

  ::std::memset( str,    0, sizeof(str) );
  ::std::memset( outBuf, 0, sizeof(outBuf) );

  // 5bitずつを16文字に分ける
  for( s32 index = STR_MAX - 1 ; index >= 0 ; index -- )
  {
    num = 0;
    num = nexTemp & mask;
    str[index] = _GetGameSyncCode( num );

    nexTemp = nexTemp >> 5;

    if( index == 4 )
    {//CRCをくっつける
      crcTemp = crcTemp << 4;
      nexTemp = nexTemp | crcTemp;
      GFL_PRINT("crc[%d]temp[%llu]\n",crcTemp,nexTemp);
    }
  }

  //ハイフンを付加するオプションが有効
  if( addHyphenCode )
  {
    for( s32 index = 0; index < STR_MAX; ++index )
    {
      if( ((index & 0x03) == 0) && (index != 0) )
      {//4文字区切り
        outBuf[outIndex] = L'-';
        outIndex++;
      }
      outBuf[outIndex] = str[ index ];
      outIndex++;
    }
  }
  else{
    ::std::memcpy( outBuf, str, sizeof(str) );
  }

  GFL_PRINT("Convert %ls \n", outBuf);

  // 結果格納
  outStr->SetStr( outBuf );
}

//--------------------------------------------------------------------------
/**
 * @brief   U64ToStrで生成された文字列をu64の数値に変換
 *
 * @param   [in]inStr   変換元文字列
 * @return  id          変換後数値
 */
//--------------------------------------------------------------------------
u64 ConvertUtility::StrToU64( gfl2::str::StrBuf *inStr )
{
  // 文字に格納する
  gfl2::str::STRCODE strArray[ STR_MAX + 1 ];
  inStr->PutStr( strArray, STR_MAX + 1 );

  u64 id    = 0;
  u64 crc16 = 0;

  s32 index;
  // CRC
  GFL_PRINT("crc convert\n");
  for( index = 0; index < 4; ++index )
  {
    u16 num = _GetGameSyncNum( strArray[ index ] );
    // 変換エラー
    if( num == RESULT_ERROR ){
      return RESULT_ERROR;
    }
    crc16 |= num;
    crc16 = crc16 << 5;
  }
  crc16 = crc16 >> 5;
  crc16 = crc16 >> 4;

  // ID
  GFL_PRINT("id convert\n");
  for( ; index < STR_MAX; ++index )
  {
    u16 num = _GetGameSyncNum( strArray[ index ] );
    // 変換エラー
    if( num == RESULT_ERROR ){
      return RESULT_ERROR;
    }
    id |= num;
    if( index < STR_MAX - 1 )
      id = id << 5;
  }

  // BTS[5185]:【シーケンス：IDの一部が一致しているだけでバトルビデオを検索できる】の対応
  // CRCが一致検索していなかった場合は数値をクリアする
  u64 tmpCrc16  = 0;
#if defined(GF_PLATFORM_CTR)
  tmpCrc16 = nn::util::Crc16Ccitt::Calculate( &id , sizeof(id) );
#elif defined(GF_PLATFORM_WIN32)
#endif

  if( crc16 == tmpCrc16 )
  {
    GFL_PRINT("成功\n");
  }
  else
  {
    GFL_PRINT("失敗\n");
    id = 0xFFFFFFFFFFFFFFFF;
  }

  GFL_PRINT("Convert %llu crc %llu \n", id, crc16 );
  return id;
}

//ゲームシンク表示文字列取得
wchar_t ConvertUtility::_GetGameSyncCode( u16 num )
{
  wchar_t ch = 0;

  if( num >= 0 && num <= 9 )
  {
    ch = L'0' + num;
  }
  else
  {
    ch = L'A' + (num - 10);
  }

  if( ch == L'0' )
  {
    ch = L'W';
  }
  else if( ch == L'O' )
  {
    ch = L'Z';
  }
  else if( ch == L'1' )
  {
    ch = L'X';
  }
  else if( ch == L'I' )
  {
    ch = L'Y';
  }

  GFL_PRINT("[%u:%d]\n",num,ch);

  return ch;
}

//ゲームシンク表示文字列取得
u16 ConvertUtility::_GetGameSyncNum( wchar_t ch )
{
  u16 num = 0;

  // BTS[5185]：【シーケンス：IDの一部が一致しているだけでバトルビデオを検索できる】
  // 使用されない文字「0」「1」「I」「O」は使用しない
  if( ch == L'0' || ch == L'1' || ch == L'I' || ch == L'O' )
  {
    GFL_PRINT("変換に使用されない文字なのでエラー %d \n", ch );
    return RESULT_ERROR;
  }

  if( ch == L'W' )
  {
    ch = L'0';
  }
  else if( ch == L'Z' )
  {
    ch = L'O';
  }
  else if( ch == L'X')
  {
    ch =  L'1';
  }
  else if( ch == L'Y')
  {
    ch = L'I';
  }

  if( ch >= L'0' && ch <= L'9' )
  {
    num = ch - L'0';
  }
  else
  {
    num = (ch - L'A' ) + 10;
  }

  GFL_PRINT("[%u:%d]\n",num,ch);

  return num;
}

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetApp)
