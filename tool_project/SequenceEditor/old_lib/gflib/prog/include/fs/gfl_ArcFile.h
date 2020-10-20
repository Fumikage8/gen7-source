//=============================================================================
/**
 * @brief  GF�A�[�J�C�u�t�@�C���̓ǂݍ���
 * @file   gfl_ArcFile.h
 * @author obata_toshihiro
 * @date   2011.03.19
 */
//=============================================================================
#ifndef __ARCHIVE_READER_H__
#define __ARCHIVE_READER_H__ 
#pragma once

#include <fs/gfl_RomFile.h>

#if GFL_HIO_ENABLE
#include <fs/gfl_HioFile.h>
#endif


namespace gfl {
  namespace fs {

    class IArcFileAccessor;
    class SetupThread;


    class ArcFile
    { 
      GFL_FORBID_COPY_AND_ASSIGN( ArcFile );
      friend class SetupThread;

      public: 
      typedef u32 ARCID;    // �A�[�J�C�u�t�@�C������肷�邽�߂�ID 
      typedef u32 ARCDATID; // �A�[�J�C�u�t�@�C�����̃f�[�^����肷�邽�߂�ID 
      static const u32 ARCID_NULL = 0xffffffff;


      //-----------------------------------------------------------------------
      // �N���X�̐ÓI�Ȑݒ�
      //----------------------------------------------------------------------- 
      /**
       * @brief ����R�[�h��ݒ肷��
       * @param language  �ݒ肷�錾��R�[�h
       */
      static void SetLanguage( u32 language );

      /**
       * @brief �A�[�J�C�u�t�@�C���̖��O�e�[�u����ݒ肷��
       * @param filename_table  �A�[�J�C�uID�ɑΉ�����t�@�C�����̔z��
       * @param arcfile_num     �t�@�C�����z��̗v�f��
       */
      static void SetFileNameTable( const wchar_t** filename_table, size_t arcfile_num );

      /**
       * @brief �A�[�J�C�uID�ɑΉ�����t�@�C�������擾����
       * @param arcid  �t�@�C�������擾����A�[�J�C�uID
       * @return �Ή�����ROM�A�[�J�C�u��̃t�@�C����
       */
      static const wchar_t* GetRomFileName( ARCID arcid );
#if GFL_FILE_DEBUG
      //SD�J�[�h�ǂݍ��݋@�\
      /**
       * @brief Arc��SD�J�[�h����ǂݍ��ނ��߂̃t�@�C�����e�[�u���̐ݒ�
       * @param filename_table  �A�[�J�C�uID�ɑΉ�����t�@�C�����̔z��
       */
      static void SetSDMountFileNameTable( const wchar_t** filename_table );

      /**
       * @brief Arc��SD�J�[�h����ǂݍ��ނ��̃t���O(�t�@�C�������݂��Ȃ���Βʏ�̓ǂݍ��݂����܂�)
       * @param bool �L���E����
       */
      static void SetSDMountIsEnable( bool flg );

      /**
       * @brief Arc��SD�J�[�h����ǂݍ��ނ��̃t���O�̎擾
       * @return bool �L���E����
       */
      static bool GetSDMountIsEnable( void );

      /**
       * @brief Arc��SD�J�[�h����ǂݍ��ގ��ɑΉ�����t�@�C�������擾����
       * @param arcid  �t�@�C�������擾����A�[�J�C�uID
       * @return �Ή�����A�[�J�C�u��̃t�@�C����
       */
      static const wchar_t* GetSDMountFileName( ARCID arcid );
      
#endif

#if GFL_HIO_ENABLE
      /**
       * @brief HIO�A�N�Z�X���邽�߂̃t�@�C�����e�[�u����ݒ肷��
       * @param filename_table  �A�[�J�C�uID�ɑΉ�����t�@�C�����̔z��
       */
      static void SetHioFileNameTable( const wchar_t** filename_table );

      /**
       * @brief �A�[�J�C�uID�ɑΉ�����t�@�C�������擾����
       * @param arcid  �t�@�C�������擾����A�[�J�C�uID
       * @return �Ή�����z�X�g�t�@�C���V�X�e����̃t�@�C����
       */
      static const wchar_t* GetHioFileName( ARCID arcid );
#endif



