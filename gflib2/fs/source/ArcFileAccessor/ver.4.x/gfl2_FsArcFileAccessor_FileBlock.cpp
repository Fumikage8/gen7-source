//=============================================================================
/**
 * @brief  アーカイブファイルを構成するブロックの基本クラス
 * @file   gfl2_FsArcFileAccessor_FileBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_FileBlock.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver4)


      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileBlock::ArcFileBlock( void ) :
        m_block_data( 0/*NULL*/ )
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


GFL_NAMESPACE_END(ver4)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
