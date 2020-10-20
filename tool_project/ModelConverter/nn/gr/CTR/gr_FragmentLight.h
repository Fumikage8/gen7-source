/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_FragmentLight.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50933 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_FRAGMENT_LIGHT_H_
#define NN_GR_CTR_GR_FRAGMENT_LIGHT_H_

#include <nn/gr/CTR/gr_Utility.h>
#include <nn/gr/CTR/gr_LookUpTable.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief フラグメントライティング設定のためのクラスです。
             */
            class FragmentLight
            {
            public :
                /*!
                    @brief 光源ごとの設定のためのクラスです。
                 */
                class Source
                {
                public :

                    /*!
                        @brief 光源番号です。
                               型は u8 です。
                               0-7までの値を設定します。
                    */
                    u8 id;

                    /*!
                        @brief trueにすることで、両面にライトを当てることが出来ます。
                               型は bool です。
                    */
                    bool isEnableTwoSideDiffuse;

                    /*!
                        @brief ジオメトリック･ファクタ0を使用するかどうかの設定です。
                               型は bool です。
                    */
                    bool isEnableGeomFactor0;

                    /*! 
                        @brief ジオメトリック･ファクタ1を使用するかどうかの設定です。
                               型は bool です。
                    */
                    bool isEnableGeomFactor1;

                    /*!
                        @brief 拡散光の赤成分の設定です。
                        　　　 型は u8 です。
                               dmp_FragmentMaterial.diffuse×dmp_FragmentLightSource[i].diffuseの値[0.f, 1.f]を[0, 255]にマップして設定します。
                    */
                    u8 diffuseR;

                    /*!
                        @brief 拡散光の緑成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.diffuse×dmp_FragmentLightSource[i].diffuseの値[0.f, 1.f]を
                               [0, 255]にマップして設定します。
                    */
                    u8 diffuseG;

                    /*!
                        @brief 拡散光の青成分の設定です。
                        　　　 型は u8 です。
                               dmp_FragmentMaterial.diffuse×dmp_FragmentLightSource[i].diffuseの値[0.f, 1.f]を
                               [0, 255]にマップして設定します。
                    */
                    u8 diffuseB;

                    /*!
                        @brief 環境光の赤成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.ambient×dmp_FragmentLightSource[i].ambientの値[0.f, 1.f]を
                               [0, 255]にマップして設定します。
                    */
                    u8 ambientR;

                    /*!
                        @brief 環境光の緑成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.ambient×dmp_FragmentLightSource[i].ambientの値[0.f, 1.f]を[0, 255]にマップして設定します。
                    */
                    u8 ambientG;

                    /*!
                        @brief 環境光の青成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.ambient×dmp_FragmentLightSource[i].ambientの値[0.f, 1.f]を[0, 255]にマップして設定します。                               
                    */
                    u8 ambientB;

                    /*!
                        @brief 鏡面光0の赤成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.specular0×dmp_FragmentLightSource[i].specular0の値[0.f, 1.f]を[0, 255]にマップして設定します。                               
                    */
                    u8 specular0R;

                    /*!
                        @brief 鏡面光0の緑成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.specular0×dmp_FragmentLightSource[i].specular0の値[0.f, 1.f]を[0, 255]にマップして設定します。                               
                    */
                    u8 specular0G;

                    /*!
                        @brief 鏡面光0の青成分の設定です。
                               型は u8 です。
                               dmp_FragmentMaterial.specular0×dmp_FragmentLightSource[i].specular0の値[0.f, 1.f]を[0, 255]にマップして設定します。                               
                    */
                    u8 specular0B;

                    /*!
                        @brief 鏡面光1の赤成分の設定です。
                               型は u8 です。
                               Light::isEnableLutRefl に true を設定する場合はdmp_FragmentLightSource[i].specular1の値[0.f, 1.f]を、                               
                               false の場合は dmp_FragmentMaterial.specular1×dmp_FragmentLightSource[i].specular1の値[0.f, 1.f]を[0, 255]にマップして設定します。
                    */
                    u8 specular1R;

                    /*!
                        @brief 鏡面光1の緑成分の設定です。
                               型は u8 です。
                               Light::isEnableLutRefl に true を設定する場合はdmp_FragmentLightSource[i].specular1の値[0.f, 1.f]を、                              
                               false の場合はdmp_FragmentMaterial.specular1×dmp_FragmentLightSource[i].specular1の値[0.f, 1.f]を[0, 255]にマップして設定します。                               
                    */
                    u8 specular1G;

                    /*!
                        @brief 鏡面光1の青成分の設定です。
                               型は u8 です。
                               Light::isEnableLutRefl に true を設定する場合はdmp_FragmentLightSource[i].specular1の値[0.f, 1.f]を、
                               false の場合はdmp_FragmentMaterial.specular1×dmp_FragmentLightSource[i].specular1の値[0.f, 1.f]を[0, 255]にマップして設定します。                               
                    */
                    u8 specular1B;

                protected :
                    /*!
                        @brief ライト位置のX,Yを描画コマンド形式に変換して保持します。
                               型は bit32 です。
                    */
                    bit32  posXY;

                    /*!
                         @brief ライト位置のZを描画コマンド形式に変換して保持します。
                                型は bit32 です。
                     */
                    bit32  posZ;

                    /*!
                         @brief ライト位置を無限遠に飛ばすかどうかのフラグです。
                                型は u8 です。
                     */
                    u8   isInfinity;
                    NN_PADDING3;

                    /*!
                         @brief 距離減衰のバイアスを描画コマンド形式に変換して保持します。
                                型は bit32 です。
                     */
                    bit32 distAttnBias;

                    /*!
                         @brief 距離減衰のスケールを描画コマンド形式に変換して保持します。
                                型は bit32 です。
                     */
                    bit32 distAttnScale;

                    /*!
                         @brief スポットライト向きのX,Yを描画コマンド形式に変換して保持します。
                                型は bit32 です。
                     */
                    bit32 spotDirectionXY;

                    /*!
                         @brief スポットライト向きのZを描画コマンド形式に変換して保持します。
                                型は bit32 です。
                     */
                    bit32 spotDirectionZ;


                public :

                    /*!
                        @brief ライトの位置を設定します。設定された値は、描画コマンドに設定するために16ビット浮動小数に変換して保持します。

                        @param[in] position    設定するライトの位置です。
                        @param[in] is_infinity 位置を無限遠に飛ばす場合はtrueを設定します。
                     */
                    void SetPosition( const nn::math::VEC3& position, const bool is_infinity )
                    {
                        posXY = PICA_CMD_DATA_FRAG_LIGHT_POSITION_XY( Float32ToFloat16( position.x ),
                                                                      Float32ToFloat16( position.y ) );
                        posZ  = PICA_CMD_DATA_FRAG_LIGHT_POSITION_Z(  Float32ToFloat16( position.z ) );
                        isInfinity = is_infinity ? 1 : 0;
                    }

                    /*!
                        @brief ライトの位置を設定します。設定された値は、描画コマンドに設定するために16ビット浮動小数に変換して保持します。

                        @param[in] position_x  設定するライトの x 座標です。
                        @param[in] position_y  設定するライトの y 座標です。
                        @param[in] position_z  設定するライトの z 座標です。
                        @param[in] is_infinity 位置を無限遠に飛ばす場合はtrueを設定します。
                     */
                    void SetPosition( const f32 position_x, const f32 position_y, const f32 position_z, const bool is_infinity )
                    {
                        posXY = PICA_CMD_DATA_FRAG_LIGHT_POSITION_XY( Float32ToFloat16( position_x ),
                                                                      Float32ToFloat16( position_y ) );
                        posZ  = PICA_CMD_DATA_FRAG_LIGHT_POSITION_Z(  Float32ToFloat16( position_z ) );
                        isInfinity = is_infinity ? 1 : 0;
                    }


                    /*!
                        @brief スポットライトの向きを設定します。設定された値は、描画コマンドに設定するために13ビット固定小数に変換して保持します。

                        @param[in] spot_direction  設定するスポットライトの向きです。
                     */
                    void SetSpotDirection( const nn::math::VEC3& spot_direction )
                    {
                        spotDirectionXY = PICA_CMD_DATA_FRAG_LIGHT_SPOT_XY( Float32ToFix13Fraction11( -spot_direction.x ),
                                                                            Float32ToFix13Fraction11( -spot_direction.y ) );
                        spotDirectionZ  = PICA_CMD_DATA_FRAG_LIGHT_SPOT_Z(  Float32ToFix13Fraction11( -spot_direction.z ) );
                    }


                    /*!
                        @brief スポットライトの向きを設定します。設定された値は、描画コマンドに設定するために13ビット固定小数に変換して保持します。

                        @param[in] spot_direction_x  設定するスポットライト向きの x です。
                        @param[in] spot_direction_y  設定するスポットライト向きの y です。
                        @param[in] spot_direction_z  設定するスポットライト向きの z です。
                     */
                    void SetSpotDirection( const f32& spot_direction_x, const f32& spot_direction_y, const f32& spot_direction_z )
                    {
                        spotDirectionXY = PICA_CMD_DATA_FRAG_LIGHT_SPOT_XY( Float32ToFix13Fraction11( - spot_direction_x ),
                                                                            Float32ToFix13Fraction11( - spot_direction_y ) );
                        spotDirectionZ  = PICA_CMD_DATA_FRAG_LIGHT_SPOT_Z(  Float32ToFix13Fraction11( - spot_direction_z ) );
                    }


                    /*!
                        @brief 距離減衰のスケール、バイアスを設定します。 設定された値は、描画コマンドに設定するために20ビット浮動小数に変換して保持します。

                        @param[in] scale 距離減衰のスケールです。
                        @param[in] bias 距離減衰のバイアスです。
                     */
                    void SetDistAttnScaleBias( const f32 scale, const f32 bias )
                    {
                        distAttnScale = Float32ToFloat20( scale );
                        distAttnBias  = Float32ToFloat20( bias );
                    };

                    /*!
                        @brief 距離減衰のスケール、バイアスを設定します。
                               引数には、あらかじめ20ビット浮動小数に変換した値を指定する必要があります。

                        @param[in] scale_float20 距離減衰のスケールです（20ビット浮動小数）。
                        @param[in] bias_float20 距離減衰のバイアスです（20ビット浮動小数）。
                     */
                    void SetDistAttnScaleBiasDirectly( const f32 scale_float20, const f32 bias_float20 )
                    {
                        distAttnScale = static_cast<bit32>(scale_float20);
                        distAttnBias  = static_cast<bit32>(bias_float20);
                    };

                    /*!
                        @brief 距離減衰のスケール、バイアスを、減衰開始距離、減衰終了距離を指定して設定します。設定された値は、スケール・バイアスに変換後、描画コマンドに設定するために20ビット浮動小数に変換して保持します。

                        @param[in] start 距離減衰の開始距離です。
                        @param[in] end   距離減衰の終了距離です。
                     */
                    void SetDistAttnStartEnd( const f32 start, const f32 end )
                    {
                        f32 r = 1.f / ( end - start );
                        SetDistAttnScaleBias( r, -start * r );
                    };

                public :
                    /*!
                        @brief 設定された情報をもとに、光源についてのコマンドを全て生成します。
                               つまり、@ref MakeColorCommand 関数と @ref MakeGeometryCommand 関数が生成するコマンドを含みます。

                        @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeAllCommand( bit32* command ) const;

                    /*!
                        @brief 設定された情報をもとに、光源についてのコマンドを全て生成します。
                               内部では MakeAllCommand() を呼んでいます。互換性を保つために残してあります。

                        @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command ) const;

                    /*!
                        @brief 設定された情報をもとに、光源のカラー情報のコマンドを生成します。

                        @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeColorCommand( bit32* command ) const;

                    /*!
                        @brief 設定された情報をもとに、光源の幾何情報のコマンドを生成します。

                        @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeGeometryCommand( bit32* command ) const;

                    /*!
                        @brief 設定された情報をもとに、光源の幾何情報のコマンドを生成します。
                               ただし @ref MakeGeometryCommand 関数とは異なり、
                               光源の位置およびスポットライトの向きのコマンドのみを生成します。

                        @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeAttitudeCommand( bit32* command ) const;
                public :
                    /*!
                        @brief コンストラクタでメンバ変数を初期化します。
                    */
                    explicit Source();
                };

            public :
                //! 光源の最大数は8です。
                static const u32 LIGHT_SOURCE_MAX = 8;

                /*!
                    @brief グローバルアンビエントの赤成分の設定です。
                           型は u8 です。
                           [0.f, 1.f]を[0, 255]にマップして設定します。
                */
                // dmp_FragmentMaterial.emission＋dmp_FragmentMaterial.ambient×dmp_FragmentLighting.ambientの値[0.f, 1.f]を
                // [0, 255]にマップして設定します。
                u8 globalAmbientR;


                /*!
                    @brief グローバルアンビエントの緑成分の設定です。
                           型は u8 です。
                           [0.f, 1.f]を[0, 255]にマップして設定します。
                 */
                 // dmp_FragmentMaterial.emission＋dmp_FragmentMaterial.ambient×dmp_FragmentLighting.ambientの値[0.f, 1.f]を
                 // [0, 255]にマップして設定します。
                u8 globalAmbientG;


                /*!
                    @brief グローバルアンビエントの青成分の設定です。
                           型は u8 です。
                           [0.f, 1.f]を[0, 255]にマップして設定します。
                 */
                 // dmp_FragmentMaterial.emission＋dmp_FragmentMaterial.ambient×dmp_FragmentLighting.ambientの値[0.f, 1.f]を
                 // [0, 255]にマップして設定します。
                u8 globalAmbientB;
                NN_PADDING1;

                /*!
                    @brief 光源の設定を保持します。
                           型は nn::gr::CTR::FragmentLight::Source[] です。
                */
                Source source[ LIGHT_SOURCE_MAX ];

                /*!
                    @brief 光源の有効無効を設定します。
                           型は, bool[] です。
                           配列の上限値は, LIGHT_SOURCE_MAX です。
                */
                bool isEnable[ LIGHT_SOURCE_MAX ];

                /*!
                    @brief スポットライトが有効かどうかの設定です。
                           型は, bool[] です。
                           配列の上限値は, LIGHT_SOURCE_MAX です。
                */
                bool isEnableSpot[ LIGHT_SOURCE_MAX ];

                /*!
                    @brief 距離減衰の有効無効の設定です。
                           型は, bool[] です。
                           配列の上限値は, LIGHT_SOURCE_MAX です。
                */
                bool isEnableDistAttn[ LIGHT_SOURCE_MAX ];

                /*!
                    @brief シャドウ調整要素の設定です。
                           型は, bool[] です。
                           配列の上限値は, LIGHT_SOURCE_MAX です。
                */
                bool isShadowed[ LIGHT_SOURCE_MAX ];

                /*!
                    @brief レイヤーコンフィグの設定です。
                           型は @ref PicaDataFragLightEnvLayerConfig です。
                */
                /*
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG0
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG1
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG2
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG3
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG4
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG5
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG6
                 PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG7
                 のいずれかを設定します。
                 */
                PicaDataFragLightEnvLayerConfig layerConfig;

                /*!
                    @brief フレネルの適用先の設定です。
                           型は @ref PicaDataFragLightEnvFresnel です。
                */
                /*
                 PICA_DATA_FRAG_LIGHT_ENV_NO_FRESNEL
                 PICA_DATA_FRAG_LIGHT_ENV_PRI_ALPHA_FRESNEL
                 PICA_DATA_FRAG_LIGHT_ENV_SEC_ALPHA_FRESNEL
                 PICA_DATA_FRAG_LIGHT_ENV_PRI_SEC_ALPHA_FRESNEL
                 のいずれかを指定します。
                 */
                PicaDataFragLightEnvFresnel  fresnelSelector;

                /*!
                    @brief シャドウ調整要素をサンプルするテクスチャの設定です。
                           型は @ref PicaDataFragLightTexture です。
                */
                /*
                 PICA_DATA_FRAG_LIGHT_TEXTURE0
                 PICA_DATA_FRAG_LIGHT_TEXTURE1
                 PICA_DATA_FRAG_LIGHT_TEXTURE2
                 PICA_DATA_FRAG_LIGHT_TEXTURE3
                 のいずれかを設定します。
                 */
                PicaDataFragLightEnvTexture     shadowSelector;

                /*!
                    @brief バンプマッピングの設定です。
                           型は @ref PicaDataFragLightEnvBump です。
                */
                /*
                 PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP
                 PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_BUMP_DMP
                 PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_TANG_DMP
                 のいずれかを設定します。
                 */
                PicaDataFragLightEnvBump        bumpMode;

                /*!
                    @brief 法線マップ、タンジェントマップとして使用されるテクスチャの設定です。
                           型は @ref PicaDataFragLightTexture です。
                */
                /*
                 PICA_DATA_FRAG_LIGHT_TEXTURE0
                 PICA_DATA_FRAG_LIGHT_TEXTURE1
                 PICA_DATA_FRAG_LIGHT_TEXTURE2
                 PICA_DATA_FRAG_LIGHT_TEXTURE3
                 のいずれかを設定します。
                 */
                PicaDataFragLightEnvTexture     bumpSelector;

                /*!
                    @brief プライマリカラーにシャドウの減衰を適用するかどうかの設定です。
                           型は bool です。
                */
                bool                         isEnableShadowPrimary;

                /*!
                    @brief セカンダリカラーにシャドウの減衰を適用するかどうかの設定です。
                           型は bool です。
                */
                bool                         isEnableShadowSecondary;

                /*!
                    @brief アルファ成分にシャドウの減衰を適用するかどうかの設定です。
                           型は bool です。
                */
                bool                         isEnableShadowAlpha;

                /*!
                    @brief シャドウ減衰を反転するかどうかの設定です。
                           型は bool です。
                */
                bool                         isInvertShadow;

                /*!
                    @brief 法線マップのz成分を再生成するかどうかの設定です。
                           型は bool です。
                */
                bool                         isEnableBumpRenorm;

                /*!
                    @brief L.n < 0時に鏡面光カラーを0にクランプするかの設定です。
                           型は bool です。
                */
                bool                         isEnableClampHighLights;
    
                /*!
                    @brief true の場合、「分布 0」項に、参照テーブルの値、もしくは、1 が設定されます。
                    型は bool です。
                    初期値は false です。
                */
                /*
                 * このフラグにtrueを設定すると、
                 * レイヤーコンフィグが D0 を使用する設定になっている場合、「分布 0」項に参照テーブルの値が適用されます。
                 * レイヤーコンフィグが D0 を使用する設定になっていない場合、1 になります。
                 * このフラグにfalseを設定すると、レイヤーコンフィグの設定にかかわらず 0 になります。
                 * 初期値は false です。
                 */
                bool                         isEnableLutD0;

                /*!
                    @brief true の場合、「分布 1」項に、参照テーブルの値、もしくは、1 が設定されます。
                           型は bool です。
                           初期値は false です。
                */
                /*
                 * このフラグにtrueを設定すると、
                 * レイヤーコンフィグが D1 を使用する設定になっている場合、「分布 1」項に参照テーブルの値が適用されます。
                 * レイヤーコンフィグが D1 を使用する設定になっていない場合、1 になります。
                 * このフラグにfalseを設定すると、レイヤーコンフィグの設定にかかわらず 0 になります。
                 * 初期値は false です。
                 */
                bool                         isEnableLutD1;

                /*!
                    @brief true の場合、「反射」項に、参照テーブルの値、もしくは、1 が設定されます。
                           型は bool です。
                           初期値は true です。
                */
                /*
                 * このフラグにtrueを設定すると、
                 * レイヤーコンフィグが RR、RG、RB を使用する設定になっている場合、それぞれの「反射」項に参照テーブルの値が適用されます。
                 * レイヤーコンフィグが RR、RG、RB を使用する設定になっていない場合、1 になります。
                 * このフラグにfalseを設定すると、レイヤーコンフィグの設定にかかわらず 0 になります。
                 * 初期値は true です。
                 */

                bool                         isEnableLutRefl;
                NN_PADDING2;



                /*!
                    @brief 参照テーブルの値の調整のためのクラスです。
                */
                class LutConfig
                {
                public :
                    /*!
                        @brief 参照テーブル引数の設定です。
                               型は @ref PicaDataFragLightEnvLutInput です。
                    */
                    /*
                     設定可能な値は、
                     PICA_DATA_FRAG_LIGHT_ENV_NH_DMP
                     PICA_DATA_FRAG_LIGHT_ENV_VH_DMP
                     PICA_DATA_FRAG_LIGHT_ENV_NV_DMP
                     PICA_DATA_FRAG_LIGHT_ENV_LN_DMP
                     PICA_DATA_FRAG_LIGHT_ENV_SP_DMP
                     PICA_DATA_FRAG_LIGHT_ENV_CP_DMP
                     のいずれかです。
                     */
                    PicaDataFragLightEnvLutInput input;

                    /*!
                        @brief 参照テーブルの引数の範囲の設定です。
                               型は bool です。
                    */
                    /*
                     * 参照テーブルの引数の範囲を[0, 1] にする場合は true、 [-1, 1]にする場合は false を設定します。
                     */
                    bool isAbs;

                    /*!
                        @brief 参照テーブルの出力にかけるスケールの設定です。
                        　　　 型は @ref PicaDataFragLightEnvLutScale です。
                     */
                    PicaDataFragLightEnvLutScale scale;

                    NN_PADDING1;

                    /*!
                        @brief 初期値を設定します。
                    */
                    explicit LutConfig();
                };

                /*!
                    @brief 参照テーブルD0についての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigD0;

                /*!
                    @brief 参照テーブルD1についての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigD1;

                /*!
                    @brief 参照テーブルSPについての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigSP;

                /*!
                    @brief 参照テーブルFRについての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigFR;

                /*!
                    @brief 参照テーブルRBについての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigRB;

                /*!
                    @brief 参照テーブルRGについての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigRG;

                /*!
                    @brief 参照テーブルRRについての設定です。
                           型は nn::gr:CTR::FragmentLight::LutConfig です。
                */
                LutConfig lutConfigRR;

            public :
                /*!
                    @brief 各設定の初期値を設定します。
                */
                FragmentLight();

                /*!
                    @brief 設定された情報をもとに、フラグメントライティングの光源ごとの設定コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeLightSourceCommand( bit32* command ) const;

                /*!
                    @brief 設定された情報をもとに、フラグメントライティングの参照テーブル設定のコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeLutConfigCommand( bit32* command ) const;

                /*!
                    @brief 設定された情報をもとに、フラグメントライティングのライト環境の設定コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x8f レジスタに書き込む前に 0x80 へのダミーコマンドを生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeLightEnvCommand( bit32* command, bool isAddDummyCommand = true ) const;

                /*!
                    @brief 設定された情報をもとに、フラグメントライティングの設定コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x8f レジスタに書き込む前に 0x80 へのダミーコマンドを生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeAllCommand( bit32* command, bool isAddDummyCommand = true ) const;

                /*!
                    @brief フラグメントライティングを無効化する最低限のコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x8f レジスタに書き込む前に 0x80 へのダミーコマンドを生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command, bool isAddDummyCommand = true );
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_FRAGMENT_LIGHT_H_