      //-----------------------------------------------------------------------
      // �R���X�g���N�^�E�f�X�g���N�^
      //----------------------------------------------------------------------- 
      /**
       * @brief �������p�����[�^
       */
      enum InitBit 
      {
       // ���t�@�C�����J��
       // �t�@�C�����J�����g�X���b�h�ŊJ���܂�.
       // �R���X�g���N�^����Ԃ������_�Ńt�@�C�����J����Ă��܂�.
       // �t�@�C����ʃX���b�h�ŊJ�������ꍇ, 
       // OPEN ���w�肹���� StartSetupForFastMode() ���Ă�ł�������.
        OPEN = 1 << 0,

       // ��HIO ���g�p����
       // PC ��ɂ���t�@�C���� HIO �œǂݍ��݂܂�.
       // ARCID �Ńt�@�C�����w�肵���ꍇ�ɂ̂ݗL���ł�.
        HIO  = 1 << 1,

       // ���t�@�C�����J���A�������샂�[�h�ɂ��܂��B
       // SetupForFastMode���R���X�g���N�^���ōs���܂��B
       // �t�@�C����ʃX���b�h�ŊJ�������ꍇ, 
       // OPEN_FASTMODE ���w�肹���� StartSetupForFastMode() ���Ă�ł�������.
        OPEN_FASTMODE  = (1 << 2) | OPEN,
      };

      /**
       * @brief �R���X�g���N�^
       *
       * @param heap       ��ƃ��[�N�Ɏg�p����q�[�v
       * @param arcID      �֘A�t����A�[�J�C�u�t�@�C��
       * @param init_flag  �R���X�g���N�^�ōs�������� InitBit �̘_���a�Ŏw��
       */
      ArcFile( gfl::heap::HeapBase* heap, ARCID arcID, u32 init_flag );

      /**
       * @brief �R���X�g���N�^
       *
       * @param heap       ��ƃ��[�N�Ɏg�p����q�[�v
       * @param filename   �֘A�t����A�[�J�C�u�t�@�C��
       * @param init_flag  �R���X�g���N�^�ōs�������� InitBit �̘_���a�Ŏw��
       */
      ArcFile( gfl::heap::HeapBase* heap, const wchar_t* filename, u32 init_flag );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~ArcFile( void ); 



      //----------------------------------------------------------------------- 
      // �t�@�C���I�[�v��
      // �R���X�g���N�^�� OPEN ���w�肵�Ă���ꍇ�͕K�v����܂���.
      //----------------------------------------------------------------------- 
      /**
       * @brief �t�@�C�����J��
       */
      void Open( void );

      /**
       * @brief �t�@�C�����J���Ă��邩�ǂ����𒲂ׂ�
       * @retval true
       * @retval false
       */
      bool IsOpen( void ) const;

      /**
       * @brief �t�@�C�����J�������_�ŃV�O�i����ԂɂȂ�C�x���g�I�u�W�F�N�g���擾����
       * @note ��x�V�O�i����ԂɂȂ�����A�ȍ~���Z�b�g�͂���܂���B
       * @attention �擾�����C�x���g�I�u�W�F�N�g�̏�Ԃ�ύX���Ȃ��ł��������B
       */
      nn::os::LightEvent* GetOpenEvent( void );


      //-----------------------------------------------------------------------
      // �������[�h�ւ̈ڍs
      //-----------------------------------------------------------------------
      /**
       * @brief �������[�h�ւ̈ڍs���J�n����
       * @param heap_for_thread_work       �X���b�h�̍�Ɨ̈�Ƃ��Ďg�p����q�[�v
       * @param heap_for_fastaccess_table  �����A�N�Z�X�p�f�[�^�̊m�ۂɎg�p����q�[�v
       *
       * @note WaitSetupForFastMode()�ňڍs�̊�����҂��Ă�������.
       */
      void StartSetupForFastMode( 
          gfl::heap::HeapBase* heap_for_thread_work,
          gfl::heap::HeapBase* heap_for_fastaccess_table );

