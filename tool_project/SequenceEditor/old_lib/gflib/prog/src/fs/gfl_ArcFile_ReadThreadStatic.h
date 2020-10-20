//=============================================================================
/**
 * @brief �A�[�J�C�u�ǂݍ��݃X���b�h
 * @file gfl_ArcFile_ReadThread.h
 * @author obata_toshihiro
 * @date 2012.01.08
 */
//=============================================================================
#ifndef __GFL_ARCFILE_READTHREADSTATIC_H__
#define __GFL_ARCFILE_READTHREADSTATIC_H__
#include <base/gfl_Thread.h>

namespace gfl {
  namespace fs {

    class Thread;


    class ArcFileReadThreadStatic : public base::Thread
    {
      GFL_FORBID_COPY_AND_ASSIGN( ArcFileReadThreadStatic );

      public:

      struct Description 
      {
        void* dest_buf;												// [in]  �f�[�^�i�[��A�h���X
        u32		dest_size;                      // [in]  �f�[�^�i�[��T�C�Y
        ArcFile* arcfile;                     // [in]  �ǂݍ��ރf�[�^��������A�[�J�C�u�t�@�C��
        ArcFile::ARCDATID datID;              // [in]  �ǂݍ��ރf�[�^�̃A�[�J�C�u���f�[�^ID
        bool compressed;                      // [in]  true:���k����Ă���, false:���k����Ă��Ȃ�
        gfl::heap::HeapBase* heap_for_arc;    // [in]  �A�[�J�C�u�t�@�C���̍������e�[�u�����m�ۂ���q�[�v
        gfl::heap::HeapBase* heap_for_uncomp; // [in]  ���k�f�[�^�̉𓀍�ƂɎg�p����q�[�v
				u32* read_size;                       // [out] ���ۂɓǂݍ��񂾃f�[�^�T�C�Y 2012/07/26 by nakahiro

        Description( void ) :
          dest_buf( NULL ),
          dest_size( 0 ),
          arcfile( NULL ),
          datID( 0 ),
          compressed( false ),
          heap_for_arc( NULL ),
          heap_for_uncomp( NULL ),
          read_size( NULL )
        { }
      };

      /**
       * @brief �R���X�g���N�^
       * @param heap_for_thread �X���b�h�̓���Ɏg�p����q�[�v
       * @param desc            �A�[�J�C�u�ǂݍ��݂̏ڍ׏��
       */
      ArcFileReadThreadStatic( 
          gfl::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~ArcFileReadThreadStatic();


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
