/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     main.cpp

  Copyright (C)2009 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 7298 $
 *---------------------------------------------------------------------------*/

/*

  To generate the public key/private key pairs, one can follow the example given in tools/sign.pl
 */

#include <stdio.h>

#include <nn/os.h>
#include <nn/dbg.h>
#include <nn/math/math_Rand.h>

#include <mw/zorro/zorro.h>

#include <string.h> // for memcmp

static u8 publicKeyData[] =
{
    0x30, 0x7c, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6b, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xdb, 0x53, 0x6e, 0x5e, 0xb7, 0xef, 0xfe, 0xa4, 0xc8, 0x2a, 0x73, 0xf4, 0xee, 0xca, 0x08, 0x11, 0x2e, 0xfc, 0x32, 0xcb, 0x93, 0xdf, 0xd2, 0x9b, 0xea, 0xf2, 0x54, 0x64, 0xec, 0x36, 0x0e, 0x69, 0x37, 0xbe, 0x10, 0x6e, 0x46, 0xd9, 0x54, 0x21, 0x7d, 0xf7, 0x33, 0x35, 0xfc, 0x15, 0x98, 0xb7, 0x80, 0x11, 0xde, 0x82, 0xdf, 0x1e, 0xa4, 0x45, 0x4a, 0xdd, 0x3c, 0x57, 0x5d, 0x2f, 0xf9, 0x96, 0xdb, 0x56, 0x1f, 0x60, 0xeb, 0x5f, 0x60, 0xc0, 0xbe, 0x6c, 0xf1, 0xbf, 0x26, 0x72, 0x13, 0x70, 0x83, 0x9d, 0x08, 0xc6, 0x66, 0x16, 0xb1, 0xce, 0x2a, 0x44, 0x40, 0x87, 0xa2, 0xfd, 0x1c, 0xcf, 0x02, 0x03, 0x01, 0x00, 0x01,
};