      /**
       * @breif �������[�h�ւ̈ڍs������҂�
       * @retval true   ��������
       * @retval false  ������
       *
       * @attention
       * true ���Ԃ�܂ŌJ��Ԃ��Ă�, �ڍs�̊�����҂��Ă�������.
       */
      bool WaitSetupForFastMode( void ); 

      /**
       * @brief �������[�h���ǂ����𒲂ׂ�
       * @retval true   �������[�h
       * @retval false  �ʏ탂�[�h
       */
      bool IsFastMode( void ) const;

      /**
       * @brief �������[�h�ւ̈ڍs�������������_�ŃV�O�i����ԂɂȂ�C�x���g�I�u�W�F�N�g���擾����
       * @note ��x�V�O�i����ԂɂȂ�����A�ȍ~���Z�b�g�͂���܂���B
       * @attention �擾�����C�x���g�I�u�W�F�N�g�̏�Ԃ�ύX���Ȃ��ł��������B
       */
      nn::os::LightEvent* GetFastSetupEvent( void );

      //=======================================================================
      /**
       * @brief �������[�h�ֈڍs����( �������� )
       * @param heap  �����A�N�Z�X���邽�߂̃f�[�^�m�ۂɎg�p����q�[�v
       */
      //=======================================================================
      void SetupForFastMode( gfl::heap::HeapBase* heap ); 



      //-----------------------------------------------------------------------
      // �A�[�J�C�u�t�@�C���ɂ��Ă̏��擾
      //-----------------------------------------------------------------------
      /**
       * @brief �A�[�J�C�u�Ɋ܂܂��f�[�^�̐����擾����
       *
       * @note �ʏ탂�[�h�̏ꍇ
       * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
       */
      u16 GetDataCount( void );

      /**
       * @brief �f�[�^1������( �p�f�B���O���܂� )�T�C�Y���擾����
       * @param data_id  �A�[�J�C�u�t�@�C�����f�[�^ID
       *
       * @note �ʏ탂�[�h�̏ꍇ
       * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
       */
      size_t GetDataSize( ARCDATID data_id ); 

      /**
       * @brief �f�[�^1������( �p�f�B���O���܂܂Ȃ� )�T�C�Y���擾����
       * @param data_id  �A�[�J�C�u�t�@�C�����f�[�^ID
       *
       * @note �ʏ탂�[�h�̏ꍇ
       * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
       *
       * @note �f�[�^��ǂݍ��ނ��߂ɕK�v�ȃo�b�t�@�T�C�Y��, �p�f�B���O���܂񂾃T�C�Y�ł��B
       * �f�[�^��ǂݍ��ނ��߂̃f�[�^�T�C�Y�� GetDataSize() �Ŏ擾���Ă��������B
       *
       * @attention
       * ���̃��\�b�h��, ver.4.0 �ȍ~�̃A�[�J�C�u�t�@�C���ł̂ݗL���ƂȂ�܂��B
       * ver.3.x �܂ł̃A�[�J�C�u�t�@�C���ł̓p�f�B���O���܂ރf�[�^�T�C�Y���Ԃ�܂��B
       */
      size_t GetRealDataSize( ARCDATID data_id ); 

      /**
       * @brief �A�[�J�C�u�Ɋ܂܂��f�[�^�̂���, �ł��傫�ȃf�[�^�̃T�C�Y���擾����
       *
       * @note 
       * �ʏ탂�[�h�̏ꍇ, 
       * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
       *
       * @attention
       * ���̃��\�b�h��, ver.3.0 �ȍ~�̃A�[�J�C�u�t�@�C���ł̂ݗL���ƂȂ�܂��B
       * ver.2.x �܂ł̃A�[�J�C�u�t�@�C���ł͕K��0���Ԃ�܂��B
       */
      size_t GetMaxDataSize( void );



      //-----------------------------------------------------------------------
      // �����ǂݍ���
      //-----------------------------------------------------------------------
      /**
       * @brief �f�[�^��1�����ǂݍ���
       * @param[in]  data_id      �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[out] dest_buffer  �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
       * @retval ���ۂɓǂݍ��񂾃o�C�g��
       */
      size_t LoadData( ARCDATID data_id, void* dest_buffer );

      /**
       * @brief �f�[�^1���̈ꕔ��ǂݍ���
       * @param[in]  data_id           �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[in]  offset_from_data  �ǂݍ��݊J�n�I�t�Z�b�g(byte, �f�[�^�擪����̃I�t�Z�b�g)
       * @param[in]  read_size         �ǂݍ��݃T�C�Y(byte)
       * @param[out] dest_buffer       �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
       */
      void LoadDataPiece( 
          ARCDATID data_id, u32 offset_from_data, u32 read_size, void* dest_buffer ); 

      /**
       * @brief �f�[�^1���̓ǂݍ��݃|�C���^���V�[�N����( ��舵�����ӁI)
       * @param[in] data_id            �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[in] offset_from_data   �V�[�N�T�C�Y( byte, �f�[�^�擪����̃I�t�Z�b�g)
       */
      void SeekDataPtr( ARCDATID data_id, u32 offset_from_data );

      /**
       * @brief SeekDataPtr()�ŃV�[�N�����|�C���^����f�[�^�ǂݍ���( ��舵�����ӁI)
       * @param[in]  size         �ǂݍ��݃T�C�Y
       * @param[out] dest_buffer  �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
       * @retval ���ۂɓǂݍ��񂾃o�C�g��
       */
      size_t LoadDataPtr( u32 size, void* dest_buffer );

			/**
			 * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
       * @param[in]		  data_id	          �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[in]		  heap		          �𓀍�Ɨp�q�[�v
       * @param[in,out]	buffer	          �𓀃f�[�^�̓W�J��o�b�t�@
       * @param[in]	    buffer_size       �𓀃f�[�^�̓W�J��o�b�t�@�̃T�C�Y
       * @param[out]	  uncompressed_size �W�J�f�[�^�̃T�C�Y
			 */
			void LoadCompressedData( 
          ARCDATID data_id, 
          gfl::heap::HeapBase* heap,
          void* buffer,
          size_t buffer_size,
          size_t* uncompressed_size = NULL ); 

      /**
       * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
       * @param[in]		datID			  �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[in]		heapForWork	�𓀍�ƂɎg�p����q�[�v
       * @param[in]		heapForData	�𓀃f�[�^���m�ۂ���q�[�v
       * @param[in]		align				�W�J�f�[�^�̃A���C��
       * @param[out]	pSize				�W�J�f�[�^�̃T�C�Y
       * @return	�W�J�����f�[�^
       */
      void * LoadCompressedDataAlloc( 
          ARCDATID datID, 
          gfl::heap::HeapBase * heapForWork,
          gfl::heap::HeapBase * heapForData,
          int align,
          size_t * pSize );

			/**
			 * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
       *
       * @attention ���̊֐��͎g�p���Ȃ��ł��������B
			 */
			void LoadData( 
          ARCDATID data_id, 
          gfl::heap::HeapBase* heap,
          void* buffer,
          size_t buffer_size,
          size_t* uncompressed_size = NULL );

      /**
       * @brief	���k�f�[�^��1�����W�J���ēǂݍ���
       *
       * @attention ���̊֐��͎g�p���Ȃ��ł��������B
       */
      void * LoadData( 
          ARCDATID datID,
          gfl::heap::HeapBase * heap,
          int align = 4,
          size_t * pSize = NULL );



      //-----------------------------------------------------------------------
      // �񓯊��ǂݍ���
      //-----------------------------------------------------------------------
      /**
       * @brief �񓯊��ǂݍ��݂��J�n����
       *
       * @param[in]  heap             �X���b�h�̐����Ɏg�p����q�[�v
       * @param[in]  data_id          �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[out] buffer           �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
       * @param[in]  thread_priority  �X���b�h�̗D��x( �� 0 �` 31 �� )
       * @param[in]  callback_func    �ǂݍ��ݏI�����̃R�[���o�b�N�֐�( NULL�Ȃ�Ă΂�Ȃ� )
       * @param[in]  callback_work    �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( �s�v�Ȃ�NULL )
       *
       * @return �񓯊��ǂݍ��݂��J�n�ł������ǂ���
       * @retval true   �J�n�ł���
       * @retval false  �J�n�ł��Ȃ�����
       *
       * @attention 
       * ArcFile�C���X�^���X1�ɂ�, ������2�ȏ�̔񓯊��ǂݍ��݂����s���邱�Ƃ͏o���܂���.
       * ���łɎ��s���̔񓯊��ǂݍ��݂�����ꍇ, ���̊֐��͎��s���܂�( false ���Ԃ�܂� ).
       *
       * @note
       * WaitAsyncRead()�œǂݍ��݂̊�����҂��Ă�������.
       *
       * @note �R�[���o�b�N���֐��ł͂Ȃ����X�i�N���X��n���悤�ɂ�����
       */
      bool StartAsyncRead( 
          gfl::heap::HeapBase* heap, 
          ARCDATID data_id,
          void* buffer, 
          gfl::base::Thread::Priority thread_priority,
          File::CallBackFuncOnAsyncAccessFinished callback_func = NULL, 
          void* callback_work = NULL );

