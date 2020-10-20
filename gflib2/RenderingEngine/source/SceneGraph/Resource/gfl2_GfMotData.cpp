#if defined(GF_PLATFORM_WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <debug/include/gfl2_Assert.h>

#include <Windows.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>

using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

static std::vector<std::shared_ptr<c8>> s_cacheMemoryList;

static std::string GetAnimationConverterPath()
{
  size_t requiredSize = 0;
  char niji_tool_dir[256];
  getenv_s(&requiredSize, niji_tool_dir, sizeof(niji_tool_dir), "NIJI_TOOL_DIR");

  return std::string(niji_tool_dir) + "AnimationConverter\\AnimationConverter.exe";
}

std::string GetAnimationConverterVersion()
{
  std::string converterPath = GetAnimationConverterPath();

  DWORD reserved = 0;
  DWORD fileVesionInfoSize = GetFileVersionInfoSize(converterPath.c_str(), &reserved);

  std::vector<u8> fileVersionBuf(fileVesionInfoSize);
  BOOL result = GetFileVersionInfo(converterPath.c_str(), 0, fileVesionInfoSize, &fileVersionBuf[0]);

  LPWORD lpWord = NULL;
  UINT dwLen;
  result = VerQueryValue(&fileVersionBuf[0], "\\VarFileInfo\\Translation", (void**)&lpWord, &dwLen);
  if (result)
  {
    DWORD langAndCharSet = MAKELONG(lpWord[1], lpWord[0]);

    char key[256];
    LPSTR lpStr;
    sprintf_s( key, "\\StringFileInfo\\%08x\\%s", langAndCharSet, "FileVersion" );
    result = VerQueryValue( &fileVersionBuf[0], key,  (void **)&lpStr, &dwLen );

    if (result)
    {
      return lpStr;
    }
  }

  return "0";
}

void AnimationConvert(const std::string& mode, const std::string& inputPath, const std::string& outputPath)
{
  std::string converterPath = GetAnimationConverterPath();

  std::string option;
  option = "-m " + mode + " -i " + inputPath + " -o " + outputPath;

  std::string	arg = converterPath + " " + option;

  STARTUPINFO si;// スタートアップ情報
  PROCESS_INFORMATION pi;// プロセス情報

  ZeroMemory(&si,sizeof(si));
  si.cb=sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;

  CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );
  CloseHandle(pi.hThread);
  WaitForSingleObject(pi.hProcess,INFINITE);
  CloseHandle(pi.hProcess);
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfMotData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfMotData::Impl : public gfx::GLMemory, public rapidxml::xml_document<>
{
public:

	Impl();
	virtual ~Impl();

	bool ReadData(const std::string& filePath, const std::string& fileDir, c8* pMotionData);

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );

  c8* m_pData;
  u32 m_DataSize;
};

GfMotData::Impl::Impl() : gfx::GLMemory(),
  m_pData(NULL),
  m_DataSize(0)
{
}

GfMotData::Impl::~Impl()
{
}

#pragma comment(lib, "version.lib")
bool GfMotData::Impl::ReadData(const std::string& filePath, const std::string& fileDir, c8* pMotionData)
{
  static std::string s_converterVersion = GetAnimationConverterVersion();

  util::FnvHash hash( reinterpret_cast<u8*>(pMotionData) );
  char cacheFilePath[ParserUtil::GFL_MAX_FILEPATH];	
  memset(cacheFilePath, 0, sizeof(cacheFilePath));
  sprintf_s( cacheFilePath, "%sAnimationCache/%s.%d.cache", fileDir.c_str(), s_converterVersion.c_str(), hash.value );	

  s32 cacheSize = 0;
  c8* pCache = ParserUtil::ReadFile( cacheFilePath, &cacheSize );

  // キャッシュが見つかったらバージョン確認
  if (pCache != NULL)
  {
    u32 gfbmotVersion = *(u32*)pCache;

    if (!AnimationResourceNode::IsGfbmotSupportedVersion(gfbmotVersion))
    {
      // 対応バージョンではないので使えない
      delete[] pCache;
      pCache = NULL;
    }
  }

  // キャッシュが無いので生成する
  if (pCache == NULL)
  {
    // 面倒なのでAnimationConverter.exeを呼び出すだけに変更
    AnimationConvert("gfmot", filePath, cacheFilePath);

    // 生成されたファイルを読み込み
    pCache = ParserUtil::ReadFile( cacheFilePath, &cacheSize );
  }

  // 読み込めたかチェック
  GFL_ASSERT(pCache != NULL);

  m_pData = pCache;
  m_DataSize = cacheSize;

  // メモリリーク対策
  s_cacheMemoryList.push_back(std::shared_ptr<c8>(m_pData));

  return true;
}

s32 GfMotData::Impl::GetSize( void )
{
  return 1;
}

const void* GfMotData::Impl::GetData( u32 index )
{
  (void)index;

	return m_pData;
}

u32 GfMotData::Impl::GetDataSize( u32 index )
{
  (void)index;

	return m_DataSize;
}

IResourceData::Type GfMotData::Impl::GetType( u32 index )
{
  (void)index;

  static const IResourceData::Type motType("animres");
  return motType;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfMotDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfMotData::GfMotData() : IResourceData()
{
	m_pImpl = new Impl();
}

GfMotData::GfMotData(::std::string filePath) : IResourceData()
{
	m_pImpl = new Impl();

  this->ReadFile(filePath);
}

GfMotData::~GfMotData()
{
	delete m_pImpl;
}

//! @brief ファイルからの読み込み（相対パス対応）
bool GfMotData::ReadFile(::std::string filePath)
{
  // 相対パスを絶対パスに変換
  char fullPath[1024];
  char* filePart;

  GetFullPathNameA(filePath.c_str(), sizeof(fullPath), fullPath, &filePart);
  filePath = fullPath;
  ParserUtil::ReplaceStr( filePath, "\\", "/" );

	s32 size = 0;
	c8* pData = ParserUtil::ReadFile( filePath.c_str(), &size );

  if (pData == NULL)
  {
    return false;
  }

	::std::string		fileDir = ParserUtil::GetFilePathStr(filePath.c_str());
	bool result = m_pImpl->ReadData(filePath, fileDir, pData);

	delete[] pData;

  return result;
}

//! @brief 全データの数の取得
s32 GfMotData::GetSize(void)
{
	return m_pImpl->GetSize();
}

//! @brief データの取得
const void* GfMotData::GetData(u32 index)
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfMotData::GetDataSize(u32 index)
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfMotData::GetType(u32 index)
{
	return m_pImpl->GetType(index);
}

}}}}


#endif
