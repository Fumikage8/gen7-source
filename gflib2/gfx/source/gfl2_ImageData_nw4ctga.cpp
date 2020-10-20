#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_GFGL.h>

#if defined(GF_PLATFORM_WIN32)
#include "gfl2_DDSInformation.h"
#include <algorithm>
#include <vector>


namespace gfl2 { namespace gfx {

// @attention もう少しましな実装にする

enum TexFormat
{
  TEX_FORMAT_RGBA8,     // 32bit ○
  TEX_FORMAT_RGB5_A1,   // 16bit ○
  TEX_FORMAT_RGBA4,     // 16bit ○
  TEX_FORMAT_RGB565,    // 16bit ○
  TEX_FORMAT_RGB8,      // 24bit ○
  TEX_FORMAT_ETC1,      // 4bit ○
  TEX_FORMAT_ETC1_A4,   // 4bit ○
  TEX_FORMAT_A8,        // 8bit ○
  TEX_FORMAT_A4,        // 4bit ○
  TEX_FORMAT_L8,        // 8bit ○
  TEX_FORMAT_L4,        // 4bit ○
  TEX_FORMAT_LA8,       // 16bit ○
  TEX_FORMAT_LA4,       // 8bit ○
  TEX_FORMAT_HILO8,     // 16bit ○
};

struct texel8888
{
  u8 r;
  u8 g;
  u8 b;
  u8 a;
};

template <typename T>
static T ChangeAddressingCore(T* pBlock)
{
  static u32 ofs1_list[] =
  {
    0, 2, 8, 10, 32, 34, 40, 42,
  };

  T newData;

  for (u32 i = 0; i < 8; ++i)
  {
    u32 ofs0 = i * 8;
    u32 ofs1 = ofs1_list[i];
    newData.data[0 + ofs0] = pBlock->data[0 + ofs1];
    newData.data[1 + ofs0] = pBlock->data[1 + ofs1];
    newData.data[2 + ofs0] = pBlock->data[4 + ofs1];
    newData.data[3 + ofs0] = pBlock->data[5 + ofs1];
    newData.data[4 + ofs0] = pBlock->data[16 + ofs1];
    newData.data[5 + ofs0] = pBlock->data[17 + ofs1];
    newData.data[6 + ofs0] = pBlock->data[20 + ofs1];
    newData.data[7 + ofs0] = pBlock->data[21 + ofs1];
  }

  return newData;
}

static void ChangeAddressing(u8* pData, u32 width, u32 height, TexFormat format)
{
	// 8x8テクセル単位でジグザグに格納されている
	struct texel32
	{
		u8 t[4];
	};
	struct block32
	{
		texel32 data[64];
	};
	block32* pBlock = (block32*)pData;

	// ブロックをリニアに変換
	std::vector<block32> newDataList;
	u32 blockWidth = width / 8;
	u32 blockHeight = height / 8;
	for (u32 h = 0; h < blockHeight; ++h)
	{
		for (u32 w = 0; w < blockWidth; ++w)
		{
			newDataList.push_back(ChangeAddressingCore(pBlock));
			++pBlock;
		}
	}

	// 結果を書き戻す
	texel32* pResult = (texel32*)pData;
	for (u32 h = 0; h < height; ++h)
	{
		u32 blockIndexH = h / 8;
		u32 dataIndexH = (h * 8) % 64;
		for (u32 w = 0; w < width; ++w)
		{
			u32 blockIndexW = w / 8;
			u32 blockIndex = blockIndexH * blockWidth + blockIndexW;
			*pResult = newDataList[blockIndex].data[dataIndexH + w % 8];
			++pResult;
		}
	}
}

static void DecodeETC1(texel8888* out_pResult, u8* pData, u8*& out_pNextData, u32 width, u32 height, TexFormat format)
{
  u32 loopCount = width * height;

  struct texel44
  {
    u8 a0 : 4;
    u8 a1 : 4;
  };
  struct etc1_individual // diffBitが0のとき
  {
    u8 baseColor2B : 4;
    u8 baseColor1B : 4;

    u8 baseColor2G : 4;
    u8 baseColor1G : 4;

    u8 baseColor2R : 4;
    u8 baseColor1R : 4;
  };
  struct etc1_differential // diffBitが1のとき
  {
    s8 diffColor2B : 3;
    u8 baseColor1B : 5;

    s8 diffColor2G : 3;
    u8 baseColor1G : 5;

    s8 diffColor2R : 3;
    u8 baseColor1R : 5;
  };

  struct texel_etc1
  {
    u8 lsb0;
    u8 lsb1;
    u8 msb0;
    u8 msb1;

    u8 flipBit : 1;
    u8 diffBit : 1;
    u8 tableCw2 : 3;
    u8 tableCw1 : 3;

    u8 colorData[3];
  };

  struct texel_etc1_a4
  {
    texel44 alpha[8];
    texel_etc1 etc1;
  };

  struct texel888
  {
    u8 r;
    u8 g;
    u8 b;
  };
  struct block8888
  {
    texel8888 t[4][4];
  };

  static const s16 tableCodeword[2][8] =
  {
    {2, 5, 9, 13, 18, 24, 33, 47},
    {8, 17, 29, 42, 60, 80, 106, 183},
  };

  std::vector<block8888> blockList;
  texel_etc1_a4* data_a4 = (texel_etc1_a4*)pData;
  texel_etc1* data = (texel_etc1*)pData;

  if (format == TEX_FORMAT_ETC1_A4)
  {
    data = &data_a4->etc1;
  }

  loopCount /= 16;
  for (u32 i = 0; i < loopCount; ++i)
  {
    block8888 resultColor;
    u8 baseColorBlock[4][4];
    texel888 subBlock[2];

    if (data->diffBit == 0)
    {
      // 個別モード
      etc1_individual* color = (etc1_individual*)data->colorData;

      subBlock[0].r = color->baseColor1R * 17;
      subBlock[0].g = color->baseColor1G * 17;
      subBlock[0].b = color->baseColor1B * 17;

      subBlock[1].r = color->baseColor2R * 17;
      subBlock[1].g = color->baseColor2G * 17;
      subBlock[1].b = color->baseColor2B * 17;
    }
    else
    {
      // 差分モード
      etc1_differential* color = (etc1_differential*)data->colorData;

      subBlock[0].r = (u8)(color->baseColor1R * 8.25);
      subBlock[0].g = (u8)(color->baseColor1G * 8.25);
      subBlock[0].b = (u8)(color->baseColor1B * 8.25);

      subBlock[1].r = (u8)((color->baseColor1R + color->diffColor2R) * 8.25);
      subBlock[1].g = (u8)((color->baseColor1G + color->diffColor2G) * 8.25);
      subBlock[1].b = (u8)((color->baseColor1B + color->diffColor2B) * 8.25);
    }

    if (data->flipBit == 0)
    {
      // 縦分割
      for (u32 h = 0; h < 4; ++h)
      {
        for (u32 w = 0; w < 2; ++w)
        {
          baseColorBlock[h][w] = 0;
        }
        for (u32 w = 2; w < 4; ++w)
        {
          baseColorBlock[h][w] = 1;
        }
      }
    }
    else
    {
      // 横分割
      for (u32 w = 0; w < 4; ++w)
      {
        for (u32 h = 0; h < 2; ++h)
        {
          baseColorBlock[h][w] = 1;
        }
        for (u32 h = 2; h < 4; ++h)
        {
          baseColorBlock[h][w] = 0;
        }
      }
    }

    // 値復元
    u16 lsb01 = (data->lsb1 << 8) | data->lsb0;
    u16 msb01 = (data->msb1 << 8) | data->msb0;

    u32 index = 0;
    for (u32 w = 0; w < 4; ++w)
    {
      for (u32 h = 0; h < 4; ++h)
      {
        u16 lsb = (lsb01 & (1 << index)) != 0;
        u16 msb = (msb01 & (1 << index)) != 0;

        u8 baseColorIndex = baseColorBlock[3 - h][w];
        texel888 baseColor = subBlock[baseColorIndex];
        u8 tableIndex = (baseColorIndex == 0)
          ? data->tableCw1
          : data->tableCw2;
        s16 subValue = tableCodeword[lsb][tableIndex];
        if (msb)
        {
          subValue = -subValue;
        }

        texel8888 result;
        result.r = (u8)std::max(std::min(baseColor.r + subValue, 255), 0);
        result.g = (u8)std::max(std::min(baseColor.g + subValue, 255), 0);
        result.b = (u8)std::max(std::min(baseColor.b + subValue, 255), 0);
        result.a = 255;
        resultColor.t[3 - h][w] = result;

        ++index;
      }
    }

    // αを展開
    index = 0;
    if (format == TEX_FORMAT_ETC1_A4)
    {
      for (u32 w = 0; w < 4; ++w)
      {
        for (u32 h = 0; h < 2; ++h)
        {
          resultColor.t[3 - (h * 2)][w].a = 255 * data_a4->alpha[index].a0 / 15;
          resultColor.t[3 - (h * 2 + 1)][w].a = 255 * data_a4->alpha[index].a1 / 15;
          ++index;
        }
      }
    }

    blockList.push_back(resultColor);

    if (format == TEX_FORMAT_ETC1)
    {
      ++data;
    }
    else
    {
      ++data_a4;
      data = &data_a4->etc1;
    }
  }

  if (format == TEX_FORMAT_ETC1)
  {
    out_pNextData = (u8*)data;
  }
  else
  {
    out_pNextData = (u8*)data_a4;
  }

  // 並べなおす
  u32 blockWidth = width / 4;
  for (u32 h = 0; h < height; ++h)
  {
    u32 bh = h / 4;
    u32 metaBh = bh / 2;

    bh = metaBh * (blockWidth * 2) + (bh % 2) * 2;
    for (u32 w = 0; w < width; ++w)
    {
      u32 bw = w / 4;
      if (bw % 2 == 0)
      {
        bw *= 2;
      }
      else
      {
        bw *= 2;
        bw -= 1;
      }

      *out_pResult = blockList[bh + bw].t[3 - (h % 4)][w % 4];
      ++out_pResult;
    }
  }
}

static void* ConvertToRGBA8(gfl2::gfx::IGLAllocator* pAllocator, u8* pData, u8*& out_pNextData, u32 width, u32 height, TexFormat format)
{
  u32 loopCount = width * height;


  // RGBA8に展開しつつバイトオーダーも修正
  texel8888* pResult = (texel8888*)pAllocator->SystemMemoryMallocAlign(loopCount * sizeof(texel8888), 128);
  texel8888* pTmpResult = pResult;

  switch (format)
  {
  case TEX_FORMAT_A4:
    {
      struct texel44
      {
        u8 a0 : 4;
        u8 a1 : 4;
      };
      texel44* data = (texel44*)pData;

      loopCount /= 2;
      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = 255;
        pResult->g = 255;
        pResult->b = 255;
        pResult->a = 255 * data->a0 / 15;

        ++pResult;

        pResult->r = 255;
        pResult->g = 255;
        pResult->b = 255;
        pResult->a = 255 * data->a1 / 15;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_A8:
    {
      struct texel8
      {
        u8 a;
      };
      texel8* data = (texel8*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = 255;
        pResult->g = 255;
        pResult->b = 255;
        pResult->a = data->a;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_L4:
    {
      struct texel44
      {
        u8 l0 : 4;
        u8 l1 : 4;
      };
      texel44* data = (texel44*)pData;

      loopCount /= 2;
      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = 255 * data->l0 / 15;
        pResult->g = 255 * data->l0 / 15;
        pResult->b = 255 * data->l0 / 15;
        pResult->a = 255;

        ++pResult;

        pResult->r = 255 * data->l1 / 15;
        pResult->g = 255 * data->l1 / 15;
        pResult->b = 255 * data->l1 / 15;
        pResult->a = 255;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_L8:
    {
      struct texel8
      {
        u8 l;
      };
      texel8* data = (texel8*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = data->l;
        pResult->g = data->l;
        pResult->b = data->l;
        pResult->a = 255;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_LA4:
    {
      struct texel88
      {
        u8 a : 4;
        u8 l : 4;
      };
      texel88* data = (texel88*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = 255 * data->l / 15;
        pResult->g = 255 * data->l / 15;
        pResult->b = 255 * data->l / 15;
        pResult->a = 255 * data->a / 15;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_LA8:
    {
      struct texel88
      {
        u8 a;
        u8 l;
      };
      texel88* data = (texel88*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = data->l;
        pResult->g = data->l;
        pResult->b = data->l;
        pResult->a = data->a;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_HILO8:
    {
      struct texel88
      {
        u8 g;
        u8 r;
      };
      texel88* data = (texel88*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = data->r;
        pResult->g = data->g;
        pResult->b = 0;
        pResult->a = 255;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_RGB5_A1:
    {
      struct texel5551_r
      {
        u16 a : 1;
        u16 b : 5;
        u16 g : 5;
        u16 r : 5;
      };
      texel5551_r* data = (texel5551_r*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = (u8)(255 * data->r / 31);
        pResult->g = (u8)(255 * data->g / 31);
        pResult->b = (u8)(255 * data->b / 31);
        pResult->a = (u8)(255 * data->a);

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_RGBA4:
    {
      struct texel4444_r
      {
        u16 a : 4;
        u16 b : 4;
        u16 g : 4;
        u16 r : 4;
      };
      texel4444_r* data = (texel4444_r*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = (u8)(255 * data->r / 15);
        pResult->g = (u8)(255 * data->g / 15);
        pResult->b = (u8)(255 * data->b / 15);
        pResult->a = (u8)(255 * data->a / 15);

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_RGB565:
    {
      struct texel565_r
      {
        u16 b : 5;
        u16 g : 6;
        u16 r : 5;
      };
      texel565_r* data = (texel565_r*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = (u8)(255 * data->r / 31);
        pResult->g = (u8)(255 * data->g / 63);
        pResult->b = (u8)(255 * data->b / 31);
        pResult->a = 255;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_RGB8:
    {
      struct texel888_r
      {
        u8 b;
        u8 g;
        u8 r;
      };
      texel888_r* data = (texel888_r*)pData;
		
      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = data->r;
        pResult->g = data->g;
        pResult->b = data->b;
        pResult->a = 255; 
				
        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_RGBA8:
    {
      struct texel8888_r
      {
        u8 a;
        u8 b;
        u8 g;
        u8 r;
      };
      texel8888_r* data = (texel8888_r*)pData;

      for (u32 i = 0; i < loopCount; ++i)
      {
        pResult->r = data->r;
        pResult->g = data->g;
        pResult->b = data->b;
        pResult->a = data->a;

        ++data;
        ++pResult;
      }

      out_pNextData = (u8*)data;
    }
    break;

  case TEX_FORMAT_ETC1:
  case TEX_FORMAT_ETC1_A4:
    {
      DecodeETC1(pResult, pData, out_pNextData, width, height, format);
    }
    break;
  }

  return pTmpResult;
}

static void ConvFlipV(texel8888* pData, u32 width, u32 height)
{
	// 上下を入れ替える
	for (u32 h = 0; h < height; ++h)
	{
		u32 hFlip = height - (h + 1);
		if (h >= hFlip)
		{
			break;
		}

		for (u32 w = 0; w < width; ++w)
		{
			std::swap(pData[h * width + w], pData[hFlip * width + w]);
		}
	}
}
/*
static void ConvFlipV(texel8888* pData, u32 width, u32 height)
{
	// OPTIMIZED:
	texel8888 *pDataBackup = new texel8888[width*height];
	memcpy(pDataBackup,pData,width*height*sizeof(texel8888));

	int sourceOff = 0,destOff = width*(height-1);
  // 上下を入れ替える
  for (u32 h = 0; h < height; ++h)
  {
		// Copy one line at a time:
		memcpy(pData + sourceOff,pDataBackup + destOff, sizeof(texel8888)*width );
		sourceOff+=width;
		destOff-=width;
  }

	delete[] pDataBackup;
}
*/
//-----------------------------------------------------------------------------
/// @brief Nw4cTgaの読み込み
//-----------------------------------------------------------------------------
void ImageData::readNw4cTgaData(const void* buffer)
{
  GFL_WARNING_PUSH
  GFL_WARNING_WIN32_IGNORE(4200)
  // nw4cの付加情報
  struct DataBlock
  {
    char dataType[8];
    u32 dataBlockSize;
    u8 data[];
  };
  GFL_WARNING_POP

  m_Format = Format::A8R8G8B8;
  m_Depth = 1;
  TexFormat format = TEX_FORMAT_RGBA8;
  u8* pSeek = (u8*)buffer;

  bool isEnd = false;
  while (!isEnd)
  {
    DataBlock* pBlock = (DataBlock*)pSeek;

    if (std::memcmp(pBlock->dataType, "nw4c_tfm", sizeof(pBlock->dataType)) == 0) // テクスチャフォーマット
    {
      // データサイズより比較するサイズの方が大きくなるけど下にデータもあるし大丈夫
      u32 dataSize = pBlock->dataBlockSize - sizeof(DataBlock);
      if (std::memcmp(pBlock->data, "L4", dataSize) == 0)
      {
        format = TEX_FORMAT_L4;
      }
      else if (std::memcmp(pBlock->data, "L8", dataSize) == 0)
      {
        format = TEX_FORMAT_L8;
      }
      else if (std::memcmp(pBlock->data, "A4", dataSize) == 0)
      {
        format = TEX_FORMAT_A4;
      }
      else if (std::memcmp(pBlock->data, "A8", dataSize) == 0)
      {
        format = TEX_FORMAT_A8;
      }
      else if (std::memcmp(pBlock->data, "La4", dataSize) == 0)
      {
        format = TEX_FORMAT_LA4;
      }
      else if (std::memcmp(pBlock->data, "La8", dataSize) == 0)
      {
        format = TEX_FORMAT_LA8;
      }
      else if (std::memcmp(pBlock->data, "Hilo8", dataSize) == 0)
      {
        format = TEX_FORMAT_HILO8;
      }
      else if (std::memcmp(pBlock->data, "Rgb565", dataSize) == 0)
      {
        format = TEX_FORMAT_RGB565;
      }
      else if (std::memcmp(pBlock->data, "Rgb8", dataSize) == 0)
      {
        format = TEX_FORMAT_RGB8;
      }
      else if (std::memcmp(pBlock->data, "Rgb5_a1", dataSize) == 0)
      {
        format = TEX_FORMAT_RGB5_A1;
      }
      else if (std::memcmp(pBlock->data, "Rgba4", dataSize) == 0)
      {
        format = TEX_FORMAT_RGBA4;
      }
      else if (std::memcmp(pBlock->data, "Rgba8", dataSize) == 0)
      {
        format = TEX_FORMAT_RGBA8;
      }
      else if (std::memcmp(pBlock->data, "Etc1", dataSize) == 0)
      {
        format = TEX_FORMAT_ETC1;
      }
      else if (std::memcmp(pBlock->data, "Etc1_a4", dataSize) == 0)
      {
        format = TEX_FORMAT_ETC1_A4;
      }
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_cub", sizeof(pBlock->dataType)) == 0) // キューブマップなら存在
    {
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_mps", sizeof(pBlock->dataType)) == 0) // ミップマップレベル
    {
      m_Depth = pBlock->data[0] - '0';
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_txd", sizeof(pBlock->dataType)) == 0) // テクスチャデータ
    {
      u32 width = m_Width;
      u32 height = m_Height;
      u8* pData = pBlock->data;
      for (u32 mipLevel = 0; mipLevel < m_Depth; ++mipLevel)
      {
        // 強制的にRGBA8に展開する
        u8* pNextData = NULL;
				m_pAllocData[mipLevel] = ConvertToRGBA8(m_pTemporaryAllocator, pData, pNextData, width, height, format);				
        m_pImageData[mipLevel] = m_pAllocData[mipLevel];
        pData = pNextData;

        // アドレッシング変換
        if ((format != TEX_FORMAT_ETC1) && (format != TEX_FORMAT_ETC1_A4))
        {
          ChangeAddressing((u8*)m_pImageData[mipLevel], width, height, format);
        }

        // 上下反転させる
        ConvFlipV((texel8888*)m_pImageData[mipLevel], width, height);

        width /= 2;
        height /= 2;
      }
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_udt", sizeof(pBlock->dataType)) == 0) // ユーザーデータ
    {
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_mpi", sizeof(pBlock->dataType)) == 0) // ミップマップデータ
    {
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_gnm", sizeof(pBlock->dataType)) == 0) // ツールの名前
    {
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_gvr", sizeof(pBlock->dataType)) == 0) // ツールのバージョン
    {
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_psh", sizeof(pBlock->dataType)) == 0) // PhotoShop専用データ
    {
    }
    else if (std::memcmp(pBlock->dataType, "nw4c_end", sizeof(pBlock->dataType)) == 0) // データ終端
    {
      isEnd = true;
    }

    // 次のブロックへ
    pSeek += pBlock->dataBlockSize;
  }
}

}}

#endif // #if defined(GF_PLATFORM_WIN32)

