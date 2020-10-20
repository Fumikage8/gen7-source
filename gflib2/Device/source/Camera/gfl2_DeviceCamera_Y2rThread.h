#if GF_PLATFORM_CTR
#if !defined( GFL2_DEVICECAMERA_Y2RTHREAD_H_INCLUDE )
#define GFL2_DEVICECAMERA_Y2RTHREAD_H_INCLUDE
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_Y2rThread.h
 * @brief  YUB ==> RGB 変換スレッド
 * @author obata_toshihiro
 * @date   2010.11.27
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <heap/include/gfl2_Heap.h>
#include <Device/include/Camera/gfl2_DeviceCamera_Types.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)

      class BufferManager;


      class Y2rThread : public gfl2::thread::ThreadInterface
      {
        GFL_FORBID_COPY_AND_ASSIGN( Y2rThread ); // コピーコンストラクタ, 代入演算子を禁止


        public:
        // コンバート単位の設定
        struct ConvertParam {
          nn::os::Event*          pConvertStartEvent;   // コンバート開始が通知されるイベント
          BufferManager*          pBuffers;             // カメラのデータバッファ
          s16                     trimmingWidth;        // トリミング後の画像の横幅
          s16                     trimmingHeight;       // トリミング後の画像の縦幅
          s16                     outputWidth;          // 出力画像の幅
          s16                     outputHeight;         // 出力画像の高さ
          nn::y2r::OutputFormat   outputFormat;         // RGB バッファのフォーマット
          nn::y2r::BlockAlignment outputBlockAlignment; // RGB データの並び順
          s16                     outputAlpha;          // 出力画像のアルファ値 ( 出力フォーマットが 16bit or 32bit の場合にのみ有効 )
          s32                     outputOffset;         // RGB データ格納先のオフセット
        };

        // セットアップパラメータ
        struct SetupParam {
          gfl2::heap::HeapBase* pHeap;                      // 使用するヒープ
          ConvertParam         convertParam[ CAMERA_NUM ]; // 各コンバートの設定
          u32*                 errorBitFlag;  // ERROR_BIT_????
        }; 
        //-------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //-------------------------------------------------------------------------------
        Y2rThread( const SetupParam& param );

        //-------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //-------------------------------------------------------------------------------
        virtual ~Y2rThread(); 

        //-------------------------------------------------------------------------------
        /**
         * @brief スレッド開始
         */
        //-------------------------------------------------------------------------------
        void Start( int no ); 

        //-------------------------------------------------------------------------------
        /**
         * @brief プライオリティの設定
         */
        //-------------------------------------------------------------------------------
        void SetPriority( int no ); 

        bool IsAlive( void ){ return m_pThread->IsAlive(); };
        void Kill( void );

        private:
        // スレッド処理
        void Initialize( void ); // 初期化処理
        void Finalize( void );   // 終了処理

        virtual void Function( void );

        protected:
        nn::os::Event  m_y2rTransferEndEvent;   // データ転送の完了が通知されるイベント
        nn::os::Event  m_threadEndEvent;        // スレッドを終了させるためのイベント
        
        u32* m_errorBitFlag;  // ERROR_BIT_????
        bool m_threadEndFlag;  // SBTS[2574]: 【とまり：QRコード読み取り画面でカメラ機能が停止することがあり、その状態で戻ろうとするととまる】  //sango,2014/06/25,kawada_koji

        gfl2::thread::Thread* m_pThread;

        // コンバートの設定
        struct ConvertSetting {
          nn::os::Event*          pConvertStartEvent;   // 変換開始が依頼されるイベント
          BufferManager*          pBuffers;             // カメラのデータバッファ
          s16                     trimmingWidth;        // トリミング後の画像の横幅
          s16                     trimmingHeight;       // トリミング後の画像の縦幅
          size_t                  inputImageSize;       // 入力画像のサイズ
          s16                     inputLineSize;        // 入力画像の1ラインサイズ
          nn::y2r::OutputFormat   outputFormat;         // RGB バッファのフォーマット
          nn::y2r::BlockAlignment outputBlockAlignment; // RGB データの並び順
          s16                     outputAlpha;          // 出力画像のアルファ値 ( 出力フォーマットが 16bit or 32bit の場合にのみ有効 )
          size_t                  outputImageSize;      // 出力画像のサイズ
          s16                     outputBlockSize;      // 出力バッファの1ブロックサイズ
          s16                     outputOffset;         // 出力先の先頭オフセット値
          s16                     outputTransferStride; // 1回の転送毎に加算されるオフセット値
        };
        ConvertSetting m_convertSetting[ CAMERA_NUM ];


        private:
        // Y2R の初期化
        void InitY2r( void );

        // 変換パラメータから, コンバートの設定を算出する
        void CalcConvertSettings( const ConvertParam& param, ConvertSetting* pSetting );

        // 動作中のコンバートの停止させて, 停止が完了するのを待つ
        void StopConversionAndWait( void );

        // 変換の設定を更新する
        void SetConvertSettings( const ConvertSetting& setting );

        // YUV ==> RGB 変換を実行する
        void ConvertY2R( ConvertSetting* pSetting );


        private:
        // Y2R変換失敗時にリトライする回数
        static const u8 CONVERT_RETRY_NUM = 1;

        // 1ブロック毎のピクセル数
        static const u8 PIXEL_PER_BLOCK = 8 * 8;
      };

GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GFL2_DEVICECAMERA_Y2RTHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
