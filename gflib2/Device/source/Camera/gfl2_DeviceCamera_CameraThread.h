#if GF_PLATFORM_CTR
#if !defined( GFL2_DEVICECAMERA_CAMERATHREAD_H_INCLUDE )
#define GFL2_DEVICECAMERA_CAMERATHREAD_H_INCLUDE
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_CameraThread.h
 * @brief  カメラ1機の処理をするスレッド
 * @author obata_toshihoro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)

      class BufferManager;


      class CameraThread : public gfl2::thread::ThreadInterface
      {
        GFL_FORBID_COPY_AND_ASSIGN( CameraThread ); // コピーコンストラクタ, 代入演算子を禁止

        public:
        /**
         * @brief セットアップパラメータ
         */
        struct SetupParam {
          gfl2::heap::HeapBase* pHeap;                   // 使用するヒープ
          nn::camera::Port port;                        // カメラの接続先ポート
          nn::camera::CameraSelect select;              // どのカメラなのか
          nn::camera::Size captureSize;                 // カメラがキャプチャする画像の解像度
          s16 captureWidth;                             // 解像度の幅
          s16 captureHeight;                            // 解像度の高さ
          gfl2::device::camera::BufferManager* pBuffers; // カメラデータのバッファ
          nn::os::Event* pY2rStartEvent;                // YUV データの転送完了を通知するイベント
          s16 trimmingWidth;                            // トリミング後の画像の横幅
          s16 trimmingHeight;                           // トリミング後の画像の縦幅
          bool noiseFilterEnable;                       // ノイズフィルターの有無
          bool autoExposureEnable;                      // 自動露出機能の有無
          s16 autoExposureRect_left;                    // 自動露出の計算に使用する矩形領域 ( 左上 x 座標 )
          s16 autoExposureRect_top;                     // 自動露出の計算に使用する矩形領域 ( 左上 y 座標 )
          s16 autoExposureRect_width;                   // 自動露出の計算に使用する矩形領域 ( 横幅 )
          s16 autoExposureRect_height;                  // 自動露出の計算に使用する矩形領域 ( 縦幅 )
          nn::camera::WhiteBalance whiteBalance;        // ホワイトバランスの指定
          bool autoWhiteBalanceEnable;                  // オートホワイトバランスの有無
          s16 autoWhiteBalanceRect_left;                // オートホワイトバランスの計算に使用する矩形領域 ( 左上 x 座標 )
          s16 autoWhiteBalanceRect_top;                 // オートホワイトバランスの計算に使用する矩形領域 ( 左上 y 座標 )
          s16 autoWhiteBalanceRect_width;               // オートホワイトバランスの計算に使用する矩形領域 ( 横幅 )
          s16 autoWhiteBalanceRect_height;              // オートホワイトバランスの計算に使用する矩形領域 ( 縦幅 )
          s8 sharpness;                                 // 鮮明度
          nn::camera::Flip flip;                        // 反転処理
          nn::camera::FrameRate frameRate;              // フレームレート
          nn::camera::PhotoMode photoMode;              // 撮影モード
          nn::camera::Effect effect;                    // エフェクト
          nn::camera::Contrast contrast;                // コントラスト
          nn::camera::LensCorrection lensCorrection;    // レンズ補正の程度
          u32*                 errorBitFlag;  // ERROR_BIT_????
        };
        //-------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //-------------------------------------------------------------------------------
        CameraThread( const SetupParam& param );

        //-------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //-------------------------------------------------------------------------------
        virtual ~CameraThread();

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


        public:
        //-------------------------------------------------------------------------------
        /**
         * @brief カメラを起動する
         */
        //-------------------------------------------------------------------------------
        void Activate();


        //-------------------------------------------------------------------------------
        /**
         * @brief キャプチャを開始する
         */
        //-------------------------------------------------------------------------------
        void StartCapture();

        //-------------------------------------------------------------------------------
        /** 
         * @brief キャプチャを停止する
         */
        //-------------------------------------------------------------------------------
        void StopCapture( void );

        //-------------------------------------------------------------------------------
        /**
         * @brief キャプチャ中かどうかを調べる
         */
        //-------------------------------------------------------------------------------
        bool IsBusy( void ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief 露光を設定します。
         * @param[in] exposure  露出量を指定します。指定可能な値の範囲は [-5, +5] です。
         */
        //-------------------------------------------------------------------------------
        void SetExposureEx(s8 exposure);


        bool IsAlive( void ){ return m_pThread->IsAlive(); };
        void Kill( void );

        private:
        // スレッド処理
        void Initialize( void ); // 初期化処理
        void Finalize( void );   // 終了処理

        virtual void Function( void );

        protected:
        nn::camera::Port m_port;            // カメラの接続先ポート
        nn::camera::CameraSelect m_select;  // どのカメラなのか
        gfl2::device::camera::BufferManager* m_pBuffers; // キャプチャデータのバッファ
        nn::os::Event m_threadEndEvent;     // スレッドを抜けるためのイベント
        nn::os::Event m_yuvReceiveEndEvent; // YUV データの受信完了が通知されるイベント
        nn::os::Event m_bufferErrorEvent;   // バッファエラーが通知されるイベント
        nn::os::Event m_vsyncEvent;         // カメラの VSync が通知されるイベント
        nn::os::Event m_activateEvent;      // カメラを起動するためのイベント
        nn::os::Event m_captureStartEvent;  // キャプチャを開始するためのイベント
        nn::os::Event m_captureStopEvent;   // キャプチャを停止するためのイベント
        nn::os::Event* m_pY2rStartEvent;    // 1フレーム分のYUVデータの受信完了を通知するイベント
        s32 m_yuvTransferUnit;              // 1回の転送サイズ
        
        // StopCapture した後, IsBusy でキャプチャ終了を待つ場合, IsBusy が常に true を返す状態になることがある.
        // その場合は一定時間でタイムアウトし, キャプチャが停止したとみなす.
        bool m_isCaptureStoppedByTimeout;

        // StopCapture した後, IsBusy でキャプチャ終了を待つ場合,
        // 途中で StartCapture が入ってしまうと, キャプチャが終了せず無限に待ち続けてしまう.
        // そのため, キャプチャの開始・停止処理は排他的に行う.
        nn::os::CriticalSection m_CS_forCaptureSwitch;

        u32* m_errorBitFlag;  // ERROR_BIT_????
        bool m_threadEndFlag;  // SBTS[2574]: 【とまり：QRコード読み取り画面でカメラ機能が停止することがあり、その状態で戻ろうとするととまる】  //sango,2014/06/25,kawada_koji

        gfl2::thread::Thread* m_pThread;

        private:
        // カメラを初期化する
        void InitCamera( const SetupParam& param );
        void SetCaptureSize( const SetupParam& param );
        void SetNoiseFilter( const SetupParam& param );
        void SetExposure( const SetupParam& param );
        void SetWhiteBalance( const SetupParam& param );
        void SetTrimming( const SetupParam& param );
        void SetSharpness( const SetupParam& param );
        void SetFlip( const SetupParam& param );
        void SetFrameRate( const SetupParam& param );
        void SetPhotoMode( const SetupParam& param );
        void SetEffect( const SetupParam& param );
        void SetContrast( const SetupParam& param );
        void SetLensCorrection( const SetupParam& param );

        // 1フレーム分のデータ転送の設定をする
        void SetNextFrameReceiving( void );

        // カメラの Vsync 処理
        void VsyncFunc( void );

        // キャプチャの開始を排他的に行う
        void StartCaptureExclusively( void );

        // キャプチャの停止と停止待ちを排他的に行う
        void StopCaptureAndWaitCaptureStopExclusively( void );


        private:
        // スレッドが確保するスタックサイズ
        static const u16 STACK_SIZE = 4096;


#if GFL_DEBUG
        // FatalErrorが起きたときの状況をつくり出すフラグ
        public:
        void DebugSetErrorBit(void);
#endif

      };


GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GFL2_DEVICECAMERA_CAMERATHREAD_H_INCLUDE
#endif // GF_PLATFORM_CTR
