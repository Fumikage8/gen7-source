#if GF_PLATFORM_CTR
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_Y2rThread.cpp
 * @brief  YUB ==> RGB 変換スレッド
 * @author obata_toshihiro
 * @date   2010.11.27
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <nn.h>
#include <nn/y2r.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "gfl2_DeviceCamera_BufferManager.h"
#include "gfl2_DeviceCamera_Y2rThread.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)


      //---------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //---------------------------------------------------------------------------------
      Y2rThread::Y2rThread( const SetupParam& param ) :
        m_y2rTransferEndEvent(),
        m_threadEndEvent(),
        m_errorBitFlag(param.errorBitFlag),
        m_threadEndFlag(false)
      {
        m_threadEndEvent.Initialize( false );
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::GetTransferEndEvent( &m_y2rTransferEndEvent );
        }
        else
        {
          m_y2rTransferEndEvent.Initialize( false );
        }

        for( int i=0; i<2; i++ )
        {
          this->CalcConvertSettings( param.convertParam[i], &m_convertSetting[i] ); 
        }

        this->InitY2r();

        m_pThread = GFL_NEW( param.pHeap ) gfl2::thread::Thread( this, param.pHeap, true, 4096 );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //---------------------------------------------------------------------------------
      Y2rThread::~Y2rThread() 
      {
        m_threadEndEvent.Finalize();
        m_y2rTransferEndEvent.Finalize();

        GFL_SAFE_DELETE( m_pThread );
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド開始
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Start( int no )
      {
        m_pThread->Start( no );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief プライオリティの設定
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::SetPriority( int no )
      {
        m_pThread->SetPriority( no );
      }


      /**
       * @brief 変換パラメータから, コンバートの設定を算出する
       */
      void Y2rThread::CalcConvertSettings( const ConvertParam& param, ConvertSetting* pSetting )
      {
        pSetting->pConvertStartEvent = param.pConvertStartEvent;
        pSetting->pBuffers = param.pBuffers;
        
        // トリミング後の画像サイズ
        pSetting->trimmingWidth = param.trimmingWidth;
        pSetting->trimmingHeight = param.trimmingHeight;
         
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // 入力画像のサイズ
          pSetting->inputImageSize = 
            nn::camera::GetFrameBytes( param.trimmingWidth, param.trimmingHeight );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
         
          // 入力画像の1ラインサイズ
          pSetting->inputLineSize = nn::camera::GetLineBytes( param.trimmingWidth );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        }
        else
        {
          pSetting->inputImageSize = 0;
          pSetting->inputLineSize = 0;
        }

        // RGB バッファのフォーマット
        pSetting->outputFormat = param.outputFormat;
        
        // RGB データの並び順
        pSetting->outputBlockAlignment = param.outputBlockAlignment;
        
        // 出力画像のアルファ値
        pSetting->outputAlpha = param.outputAlpha;
         
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // 出力画像のサイズ
          pSetting->outputImageSize = 
            nn::y2r::GetOutputImageSize(  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                param.trimmingWidth, param.trimmingHeight, param.outputFormat );
         
          // 出力バッファの1ブロックサイズ
          pSetting->outputBlockSize = 
            nn::y2r::GetOutputBlockSize( param.trimmingWidth, param.outputFormat );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        }
        else
        {
          pSetting->outputImageSize = 0;
          pSetting->outputBlockSize = 0;
        }

        // 出力先のオフセット値
        pSetting->outputOffset = 0; // テクスチャの左上に配置
         
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // 1回の転送毎に加算されるオフセット値
          int margin_pixel_num = param.outputWidth - param.trimmingWidth;
          int margin_block_num = margin_pixel_num / 8;
          int block_size = PIXEL_PER_BLOCK * nn::y2r::GetOutputFormatBytes( param.outputFormat );  // Initializeしていないと使えない  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          pSetting->outputTransferStride = margin_block_num * block_size;
        }
        else
        {
          pSetting->outputTransferStride = 0;
        }
      }


      /**
       * @brief Y2R の初期化
       */
      void Y2rThread::InitY2r( void )
      { 
        // 動作中のコンバートを強制終了
        this->StopConversionAndWait();

        // 入力フォーマットを設定
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::SetInputFormat( nn::y2r::INPUT_YUV422_BATCH ); // カメラからの入力はフォーマット固定
        }

        // Y2R 変換の完了通知を受ける
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::SetTransferEndInterrupt( true );
        }

        // 変換係数を設定
        nn::y2r::StandardCoefficient coefficient;
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::GetSuitableY2rStandardCoefficient( &coefficient );
          if(result == nn::camera::ResultFatalError()) {
            //GFL_ASSERT_STOP(0); //@fix  // ゲームを止めたくないのでコメントアウト。エラー発生のまま処理を続けるようにする。  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_GETSUITABLEY2RSTANDARDCOEFFICIENT_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSCより100milliは目安
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::GetSuitableY2rStandardCoefficient( &coefficient );
            }
          }
        }

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::SetStandardCoefficient( coefficient );
        }
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド初期化処理
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Initialize( void )
      {
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド終了処理
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Finalize( void )
      { 
        this->StopConversionAndWait();
      }


      /**
       * @brief 動作中のコンバートの停止させて, 停止が完了するのを待つ
       */
      void Y2rThread::StopConversionAndWait( void )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::StopConversion();
         
          while( nn::y2r::IsBusyConversion() )
          {
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
          }
        }
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief スレッド破棄時の処理
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Kill( void )
      {
        m_pThread->Kill();
        m_threadEndEvent.Signal();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief スレッドメイン処理
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Function( void )
      {
        Initialize();

        if( m_threadEndFlag )
        {
          nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 50 ) );  // SBTS[2574]: 【とまり：QRコード読み取り画面でカメラ機能が停止することがあり、その状態で戻ろうとするととまる】  //sango,2014/06/25,kawada_koji
        }
        else
        {
          nn::os::Event* p_events[3] =
          {
            m_convertSetting[0].pConvertStartEvent, // 変換開始イベント
            m_convertSetting[1].pConvertStartEvent, // 変換開始イベント
            &m_threadEndEvent,       // スレッド終了イベント
          };
         
          s32 event_idx = nn::os::WaitObject::WaitAny( 
              reinterpret_cast<nn::os::WaitObject**>(p_events), 3 );
         
         
          if( ( event_idx == 0 ) || ( event_idx == 1 ) ) // 変換開始イベント
          {
            // 左右のうち、先に依頼が来たほうから Y2R 変換を開始
            this->SetConvertSettings( m_convertSetting[ event_idx ] );
            this->ConvertY2R( &m_convertSetting[ event_idx ] );
         
            // もう一方のカメラ画像も変換
            s32 another = ( event_idx == 0 ) ? 1 : 0;
            if( p_events[ another ]->Wait(0) )
            {
              this->SetConvertSettings( m_convertSetting[ another ] );
              this->ConvertY2R( &m_convertSetting[ another ] );
            }
          }
          else if( event_idx == 2 ) // スレッド終了イベント
          {
            this->StopConversionAndWait();
            m_threadEndFlag = true;
          }
        }
      } 


      /**
       * @brief 変換の設定を更新する
       */
      void Y2rThread::SetConvertSettings( const ConvertSetting& setting )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // 出力フォーマットを設定
          nn::y2r::SetOutputFormat( setting.outputFormat );
         
          // 出力データの回転角度を設定
          nn::y2r::SetRotation( nn::y2r::ROTATION_NONE );
         
          // 出力データの並び順を設定
          nn::y2r::SetBlockAlignment( setting.outputBlockAlignment );
         
          // Y2R 変換の入力となる画像のサイズを設定
          nn::y2r::SetInputLineWidth( setting.trimmingWidth );
          nn::y2r::SetInputLines( setting.trimmingHeight );
         
          // 出力画像のアルファ値を設定
          nn::y2r::SetAlpha( setting.outputAlpha );
        }
      }


      /** 
       * @brief YUV ==> RGB 変換を実行する
       */
      void Y2rThread::ConvertY2R( ConvertSetting* pSetting )
      {
        // カメラの読み出しバッファを切り替えます。
        pSetting->pBuffers->SwitchReadingYuvBuffer();

        bool transfer_finished = false;
        s32 try_count = 0;

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          do { 
         
            // 入力画像(YUV)の転送設定
            nn::y2r::CTR::SetSendingYuv(
                pSetting->pBuffers->GetYuvBuffer_Reading(),
                pSetting->inputImageSize,
                pSetting->inputLineSize );
         
            // 出力画像(RGB)の転送設定
            nn::y2r::CTR::SetReceiving(
                reinterpret_cast<u8*>( pSetting->pBuffers->GetRgbBuffer_Receiving() ) + 
                pSetting->outputOffset,
                pSetting->outputImageSize,
                pSetting->outputBlockSize,
                pSetting->outputTransferStride );
         
            // Y2R変換を開始
            nn::y2r::CTR::StartConversion();
         
            // Y2R変換の完了を待つ
            // Y2Rハードウェアの不具合対策のためにタイムアウトを設定 ==> タイムアウト時はリトライする
            transfer_finished = m_y2rTransferEndEvent.Wait( nn::fnd::TimeSpan::FromMilliSeconds(20) );
            try_count++;
         
         
            if( transfer_finished ) 
            {
              pSetting->pBuffers->SwapRgbBuffers(); // RGB バッファを切り替える
            }
            else 
            {
              GFL_PRINT( "Y2R TIMEOUT: Y2R may have hung up.\n" );
              nn::y2r::StopConversion(); // 変換を強制終了する
         
              // 何度も失敗した場合は, 変換をあきらめて次へ進む
              if( CONVERT_RETRY_NUM < try_count ) {
                transfer_finished = true;
              }
            } 
         
          } while ( transfer_finished == false );
        }

      }


GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
