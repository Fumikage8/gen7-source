//=============================================================================
/**
 * @brief  アーカイブファイルのファイルイメージブロック
 * @file   gfl_ArcFileAccessor_FileImageBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEIMAGEBLOCK_V3_H__
#define __GFL_ARCFILEACCESSOR_FILEIMAGEBLOCK_V3_H__
#include "gfl_ArcFileAccessor_FileBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 {


      class ArcFileImageBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileImageBlock );
        friend class ArcFileFactory;

        public:

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのサイズ[Byte]を計算する
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( void ); 

        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //---------------------------------------------------------------------
        ArcFileImageBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //---------------------------------------------------------------------
        ~ArcFileImageBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief イメージデータのサイズを取得する
         */
        //---------------------------------------------------------------------
        u32 ImageSize( void ) const;



        private:

        // ブロックの構成
        struct Attribute 
        {
          ArcFileBlock::Header header;  // ブロックのヘッダ
          u32 image_size;            // ファイルイメージの大きさ
        };

        /**
         * @brief ブロックデータを構成データとして取得する
         */
        const Attribute* GetAttribute( void ) const;
      };


    } // namespace ver3
  } // namespace fs
} // namespace gfl


#endif  // __GFL_ARCFILEACCESSOR_FILEIMAGEBLOCK_V3_H__
