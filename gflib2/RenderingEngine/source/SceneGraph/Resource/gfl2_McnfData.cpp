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
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_McnfData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNode.h>
#include <debug/include/gfl2_Assert.h>

#include <Windows.h>

using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

extern std::string GetAnimationConverterVersion();
extern void AnimationConvert(const std::string& mode, const std::string& inputPath, const std::string& outputPath);

static std::vector<std::shared_ptr<c8>> s_cacheMemoryList;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  McnfData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class McnfData::Impl : public gfx::GLMemory, public rapidxml::xml_document<>
{
public:

	Impl();
	virtual ~Impl();

  bool ReadData(const std::string& filePath, const std::string& fileDir, c8* pData);

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );

  c8* m_pData;
  u32 m_DataSize;
};

McnfData::Impl::Impl() : gfx::GLMemory(),
  m_pData(NULL),
  m_DataSize(0)
{
}

McnfData::Impl::~Impl()
{

}

bool McnfData::Impl::ReadData(const std::string& filePath, const std::string& fileDir, c8* pData)
{
  static std::string s_converterVersion = GetAnimationConverterVersion();

  util::FnvHash hash( reinterpret_cast<u8*>(pData) );
  char cacheFilePath[ParserUtil::GFL_MAX_FILEPATH];	
  memset(cacheFilePath, 0, sizeof(cacheFilePath));
  sprintf_s( cacheFilePath, "%sAnimationCache/%s.%d.cache", fileDir.c_str(), s_converterVersion.c_str(), hash.value );	

  s32 cacheSize = 0;
  c8* pCache = ParserUtil::ReadFile( cacheFilePath, &cacheSize );

  // キャッシュが見つかったらバージョン確認
  if (pCache != NULL)
  {
    u32 bmcnfVersion = *(u32*)pCache;

    if (!MotionConfigResourceNode::IsBmcnfSupportedVersion(bmcnfVersion))
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
    AnimationConvert("mcnf", filePath, cacheFilePath);

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

s32 McnfData::Impl::GetSize( void )
{
	return 1;
}

const void* McnfData::Impl::GetData( u32 index )
{
  (void)index;

	return m_pData;
}

u32 McnfData::Impl::GetDataSize( u32 index )
{
  (void)index;

	return m_DataSize;
}

IResourceData::Type McnfData::Impl::GetType( u32 index )
{
  (void)index;

  return IResourceData::Type("mcnf");
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfMotDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
McnfData::McnfData() : IResourceData()
{
	m_pImpl = new Impl();
}

McnfData::McnfData(std::string filePath) : IResourceData()
{
	m_pImpl = new Impl();

  this->ReadFile(filePath);
}

McnfData::~McnfData()
{
	delete m_pImpl;
}

//! @brief ファイルからの読み込み（相対パス対応）
bool McnfData::ReadFile( std::string filePath )
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

	std::string		fileDir = ParserUtil::GetFilePathStr(filePath.c_str());
	bool result = m_pImpl->ReadData(filePath, fileDir, pData);

	delete[] pData;

  return result;
}

//! @brief 全データの数の取得
s32 McnfData::GetSize( void )
{
	return m_pImpl->GetSize();
}

//! @brief データの取得
const void* McnfData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 McnfData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type McnfData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}

}}}}


#endif