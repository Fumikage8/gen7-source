#ifndef __gfl_DeviceCamera_BufferManager__
#define __gfl_DeviceCamera_BufferManager__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_BufferManager.h
 * @brief  �J�������g�p����o�b�t�@�̊Ǘ��N���X
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h> // for GFL_FORBID_COPY_AND_ASSIGN

namespace gfl {
  namespace device { 
    namespace camera {

      class BufferManager 
      {
        GFL_FORBID_COPY_AND_ASSIGN( BufferManager ); // �R�s�[�R���X�g���N�^, ������Z�q���֎~

        public:
        /**
         * @brief �Z�b�g�A�b�v�p�����[�^
         */
        struct SetupParam {
          gfl::heap::HeapBase* pHeap;         // �o�b�t�@�̊m�ۂɎg�p����q�[�v
          u8                   yuvBufferNum;  // YUV �o�b�t�@�̐�
          u32                  yuvBufferSize; // YUV �o�b�t�@�̃T�C�Y
          u32                  rgbBufferSize; // RGB �o�b�t�@�̃T�C�Y
        }; 
        BufferManager( const SetupParam& param ); 
        virtual ~BufferManager(); 


        public:
        // �o�b�t�@�擾
        void* GetYuvBuffer_Receiving( void ) const; // ��M���� YUV �o�b�t�@
        void* GetYuvBuffer_Latest( void ) const;    // �ŐV�� YUV �o�b�t�@
        void* GetYuvBuffer_Reading( void ) const;   // �ǂݎ�蒆�� YUV �o�b�t�@
        void* GetRgbBuffer_Receiving( void ) const; // ��M���� RGB �o�b�t�@
        void* GetRgbBuffer_Latest( void ); // �ŐV�� RGB �o�b�t�@
        const void* GetRgbBuffer_Latest( void ) const; // �ŐV�� RGB �o�b�t�@

        // �o�b�t�@�T�C�Y�擾
        inline u32 GetYuvBufferSize( void ) const { return m_yuvBufferSize; }
        inline u32 GetRgbBufferSize( void ) const { return m_rgbBufferSize; }

        // �o�b�t�@�̐؂�ւ�
        void SwitchReceivingYuvBuffer( void ); // ��M���� YUV �o�b�t�@��؂�ւ���
        void SwitchReadingYuvBuffer( void );   // �ǂݎ�� YUV �o�b�t�@��؂�ւ���
        void SwapRgbBuffers( void );           // RGB �o�b�t�@�����ւ���


        protected: 
        u8 m_yuvBufferNum;       // YUV �o�b�t�@�̐�
        void** m_yuvBuffers;     // YUV �o�b�t�@�z��
        u32 m_yuvBufferSize;     // YUV �o�b�t�@�̃T�C�Y
        u8 m_yuvReceivingBufIdx; // ��M���� YUV �o�b�t�@�̃C���f�b�N�X
        u8 m_yuvLatestBufIdx;    // �ŐV�� YUV �o�b�t�@�̃C���f�b�N�X
        u8 m_yuvReadingBufIdx;   // �ǂݎ�蒆�� YUV �o�b�t�@�̃C���f�b�N�X

        static const u8 RGB_BUFFER_NUM = 2;   // RGB �o�b�t�@�̐�
        void* m_rgbBuffers[ RGB_BUFFER_NUM ]; // RGB �o�b�t�@�z��
        u32 m_rgbBufferSize;                  // RGB �o�b�t�@�̃T�C�Y
        u8 m_rgbLatestBufIdx;                 // �ŐV�� RGB �o�b�t�@�̃C���f�b�N�X

        // YUV �o�b�t�@�؂�ւ����̃N���e�B�J���Z�N�V����
        // ��M�o�b�t�@�Ɠǂݎ��o�b�t�@�̐؂�ւ��������ɍs�����,
        // ��M, �ǂݎ�肪�����o�b�t�@�ɑ΂��čs����댯�������邽��,
        // YUV �o�b�t�@�̐؂�ւ��͔r���I�Ɏ��s����.
        nn::os::CriticalSection m_criticalSectionForYuvBufferSwitch; 


        private: 
        void AllocYuvBuffers( const SetupParam& param );
        void AllocRgbBuffers( const SetupParam& param );
        void FreeYuvBuffers( void );
        void FreeRgbBuffers( void );
      }; 

    } // namespace camera
  } // namespace device
} // namespace gfl
#endif
