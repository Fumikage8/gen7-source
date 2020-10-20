#if GF_PLATFORM_CTR
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_BufferManager.cpp
 * @brief  カメラが使用するバッファの管理クラス
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_assert.h>
#include <heap/include/gfl2_Heap.h>
#include "gfl2_DeviceCamera_BufferManager.h"
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)


      //---------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //---------------------------------------------------------------------------------
      BufferManager::BufferManager( const SetupParam& param ) : 
        m_yuvBufferNum( 0 ),
        m_yuvBuffers( NULL ),
        m_yuvReceivingBufIdx( 0 ),
        m_yuvLatestBufIdx( 2 ),
        m_yuvReadingBufIdx( 2 ),
        m_rgbBuffers(),
        m_rgbBufferSize( 0 ),
        m_rgbLatestBufIdx( 0 ),
        m_criticalSectionForYuvBufferSwitch()
      {
        m_criticalSectionForYuvBufferSwitch.Initialize();
        this->AllocYuvBuffers( param );
        this->AllocRgbBuffers( param );
      }

      /**
       * @brief YUV バッファを確保する
       */
      void BufferManager::AllocYuvBuffers( const SetupParam& param )
      {
        GFL_ASSERT( m_yuvBuffers == NULL );  //@check
        GFL_ASSERT( 0 < param.yuvBufferNum );  //@check

        m_yuvBuffers = GFL_NEW(param.pHeap) void*[ param.yuvBufferNum ]; 
        for( int i=0; i<param.yuvBufferNum; i++ ) 
        {
          // 64バイト以上のアラインメントでないと転送処理に遅延が生じる
          m_yuvBuffers[i] = GflHeapAllocMemoryAlign( param.pHeap, param.yuvBufferSize, 64 );
          gfl2::std::MemClear(m_yuvBuffers[i], param.yuvBufferSize);
        }
        m_yuvBufferNum  = param.yuvBufferNum;
        m_yuvBufferSize = param.yuvBufferSize;
      }

      /**
       * @brief RGB バッファを確保する
       */
      void BufferManager::AllocRgbBuffers( const SetupParam& param )
      { 
        for( int i=0; i<RGB_BUFFER_NUM; i++ )
        {
          // 64バイト以上のアラインメントでないと転送処理に遅延が生じる
          m_rgbBuffers[i] = GflHeapAllocMemoryAlign( param.pHeap, param.rgbBufferSize, 64 );
          gfl2::std::MemClear(m_rgbBuffers[i], param.yuvBufferSize);
        }
        m_rgbBufferSize = param.rgbBufferSize;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //---------------------------------------------------------------------------------
      BufferManager::~BufferManager()
      {
        m_criticalSectionForYuvBufferSwitch.Finalize();
        this->FreeYuvBuffers();
        this->FreeRgbBuffers();
      }

      /**
       * @brief YUV バッファを解放する
       */
      void BufferManager::FreeYuvBuffers( void )
      {
        if( m_yuvBuffers ) {
          for( int i=0; i<m_yuvBufferNum; i++ )
          {
            if( m_yuvBuffers[i] ) {
              GflHeapFreeMemory( m_yuvBuffers[i] );
            }
          }
          GFL_DELETE m_yuvBuffers;
        }
      }

      /**
       * @brief RGB バッファを解放する
       */
      void BufferManager::FreeRgbBuffers( void )
      {
        for( int i=0; i<RGB_BUFFER_NUM; i++ )
        {
          if( m_rgbBuffers[i] ) {
            GflHeapFreeMemory( m_rgbBuffers[i] );
          }
        }
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief 受信中の YUV バッファを取得する
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetYuvBuffer_Receiving( void ) const
      {
        return m_yuvBuffers[ m_yuvReceivingBufIdx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief 最新の YUV バッファを取得する
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetYuvBuffer_Latest( void ) const
      {
        return m_yuvBuffers[ m_yuvLatestBufIdx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief 読み取り中の YUV バッファを取得する
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetYuvBuffer_Reading( void ) const
      {
        return m_yuvBuffers[ m_yuvReadingBufIdx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief 受信中の RGB バッファを取得する
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetRgbBuffer_Receiving( void ) const
      {
        u8 idx = ( m_rgbLatestBufIdx == 0 ) ? 1 : 0;
        return m_rgbBuffers[ idx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief 最新の RGB バッファを取得する
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetRgbBuffer_Latest( void )
      {
        return m_rgbBuffers[ m_rgbLatestBufIdx ];
      } 

      //---------------------------------------------------------------------------------
      /**
       * @brief 最新の RGB バッファを取得する
       */
      //---------------------------------------------------------------------------------
      const void* BufferManager::GetRgbBuffer_Latest( void ) const
      {
        return m_rgbBuffers[ m_rgbLatestBufIdx ];
      } 


      //---------------------------------------------------------------------------------
      /**
       * @brief 受信する YUV バッファを切り替える
       */
      //---------------------------------------------------------------------------------
      void BufferManager::SwitchReceivingYuvBuffer( void )
      {
        // 読み取りバッファの切り替えと衝突しないようにブロックする
        nn::os::CriticalSection::ScopedLock lock( m_criticalSectionForYuvBufferSwitch );

        m_yuvLatestBufIdx = m_yuvReceivingBufIdx;

        do {
          m_yuvReceivingBufIdx = ( m_yuvReceivingBufIdx + 1 ) % m_yuvBufferNum;
        } while( m_yuvReceivingBufIdx == m_yuvReadingBufIdx );
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief 読み取る YUV バッファを切り替える
       */
      //---------------------------------------------------------------------------------
      void BufferManager::SwitchReadingYuvBuffer( void )
      {
        // 受信バッファの切り替えと衝突しないようにブロックする
        nn::os::CriticalSection::ScopedLock lock( m_criticalSectionForYuvBufferSwitch );

        m_yuvReadingBufIdx = m_yuvLatestBufIdx;
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief RGB バッファを入れ替える
       */
      //---------------------------------------------------------------------------------
      void BufferManager::SwapRgbBuffers( void )
      { 
        m_rgbLatestBufIdx ^= 1;
      }


GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
