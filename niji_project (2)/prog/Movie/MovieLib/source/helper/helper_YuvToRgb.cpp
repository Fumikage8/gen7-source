#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_YuvToRgb.cpp
 Description: Helper file that manages YUV to RGB conversion using 
              CTR nn::y2r API.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include "helper_YuvToRgb.h"

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {
namespace yuvtorgb {

/*---------------------------------------------------------------------------*/

static nn::os::Event s_TransfertEndEvent;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void Initialize(void)
{
    nn::y2r::CTR::Initialize();
    nn::y2r::CTR::SetTransferEndInterrupt(true);
    nn::y2r::CTR::GetTransferEndEvent(&s_TransfertEndEvent);
    s_TransfertEndEvent.ClearSignal();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void Finalize(void)
{
  s_TransfertEndEvent.Finalize();
  nn::y2r::CTR::Finalize();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static void StopConversion (void)
{
    nn::y2r::CTR::StopConversion();
    do
    {}
    while (nn::y2r::CTR::IsBusyConversion());
    s_TransfertEndEvent.ClearSignal();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void LaunchConversion (const int width, const int height, 
                       const nn::y2r::CTR::OutputFormat outputFormat,
                       const nn::y2r::CTR::StandardCoefficient standardCoefficient,    
                       const nn::y2r::CTR::BlockAlignment blockAlignent,
                       const u8* inYBuffer, const u8* inUBuffer, const u8* inVBuffer, 
                       const int inYStride, const int inUStride, const int inVStride,
                       u8* outRgbBuffer, const int outStride)
{
    StopConversion();
    
    nn::y2r::CTR::SetInputFormat(nn::y2r::INPUT_YUV420_INDIV_8);
    nn::y2r::CTR::SetOutputFormat(outputFormat);
    nn::y2r::CTR::SetRotation(nn::y2r::CTR::ROTATION_NONE);
    nn::y2r::CTR::SetBlockAlignment(blockAlignent);
    nn::y2r::CTR::SetTransferEndInterrupt(true);
    nn::y2r::CTR::SetInputLineWidth(width);
    nn::y2r::CTR::SetInputLines(height);
    nn::y2r::CTR::SetStandardCoefficient(standardCoefficient);
    nn::y2r::CTR::SetAlpha(0xFF);
    
    nn::y2r::CTR::SetSendingY(reinterpret_cast<const void*>(inYBuffer), width * height, width, inYStride);
    nn::y2r::CTR::SetSendingU(reinterpret_cast<const void*>(inUBuffer), (width/2)*(height/2), (width/2), inUStride);
    nn::y2r::CTR::SetSendingV(reinterpret_cast<const void*>(inVBuffer), (width/2)*(height/2), (width/2), inVStride);

    int pixelSize;
    switch(outputFormat)
    {
        case nn::y2r::CTR::OUTPUT_RGB_32:
            pixelSize = 4;
            break;
        case nn::y2r::CTR::OUTPUT_RGB_24:
            pixelSize = 3;
            break;    
        case nn::y2r::CTR::OUTPUT_RGB_16_555:
        case nn::y2r::CTR::OUTPUT_RGB_16_565:
            pixelSize = 2;
            break;
        default:
            pixelSize = 0;
            break;
    }

    nn::y2r::CTR::SetReceiving(reinterpret_cast<void*>(outRgbBuffer), width * height * pixelSize, width * pixelSize * 8, outStride * pixelSize * 8);

    nn::y2r::CTR::StartConversion();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void LaunchConversion (const int width, const int height,
                       const nn::y2r::CTR::OutputFormat outputFormat,
                       const nn::y2r::CTR::StandardCoefficient standardCoefficient,
                       const u8* inYuvBuffer, u8* outRgbBuffer, const int outStride)
{
    StopConversion();

    nn::y2r::CTR::SetInputFormat(nn::y2r::CTR::INPUT_YUV422_BATCH);
    nn::y2r::CTR::SetOutputFormat(outputFormat);
    nn::y2r::CTR::SetRotation(nn::y2r::CTR::ROTATION_NONE);
    nn::y2r::CTR::SetBlockAlignment(nn::y2r::CTR::BLOCK_8_BY_8);
    nn::y2r::CTR::SetTransferEndInterrupt(true);
    nn::y2r::CTR::SetInputLineWidth(width);
    nn::y2r::CTR::SetInputLines(height);
    nn::y2r::CTR::SetStandardCoefficient(standardCoefficient);
    nn::y2r::CTR::SetAlpha(0xFF);
    
    nn::y2r::CTR::SetSendingYuv(reinterpret_cast<const void*>(inYuvBuffer), (width * height * 2), (width * 2));

    int pixelSize;
    switch(outputFormat)
    {
        case nn::y2r::CTR::OUTPUT_RGB_32:
            pixelSize = 4;
            break;
        case nn::y2r::CTR::OUTPUT_RGB_24:
            pixelSize = 3;
            break;    
        case nn::y2r::CTR::OUTPUT_RGB_16_555:
        case nn::y2r::CTR::OUTPUT_RGB_16_565:
            pixelSize = 2;
            break;
        default:
            pixelSize = 0;
            break;
    }

    nn::y2r::CTR::SetReceiving(reinterpret_cast<void*>(outRgbBuffer), width * height * pixelSize, width * pixelSize * 8, outStride * pixelSize * 8);

    nn::y2r::CTR::StartConversion();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

bool ConversionFinished (void)
{
    return !nn::y2r::CTR::IsBusyConversion();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

nn::os::Event* GetEndConversionEvent (void)
{
    return &s_TransfertEndEvent;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

} // end of namespace yuvtorgb
} // end of namespace helper
} // end of namespace mo
} // end of namespace mw


/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
