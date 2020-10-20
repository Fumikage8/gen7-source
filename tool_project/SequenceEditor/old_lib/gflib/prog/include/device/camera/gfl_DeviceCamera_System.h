#ifndef __GFL_DEVICECAMERA_SYSTEM_H__
#define __GFL_DEVICECAMERA_SYSTEM_H__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_System.h
 * @brief  カメラシステム
 * @author obata_toshihiro
 * @date   2010.11.26
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once




/*
SBTS[2572]: カメラ起動中はスリープしてはならない。


[詳しい解説]
kujiraやsangoのprojectのつくりでは、
gflib_cppのデバイスカメラ起動中はスリープをしてはならない。
もしカメラ起動中にスリープをしてしまうと以下のようなフローに陥り、
メインスレッドnnMainが進まなくなってしまう。


(1) ３DS本体がスリープしたがる

フタを閉じて３DS本体をスリープさせようとすると、
まず３DS本体がスリープしたがり
projectのSleepHandler::SleepQueryCallbackを呼び出す。
この時点ではnn::applet::REPLY_LATERを返しているので
アプリケーションはまだスリープには入らない。

project\prog\src\system\sleep.cpp
AppletQueryReply SleepHandler::SleepQueryCallback( uptr arg )
  return nn::applet::REPLY_LATER;  //返答を保留します。(これを返した後は、速やかにReplySleepQuery()で返答してください)


(2) nn::cameraがスリープする

アプリケーションはスリープしていないが
nn::cameraはスリープを開始する。
そのため、nn::cameraの関数はResultIsSleepingを返すようになる。
gflib_cppのカメラはResultIsSleepingが返ってくると
ResultIsSleepingが解消されるまでその場でループして待つつくりになっている。

gflib_cpp\gflib\prog\src\device\camera\gfl_DeviceCamera_System.cpp
      void System::InitializeLibrary( void )
          while( result == nn::camera::CTR::ResultIsSleeping() ) {
            //NTSCより100milliは目安
            nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
            result = nn::camera::Initialize();
          }


(3) メインスレッドが進まない

gflib_cppのカメラの初期化(gfl::device::camera::Systemのnew)を
メインスレッドで行っている場合、
カメラの初期化がResultIsSleeping解消待ちループから進まなくなっているため
メインスレッドがその場所から進まなくなる。

本来なら、アプリケーションがスリープに入るには

project\prog\src\system\main.cpp
extern "C" bool GameMainLoop( gamesystem::GameManager* pGameManager )
      if(!sleepFunction(pGameManager, &isStopWifi)){

    project\prog\src\system\sleep.cpp
    void SleepHandler::SleepSystem( void )
      nn::applet::ReplySleepQuery(nn::applet::REPLY_ACCEPT);

が呼ばれなければならないが、
今はメインスレッドがカメラの初期化を行っている場所から
進まなくなっているため
ここには到達できずスリープできないままでいることになる。


(4) 復帰しなくなる

フタを開けて３DS本体をスリープから復帰させようとしても、
アプリケーションが正常にスリープしていないため
復帰することもできない。

スリープからの復帰は、正常に
project\prog\src\system\sleep.cpp
void SleepHandler::SleepSystem( void )
  nn::applet::ReplySleepQuery(nn::applet::REPLY_ACCEPT);
に到達していて初めて可能なものなので、
メインスレッドが進まず
ここに到達していない今は復帰もできない。

そのため、フタは開いているが、
nn::cameraの関数はResultIsSleepingを返し続けるという状況になり、
相変わらずカメラの初期化のResultIsSleeping解消待ちループから
メインスレッドは進まないでいる。


以上です。

kujiraやsangoのprojectのつくりでは
スリープ要求があったときに直ちにスリープには移行しないので、
メインスレッドnnMainからスリープ復帰を待つような仕組みのあるものを呼び出すときは
同様のことが起きないように気を付けましょう！
*/




#include <nn.h>
#include <gfl_Heap.h> // for gfl::heap
#include <gfl_Macros.h> // for GFL_FORBID_COPY_AND_ASSIGN
#include <device/camera/gfl_DeviceCamera_Types.h>


namespace gfl {
  namespace device { 
    namespace camera { 