static u8 privateKeyData[] =
{
    0x30, 0x82, 0x01, 0xcb, 0x02, 0x01, 0x00, 0x02, 0x61, 0x00, 0xdb, 0x53, 0x6e, 0x5e, 0xb7, 0xef, 0xfe, 0xa4, 0xc8, 0x2a, 0x73, 0xf4, 0xee, 0xca, 0x08, 0x11, 0x2e, 0xfc, 0x32, 0xcb, 0x93, 0xdf, 0xd2, 0x9b, 0xea, 0xf2, 0x54, 0x64, 0xec, 0x36, 0x0e, 0x69, 0x37, 0xbe, 0x10, 0x6e, 0x46, 0xd9, 0x54, 0x21, 0x7d, 0xf7, 0x33, 0x35, 0xfc, 0x15, 0x98, 0xb7, 0x80, 0x11, 0xde, 0x82, 0xdf, 0x1e, 0xa4, 0x45, 0x4a, 0xdd, 0x3c, 0x57, 0x5d, 0x2f, 0xf9, 0x96, 0xdb, 0x56, 0x1f, 0x60, 0xeb, 0x5f, 0x60, 0xc0, 0xbe, 0x6c, 0xf1, 0xbf, 0x26, 0x72, 0x13, 0x70, 0x83, 0x9d, 0x08, 0xc6, 0x66, 0x16, 0xb1, 0xce, 0x2a, 0x44, 0x40, 0x87, 0xa2, 0xfd, 0x1c, 0xcf, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x61, 0x00, 0xcf, 0xf6, 0xd6, 0x4a, 0x9d, 0xe2, 0xc4, 0xf3, 0xfe, 0x90, 0xb2, 0x8e, 0xe6, 0xf5, 0x4e, 0x9c, 0xec, 0x8e, 0x5a, 0xa3, 0xca, 0x3d, 0x8a, 0xf8, 0x28, 0xd9, 0x4a, 0xc8, 0xbf, 0x69, 0xaa, 0x54, 0x7e, 0x74, 0x8c, 0x23, 0xe4, 0xae, 0x22, 0x25, 0xf7, 0xc9, 0x34, 0x17, 0xd6, 0x45, 0xac, 0x1d, 0x61, 0xad, 0xa5, 0x98, 0x8b, 0xc0, 0x00, 0xac, 0x18, 0x57, 0x63, 0xcc, 0x4d, 0x3e, 0xe6, 0x40, 0x3e, 0x64, 0x95, 0x6b, 0xcd, 0xff, 0xe9, 0x19, 0x87, 0xdd, 0xd6, 0x6d, 0xeb, 0x61, 0xe7, 0x7c, 0x8f, 0xdd, 0xdc, 0x5b, 0x36, 0x88, 0xe7, 0x7c, 0xaf, 0x22, 0x3a, 0x9d, 0x7f, 0xc0, 0xf5, 0x51, 0x02, 0x31, 0x00, 0xf1, 0xf7, 0xb6, 0xea, 0xf3, 0x03, 0x93, 0xd0, 0xdb, 0xac, 0x54, 0xd2, 0xd7, 0x54, 0x11, 0xb7, 0xe1, 0x7e, 0x05, 0x16, 0x68, 0x78, 0x60, 0x34, 0x17, 0xdd, 0xec, 0x40, 0x9e, 0xbf, 0x74, 0xbb, 0x0d, 0xb4, 0x51, 0xbb, 0x7b, 0xe0, 0xb6, 0x76, 0xef, 0xb2, 0xdc, 0xce, 0xd9, 0xe8, 0x84, 0x9b, 0x02, 0x31, 0x00, 0xe8, 0x0b, 0x93, 0x03, 0xe8, 0x69, 0x8c, 0x91, 0xd3, 0xde, 0x2c, 0x8e, 0x1e, 0x03, 0xf7, 0x7e, 0xfc, 0x97, 0xb6, 0xd3, 0x6e, 0xc2, 0x39, 0xe1, 0x49, 0xb2, 0x83, 0xc2, 0xca, 0xe5, 0xb7, 0x42, 0x7c, 0x3c, 0x29, 0x20, 0x57, 0x63, 0x50, 0x52, 0x4c, 0x97, 0x6e, 0x0e, 0x0d, 0x26, 0x99, 0xdd, 0x02, 0x30, 0x37, 0x3f, 0x8c, 0x70, 0x91, 0x14, 0x5c, 0x07, 0xf0, 0x60, 0xa4, 0xed, 0x8f, 0x65, 0x0e, 0xb6, 0xa1, 0xb2, 0x18, 0x8f, 0x49, 0x43, 0x53, 0x14, 0x11, 0xef, 0x06, 0x26, 0xab, 0x18, 0xa4, 0x1a, 0xa1, 0xc2, 0x6e, 0x77, 0x14, 0x8d, 0xf6, 0x9b, 0xa9, 0x97, 0x79, 0x7b, 0x71, 0x9d, 0xf9, 0xc9, 0x02, 0x30, 0x65, 0x7e, 0x52, 0x82, 0x5e, 0x10, 0x74, 0x8c, 0x80, 0x27, 0xe8, 0x43, 0x0c, 0xfb, 0xdd, 0x2f, 0xec, 0xd9, 0x8a, 0xfc, 0xf5, 0xaf, 0xfc, 0x52, 0xfe, 0xc2, 0x17, 0x01, 0x87, 0x6f, 0x6c, 0x75, 0xa7, 0x13, 0x45, 0x45, 0xc1, 0x13, 0x4e, 0x59, 0x80, 0xe8, 0xa7, 0x19, 0x7c, 0x45, 0x0a, 0xfd, 0x02, 0x31, 0x00, 0xb0, 0x4a, 0x9c, 0xc0, 0x43, 0x4c, 0xdc, 0x1a, 0x46, 0xca, 0xf8, 0x84, 0x2e, 0xe6, 0x22, 0x11, 0xf0, 0x1e, 0x59, 0xd3, 0x5a, 0x76, 0x01, 0xec, 0xaa, 0x35, 0x75, 0x55, 0xa4, 0x99, 0xc0, 0xc7, 0xed, 0xe5, 0xa5, 0xef, 0x53, 0x6e, 0x1f, 0xb9, 0x4b, 0x5b, 0x85, 0xa6, 0xe5, 0x93, 0x15, 0x75,
};

static const u32 modulusBitLen = 768;

static const u32 overhead = 8;

void printHex(u8 const* message, u32 messageLen)
{
    char* hexMsg = new char[2 * messageLen + 1];
    memset(hexMsg, 0, 2 * messageLen + 1);
    char tbl[17] = "0123456789abcdef";
    for(int ch = 0; ch < messageLen; ++ch)
    {
        hexMsg[ch * 2 + 0] = tbl[message[ch] >> 4];
        hexMsg[ch * 2 + 1] = tbl[message[ch] & 0x0F];
    }
    NN_LOG(hexMsg);
    delete[] hexMsg;
}

