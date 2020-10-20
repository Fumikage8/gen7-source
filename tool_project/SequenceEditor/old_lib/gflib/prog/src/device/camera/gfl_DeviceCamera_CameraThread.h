#ifndef __gfl_DeviceCamera_CameraThread__
#define __gfl_DeviceCamera_CameraThread__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_CameraThread.h
 * @brief  カメラ1機の処理をするスレッド
 * @author obata_toshihoro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <bugfix_gflib.h>
#include <gfl_Macros.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>

namespace gfl {
  namespace device {
    namespace camera {

      class BufferManager;


      class CameraThread : public gfl::base::Thread 
      {
        GFL_FORBID_COPY_AND_ASSIGN( CameraThread ); // コピーコンストラクタ, 代入演算子を禁止

        public:
        /**
         * @brief セットアップパラメータ
         */
        struct SetupParam {
          gfl::heap::HeapBase* pHeap;                   // 使用するヒープ
          nn::camera::Port port;                        // カメラの接続先ポート
          nn::camera::CameraSelect select;              // どのカメラなのか
          nn::camera::Size captureSize;                 // カメラがキャプチャする画像の解像度
          s16 captureWidth;                             // 解像度の幅
          s16 captureHeight;                            // 解像度の高さ
          gfl::device::camera::BufferManager* pBuffers; // カメラデータのバッファ
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


        private:
        // スレッド処理
        virtual void Initialize( void ); // 初期化処理
        virtual void Finalize( void );   // 終了処理
        virtual void Main( void );       // メイン処理
        virtual void OnKill( void );     // スレッド破棄時の処理


        protected:
        nn::camera::Port m_port;            // カメラの接続先ポート
        nn::camera::CameraSelect m_select;  // どのカメラなのか
        gfl::device::camera::BufferManager* m_pBuffers; // キャプチャデータのバッファ
        nn::os::Event m_threadEndEvent;     // スレッドを抜けるためのイベント
        nn::os::Event m_yuvReceiveEndEvent; // YUV データの受信完了が通知されるイベント
        nn::os::Event m_bufferErrorEvent;   // バッファエラーが通知されるイベント
        nn::os::Event m_vsyncEvent;         // カメラの VSync が通知されるイベント
        nn::os::Event m_activateEvent;      // カメラを起動するためのイベント
        nn::os::Event m_captureStartEvent;  // キャプチャを開始するためのイベント
        nn::os::Event m_captureStopEvent;   // キャプチャを停止するためのイベント
        nn::os::Event* m_pY2rStartEvent;    // 1フレーム分のYUVデータの受信完了を通知するイベント
        s32 m_yuvTransferUnit;              // 1回の転送サイズ
        
#ifdef BUGFIX_BTS6907_20130528  // カメラキャプチャ中に電源メニューを表示すると, キャプチャが終了しない不具合の修正
        // StopCapture した後, IsBusy でキャプチャ終了を待つ場合, IsBusy が常に true を返す状態になることがある.
        // その場合は一定時間でタイムアウトし, キャプチャが停止したとみなす.
        bool m_isCaptureStoppedByTimeout;
#endif

        // StopCapture した後, IsBusy でキャプチャ終了を待つ場合,
        // 途中で StartCapture が入ってしまうと, キャプチャが終了せず無限に待ち続けてしまう.
        // そのため, キャプチャの開始・停止処理は排他的に行う.
        nn::os::CriticalSection m_CS_forCaptureSwitch;

        u32* m_errorBitFlag;  // ERROR_BIT_????
        bool m_threadEndFlag;  // SBTS[2574]: 【とまり：QRコード読み取り画面でカメラ機能が停止することがあり、その状態で戻ろうとするととまる】  //sango,2014/06/25,kawada_koji


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


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif
