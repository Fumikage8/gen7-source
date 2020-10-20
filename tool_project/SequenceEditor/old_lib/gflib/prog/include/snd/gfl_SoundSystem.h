//=============================================================================
/**
 * @file gfl_SoundSystem.h
 * @brief �T�E���h�V�X�e��
 * @author obata_toshihiro
 * @date 2010.10.29
 */
//=============================================================================
#ifndef __GFL_SOUNDSYSTEM_H__
#define __GFL_SOUNDSYSTEM_H__
#pragma once

#include <nw.h>
#if GFL_HIO_ENABLE
#include <nw/snd/snd_HioSoundArchive.h>
#endif
#include "gfl_Heap.h"
#include "gfl_Math.h"
#include "base/gfl_Singleton.h"


namespace gfl {
  namespace snd {

    class SoundHeap;
    class SoundDataManagerThread;

    class SoundSystem : public gfl::base::Singleton<SoundSystem> 
    {
      GFL_FORBID_COPY_AND_ASSIGN( SoundSystem ); 
      public: 

      // �T�E���hID�̖����l
      static const u32 INVALID_SOUND_ID = 0xffffffff; 

      // �T�E���h�A�[�J�C�u�̓ǂݍ��ݕ�
      enum SoundArchiveType {
        SOUND_ARCHIVE_TYPE_ROM,     // ROM��̃A�[�J�C�u���g�p����
        SOUND_ARCHIVE_TYPE_MEMORY,  // ��������̃A�[�J�C�u���g�p����
#if GFL_HIO_ENABLE
        SOUND_ARCHIVE_TYPE_HIO,     // PC��̃A�[�J�C�u���g�p����
#endif
      }; 

      // �������p�����[�^
      struct InitParam 
      {
        gfl::heap::HeapBase* pAppHeap;            // �V�X�e�����g�p����A�v���P�[�V�����q�[�v
        gfl::heap::HeapBase* pDevHeap;            // �V�X�e�����g�p����f�o�C�X�q�[�v
        u32                  soundHeapSize;       // �T�E���h�q�[�v�̃T�C�Y
        u8                   soundHandleNum;      // �Ǘ�����T�E���h�n���h���̐�
        u8                   sound3DActorNum;     // 3D�A�N�^�[�̐�
        SoundArchiveType     soundArchiveType;    // �T�E���h�A�[�J�C�u���ǂ��ɂ���̂�
        const char*          bcsarFilePath;       // �T�E���h�A�[�J�C�u�t�@�C���̃p�X( SOUND_ARCHIVE_TYPE_HIO �̏ꍇ�͐�΃p�X���w�� )
        const void*          bcsarData;           // �T�E���h�A�[�J�C�u�̃f�[�^( SOUND_ARCHIVE_TYPE_MEMORY ���w�肵���ꍇ�̂ݗL�� )
        bool                 aacUse;              // AAC �T�E���h���g�p���邩�H
        char*                aacDspComponentPath; // AAC �f�R�[�h�p DSP �R���|�[�l���g�̃p�X
        s32                  soundThreadPriority; // �T�E���h�X���b�h�̃v���C�I���e�B
        s32                  soundDataLoadThreadPriority; // �T�E���h�f�[�^���[�h�X���b�h�̃v���C�I���e�B

