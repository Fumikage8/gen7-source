/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Texture.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_TEXTURE_H_
#define NN_GR_CTR_GR_TEXTURE_H_

#include "gr_gl.h"
#include "util_SizedEnum.h"
#include "gr_Utility.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief テクスチャ設定のためのクラスです。
             */
            class Texture
            {
            public :

                /*!
                    @brief 各テクスチャユニットの共通設定のためのクラスです。
                 */
                class UnitBase
                {
                public :
                    /*!
                        @brief テクスチャの物理アドレスの設定です。型は uptr です。
                     */
                    uptr physicalAddr;


                    /*!
                        @brief テクスチャの幅の設定です。型は u16 です。
                     */
                    u16 width;


                    /*!
                        @brief テクスチャの高さの設定です。型は u16 です。
                     */
                    u16 height;

                    /*!
                        @brief テクスチャのフォーマットの設定です。
                               型は @ref PicaDataTextureFormat です。
                    */
                    util::SizedEnum1<PicaDataTextureFormat> format;

                    /*!
                        @brief テクスチャ座標 s の設定です。
                               型は @ref PicaDataTextureWrap です。
                               初期値はPICA_DATA_TEXTURE_WRAP_REPEATです。
                     */
                    util::SizedEnum1<PicaDataTextureWrap> wrapT;

                    /*!
                        @brief テクスチャ座標 s の設定です。
                               型は @ref PicaDataTextureWrap です。
                               初期値はPICA_DATA_TEXTURE_WRAP_REPEATです。
                    */
                    util::SizedEnum1<PicaDataTextureWrap> wrapS;

                    /*!
                        @brief テクスチャ拡大フィルタの設定です。
                               型は @ref PicaDataTextureMagFilter です。
                               初期値はPICA_DATA_TEXTURE_MAG_FILTER_NEARESTです。
                    */
                    util::SizedEnum1<PicaDataTextureMagFilter> magFilter;


                    /*!
                        @brief テクスチャ縮小フィルタの設定です。
                               型は @ref PicaDataTextureMinFilter です。
                               初期値は PICA_DATA_TEXTURE_MIN_FILTER_NEAREST です。
                    */
                    util::SizedEnum1<PicaDataTextureMinFilter> minFilter;
                    NN_PADDING3;

                    /*!
                        @brief LODバイアスの設定です。
                               型は f32 です。
                               初期値は0.fです。
                               [-16.f : 16.f]の範囲で設定してください。
                               コマンド生成時には13bit固定小数（fraction8）に変換されます。
                               MipMapが有効でないときは、無視されます。
                               @see IsEnableMipMap()
                    */
                    f32 lodBias;

                    /*!
                        @brief 最小LODレベルを設定します。値は u8 です。
                               初期値は0です。
                               0以上の値で設定します。
                               MipMapが有効でないときは、無視されます。
                        @see IsEnableMipMap()
                     */
                    u8 minLodLevel;


                    /*!
                        @brief 最大LODレベルを設定します。値は u8 です。
                               初期値は0です。
                               ロードしたMipMapの段数-1を設定してください。
                               MipMapが有効でないときは、無視されます。
                        @see IsEnableMipMap()
                    */
                    u8 maxLodLevel;


                    /*!
                        @brief ボーダーカラーの赤成分です。
                               値は u8 です。
                               初期値は0です。
                     */
                    u8 borderColorR;

                    /*!
                        @brief ボーダーカラーの緑成分です。
                               値は u8 です。
                               初期値は0です。
                     */
                    u8 borderColorG;


                    /*!
                        @brief ボーダーカラーの青成分です。
                               値は u8 です。
                               初期値は0です。
                     */
                    u8 borderColorB;


                    /*!
                        @brief ボーダーカラーのアルファ成分です。
                               値は u8 です。
                               初期値は0です。
                     */
                    u8 borderColorA;

                    NN_PADDING2;

                protected :
                    /*!
                        @brief MipMapが有効かどうかを判定します。

                        @return 有効なときに true を返します。
                    */
                    bool IsEnableMipMap() const
                    {
                        if ( minFilter == PICA_DATA_TEXTURE_MIN_FILTER_LINEAR  )
                        {
                            return false;
                        }

                        if ( minFilter == PICA_DATA_TEXTURE_MIN_FILTER_NEAREST )
                        {
                            return false;
                        }

                        return true;
                    }

                    /*!
                        @brief 初期値を設定します。
                     */
                    UnitBase();
                };

            public :

                /*!
                    @brief テクスチャユニット0の設定のためのクラスです。
                 */
                class Unit0 : public UnitBase
                {
                public :
                     /*!
                        @brief テクスチャユニット0のサンプラータイプの設定です。
                               型は @ref PicaDataTexture0SamplerType です。
                               初期値はPICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_FALSEです。
                     */
                    util::SizedEnum1<PicaDataTexture0SamplerType> texType;
                    NN_PADDING3;

                    /*!
                        @brief キューブマップ[ +x ]テクスチャの物理アドレスの設定です。
                               型は uptr です。
                    */
                    uptr cubeMapAddrPosX;

                    /*!
                        @brief キューブマップ[ -x ]テクスチャの物理アドレスの設定です。
                               型は uptr です。
                    */
                    uptr cubeMapAddrNegX;

                    /*!
                        @brief キューブマップ[ +y ]テクスチャの物理アドレスの設定です。
                               型は uptr です。
                    */
                    uptr cubeMapAddrPosY;

                    /*!
                        @brief キューブマップ[ -Y ]テクスチャの物理アドレスの設定です。
                               型は uptr です。
                    */
                    uptr cubeMapAddrNegY;

                    /*!
                        @brief キューブマップ[ +Z ]テクスチャの物理アドレスの設定です。
                               型は uptr です。
                    */
                    uptr cubeMapAddrPosZ;

                    /*!
                        @brief キューブマップ[ -Z ]テクスチャの物理アドレスの設定です。
                               型は uptr です。
                    */
                    uptr cubeMapAddrNegZ;

                public :
                    /*!
                        @brief 設定された情報をもとに、テクスチャユニット0の設定コマンドを生成します。

                        @param[in] command                描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] is_update_texture_func サンプラータイプなどの設定を変えた場合はtrueを設定します。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool is_update_texture_func = true ) const;

                    /*!
                        @brief 初期値を設定します。

                        @param[in] texture_ テクスチャです。
                     */
                    explicit Unit0( const Texture& texture_ );

                protected :
                    const Texture& m_Texture;
                };

                /*!
                    @brief テクスチャユニット1の設定のためのクラスです。
                 */
                class Unit1 : public UnitBase
                {
                public :
                    /*!
                        @brief テクスチャユニット1のサンプラータイプの設定です。
                               型は @ref PicaDataTexture1SamplerType です。
                               初期値はPICA_DATA_TEXTURE1_SAMPLER_TYPE_FALSEです。
                     */
                    util::SizedEnum1<PicaDataTexture1SamplerType> texType;
                    NN_PADDING3;

                public :
                    /*!
                        @brief 設定された情報をもとに、テクスチャユニット1の設定コマンドを生成します。

                        @param[in] command                描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] is_update_texture_func サンプラータイプなどの設定を変えた場合はtrueを設定します。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool is_update_texture_func = true ) const;

                    /*!
                        @brief 初期値を設定します。

                        @param[in] texture_ テクスチャです。
                     */
                    explicit Unit1( const Texture& texture_ );

                protected :
                    const Texture& m_Texture;
                };

                /*!
                    @brief テクスチャユニット2の設定のためのクラスです。
                 */
                class Unit2 : public UnitBase
                {
                public :
                    /*!
                        @brief テクスチャユニット2のサンプラータイプの設定です。
                               型は @ref PicaDataTexture2SamplerType です。
                     */
                    util::SizedEnum1<PicaDataTexture2SamplerType> texType;

                    /*!
                        @brief テクスチャユニット2のテクスチャ座標選択の設定です。
                               型は @ref PicaDataTexture2TexCoord です。
                               初期値は PICA_DATA_TEXTURE2_TEXCOORD_TEXTURE2 です。
                     */
                    util::SizedEnum1<PicaDataTexture2TexCoord>    texCoord;

                    NN_PADDING2;

                public :
                    /*!
                        @brief 設定された情報をもとに、テクスチャユニット2の設定コマンドを生成します。

                        @param[in] command                描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] is_update_texture_func サンプラータイプなどの設定を変えた場合はtrueを設定します。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                     bit32* MakeCommand( bit32* command, bool is_update_texture_func = true ) const;

                    /*!
                        @brief 初期値を設定します。

                        @param[in] texture_ テクスチャです。
                     */
                    explicit Unit2( const Texture& texture_ );

                protected :
                    const Texture& m_Texture;
                };

                /*!
                    @brief テクスチャユニット3の設定のためのクラスです。
                 */
                class Unit3 : protected UnitBase
                {
                public :
                    /*!
                        @brief テクスチャユニット3のサンプラータイプの設定です。
                               型は @ref PicaDataTexture3SamplerType です。
                               初期値は PICA_DATA_TEXTURE3_SAMPLER_TYPE_FALSE です。
                     */
                    util::SizedEnum1<PicaDataTexture3SamplerType> texType;

                     /*!
                        @brief テクスチャユニット3のテクスチャ座標選択の設定です。
                               型は @ref PicaDataTexture3TexCoord です。
                               初期値はPICA_DATA_TEXTURE3_TEXCOORD_TEXTURE0です。
                     */
                    util::SizedEnum1<PicaDataTexture3TexCoord>    texCoord;

                    NN_PADDING2;

                public :
                    /*!
                        @brief 設定された情報をもとに、テクスチャユニット3の設定コマンドを生成します。

                        @param[in] command                描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] is_update_texture_func サンプラータイプなどの設定を変えた場合はtrueを設定します。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool is_update_texture_func = true ) const;

                    /*!
                        @brief 初期値を設定します。

                        @param[in] texture_ テクスチャです。
                     */
                    explicit Unit3( const Texture& texture_ );

                protected :
                    const Texture& m_Texture;
                };

            public :
                /*!
                    @brief テクスチャユニット0についての設定です。
                           型は nn::gr::CTR::Texture::Unit0 です。
                */
                Unit0 unit0;

                /*!
                    @brief テクスチャユニット1についての設定です。
                           型は nn::gr::CTR::Texture::Unit1 です。
                */
                Unit1 unit1;

                /*!
                    @brief テクスチャユニット2についての設定です。
                           型は nn::gr::CTR::Texture::Unit2 です。
                */
                Unit2 unit2;

                /*!
                    @brief テクスチャユニット3についての設定です。
                           型は nn::gr::CTR::Texture::Unit3 です。
                */
                Unit3 unit3;

            public :
                /*!
                    @brief 各設定の初期値を設定します。
                */
                Texture();

                /*!
                    @brief 設定された情報をもとに、すべてのテクスチャの設定コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeCommand( bit32* command, bool isAddDummyCommand = true ) const;

                /*!
                    @brief 設定された情報をもとに、テクスチャユニットの有効/無効コマンドなどを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeFuncCommand( bit32* command, bool isAddDummyCommand = true ) const;

                /*!
                    @brief テクスチャを無効化する最小限のコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command, bool isAddDummyCommand = true );

            protected:
                /*!
                    @brief 0x80 へのダミーコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                 static bit32* MakeDummyCommand_( bit32* command );
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_TEXTURE_H_
