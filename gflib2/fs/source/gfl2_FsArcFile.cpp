//==============================================================================
/**
 @file    gfl2_FsArcFile.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   garcファイル
 */
//==============================================================================


/*

用語解説

garc、アーカイブファイル・・・
gaix・・・
arcid・・・
arcdatid、データ・・・
言語・・・
シグネチャ・・・
バージョン・・・
ファストモード、AccessInfo、索引テーブル、AccessTable、ArcFileAccessor・・・
dataSize・・・
realDataSize・・・

*/


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsArcFile.h>
#include "ArcFileAccessor/gfl2_FsIArcFileAccessor.h"
//#include "ArcFileAccessor/ver.4.x/gfl2_FsArcFileAccessor.h"       // サポート終了に伴いコメントアウト。fs\source\ArcFileAccessor\ver.4.xはビルドされません。
//#include "ArcFileAccessor/ver.5.x/gfl2_FsArcFileAccessor_ver5.h"  // サポート終了に伴いコメントアウト。fs\source\ArcFileAccessor\ver.5.xはビルドされません。
#include "ArcFileAccessor/ver.6.x/gfl2_FsArcFileAccessor_ver6.h"

#include <fs/include/gfl2_FsPcUtil.h>


#if GFL_FS_GASC_ENABLE
#include "arcsrc/arcsrc_Accessor.h"
#endif




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


const u8 ArcFile::LATEST_MAJOR_VERSION = 6;


ArcFile::ArcFile(void)
  : FileBase(),
    m_arcId(ARCID_NULL),
    m_arcLang(ToolDefine::ArcLangType::JPN),
#if GFL_DEBUG
    m_majorVersion(0),
    m_minorVersion(0),
#endif
    m_accessor(NULL),
    m_blockData(NULL),
    m_fastAccessTable(NULL),
    m_dataNum(0),
    m_maxDataSize(0),
    m_maxRealDataSize(0)
#if GFL_FS_GASC_ENABLE
    , m_arcSrcBuf(NULL)
    , m_arcSrcAccessor(NULL)
    , m_arcSrcUseFlag(false)
#endif
{}
ArcFile::~ArcFile()
{
  Result res;
  res |= this->DestroyArcSrcAccessor();
  res |= this->Close();
}

Result ArcFile::Open(gfl2::heap::HeapBase* heapForFile, gfl2::heap::HeapBase* heapForTemp, const DeviceBase* device, const char* fileName)
{
  Result res;
  
  res |= this->open(heapForFile, device, fileName, Define::OpenMode::READ);

#if GFL_DEBUG
  if(res.IsSuccess()) res |= this->checkSignatureAndVersion(fileName);
#endif
  if(res.IsSuccess()) res |= this->setupForFastMode(heapForFile, heapForTemp);

  if(!res.IsSuccess()) this->Close();

  return res;
}

Result ArcFile::Close(void)
{
  GFL_SAFE_DELETE_ARRAY(m_fastAccessTable);
  GflHeapSafeFreeMemory(m_blockData);  // @fix cov_ctr[10071]: void*型にした
  GFL_SAFE_DELETE(m_accessor);

  return this->close();
}

Result ArcFile::GetDataCount(u32* pDataNum) const
{
  Result res;
  *pDataNum = m_dataNum;
  return res;
}

Result ArcFile::GetDataSize(size_t* pDataSize, ARCDATID datId, gfl2::heap::HeapBase* heapForTemp) const
{
  return GetDataSize(pDataSize, datId, m_arcLang, heapForTemp);
}
Result ArcFile::GetDataSize(size_t* pDataSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp) const
{
#if GFL_FS_GASC_ENABLE
  if( doesUseArcSrc() )
  {
    Result arcSrcRes;
    arcSrcRes |= this->getArcSrcDataSize(pDataSize, datId, lang, heapForTemp);
    if(arcSrcRes.IsSuccess()) return arcSrcRes;
  }
#endif

  Result res;
  
  if(datId < m_dataNum)
  {
    if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
    {
      *pDataSize = m_fastAccessTable[datId].dataSize;
    }
    else
    {
      *pDataSize = m_accessor->GetDataSize(datId, lang);
    }
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ARCDATID_INCORRECT;
  }
  
  return res;
} 

