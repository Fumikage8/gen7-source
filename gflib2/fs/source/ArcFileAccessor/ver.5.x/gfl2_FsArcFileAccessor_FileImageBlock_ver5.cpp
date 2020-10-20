//=============================================================================
/**
 * @brief  アーカイブファイルのファイルイメージブロック
 * @file   gfl2_FsArcFileAccessor_FileImageBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_ver5.h"
#include "./gfl2_FsArcFileAccessor_FileImageBlock_ver5.h" 


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver5)


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


GFL_NAMESPACE_END(ver5)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
