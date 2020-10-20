//=============================================================================
/**
 * @brief アーカイブ読み込みスレッド
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
        void* dest_buf;												// [in]  データ格納先アドレス
        u32		dest_size;                      // [in]  データ格納先サイズ
        ArcFile* arcfile;                     // [in]  読み込むデータが属するアーカイブファイル
        ArcFile::ARCDATID datID;              // [in]  読み込むデータのアーカイブ内データID
        bool compressed;                      // [in]  true:圧縮されている, false:圧縮されていない
        gfl::heap::HeapBase* heap_for_arc;    // [in]  アーカイブファイルの高速化テーブルを確保するヒープ
        gfl::heap::HeapBase* heap_for_uncomp; // [in]  圧縮データの解凍作業に使用するヒープ
				u32* read_size;                       // [out] 実際に読み込んだデータサイズ 2012/07/26 by nakahiro

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
       * @brief コンストラクタ
       * @param heap_for_thread スレッドの動作に使用するヒープ
       * @param desc            アーカイブ読み込みの詳細情報
       */
      ArcFileReadThreadStatic( 
          gfl::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief デストラクタ
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