      class BufferManager;
      class CameraThread;
      class Y2rThread;
      class Controller;


      class System
      {
        GFL_FORBID_COPY_AND_ASSIGN( System ); // コピーコンストラクタ, 代入演算子を禁止


        public:
        /**
         * @brief カメラごとの設定
         */
        struct CameraParam {
          bool isValid;                        // 以降のパラメータが有効かどうか
          DeviceID deviceID;                   // どのカメラなのか？
          CaptureSize captureSize;             // カメラがキャプチャする画像の解像度
          s32 trimmingWidth;                   // キャプチャ画像(YUV)のトリミング後の幅
          s32 trimmingHeight;                  // キャプチャ画像(YUV)のトリミング後の高さ
          s32 outputWidth;                     // 出力画像(RGB)の幅
          s32 outputHeight;                    // 出力画像(RGB)の高さ
          OutputFormat outputFormat;           // 出力ピクセルのRGBフォーマット
          BlockAlignment outputBlockAlignment; // 出力画像のRGBデータの並び順
          s16 outputAlpha;                     // 出力画像(RGB)のアルファ値 (R8G8B8A8 or R5G5B5A1 の場合のみ有効)
          bool noiseFilterEnable;              // ノイズフィルターの有無
          bool autoExposureEnable;             // 自動露出機能の有無
          s16 autoExposureRect_left;           // 自動露出の計算に使用する矩形領域 (左上 x 座標)
          s16 autoExposureRect_top;            // 自動露出の計算に使用する矩形領域 (左上 y 座標)
          s16 autoExposureRect_width;          // 自動露出の計算に使用する矩形領域 (幅)
          s16 autoExposureRect_height;         // 自動露出の計算に使用する矩形領域 (高さ)
          WhiteBalance whiteBalance;           // ホワイトバランスの指定
          bool autoWhiteBalanceEnable;         // オートホワイトバランスの有無
          s16 autoWhiteBalanceRect_left;       // オートホワイトバランスの計算に使用する矩形領域 (左上 x 座標)
          s16 autoWhiteBalanceRect_top;        // オートホワイトバランスの計算に使用する矩形領域 (左上 y 座標)
          s16 autoWhiteBalanceRect_width;      // オートホワイトバランスの計算に使用する矩形領域 (幅)
          s16 autoWhiteBalanceRect_height;     // オートホワイトバランスの計算に使用する矩形領域 (高さ)
          s8 sharpness;                        // 鮮明度 [-4, 5]
          Flip flip;                           // 反転処理
          FrameRate frameRate;                 // フレームレート
          PhotoMode photoMode;                 // 撮影モード
          Effect effect;                       // エフェクト
          Contrast contrast;                   // コントラスト
          LensCorrection lensCorrection;       // レンズ補正の程度
        };

        /**
         * @brief セットアップパラメータ
         */
        struct SetupParam { 
          gfl::heap::HeapBase* pHeap;                              // システムが使用するヒープ
          gfl::heap::HeapBase* pDeviceHeap;                        // システムが使用するデバイスヒープ
          CameraParam          cameraParam[ CAMERA_NUM ];          // 各カメラの設定
          u32                  cameraThreadPriority[ CAMERA_NUM ]; // 各カメラ制御スレッドの優先度
          u32                  y2rThreadPriority;                  // キャプチャ画像処理スレッドの優先度
        };


        //-------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //-------------------------------------------------------------------------------
        System( const SetupParam& param );  // SBTS[2572]: カメラ起動中はスリープしてはならない。詳しい解説はgflib_cpp/gflib/prog/include/device/camera/gfl_DeviceCamera_System.hに。

        //-------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //-------------------------------------------------------------------------------
        virtual ~System(); 


        public:
        //-------------------------------------------------------------------------------
        /**
         * @brief カメラへのアクセッサを取得する
         * @param cameraID  取得対象のカメラ
         * @retval NULL  指定したカメラが存在しない場合
         */
        //-------------------------------------------------------------------------------
        Controller* GetCameraController( CameraID cameraID ) const;



        protected:
        gfl::heap::HeapBase* m_pHeap;       // システムが使用するヒープ
        gfl::heap::HeapBase* m_pDeviceHeap; // システムが使用するデバイスヒープ

