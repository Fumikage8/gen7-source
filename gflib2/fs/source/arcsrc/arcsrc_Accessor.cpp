//=============================================================================
/**
 * @brief  「garcのソースを記録したファイル」にアクセスする
 * @file   arcsrc_Accessor.h
 * @author kawada_koji
 * @date   2014.12.12
 */
//=============================================================================


#include "arcsrc_Accessor.h"


namespace arcsrc
{


Accessor::Accessor(void)
  : m_buf(0),
    m_dataNum(0),
    m_tablePosOfDataInfoPos(0),
    m_workDirName(0)
{}

Accessor::~Accessor()
{}

void Accessor::SetBuf(const void* buf)
{
  m_buf = buf;
  if(m_buf)
  {
    // 「garcのソースを記録したファイル」を解析する
    const u8* bufArray =  static_cast<const u8*>(m_buf);
    u32 byteCount = 0;
 
    /*u32 signature = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));*/  byteCount += 4;
    /*u16 version   = *(static_cast<const u16*>(static_cast<const void*>(&bufArray[byteCount])));*/  byteCount += 2;
    byteCount += 2;

    m_dataNum = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));  byteCount += 4;

    m_tablePosOfDataInfoPos = byteCount;

    byteCount = m_tablePosOfDataInfoPos + m_dataNum *4;
    m_workDirName = static_cast<const char*>(static_cast<const void*>(&bufArray[byteCount]));
  }
  else
  {
    m_dataNum = 0;
    m_tablePosOfDataInfoPos = 0;
    m_workDirName = 0;
  }
}

size_t Accessor::GetDataCount(void) const
{
  return m_dataNum;
}

const char* Accessor::GetWorkDirName(void) const
{
  return m_workDirName;
}

size_t Accessor::GetDataPosInArcFile(ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const
{
  if(m_buf)
  {
    if(0<=datId && datId<m_dataNum)
    {
      // ファイル情報を解析する
      const u8* bufArray = static_cast<const u8*>(m_buf);
      u32 byteCount = this->getFileInfoPos(datId, lang);

      if(byteCount > 0)
      {
        u32 dataPosInArcFile = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));
        return dataPosInArcFile;
      }
    }
  }
  return 0;
}

const char* Accessor::GetFileName(ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const
{
  if(m_buf)
  {
    if(0<=datId && datId<m_dataNum)
    {
      // ファイル情報を解析する
      const u8* bufArray = static_cast<const u8*>(m_buf);
      u32 byteCount = this->getFileInfoPos(datId, lang);

      if(byteCount > 0)
      {
        byteCount += 4;
        const char* fileName = static_cast<const char*>(static_cast<const void*>(&bufArray[byteCount]));
        return fileName;
      }     
    }
  }
  return 0;
}

size_t Accessor::getDataInfoPos(ARCDATID datId) const
{
  if(m_buf)
  {
    if(0<=datId && datId<m_dataNum)
    {
      // 「データ情報の開始位置」を解析する
      const u8* bufArray = static_cast<const u8*>(m_buf);
      u32 byteCount = m_tablePosOfDataInfoPos + datId * 4;

      u32 dataInfoPos = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));
      return dataInfoPos;
    }
  }
  return 0;
}

size_t Accessor::getFileInfoPos(ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const
{
  if(m_buf)
  {
    if(0<=datId && datId<m_dataNum)
    {
      // データ情報を解析する
      const u8* bufArray = static_cast<const u8*>(m_buf);
      u32 byteCount = this->getDataInfoPos(datId);

      if(byteCount > 0)
      {
        u32 langBit = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));  byteCount += 4;

        s32 i = ToolDefine::ArcLangType::DEF;
        u32 fileInfoPos = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));
        byteCount += 4;
        ++i;

        // ToolDefine::ArcLangType::DEF以降
        while(i<ToolDefine::ArcLangType::LANG_NUM)
        {
          if( langBit & (1<<i) )
          {
            if(lang == i)
            {
              fileInfoPos = *(static_cast<const u32*>(static_cast<const void*>(&bufArray[byteCount])));
              break;
            }
            byteCount += 4;
          }
          ++i;
        }

        return fileInfoPos;
      }
    }
  }
  return 0;
}


}  // arcsrc
