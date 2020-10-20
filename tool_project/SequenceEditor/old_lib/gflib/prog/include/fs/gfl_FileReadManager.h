//=============================================================================
/**
 * @brief  �t�@�C���ǂݍ��݊Ǘ�
 * @file   gfl_FileReadManager.h
 * @author obata_toshihiro
 * @date   2012.01.08
 */
//=============================================================================
#ifndef __GFL_FILEREADMANAGER_H__
#define __GFL_FILEREADMANAGER_H__
#pragma once


#include <base/gfl_ThreadManager.h>
#include <base/gfl_Queue.h>
#include <fs/gfl_ArcFile.h>

namespace gfl {
  namespace fs {

    class Thread;


    /**
     * @note �ǂݍ��ݎ菇
     * 1. �ǂݍ��݂����A�[�J�C�u�� AppendManageFile() �œo�^����.
     * 2. AddReadRequest() �œǂݍ��݃��N�G�X�g�𔭍s����.
     * 3. IsReadFinished(), IsAllRequestFinished() �œǂݍ��݂̊�����҂�.
     * 4. �o�^�����A�[�J�C�u���K�v�Ȃ��Ȃ�����, RemoveManageFile() �œo�^����������.
     */
    class FileReadManager : 
      public base::ThreadManager,
      public base::IThreadManageCallback
    {
      GFL_FORBID_COPY_AND_ASSIGN( FileReadManager );

      public: 

      /**
       * @brief ���N�G�X�g�̎��s�D��x
       * @note �l�������������D��x������
       */
      static const u8 REQUEST_PRIORITY_HIGHEST =  0;
      static const u8 REQUEST_PRIORITY_NORMAL  = 16;
      static const u8 REQUEST_PRIORITY_LOWEST  = 31;


      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param heap_for_management  �C���X�^���X���m�ۂ���q�[�v
       * @param arcfile_count        �Ǘ�����A�[�J�C�u�t�@�C���̐�
       * @param max_request_count    �����Ɏ󂯕t���邱�Ƃ��ł���ǂݍ��݃��N�G�X�g�̐�
       * @param max_read_count       �����ɔ񓯊��ǂݍ��݂����s���鐔
       * @param read_thread_priority �ǂݍ��݃X���b�h�̗D�揇��
       */
      //-----------------------------------------------------------------------
      FileReadManager( 
          gfl::heap::HeapBase* heap_for_management,
          u32 arcfile_count,
          u32 max_request_count,
          u8 max_read_count,
          base::Thread::Priority read_thread_priority );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~FileReadManager();

      //-----------------------------------------------------------------------
      /**
       * @brief �X�V����
       */
      //-----------------------------------------------------------------------
      void UpdateFileRead( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �A�[�J�C�u�t�@�C�����o�^�ς݂��H
       * @param arcID �`�F�b�N����A�[�J�C�u�t�@�C����ID
       * @retval true  �o�^�ς�
       * @retval false �o�^����Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsFileManaged( ArcFile::ARCID arcID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�[�J�C�u�t�@�C����o�^����
       * @param heapForArc  ArcFile��u���q�[�v
       * @param arcID       �o�^����A�[�J�C�u�t�@�C����ID
       * @param hio         true:HIO�œǂ�, false:ROM����ǂ�
       *
       * @attention
       * heapForArc �Ɋm�ۂ���郁������ 
       * RemoveManageFile() ���ĂԂ��Ƃŉ������܂��B
       * �q�[�v�̒f�Љ��ɒ��ӂ��Ă��������B
       */
      //-----------------------------------------------------------------------
      void AppendManageFile( 
          gfl::heap::HeapBase* heapForArc,
          ArcFile::ARCID arcID,
          bool hio = false );

      //-----------------------------------------------------------------------
      /**
       * @brief �A�[�J�C�u�t�@�C�����Ǘ��Ώۂ��珜�O����
       * @param arcID �o�^����������A�[�J�C�u�t�@�C����ID
       *
       * @attention
       * arcID �ւ̓ǂݍ��݂��I�����Ă��Ȃ��ꍇ, �J�����g�X���b�h���x�~�����܂��B
       */
      //-----------------------------------------------------------------------
      void RemoveManageFile( ArcFile::ARCID arcID ); 

      //-----------------------------------------------------------------------
      /**
       * @brief �o�^����Ă���A�[�J�C�u�t�@�C�����擾����
       * @param arcID  �擾�Ώۂ̃A�[�J�C�u�t�@�C��ID
       * @retval NULL  �w�肵���A�[�J�C�u�t�@�C�����o�^����Ă��Ȃ��ꍇ
       */
      //-----------------------------------------------------------------------
      gfl::fs::ArcFile* GetManageFile( ArcFile::ARCID arcID ) const;

      //-------------------------------------------------------------------------
      /**
       * @brief �w��A�[�J�C�u��FastMode=���������[�h�Ɉڍs����(�����ǂݍ��ݔ�)
       *
       * @param  arcID  FastMode�Ɉڍs������A�[�J�C�uID
       * @param  heapForFastTable  �������e�[�u�����m�ۂ���q�[�v(NULL�w��Ȃ�AppendManageFile�ɓn�����q�[�v����m��)
       *
       * @retval true   FastMode�Ɉڍs�ł���
       * @retval false  FastMode�Ɉڍs�o���Ȃ�����(���o�^,HIT�I�v�V�������L���A����FastMode�Ȃ�)
       *
       * @note
       * AppendManageFile()�ŃA�[�J�C�u��o�^�����i�K�ł́A�t�@�C���̍������e�[�u���̓�������Ƀ��[�h����Ă��Ȃ��B
       *
       * AppendManageFile()�ƍŏ���AddReadRequest()�̔��s�ʒu������Ă���ƁA
       * �������e�[�u�����������ɏ��^�C�~���O������邽�߁A�q�[�v�̒f�Љ��v���ɂȂ肦��B
       *
       * ���̂悤�ȏꍇ��AppendManageFile()����ɁA�����I��FastMode�ւ̈ڍs�����Ă������ƂŒf�Љ���h��
       */
      //-------------------------------------------------------------------------
      bool SetupForFastMode( ArcFile::ARCID arcID, gfl::heap::HeapBase* heapForFastTable = NULL );
    
      //-------------------------------------------------------------------------
      /**
       * @brief �w��A�[�J�C�u��FastMode�ڍs���J�n����(�񓯊��ǂݍ��ݔ�)
       *
       * @param  arcID  FastMode�Ɉڍs������A�[�J�C�uID
       * @retval true  FastMode�ڍs�J�n
       * @retval false FastMode�J�n�o���Ȃ�����(���o�^,HIT�I�v�V�������L���A����FastMode�Ȃ�)
       *
       * ����ɕK��WaitSetupForFastMode�ő҂��ƁB
       *
       * �Ăяo��ArcFile::StartSetupForFastMode()��
       * ���������[�h�ڍs�ɓ����ŃX���b�h��p���邽�߁A
       * StartSetupForFastMode()�Ăяo�������WaitSetupForFastMode()�ő҂��Ȃ���
       * �������e�[�u�������A�h���X����܂�Ȃ��\��������B
       *
       * StartSetupForFastMode��false��Ԃ����ꍇ�ɁA
       * WaitSetupForFastMode()�ő҂��Ă����U��true(=�I��)���Ԃ�A���S�Ɏg����悤�ɂ��Ă���܂��B
       *
       * �������A�w�肵���A�[�J�C�u�����o�^�̏ꍇ�AGFL_ASSERT()�͔������܂��B
       *
       * @note
       * AppendManageFile()�ŃA�[�J�C�u��o�^�����i�K�ł́A�t�@�C���̍������e�[�u���̓�������Ƀ��[�h����Ă��Ȃ��B
       *
       * AppendManageFile()�ƍŏ���AddReadRequest()�̔��s�ʒu������Ă���ƁA
       * �������e�[�u�����������ɏ��^�C�~���O������邽�߁A�q�[�v�̒f�Љ��v���ɂȂ肦��B
       *
       * ���̂悤�ȏꍇ��AppendManageFile()����ɁA�����I��FastMode�ւ̈ڍs�����Ă������ƂŒf�Љ���h��
       *
       * �ꉞ�AArcFile�N���X�̓��������Ƃ��āA���������[�h�ڍs�X���b�h�������Ă���Ԃ�
       * ArcFile�N���X���j�����ꂽ�ꍇ�A�X���b�h��ArcFile�N���X�̃f�X�g���N�^�ɎE����邪
       * ��{�I�Ȏg�����Ƃ��Ă̓��N�G�X�g���o�������Wait�ő҂ׂ��ł���B
       */
      //-------------------------------------------------------------------------
       bool StartSetupForFastMode( ArcFile::ARCID arcID,
                                   gfl::heap::HeapBase* heapForThread = NULL, 
                                   gfl::heap::HeapBase* heapForFastTable = NULL );
  
      //-------------------------------------------------------------------------
      /**
       * @brief �w��A�[�J�C�u��FastMode�ڍs��҂�(�񓯊��ǂݍ��ݔ�)
       *
       * @retval true  �ڍs����
       * @retval false �܂��ڍs��
       *
       * StartSetupForFastMode()�ŊJ�n���AWaitSetupForFastMode�ő҂�
       * true ���Ԃ�܂ŌJ��Ԃ��Ă�, �ڍs�̊�����҂��Ă�������.
       *
       * �w�肵���A�[�J�C�u�����o�^�̏ꍇ�Ȃǂ́A�A�T�[�g������true���Ԃ�܂�
       *
       * @note
       * AppendManageFile()�ŃA�[�J�C�u��o�^�����i�K�ł́A�t�@�C���̍������e�[�u���̓�������Ƀ��[�h����Ă��Ȃ��B
       *
       * AppendManageFile()�ƍŏ���AddReadRequest()�̔��s�ʒu������Ă���ƁA
       * �������e�[�u�����������ɏ��^�C�~���O������邽�߁A�q�[�v�̒f�Љ��v���ɂȂ肦��B
       *
       * ���̂悤�ȏꍇ��AppendManageFile()����ɁA�����I��FastMode�ւ̈ڍs�����Ă������ƂŒf�Љ���h��
       *
       * �񓯊��ł̂�����́A�Ăяo��ArcFile::StartSetupForFastMode()��
       * ���������[�h�ڍs�ɓ����ŃX���b�h��p���邽�߁A
       * StartSetupForFastMode()�Ăяo�������WaitSetupForFastMode()�ő҂��Ȃ���
       * �������e�[�u�������A�h���X����܂�Ȃ��\��������̂Œ��ӁB
       *
       * �ꉞ�AArcFile�N���X�̓��������Ƃ��āA���������[�h�ڍs�X���b�h�������Ă���Ԃ�
       * ArcFile�N���X���j�����ꂽ�ꍇ�A�X���b�h��ArcFile�N���X�̃f�X�g���N�^�ɎE����邪
       * ��{�I�Ȏg�����Ƃ��Ă̓��N�G�X�g���o�������Wait�ő҂ׂ��ł���B
       */
      //-------------------------------------------------------------------------
      bool WaitSetupForFastMode( ArcFile::ARCID arcID );

      /**
       * @brief �ǂݍ��݃��N�G�X�g
       */
      struct ReadRequest
      {
        u8 requestPriority;                  // [in] ���N�G�X�g�̎��s�D��x( �����������D��x������ )
        ArcFile::ARCID arcID;                // [in] �ǂݍ��ރf�[�^�̃A�[�J�C�u��ID
        ArcFile::ARCDATID datID;             // [in] �ǂݍ��ރf�[�^�̃A�[�J�C�u���f�[�^ID
        bool compressed;                     // [in] �ǂݍ��ރf�[�^�����k����Ă��邩�H( true:���k����Ă���, false:���k����Ă��Ȃ� )
        gfl::heap::HeapBase* heapForWork;    // [in] �X���b�h�̃C���X�^���X�m�ۃq�[�v ������m�ې����I
        gfl::heap::HeapBase* heapForUncomp;  // [in] ���k�f�[�^�̉𓀍�ƂɎg�p����q�[�v
                                             //      compressed �� true �̏ꍇ�ɂ̂ݎg�p���܂��B
                                             //      NULL �Ȃ� heapForBuffer ���g�p���܂��B
        gfl::heap::HeapBase* heapForBuffer;  // [in]�ǂݍ��ݐ�o�b�t�@���m�ۂ���q�[�v
        u32 destBufferAlignment;             // [in] �ǂݍ��ݐ�o�b�t�@�̃A���C�������g
        void** ppDestBuffer;                 // [out] �ǂݍ��񂾃f�[�^�ւ̃|�C���^���Z�b�g����|�C���^�ϐ��̃A�h���X
        u32* pDestBufferSize;                // [out] �ǂݍ��񂾃f�[�^�T�C�Y�̊i�[��( �s�v�Ȃ�NULL )

        ReadRequest() :
          requestPriority( REQUEST_PRIORITY_NORMAL ),
          arcID( 0 ),
          datID( 0 ),
          compressed( false ),
          heapForWork( NULL ),
          heapForUncomp( NULL ),
          heapForBuffer( NULL ),
          destBufferAlignment( 0 ),
          ppDestBuffer( NULL ),
          pDestBufferSize( NULL )
        { }
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �ǂݍ��݃��N�G�X�g��o�^����
       * @param req  ���N�G�X�g�f�[�^
       * @retval true  ���N�G�X�g��o�^�ł���
       * @retval false ���N�G�X�g��o�^�ł��Ȃ�����
       *
       * @note
       * ���N�G�X�g�����s������,
       * �Ώۂ̃f�[�^�T�C�Y�̃o�b�t�@�� heapForBuffer ����m�ۂ���, �f�[�^��ǂݍ��݂܂�.
       * *ppDestBuffer �Ɋm�ۂ����o�b�t�@�ւ̃|�C���^���Z�b�g���܂�.
       * *pDestBufferSize �ɓǂݍ��񂾃T�C�Y���Z�b�g���܂�.
       */
      //-----------------------------------------------------------------------
      bool AddReadRequest( const ReadRequest& req );


			// �� 2012.7.4.�ǉ� tetsu
      /**
       * @brief �ǂݍ��݃��N�G�X�g(�O���w���ɓǂݍ���)
       */
      struct ReadRequestStatic
      {
        u8 requestPriority;										// [in] ���N�G�X�g�̎��s�D��x( �����������D��x������ )
        ArcFile::ARCID arcID;									// [in] �ǂݍ��ރf�[�^�̃A�[�J�C�u��ID
        ArcFile::ARCDATID datID;							// [in] �ǂݍ��ރf�[�^�̃A�[�J�C�u���f�[�^ID
        bool compressed;											// [in] �ǂݍ��ރf�[�^�����k����Ă��邩�H( true:���k����Ă���, false:���k����Ă��Ȃ� )
        gfl::heap::HeapBase* heapForWork;     // [in] �X���b�h�̃C���X�^���X�m�ۃq�[�v ������m�ې����I
        gfl::heap::HeapBase* heapForUncomp;		// [in] ���k�f�[�^�̉𓀍�ƂɎg�p����q�[�v
																							//      compressed �� true �̏ꍇ�ɂ̂ݎg�p���܂��B
        void* pDestBuffer;										// [in] �ǂݍ��ݐ�|�C���^
        u32		destBufferSize;                 // [in] �Ǔǂݍ��ݐ�f�[�^�T�C�Y
				u32* pReadSize;                       // [out] ���ۂɓǂݍ��񂾃f�[�^�T�C�Y 2012/07/26 by nakahiro

        ReadRequestStatic() :
          requestPriority( REQUEST_PRIORITY_NORMAL ),
          arcID( 0 ),
          datID( 0 ),
          compressed( false ),
          heapForWork( NULL ),
          heapForUncomp( NULL ),
          pDestBuffer( NULL ),
          destBufferSize( 0 ),
          pReadSize( NULL )
        { }
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �ǂݍ��݃��N�G�X�g��o�^����(�O���w���ɓǂݍ���)
       * @param req  ���N�G�X�g�f�[�^
       * @retval true  ���N�G�X�g��o�^�ł���
       * @retval false ���N�G�X�g��o�^�ł��Ȃ�����
			 * 
			 * ���N�G�X�gID�͓ǂݍ��ݐ�o�b�t�@�Ɠ����� ID = (u32)pDestBuffer
       */
      //-----------------------------------------------------------------------
      bool AddReadRequestStatic( const ReadRequestStatic& req );

			//-----------------------------------------------------------------------
			/**
			 * @brief ID�𒼐ڎw�肵�ēǂݍ��݃��N�G�X�g���L�����Z������
			 * @retval true   �L�����Z�����܂���
			 * @retval false  �L�����Z���ł��܂���ł���( ���łɓǂݍ��ݒ� )
       *
       * @note 
       * ���ɓǂݍ��ݒ��̃��N�G�X�g�̓L�����Z���ł��܂���B
       * IsReadFinished() ��, �ǂݍ��݂̏I����҂��Ă��������B
       */
      //-----------------------------------------------------------------------
			bool CancelReadRequest( u32 requestID );

			//-------------------------------------------------------------------------
      /**
       * @brief ID�𒼐ڎw�肵�Ďw�肵���f�[�^�̓ǂݍ��݂��������������m�F
       * @retval true  ��������
       * @retval false �������Ă��Ȃ�
       */
      //-------------------------------------------------------------------------
			bool IsReadFinished( u32 requestID ) const;

			// ��2012.7.4.�ǉ� tetsu

      //-----------------------------------------------------------------------
      /**
       * @brief �ǂݍ��݃��N�G�X�g���L�����Z������
       * @param[in] ppDestBuffer �ǂݍ��񂾃f�[�^�ւ̃|�C���^���Z�b�g����|�C���^�ϐ��̃A�h���X
       * @retval true   �L�����Z�����܂���
       * @retval false  �L�����Z���ł��܂���ł���( ���łɓǂݍ��ݒ� )
       *
       * @note 
       * ���ɓǂݍ��ݒ��̃��N�G�X�g�̓L�����Z���ł��܂���B
       * IsReadFinished() ��, �ǂݍ��݂̏I����҂��Ă��������B
       */
      //-----------------------------------------------------------------------
      bool CancelReadRequest( void** ppDestBuffer );

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���f�[�^�̓ǂݍ��݂������������H
       * @param[in] ppDestBuffer �ǂݍ��񂾃f�[�^�ւ̃|�C���^���Z�b�g����|�C���^�ϐ��̃A�h���X
       * @retval true  ��������
       * @retval false �������Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsReadFinished( void** ppDestBuffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �S�Ă̓ǂݍ��݂������������H
       * @retval true   ��������
       * @retval falsek �������Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsAllReadFinished( void ) const;






      /**
       * @brief �ǂݍ��݃��N�G�X�g��o�^����
       *
       * @attention ���̊֐��͎g�p���Ȃ��ł��������B
       */
      NW_DEPRECATED_FUNCTION( bool AddReadRequest( 
          void** ppDestBuffer,
          u32* pDestBufferSize,
          gfl::heap::HeapBase* heapForBuffer,
          u32 destBufferAlignment,
          ArcFile::ARCID arcID,
          ArcFile::ARCDATID datID,
          bool compressed = false,
          u8 requestPriority = REQUEST_PRIORITY_NORMAL ) );






      private:

      // �t�@�C���Ǘ����
      struct FileManageData
      {
        ArcFile::ARCID arcID;
        ArcFile* arcFile;
        gfl::heap::HeapBase* heapForArcFile;  // �A�[�J�C�u�t�@�C�����̂�u���q�[�v
        u32 requestCount;                     // �A�[�J�C�u�t�@�C���ɑ΂��郊�N�G�X�g�J�E���^
      };

      gfl::heap::HeapBase* m_pWorkHeap;
      base::Thread::Priority m_readThreadPriority;
      FileManageData* m_manageData;
      u32 m_maxManageDataCount;

      void InitManageData( FileManageData* );
      void DeleteManageData( FileManageData* );
      FileManageData* GetManageData( ArcFile::ARCID ) const;
      FileManageData* GetEmptyManageData( void ) const;
      u32 CalcRequestID( void** ) const;
      void FinalizeRequest( const base::ThreadManager::Request& );

      /**
       * @brief ���N�G�X�g�I�����̃R�[���o�b�N
       * @param finished_request  �I���������N�G�X�g
       */
      virtual void OnRequestFinished( const base::ThreadManager::Request& finished_request );

      /**
       * @brief ���N�G�X�g�����s���ꂸ�ɏI�������ۂ̃R�[���o�b�N
       * @param canceled_request  ���s����Ȃ��������N�G�X�g
       */
      virtual void OnRequestCanceled( const base::ThreadManager::Request& canceled_request );
    };


  }  //namespace fs
}  //namespace gfl

#endif  //__GFL_FILEREADMANAGER_H__
