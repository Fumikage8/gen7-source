/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Vertex.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_VERTEX_H_
#define NN_GR_CTR_GR_VERTEX_H_

#include "gr_gl.h"
#include "util_SizedEnum.h"
#include "gr_Utility.h"
#include "gr_BindSymbol.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief 頂点バッファオブジェクト関連の設定のためのクラスです。
             */
            class Vertex
            {
            public :
                //! 頂点属性の最大数は12です（v0,v1,...,v11）。
                static const u32 VERTEX_ATTRIBUTE_MAX           = 12;

                //! 頂点属性の要素数の最大数は4です（x,y,z,w）。
                static const u32 VERTEX_ATTRIBUTE_DIMENSION_MAX = 4;

                //! 頂点有効化のコマンド最大数です。
                static const u32 VERTEX_ENABLE_COMMAND_MAX = 12 + VERTEX_ATTRIBUTE_MAX * 6;

                /*!
                    @brief インターリーブ情報を設定するためのクラスです。
                */
                class InterleaveInfo
                {
                public:
                    /*!
                        @brief コンストラクタです。
                    */
                    InterleaveInfo()
                    {
                        dataNum = 0;

                        for ( u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++ )
                        {
                            // 0x0 で初期化しておきます。
                            dataType[ index ] = PICA_DATA_SIZE_1_BYTE;
                        }

                        for ( u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++ )
                        {
                            // NULL で初期化しておきます。
                            symbol[ index ] = NULL;
                        }
                    }

                    /*!
                        @brief データ数です。
                    */
                    u8                            dataNum;
                    NN_PADDING3;

                    /*!
                        @brief 頂点データの型の配列です。
                               型は @ref PicaDataVertexAttrType です。
                    */
                    util::SizedEnum1<PicaDataVertexAttrType>       dataType[ VERTEX_ATTRIBUTE_MAX ];

                    /*!
                        @brief 頂点シェーダの入力のシンボルの配列です。
                               型は nn::gr::CTR::BindSymbolVSInput[] です。
                    */
                    const nn::gr::CTR::BindSymbolVSInput* symbol[ VERTEX_ATTRIBUTE_MAX ];
                };

            public :
                /*!
                    @brief 頂点インデックスのストリーム設定のためのクラスです。
                 */
                class IndexStream
                {
                public :
                    /*!
                        @brief コンストラクタです。
                    */
                    IndexStream() :
                    physicalAddr( 0 ),
                    drawVtxNum( 0 ),
                    isUnsignedByte( false )
                    {
                    }

                    /*!
                        @brief 頂点ストリームの物理アドレスの設定です。型は uptr です。
                    */
                    uptr  physicalAddr;

                    /*!
                        @brief 描画頂点数の設定です。型は u32 です。
                    */
                    u32  drawVtxNum;

                    /*!
                        @brief インデックスの要素がGLubyteの場合、 true を設定します。型は bool です。
                    */
                    bool isUnsignedByte;
                    NN_PADDING3;
                };

            public: // 頂点属性の設定
                /*!
                    @brief ロードアレイを設定します。DMPGLにおける glEnableVertexAttribArray() に対応します。

                    @param[in] symbol        頂点シェーダの入力のシンボルです。
                    @param[in] physical_addr ロードアレイの物理アドレスを指定します。
                    @param[in] type          各要素の型、および、要素数をもとに配列の型を指定します。
                 */
                void EnableAttrAsArray( const nn::gr::CTR::BindSymbolVSInput& symbol, const uptr physical_addr, const PicaDataVertexAttrType type );

                /*!
                    @brief 固定頂点を設定します。DMPGLにおける glVertexAttrib[N]fv()に対応します。
                           バインドする入力レジスタは、ShaderクラスのSearchBindSymbol()で調べることが出来ます。

                    @param[in] symbol        頂点シェーダの入力のシンボルです。
                    @param[in] dimension     要素の数を指定します。
                    @param[in] param       固定頂点パラメータを指定します。param[0],[1],[2],[3]が、それぞれ、x,y,z,wに対応します。

                    @see       Shader::SearchBindSymbol()
                */
                void EnableAttrAsConst( const nn::gr::CTR::BindSymbolVSInput& symbol, const u8 dimension, const f32 param[] );

                /*!
                    @brief インターリーブアレイを設定します。

                    @param[in] interleave_info インターリーブ情報の構造体です。
                    @param[in] physical_addr   物理アドレスを設定します。
                */
                void EnableInterleavedArray( const nn::gr::CTR::Vertex::InterleaveInfo& interleave_info, const uptr physical_addr );

            public : // 設定からコマンドを生成する
                /*!
                    @brief 設定された情報をもとに、頂点属性の有効化コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */

                bit32* MakeEnableAttrCommand( bit32* command ) const
                {
                    if ( m_CmdCacheVertexNum == 0 )
                    { 
                        // キャッシュの生成
                        m_CmdCacheVertexNum = MakeEnableAttrCommand_( m_CmdCacheVertex ) - m_CmdCacheVertex;
                    }

                    std::memcpy( command, m_CmdCacheVertex, m_CmdCacheVertexNum * sizeof( bit32 ) );

										bit32*			pSeek = command + m_CmdCacheVertexNum;

                    return pSeek;
                }

                /*!
                    @brief 設定された情報をもとに、描画コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] index_stream インデックスのストリームです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeDrawCommand( bit32* command, const IndexStream& index_stream ) const;

                /*!
                    @brief     有効な頂点属性を無効化するコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeDisableAttrCommand( bit32* command ) const
                {
                    for ( int index = 0; index < VERTEX_ATTRIBUTE_MAX; ++index )
                    {
                        // 有効なロードアレイを無効化するコマンドの生成
                        if ( m_LoadArray[ index ].IsEnable() )
                        {
                            // 0x203 + 3N, ロードアレイのアドレスオフセットを無効化するコマンドの生成
                            *command++ = 0;
                            *command++ =
                                PICA_CMD_HEADER_BURSTSEQ( PICA_REG_LOAD_ARRAY0_ATTR_OFFSET + 3 * index, 0x3 );
                            // 0x204 + 3N, ロードアレイの 0-7 番目の要素を無効化するコマンドの生成
                            *command++ = 0;
                            // 0x205 + 3N [15: 0]ロードアレイの 8-11 番目の要素を無効化するコマンドの生成
                            //            [23:16] ロードアレイの 1頂点のバイト数を無効化するコマンドの生成
                            //            [31:28] ロードアレイの 要素数を無効化するコマンドの生成
                            *command++ = 0;
                        }
                        else
                        {
                            *command++ = 0;
                            // 0x205 + 3N [23:16] ロードアレイの 1頂点のバイト数を無効化するコマンドの生成
                            //            [31:28] ロードアレイの 要素数を無効化するコマンドの生成
                            *command++ =
                                PICA_CMD_HEADER_SINGLE( PICA_REG_LOAD_ARRAY0_ELEMENT1 + 3 * index );
                        }

                        // 有効な固定頂点属性を無効化するコマンドの生成
                        if ( m_AttrConst[ index ].IsEnable() )
                        {
                            *command++ = index;
                            *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FIXED_ATTR, 4 );
                            *command++ = 0;
                            *command++ = 0;
                            *command++ = 0;
                            // padding
                            *command++ = 0;
                        }
                    }

                    return command;
                }

                /*!
                    @brief 頂点属性の無効化するコマンドを全て生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command )
                {
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( 0x201 );

                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( 0x202 );

                    // 0x203 + 3N, 0x204 + 3N, 0x205 + 3N (N = 0-11) を全て無効化するコマンドの生成
                    const int size = ( 2 + 3 * VERTEX_ATTRIBUTE_MAX ) * sizeof( bit32 );
                    std::memset( command, 0, size );
                    command[ 1 ] = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_LOAD_ARRAY0_ATTR_OFFSET, VERTEX_ATTRIBUTE_MAX * 3 );
                    command += size / sizeof( bit32 );

                    for ( int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i )
                    {
                        // 0x232
                        *command++ = i;
                        *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FIXED_ATTR, 4 );

                        // 0x233
                        *command++ = 0;
                        // 0x234
                        *command++ = 0;

                        // 0x235
                        *command++ = 0;
                        // padding
                        *command++ = 0;
                    }

                    return command;
                }

            public : // 無効化

                /*!
                    @brief 頂点属性の無効化をおこないます。DMPGLにおけるglDisableVertexAttrib()に対応します。

                    @param[in] symbol      無効化する入力シンボルを指定します。
                 */
                void DisableAttr( const nn::gr::CTR::BindSymbolVSInput& symbol )
                {
                    DisableAttr_( symbol.start );
                }

                /*!
                    @brief すべての頂点属性の無効化をおこないます。
                 */
                void DisableAll()
                {
                    m_CmdCacheVertexNum = 0;

                    for ( u32 index = 0; index < VERTEX_ENABLE_COMMAND_MAX; index++ )
                    {
                        m_CmdCacheVertex[ index ] = 0;
                    }

                    for ( u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++ )
                    {
                        m_IsEnableReg[ index ] = false;
                        m_LoadArray[ index ].DisableAll();
                        m_AttrConst[ index ].DisableAll();
                    }
                }

            public :
                /*!
                    コンストラクタでは、すべての頂点属性が無効になった状態に初期化されます。
                 */
                explicit Vertex()
                {
                    DisableAll();
                }

            protected :
                /*!
                    @brief ロードアレイ設定のためのクラスです。
                 */
                class LoadArray
                {
                public:
                    LoadArray() :
                    physicalAddr( 0 )
                    {
                    }

                    /*!
                        @brief ロードアレイの物理アドレスの設定です。型は uptr です。
                    */
                    uptr physicalAddr;

                    /*!
                        @brief ロードアレイの型の設定する配列です。
                               型は @ref PicaDataVertexAttrType[] です。
                    */
                    util::SizedEnum1<PicaDataVertexAttrType> type[ VERTEX_ATTRIBUTE_MAX ];

                    /*!
                        @brief 属性のサイズを指定する配列です。
                               型は u32[] です。
                               配列のサイズは nn::gr::CTR::Vertex::VERTEX_ATTRIBUTE_MAX です。
                    */
                    u32 byte[ VERTEX_ATTRIBUTE_MAX ];

                    /*!
                        @brief 関連付けられているレジスタを指定します。
                               型は s32[] です。
                               配列のサイズは nn::gr::CTR::Vertex::VERTEX_ATTRIBUTE_MAX です。
                    */
                    s32 bind[ VERTEX_ATTRIBUTE_MAX ];

                    /*!
                        @brief 有効化されているかどうかを返します。

                        @return 有効なときに true、そうでないときに false を返します。
                    */
                    bool IsEnable() const { return physicalAddr != 0; }

                    /*!
                        @brief 有効性をチェックして、無効化します。
                    */
                    void CheckDisable();

                    /*!
                        @brief 全て無効化して、値を初期化します。
                    */
                    void DisableAll();
                };

                /*!
                    @brief 固定頂点属性の設定のためのクラスです。
                 */
                class AttrConst
                {
                public :
                    /*!
                        @brief コンストラクタです。
                    */
                    AttrConst() :
                    dimension( 0 )
                    {
                    }

                    /*!
                        @brief 固定頂点属性の要素数の設定です（1から4）。
                               型は u8 です。
                    */
                    u8 dimension;
                    NN_PADDING3;

                    /*!
                        @brief 固定頂点属性の固定値の設定です。
                               配列のサイズは nn::gr::CTR::Vertex::VERTEX_ATTRIBUTE_DIMENSION_MAX です。
                    */
                    f32 param[ VERTEX_ATTRIBUTE_DIMENSION_MAX ];

                    /*!
                        @brief 有効化されているかどうかを返します。
                    */
                    bool IsEnable() const { return dimension != 0; }

                    /*!
                        @brief 無効化します。
                    */
                    void Disable()
                    {
                        dimension = 0;
                    }

                    /*!
                        @brief 全て無効化して、値を初期化します。
                    */
                    void DisableAll()
                    {
                        dimension = 0;

                        for ( u32 index = 0; index < VERTEX_ATTRIBUTE_DIMENSION_MAX; index++ )
                        {
                            param[ index ] = 0;
                        }
                    }
                };

            protected :
                /*!
                    @brief 属性を無効化します。

                    @param[in] bind_reg      バインドするレジスタを指定します。
                */
                void DisableAttr_( const bit32 bind_reg );

                /*!
                    @brief 属性を有効化するコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return  書き込まれた描画コマンドの終端の次のアドレスを返します。
                */
                bit32* MakeEnableAttrCommand_( bit32* command ) const;

            protected :
                mutable u32 m_CmdCacheVertexNum;
                mutable bit32 m_CmdCacheVertex[ VERTEX_ENABLE_COMMAND_MAX ];

                bool        m_IsEnableReg[ VERTEX_ATTRIBUTE_MAX ];

                LoadArray   m_LoadArray[ VERTEX_ATTRIBUTE_MAX ];
                AttrConst   m_AttrConst[ VERTEX_ATTRIBUTE_MAX ];
            };

        }  // namespace CTR
    }  // namespace gr
} // namespace nn


#endif // NN_GR_CTR_GR_VERTEX_H_
