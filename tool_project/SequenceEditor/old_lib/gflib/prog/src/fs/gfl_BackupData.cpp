//========================================================================================
/**
 * @file   gfl_BackupData.cpp
 * @brief  バックアップデータ
 * @author obata_toshihiro
 * @date   2011.02.21
 */
//========================================================================================
#include <gfl_Base.h>
#include <gfl_Math.h>
#include <gfl_Heap.h>
#include <fs/gfl_BackupData.h>


namespace gfl {
  namespace fs { 


    //--------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap       バッファの確保に使用するヒープ
     * @param data_form  バックアップデータを定義する情報
     */
    //--------------------------------------------------------------------------------
    BackupData::BackupData( gfl::heap::HeapBase* heap, const DataForm* data_form ) : 
      m_data_form( data_form ),
      m_block_info( NULL ),
      m_buffer( NULL )
    {
      // ブロック情報の登録・バッファの確保
      m_block_info = GFL_NEW_ARRAY(heap) BlockInfo[ data_form->block_num ]; 
      size_t buffer_size = 0;
      for( int i=0; i<data_form->block_num; i++ )
      {
        m_block_info[i].offset_address = buffer_size;
        buffer_size += this->GetBlockSize(i);
      } 
      m_buffer = GflHeapAllocMemoryAlign( heap, buffer_size, BLOCK_ALIGNMENT );

      // ブロック初期化
      this->Clear();
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //--------------------------------------------------------------------------------
    BackupData::~BackupData( void )
    {
      if( m_block_info ) {
        GFL_DELETE_ARRAY( m_block_info );
      } 
      if( m_buffer ) {
        GflHeapFreeMemory( m_buffer );
      }
    }


    //--------------------------------------------------------------------------------
    /**
     * @brief 同じデータを持つインスタンスを生成する
     *
     * @param heap  インスタンスの生成に使用するヒープ
     *
     * @return 生成したインスタンス
     */
    //--------------------------------------------------------------------------------
    BackupData* BackupData::CreateClone( gfl::heap::HeapBase* heap ) const
    { 
      BackupData* clone = GFL_NEW(heap) BackupData( heap, this->m_data_form );
      this->CopyTo( clone );
      return clone;
    }


    //--------------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     *
     * @param dest  コピー先のインスタンス
     *
     * @retval true   コピー成功
     * @retval false  コピー失敗
     *
     * @note コピー元とコピー先は同じデータである必要があります.
     */
    //--------------------------------------------------------------------------------
    bool BackupData::CopyTo( BackupData* dest ) const
    {
      if( ( this->m_data_form->form_id != dest->m_data_form->form_id ) ||
          ( this->m_data_form->block_num != dest->m_data_form->block_num ) ||
          ( this->GetBufferSize() != dest->GetBufferSize() ) )
      {
        GFL_ASSERT(0);  // データフォーマットが一致しない
        return false;  
      } 

      gfl::std::MemCopy( this->m_buffer, dest->m_buffer, this->GetBufferSize() );
      return true;
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief バッファのサイズを取得する
     */
    //--------------------------------------------------------------------------------
    size_t BackupData::GetBufferSize( void ) const
    {
      size_t buffer_size = 0; 
      for( int i=0; i<m_data_form->block_num; i++ ) {
        buffer_size += GetBlockSize(i);
      }
      return buffer_size;
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief ブロックサイズを取得する
     *
     * @param index  対象ブロックのインデックス
     *
     * @note
     * 各ブロックの先頭アドレスが4バイトアラインメントになるように,
     * 各ブロックのサイズは4の倍数になるように切り上げる.
     */
    //--------------------------------------------------------------------------------
    size_t BackupData::GetBlockSize( BlockIndex index ) const
    {
      size_t size = m_data_form->block_form[index].size_get_func();
      return gfl::math::RoundUp( size, BLOCK_ALIGNMENT );
    } 


    //--------------------------------------------------------------------------------
    /**
     * @brief クリアする( 初期状態にする )
     */
    //--------------------------------------------------------------------------------
    void BackupData::Clear( void )
    {
      for( int i=0; i<m_data_form->block_num; i++ )
      {
        this->ClearBlock(i);
      }
    }

    /**
     * @brief 指定したブロックを初期化する
     */
    void BackupData::ClearBlock( BlockIndex index )
    {
      if( m_data_form->block_num <= index )
      {
        GFL_ASSERT(0);
        return;
      }

      void* data_ptr = this->GetBlockPointer(index);
      m_data_form->block_form[index].clear_func( data_ptr );
    } 

    //--------------------------------------------------------------------------------
    /**
     * @brief ブロックの先頭アドレスを取得する
     *
     * @param index  対象ブロックのインデックス
     */
    //--------------------------------------------------------------------------------
    void* BackupData::GetBlockPointer( BlockIndex index ) const
    {
      if( m_data_form->block_num <= index )
      {
        GFL_ASSERT(0);
        index = 0;  // インデックス0のブロックアドレスを返す
      } 

      uptr address = reinterpret_cast<uptr>( m_buffer ) + m_block_info[index].offset_address; 
      return reinterpret_cast<void*>( address ); 
    }



  }  // namespace fs
}  // namespace gfl 
