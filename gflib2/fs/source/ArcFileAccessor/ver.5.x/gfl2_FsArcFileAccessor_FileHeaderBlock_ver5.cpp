//=============================================================================
/**
 * @brief  アーカイブファイルのヘッダブロック
 * @file   gfl2_FsArcFileAccessor_FileHeaderBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_ver5.h"
#include "./gfl2_FsArcFileAccessor_FileHeaderBlock_ver5.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver5)


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


GFL_NAMESPACE_END(ver5)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
