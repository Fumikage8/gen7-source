#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpSystem.cpp
 * @brief  HTTPライブラリシステムラッパークラス。
 * @author h.suzuki
 * @date   2011.10.20
 */
//=============================================================================
#include <gflnet2/include/http/gflnet2_HttpSystem.h>


#include <nn/Result.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(http)

  void*   HttpSystem::m_postParamBuffer = NULL;    // Postパラメータ用バッファ
  bool    HttpSystem::m_isInit          = false;   // 初期済みフラグ

  //------------------------------------------------------------------
  /**
   * @brief   HttpSystemコンストラクタ＆デストラクタ(隠蔽化)
   */
  //------------------------------------------------------------------
  HttpSystem::HttpSystem(){
  }

  HttpSystem::~HttpSystem(){
  }

  //------------------------------------------------------------------
  /**
   * @brief  HttpSystemの初期化
   *
   */
  //------------------------------------------------------------------
  nn::Result HttpSystem::Initialize( gfl2::heap::HeapBase* heap, size_t postBufferSize )
  {
    // 既に初期済みの場合はSUCCESSを返却
    if( m_isInit ){
      return nn::ResultSuccess();
    }

    if( heap != NULL ){
      m_postParamBuffer = GflHeapAllocMemoryAlign4096Lower(heap , postBufferSize );// 4096アラインメントメモリアドレス
    }

    nn::Result result = nn::http::Initialize( (uptr)m_postParamBuffer, postBufferSize );

    if( result.IsSuccess() ){
       m_isInit = true;
    }
    else{
      FreeBufferMemory();
    }

    return result;
  }

  //------------------------------------------------------------------
  /**
   * @brief  HttpSystemの終了処理
   */
  //------------------------------------------------------------------
  nn::Result HttpSystem::Finalize( void )
  {
    if( !m_isInit ){
      return nn::ResultSuccess();
    }
    m_isInit = false;

    nn::Result ret = nn::http::Finalize();

    FreeBufferMemory();

    if( ret == nn::http::ResultNotInitializedErr() ){
      return nn::ResultSuccess(); // Finalize不要
    }

    return ret;
  }

  //------------------------------------------------------------------
  /**
   * @brief  Postパラメータ用バッファメモリを開放
   */
  //------------------------------------------------------------------
  void HttpSystem::FreeBufferMemory( void )
  {
    if( m_postParamBuffer != NULL )
    {
      GflHeapFreeMemory( m_postParamBuffer );
      m_postParamBuffer = NULL;
    }
  }

#if GFL_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief エラーレポート（デバッグ用）
   *
   * @param result 実行結果
   */
  //------------------------------------------------------------------
  void HttpSystem::ReportHttpError( const nn::Result& result )
  {
    if( result == nn::http::ResultInvalidStatusErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultInvalidStatusErr! \n"  );
    }
    else if( result == nn::http::ResultInvalidParamErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultInvalidParamErr! \n"  );
    }
    else if( result == nn::http::ResultOutOfMemoryErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultOutOfMemoryErr! \n"  );
    }
    else if( result == nn::http::ResultCreateEventErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultCreateEventErr! \n"  );
    }
    else if( result == nn::http::ResultCreateMutexErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultCreateMutexErr! \n"  );
    }
    else if( result == nn::http::ResultCreateQueueErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultCreateQueueErr! \n"  );
    }
    else if( result == nn::http::ResultCreateThreadErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultCreateThreadErr! \n"  );
    }
    else if( result == nn::http::ResultNotConnectedErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultNotConnectedErr! \n"  );
    }
    else if( result == nn::http::ResultAlreadyAssignHostErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultAlreadyAssignHostErr! \n"  );
    }
    else if( result == nn::http::ResultSessionErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultSessionErr! \n"  );
    }
    else if( result == nn::http::ResultClientProcessMaxErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultClientProcessMaxErr! \n"  );
    }
    else if( result == nn::http::ResultAlreadyInitializedErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultAlreadyInitializedErr! \n"  );
    }
    else if( result == nn::http::ResultNotInitializedErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultNotInitializedErr! \n"  );
    }
    else if( result == nn::http::ResultMessageSendToListenThreadErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultMessageSendToListenThreadErr! \n"  );
    }
    else if( result == nn::http::ResultMessageRecvInListenThreadErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultSessionErr! \n"  );
    }
    else if( result == nn::http::ResultConnectionNoMoreErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultConnectionNoMoreErr! \n"  );
    }
    else if( result == nn::http::ResultNoSuchConnectionErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultNoSuchConnectionErr! \n"  );
    }
    else if( result == nn::http::ResultConnectionStatusErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultConnectionStatusErr! \n"  );
    }
    else if( result == nn::http::ResultConnectionAddErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultConnectionAddErr! \n"  );
    }
    else if( result == nn::http::ResultConnectionCanceledErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultConnectionCanceledErr! \n"  );
    }
    else if( result == nn::http::ResultConnectionHostMaxErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultConnectionHostMaxErr! \n"  );
    }
    else if( result == nn::http::ResultConnectionProcessMaxErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultConnectionProcessMaxErr! \n"  );
    }
    else if( result == nn::http::ResultRequestUrlErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultRequestUrlErr! \n"  );
    }
    else if( result == nn::http::ResultRequestPortErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultRequestPortErr! \n"  );
    }
    else if( result == nn::http::ResultRequestUnknownMethodErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultRequestUnknownMethodErr! \n"  );
    }
    else if( result == nn::http::ResultRequestHeaderErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultRequestHeaderErr! \n"  );
    }
    else if( result == nn::http::ResultRequestNoNewLineErr() ){
      GFL_PRINT("gflnet2::http::HttpSystem failed  / ResultRequestNoNewLineErr! \n"  );
    }
    else{
      GFL_PRINT("gflnet2::http::HttpSystem::Initialize() failed  / Reason Unknown[%d] \n", result.GetDescription()  );
    }
  }
#endif

GFL_NAMESPACE_END(http)
GFL_NAMESPACE_END(gflnet2)

#endif