      /**
       * @brief �񓯊��ǂݍ��݂��J�n����( �I�t�Z�b�g�w��ver. )
       *
       * @param[in]  heap             �X���b�h�̐����Ɏg�p����q�[�v
       * @param[in]  data_id          �A�[�J�C�u�t�@�C�����f�[�^ID
       * @param[in]  read_offset      �w�肵���f�[�^�̐擪����̃I�t�Z�b�g
       * @param[in]  read_size        �ǂݍ��ރf�[�^�T�C�Y
       * @param[out] buffer           �ǂݍ��񂾃f�[�^�̊i�[��( 4 byte alignment )
       * @param[in]  thread_priority  �X���b�h�̗D��x( �� 0 �` 31 �� )
       * @param[in]  callback_func    �ǂݍ��ݏI�����̃R�[���o�b�N�֐�( NULL�Ȃ�Ă΂�Ȃ� )
       * @param[in]  callback_work    �R�[���o�b�N�֐��ɓn���C�ӂ̃��[�N( �s�v�Ȃ�NULL )
       *
       * @return �񓯊��ǂݍ��݂��J�n�ł������ǂ���
       * @retval true   �J�n�ł���
       * @retval false  �J�n�ł��Ȃ�����
       *
       * @attention 
       * ArcFile�C���X�^���X1�ɂ�, ������2�ȏ�̔񓯊��ǂݍ��݂����s���邱�Ƃ͏o���܂���.
       * ���łɎ��s���̔񓯊��ǂݍ��݂�����ꍇ, ���̊֐��͎��s���܂�( false ���Ԃ�܂� ).
       *
       * @note
       * WaitAsyncRead()�œǂݍ��݂̊�����҂��Ă�������.
       *
       * @note �R�[���o�b�N���֐��ł͂Ȃ����X�i�N���X��n���悤�ɂ�����
       */
      bool StartAsyncRead( 
          gfl::heap::HeapBase* heap, 
          ARCDATID data_id,
          u32 read_offset,
          u32 read_size,
          void* buffer, 
          gfl::base::Thread::Priority thread_priority,
          File::CallBackFuncOnAsyncAccessFinished callback_func = NULL, 
          void* callback_work = NULL );

      /**
       * @brief �񓯊��ǂݍ��݂̊�����҂�
       * @retval true   ������
       * @retval false  ����
       *
       * @attention
       * false ���Ԃ�܂ŌJ��Ԃ��Ă�, �ڍs�̊�����҂��Ă�������.
       */
      bool WaitAsyncRead( void ); 

      /**
       * @brief ���s���̔񓯊��ǂݍ��݂��L�����Z������
       *
       * @note
       * WaitAsyncRead()�œǂݍ��݂̊�����҂��Ă�������.
       */
      void CancelAsyncRead( void );


      //-----------------------------------------------------------------------
      // HIO
      //-----------------------------------------------------------------------
      /**
       * @brief HIO �ɂ��ǂݍ��݂��H
       * @retval true   HIO �ǂݍ���
       * @retval false  HIO �ǂݍ��݂łȂ�
       */
      bool IsHioFile( void ) const;