void SignVerify(u8 const* message, u32 messageLen)
{
    NN_LOG("Test for message: ");
    printHex(message, messageLen);
    NN_LOG("\n");

    u32 const signatureLen = overhead + messageLen;
    u8* signature = new u8[signatureLen];
    u32 curSignatureLen = signatureLen;
    u32 const tempBufferLen = (modulusBitLen / 8) * 3;
    u8* tempBuffer = new u8[tempBufferLen];
    bool result;

    result = mw::zorro::OpssrCmcSha1_Sign(signature, curSignatureLen, message, messageLen,
                                          overhead, publicKeyData, sizeof(publicKeyData),
                                          privateKeyData, sizeof(privateKeyData), modulusBitLen,
                                          tempBuffer, tempBufferLen);

    if (result)
        NN_LOG("Signature succeeded.\n");
    else
        NN_LOG("Signature failed...\n");

    NN_ASSERT(signatureLen == curSignatureLen);

    NN_LOG("Signature: message ");
    printHex(signature, signatureLen);
    NN_LOG("\n");

    u8* recoveredMessage = new u8[messageLen];
    u32 recoveredMessageLen = messageLen;
    result = mw::zorro::OpssrCmcSha1_Verify(recoveredMessage, recoveredMessageLen,
                                            signature, signatureLen, overhead,
                                            publicKeyData, sizeof(publicKeyData), publicKeyData,
                                            sizeof(publicKeyData), modulusBitLen, tempBuffer,
                                            tempBufferLen);
    if (result)
        NN_LOG("Verification succeeded.\n");
    else
        NN_LOG("Verification failed...\n");

    int cmp = memcmp(message, recoveredMessage, messageLen);
    if (cmp == 0)
        NN_LOG("Recovered message is the same as original.\n");
    else
        NN_LOG("Recovered message is not the same as original.\n");

    delete[] tempBuffer;
    delete[] recoveredMessage;
    delete[] signature;
}

// This signature has been generated using tools/sign.pl.
static u8 messageSignaturePerl[] =
{
    0xd9, 0x8a, 0x77, 0x64, 0x4d, 0xd4, 0xc6, 0x12, 0x69, 0x74, 0x76, 0x92, 0xb6, 0x3c, 0x70, 0xd0, 0xe3, 0x1c, 0x9e, 0xa2, 0x58, 0x09, 0x1b, 0x6d, 0x0f, 0x3d, 0xb3, 0x3d, 0x37, 0x7c, 0x6c, 0xee, 0x21, 0x6c, 0x72, 0xa2, 0x89, 0x96, 0x5f, 0xa6, 0x60, 0xdf, 0xf2, 0xbb, 0xe5, 0x9b, 0x72, 0xbb, 0x22, 0x9c, 0x26, 0xfd, 0x45, 0x70, 0x67, 0xcd, 0xbe, 0x1b, 0x2f, 0x6d, 0x9c, 0xbd, 0xd1, 0x44, 0xc3, 0xb9, 0x63, 0x5e, 0xda, 0x85, 0x0a, 0x58, 0x05, 0xd8, 0xa2, 0x3f, 0x25, 0x63, 0x81, 0x55, 0xbd, 0x91, 0x75, 0xe2, 0xd0, 0x05, 0xf0, 0x79, 0xf5, 0xd5, 0x8d, 0x4e, 0x9d, 0x8e, 0x34, 0xbc,
};

void VerifyPerlMessage()
{
    u32 const recoveredMessagePerlLen = sizeof(messageSignaturePerl) - overhead;
    u8 recoveredMessagePerl[recoveredMessagePerlLen + 1] = {}; // to put a final 0
    u32 curRecoveredLen = recoveredMessagePerlLen;
    u32 const tempBufferLen = (modulusBitLen / 8) * 3;
    u8* tempBuffer = new u8[tempBufferLen];
    bool result;
    result = mw::zorro::OpssrCmcSha1_Verify(recoveredMessagePerl, curRecoveredLen,
                                            messageSignaturePerl, sizeof(messageSignaturePerl),
                                            overhead, publicKeyData, sizeof(publicKeyData),
                                            publicKeyData, sizeof(publicKeyData), modulusBitLen,
                                            tempBuffer, tempBufferLen);
    delete[] tempBuffer;
    if (result)
        NN_LOG("Perl message verification succeeded.\n");
    else
        NN_LOG("Perl message verification failed...\n");

    NN_LOG("Message is: %s\n", recoveredMessagePerl);

}


extern "C" void nnMain()
{
    u8 const message[] = "Hello world! This message should be like, a hundred or some bytes long. "
        "Now let us try to sign it.";
    u32 const messageLen = sizeof(message);
    SignVerify(message, messageLen);

    // sign/verify a message with only 88 characters which is the minimum for our settings here
    SignVerify(message, 88);

    VerifyPerlMessage();

    // test random messages
    u8 msg[256];
    nn::math::RandomGenerator rg(0);
    while(1)
    {
        u32 size = (rg.Generate() % (255 - 88)) + 88;
        for (int i = 0; i < size; ++i)
            msg[i] = u8(rg.Generate());
        SignVerify(msg, size);
    }
}
