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
    @brief Signature using OPSSR signing scheme with AES-CMC based keyed permutation and SHA1.

        This signature scheme is a *unique* signature scheme, i.e. for each (key, message) pair,
        there is only one possible signature. So do not use these functions for key-sharing
        protocols without randomizing properly the messages.

        This scheme provides signature only and, even if it seems encrypted, the signature can
        be decrypted using only public information. If you also want confidentiality, you should
        apply encryption on top.

        The security of the signature depends on the length of the RSA key used and the amount of
        padding size (overhead) used.
        The size of the signature must be at least as long as the size of the RSA modulus (n) used.

        The following parameters are recommended:
        | RSA key size | overhead amount | security | minimum signature size |
        +--------------+-----------------+----------+------------------------+
        | 768 bits     | 8 bytes         | 64 bits  | 96 bytes               |
        | 1024 bits    | 10 bytes        | 80 bits  | 128 bytes              |
        | 2048 bits    | 16 bytes        | 128 bits | 256 bytes              |

        A factorization of the RSA key would enable signing any payload.
        64 bits security is hard to break but is feasible today with enough power. This should only
        be used for data that is not important to the integrity of the system.
        80 bits security is a reasonable tradeoff between minimal overhead and strongness.
        Do not use 80 bits or less security for system-wide data that needs to be protected for the
        lifetime of the product.
        128 bits security should hold for the product lifetime except for a major breakthrough in
        cryptographic attacks.

        The public data parameter must be the same for signature and verification. It must be a
        value depending on the public part of the key. We recommend to use the contents of the
        RSA public key file, which is the convention used by the sign and verify scripts.
*/
namespace zorro {

/*!
    @param[out] signature      Pointer to the signature.
    @param[inout] signatureLen The size of the signature buffer. The function will fail if this is
                               not greater than the size of the message plus the overhead.
                               Otherwise, it is set to the length of the signature.
    @param[in] message         Pointer to the message to sign.
    @param[in] messageLen      Length of the message, in bytes. Must be at least modulus size minus
                               overhead.
    @param[in] overhead        Overhead amount, in bytes.
    @param[in] publicData      Public key data pointer.
    @param[in] publicDataLen   Public key data length.
    @param[in] derKey          Pointer to the RSA private key containing the key with which signing
                               will be done, in DER format.
    @param[in] derKeyLen       RSA private key data length.
    @param[in] modulusBitLen   Length of RSA modulus in bits.
    @param[in] tempBuffer      Temporary buffer address.
    @param[in] tempBufferLen   Temporary buffer length. Must be at least 2 times the length of the
                               RSA modulus.
    @return                    true on success, false on failure.
 */
bool OpssrCmcSha1_Sign(u8* signature, u32& signatureLen, const u8* message, u32 messageLen,
                       u32 overhead, const u8* publicData, u32 publicDataLen, const u8* derKey,
                       u32 derKeyLen, u32 modulusBitLen, u8* tempBuffer, u32 tempBufferLen);

/*!
    @brief OPSSR Signature verification.

    @param[out] recoveredMessage Pointer to the recovered message.
    @param[inout] recoveredMessageLen The size of the recovered message buffer. The function will
                               fail if this is not greater than the size of the message minus the
                               overhead. Otherwise, it is set to the length of the message.
    @param[in] signature       Pointer to the signature.
    @param[in] signatureLen    Length of the signature, in bytes.
    @param[in] overhead        Overhead amount.
    @param[in] publicData      Public key data pointer.
    @param[in] publicDataLen   Public key data length.
    @param[in] derKey          Pointer to the RSA public key containing the key with which
                               verifying will be done, in DER format.
    @param[in] derKeyLen       RSA public key data length.
    @param[in] modulusBitLen   Length of RSA modulus in bits.
    @param[in] tempBuffer      Temporary buffer address.
    @param[in] tempBufferLen   Temporary buffer length. Must be at least 3 times the length of the
                               RSA modulus.
    @return                    true on success, false on failure.
 */
bool OpssrCmcSha1_Verify(u8* recoveredMessage, u32& recoveredMessageLen,
                         const u8* signature, u32 signatureLen, u32 overhead,
                         const u8* publicData, u32 publicDataLen, const u8* derKey,
                         u32 derKeyLen, u32 modulusBitLen, u8* tempBuffer, u32 tempBufferLen);
}
}

#endif // NN_MW_ZORRO_ZORRO_H_