Result ArcFile::GetRealDataSize(size_t* pRealDataSize, ARCDATID datId, gfl2::heap::HeapBase* heapForTemp) const
{
  return GetRealDataSize(pRealDataSize, datId, m_arcLang, heapForTemp);
}
Result ArcFile::GetRealDataSize(size_t* pRealDataSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp) const
{
#if GFL_FS_GASC_ENABLE
  if( doesUseArcSrc() )
  {
    Result arcSrcRes;
    arcSrcRes |= this->getArcSrcDataSize(pRealDataSize, datId, lang, heapForTemp);
    if(arcSrcRes.IsSuccess()) return arcSrcRes;
  }
#endif

  Result res;
  
  if(datId < m_dataNum)
  {
    if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
    {
      *pRealDataSize = m_fastAccessTable[datId].realDataSize;
    }
    else
    {
      *pRealDataSize = m_accessor->GetRealDataSize(datId, lang);
    }
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ARCDATID_INCORRECT;
  }
  
  return res;
}

Result ArcFile::GetOffsetFromArchiveTop(size_t* pOffsetFromArchiveTop, ARCDATID datId) const
{
  return GetOffsetFromArchiveTop(pOffsetFromArchiveTop, datId, m_arcLang);
}
Result ArcFile::GetOffsetFromArchiveTop(size_t* pOffsetFromArchiveTop, ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const
{
  Result res;
  
  if(datId < m_dataNum)
  {
    if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
    {
      *pOffsetFromArchiveTop = m_fastAccessTable[datId].offsetFromArchiveTop;
    }
    else
    {
      *pOffsetFromArchiveTop = m_accessor->GetOffsetToData(datId, lang);
    }
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ARCDATID_INCORRECT;
  }
  
  return res;
}

Result ArcFile::GetMaxDataSize(size_t* pDataSize) const
{
  Result res;
  *pDataSize = m_maxDataSize;
  return res;
}
Result ArcFile::GetMaxRealDataSize(size_t* pDataSize) const
{
  Result res;
  *pDataSize = m_maxRealDataSize;
  return res;
}

Result ArcFile::LoadData(size_t* realReadSize, void* buf, size_t bufSize, ARCDATID datId, gfl2::heap::HeapBase* heapForTemp)
{
  return this->LoadData(realReadSize, buf, bufSize, datId, m_arcLang, heapForTemp);
}
Result ArcFile::LoadData(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp)
{
#if GFL_FS_GASC_ENABLE
  if( doesUseArcSrc() )
  {
    Result arcSrcRes;
    arcSrcRes |= this->loadArcSrcData(pRealReadSize, pBuf, bufSize, datId, lang, heapForTemp);
    if(arcSrcRes.IsSuccess()) return arcSrcRes;
  }
#endif

  Result res;
  
  if(datId < m_dataNum)
  {
    size_t dataSize = 0;
    size_t realDataSize = 0;
    size_t offsetFromArchiveTop = 0;
    if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
    {
      dataSize = m_fastAccessTable[datId].dataSize;
      realDataSize = m_fastAccessTable[datId].realDataSize;
      offsetFromArchiveTop = m_fastAccessTable[datId].offsetFromArchiveTop;
    }
    else
    {
      dataSize = m_accessor->GetDataSize(datId, lang);
      realDataSize = m_accessor->GetRealDataSize(datId, lang);
      offsetFromArchiveTop = m_accessor->GetOffsetToData(datId, lang);
    }
    res |= this->Read( pRealReadSize, pBuf, bufSize, offsetFromArchiveTop, dataSize );  // パディングを含むサイズ読み込む。
    *pRealReadSize = realDataSize;  // パディングを含まないサイズにしておく。
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ARCDATID_INCORRECT;
  }
  
  return res;
}

Result ArcFile::LoadData(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, ARCDATID datId, gfl2::heap::HeapBase* heapForBuf, u32 align, gfl2::heap::HeapBase* heapForTemp)
{
  return this->LoadData(ppBuf, pBufSize, pRealReadSize, datId, m_arcLang, heapForBuf, align, heapForTemp);
}
Result ArcFile::LoadData(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForBuf, u32 align, gfl2::heap::HeapBase* heapForTemp)
{
#if GFL_FS_GASC_ENABLE
  if( doesUseArcSrc() )
  {
    Result arcSrcRes;
    arcSrcRes |= this->loadArcSrcData(ppBuf, pBufSize, pRealReadSize, datId, lang, heapForBuf, align, heapForTemp);
    if(arcSrcRes.IsSuccess()) return arcSrcRes;
  }
#endif

  Result res;
  
  if(datId < m_dataNum)
  {
    size_t dataSize = 0;
    size_t realDataSize = 0;
    size_t offsetFromArchiveTop = 0;
    if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
    {
      dataSize = m_fastAccessTable[datId].dataSize;
      realDataSize = m_fastAccessTable[datId].realDataSize;
      offsetFromArchiveTop = m_fastAccessTable[datId].offsetFromArchiveTop;
    }
    else
    {
      dataSize = m_accessor->GetDataSize(datId, lang);
      realDataSize = m_accessor->GetRealDataSize(datId, lang);
      offsetFromArchiveTop = m_accessor->GetOffsetToData(datId, lang);
    }
    res |= this->Read(ppBuf, pBufSize, pRealReadSize, offsetFromArchiveTop, dataSize, heapForBuf, align);  // パディングを含むサイズ読み込む。
    *pRealReadSize = realDataSize;  // パディングを含まないサイズにしておく。
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ARCDATID_INCORRECT;
  }
  
  return res;
}

Result ArcFile::LoadDataPiece(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, size_t offsetFromData, size_t readSize )
{
  return this->LoadDataPiece(pRealReadSize, pBuf, bufSize, datId, m_arcLang, offsetFromData, readSize);
}
Result ArcFile::LoadDataPiece(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, size_t offsetFromData, size_t readSize )
{
  Result res;
  res |= this->SeekDataPtr(datId, lang, offsetFromData);
  if(res.IsSuccess())
  {
    res |= this->LoadDataPtr(pRealReadSize, pBuf, bufSize, readSize);
  }
  return res;
}

Result ArcFile::SeekDataPtr( ARCDATID datId, size_t offsetFromData )
{
  return this->SeekDataPtr(datId, m_arcLang, offsetFromData);
}
Result ArcFile::SeekDataPtr( ARCDATID datId, ToolDefine::ArcLangType::Tag lang, size_t offsetFromData )
{
  Result res;
  
  if(datId < m_dataNum)
  {
    size_t offsetFromArchiveTop = 0;
    if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
    {
      offsetFromArchiveTop = m_fastAccessTable[datId].offsetFromArchiveTop;
    }
    else
    {
      offsetFromArchiveTop = m_accessor->GetOffsetToData(datId, lang);
    }
    res |= this->Seek( offsetFromArchiveTop + offsetFromData, Define::Origin::BEGIN );
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ARCDATID_INCORRECT;
  }

  return res;
}

Result ArcFile::LoadDataPtr( size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize )
{
  Result res;
  res |= this->Read(pRealReadSize, pBuf, bufSize, readSize);
  return res;
}


Result ArcFile::CreateArcSrcAccessor(gfl2::heap::HeapBase* heapForArcSrc, gfl2::heap::HeapBase* heapForTemp, const DeviceBase* device, const char* arcSrcFileName)
{
  Result res;
#if GFL_FS_GASC_ENABLE
  // ファイル読み込み
  {
    GFL_UNUSED(device);
    GFL_UNUSED(heapForTemp);  // 今はheapForTempを使用していないが、deviceを指定するようにしてPcUtilをやめたら、一時的に使用するメモリとして必要になるかもしれないので残しておく。
    size_t bufSize  = 0;
    size_t fileSize = 0;
    res |= PcUtil::ReadFile(&m_arcSrcBuf, &bufSize, &fileSize, arcSrcFileName, heapForArcSrc, 4);
  }
  // アクセッサ作成
  {
    if(res.IsSuccess())
    {
      m_arcSrcAccessor = GFL_NEW(heapForArcSrc) arcsrc::Accessor;
      m_arcSrcAccessor->SetBuf(m_arcSrcBuf);
    }
  }
#endif
  return res;
}

Result ArcFile::DestroyArcSrcAccessor(void)
{
  Result res;
#if GFL_FS_GASC_ENABLE
  GFL_SAFE_DELETE(m_arcSrcAccessor);
  GflHeapSafeFreeMemory(m_arcSrcBuf);
#endif
  return res;
}

void ArcFile::SetArcSrcUseFlag(bool useFlag)
{
#if GFL_FS_GASC_ENABLE
  m_arcSrcUseFlag = useFlag;
#else
  GFL_UNUSED(useFlag);
#endif
}
bool ArcFile::GetArcSrcUseFlag(void) const
{
#if GFL_FS_GASC_ENABLE
  return m_arcSrcUseFlag;
#else
  return false;
#endif
}


#if GFL_DEBUG
Result ArcFile::checkSignatureAndVersion(const char* fileName)
{
  Result res;

  // ファイルの先頭数バイトを読み込む
  u8 buf[4+4+2+2] = {0};  // signature+size+byte_order+version

  size_t realReadSize = 0;
  res |= this->Read( &realReadSize, buf, 12, 0, 12 );
 
  // ファイルシグネチャの確認
  if(res.IsSuccess()) res |= checkSignature(fileName, buf);
  // ファイルバージョンの確認
  if(res.IsSuccess()) res |= checkVersion(&m_majorVersion, &m_minorVersion, fileName, buf);

  return res;
}

Result ArcFile::checkSignature(const char* fileName, const u8* buf)
{
  // ファイルシグネチャの確認
  Result res;

  u8 signature[4] = {0};
  signature[0] = buf[0];
  signature[1] = buf[1];
  signature[2] = buf[2];
  signature[3] = buf[3];
 
  // 比較
  if( ( signature[0] != 'C' ) || 
      ( signature[1] != 'R' ) || 
      ( signature[2] != 'A' ) || 
      ( signature[3] != 'G' ) ) {
    GFL_PRINT( "ERROR!! *.garc以外のファイル %s を読み込みました。\n", fileName );
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT。ケア済みなのでこのまま残す。
    res |= Result::Bit::FILE_SIGNATURE_INCORRECT;
  }

  return res;
}

Result ArcFile::checkVersion(u8* a_majorVersion, u8* a_minorVersion, const char* fileName, const u8* buf)
{
  // ファイルバージョンの確認
  
  Result res;

  // ファイルバージョンの確認
  //const u8 SUPPORTED_MAJOR_VERSION[] = { 2, 3, LATEST_MAJOR_VERSION };  // ver.2.x, ver.3.x ver.4.x がサポート対象
  const u8 SUPPORTED_MAJOR_VERSION[] = { 4, 5, LATEST_MAJOR_VERSION };  // ver.4.x, ver.5.x, ver.6.x がサポート対象

  // 設定
  u8 majorVersion = buf[11];
  u8 minorVersion = buf[10];

  // 比較
  bool supported = false;
  for( u32 i=0; i<GFL_NELEMS(SUPPORTED_MAJOR_VERSION); i++ ) {
    if( majorVersion == SUPPORTED_MAJOR_VERSION[i] ) {
      supported = true;
      break;
    }
  }

  if( !supported ) 
  {
    GFL_PRINT( "ERROR!! アーカイブファイル %s は ver.%d.%d で、ライブラリのサポートは終了しています。 ver.%d.x に再アーカイブしてください。\n",
        fileName, majorVersion, minorVersion, LATEST_MAJOR_VERSION );
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT。ケア済みなのでこのまま残す。
    res |= Result::Bit::FILE_VERSION_TOO_OLD;
  }
  else if( majorVersion != LATEST_MAJOR_VERSION ) 
  {
    GFL_PRINT( "WARNING!! アーカイブファイル %s は ver.%d.%d で、ライブラリのサポートは終了予定です。 ver.%d.x に再アーカイブしてください。\n",
        fileName, majorVersion, minorVersion, LATEST_MAJOR_VERSION );
  }

  *a_majorVersion = majorVersion;
  *a_minorVersion = minorVersion;

  return res;
}
#endif

Result ArcFile::setupForFastMode(gfl2::heap::HeapBase* heapForFile, gfl2::heap::HeapBase* heapForTemp)
{
  Result res;

  // m_arcLang==ToolDefine::ArcLangType::ALL_LANGのときで初期化し、直後に直す。
  gfl2::heap::HeapBase* heapForAccessorAndBlockData = heapForFile;
  gfl2::heap::HeapBase* heapForAccessTable          = NULL;
  if(m_arcLang != ToolDefine::ArcLangType::ALL_LANG)
  {
    heapForAccessorAndBlockData = heapForTemp;
    heapForAccessTable          = heapForFile;
  }

//#if GFL_DEBUG
//  m_accessor = createArcFileAccessor(heapForAccessorAndBlockData, m_majorVersion);  // サポート終了に伴いコメントアウト
//#else
  m_accessor = createArcFileAccessor(heapForAccessorAndBlockData, LATEST_MAJOR_VERSION);
//#endif
  if(m_accessor)
  {
    res |= this->createBlockData(&m_blockData, heapForAccessorAndBlockData, m_accessor);
    if(res.IsSuccess())
    {
      m_accessor->AttachData(m_blockData);

      if(m_arcLang != ToolDefine::ArcLangType::ALL_LANG)
      {
        m_fastAccessTable = createDataAccessTable(heapForAccessTable, m_accessor, m_arcLang);
        if(m_fastAccessTable == NULL)
        {
          res |= Result::Bit::ARC_FILE_ACCESS_INFO_TABLE_CREATE_FAIL;
        }
      }

      m_dataNum         = m_accessor->GetFileIdNum();
      m_maxDataSize     = static_cast<u32>(m_accessor->GetMaxDataSize());
      m_maxRealDataSize = static_cast<u32>(m_accessor->GetMaxRealDataSize());
      /*u32 alignment     = static_cast<u32>(m_accessor->GetAlignment());*/
    }
  }
  else
  {
    res |= Result::Bit::ARC_FILE_ACCESSOR_CREATE_FAIL;
  }
  
  if(m_arcLang!=ToolDefine::ArcLangType::ALL_LANG)
  {
    GflHeapSafeFreeMemory(m_blockData);  // @fix cov_ctr[11747]: void*型にした
    GFL_SAFE_DELETE(m_accessor);
  }

  return res;
}

IArcFileAccessor* ArcFile::createArcFileAccessor(gfl2::heap::HeapBase* heap, u8 majorVersion)
{
  IArcFileAccessor* accessor = NULL;

  switch( majorVersion ) {
  //case 2: accessor = GFL_NEW( heap ) ver2::ArcFileAccessor(); break; 
  //case 3: accessor = GFL_NEW( heap ) ver3::ArcFileAccessor(); break; 
  //case 4: accessor = GFL_NEW( heap ) ver4::ArcFileAccessor(); break;  // サポート終了に伴いコメントアウト
  //case 5: accessor = GFL_NEW( heap ) ver5::ArcFileAccessor(); break;  // サポート終了に伴いコメントアウト
  case LATEST_MAJOR_VERSION: accessor = GFL_NEW( heap ) ver6::ArcFileAccessor(); break; 
  } 
  
  GFL_ASSERT(accessor);  // 開発中に気付かせるASSERT。ケア済みなのでこのまま残す。

  return accessor;
}

Result ArcFile::createBlockData(void** pBlockData, gfl2::heap::HeapBase* heap, const IArcFileAccessor* accessor)
{
  Result res;
  void* blockData = NULL;
  
  size_t totalBlockSize = 0;
  res |= readTotalBlockSize(&totalBlockSize, accessor);

  if(res.IsSuccess())
  {
    blockData = GflHeapAllocMemoryAlign(heap, totalBlockSize, 4);  // @fix cov_ctr[10071]: void*型にした  // @fix cov_ctr[11747]: void*型にした
 
    size_t realReadSize = 0;
    res |= this->Read(&realReadSize, blockData, totalBlockSize, 0, totalBlockSize);

    *pBlockData = blockData;
  }
  else
  {
    res |= Result::Bit::ARC_FILE_BLOCK_DATA_CREATE_FAIL;
  }

  return res;
}

Result ArcFile::readTotalBlockSize(size_t* totalBlockSize, const IArcFileAccessor* accessor)
{
  Result res;
  u32 l_totalBlockSize = 0;

  size_t offsetToTotalBlockSize = accessor->GetOffsetToTotalBlockSize();

  size_t realReadSize = 0;
  res |= this->Read(&realReadSize, &l_totalBlockSize, sizeof(u32), offsetToTotalBlockSize, sizeof(u32));

  *totalBlockSize = static_cast<size_t>(l_totalBlockSize);

  return res;
}

ArcFile::AccessInfo* ArcFile::createDataAccessTable(gfl2::heap::HeapBase* heap, const IArcFileAccessor* accessor, ToolDefine::ArcLangType::Tag arcLang)
{
  u32 file_id_num = accessor->GetFileIdNum();
  AccessInfo* access_table = GFL_NEW_ARRAY(heap) ArcFile::AccessInfo[ file_id_num ];

  for( u32 file_id=0; file_id<file_id_num; file_id++ ) {
    access_table[ file_id ].dataSize = static_cast<u32>(accessor->GetDataSize( file_id, arcLang ));
    access_table[ file_id ].realDataSize = static_cast<u32>(accessor->GetRealDataSize( file_id, arcLang ));
    access_table[ file_id ].offsetFromArchiveTop = static_cast<u32>(accessor->GetOffsetToData( file_id, arcLang ));
  } 
  return access_table;
}

#if GFL_FS_GASC_ENABLE
bool ArcFile::doesUseArcSrc(void) const
{
  return (m_arcSrcAccessor!=NULL && m_arcSrcUseFlag);
}

Result ArcFile::getArcSrcDataSize(size_t* pDataSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp) const
{
  // doesUseArcSrcがtrueのときしかこの関数を呼ばないようにして下さい。
  Result res;

  GFL_UNUSED(heapForTemp);  // 今はheapForTempを使用していないが、deviceを指定するようにしてPcUtilをやめたら、一時的に使用するメモリとして必要になるかもしれないので残しておく。

  char prevDir[Define::FILE_NAME_FULL_LEN_MAX];
  res |= PcUtil::SetCurrentWorkDirectory(m_arcSrcAccessor->GetWorkDirName(), prevDir, Define::FILE_NAME_FULL_LEN_MAX);
  if(res.IsSuccess())
  {
    res |= PcUtil::GetFileSize(pDataSize, m_arcSrcAccessor->GetFileName(datId, lang));
  }
  res |= PcUtil::SetCurrentWorkDirectory(prevDir, NULL, 0);

  return res;
}
Result ArcFile::loadArcSrcData(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp)
{
  // doesUseArcSrcがtrueのときしかこの関数を呼ばないようにして下さい。
  Result res;

  GFL_UNUSED(heapForTemp);  // 今はheapForTempを使用していないが、deviceを指定するようにしてPcUtilをやめたら、一時的に使用するメモリとして必要になるかもしれないので残しておく。
  
  char prevDir[Define::FILE_NAME_FULL_LEN_MAX];
  res |= PcUtil::SetCurrentWorkDirectory(m_arcSrcAccessor->GetWorkDirName(), prevDir, Define::FILE_NAME_FULL_LEN_MAX);
  if(res.IsSuccess())
  {
    res |= PcUtil::ReadFile(pRealReadSize, pBuf, bufSize, m_arcSrcAccessor->GetFileName(datId, lang));
  }
  res |= PcUtil::SetCurrentWorkDirectory(prevDir, NULL, 0);
  
  return res;
}
Result ArcFile::loadArcSrcData(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForBuf, u32 align, gfl2::heap::HeapBase* heapForTemp)
{
  // doesUseArcSrcがtrueのときしかこの関数を呼ばないようにして下さい。
  Result res;

  GFL_UNUSED(heapForTemp);  // 今はheapForTempを使用していないが、deviceを指定するようにしてPcUtilをやめたら、一時的に使用するメモリとして必要になるかもしれないので残しておく。
  
  char prevDir[Define::FILE_NAME_FULL_LEN_MAX];
  res |= PcUtil::SetCurrentWorkDirectory(m_arcSrcAccessor->GetWorkDirName(), prevDir, Define::FILE_NAME_FULL_LEN_MAX);
  if(res.IsSuccess())
  {
    res |= PcUtil::ReadFile(ppBuf, pBufSize, pRealReadSize, m_arcSrcAccessor->GetFileName(datId, lang), heapForBuf, align);
  }
  res |= PcUtil::SetCurrentWorkDirectory(prevDir, NULL, 0);
  
  return res;
}
#endif  // GFL_FS_GASC_ENABLE


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

