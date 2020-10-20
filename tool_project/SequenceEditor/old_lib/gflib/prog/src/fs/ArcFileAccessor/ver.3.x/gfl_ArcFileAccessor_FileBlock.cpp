//=============================================================================
/**
 * @brief  アーカイブファイルを構成するブロックの基本クラス
 * @file   gfl_ArcFileAccessor_FileBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor_FileBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 {


      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileBlock::ArcFileBlock( void ) :
        m_block_data( NULL )
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileBlock::~ArcFileBlock( void )
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief ブロックの識別子を取得する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileBlock::Signature( void ) const
      {
        const ArcFileBlock::Header* header = reinterpret_cast<const ArcFileBlock::Header*>( m_block_data );
        return header->signature;
      }

      //-----------------------------------------------------------------------
      /**
       * @brief ブロックのサイズ[Byte]を取得する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileBlock::BlockSize( void ) const
      {
        const ArcFileBlock::Header* header = reinterpret_cast<const ArcFileBlock::Header*>( m_block_data );
        return header->block_size;
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