        InitParam( void ) :
          pAppHeap( NULL ), 
          pDevHeap( NULL ),
          soundHeapSize( 0 ),
          soundHandleNum( 0 ),
          sound3DActorNum( 0 ), 
          soundArchiveType( SOUND_ARCHIVE_TYPE_ROM ),
          bcsarFilePath( NULL ),
          bcsarData( NULL ),
          aacUse( false ),
          aacDspComponentPath( NULL ),
          soundThreadPriority( nw::snd::SoundSystem::SoundSystemParam::DEFAULT_SOUND_THREAD_PRIORITY ),
          soundDataLoadThreadPriority( nw::snd::SoundSystem::SoundSystemParam::DEFAULT_TASK_THREAD_PRIORITY )
        {}
      }; 


      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       *
       * @param const InitParam& initParam �������p�����[�^
       */
      //-----------------------------------------------------------------------
      SoundSystem( const InitParam& initParam );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~SoundSystem(); 

      //-----------------------------------------------------------------------
      /**
       * @brief �t���[������( ���t���[���Ă�ł������� )
       */
      //-----------------------------------------------------------------------
      void Update( void );




      // ���[�h�Ώۂ�I�����邽�߂̃t���O
      enum {
        LOAD_SEQ        = nw::snd::SoundDataManager::LOAD_SEQ,  // �V�[�P���X�T�E���h
        LOAD_WAVE_SOUND = nw::snd::SoundDataManager::LOAD_WSD,  // �E�F�[�u�T�E���h
        LOAD_BANK       = nw::snd::SoundDataManager::LOAD_BANK, // �o���N
        LOAD_WAVE_ARC   = nw::snd::SoundDataManager::LOAD_WARC, // �g�`�A�[�J�C�u
        LOAD_ALL        = nw::snd::SoundDataManager::LOAD_ALL,  // �֘A����S�Ẵt�@�C��
      }; 

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�A�[�J�C�u���̃f�[�^���V�X�e�������q�[�v�Ƀ��[�h����
       *
       * @param itemID    ���[�h����f�[�^�̃A�C�e��ID
       * @param loadFlag  ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
       */
      //-----------------------------------------------------------------------
      bool LoadSoundData( u32 itemID, u32 loadFlag = LOAD_ALL );

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�A�[�J�C�u���̃f�[�^���w�肵���T�E���h�q�[�v�Ƀ��[�h����
       *
       * @param[out] dest      �ǂݍ��񂾃f�[�^��z�u����T�E���h�q�[�v
       * @param[in]  itemID    ���[�h����f�[�^�̃A�C�e��ID
       * @param[in]  loadFlag  ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
       */
      //-----------------------------------------------------------------------
      bool LoadSoundData( SoundHeap* dest, u32 itemID, u32 loadFlag = LOAD_ALL );

      
      //----------------------------------------------------------------------------
      /**
       * @brief �T�E���h�A�[�J�C�u���̃f�[�^���V�X�e�������q�[�v�Ƀ��[�h���� �񓯊���
       *
       * @param thread_heap �X���b�h�q�[�v
       * @param priority    �X���b�h�D�揇��
       * @param itemID      ���[�h����f�[�^�̃A�C�e��ID
       * @param loadFlag    ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
       *
       *	@retval true    �ǂ݊J�n
       *	@retval false   �ǂ݊J�n���s
       */
      //-----------------------------------------------------------------------------
      bool LoadSoundDataAsync( gfl::heap::HeapBase * thread_heap, gfl::base::Thread::Priority priority, u32 itemID, u32 loadFlag = LOAD_ALL );

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�A�[�J�C�u���̃f�[�^���V�X�e�������q�[�v�Ƀ��[�h���� �񓯊���
       *
       * @param[out] dest      �ǂݍ��񂾃f�[�^��z�u����T�E���h�q�[�v
       * @param[in]  thread_heap �X���b�h�q�[�v
       * @param[in]  priority    �X���b�h�D�揇��
       * @param[in]  itemID    ���[�h����f�[�^�̃A�C�e��ID
       * @param[in]  loadFlag  ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
       */
      //-----------------------------------------------------------------------
      bool LoadSoundDataAsync( SoundHeap* dest, gfl::heap::HeapBase * thread_heap, gfl::base::Thread::Priority priority, u32 itemID, u32 loadFlag = LOAD_ALL );


      //----------------------------------------------------------------------------
      /**
       *	@brief  �T�E���h�A�[�J�C�u���̃f�[�^���w�肵���T�E���h�q�[�v�Ƀ��[�h����@�񓯊��@�҂�
       *
       *	@retval true  ���[�h����
       *	@retval false ���[�h��
       */
      //-----------------------------------------------------------------------------
      bool WaitLoadSoundDataAsync(void);

      //----------------------------------------------------------------------------
      /**
       *	@brief  �T�E���h�A�[�J�C�u�@�񓯊��ǂݍ��݂̌��ʂ��擾
       *
       *	@retval true  ����
       *	@retval false ���s
       */
      //-----------------------------------------------------------------------------
      bool GetLoadSoundDataAsyncResult(void) const;


      //----------------------------------------------------------------------------
      /**
       *	@brief  �T�E���h�f�[�^�}�l�[�W���փT�E���h�f�[�^��ݒ肷��B
       *
       *	@param    fileID    �Z�b�g����f�[�^�̃t�@�C��ID�ł��B
       *	@param    address   �A�h���X
       */
      //-----------------------------------------------------------------------------
      void SetSoundFileAddress( u32 fileID, const void * address );

      //----------------------------------------------------------------------------
      /**
       *	@brief  �T�E���h�f�[�^�}�l�[�W���փT�E���h�O���[�v�t�@�C����o�^����B
       *
       *	@param    groupFile �O���[�v�t�@�C��
       *	@param    size      �T�C�Y
       */
      //-----------------------------------------------------------------------------
      void SetSoundFileAddressInGroupFile( const void * groupFile, size_t size );

      //----------------------------------------------------------------------------
      /**
       *	@brief  �T�E���h�f�[�^�}�l�[�W������T�E���h�O���[�v�t�@�C����o�^��������
       *
       *	@param    groupFile �O���[�v�t�@�C��
       *	@param    size      �T�C�Y
       */
      //-----------------------------------------------------------------------------
      void ClearSoundFileAddressInGroupFile( const void * groupFile, size_t size );


      //-----------------------------------------------------------------------
      /**
       * @brief �Đ�����
       *
       * @param handleIdx    �Đ���̐���Ɏg�p����n���h���̃C���f�b�N�X
       * @param itemID       �Đ�����T�E���h�̃A�C�e��No.
       * @param fadeInFrames �t�F�[�h�C���ɂ�����t���[����
       * @param startInfo    �Đ��J�n�ڍאݒ�\����
       *
       * @return �Đ������������ꍇ true
       */
      //-----------------------------------------------------------------------
      bool Play( u8 handleIdx, u32 itemID, s32 fadeInFrames = 0, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );

      //-----------------------------------------------------------------------
      /**
       * @brief �Đ��̏������J�n����
       *
       * @param handleIdx   �Đ���̐���Ɏg�p����n���h���̃C���f�b�N�X
       * @param itemID      �Đ�����T�E���h�̃A�C�e��No.
       * @param startInfo   �Đ��J�n�ڍאݒ�\����
       *
       * @retval true   ����
       * @retval false  ���s
       */
      //-----------------------------------------------------------------------
      bool PrepareSound( u8 handleIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );

      //-----------------------------------------------------------------------
      /**
       * @brief PrepareSound()�ŊJ�n�����Đ������������������ǂ����𒲂ׂ�
       *
       * @param handleIdx  �Đ�����T�E���h�n���h���̃C���f�b�N�X
       *
       * @retval true   �Đ�����������
       * @retval false  �Đ�������������
       */
      //-----------------------------------------------------------------------
      bool IsPrepared( u8 handleIdx );

      //-----------------------------------------------------------------------
      /**
       * @brief PrepareSound()�ŏ��������T�E���h���Đ�����
       *
       * @param handleIdx     �Đ�����T�E���h�n���h���̃C���f�b�N�X
       * @param fadeInFrames  �t�F�[�h�C���ɂ�����t���[����
       *
       * @note �Đ��������������Ă��Ȃ��ꍇ, ����������҂��Ă���Đ����J�n���܂�.
       */
      //-----------------------------------------------------------------------
      void PlayPrepared( u8 handleIdx, s32 fadeInFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief ��~����
       *
       * @param handleIdx     ������s���n���h���̃C���f�b�N�X
       * @param fadeOutFrames �t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void Stop( u8 handleIdx, s32 fadeOutFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief �S�ẴT�E���h���~����
       * @param fadeOutFrames �t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void StopAll( s32 fadeOutFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief �ꎞ��~ or �ĊJ ����
       *
       * @param handleIdx  ������s���n���h���̃C���f�b�N�X
       * @param pauseOn    true:�ꎞ��~ false:�ĊJ
       * @param fadeFrames �t�F�[�h�C��, �t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void Pause( u8 handleIdx, bool pauseOn, s32 fadeFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief �ꎞ��~�����ǂ����𒲂ׂ�
       *
       * @param handleIdx ������s���n���h���̃C���f�b�N�X
       *
       * @retval true   �ꎞ��~��
       * @retval false  �ꎞ��~���łȂ�
       */
      //-----------------------------------------------------------------------
      bool IsPause( u8 handleIdx ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief �Đ�, ��~���ɂ�����
       *        �t�F�[�h�C��, �t�F�[�h�A�E�g�����܂ł̎c��t���[�������擾����
       *
       * @param handleIdx ������s���n���h���̃C���f�b�N�X
       */
      //-----------------------------------------------------------------------
      s32 GetRemainingFadeFrames( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �ꎞ��~���ɂ�����
       *        �t�F�[�h�C��, �t�F�[�h�A�E�g�����܂ł̎c��t���[�������擾����
       *
       * @param handleIdx ������s���n���h���̃C���f�b�N�X
       */
      //-----------------------------------------------------------------------
      s32 GetRemainingPauseFadeFrames( u8 handleIdx ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief �n���h���ɃT�E���h���֘A�t�����Ă��邩�ǂ����𒲂ׂ�
       *
       * @param handleIdx  ���ׂ�n���h���̃C���f�b�N�X
       *
       * @retval true    �֘A�t�����Ă���
       * @retval false   �֘A�t�����Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsAttachedSound( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �n���h���֘A�t�����Ă���T�E���h��ID���擾����
       *
       * @param handleIdx  ���ׂ�n���h���̃C���f�b�N�X
       *
       * @return �w�肵���n���h���Ɋ֘A�t�����Ă���T�E���hID
       * @retval INVALID_SOUND_ID  �T�E���h���֘A�t�����Ă��Ȃ��ꍇ
       */
      //-----------------------------------------------------------------------
      u32 GetAttachedSound( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �X�g���[���T�E���h�̍Đ��ʒu���擾����
       *
       * @param handleIdx  �X�g���[���T�E���h�Đ����̃n���h���̃C���f�b�N�X
       *
       * @return �X�g���[���f�[�^�̍Đ��ʒu( �T���v���� )
       * @retval ����  �n���h���������̏ꍇ
       */
      //-----------------------------------------------------------------------
      s32 GetStreamSamplePosition( u8 handleIdx ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief ���ʂ�ύX����
       *
       * @param handleIdx ������s���n���h���̃C���f�b�N�X
       * @param volume    �ݒ肷�鉹�ʂ̔{�� ( 0.0 �` )
       * @param frames    ���ʂ̕ω��ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void SetVolume( u8 handleIdx, float volume, s32 frames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief ������ύX����
       *
       * @param handleIdx ������s���n���h���̃C���f�b�N�X
       * @param pitch     ���g���䗦
       *
       * @note ���g���䗦 
       *   0.5 = 1 �I�N�^�[�u ��
       *   1.0 = ���{ ( �f�t�H���g�l )
       *   2.0 = 1 �I�N�^�[�u ��
       */
      //-----------------------------------------------------------------------
      void SetPitch( u8 handleIdx, float pitch );

      //-----------------------------------------------------------------------
      /**
       * @brief 12���ϗ��ɑ����������ɕύX����
       *
       * @param u8 handleIdx  ������s���n���h���̃C���f�b�N�X
       * @param s8 level      12���ϗ��̃��x��
       *
       * @note level�̎w���
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2�I�N�^�[�u��������
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1�I�N�^�[�u��������
       * EQUAL_TEMPERAMENT_LEVEL_UNISON             �ʏ�̉���
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1�I�N�^�[�u�Ⴂ����
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2�I�N�^�[�u�Ⴂ����
       */
      //-----------------------------------------------------------------------
      void SetPitchByEqualTemperament( u8 handleIdx, s8 level ); 
      static const s8 ONE_OCTAVE_STEP_COUNT = 12;
      static const s8 EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH = ONE_OCTAVE_STEP_COUNT;  // 1�I�N�^�[�u�����Ȃ���g����
      static const s8 EQUAL_TEMPERAMENT_LEVEL_UNISON = 0;                               // ���{�̎��g����
      static const s8 EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW = -ONE_OCTAVE_STEP_COUNT;  // 1�I�N�^�[�u�Ⴍ�Ȃ���g����

      //-----------------------------------------------------------------------
      /**
       * @brief �p�� ( ���E�̒�� ) ��ύX����
       *
       * @param handleIdx  ������s���n���h���̃C���f�b�N�X
       * @param pan        �p���̑��Βl
       *
       * @note
       * pan ��  0.0 �Ȃ�, �f�[�^�ʂ�̕������畷������.
       * pan �� -1.0 �Ȃ�, ���ʂ��畷�����Ă������������畷������悤�ɂȂ�.
       * pan �� +1.0 �Ȃ�, ���ʂ��畷�����Ă��������E���畷������悤�ɂȂ�.
       */
      //-----------------------------------------------------------------------
      void SetPan( u8 handleIdx, float pan );

      //-----------------------------------------------------------------------
      /**
       * @brief �T���E���h�p�� ( �O��̒�� ) ��ύX����
       *
       * @param handleIdx  ������s���n���h���̃C���f�b�N�X
       * @param pan        �p���̑��Βl
       *
       * @note
       * pan ��  0.0 �Ȃ�, �f�[�^�ʂ�̕������畷������.
       * pan �� +1.0 �Ȃ�, �őO�����畷�����Ă��������������畷������悤�ɂȂ�.
       * pan �� +2.0 �Ȃ�, �őO�����畷�����Ă��������Ō�����畷������悤�ɂȂ�.
       */
      //-----------------------------------------------------------------------
      void SetSurroundPan( u8 handleIdx, float pan );

      //-----------------------------------------------------------------------
      /**
       * @brief �e���|��ύX����
       *
       * @param handleIdx   ������s���n���h���̃C���f�b�N�X
       * @param tempoRatio  �e���|�̔{��
       *
       * @attention �V�[�P���X�T�E���h�ɑ΂��Ă̂ݗL���ł��B
       */
      //-----------------------------------------------------------------------
      void SetTempoRatio( u8 handleIdx, f32 tempoRatio );

	    //-------------------------------------------------------------------------
		  /**
	     * @brief �g�����X�|�[�Y
	     *
	     * @param handleIdx    ������s���n���h���̃C���f�b�N�X
	     * @param tempoRatio  �e���|�̔{��
	     *
	     * @attention �V�[�P���X�T�E���h�ɑ΂��Ă̂ݗL���ł��B
	     */
			//-------------------------------------------------------------------------
			void SetTranspose( u8 handleIdx, s8 transposeValue, u32 transposeTrackBit );



      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���v���C���[�ōĐ����̃T�E���h�̌����擾����
       *
       * @param playerID �v���C���[�̃A�C�e��ID
       */
      //-----------------------------------------------------------------------
      u32 GetPlayingSoundCount( u32 playerID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���v���C���[�̉��ʂ��擾����
       *
       * @param playerID �v���C���[�̃A�C�e��ID
       */
      //-----------------------------------------------------------------------
      f32 GetPlayerVolume( u32 playerID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���v���C���[�̉��ʂ�ύX����
       *
       * @param playerID �v���C���[�̃A�C�e��ID
       * @param volume    �ݒ肷�鉹��[0.0, 2.0]
       */
      //-----------------------------------------------------------------------
      void SetPlayerVolume( u32 playerID, f32 volume );

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���v���C���[�ōĐ����̑S�ẴT�E���h���~����
       *
       * @param playerID      �v���C���[�̃A�C�e��ID
       * @param fadeOutFrame  �t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void StopPlayerSound( u32 playerID, u32 fadeOutFrame );

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���v���C���[�ōĐ����̑S�ẴT�E���h���ꎞ��~�E�ĊJ����
       *
       * @param playerID   �v���C���[�̃A�C�e��ID
       * @param pauseOn    true:�ꎞ��~, false:�ĊJ
       * @param fadeFrame  �t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void PausePlayerSound( u32 playerID, bool pauseOn, u32 fadeFrame );




      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�q�[�v�̏�Ԃ�ۑ�����
       *
       * @return �ۑ���̊K�w���x�� ( �ۑ��Ɏ��s�����ꍇ�� -1 )
       */
      //-----------------------------------------------------------------------
      s32 SaveHeapState( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �ۑ������T�E���h�q�[�v�̏�Ԃ𕜌�����
       *
       * @param level ��������K�w���x��
       */
      //-----------------------------------------------------------------------
      void LoadHeapState( s32 level );

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�q�[�v���N���A����
       */
      //-----------------------------------------------------------------------
      void ClearHeap( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���݂̃T�E���h�q�[�v�̊K�w���x�����擾����
       */
      //-----------------------------------------------------------------------
      s32 GetCurrentHeapLevel( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�q�[�v�̃T�C�Y���擾����
       */
      //-----------------------------------------------------------------------
      size_t GetHeapSize( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�q�[�v�̋󂫗e�ʂ��擾����
       */
      //-----------------------------------------------------------------------
      size_t GetHeapFreeSize( void ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief �}�X�^�[�{�����[���� �ݒ� ����
       *
       * @param volume    ���ʂ̔{�� ( 0.0 �` )
       * @param fadeTimes ���ʕω��ɂ����鎞��[�~���b]
       */
      //-----------------------------------------------------------------------
      void SetMasterVolume( float volume, s32 fadeTimes );

      //-----------------------------------------------------------------------
      /**
       * @brief �}�X�^�[�{�����[���� �擾 ����
       *
       * @return �}�X�^�[�{�����[���̔{�� ( 0.0 �` )
       */
      //-----------------------------------------------------------------------
      float GetMasterVolume( void ) const;




      // �o�̓��[�h
      enum OutputMode {
        OUTPUT_MODE_MONO     = nw::snd::OUTPUT_MODE_MONO,     // ���m�������[�h ( �p������ )
        OUTPUT_MODE_STEREO   = nw::snd::OUTPUT_MODE_STEREO,   // �X�e���I���[�h
        OUTPUT_MODE_SURROUND = nw::snd::OUTPUT_MODE_SURROUND, // �T���E���h���[�h
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�o�̓��[�h��ݒ肷��
       *
       * @param mode �ݒ肷��o�̓��[�h
       */
      //-----------------------------------------------------------------------
      void SetOutputMode( OutputMode mode );

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�o�̓��[�h���擾����
       */
      //-----------------------------------------------------------------------
      OutputMode GetOutputMode( void ) const; 




      // �V�[�P���X�̃~���[�g���
      enum SeqMute {
        SEQ_MUTE_OFF     = nw::snd::SEQ_MUTE_OFF,     // �V�[�P���X�g���b�N���~���[�g��Ԃ���������.
        SEQ_MUTE_NO_STOP = nw::snd::SEQ_MUTE_NO_STOP, // �V�[�P���X�g���b�N���~���[�g��Ԃɂ���. �������̉��ɕω��͂Ȃ�.
        SEQ_MUTE_RELEASE = nw::snd::SEQ_MUTE_RELEASE, // �V�[�P���X�g���b�N���~���[�g��Ԃɂ���. �������̉����ɂ₩�ɒ�~������.
        SEQ_MUTE_STOP    = nw::snd::SEQ_MUTE_STOP,    // �V�[�P���X�g���b�N���~���[�g��Ԃɂ���. �������̉��𒼂��ɒ�~����.
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̃~���[�g��Ԃ�ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param mute         �~���[�g���
       */
      //-----------------------------------------------------------------------
      void SetTrackMute( u8 handleIdx, u32 trackBitFlag, SeqMute mute );

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̃T�C�����g��Ԃ�ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param silenceFlag  �T�C�����g��Ԃɂ���ꍇ true, ��������ꍇ false
       * @param fadeTimes    �t�F�[�h���� (�~���b)
       */
      //-----------------------------------------------------------------------
      void SetTrackSilence( u8 handleIdx, u32 trackBitFlag, bool silenceFlag, s32 fadeTimes ); 

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̉��ʂ�ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param volume       ���ʂ̔{�� ( �f�t�H���g�� 1.0 )
       * @param frame        ���ʕω��ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void SetTrackVolume( u8 handleIdx, u32 trackBitFlag, f32 volume, s32 frame );

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̉�����ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param pitch        ���g���䗦
       *
       * @note ���g���䗦 
       *   0.5 = 1 �I�N�^�[�u ��
       *   1.0 = ���{ ( �f�t�H���g�l )
       *   2.0 = 1 �I�N�^�[�u ��
       */
      //-----------------------------------------------------------------------
      void SetTrackPitch( u8 handleIdx, u32 trackBitFlag, f32 pitch );

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̉�����12���ϗ��ɑ����������ɕύX����
       *
       * @param handleIdx     ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag  �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param level         12���ϗ��̃��x��
       *
       * @note level�̎w���
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2�I�N�^�[�u��������
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1�I�N�^�[�u��������
       * EQUAL_TEMPERAMENT_LEVEL_UNISON             �ʏ�̉���
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1�I�N�^�[�u�Ⴂ����
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2�I�N�^�[�u�Ⴂ����
       */
      //-----------------------------------------------------------------------
      void SetTrackPitchByEqualTemperament( u8 handleIdx, u32 trackBitFlag, s8 level ); 

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̃p�� ( ���E�̒�� ) ��ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param pan          �p���̒l
       *
       * @note �p���̒l
       *   -1.0 = ��
       *    0.0 = ��
       *    1.0 = �E
       */
      //-----------------------------------------------------------------------
      void SetTrackPan( u8 handleIdx, u32 trackBitFlag, f32 pan );

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N�̃T���E���h�p�� ( �O��̒�� ) ��ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param pan          �p���̒l
       u*/
      //-----------------------------------------------------------------------
      void SetTrackSurroundPan( u8 handleIdx, u32 trackBitFlag, f32 pan ); 

      //-----------------------------------------------------------------------
      /**
       * @brief �g���b�N���Ƃ̃o���N��ύX����
       *
       * @param handleIdx    ������s���n���h���̃C���f�b�N�X
       * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
       * @param bankIdx      �ݒ肷��o���N�̃C���f�b�N�X ( 0 �` 3 )
       */
      //-----------------------------------------------------------------------
      void SetTrackBankIndex( u8 handleIdx, u32 trackBitFlag, s32 bankIdx );




      // 3D �T�E���h�p�����[�^
      struct Sound3DParam 
      {
        s32 maxPriorityReduction; // �ő�v���C�I���e�B������
        f32 panRange;             // �p���̕ω��� ( 0.0 �` 1.0 )
        f32 sonicVelocity;        // ���� ( �h�b�v���[���ʂ̌v�Z�Ɏg�p ) [*/frame]
      };

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�T�E���h�̃p�����[�^��ݒ肷��
       *
       * @param param �ݒ肷��3D�T�E���h�p�����[�^
       */
      //-----------------------------------------------------------------------
      void Set3DParam( const Sound3DParam& param );

      //-----------------------------------------------------------------------
      /**
       * @brief ���ݐݒ肳��Ă���3D�T�E���h�̃p�����[�^���擾����
       *
       * @param[out] pOutParam �p�����[�^�̊i�[��
       */
      //-----------------------------------------------------------------------
      void Get3DParam( Sound3DParam* pOutParam ) const;




      // 3D �T�E���h ���X�i�[�p�����[�^
      struct ListenerParam 
      {
        f32 interiorSize;      // �C���e���A�T�C�Y
        f32 maxVolumeDistance; // ���ʂ��ő�ɂȂ�͈�
        f32 unitDistance;      // ���ʂ���������P�ʋ���
      };

      //-----------------------------------------------------------------------
      /**
       * @brief ���ݐݒ肳��Ă��郊�X�i�[�̃p�����[�^��ݒ肷��
       *
       * @param param �ݒ肷�郊�X�i�[�p�����[�^
       */
      //-----------------------------------------------------------------------
      void SetListenerParam( const ListenerParam& param );

      //-----------------------------------------------------------------------
      /**
       * @brief ���X�i�[�̃p�����[�^���擾����
       * 
       * @param[out] pOutParam �p�����[�^�̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetListenerParam( ListenerParam* pOutParam ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���X�i�[�s���ݒ肷��
       *
       * @param matrix �ݒ肷�郊�X�i�[�s��
       */
      //-----------------------------------------------------------------------
      void SetListenerMatrix( const gfl::math::MTX34& matrix );

      //-----------------------------------------------------------------------
      /**
       * @brief ���X�i�[�̍��W��ύX����
       *
       * @param pos ���X�i�[�̍��W
       * @param up  ���X�i�[�̏���x�N�g��
       * @param dir ���X�i�[�̌����x�N�g��
       */
      //-----------------------------------------------------------------------
      void SetListenerPos( 
          const gfl::math::VEC3& pos,
          const gfl::math::VEC3& up,
          const gfl::math::VEC3& dir );

      //-----------------------------------------------------------------------
      /**
       * @brief ���X�i�[�̑��x��ݒ肷��( �h�b�v���[���ʂɔ��f����܂� )
       *
       * @param velocity ���X�i�[�̈ړ����x
       *
       * @note
       * SetListenerMatrix(), SetListenerPos() ���Ŏ����I�Ɍv�Z����邽��, 
       * �ʏ�̓A�v���P�[�V�������Őݒ肷��K�v�͂���܂���.
       *
       * @note
       * ���X�i�[���W����u�ŉ����ɔ�Ԃ悤�ȏꍇ�Ɍ���,
       * ���x���������v�Z�ł��Ȃ�����
       * �A�v���P�[�V�������ŏ㏑������K�v������܂�.
       */
      //-----------------------------------------------------------------------
      void SetListenerVelocity( const gfl::math::VEC3& velocity );


      //-----------------------------------------------------------------------
      /**
       * @brief 3D�T�E���h���Đ�����
       *
       * @param actorIdx    �Đ���̐�����s��3D�A�N�^�[�̃C���f�b�N�X
       * @param itemID      �Đ�����T�E���h�̃A�C�e��ID
       * @param startInfo   �ڍׂȍĐ��p�����[�^
       *
       * @return �Đ������������ꍇ true
       */
      //-----------------------------------------------------------------------
      bool Play3DActor( u8 actorIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );
    
      //-------------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̍Đ������J�n
       *
       * @param actorIdx    �Đ���̐�����s��3D�A�N�^�[�̃C���f�b�N�X
       * @param itemID      �Đ�����T�E���h�̃A�C�e��ID
       * @param startInfo   �ڍׂȍĐ��p�����[�^(NULL�܂��͏ȗ���)
       *
       * @return ���łɍĐ��������ł��Ă���ꍇ true
       *
       * @note
       * ���̊֐���True���Ԃ����A�������́A
       * �Ăяo�����IsPrepared3DActor()��True���Ԃ�܂ő҂��Ă���PlayPrepared3DActor()�ōĐ����J�n���邱��
       *
       */
      //-------------------------------------------------------------------------
      bool Prepare3DActor( u8 actorIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );
    
      //-------------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̍Đ��������ł��Ă��邩�H
       *
       * @param actorIdx    3D�A�N�^�[�̃C���f�b�N�X
       *
       * @return ���łɍĐ��������ł��Ă���ꍇ true
       */
      //-------------------------------------------------------------------------
      bool IsPrepared3DActor( u8 actorIdx );
    
      //-------------------------------------------------------------------------
      /**
       * @brief �Đ��������ł��Ă���3D�A�N�^�[�̍Đ����J�n����
       *
       * @param actorIdx    3D�A�N�^�[�̃C���f�b�N�X
       *
       * @note
       * Prepar3DActor()�Ăяo����True���Ԃ����A��������
       * Prepar3DActor()�Ăяo����AIsPrepaed3DActor()�ő҂��ATrue���Ԃ�܂ő҂��Ă���Ăяo������
       */
      //-------------------------------------------------------------------------
      void PlayPrepared3DActor( u8 actorIdx );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�ōĐ����̑S�ẴT�E���h���~����
       *
       * @param actorIdx   ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param fadeFrames �t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void Stop3DActor( u8 actorIdx, s32 fadeFrames );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�ōĐ����̑S�ẴT�E���h���ꎞ��~�E�ĊJ����
       *
       * @param actorIdx   ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param pauseOn    true :�ꎞ��~, false:�ĊJ
       * @param fadeFrames �t�F�[�h�C���E�t�F�[�h�A�E�g�ɂ�����t���[����
       */
      //-----------------------------------------------------------------------
      void Pause3DActor( u8 actorIdx, bool pauseOn, s32 fadeFrames );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̉��ʂ�ύX����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param volume   �ݒ肷�鉹�ʂ̔{��( 0.0 �` 1.0 )
       */
      //-----------------------------------------------------------------------
      void Set3DActorVolume( u8 actorIdx, f32 volume );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̉��ʂ��擾����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       *
       * @return �w�肵��3D�A�N�^�[�ɐݒ肳��Ă��鉹�ʂ̔{��
       */
      //-----------------------------------------------------------------------
      f32 Get3DActorVolume( u8 actorIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̉�����ύX����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param pitch    ���g���䗦
       *
       * @note ���g���䗦 
       *   0.5 = 1 �I�N�^�[�u ��
       *   1.0 = ���{( �f�t�H���g�l )
       *   2.0 = 1 �I�N�^�[�u ��
       */
      //-----------------------------------------------------------------------
      void Set3DActorPitch( u8 actorIdx, f32 pitch );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̉������擾����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       *
       * @return �w�肵��3D�A�N�^�[�ɐݒ肳��Ă�����g���䗦
       *
       * @note���g���䗦 
       *   0.5 = 1 �I�N�^�[�u ��
       *   1.0 = ���{ ( �f�t�H���g�l )
       *   2.0 = 1 �I�N�^�[�u ��
       */
      //-----------------------------------------------------------------------
      f32 Get3DActorPitch( u8 actorIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̃p�� ( ���E�̒�� ) ��ύX����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param pan      �p���̒l
       *
       * @note �p���̒l
       *   -1.0 = ��
       *    0.0 = ��
       *    1.0 = �E
       */
      //-----------------------------------------------------------------------
      void Set3DActorPan( u8 actorIdx, f32 pan );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̃p��( ���E�̒�� )���擾����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       *
       * @return �w�肵��3D�A�N�^�[�ɐݒ肳��Ă���p���̒l
       *
       * @note �p���̒l
       *   -1.0 = ��
       *    0.0 = ��
       *    1.0 = �E
       */
      //-----------------------------------------------------------------------
      f32 Get3DActorPan( u8 actorIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̍��W��ύX����
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param pos      �ݒ肷����W
       */
      //-----------------------------------------------------------------------
      void Set3DActorPos( u8 actorIdx, const gfl::math::VEC3& pos );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̍��W���擾����
       *
       * @param[in]  actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param[out] pos      �擾�������W�̊i�[��
       */
      //-----------------------------------------------------------------------
      void Get3DActorPos( u8 actorIdx, gfl::math::VEC3* pos ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�̑��x��ݒ肷��( �h�b�v���[���ʂɔ��f����܂� )
       *
       * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param pos      �ݒ肷����W
       *
       * @note
       * Set3DActorPos() �ō��W��ݒ肵���ۂ�
       * �o�^���̍������玩���I�Ɍv�Z����邽��,
       * �ʏ�̓A�v���P�[�V�������Őݒ肷��K�v�͂���܂���.
       *
       * @note
       * �A�N�^�[���W����u�ŉ����ɔ�Ԃ悤�ȏꍇ�Ɍ���, 
       * ���x���������v�Z�ł��Ȃ�����
       * �A�v���P�[�V�������ŏ㏑������K�v������܂�.
       */
      //-----------------------------------------------------------------------
      void Set3DActorVelocity( u8 actorIdx, const gfl::math::VEC3& velocity );

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[�ōĐ����̃T�E���h�����擾����
       * @param actorIdx        ������s��3D�A�N�^�[�̃C���f�b�N�X
       * @param actorPlayerIdx  �Ώۂ̃A�N�^�[�v���C���[�̃C���f�b�N�X
       */
      //-----------------------------------------------------------------------
      u32 Get3DActorPlayingSoundCount( u8 actorIdx, u8 actorPlayerIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3D�A�N�^�[���S�A�N�^�[�v���C���[�ōĐ����̃T�E���h�����擾����
       * @param actorIdx  ������s��3D�A�N�^�[�̃C���f�b�N�X
       */
      //-----------------------------------------------------------------------
      u32 Get3DActorPlayingSoundCount( u8 actorIdx ) const;



      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�ɐݒ肳��Ă��郆�[�U�p�����[�^���擾
       *
       * @param itemID  �擾����A�C�e��ID
       */
      //-----------------------------------------------------------------------
      u32 GetSoundUserParam( u32 itemID );

      //-----------------------------------------------------------------------
      /*
       * @brief �V�[�P���X�ϐ����擾
       *
       * @param[in]  handleIdx �l���擾����T�E���h�n���h���C���f�b�N�X
       * @param[in]  varNo     �ϐ��i���o�[(0~15)
       * @param[out] var       �擾�����l���i�[���郏�[�N�ւ̃|�C���^
       * 
       * @retval  true:�擾�����@false:�擾���s
       */
      //-----------------------------------------------------------------------
      bool ReadVariable( u8 handleIdx, int varNo, s16* var );

      //-----------------------------------------------------------------------
      /*
       * @brief �V�[�P���X�ϐ��Ɋi�[
       *
       * @param[in] handleIdx �l���i�[����T�E���h�n���h���C���f�b�N�X
       * @param[in] varNo     �ϐ��i���o�[(0~15)
       * @param[in] var       �i�[����l
       *
       * @retval  true:�i�[�����@false:�i�[���s
       */
      //-----------------------------------------------------------------------
      bool WriteVariable( u8 handleIdx, int varNo, s16 var );

      //-----------------------------------------------------------------------
      /*
       * @brief �V�[�P���X�O���[�o���ϐ����擾
       *
       * @param[in]  handleIdx �l���擾����T�E���h�n���h���C���f�b�N�X
       * @param[in]  varNo     �ϐ��i���o�[(0~15)
       * @param[out] var       �擾�����l���i�[���郏�[�N�ւ̃|�C���^
       * 
       * @retval  true:�擾�����@false:�擾���s
       */
      //-----------------------------------------------------------------------
      bool ReadGlobalVariable( u8 handleIdx, int varNo, s16* var );

      //-----------------------------------------------------------------------
      /*
       * @brief �V�[�P���X�O���[�o���ϐ��Ɋi�[
       *
       * @param[in] handleIdx �l���i�[����T�E���h�n���h���C���f�b�N�X
       * @param[in] varNo     �ϐ��i���o�[(0~15)
       * @param[in] var       �i�[����l
       *
       * @retval  true:�i�[�����@false:�i�[���s
       */
      //-----------------------------------------------------------------------
      bool WriteGlobalVariable( u8 handleIdx, int varNo, s16 var );

      //-----------------------------------------------------------------------
      /*
       * @brief �T�E���h�^�C�v�擾
       *
       * @param[in] handleIdx �擾����T�E���h�n���h���C���f�b�N�X
       *
       * @retval  nw::snd::SoundArchive::SoundType
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundArchive::SoundType GetSoundType( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /*
       * @brief �T�E���h�^�C�v�擾
       *
       * @param[in] itemID  ���ׂ�T�E���h��ID
       *
       * @retval  nw::snd::SoundArchive::SoundType
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundArchive::SoundType GetSoundTypeOfItem( u32 itemID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �G�t�F�N�g��AUX�o�X�ɒǉ����܂��B
       *
       * @param busID �o�XID�iA,B����܂��j
       * @param effect  �G�t�F�N�g�̒��ۃN���X���p����������
       *
       * @return true�ǉ��ɐ��� false�ǉ��Ɏ��s
       */
      //-----------------------------------------------------------------------
      bool AppendEffect( nw::snd::AuxBus busID, nw::snd::FxBase *effect );
      
      //-----------------------------------------------------------------------
      /**
       * @brief �G�t�F�N�g��AUX�o�X����폜���܂��B
       *
       * @param busID �o�XID�iA,B����܂��j
       * @param �t�F�[�h���ԁi�~���b�j
       */
      //-----------------------------------------------------------------------
      void RemoveEffect( nw::snd::AuxBus busID, int fadeTimes );

      //-----------------------------------------------------------------------
      /**
       * @brief ���o�[�u�G�t�F�N�g��AUX�o�X�ɒǉ����܂��B
       *
       * @param busID �o�XID�iA,B����܂��j
       * @param reverb_buffer_heap  ���o�[�u�p�̃o�b�t�@�擾�p�q�[�v
       * @param param �p����
       *
       * @return true�ǉ��ɐ��� false�ǉ��Ɏ��s
       */
      //-----------------------------------------------------------------------
      bool AppendReverbEffect( nw::snd::AuxBus busID, gfl::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param );
      
      //-----------------------------------------------------------------------
      /**
       * @brief �G�t�F�N�g��AUX�o�X����폜���܂��B
       *        RemoveEffect�Ƃ̈Ⴂ�́A���݂��Ă���΃��o�[�u�p�o�b�t�@��������邱�Ƃł�
       *
       * @param busID �o�XID�iA,B����܂��j
       * @param �t�F�[�h���ԁi�~���b�j
       */
      //-----------------------------------------------------------------------
      void RemoveReverbEffect( nw::snd::AuxBus busID, int fadeTimes );

      protected:
      gfl::heap::HeapBase* m_pAppHeap;
      gfl::heap::HeapBase* m_pDevHeap;

      nw::snd::SoundArchive* m_pSoundArchive;
      nw::snd::RomSoundArchive m_romSoundArchive; 
      nw::snd::MemorySoundArchive m_memSoundArchive;
      nw::snd::SoundDataManager m_soundDataManager;
      nw::snd::SoundArchivePlayer m_soundArchivePlayer;
      gfl::snd::SoundHeap* m_soundHeap;

      gfl::snd::SoundDataManagerThread* m_pSoundDataManagerThread;
      bool                              m_SoundDataManagerThreadResult;
      nn::os::CriticalSection m_criticalSectionForSoundDataManagerLoad;
      
      nw::snd::SoundHandle* m_pSoundHandles;
      u8 m_soundHandleNum;

      nw::snd::Sound3DManager m_sound3DManager;
      nw::snd::Sound3DListener m_sound3DListener;
      nw::snd::Sound3DActor* m_pSound3DActors;
      nw::snd::SoundHandle* m_pSound3DHandles;
      u8 m_sound3DActorNum;
      
      
      nw::snd::FxReverb m_fxReverb; 
      void* m_pMemoryForSoundReverb;

      void* m_pMemoryForDspComponent;
      void* m_pMemoryForAacLibrary;
      void* m_pMemoryForSoundSystem;
      void* m_pMemoryForSoundArchiveHeader;
      void* m_pMemoryForSoundDataManager;
      void* m_pMemoryForSoundArchivePlayer;
      void* m_pMemoryForStreamBuffer;
      void* m_pMemoryForCacheBuffer;
      void* m_pMemoryForSoundHeap; 
      void* m_pMemoryForSound3DManager;

      bool m_isAacLibraryInitialized;  // AAC ���C�u�������������������H

      // 12���ϗ��ł̎��g����z��
      static const u8 EQUAL_TEMPERAMENT_LEVEL_NUM = ONE_OCTAVE_STEP_COUNT + 1;
      static const f32 m_highFrequencyRatioForEqualTemperament[ EQUAL_TEMPERAMENT_LEVEL_NUM ];  
      static const f32 m_lowFrequencyRatioForEqualTemperament[ EQUAL_TEMPERAMENT_LEVEL_NUM ];  

      protected:
      // �X���b�h�ǂݍ��݂��s���̂ŁA�O�����J���Ȃ��B
      nw::snd::SoundDataManager* GetSoundDataManager( void );

      private:
      void InitDspComponent( const InitParam& );
      void InitSDKLibrary( void );
      void InitSoundSystem( const InitParam& );
      void InitSoundArchive( const InitParam& );
      void InitRomSoundArchive( const InitParam& );
      void InitMemorySoundArchive( const InitParam& );
      void InitSoundDataManager( void );
      void InitSoundArchivePlayer( void );
      void InitSoundHeap( const InitParam& );
      void CreateSoundHandles( const InitParam& );
      void InitSound3DManager( void );
      void InitSound3DListener( void );
      void CreateSound3DActorsAndHandles( const InitParam& );
      void InitSound3DActors( void ); 
      void DeleteSoundHandles( void );
      void DeleteSound3DActorsAndHandles( void ); 
      nw::snd::SoundArchive::SoundType GetAttachedSoundType( const nw::snd::SoundHandle* ) const; 
      f32 CalcPitchByEqualTemperament( s8 ) const;


#if GFL_HIO_ENABLE
      private:
      nw::snd::HioSoundArchive m_hioSoundArchive;
      void InitHioSoundArchive( const InitParam& initParam );
#endif


      public:
      //-----------------------------------------------------------------------
      /**
       * @brief �e�탁���o���擾����
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundArchive* GetSoundArchive( void );
      nw::snd::SoundArchivePlayer* GetSoundArchivePlayer( void );
      nw::snd::SoundHandle* GetSoundHandle( u8 handleIdx );
      nw::snd::SoundHeap* GetSoundHeap( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �T�E���h�q�[�v�̓��e���o�͂���
       */
      //-----------------------------------------------------------------------
      void DumpSoundHeap( void );
    };


  } //namespace snd
} //namespace gfl

#endif //__GFL_SOUNDSYSTEM_H__
