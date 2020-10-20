/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_Y2rThread.cpp
 * @brief  YUB ==> RGB �ϊ��X���b�h
 * @author obata_toshihiro
 * @date   2010.11.27
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <nn.h>
#include <nn/y2r.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>
#include <gfl_Debug.h>
#include "gfl_DeviceCamera_BufferManager.h"
#include "gfl_DeviceCamera_Y2rThread.h"


namespace gfl {
  namespace device { 
    namespace camera { 


      //---------------------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //---------------------------------------------------------------------------------
      Y2rThread::Y2rThread( const SetupParam& param ) :
        Thread( param.pHeap, 4096 ),
        m_y2rTransferEndEvent(),
        m_threadEndEvent(),
        m_errorBitFlag(param.errorBitFlag),
        m_threadEndFlag(false)
      {
        m_threadEndEvent.Initialize( false );
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::GetTransferEndEvent( &m_y2rTransferEndEvent );
        }
        else
        {
          m_y2rTransferEndEvent.Initialize( false );
        }

        for( int i=0; i<2; i++ )
        {
          this->CalcConvertSettings( param.convertParam[i], &m_convertSetting[i] ); 
        }

        this->InitY2r();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //---------------------------------------------------------------------------------
      Y2rThread::~Y2rThread() 
      {
        m_threadEndEvent.Finalize();
        m_y2rTransferEndEvent.Finalize();
      }



      /**
       * @brief �ϊ��p�����[�^����, �R���o�[�g�̐ݒ���Z�o����
       */
      void Y2rThread::CalcConvertSettings( const ConvertParam& param, ConvertSetting* pSetting )
      {
        pSetting->pConvertStartEvent = param.pConvertStartEvent;
        pSetting->pBuffers = param.pBuffers;
        
        // �g���~���O��̉摜�T�C�Y
        pSetting->trimmingWidth = param.trimmingWidth;
        pSetting->trimmingHeight = param.trimmingHeight;
         
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // ���͉摜�̃T�C�Y
          pSetting->inputImageSize = 
            nn::camera::GetFrameBytes( param.trimmingWidth, param.trimmingHeight );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
         
          // ���͉摜��1���C���T�C�Y
          pSetting->inputLineSize = nn::camera::GetLineBytes( param.trimmingWidth );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        }
        else
        {
          pSetting->inputImageSize = 0;
          pSetting->inputLineSize = 0;
        }

        // RGB �o�b�t�@�̃t�H�[�}�b�g
        pSetting->outputFormat = param.outputFormat;
        
        // RGB �f�[�^�̕��я�
        pSetting->outputBlockAlignment = param.outputBlockAlignment;
        
        // �o�͉摜�̃A���t�@�l
        pSetting->outputAlpha = param.outputAlpha;
         
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // �o�͉摜�̃T�C�Y
          pSetting->outputImageSize = 
            nn::y2r::GetOutputImageSize(  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
                param.trimmingWidth, param.trimmingHeight, param.outputFormat );
         
          // �o�̓o�b�t�@��1�u���b�N�T�C�Y
          pSetting->outputBlockSize = 
            nn::y2r::GetOutputBlockSize( param.trimmingWidth, param.outputFormat );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
        }
        else
        {
          pSetting->outputImageSize = 0;
          pSetting->outputBlockSize = 0;
        }

        // �o�͐�̃I�t�Z�b�g�l
        pSetting->outputOffset = 0; // �e�N�X�`���̍���ɔz�u
         
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // 1��̓]�����ɉ��Z�����I�t�Z�b�g�l
          int margin_pixel_num = param.outputWidth - param.trimmingWidth;
          int margin_block_num = margin_pixel_num / 8;
          int block_size = PIXEL_PER_BLOCK * nn::y2r::GetOutputFormatBytes( param.outputFormat );  // Initialize���Ă��Ȃ��Ǝg���Ȃ�  //sango,2014/06/10,kawada_koji,m_errorBitFlag
          pSetting->outputTransferStride = margin_block_num * block_size;
        }
        else
        {
          pSetting->outputTransferStride = 0;
        }
      }


      /**
       * @brief Y2R �̏�����
       */
      void Y2rThread::InitY2r( void )
      { 
        // ���쒆�̃R���o�[�g�������I��
        this->StopConversionAndWait();

        // ���̓t�H�[�}�b�g��ݒ�
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::SetInputFormat( nn::y2r::INPUT_YUV422_BATCH ); // �J��������̓��͂̓t�H�[�}�b�g�Œ�
        }

        // Y2R �ϊ��̊����ʒm���󂯂�
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::SetTransferEndInterrupt( true );
        }

        // �ϊ��W����ݒ�
        nn::y2r::StandardCoefficient coefficient;
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::Result result = nn::camera::GetSuitableY2rStandardCoefficient( &coefficient );
          if(result == nn::camera::ResultFatalError()) {
            //GFL_ASSERT_STOP(0); //@fix  // �Q�[�����~�߂����Ȃ��̂ŃR�����g�A�E�g�B�G���[�����̂܂܏����𑱂���悤�ɂ���B  //sango,2014/06/10,kawada_koji,m_errorBitFlag
            *m_errorBitFlag |= ERROR_BIT_CAMERA_GETSUITABLEY2RSTANDARDCOEFFICIENT_FATAL_ERROR;
          }
          if(*m_errorBitFlag==ERROR_BIT_NONE)
          {
#ifdef BUGFIX_BTS6963_20130530
            while( result == nn::camera::CTR::ResultIsSleeping() ) {
              //NTSC���100milli�͖ڈ�
              nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(100));
              result = nn::camera::GetSuitableY2rStandardCoefficient( &coefficient );
            }
