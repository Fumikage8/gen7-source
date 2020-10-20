#if !defined(V4_GFL2_FSARCFILEACCESSOR_FILEIMAGEBLOCK_H_INCLUDED)
#define V4_GFL2_FSARCFILEACCESSOR_FILEIMAGEBLOCK_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルのファイルイメージブロック
 * @file   gfl2_FsArcFileAccessor_FileImageBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_FileBlock.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver4)


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


GFL_NAMESPACE_END(ver4)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // V4_GFL2_FSARCFILEACCESSOR_FILEIMAGEBLOCK_H_INCLUDED
