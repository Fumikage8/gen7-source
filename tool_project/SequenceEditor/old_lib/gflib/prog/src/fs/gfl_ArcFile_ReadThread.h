//=============================================================================
/**
 * @brief アーカイブ読み込みスレッド
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
        void** dest_buf;                      // [out] 読み込んだデータの格納先アドレス
        u32* dest_size;                       // [out] 読み込んだデータのサイズ( 不要ならNULL )
        ArcFile* arcfile;                     // [in]  読み込むデータが属するアーカイブファイル
        ArcFile::ARCDATID datID;              // [in]  読み込むデータのアーカイブ内データID
        bool compressed;                      // [in]  true:圧縮されている, false:圧縮されていない
        gfl::heap::HeapBase* heap_for_arc;    // [in]  アーカイブファイルの高速化テーブルを確保するヒープ
        gfl::heap::HeapBase* heap_for_buf;    // [in]  読み込んだデータを格納先を確保するヒープ
        gfl::heap::HeapBase* heap_for_uncomp; // [in]  圧縮データの解凍作業に使用するヒープ
        u32 buf_alignment;                    // [in]  読み込み先バッファのアラインメント

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
       * @brief コンストラクタ
       * @param heap_for_thread スレッドの動作に使用するヒープ
       * @param desc            アーカイブ読み込みの詳細情報
       */
      ArcFileReadThread( 
          gfl::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief デストラクタ
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
