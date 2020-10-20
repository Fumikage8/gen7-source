//=============================================================================
/**
 * @file gfl_UI_Device.cpp
 * @brief 入力デバイスの抽象クラス
 * @author obata_toshihiro
 * @date 2010.10.22
 */
//=============================================================================
#include <nn.h>
#include "gfl_Std.h"
#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_RepeatTimer.h"
#include "./gfl_UI_LogPlayer.h"
#include "./gfl_UI_LogRecorder.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap         使用するヒープ
     * @param implementer  実際に入力の読み取りを行うインスタンス
     */
    //-------------------------------------------------------------------------
    Device::Device( gfl::heap::HeapBase* heap, DeviceImplementer* implementer ) :
      m_pImplementer( implementer ),
      m_pRepeatTimer( NULL ),
      m_pLogPlayer( NULL ),
      m_pLogRecorder( NULL ),
      m_frameRate( FRAME_RATE_60 ),
      m_frameCount( 0 ),
      m_isRunning( false ),
      m_isSoftwareControlEnable( true ),
      m_isLogRecording( false ),
      m_isLogPlaying( false ),
      m_isLogLoopPlayEnable( false ),
      m_nowDetectiveData( NULL ),
      m_prevDetectiveData( NULL ),
      m_effectiveDataBuffer( NULL ),
      m_effectiveData( NULL ),
      m_invalidEffectiveData( NULL ),
      m_nowRawDetectiveData( NULL ),
      m_prevRawDetectiveData( NULL ),
      m_rawEffectiveDataBuffer( NULL ),
      m_rawEffectiveData( NULL )
    {
      m_pRepeatTimer = GFL_NEW( heap ) RepeatTimer( DEFAULT_REPEAT_START_WAIT, DEFAULT_REPEAT_INTERVAL );
      m_pLogRecorder = GFL_NEW( heap ) LogRecorder( implementer );
      m_pLogPlayer = GFL_NEW( heap ) LogPlayer( m_pLogRecorder );
      m_nowDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_prevDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_effectiveDataBuffer = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_effectiveData = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_invalidEffectiveData = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_nowRawDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_prevRawDetectiveData = GflHeapAllocMemory( heap, implementer->GetDetectiveDataSize() );
      m_rawEffectiveDataBuffer = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
      m_rawEffectiveData = GflHeapAllocMemory( heap, implementer->GetEffectiveDataSize() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Device::~Device()
    {
      GFL_DELETE m_pImplementer;
      GFL_DELETE m_pRepeatTimer;
      GFL_DELETE m_pLogPlayer;
      GFL_DELETE m_pLogRecorder;
      GflHeapFreeMemory( m_nowDetectiveData );
      GflHeapFreeMemory( m_prevDetectiveData );
      GflHeapFreeMemory( m_effectiveDataBuffer );
      GflHeapFreeMemory( m_effectiveData );
      GflHeapFreeMemory( m_invalidEffectiveData );
      GflHeapFreeMemory( m_nowRawDetectiveData );
      GflHeapFreeMemory( m_prevRawDetectiveData );
      GflHeapFreeMemory( m_rawEffectiveDataBuffer );
      GflHeapFreeMemory( m_rawEffectiveData );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief フレームレートを取得する
     * @return 現在設定されているフレームレート
     */
    //-------------------------------------------------------------------------
    Device::FrameRate Device::GetFrameRate( void ) const
    {
      return m_frameRate;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief フレームレートを設定する
     * @param rate  新たに設定するフレームレート
     */
    //-------------------------------------------------------------------------
    void Device::SetFrameRate( Device::FrameRate rate )
    {
      m_frameRate = rate;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 入力の更新タイミングを同期する
     */
    //-------------------------------------------------------------------------
    void Device::SynchronizeInputUpdate( void )
    {
      m_frameCount = 0;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ログを記録するバッファを確保する
     *
     * @param heap         使用するヒープ
     * @param buffer_size  バッファのサイズ
     */
    //-------------------------------------------------------------------------
    void Device::AllocLogBuffer( gfl::heap::HeapBase* heap, u32 buffer_size )
    {
      m_pLogRecorder->AllocBuffer( heap, buffer_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログを記録するバッファをセットする( バッファの解放は行いません )
     *
     * @param p_buffer     バッファ先頭へのポインタ
     * @param buffer_size  バッファのサイズ
     */
    //-------------------------------------------------------------------------
    void Device::SetLogBuffer( void* p_buffer, u32 buffer_size )
    {
      m_pLogRecorder->SetBuffer( p_buffer, buffer_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの採取を開始する
     */
    //-------------------------------------------------------------------------
    void Device::StartLogRec( void )
    {
      this->StopLogPlay(); // ログ再生を止める
      m_pLogRecorder->ClearLog();
      m_isLogRecording = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの採取を終了する
     */
    //-------------------------------------------------------------------------
    void Device::StopLogRec( void )
    {
      m_isLogRecording = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの採取中かどうかを調べる
     * @retval true  ログ採取中
     * @retval false ログ採取中でない
     */
    //-------------------------------------------------------------------------
    bool Device::IsLogRecording( void ) const
    {
      return m_isLogRecording;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 記録したログのフレーム数を取得する
     */
    //-------------------------------------------------------------------------
    u32 Device::GetRecordedLogFrameCount( void ) const
    {
      return m_pLogRecorder->GetLogDataNum();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 記録したログについて, 再生対象のフレーム数を設定する
     * @param playFrameCount  再生対象とするフレーム数
     */
    //-------------------------------------------------------------------------
    void Device::SetPlayableLogFrameCount( s32 playFrameCount )
    {
      const s32 end_pos = playFrameCount - 1;
      m_pLogPlayer->SetPlayEndPos( end_pos );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの再生を開始する
     */
    //-------------------------------------------------------------------------
    void Device::StartLogPlay( void )
    {
      ICHI_PRINT("Device::StartLogPlay\n");
      this->StopLogRec(); // ログ採取を止める
      m_pLogPlayer->SetPlayPosHead();
      m_isLogPlaying = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの再生を終了する
     */
    //-------------------------------------------------------------------------
    void Device::StopLogPlay( void )
    {
      ICHI_PRINT("Device::StopLogPlay\n");
      m_isLogPlaying = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログの再生中かどうかを調べる
     * @retval true  ログ再生中
     * @retval false ログ再生中でない
     */
    //-------------------------------------------------------------------------
    bool Device::IsLogPlaying( void ) const
    {
      return m_isLogPlaying;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログのループ再生が有効か？
     * @retval true   ループ再生する
     * @retval false  ループ再生しない
     */
    //-------------------------------------------------------------------------
    bool Device::IsLogLoopPlayEnable( void ) const
    {
      return m_isLogLoopPlayEnable;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ログのループ再生が有効かどうか？設定する
     * @param isLoopEnable  true:ループ再生する, false:ループ再生しない
     *
     * @note デフォルト値は false です
     */
    //-------------------------------------------------------------------------
    void Device::SetLogLoopPlayEnable( bool isLoopEnable )
    {
      m_isLogLoopPlayEnable = isLoopEnable;
    }

    //-------------------------------------------------------------------
    /**
     *	@brief ログのバッファを取得 
     */
    //--------------------------------------------------------------------
    void* Device::GetLogBuffer( void ) const
    {
      return m_pLogPlayer->GetLogBuffer();
    }

    //-------------------------------------------------------------------
    /**
     *	@brief ログの最大バッファサイズを取得 
     */
    //--------------------------------------------------------------------
    u32 Device::GetLogBufferMaxSize( void ) const
    {
      return ( m_pImplementer->GetDetectiveDataSize() * m_pLogRecorder->GetRecordableLogDataNum() );
    }

    //--------------------------------------------------------------------
    /**
     * @brief データの記録位置を取得する
     *
     * @return データの記録位置
     *
     */
    //--------------------------------------------------------------------
    u32 Device::GetLogRecPos( void ) const
    {
      return m_pLogRecorder->GetLogDataNum();
    }
    
    //--------------------------------------------------------------------
    /**
     * @brief データの記録位置を設定する
     *
     * @param recPos  データの記録位置
     *
     * @note recPos 以降のデータは失われます
     */
    //--------------------------------------------------------------------
    void Device::SetLogRecPos( u32 recPos )
    {
      m_pLogRecorder->SetLogRecPos( recPos );
    }

    //--------------------------------------------------------------------
    /**
     * @brief データの記録最大位置を取得する
     *
     * @return データの記録最大位置
     *
     */
    //--------------------------------------------------------------------
    u32 Device::GetLogRecMaxPos( void ) const
    {
      return m_pLogRecorder->GetRecordableLogDataNum();
    }


    //-------------------------------------------------------------------------
    /**
     * @brief リピート入力の検出パラメータをデフォルト値にリセットする
     */
    //-------------------------------------------------------------------------
    void Device::SetDefaultRepeatParam( void )
    {
      RepeatParam default_param = { DEFAULT_REPEAT_START_WAIT, DEFAULT_REPEAT_INTERVAL };
      this->SetRepeatParam( default_param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピート入力の検出パラメータを設定する
     * @param param 設定する検出パラメータ
     */
    //-------------------------------------------------------------------------
    void Device::SetRepeatParam( const RepeatParam& param )
    {
      m_pRepeatTimer->SetRepeatParam( param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピート入力の検出パラメータを取得する
     * @param[out] p_param 検出パラメータの格納先
     */
    //-------------------------------------------------------------------------
    void Device::GetRepeatParam( RepeatParam* p_param ) const
    {
      m_pRepeatTimer->GetRepeatParam( p_param );
    } 

#if GFL_DEBUG
    //----------------------------------------------------------------------------
    /**
     *	@brief  デバッグキー　無効化設定
     */
    //-----------------------------------------------------------------------------
    void Device::SetDebugKeyDisable( bool is_disable )
    {
      mDebugKeyDisable = is_disable;
    }
    bool Device::IsDebugKeyDisable( void ) const
    {
      return mDebugKeyDisable;
    }
#endif // GFL_DEBUG



    //-------------------------------------------------------------------------
    /**
     * @brief デバイス更新処理
     */
    //-------------------------------------------------------------------------
    void Device::UpdateDevice( void )
    {
      m_frameCount++;
      m_pImplementer->Main(); 

      this->UpdateRawEffectiveData(); 


      this->CopyNowDetectiveData_ToPrevDetectiveData();

      if( this->IsLogPlaying() ) 
      {
        this->UpdateNowDetectiveData_ByLogPlayer();
        m_pLogPlayer->IncPlayPos();
        if( m_pLogPlayer->IsPlayEnd() ) {
          if( this->IsLogLoopPlayEnable() ) {
            this->StartLogPlay();
          }
          else {
            this->StopLogPlay();
          }
        }
      }
      else 
      {
        this->UpdateNowDetectiveData_ByImplementer();
      }

      if( this->IsLogRecording() ) 
      {
        if( m_pLogRecorder->CanRec() ) {
          m_pLogRecorder->RecCurrentInput();
        }
        else {
          this->StopLogRec();
        }
      }

      m_pRepeatTimer->Update(); 
      if( this->IsRepeatingSameInput() == false ) {
        m_pRepeatTimer->Reset();
      }

      this->UpdateEffectiveDataBufferByLastDetectiveData();
      if( this->IsOnEffectiveDataUpdateFrame() ) {
        this->CopyEffectiveDataBuffer_ToEffectiveData();
        this->ClearEffectiveDataBuffer();
      }
    }

    /**
     * @brief 加工されていないデバイス入力データを更新する
     */
    void Device::UpdateRawEffectiveData( void )
    {
      const u32 detective_datasize = m_pImplementer->GetDetectiveDataSize();
      gfl::std::MemCopy( m_nowRawDetectiveData, m_prevRawDetectiveData, detective_datasize );
      m_pImplementer->GetDetectiveData( m_nowRawDetectiveData );
      this->StoreEffectiveData( m_rawEffectiveDataBuffer, m_nowRawDetectiveData, m_prevRawDetectiveData );
      if( this->IsOnEffectiveDataUpdateFrame() ) {
        const u32 effective_datasize = m_pImplementer->GetEffectiveDataSize();
        gfl::std::MemCopy( m_rawEffectiveDataBuffer, m_rawEffectiveData, effective_datasize );
        gfl::std::MemClear( m_rawEffectiveDataBuffer, effective_datasize );
      }
    }

    /**
     * @brief 現在の検出データを, 前回の検出データにコピーする
     */
    void Device::CopyNowDetectiveData_ToPrevDetectiveData( void )
    {
      u32 data_size = m_pImplementer->GetDetectiveDataSize();
      gfl::std::MemCopy( m_nowDetectiveData, m_prevDetectiveData, data_size );
    }

    /**
     * @brief ログの再生により, 現在の検出データを更新する
     */
    void Device::UpdateNowDetectiveData_ByLogPlayer( void )
    {
      m_pLogPlayer->GetLogData( m_nowDetectiveData );
    }

    /**
     * @brief デバイスにより, 現在の検出データを更新する
     */
    void Device::UpdateNowDetectiveData_ByImplementer( void )
    {
      m_pImplementer->GetDetectiveData( m_nowDetectiveData );
    }

    /**
     * @brief 現在の検出データと前回の検出データを比較し, 同じデータが続いているかどうかを調べる
     */
    bool Device::IsRepeatingSameInput( void ) const
    {
      u32 data_size = m_pImplementer->GetDetectiveDataSize(); 
      return (gfl::std::MemComp( m_nowDetectiveData, m_prevDetectiveData, data_size) == 0); // ビット単位で同じなら, リピートとする
    } 

    /**
     * @brief 最新の検出データにより, 実効値バッファを更新する
     */
    void Device::UpdateEffectiveDataBufferByLastDetectiveData( void )
    {
      this->StoreEffectiveData( m_effectiveDataBuffer, m_nowDetectiveData, m_prevDetectiveData );
    }

    // 各フレームレートに対する, 入力状態更新間隔
    u8 Device::m_inputUpdateInterval[ Device::FRAME_RATE_NUM ] = 
    {
      1,  // FRAME_RATE_60
      2,  // FRAME_RATE_30
    }; 

    /**
     * @brief 現在のフレームが実効データを更新するタイミングかどうかを調べる
     */
    bool Device::IsOnEffectiveDataUpdateFrame( void ) const
    {
      u8 interval = m_inputUpdateInterval[ m_frameRate ];
      return ( m_frameCount % interval == 0 );
    }

    /**
     * @brief 実効値バッファを実効データにコピーする
     */
    void Device::CopyEffectiveDataBuffer_ToEffectiveData( void )
    {
      u32 data_size = m_pImplementer->GetEffectiveDataSize();
      gfl::std::MemCopy( m_effectiveDataBuffer, m_effectiveData, data_size );
    }

    /**
     * @brief 実効値バッファをクリアする
     */
    void Device::ClearEffectiveDataBuffer( void )
    {
      u32 data_size = m_pImplementer->GetEffectiveDataSize();
      gfl::std::MemClear( m_effectiveDataBuffer, data_size );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief デバイスが稼働状態にあるか？
     * @retval true   デバイスは稼働している
     * @retval false  デバイスは稼働していない
     */
    //-------------------------------------------------------------------------
    bool Device::IsDeviceRunning( void ) const
    {
      return m_isRunning;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デバイスの稼働状態を変更する
     * @param isEnable  true:デバイスを稼働する, false:デバイスの挙動を停止する
     */
    //-------------------------------------------------------------------------
    void Device::SetDeviceRunningEnable( bool isEnable )
    {
      m_isRunning = isEnable;
    }



    //-----------------------------------------------------------------------
    /**
     * @brief デバイス入力に対する全ての制御の On/Off を切り替える
     * @param isEnable  true:入力値の制御を有効にする, false:入力値の制御を無効にする
     *
     * @note isEnable に false を指定した場合, 
     *       ( いかなる他のメソッドの影響も受けていない )ハードウェア入力を返します
     *
     * @note このメソッドから制御が戻った時点から有効になります
     */
    //-----------------------------------------------------------------------
    void Device::SetSoftwareControlEnable( bool isEnable )
    {
      m_isSoftwareControlEnable = isEnable;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief デバイス入力に対する制御が有効か？
     * @retval true   入力値に対する制御が有効
     * @retval false  入力値に対する制御が無効
     */
    //-----------------------------------------------------------------------
    bool Device::IsSoftwareControlEnable( void ) const
    {
      return m_isSoftwareControlEnable;
    }





    /**
     * @brief 参照すべき実効データを取得する
     */
    const void* Device::GetRunningEffectiveData( void ) const
    {
#if GFL_DEBUG
      if( this->IsDebugKeyDisable() ) {
        return m_invalidEffectiveData;
      }
#endif 
      if( this->IsDeviceRunning() == false ) {
        return m_invalidEffectiveData;
      }

      if( this->IsSoftwareControlEnable() == false ) {
        return m_rawEffectiveData;
      }

      return m_effectiveData;
    }




  } //namespace ui
} //namespace gfl