#endif
          }
        }

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::SetStandardCoefficient( coefficient );
        }
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h����������
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Initialize( void )
      {
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h�I������
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Finalize( void )
      { 
        this->StopConversionAndWait();
      }


      /**
       * @brief ���쒆�̃R���o�[�g�̒�~������, ��~����������̂�҂�
       */
      void Y2rThread::StopConversionAndWait( void )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          nn::y2r::StopConversion();
         
          while( nn::y2r::IsBusyConversion() )
          {
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
          }
        }
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h�j�����̏���
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::OnKill( void )
      {
        m_threadEndEvent.Signal();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �X���b�h���C������
       */
      //---------------------------------------------------------------------------------
      void Y2rThread::Main( void )
      {
        if( m_threadEndFlag )
        {
          nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 50 ) );  // SBTS[2574]: �y�Ƃ܂�FQR�R�[�h�ǂݎ���ʂŃJ�����@�\����~���邱�Ƃ�����A���̏�ԂŖ߂낤�Ƃ���ƂƂ܂�z  //sango,2014/06/25,kawada_koji
        }
        else
        {
          nn::os::Event* p_events[3] =
          {
            m_convertSetting[0].pConvertStartEvent, // �ϊ��J�n�C�x���g
            m_convertSetting[1].pConvertStartEvent, // �ϊ��J�n�C�x���g
            &m_threadEndEvent,       // �X���b�h�I���C�x���g
          };
         
          s32 event_idx = nn::os::WaitObject::WaitAny( 
              reinterpret_cast<nn::os::WaitObject**>(p_events), 3 );
         
         
          if( ( event_idx == 0 ) || ( event_idx == 1 ) ) // �ϊ��J�n�C�x���g
          {
            // ���E�̂����A��Ɉ˗��������ق����� Y2R �ϊ����J�n
            this->SetConvertSettings( m_convertSetting[ event_idx ] );
            this->ConvertY2R( &m_convertSetting[ event_idx ] );
         
            // ��������̃J�����摜���ϊ�
            s32 another = ( event_idx == 0 ) ? 1 : 0;
            if( p_events[ another ]->Wait(0) )
            {
              this->SetConvertSettings( m_convertSetting[ another ] );
              this->ConvertY2R( &m_convertSetting[ another ] );
            }
          }
          else if( event_idx == 2 ) // �X���b�h�I���C�x���g
          {
            this->StopConversionAndWait();
            m_threadEndFlag = true;
          }
        }
      } 


      /**
       * @brief �ϊ��̐ݒ���X�V����
       */
      void Y2rThread::SetConvertSettings( const ConvertSetting& setting )
      {
        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          // �o�̓t�H�[�}�b�g��ݒ�
          nn::y2r::SetOutputFormat( setting.outputFormat );
         
          // �o�̓f�[�^�̉�]�p�x��ݒ�
          nn::y2r::SetRotation( nn::y2r::ROTATION_NONE );
         
          // �o�̓f�[�^�̕��я���ݒ�
          nn::y2r::SetBlockAlignment( setting.outputBlockAlignment );
         
          // Y2R �ϊ��̓��͂ƂȂ�摜�̃T�C�Y��ݒ�
          nn::y2r::SetInputLineWidth( setting.trimmingWidth );
          nn::y2r::SetInputLines( setting.trimmingHeight );
         
          // �o�͉摜�̃A���t�@�l��ݒ�
          nn::y2r::SetAlpha( setting.outputAlpha );
        }
      }


      /** 
       * @brief YUV ==> RGB �ϊ������s����
       */
      void Y2rThread::ConvertY2R( ConvertSetting* pSetting )
      {
        // �J�����̓ǂݏo���o�b�t�@��؂�ւ��܂��B
        pSetting->pBuffers->SwitchReadingYuvBuffer();

        bool transfer_finished = false;
        s32 try_count = 0;

        if(*m_errorBitFlag==ERROR_BIT_NONE)
        {
          do { 
         
            // ���͉摜(YUV)�̓]���ݒ�
            nn::y2r::CTR::SetSendingYuv(
                pSetting->pBuffers->GetYuvBuffer_Reading(),
                pSetting->inputImageSize,
                pSetting->inputLineSize );
         
            // �o�͉摜(RGB)�̓]���ݒ�
            nn::y2r::CTR::SetReceiving(
                reinterpret_cast<u8*>( pSetting->pBuffers->GetRgbBuffer_Receiving() ) + 
                pSetting->outputOffset,
                pSetting->outputImageSize,
                pSetting->outputBlockSize,
                pSetting->outputTransferStride );
         
            // Y2R�ϊ����J�n
            nn::y2r::CTR::StartConversion();
         
            // Y2R�ϊ��̊�����҂�
            // Y2R�n�[�h�E�F�A�̕s��΍�̂��߂Ƀ^�C���A�E�g��ݒ� ==> �^�C���A�E�g���̓��g���C����
            // @todo �^�C���A�E�g���Ԃ��ςɂ���
            transfer_finished = m_y2rTransferEndEvent.Wait( nn::fnd::TimeSpan::FromMilliSeconds(20) );
            try_count++;
         
         
            if( transfer_finished ) 
            {
              pSetting->pBuffers->SwapRgbBuffers(); // RGB �o�b�t�@��؂�ւ���
            }
            else 
            {
              GFL_PRINT( "Y2R TIMEOUT: Y2R may have hung up.\n" );
              nn::y2r::StopConversion(); // �ϊ��������I������
         
              // ���x�����s�����ꍇ��, �ϊ���������߂Ď��֐i��
              if( CONVERT_RETRY_NUM < try_count ) {
                transfer_finished = true;
              }
            } 
         
          } while ( transfer_finished == false );
        }

      }


    } // namespace camera 
  } // namespace device 
} // namespace gfl
