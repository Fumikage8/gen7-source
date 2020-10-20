/*-----------------------------------------------------------------------*
Project: zorro
File: zorro.h
Copyright (C) 2015 Nintendo, All rights reserved.
These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.
The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#ifndef NN_MW_ZORRO_ZORRO_H_
#define NN_MW_ZORRO_ZORRO_H_

#include <nn/types.h>

namespace mw {
	/*!
    @brief 本署名スキームはAES-CMCベースの鍵付き置換とSHA-1を用いたOPSSR署名方式に基づいています。

        注意：本署名スキームは「一意署名スキーム」、すなわちメッセージと鍵の各組み合わせに対して、可能な署名は一つしか存在しません。
        従って、以下の関数を鍵共有プロトコル等でご利用する場合、メッセージを十分にランダム化する必要があります。
        また、本スキームは「署名」という機能しか提供しません。暗号化の要素が含まれていても、公開情報のみで復号されることが可能なので、
        データの機密性を実現するには本スキームの上に別途、暗号化を適用する必要があります。

        本署名スキームの安全性はRSA鍵長とパディング長（オーバーヘッド）に依存します。
        署名長をRSAの法パラメータ（n）の長さ以上である必要があります。

        本署名スキームのご利用の際、次のようなパラメータをおすすめします。
        | RSA鍵長       | オーバーヘッド     | 安全性　　 | 最小署名長　　　　　　　　 |
        +---------------+-----------------+----------+------------------------+
        | 768 ビット     | 8 バイト         | 64 ビット | 96 バイト               |
        | 1024 ビット    | 10 バイト        | 80 ビット | 128 バイト              |
        | 2048 ビット    | 16 バイト        | 128 ビット| 256 バイト              |

        RSA鍵が素因数分解されてしまえば、攻撃者は任意のメッセージを署名できるようになるのでご注意ください。
        64ビット安全性を破ることは困難ですが、現在の計算能力で実現可能です。従って、システムの安全性から見て重要でないデータのみに
        対してご利用ください。
        80ビット安全性は低オーバーヘッドと十分な安全性の妥当なトレードオフですが、
        製品寿命にわたってシステム全体が利用するような秘密データを保護するのに不十分と思われます。
        攻撃法に大きな進展がない限り、128ビット安全性は製品寿命にわたって破られないと思われます。

        署名時と検証時、同じpublicDataパラメータを利用する必要があります。また、このパラメータは鍵の公開部分に依存する必要があるので、
        RSA公開鍵ファイルの内容を利用することが一般とされています。
	*/

namespace zorro {
	
/*!
    @param[out] signature      署名へのポインタ
    @param[inout] signatureLen 署名バッファサイズ。(メッセージサイズ)＋(オーバーヘッドサイズ)以下のサイズの場合、関数は失敗します。
                               そうでなければ署名長に設定されます。
    @param[in] message         署名対象のメッセージへのポインタ
    @param[in] messageLen      メッセージサイズ。(法パラメータの長さ)-(オーバーヘッドサイズ)以上である必要があります。
    @param[in] overhead        オーバーヘッドサイズ（バイト単位）
    @param[in] publicData      公開鍵データへのポインタ
    @param[in] publicDataLen   公開鍵データ長
    @param[in] derKey          署名するために用いられる鍵を含むRSA秘密鍵（DER形式）へのポインタ
    @param[in] derKeyLen       RSA秘密鍵データ長
    @param[in] modulusBitLen   RSAの法パラメータの長さ（ビット単位）
    @param[in] tempBuffer      Temporary buffer address.
    @param[in] tempBufferLen   Temporary buffer length. Must be at least 2 times the length of the
                               RSA modulus.
    @return                    成功時はtrue、失敗時はfalse
 */
bool OpssrCmcSha1_Sign(u8* signature, u32& signatureLen, const u8* message, u32 messageLen,
                       u32 overhead, const u8* publicData, u32 publicDataLen, const u8* derKey,
                       u32 derKeyLen, u32 modulusBitLen, u8* tempBuffer, u32 tempBufferLen);

/*!
    @brief OPSSR署名の検証

    @param[out] recoveredMessage 復元されたメッセージへのポインタ
    @param[inout] recoveredMessageLen 復元されたメッセージのバッファサイズ。(メッセージサイズ)-(オーバーヘッドサイズ)以下のサイズの場合、
                               関数は失敗します。そうでなければメッセージサイズに設定されます。
    @param[in] signature       署名へのポインタ
    @param[in] signatureLen    署名長（バイト単位)
    @param[in] overhead        オーバーヘッドサイズ
    @param[in] publicData      公開鍵データへのポインタ
    @param[in] publicDataLen   公開鍵データ長
    @param[in] derKey          署名検証するために用いられる鍵を含むRSA公開鍵（DER形式）へのポインタ
    @param[in] derKeyLen       RSA公開鍵データ長
    @param[in] modulusBitLen   RSAの法パラメータの長さ（ビット単位）
    @param[in] tempBuffer      Temporary buffer address.
    @param[in] tempBufferLen   Temporary buffer length. Must be at least 3 times the length of the
                               RSA modulus.
    @return                    成功時はtrue、失敗時はfalse
 */
bool OpssrCmcSha1_Verify(u8* recoveredMessage, u32& recoveredMessageLen,
                         const u8* signature, u32 signatureLen, u32 overhead,
                         const u8* publicData, u32 publicDataLen, const u8* derKey,
                         u32 derKeyLen, u32 modulusBitLen, u8* tempBuffer, u32 tempBufferLen);
}
}

#endif // NN_MW_ZORRO_ZORRO_H_