      //-----------------------------------------------------------------------
      // �A�[�J�C�u���̃f�[�^�ɃA�N�Z�X���邽�߂̏��
      //-----------------------------------------------------------------------
      /**
       * @brief �A�[�J�C�u���̃f�[�^�ɃA�N�Z�X���邽�߂̏��
       */
      struct AccessInfo 
      {
        u32 dataSize;             // �f�[�^�T�C�Y( �p�f�B���O���܂� )
        u32 realSize;             // �f�[�^�T�C�Y( �p�f�B���O���܂܂Ȃ� )
        u32 offsetFromArchiveTop; // �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g
      };
      /**
       * @brief �A�[�J�C�u���f�[�^�ւ̃A�N�Z�X����K�؂ȏꏊ����擾����
       * @param[in]  data_id      �t�@�C�����f�[�^ID
       * @param[out] access_info  �擾�����A�N�Z�X���̊i�[��
       *
       * @note �ʏ탂�[�h�̏ꍇ
       * �t�@�C����ǂޕK�v�����邽�߃X���b�h�̎��s�����ڂ�\��������܂�.
       */
      void GetAccessInfo( ARCDATID data_id, AccessInfo* access_info );



      private:
      static const u32 STACK_SIZE_OF_SETUP_THREAD_FOR_FAST_MODE;
      static u32 LANGUAGE_CODE;  // ���쌾��R�[�h
      static size_t ARCFILE_NUM;
      static const wchar_t** FILENAME_TABLE;
      static const wchar_t* GetFileName( ARCID , const wchar_t**, u32 );

      gfl::heap::HeapBase* m_workHeap;     // ��ƃ��[�N�m�ۗp�̃q�[�v
      gfl::fs::RomFile m_file;             // �t�@�C�����삷��I�u�W�F�N�g
      const wchar_t* m_filename;           // �ΏۃA�[�J�C�u�̃t�@�C����
      u16 m_dataNum;                       // �A�[�J�C�u���̃f�[�^��
      u32 m_maxDataSize;                   
      AccessInfo* m_fastAccessTable;       // �����A�N�Z�X�̂��߂̍����e�[�u��
      SetupThread* m_setupThread;          // �������[�h�ֈڍs���邽�߂̃X���b�h
      nn::os::LightEvent m_fileOpenEvent;  // �I�[�v�����ɃV�O�i����ԂɂȂ�C�x���g
      nn::os::LightEvent m_fastSetupEvent; // �������[�h�ւ̈ڍs�������������ɃV�O�i����ԂɂȂ�C�x���g
      nn::os::CriticalSection m_criticalSectionForFileOpen;
      nn::os::CriticalSection m_criticalSectionForFileRead;
      nn::os::CriticalSection m_criticalSectionForFastSetup;

#if GFL_FILE_DEBUG
      static const wchar_t** SD_FILENAME_TABLE;
      static bool m_isEnableSd;
#endif

#if GFL_HIO_ENABLE
      static const wchar_t** HIO_FILENAME_TABLE;
      bool m_isHioFile;
#endif

#if GFL_DEBUG
      u8 m_majorVersion;
      u8 m_minorVersion;
#endif



      void InitExclusiveObjects( void );

      bool OpenFile( const wchar_t* );
      void SeekFile( size_t ); 
      void ExclusiveRead( u32 readPos, u32 readSize, void* pDestBuffer, u32* pReadSize = NULL );
      
      bool IsSetupForFastModeFinished( void ) const; 
      bool IsSettingUpForFastMode( void ) const; 
      void EndSetupForFastMode( void ); 

      bool CheckAsyncRead( void ) const;
      void EndAsyncRead( void );
      size_t ReadTotalBlockSize( void );

      IArcFileAccessor* CreateArcFileAccessor( gfl::heap::HeapBase* );
      //void GetAccessInfoFromFile_Slow( ARCDATID, AccessInfo* );
      void GetAccessInfoFromFile_Fast( ARCDATID, AccessInfo*, size_t, void* );
      void GetAccessInfoFromTable( ARCDATID, AccessInfo* );
      AccessInfo* CreateDataAccessTable( gfl::heap::HeapBase*, const IArcFileAccessor* ) const;

      u16 GetFileCountFromFile( void ); 
      void Uncompress( void*, size_t, void*, size_t, size_t* );
    };


  }  //namespace fs
}  //namespace gfl 

#endif  //__ARCHIVE_READER_H__
