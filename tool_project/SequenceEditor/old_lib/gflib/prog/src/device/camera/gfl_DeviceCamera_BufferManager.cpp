/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_BufferManager.cpp
 * @brief  �J�������g�p����o�b�t�@�̊Ǘ��N���X
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <gfl_Debug.h>
#include <gfl_Heap.h>
#include "gfl_DeviceCamera_BufferManager.h"

namespace gfl {
  namespace device { 
    namespace camera {


      //---------------------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
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
       * @brief YUV �o�b�t�@���m�ۂ���
       */
      void BufferManager::AllocYuvBuffers( const SetupParam& param )
      {
        GFL_ASSERT( m_yuvBuffers == NULL );  //@check
        GFL_ASSERT( 0 < param.yuvBufferNum );  //@check

        m_yuvBuffers = GFL_NEW(param.pHeap) void*[ param.yuvBufferNum ]; 
        for( int i=0; i<param.yuvBufferNum; i++ ) 
        {
          // 64�o�C�g�ȏ�̃A���C�������g�łȂ��Ɠ]�������ɒx����������
          m_yuvBuffers[i] = GflHeapAllocMemoryAlign( param.pHeap, param.yuvBufferSize, 64 );
          gfl::std::MemClear(m_yuvBuffers[i], param.yuvBufferSize);
        }
        m_yuvBufferNum  = param.yuvBufferNum;
        m_yuvBufferSize = param.yuvBufferSize;
      }

      /**
       * @brief RGB �o�b�t�@���m�ۂ���
       */
      void BufferManager::AllocRgbBuffers( const SetupParam& param )
      { 
        for( int i=0; i<RGB_BUFFER_NUM; i++ )
        {
          // 64�o�C�g�ȏ�̃A���C�������g�łȂ��Ɠ]�������ɒx����������
          m_rgbBuffers[i] = GflHeapAllocMemoryAlign( param.pHeap, param.rgbBufferSize, 64 );
          gfl::std::MemClear(m_rgbBuffers[i], param.yuvBufferSize);
        }
        m_rgbBufferSize = param.rgbBufferSize;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //---------------------------------------------------------------------------------
      BufferManager::~BufferManager()
      {
        m_criticalSectionForYuvBufferSwitch.Finalize();
        this->FreeYuvBuffers();
        this->FreeRgbBuffers();
      }

      /**
       * @brief YUV �o�b�t�@���������
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
       * @brief RGB �o�b�t�@���������
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
       * @brief ��M���� YUV �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetYuvBuffer_Receiving( void ) const
      {
        return m_yuvBuffers[ m_yuvReceivingBufIdx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief �ŐV�� YUV �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetYuvBuffer_Latest( void ) const
      {
        return m_yuvBuffers[ m_yuvLatestBufIdx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief �ǂݎ�蒆�� YUV �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetYuvBuffer_Reading( void ) const
      {
        return m_yuvBuffers[ m_yuvReadingBufIdx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief ��M���� RGB �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetRgbBuffer_Receiving( void ) const
      {
        u8 idx = ( m_rgbLatestBufIdx == 0 ) ? 1 : 0;
        return m_rgbBuffers[ idx ];
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief �ŐV�� RGB �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      void* BufferManager::GetRgbBuffer_Latest( void )
      {
        return m_rgbBuffers[ m_rgbLatestBufIdx ];
      } 

      //---------------------------------------------------------------------------------
      /**
       * @brief �ŐV�� RGB �o�b�t�@���擾����
       */
      //---------------------------------------------------------------------------------
      const void* BufferManager::GetRgbBuffer_Latest( void ) const
      {
        return m_rgbBuffers[ m_rgbLatestBufIdx ];
      } 


      //---------------------------------------------------------------------------------
      /**
       * @brief ��M���� YUV �o�b�t�@��؂�ւ���
       */
      //---------------------------------------------------------------------------------
      void BufferManager::SwitchReceivingYuvBuffer( void )
      {
        // �ǂݎ��o�b�t�@�̐؂�ւ��ƏՓ˂��Ȃ��悤�Ƀu���b�N����
        nn::os::CriticalSection::ScopedLock lock( m_criticalSectionForYuvBufferSwitch );

        m_yuvLatestBufIdx = m_yuvReceivingBufIdx;

        do {
          m_yuvReceivingBufIdx = ( m_yuvReceivingBufIdx + 1 ) % m_yuvBufferNum;
        } while( m_yuvReceivingBufIdx == m_yuvReadingBufIdx );
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief �ǂݎ�� YUV �o�b�t�@��؂�ւ���
       */
      //---------------------------------------------------------------------------------
      void BufferManager::SwitchReadingYuvBuffer( void )
      {
        // ��M�o�b�t�@�̐؂�ւ��ƏՓ˂��Ȃ��悤�Ƀu���b�N����
        nn::os::CriticalSection::ScopedLock lock( m_criticalSectionForYuvBufferSwitch );

        m_yuvReadingBufIdx = m_yuvLatestBufIdx;
      }

      //---------------------------------------------------------------------------------
      /**
       * @brief RGB �o�b�t�@�����ւ���
       */
      //---------------------------------------------------------------------------------
      void BufferManager::SwapRgbBuffers( void )
      { 
        m_rgbLatestBufIdx ^= 1;
      }


    } // namespace camera
  } // namespace device
} // namespace gfl 
