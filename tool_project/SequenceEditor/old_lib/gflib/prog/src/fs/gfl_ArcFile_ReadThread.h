//=============================================================================
/**
 * @brief �A�[�J�C�u�ǂݍ��݃X���b�h
 * @file gfl_ArcFile_ReadThread.h
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#ifndef __GFL_ARCFILE_READTHREAD_H__
#define __GFL_ARCFILE_READTHREAD_H__
#include <base/gfl_Thread.h>

namespace gfl {
  namespace fs {

    class Thread;


    class ArcFileReadThread : public base::Thread
    {
      GFL_FORBID_COPY_AND_ASSIGN( ArcFileReadThread );

      public:

      struct Description 
      {
        void** dest_buf;                      // [out] �ǂݍ��񂾃f�[�^�̊i�[��A�h���X
        u32* dest_size;                       // [out] �ǂݍ��񂾃f�[�^�̃T�C�Y( �s�v�Ȃ�NULL )
        ArcFile* arcfile;                     // [in]  �ǂݍ��ރf�[�^��������A�[�J�C�u�t�@�C��
        ArcFile::ARCDATID datID;              // [in]  �ǂݍ��ރf�[�^�̃A�[�J�C�u���f�[�^ID
        bool compressed;                      // [in]  true:���k����Ă���, false:���k����Ă��Ȃ�
        gfl::heap::HeapBase* heap_for_arc;    // [in]  �A�[�J�C�u�t�@�C���̍������e�[�u�����m�ۂ���q�[�v
        gfl::heap::HeapBase* heap_for_buf;    // [in]  �ǂݍ��񂾃f�[�^���i�[����m�ۂ���q�[�v
        gfl::heap::HeapBase* heap_for_uncomp; // [in]  ���k�f�[�^�̉𓀍�ƂɎg�p����q�[�v
        u32 buf_alignment;                    // [in]  �ǂݍ��ݐ�o�b�t�@�̃A���C�������g

        Description( void ) :
          dest_buf( NULL ),
          dest_size( NULL ),
          arcfile( NULL ),
          datID( 0 ),
          compressed( false ),
          heap_for_arc( NULL ),
          heap_for_buf( NULL ),
          heap_for_uncomp( NULL ),
          buf_alignment( 0 )
        { }
      };

      /**
       * @brief �R���X�g���N�^
       * @param heap_for_thread �X���b�h�̓���Ɏg�p����q�[�v
       * @param desc            �A�[�J�C�u�ǂݍ��݂̏ڍ׏��
       */
      ArcFileReadThread( 
          gfl::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~ArcFileReadThread();


      private:

      static const u32 STACK_SIZE;
      Description m_desc;

      //virtual void Initialize( void );
      virtual void Main( void ); 
      //virtual void Finalize( void );
      //virtual void OnKill( void );

    };


  }  // namespace fs
}  // namespace gfl

#endif  // __GFL_ARCFILE_READTHREAD_H__
