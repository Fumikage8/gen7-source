#ifndef __gfl_DeviceCamera_Y2rThread__
#define __gfl_DeviceCamera_Y2rThread__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_Y2rThread.h
 * @brief  YUB ==> RGB �ϊ��X���b�h
 * @author obata_toshihiro
 * @date   2010.11.27
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <gfl_Macros.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>
#include <device/camera/gfl_DeviceCamera_Types.h>

namespace gfl {
  namespace device { 
    namespace camera { 

      class BufferManager;


      class Y2rThread : public gfl::base::Thread 
      {
        GFL_FORBID_COPY_AND_ASSIGN( Y2rThread ); // �R�s�[�R���X�g���N�^, ������Z�q���֎~


        public:
        // �R���o�[�g�P�ʂ̐ݒ�
        struct ConvertParam {
          nn::os::Event*          pConvertStartEvent;   // �R���o�[�g�J�n���ʒm�����C�x���g
          BufferManager*          pBuffers;             // �J�����̃f�[�^�o�b�t�@
          s16                     trimmingWidth;        // �g���~���O��̉摜�̉���
          s16                     trimmingHeight;       // �g���~���O��̉摜�̏c��
          s16                     outputWidth;          // �o�͉摜�̕�
          s16                     outputHeight;         // �o�͉摜�̍���
          nn::y2r::OutputFormat   outputFormat;         // RGB �o�b�t�@�̃t�H�[�}�b�g
          nn::y2r::BlockAlignment outputBlockAlignment; // RGB �f�[�^�̕��я�
          s16                     outputAlpha;          // �o�͉摜�̃A���t�@�l ( �o�̓t�H�[�}�b�g�� 16bit or 32bit �̏ꍇ�ɂ̂ݗL�� )
          s32                     outputOffset;         // RGB �f�[�^�i�[��̃I�t�Z�b�g
        };

        // �Z�b�g�A�b�v�p�����[�^
        struct SetupParam {
          gfl::heap::HeapBase* pHeap;                      // �g�p����q�[�v
          ConvertParam         convertParam[ CAMERA_NUM ]; // �e�R���o�[�g�̐ݒ�
          u32*                 errorBitFlag;  // ERROR_BIT_????
        }; 
        //-------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //-------------------------------------------------------------------------------
        Y2rThread( const SetupParam& param );

        //-------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //-------------------------------------------------------------------------------
        virtual ~Y2rThread(); 


        private:
        // �X���b�h����
        virtual void Initialize( void ); // ����������
        virtual void Finalize( void );   // �I������
        virtual void Main( void );       // ���C������
        virtual void OnKill( void );     // �X���b�h�j�����̏���


        protected:
        nn::os::Event  m_y2rTransferEndEvent;   // �f�[�^�]���̊������ʒm�����C�x���g
        nn::os::Event  m_threadEndEvent;        // �X���b�h���I�������邽�߂̃C�x���g
        
        u32* m_errorBitFlag;  // ERROR_BIT_????
        bool m_threadEndFlag;  // SBTS[2574]: �y�Ƃ܂�FQR�R�[�h�ǂݎ���ʂŃJ�����@�\����~���邱�Ƃ�����A���̏�ԂŖ߂낤�Ƃ���ƂƂ܂�z  //sango,2014/06/25,kawada_koji

        // �R���o�[�g�̐ݒ�
        struct ConvertSetting {
          nn::os::Event*          pConvertStartEvent;   // �ϊ��J�n���˗������C�x���g
          BufferManager*          pBuffers;             // �J�����̃f�[�^�o�b�t�@
          s16                     trimmingWidth;        // �g���~���O��̉摜�̉���
          s16                     trimmingHeight;       // �g���~���O��̉摜�̏c��
          size_t                  inputImageSize;       // ���͉摜�̃T�C�Y
          s16                     inputLineSize;        // ���͉摜��1���C���T�C�Y
          nn::y2r::OutputFormat   outputFormat;         // RGB �o�b�t�@�̃t�H�[�}�b�g
          nn::y2r::BlockAlignment outputBlockAlignment; // RGB �f�[�^�̕��я�
          s16                     outputAlpha;          // �o�͉摜�̃A���t�@�l ( �o�̓t�H�[�}�b�g�� 16bit or 32bit �̏ꍇ�ɂ̂ݗL�� )
          size_t                  outputImageSize;      // �o�͉摜�̃T�C�Y
          s16                     outputBlockSize;      // �o�̓o�b�t�@��1�u���b�N�T�C�Y
          s16                     outputOffset;         // �o�͐�̐擪�I�t�Z�b�g�l
          s16                     outputTransferStride; // 1��̓]�����ɉ��Z�����I�t�Z�b�g�l
        };
        ConvertSetting m_convertSetting[ CAMERA_NUM ];


        private:
        // Y2R �̏�����
        void InitY2r( void );

        // �ϊ��p�����[�^����, �R���o�[�g�̐ݒ���Z�o����
        void CalcConvertSettings( const ConvertParam& param, ConvertSetting* pSetting );

        // ���쒆�̃R���o�[�g�̒�~������, ��~����������̂�҂�
        void StopConversionAndWait( void );

        // �ϊ��̐ݒ���X�V����
        void SetConvertSettings( const ConvertSetting& setting );

        // YUV ==> RGB �ϊ������s����
        void ConvertY2R( ConvertSetting* pSetting );


        private:
        // Y2R�ϊ����s���Ƀ��g���C�����
        static const u8 CONVERT_RETRY_NUM = 1;

        // 1�u���b�N���̃s�N�Z����
        static const u8 PIXEL_PER_BLOCK = 8 * 8;
      };

    } // namespace camera 
  } // namespace device 
} // namespace gfl
#endif
