//=============================================================================
/**
 * @file   gfl_UI_Device.h
 * @brief  入力デバイスの抽象クラス
 * @author obata_toshihiro
 * @date   2010.10.22
 */
//=============================================================================
#ifndef __GFL_UI_DEVICE_H__
#define __GFL_UI_DEVICE_H__
#pragma once

#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "ui/gfl_UI_Types.h"

namespace gfl {
  namespace ui {


    class DeviceImplementer;
    class RepeatTimer;
    class LogPlayer;
    class LogRecorder; 


    class Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Device );


      public:

      // 動作フレームレート
      enum FrameRate {
        FRAME_RATE_60,
        FRAME_RATE_30,
        FRAME_RATE_NUM,
      };

      //-----------------------------------------------------------------------
      /**
       * @brief フレームレートを取得する
       * @return 現在設定されているフレームレート
       */
      //-----------------------------------------------------------------------
      FrameRate GetFrameRate( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief フレームレートを設定する
       * @param rate  新たに設定するフレームレート
       */
      //-----------------------------------------------------------------------
      void SetFrameRate( FrameRate rate ); 

      //-----------------------------------------------------------------------
      /**
       * @brief 入力の更新タイミングを同期する
       */
      //-----------------------------------------------------------------------
      void SynchronizeInputUpdate( void );




      //-----------------------------------------------------------------------
      /**
       * @brief ログを記録するバッファを確保する
       *
       * @param heap         使用するヒープ
       * @param buffer_size  バッファのサイズ
       */
      //-----------------------------------------------------------------------
      void AllocLogBuffer( gfl::heap::HeapBase* heap, u32 buffer_size );

      //-----------------------------------------------------------------------
      /**
       * @brief ログを記録するバッファをセットする( バッファの解放は行いません )
       *
       * @param p_buffer     バッファ先頭へのポインタ
       * @param buffer_size  バッファのサイズ
       */
      //-----------------------------------------------------------------------
      void SetLogBuffer( void* p_buffer, u32 buffer_size ); 

      //-----------------------------------------------------------------------
      /**
       * @brief ログの採取を開始する
       */
      //-----------------------------------------------------------------------
      void StartLogRec( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ログの採取を終了する
       */
      //-----------------------------------------------------------------------
      void StopLogRec( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ログの採取中かどうかを調べる
       * @retval true  ログ採取中
       * @retval false ログ採取中でない
       */
      //-----------------------------------------------------------------------
      bool IsLogRecording( void ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief 記録したログのフレーム数を取得する
       */
      //-----------------------------------------------------------------------
      u32 GetRecordedLogFrameCount( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 記録したログについて, 再生対象のフレーム数を設定する
       * @param playFrameCount  再生対象とするフレーム数( 負数なら最後まで再生する )
       */
      //-----------------------------------------------------------------------
      void SetPlayableLogFrameCount( s32 playFrameCount );

      //-----------------------------------------------------------------------
      /**
       * @brief ログの再生を開始する
       */
      //-----------------------------------------------------------------------
      void StartLogPlay( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ログの再生を終了する
       */
      //-----------------------------------------------------------------------
      void StopLogPlay( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ログの再生中かどうかを調べる
       * @retval true  ログ再生中
       * @retval false ログ再生中でない
       */
      //-----------------------------------------------------------------------
      bool IsLogPlaying() const; 

      //-----------------------------------------------------------------------
      /**
       * @brief ログのループ再生が有効か？
       * @retval true   ループ再生する
       * @retval false  ループ再生しない
       */
      //-----------------------------------------------------------------------
      bool IsLogLoopPlayEnable( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ログのループ再生が有効かどうか？設定する
       * @param isLoopEnable  true:ループ再生する, false:ループ再生しない
       *
       * @note デフォルト値は false です
       */
      //-----------------------------------------------------------------------
      void SetLogLoopPlayEnable( bool isLoopEnable ); 

      //-------------------------------------------------------------------
      /**
       *	@brief ログのバッファを取得 
       */
      //--------------------------------------------------------------------
      void* GetLogBuffer( void ) const;

      //-------------------------------------------------------------------
      /**
       *	@brief ログの最大バッファサイズを取得 
       */
      //--------------------------------------------------------------------
      u32 GetLogBufferMaxSize( void ) const;

      //--------------------------------------------------------------------
      /**
       * @brief データの記録位置を取得する
       *
       * @return データの記録位置
       *
       */
      //--------------------------------------------------------------------
      u32 GetLogRecPos( void ) const;

      //--------------------------------------------------------------------
      /**
       * @brief データの記録位置を設定する
       *
       * @param recPos  データの記録位置
       *
       * @note recPos 以降のデータは失われます
       */
      //--------------------------------------------------------------------
      void SetLogRecPos( u32 recPos );

      //--------------------------------------------------------------------
      /**
       * @brief データの記録最大位置を取得する
       *
       * @return データの記録最大位置
       *
       */
      //--------------------------------------------------------------------
      u32 GetLogRecMaxPos( void ) const;

      // リピート検出パラメータ
      struct RepeatParam 
      {
        u32 start_wait;   // 最初のリピート判定までの待ち時間[frame]
        u32 interval;     // リピート判定の間隔[frame]
      };

      // リピートパラメータのデフォルト値
      static const u32 DEFAULT_REPEAT_START_WAIT = 20;
      static const u32 DEFAULT_REPEAT_INTERVAL   = 5;

      //-----------------------------------------------------------------------
      /**
       * @brief リピート入力の検出パラメータをデフォルト値にリセットする
       */
      //-----------------------------------------------------------------------
      void SetDefaultRepeatParam( void );

      //-----------------------------------------------------------------------
      /**
       * @brief リピート入力の検出パラメータを設定する
       * @param param 設定する検出パラメータ
       */
      //-----------------------------------------------------------------------
      void SetRepeatParam( const RepeatParam& param );

      //-----------------------------------------------------------------------
      /**
       * @brief リピート入力の検出パラメータを取得する
       * @param[out] p_param 検出パラメータの格納先
       */
      //-----------------------------------------------------------------------
      void GetRepeatParam( RepeatParam* p_param ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief デバイス更新処理
       */
      //-----------------------------------------------------------------------
      virtual void UpdateDevice( void );


      
      //-----------------------------------------------------------------------
      /**
       * @brief デバイスが稼働状態にあるか？
       * @retval true   デバイスは稼働している
       * @retval false  デバイスは稼働していない
       */
      //-----------------------------------------------------------------------
      bool IsDeviceRunning( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief デバイスの稼働状態を変更する
       * @param isEnable  true:デバイスを稼働する, false:デバイスを停止する
       *
       * @note デバイスを停止した場合, 以降, 全ての入力が無効化されます.
       */
      //-----------------------------------------------------------------------
      void SetDeviceRunningEnable( bool isEnable );



      //-----------------------------------------------------------------------
      /**
       * @brief デバイス入力値に対する全ての制御の On/Off を切り替える
       * @param isEnable  true:入力値の制御を有効にする, false:入力値の制御を無効にする
       *
       * @note isEnable に false を指定した場合, 
       *       ( いかなる他のメソッドの影響も受けていない )ハードウェア入力を返します
       *
       * @note このメソッドから制御が戻った時点から有効になります
       */
      //-----------------------------------------------------------------------
      void SetSoftwareControlEnable( bool isEnable );

      //-----------------------------------------------------------------------
      /**
       * @brief デバイス入力値に対する制御が有効か？
       * @retval true   入力値に対する制御が有効
       * @retval false  入力値に対する制御が無効
       */
      //-----------------------------------------------------------------------
      bool IsSoftwareControlEnable( void ) const;



#if GFL_DEBUG
      //----------------------------------------------------------------------------
      /**
       *	@brief  デバッグキー　無効化設定
       */
      //-----------------------------------------------------------------------------
      void SetDebugKeyDisable( bool is_disable );
      bool IsDebugKeyDisable( void ) const;
#endif // GFL_DEBUG

      protected:

      /**
       * @brief コンストラクタ
       * @param heap         使用するヒープ
       * @param implementer  実際に入力の読み取りを行うインスタンス
       */
      Device( gfl::heap::HeapBase* heap, DeviceImplementer* implementer );

      /**
       * @brief デストラクタ
       */
      virtual ~Device(); 

      /**
       * @brief 現在の検出データと前回の検出データを比較し, 同じデータが続いているかどうかを調べる
       * @retval true   前回の検出データと今回の検出データが同じ
       * @retval false  前回の検出データと今回の検出データが異なる
       */
      virtual bool IsRepeatingSameInput( void ) const;

      /**
       * @brief 実効データを蓄積する
       * @param buffer               データの格納先
       * @param detective_data       今回の検出データ
       * @param prev_detective_data  前回の検出データ
       */
      virtual void StoreEffectiveData( void* buffer, const void* detective_data, const void* prev_detective_data ) const = 0;

      /**
       * @brief 参照すべき実効データを取得する
       */
      const void* GetRunningEffectiveData( void ) const;



      DeviceImplementer* m_pImplementer;
      RepeatTimer* m_pRepeatTimer;
      LogPlayer* m_pLogPlayer;
      LogRecorder* m_pLogRecorder;

      FrameRate m_frameRate;                              // 動作フレームレート
      u32 m_frameCount;                                   // フレームカウンタ
      static u8 m_inputUpdateInterval[ FRAME_RATE_NUM ];  // 各フレームレートに対する, 入力状態の更新間隔

      bool m_isRunning;
      bool m_isSoftwareControlEnable;
      bool m_isLogRecording;        // ログの採取中かどうか
      bool m_isLogPlaying;          // ログの再生中かどうか
      bool m_isLogLoopPlayEnable;   // ログをループ再生するか？

      void* m_nowDetectiveData;     // 現在の検出データ
      void* m_prevDetectiveData;    // 前回の検出データ
      void* m_effectiveDataBuffer;  // 実効データのバッファ
      void* m_effectiveData;        // 実効データ
      void* m_invalidEffectiveData; // 無効な実効データ

      void* m_nowRawDetectiveData;     // 現在の無加工な検出データ
      void* m_prevRawDetectiveData;    // 前回の無加工な検出データ
      void* m_rawEffectiveDataBuffer;  // 無加工な実効データのフレームレート吸収バッファ
      void* m_rawEffectiveData;        // 無加工な実行データ

#if GFL_DEBUG
      bool mDebugKeyDisable;  // デバッグキー無効
#endif

      private: 
      void UpdateRawEffectiveData( void );
      void CopyNowDetectiveData_ToPrevDetectiveData( void );
      void UpdateNowDetectiveData_ByLogPlayer( void );
      void UpdateNowDetectiveData_ByImplementer( void ); 
      void UpdateEffectiveDataBufferByLastDetectiveData( void );
      bool IsOnEffectiveDataUpdateFrame( void ) const;
      void CopyEffectiveDataBuffer_ToEffectiveData( void );
      void ClearEffectiveDataBuffer( void );

    };

  }  // namespace ui
}  // namespace gfl


#endif // __GFL_UI_DEVICE_H__
