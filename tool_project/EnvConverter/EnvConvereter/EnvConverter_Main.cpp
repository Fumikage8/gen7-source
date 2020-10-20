#include "stdafx.h"


#include "EnvConverter_Main.h"


#include <iostream>


#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>


////////////////////////////////////////////////////////////////////////////////
// デバッグ定義
// 大元
#define EnvConverter_DEBUG_ENABLE                (0)  // 0のとき無効、1のとき有効

// 終了前に入力待ちをする
#define EnvConverter_DEBUG_WAIT_INPUT_BEFORE_END (EnvConverter_DEBUG_ENABLE&&(1))  // 0のとき無効、1のとき有効
// デバッグ出力
#define EnvConverter_DEBUG_OUTPUT_ENABLE         (EnvConverter_DEBUG_ENABLE&&(1))
// 引数をごまかしてテストし易くする
#define EnvConverter_DEBUG_ARG_SET               (EnvConverter_DEBUG_ENABLE&&(1))


////////////////////////////////////////////////////////////////////////////////
// 定義
#define EnvConverter_GFBENV_SIGNATURE     "GFBENV\0\0"
#define EnvConverter_GFBENV_ALIGN         (4)

#define EnvConverter_GFBENV_VERSION_MAJOR (1)
#define EnvConverter_GFBENV_VERSION_MINOR (0)

#define EnvConverter_GFENV_VERSION_MAJOR  (1)
#define EnvConverter_GFENV_VERSION_MINOR  (0)

#define EnvConverter_GFBENV_DATE_VERSION  (0x15051320)


////////////////////////////////////////////////////////////////////////////////
class GLAllocator : public gfl2::gfx::IGLAllocator
{
public:
  GLAllocator() : gfl2::gfx::IGLAllocator(){}
  virtual ~GLAllocator(){}

#if GFL_DEBUG
  virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
#else
  virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment)
#endif
  {
    return malloc(size);
  }

  virtual void SystemMemoryFree(void* const address)
  {
    free( address );
  }

  virtual void* LocalMemoryMalloc(u32 size, u32 alignment)
  {
    return NULL;
  }

  virtual void LocalMemoryFree(void* const address)
  {

  }
};


////////////////////////////////////////////////////////////////////////////////
int EnvConverter_Main(int argc, _TCHAR* argv[])
{
  GLAllocator  allocator;
  gfl2::gfx::GFGL::Initialize(&allocator);

  EnvConverter envConverter;
  int ret = envConverter.Main(argc, argv);
  return ret;
}


////////////////////////////////////////////////////////////////////////////////
EnvConverter::EnvConverter(void)
  : m_inFileName(),
    m_outFileName(),
    m_resourceData(NULL)
{
  m_resourceData = new gfl2::renderingengine::scenegraph::resource::GfEnvData;
}

EnvConverter::~EnvConverter()
{
  if(m_resourceData) delete m_resourceData;
}


int EnvConverter::Main(int argc, _TCHAR* argv[])
{
  int ret = 0;
  
  do
  {
    // 引数解釈
    ret = readArg(argc, argv);
    if(ret != 0) break;

    // gfenvファイルを読み込む
    ret = readGfEnvFile();
    if(ret != 0) break;

    // gfbenvファイルを書き出す
    ret = writeGfBEnvFile();
    if(ret != 0) break;
  }
  while(0);

#if EnvConverter_DEBUG_WAIT_INPUT_BEFORE_END
  {
    char c;
    scanf_s("%c", &c);
  }
#endif

  return ret;
}


