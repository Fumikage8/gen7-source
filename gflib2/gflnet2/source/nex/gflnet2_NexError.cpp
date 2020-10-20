#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexError.cpp
 *
 * @brief  nn::nexのエラー管理クラス
 *
 * @author h.suzuki
 */
//=============================================================================
#include <gflnet2/include/nex/gflnet2_NexError.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

namespace gflnet2{
namespace nex{


//------------------------------------------------------------------
/**
 * @brief  nn::nex::qResult のエラーから通信エラーコードへ変換する
 *
 * @param  result エラー情報
 *
 * @param  通信エラーコード
 */
//------------------------------------------------------------------
nn::nex::qUnsignedInt32 NexError::GetNetworkErrorCode( const nn::nex::qResult& result )
{
  return nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode(result);
}


//------------------------------------------------------------------
/**
 * @brief  Nex関数がfalseを返却した際の通信エラーコードを取得する
 *
 * @param  通信エラーコード
 */
//------------------------------------------------------------------
nn::nex::qUnsignedInt32 NexError::GetNetworkFunctionErrorCode( void )
{
  qUnsignedInt err = nn::nex::SystemError::GetLast();

  //OHNO_PRINT("NexError::GetNetworkFunctionErrorCode %d\n",err);
  
  if( err == SYSTEMERROR_GEN_CTR_ERROR )
  {
    ReportResultError( nn::nex::SystemError::GetLastCtr() );
    return 0;
  }
  if(err==0){
    return 0;  //GetLastはエラーを返していない
 }
  return nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode(err);
}

//------------------------------------------------------------------
/**
 * @brief  プロトコルコールコンテキストのエラーをコンソール出力する（デバッグ用）
 *
 * @param  context    ProtocolCallContext オブジェクト
 */
//------------------------------------------------------------------
void NexError::ReportProtocolCallError( const nn::nex::ProtocolCallContext& context )
{
  GFL_PRINT("Error code: 0x%x\n", context.GetOutcome().GetReturnCode());
}


//------------------------------------------------------------------
/**
 * @brief  nn::Result のエラーからNEXのエラーコードを算出し、コンソール出力する（デバッグ用）
 *
 * @param  result リザルトコード
 */
//------------------------------------------------------------------
void NexError::ReportResultError( const nn::Result& result )
{
  NN_DBG_PRINT_RESULT(result);
}


//------------------------------------------------------------------
/**
 * @brief  NEX API がエラーを返したときの詳細情報をコンソール出力する（デバッグ用）
 */
//------------------------------------------------------------------
void NexError::ReportSystemError( void )
{
  qUnsignedInt err = nn::nex::SystemError::GetLast();

  if(err == 0)
  {
    //エラーが入っていない
    GFL_PRINT("nn::nex::SystemError::GetLast()==0\n");
  }
  else if (err == SYSTEMERROR_GEN_CTR_ERROR)
  {
    ReportResultError( nn::nex::SystemError::GetLastCtr() );
  }
  else
  {
    const size_t SZ = 256;
    qChar buf[SZ];
    nn::nex::SystemError::GetErrorString(err, buf, SZ);
    GFL_PRINT("Nex Error occured: %ls\n", buf);
    GFL_PRINT("network error code: %d\n", nn::nex::ErrorCodeConverter::ConvertToNetworkErrorCode(err));
  }
}



} // nex
} // gflnet

#endif