        BufferManager* m_buffer[ CAMERA_NUM ];           // 各カメラのバッファ
        CameraThread*  m_cameraThread[ CAMERA_NUM ];     // 各カメラスレッド
        Y2rThread*     m_y2rThread;                      // YUV ==> RGB 変換スレッド
        nn::os::Event  m_y2rStartEvent[ CAMERA_NUM ];    // カメラスレッド ==> Y2Rスレッド への変換通達イベント
        Controller*    m_cameraController[ CAMERA_NUM ]; // 各カメラへのアクセッサ

        u32 m_errorBitFlag;  // エラーが発生したとき0以外となる。対象のビットが1になっている。ERROR_BIT_????



#if GFL_DEBUG
        // FatalErrorが起きたときの状況をつくり出すフラグ
        private:
        static bool s_debugFatalErrorOccurFlag;
        public:
        static void DebugSetFatalErrorOccurFlag(bool flag)
        {
          s_debugFatalErrorOccurFlag = flag;
        }
        static bool DebugGetFatalErrorOccurFlag(void)
        {
          return s_debugFatalErrorOccurFlag;
        }
        void DebugSetErrorBitIfFatalErrorOccurFlagIsOn(void);
#endif



        private:
        void InitializeLibrary( void );                        // カメラライブラリを初期化する
        void InitBuffers( const SetupParam& param );           // 各カメラのバッファを生成する
        void InitY2rThread( const SetupParam& param );         // Y2Rスレッドを生成する
        void InitCameraThreads( const SetupParam& param );     // カメラスレッドを生成する
        void InitCameraControllers( const SetupParam& param ); // カメラのアクセッサを生成する

        BufferManager* CreateBuffer( const CameraParam& param );
        CameraThread* CreateCameraThread( CameraID cameraID, const CameraParam& param );
        Controller* CreateCameraController( CameraID cameraID, const CameraParam& param );

        void FinalizeLibrary( void );            // カメラライブラリを終了する
        void DeleteBuffers( void );              // 各カメラのバッファを破棄する
        void KillAndDeleteY2rThread( void );     // Y2Rスレッドを破棄する
        void KillAndDeleteCameraThreads( void ); // カメラスレッドを破棄する
        void DeleteCameraController( void );     // カメラのアクセッサを破棄する



        // 以下、gfl::device::camera::Controller 用のメソッド ////////////////////////////

        public:
        //-------------------------------------------------------------------------------
        /**
         * @brief カメラを起動する
         */
        //-------------------------------------------------------------------------------
        void ActivateCamera( CameraID cameraID );

        //-------------------------------------------------------------------------------
        /**
         * @brief すべてのカメラを停止させる
         */
        //-------------------------------------------------------------------------------
        void DeactivateAllCamera( void ); 

        //-------------------------------------------------------------------------------
        /**
         * @brief キャプチャを開始する
         */
        //-------------------------------------------------------------------------------
        void StartCapture( CameraID cameraID );

        //-------------------------------------------------------------------------------
        /**
         * @brief キャプチャを停止する
         */
        //-------------------------------------------------------------------------------
        void StopCapture( CameraID cameraID );

        //-------------------------------------------------------------------------------
        /**
         * @brief キャプチャ中かどうかを調べる
         */
        //-------------------------------------------------------------------------------
        bool IsCapturing( CameraID cameraID ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief バッファを取得する
         */
        //-------------------------------------------------------------------------------
        const BufferManager* GetBufferManager( CameraID cameraID ) const;

        //-------------------------------------------------------------------------------
        /**
         * @brief 露光を設定します。
         * @param[in] exposure  露出量を指定します。指定可能な値の範囲は [-5, +5] です。
         */
        //-------------------------------------------------------------------------------
        void SetExposure(CameraID cameraID, s8 exposure);

        //-------------------------------------------------------------------------------
        /**
         * @brief シャッター音の再生とカメラLEDの一時消灯を行います。
         */
        //-------------------------------------------------------------------------------
        void PlayShutterSound( ShutterSoundType type );

      }; 


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif // __GFL_DEVICECAMERA_SYSTEM_H__