////////////////////////////////////////////////////////////////////////////////
int EnvConverter::readArg(int argc, _TCHAR* argv[])
{
  int ret = 0;

  do
  {
#if EnvConverter_DEBUG_ARG_SET
    int argc = 5;
    std::string argv[5] =
    {
      "",
      "-i",
      //"../DrawEnvTest.gfenv",
      "../kawatest/DrawEnv1_v1_0.gfenv",
      "-o",
      //"../DrawEnvTest.gfbenv"
      "../kawatest/DrawEnv1_v1_0.gfbenv",
    };
#endif

    if(argc != 5)
    {
      printLnError("ERROR: usage: EnvConverter -i inFileName -o outFileName");
      printLnError("version: "+std::to_string(EnvConverter_GFBENV_VERSION_MAJOR)+"."+std::to_string(EnvConverter_GFBENV_VERSION_MINOR));
      ret = 1;
      break;
    }
    else
    {
      enum
      {
        STATE_WAIT_OPTION,
        STATE_WAIT_IN_FILE_NAME,
        STATE_WAIT_OUT_FILE_NAME,
      };
      int state = STATE_WAIT_OPTION;
      
      for(int i=1; i<argc; ++i)
      {
        std::string argStr = argv[i];
        switch(state)
        {
        case STATE_WAIT_OPTION:
          if(argStr == "-i")
          {
            state = STATE_WAIT_IN_FILE_NAME;
          }
          else if(argStr == "-o")
          {
            state = STATE_WAIT_OUT_FILE_NAME;
          }
          else
          {
            printLnError("ERROR: "+argStr+" is unknown option");
            ret = 1;
          }
          break;
        case STATE_WAIT_IN_FILE_NAME:
          m_inFileName = argStr;
          state = STATE_WAIT_OPTION;
          break;
        case STATE_WAIT_OUT_FILE_NAME:
          m_outFileName = argStr;
          state = STATE_WAIT_OPTION;
          break;
        }
        if(ret != 0) break;
      }

      if(ret == 0)
      {
        if(m_inFileName.length() == 0 || m_outFileName.length() == 0)
        {
          printLnError("ERROR: inFileName or outFileName is NULL");
          ret = 1;
          break;
        }
      }
    }
  }
  while(0);

  return ret;
}

