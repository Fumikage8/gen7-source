//=============================================================================
/**
 * @brief  アーカイブファイルのヘッダブロック
 * @file   gfl_ArcFileAccessor_FileHeaderBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileHeaderBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 {


      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileHeaderBlock::ArcFileHeaderBlock( void ) :
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileHeaderBlock::~ArcFileHeaderBlock( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief ブロックのサイズ[Byte]を計算する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileHeaderBlock::CalcBlockSize( void )
      {
        return sizeof( ArcFileHeaderBlock::Structure );
      }

      /**
       * @brief ブロックデータをヘッダとして取得する
       */
      const ArcFileHeaderBlock::Structure* ArcFileHeaderBlock::GetBlockDataAsHeaderStructure( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileHeaderBlock::Structure*>( block_data );
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
