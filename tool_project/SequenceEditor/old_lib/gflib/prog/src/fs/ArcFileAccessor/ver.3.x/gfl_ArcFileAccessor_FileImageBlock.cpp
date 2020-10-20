//=============================================================================
/**
 * @brief  アーカイブファイルのファイルイメージブロック
 * @file   gfl_ArcFileAccessor_FileImageBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileImageBlock.h" 


namespace gfl {
  namespace fs {
    namespace ver3 {


      //-----------------------------------------------------------------------
      /**
       * @brief ブロックのサイズ[Byte]を計算する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileImageBlock::CalcBlockSize( void )
      {
        return  sizeof(ArcFileImageBlock::Attribute);
      }

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileImageBlock::ArcFileImageBlock( void ) :
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileImageBlock::~ArcFileImageBlock( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief イメージデータのサイズを取得する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileImageBlock::ImageSize( void ) const 
      { 
        return this->GetAttribute()->image_size; 
      } 

      /**
       * @brief ブロックデータを構成データとして取得する
       */
      const ArcFileImageBlock::Attribute* ArcFileImageBlock::GetAttribute( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileImageBlock::Attribute*>( block_data );
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