int EnvConverter::readGfEnvFile(void)
{
  int ret = 0;

  do
  {
    // 読み込む
    bool readFileRet = m_resourceData->ReadFile( m_inFileName );

    if(readFileRet == false)
    {
      printLnError("ERROR: I can't read "+m_inFileName);
      ret = 1;
      break;
    }

    // 確認
    {
      u32       dataNum = m_resourceData->GetSize();
      u32       versionId = m_resourceData->GetVersionId();

      u32 envTextureDataNum = m_resourceData->GetEnvTextureDataNum();
      u32 lightSetNum       = m_resourceData->GetLightSetNum();
      u32 cameraNum         = m_resourceData->GetCameraNum();

      printLnDebug(std::to_string(dataNum));
      printLnDebug(std::to_string(envTextureDataNum));
      printLnDebug(std::to_string(lightSetNum));
      printLnDebug(std::to_string(cameraNum));
  
      for( u32 dataIdx = 0; dataIdx < dataNum; ++dataIdx )
      {
        gfl2::renderingengine::scenegraph::resource::IResourceData::Type  type = m_resourceData->GetType( dataIdx );
  
        const void*  data     = m_resourceData->GetData( dataIdx );
        u32          dataSize = m_resourceData->GetDataSize( dataIdx );

        printLnDebug(std::to_string(dataSize));
      }
    }
  }
  while(0);

  return ret;
}
int EnvConverter::writeGfBEnvFile(void) const
{
  int ret = 0;


  ////////
  // データサイズ取得
  ////////
  u32  dataNum         = 0;
  u32* dataSize        = NULL;
  u32* alignedDataSize = NULL;

  dataNum = m_resourceData->GetSize();
  dataSize        = new u32[dataNum];
  alignedDataSize = new u32[dataNum];
  for( u32 dataIdx = 0; dataIdx < dataNum; ++dataIdx )
  { 
    dataSize[dataIdx]        = m_resourceData->GetDataSize( dataIdx );
    alignedDataSize[dataIdx] = alignVal(dataSize[dataIdx], EnvConverter_GFBENV_ALIGN);
  } 


  ////////
  // 開く
  ////////
  FILE* file;
  fopen_s(&file, m_outFileName.c_str(), "wb");
  if(file == NULL)
  {
    printLnError("ERROR: I can't create "+m_outFileName);
    ret = 1;
  }


  ////////
  // 書き出す
  ////////
  if(ret == 0)
  {
    do
    {
      u32  byteCount         = 0;
      u32  startByteCount    = 0;
      u16  version           = 0;
      u32  dateVersion       = 0;
      u16  envTextureDataNum = 0;
      u16  lightSetNum       = 0;
      u16  cameraNum         = 0;
      
      // シグネチャ(8バイト)
      ret = writeFile(EnvConverter_GFBENV_SIGNATURE, 8, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 8;
 
      // gfbenvのバージョン(u16, 2バイト)(u16, 2バイト)
      version = EnvConverter_GFBENV_VERSION_MAJOR;
      ret = writeFile(&version, 2, 1, file, m_outFileName);  if(ret != 0) break;
      version = EnvConverter_GFBENV_VERSION_MINOR;
      ret = writeFile(&version, 2, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 2+2;
      
      // gfenvのバージョン(u16, 2バイト)(u16, 2バイト)
      version = EnvConverter_GFENV_VERSION_MAJOR;
      ret = writeFile(&version, 2, 1, file, m_outFileName);  if(ret != 0) break;
      version = EnvConverter_GFENV_VERSION_MINOR;
      ret = writeFile(&version, 2, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 2+2;

      // gfbenvの日付バージョン(u32, 2バイト)
      dateVersion = EnvConverter_GFBENV_DATE_VERSION;
      ret = writeFile(&dateVersion, 4, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 4;
 
      // EnvTextureDataの個数(u16, 2バイト)
      envTextureDataNum = m_resourceData->GetEnvTextureDataNum();
      ret = writeFile(&envTextureDataNum, 2, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 2;

      // LightSetの個数(u16, 2バイト)
      lightSetNum = m_resourceData->GetLightSetNum();
      ret = writeFile(&lightSetNum, 2, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 2;
 
      // Cameraの個数(u16, 2バイト)
      cameraNum   = m_resourceData->GetCameraNum();
      ret = writeFile(&cameraNum, 2, 1, file, m_outFileName);  if(ret != 0) break;
      byteCount += 2;

      // パディング(2バイト)
      ret = writeFileZero(2, file, m_outFileName);  if(ret != 0) break;
      byteCount += 2;
 
      // 開始位置(このファイルの先頭から何バイト目か)(u32, 4バイト)*(1+EnvTextureDataの個数+LightSetの個数+Cameraの個数)
      startByteCount = byteCount + (4+4) * (1+envTextureDataNum+lightSetNum+cameraNum);
      for( u32 dataIdx = 0; dataIdx < dataNum; ++dataIdx )
      { 
        ret = writeFile(&startByteCount, 4, 1, file, m_outFileName);  if(ret != 0) break;
        startByteCount += alignedDataSize[dataIdx];
        byteCount += 4;
      }
      if(ret != 0) break;
 
      // バイト数(u32, 4バイト)(u32, 4バイト)*(1+EnvTextureDataの個数+LightSetの個数+Cameraの個数)
      for( u32 dataIdx = 0; dataIdx < dataNum; ++dataIdx )
      {
        ret = writeFile(&(dataSize[dataIdx]), 4, 1, file, m_outFileName);  if(ret != 0) break;
        byteCount += 4;
      }
      if(ret != 0) break;
     
      // バイナリデータ
      for( u32 dataIdx = 0; dataIdx < dataNum; ++dataIdx )
      {
        const void*  data     = m_resourceData->GetData( dataIdx );
        ret = writeFile(data, dataSize[dataIdx], 1, file, m_outFileName);  if(ret != 0) break;
        
        u32 zeroNum = alignedDataSize[dataIdx] - dataSize[dataIdx];
        ret = writeFileZero(zeroNum, file, m_outFileName);  if(ret != 0) break;

        byteCount += alignedDataSize[dataIdx];
      }
      if(ret != 0) break;
    }
    while(0);
 
 
    ////////
    // 閉じる
    ////////
    int closeFileRet = fclose(file);
    if(closeFileRet != 0)
    {
      printLnError("ERROR: I can't close "+m_outFileName);
      ret = 1;
    }
  }


  ////////
  // 後始末
  ////////
  delete [] alignedDataSize;
  delete [] dataSize;


  return ret;
}

int EnvConverter::writeFileZero(size_t byteNum, FILE* file, std::string fileName)
{
  int ret = 0;
  const char zero = 0;
  for(size_t i=0; i<byteNum; ++i)
  {
    ret = writeFile(&zero, 1, 1, file, fileName);  if(ret != 0) break;
  }
  return ret;
}

int EnvConverter::writeFile(const void* buf, size_t bufSize, size_t count, FILE* file, std::string fileName)
{
  int ret = 0;
  size_t writeCount = fwrite(buf, bufSize, count, file);
  if(writeCount != count)
  {
    printLnError("ERROR: I can't write "+fileName);
    ret = 1;
  }
  return ret; 
}

u32 EnvConverter::alignVal(u32 val, u32 align)
{
  return ( (val+align-1)&(~(align-1)) );
}

void EnvConverter::printLnError(std::string str)
{
  std::cerr << str << std::endl;
}
void EnvConverter::printLnDebug(std::string str)
{
#if EnvConverter_DEBUG_OUTPUT_ENABLE
  std::cout << str << std::endl;
#endif
}

