#include <clr/include/poke_3d_lib/gfl2_PokemonModel_CLR.h>
// IDテクスチャ作成に必要
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <util/include/gfl2_FnvHash.h>
#include <clr/include/util/gfl2_String_CLR.h>
#include <clr/include/util/gfl2_Util_CLR.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_InstanceCreator_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryLocData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace poke_3d_lib {

bool PokemonModel::InitializeKeySound(System::String^ soundFilePath)
{
  std::string soundFilePath_ = gfl2::clr::util::String::GetNativeString(soundFilePath);

  wchar_t soundFilePath__[256];
  memset(soundFilePath__, 0, sizeof(wchar_t) * 256);
  //mbstowcs(soundFilePath__, soundFilePath_, soundFilePath->Length);
  size_t outSize = 0;
  mbstowcs_s(&outSize, soundFilePath__, soundFilePath->Length + 1, soundFilePath_.c_str(), _TRUNCATE);
  
  return poke_3d::model::PokemonModel::KeyAnimationSlotFunc::InitializeKeySound(soundFilePath__);
}

/**
 * @brief リソースファイルからのハッシュ値の生成
 * @param array<string^>^ ファイルパス
 * @return u32 ハッシュ値
 */
u32 PokemonModel::CreateHashFromResources(array<System::String^>^ filePaths)
{
  u32 value = gfl2::util::FnvHash::FNV_1_PRIME_32;
  for (s32 i = 0; i < filePaths->Length; ++i)
  {
    std::string filePath_ = gfl2::clr::util::String::GetNativeString(filePaths[i]);

    FILE *hFile;
    int file_size;
    fopen_s(&hFile, filePath_.c_str(), "rb");
    u8* read_buff;
    if (hFile == NULL)
    {
      continue;
    }

    fseek(hFile, 0, SEEK_END);
    file_size = ftell(hFile);
    fseek(hFile, 0, SEEK_SET);

    u32		align = 16;
    file_size += align;//必ずNULLが入るようにしたい。
    u32 buf_size = file_size + ((-file_size) & (align-1));

    gfl2::gfx::GLMemory::SetInstanceAllocator(gfl2::clr::gfx::GFGL::GetGLAllocator());
    read_buff = reinterpret_cast<u8*>(gfl2::gfx::GLMemory::Malloc(buf_size));
    memset(read_buff, 0, buf_size);
    fread(read_buff, file_size, 1, hFile);
    fclose(hFile);

    gfl2::util::FnvHash hash = gfl2::util::FnvHash(read_buff, buf_size, value);
    value = hash.value;
    gfl2::gfx::GLMemory::Free(read_buff);

    gfl2::gfx::GLMemory::ClearInstanceAllocator();
  }

  return value;
}

void PokemonModel::CreateTreeNodeColorDictionary()
{
  if (s_TreeNodeBackColorDictionary == nullptr)
  {
    s_TreeNodeBackColorDictionary = gcnew System::Collections::Generic::Dictionary<System::String^, System::Drawing::Color>();
    s_TreeNodeBackColorDictionary->Add("Joint", System::Drawing::Color::LightCyan);
    s_TreeNodeBackColorDictionary->Add("Locator", System::Drawing::Color::GreenYellow);
  }
}

void PokemonModel::TerminateKeySound()
{
  poke_3d::model::PokemonModel::KeyAnimationSlotFunc::TerminateKeySound();
}

PokemonModel::PokemonModel() :
  m_pNative(NULL),
  m_ColorShaderPassStartIndex(-1),
  m_IdTextureResourceNodes(NULL),
  m_IdTextureNum(0),
  m_RareColorTextureResourceNodes(NULL),
  m_RareColorTextureNum(0),
  m_pEffectLocatorResourceNode(NULL),
  m_EffectGfblocRowBinaryData(NULL),
    m_pPokedecoLocatorResourceNode(NULL),
  m_PokedecoGfblocRowBinaryData(NULL),
  m_pInterestPosition(NULL)
{
  m_pNative = new poke_3d::model::PokemonModel();

  s32 colorTextureNum = poke_3d::model::PokemonModel::COLORTEXTURE_MAX;
  // IDテクスチャ保存用の配列を作る(コンストラクタがprivateなのでmallocでメモリを取る)
  m_IdTextureResourceNodes = static_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode**>(
    malloc(sizeof(gfl2::renderingengine::scenegraph::resource::TextureResourceNode) * colorTextureNum));
  memset(m_IdTextureResourceNodes, 0, sizeof(gfl2::renderingengine::scenegraph::resource::TextureResourceNode) * colorTextureNum);
  for (s32 i = 0; i < poke_3d::model::PokemonModel::COLORTEXTURE_MAX; ++i)
  {
    m_IdTextureResourceNodes[i] = NULL;
  }

  // レアカラーテクスチャ保存用の配列を作る(コンストラクタがprivateなのでmallocでメモリを取る)
  m_RareColorTextureResourceNodes = static_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode**>(
    malloc(sizeof(gfl2::renderingengine::scenegraph::resource::TextureResourceNode) * colorTextureNum));
  memset(m_RareColorTextureResourceNodes, 0, sizeof(gfl2::renderingengine::scenegraph::resource::TextureResourceNode) * colorTextureNum);
  for (s32 i = 0; i < poke_3d::model::PokemonModel::COLORTEXTURE_MAX; ++i)
  {
    m_RareColorTextureResourceNodes[i] = NULL;
  }

  m_pManagedMotionResourceRootNodes = gcnew array<gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^>((s32)poke_3d::model::PokemonModel::MOTION_MAX);
  for (s32 i = 0; i < (s32)poke_3d::model::PokemonModel::MOTION_MAX; ++i)
  {
    m_pManagedMotionResourceRootNodes[i] = nullptr;
  }
}

PokemonModel::!PokemonModel()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = 0;
  }

  for (u32 i = 0; i < poke_3d::model::PokemonModel::COLORTEXTURE_MAX; ++i)
  {
    if (m_IdTextureResourceNodes[i])
    {
      delete m_IdTextureResourceNodes[i];
      m_IdTextureResourceNodes[i] = NULL;
    }
  }
  free(m_IdTextureResourceNodes);

  for (u32 i = 0; i < poke_3d::model::PokemonModel::COLORTEXTURE_MAX; ++i)
  {
    if (m_RareColorTextureResourceNodes[i])
    {
      delete m_RareColorTextureResourceNodes[i];
      m_RareColorTextureResourceNodes[i] = NULL;
    }
  }
  free(m_RareColorTextureResourceNodes);

  if (m_pEffectLocatorResourceNode)
  {
    delete m_pEffectLocatorResourceNode;
    m_pEffectLocatorResourceNode = NULL;
  }

  if (m_EffectGfblocRowBinaryData)
  {
    gfl2::gfx::GLMemory::Free(m_EffectGfblocRowBinaryData);
    m_EffectGfblocRowBinaryData = NULL;
  }

  if (m_pPokedecoLocatorResourceNode)
  {
    delete m_pPokedecoLocatorResourceNode;
    m_pPokedecoLocatorResourceNode = NULL;
  }

  if (m_PokedecoGfblocRowBinaryData)
  {
    gfl2::gfx::GLMemory::Free(m_PokedecoGfblocRowBinaryData);
    m_PokedecoGfblocRowBinaryData = NULL;
  }

  // 注視用の位置
  if (m_pInterestPosition)
  {
    delete m_pInterestPosition;
    m_pInterestPosition = NULL;
  }
}

static gfl2::heap::HeapBase* GetPokemonModelHeap()
{
  if (!gfl2::heap::Manager::IsExistHeapId(gfl2::heap::HEAPID_SYSTEM))
  {
    u32 bufSize = 0x6400000;
    void* pBuf = malloc(bufSize);
    gfl2::heap::Manager::Initialize(pBuf, bufSize, 1);
  }
  return gfl2::heap::Manager::GetDefaultNewUseHeap();
}

/**
 * @brief 作成
 *
 * @param pBaseModelResource ベースモデルリソース
 * @param 
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool PokemonModel::Create(
  gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pModelResource,
  gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pMotionConfigNode,
  PokemonModel^ pJointReferenceModel)
{
  static gfl2::heap::HeapBase* pPokemonModelHeap = GetPokemonModelHeap();
  poke_3d::model::PokemonModel* pNativeModel = NULL;
  if (pJointReferenceModel != nullptr)
  {
    pNativeModel = pJointReferenceModel->GetNative();
  }
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pNativeMotionConfigNode = NULL;
  if (pMotionConfigNode != nullptr)
  {
    pNativeMotionConfigNode = pMotionConfigNode->GetNative();
  }
  m_pNative->Create(
    gfl2::clr::gfx::GFGL::GetGLAllocator(),
    pPokemonModelHeap,
    pModelResource->GetNative(),
    pNativeModel,
    pNativeMotionConfigNode);

  // １フレ遅れ解放のチェックをやらない
  m_pNative->SetDestroyCheckEnable(false);

  return true;
}

/**
 * @brief 破棄
 */
void PokemonModel::Destroy()
{
  m_pNative->Destroy();
}

void PokemonModel::SetKeyCallBackInfo()
{
  m_pNative->SetKeyCallBackInfo();
}

//! @brief スケール設定
void PokemonModel::SetScale(gfl2::clr::math::Vector^ scale)
{
  this->m_pNative->SetScale(scale->GetNativeVector3());
}

//! @brief スケール取得
gfl2::clr::math::Vector^ PokemonModel::GetScale()
{
  return gcnew gfl2::clr::math::Vector(this->m_pNative->GetScale());
}

//! @brief 回転設定
void PokemonModel::SetRotateZYX(gfl2::clr::math::Vector^ rotate)
{
  this->m_pNative->SetRotation(rotate->GetNativeVector3());
}

//! @brief 回転取得
gfl2::clr::math::Vector^ PokemonModel::GetRotateZYX()
{
  return gcnew gfl2::clr::math::Vector(this->m_pNative->GetRotation());
}

//! @brief 位置設定
void PokemonModel::SetTranslate(gfl2::clr::math::Vector^ translate)
{
  this->m_pNative->SetPosition(translate->GetNativeVector3());
}

//! @brief 位置取得
gfl2::clr::math::Vector^ PokemonModel::GetTranslate()
{
  return gcnew gfl2::clr::math::Vector(this->m_pNative->GetPosition());
}

/**
 * @brief AABB取得
 */
 clr::math::AABB^ PokemonModel::GetModelAABB()
{
  return gcnew clr::math::AABB(this->m_pNative->GetModelAABB());
}

/**
 * @brief AABB設定
 */
 void PokemonModel::SetModelAABB(clr::math::AABB^ aabb)
{
  m_pNative->SetModelAABB(*aabb->GetNative());
}

/**
  * @brief 高さ取得
  *
  * @return モデルの高さ
  */
f32 PokemonModel::GetCmHeight()
{
  return m_pNative->GetCmHeight();
}

/**
  * @brief 高さ取得
  *
  * @return モデルの高さ
  */
f32 PokemonModel::GetAdjustHeight()
{
  return m_pNative->GetAdjustHeight();
}

/**
 * @brief 高さ設定
 *
 * @param cmHeight モデルの元の高さ
 * @param adjustHeight モデルの補正後の高さ
 */
void PokemonModel::SetHeight(f32 cmHeight, f32 adjustHeight)
{
  m_pNative->SetHeight(cmHeight, adjustHeight);
}

/**
 * @brief スケール値計算
 *
 * @return CmHeightとAdjustHeightから求めたスケール値
 */
f32 PokemonModel::CalculateAdjustScale()
{
  return m_pNative->CalculateAdjustScale();
}

/**
  * @brief MotionResourceNodeの取得
  *
  * @param index インデックス
  * @return ResourceNode
  */
clr::renderingengine::scenegraph::resource::ResourceNode^ PokemonModel::GetMotionResourceNode(s32 index)
{
  return m_pManagedMotionResourceRootNodes[index];
}

/**
  * @brief MotionResourceNodeの設定
  *
  * @param index インデックス
  * @param pResourceNode リソースノード
  */
void PokemonModel::SetMotionResourceNode(s32 index, clr::renderingengine::scenegraph::resource::ResourceNode^ pResourceNode)
{
  if (pResourceNode != nullptr)
	{
	  m_pNative->SetMotionResourceNode(index, pResourceNode->GetNative());
    m_pManagedMotionResourceRootNodes[index] = pResourceNode;
	}
	else
	{
	  m_pNative->SetMotionResourceNode(index, NULL);
    m_pManagedMotionResourceRootNodes[index] = nullptr;
	}
}

/**
  * @brief 設定されているモーション数の取得
  *
  * @return 設定されているモーション数
  */
s32 PokemonModel::GetMotionNum()
{
  return m_pNative->GetMotionNum();
}

/**
 * @brief JointInstanceNodeリストの取得
 *
 * @return JointInstanceNodeリストを返す
 */
array<clr::renderingengine::scenegraph::instance::JointInstanceNode^>^ PokemonModel::GetJointInstanceNodeList()
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* modelNode = m_pNative->GetModelInstanceNode();
  array<clr::renderingengine::scenegraph::instance::JointInstanceNode^>^ jointList = gcnew array<clr::renderingengine::scenegraph::instance::JointInstanceNode^>(modelNode->GetJointNum());

  for (u32 i = 0; i < modelNode->GetJointNum(); ++i)
  {
    jointList[i] = gcnew clr::renderingengine::scenegraph::instance::JointInstanceNode(modelNode->GetJointInstanceNode(i));
  }

  return jointList;
}

/**
 * @brief 現在のテクスチャをカラーシェーダ用テクスチャに差し替え
 * @param enable 有効/無効
 */
void PokemonModel::SetEnabledColorShaderTexture(bool enable)
{
  m_pNative->SetEnabledColorShaderTexture(enable);
}

/**
 * @brief カラーシェーダテクスチャの有効/無効の取得
 * @return b32 有効/無効
 */
bool PokemonModel::IsEnabledColorShaderTexture()
{
  return m_pNative->IsEnabledColorShaderTexture() ? true : false;
}

/**
 * @brief 環境光の設定の有効/無効の切り替え
 * @param enable 有効/無効
 */
void PokemonModel::SetEnabledAmbientColor(bool enable)
{
  m_pNative->SetEnabledAmbientColor(enable);
}

/**
 * @brief 環境光の設定の有効/無効の取得
 * @return b32 有効/無効
 */
bool PokemonModel::IsEnabledAmbientColor()
{
  return m_pNative->IsEnabledAmbientColor() ? true : false;
}

void PokemonModel::ReplaceWithRareColorTexture()
{
  u32 materialNum = m_pNative->GetModelInstanceNode()->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = m_pNative->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);

    u32 textureNum = pMaterialInstanceNode->GetTextureNum();

    for (u32 textureNo = 0; textureNo <= textureNum; ++textureNo)
    {
      const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode;
      pOriginalTextureResourceNode = pMaterialInstanceNode->GetTextureResourceNode(textureNo);

      if (!pOriginalTextureResourceNode)
      {
        continue;
      }

      const gfl2::gfx::Texture* pTexture = pOriginalTextureResourceNode->GetTexture();
      if (!pTexture)
      {
        continue;
      }

      // レアカラーテクスチャと名前が一致しているテクスチャを探す
      for (u32 rareColorTextureNo = 0; rareColorTextureNo < m_RareColorTextureNum; ++rareColorTextureNo)
      {
        const char* originalTextureName = pOriginalTextureResourceNode->GetName();
        const char* rareColorTextureName = m_RareColorTextureResourceNodes[rareColorTextureNo]->GetName();
        if (strcmp(originalTextureName, rareColorTextureName) != 0)
        {
          continue;
        }

        // レアカラーテクスチャとの差し替え(ノードを差し替えて実装)
        const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTexNode;
        pTexNode = m_RareColorTextureResourceNodes[rareColorTextureNo];
        pMaterialInstanceNode->SetTextureResourceNode(textureNo, pTexNode);
      }
    }
  }

  // PokemonModelクラスが持っているOriginalTextureを差し替え
  u32 originalColorTextureNum = m_pNative->GetColorTextureNum();
  for (u32 rareColorTextureNo = 0; rareColorTextureNo < m_RareColorTextureNum; ++rareColorTextureNo)
  {
    const char* rareColorTextureName = m_RareColorTextureResourceNodes[rareColorTextureNo]->GetName();
    for (u32 originalColorTextureNo = 0; originalColorTextureNo < originalColorTextureNum; ++originalColorTextureNo)
    {
      const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTexNode;
      pOriginalTexNode = m_pNative->GetOriginalColorTextureResourceNode(originalColorTextureNo);
      const char* originalColorTextureName = pOriginalTexNode->GetName();
      if (std::strcmp(rareColorTextureName, originalColorTextureName) != 0)
      {
        continue;
      }

      m_pNative->SetOriginalColorTextureResourceNode(
        originalColorTextureNo,
        m_RareColorTextureResourceNodes[rareColorTextureNo]);
    }
  }
}

/**
 * @brief レアカラーの定数カラー追加
 * @param info レアカラー変更用のデータ構造体
 */
void PokemonModel::AddRareConstantColor(RareColorConstantSettingInfo^ info)
{
  std::string materialName_ = gfl2::clr::util::String::GetNativeString(info->m_MaterialName);

  // 受け取ったマテリアル名から, マテリアルのインデックスを割り出す
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode;
  pModelInstanceNode = m_pNative->GetModelInstanceNode();
  s32 materialNum = (s32)pModelInstanceNode->GetMaterialNum();
  s32 materialNo = -1;
  for (s32 i = 0; i < materialNum; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode(i);

    const c8* materialName = pMaterialInstanceNode->GetName();

    if (std::strcmp(materialName, materialName_.c_str()) == 0)
    {
      materialNo = i;
      break;
    }
  }

  m_pNative->AddRareConstantColor(
    materialNo,
    info->m_ConstantColorIndex,
    gfl2::math::Vector4(info->m_Red, info->m_Green, info->m_Blue, info->m_Alpha));
}

/**
 * @brief レアカラー用の定数カラーに変更
 * @param info レアカラー変更用のデータ構造体
 */
void PokemonModel::ChangeToRareConstantColor()
{
  
}

/**
 * @brief IDテクスチャへの差し替え
 */
void PokemonModel::ReplaceWithIdTexture()
{
  // IDテクスチャと名前が一致しているテクスチャを探す
  for (u32 idTextureNo = 0; idTextureNo < m_IdTextureNum; ++idTextureNo)
  {
    m_pNative->ReplaceWithIdTextureResourceNode(m_IdTextureResourceNodes[idTextureNo]);
  }
}

/**
 * @brief カラーシェーダの色設定
 * @param color 設定する色
 */
void PokemonModel::SetInterpColor(clr::math::Vector^ color)
{
  m_pNative->SetInterpColor(*color->GetNative());
}

/**
 * @brief カラーシェーダの色取得
 * @return clr::math::Vector^ 設定されている色
 */
clr::math::Vector^ PokemonModel::GetInterpColor()
{
  return gcnew clr::math::Vector(m_pNative->GetInterpColor());
}

/**
 * @brief 環境光シェーダの色設定
 * @param color 設定する色
 */
void PokemonModel::SetAmbientColor(clr::math::Vector^ color)
{
  m_pNative->SetAmbientColor(*color->GetNative());
}

/**
 * @brief 環境光シェーダの色取得
 * @return clr::math::Vector^ 設定されている色
 */
clr::math::Vector^ PokemonModel::GetAmbientColor()
{
  return gcnew clr::math::Vector(m_pNative->GetAmbientColor());
}

/**
 * @brief カラーシェーダ描画パスのインデックス設定
 * @param index 開始インデックス
 */
void PokemonModel::SetColorShaderPassStartIndex(s32 index)
{
  assert(index >= 0);
  m_ColorShaderPassStartIndex = index;
}

/**
 * @brief カラーシェーダ描画パスのインデックス取得
 * @return 開始インデックス
 */
s32 PokemonModel::GetColorShaderPassStartIndex()
{
  return m_ColorShaderPassStartIndex;
}

/**
 * @brief カラーシェーダテクスチャの総数取得
 * @return カラーシェーダテクスチャの総数
 */
s32 PokemonModel::GetColorTextureNum()
{
  return m_pNative->GetColorTextureNum();
}

/**
 * @brief カラーシェーダテクスチャのインデックスを詰める
 * @param indexNum 詰めるインデックス数
 */
void PokemonModel::PackColorShaderTextureIndex(s32 indexNum)
{
  m_ColorShaderPassStartIndex -= indexNum;
  assert(m_ColorShaderPassStartIndex >= 0);
}

/**
 * @brief IDテクスチャの作成
 * @param texturePath IDテクスチャのパス
 */
void PokemonModel::CreateIdTexture(System::String^ texturePath)
{
  std::string texturePath_ = gfl2::clr::util::String::GetNativeString(texturePath);

  // パスからテクスチャ名のみを取り出す
  // さらにテクスチャ名の最後が"Id.tga"になっているので, ".tga"に置き換える
  System::String^ textureName = texturePath->Replace("Id.tga", ".tga");
  textureName = System::IO::Path::GetFileName(textureName);
  std::string textureName_ = gfl2::clr::util::String::GetNativeString(textureName);

  System::String^ idTextureName = System::IO::Path::GetFileName(texturePath);
  std::string idTextureName_ = gfl2::clr::util::String::GetNativeString(idTextureName);

  gfl2::gfx::ImageData imageData;
  imageData.ReadFromFile(texturePath_.c_str(), gfl2::gfx::ImageData::SwapType::OpenGL);
  gfl2::gfx::Texture* pTexture = 
    gfl2::gfx::GFGL::CreateTextureFromImage(gfl2::clr::gfx::GFGL::GetGLAllocator(), imageData, gfl2::gfx::Pool::Default);
  // テクスチャからリソースノードを作って, ModelInstanceNodeのテクスチャと差し替える
  gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
    gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
      gfl2::renderingengine::scenegraph::resource::IResourceData::Type("texture")
    );

  gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
    reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

  m_IdTextureResourceNodes[m_IdTextureNum] =  reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(
    pTextureResourceNodeFactory->CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), textureName_.c_str(), pTexture)
    );
  ++m_IdTextureNum;
}

/**
 * @brief レアカラーテクスチャの作成
 * @param texturePath レアカラーテクスチャのパス
 */
void PokemonModel::CreateRareColorTexture(System::String^ texturePath)
{
  std::string texturePath_ = gfl2::clr::util::String::GetNativeString(texturePath);

  // パスからテクスチャ名のみを取り出す
  System::String^ rareColorTextureName = System::IO::Path::GetFileName(texturePath);
  std::string rareColorTextureName_ = gfl2::clr::util::String::GetNativeString(rareColorTextureName);

  gfl2::gfx::ImageData imageData;
  imageData.ReadFromFile(texturePath_.c_str(), gfl2::gfx::ImageData::SwapType::OpenGL);
  gfl2::gfx::Texture* pTexture = 
  gfl2::gfx::GFGL::CreateTextureFromImage(gfl2::clr::gfx::GFGL::GetGLAllocator(), imageData, gfl2::gfx::Pool::Default);
  // テクスチャからリソースノードを作って, ModelInstanceNodeのテクスチャと差し替える
  gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
    gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
      gfl2::renderingengine::scenegraph::resource::IResourceData::Type("texture")
    );

  gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
    reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

  m_RareColorTextureResourceNodes[m_RareColorTextureNum] =  reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(
    pTextureResourceNodeFactory->CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), rareColorTextureName_.c_str(), pTexture)
    );
  ++m_RareColorTextureNum;
}

void PokemonModel::SetEnabledPlaySoundFlag(u32 index, bool enabled)
{
  m_pNative->SetEnabledPlaySoundFlag(index, enabled);
}

bool PokemonModel::GetKeySoundMatchFlag(u32 index)
{
  return m_pNative->GetKeySoundMatchFlag(index) ? true : false;
}

void PokemonModel::ClearKeySoundMatchFlags()
{
  m_pNative->ClearKeySoundMatchFlags();
}

bool PokemonModel::GetPlaySoundFlag(u32 index)
{
  return m_pNative->GetPlaySoundFlag(index) ? true : false;
}

bool PokemonModel::GetLoopAnimationFlag(u32 index)
{
  return m_pNative->GetLoopAnimationFlag(index) ? true : false;
}

void PokemonModel::ResetLoopAnimationFlag(u32 index)
{
  m_pNative->ResetLoopAnimationFlag(index);
}

b32* PokemonModel::GetPlaySoundFlagBuffer()
{
  return m_pNative->GetPlaySoundFlagBuffer();
}

/**
 * @brief 目, 口, ループモーションのモーションインデックスの設定
 * @param partsType パーツの種類
 * @param slotIndex 設定するスロットのインデックス
 * @param motionResourceIndex 表情用のモーションのリソースノードのインデックス(SetMotionResourceNode()で設定したインデックス)
 */
void PokemonModel::SetOverridePartsMotionIndex(s32 partsType, u32 slotIndex, s32 motionResourceIndex)
{
  m_pNative->SetOverridePartsMotionIndex(
    static_cast<gfl2::animation::OverridePartsType>(partsType),
    slotIndex,
    motionResourceIndex);
}

/**
 * @brief 目, 口, ループモーションのモーションインデックスの取得
 * @param partsType パーツの種類
 * @return s32 設定されているインデックス
 */
s32 PokemonModel::GetOverridePartsMotionIndex(s32 type)
{
  return m_pNative->GetOverridePartsMotionIndex(static_cast<gfl2::animation::OverridePartsType>(type));
}

/**
 * @brief info.dat出力
 * @param outputDirectory 出力先ディレクトリ
 * @param info 出力情報
 */
void PokemonModel::OutputInfoDat(
  System::String^ outputDirectory,
  gfl2::clr::poke_3d_lib::PokemonModel::PokemonModelInfo^ info)
{
  System::String^ infoDatPath = outputDirectory + "info.dat";
  std::string infoDatPath_ = gfl2::clr::util::String::GetNativeString(infoDatPath);

  poke_3d::model::PokemonModel::PokemonModelInfo info_;
  memset(&info_, 0, sizeof(info_));   // とりあえず0で初期化
  {// gfmdlFilePath
    std::string gfmdlFilePath_ = gfl2::clr::util::String::GetNativeString(info->m_GfmdlFilePath);
    strcpy_s(info_.m_GfmdlFilePath, gfmdlFilePath_.c_str());
  }

  info_.m_GfmotFileNum = 0;
  for (s32 i = 0; i < info->m_GfmotFilePaths->Length; ++i)
  {
    if (info->m_GfmotFilePaths[i] == nullptr)
    {
      std::memset(info_.m_GfmotFilePaths[i], 0, sizeof(info_.m_GfmotFilePaths[i]));
      continue;
    }
    std::string path_ = gfl2::clr::util::String::GetNativeString(info->m_GfmotFilePaths[i]);
    strcpy_s(info_.m_GfmotFilePaths[i], path_.c_str());
    ++(info_.m_GfmotFileNum);
  }

  info_.m_GfvshFileNum = 0;
  for (s32 i = 0; i < info->m_GfvshFilePaths->Length; ++i)
  {
    if (info->m_GfvshFilePaths[i] == nullptr)
    {
      std::memset(info_.m_GfvshFilePaths[i], 0, sizeof(info_.m_GfvshFilePaths[i]));
      continue;
    }
    std::string path_ = gfl2::clr::util::String::GetNativeString(info->m_GfvshFilePaths[i]);
    strcpy_s(info_.m_GfvshFilePaths[i], path_.c_str());
    ++(info_.m_GfvshFileNum);
  }

  info_.m_GfgshFileNum;
  for (s32 i = 0; i < info->m_GfgshFilePaths->Length; ++i)
  {
    if (info->m_GfgshFilePaths[i] == nullptr)
    {
      std::memset(info_.m_GfgshFilePaths[i], 0, sizeof(info_.m_GfgshFilePaths[i]));
      continue;
    }
    std::string path_ = gfl2::clr::util::String::GetNativeString(info->m_GfgshFilePaths[i]);
    strcpy_s(info_.m_GfgshFilePaths[i], path_.c_str());
    ++info_.m_GfgshFileNum;
  }

  info_.m_GffshFileNum = 0;
  for (s32 i = 0; i < info->m_GffshFilePaths->Length; ++i)
  {
    if (info->m_GffshFilePaths[i] == nullptr)
    {
      std::memset(info_.m_GffshFilePaths[i], 0, sizeof(info_.m_GffshFilePaths[i]));
      continue;
    }
    std::string path_ = gfl2::clr::util::String::GetNativeString(info->m_GffshFilePaths[i]);
    strcpy_s(info_.m_GffshFilePaths[i], path_.c_str());
    ++(info_.m_GffshFileNum);
  }

  info_.m_TextureFileNum = 0;
  for (s32 i = 0; i < info->m_TextureFilePaths->Length; ++i)
  {
    if (info->m_TextureFilePaths[i] == nullptr)
    {
      std::memset(info_.m_TextureFilePaths[i], 0, sizeof(info_.m_TextureFilePaths[i]));
      continue;
    }
    std::string path_ = gfl2::clr::util::String::GetNativeString(info->m_TextureFilePaths[i]);
    strcpy_s(info_.m_TextureFilePaths[i], path_.c_str());
    ++(info_.m_TextureFileNum);
  }

  for (s32 i = 0; i < info->m_LoopPlayingFrame->Length; ++i)
  {
    info_.m_LoopPlayingFrame[i] = info->m_LoopPlayingFrame[i];
  }

  // レアカラーのコンスタントカラー情報を設定
  info_.m_RareColorConstantSettingInfoNum = m_pNative->GetRareColorConstantSettingInfoNum();
  for (s32 i = 0; i < info_.m_RareColorConstantSettingInfoNum; ++i)
  {
    info_.m_RareColorConstantSettingInfos[i] = m_pNative->GetRareColorConstantSettingInfo(i);
  }

  // 必要なベクトル, マトリックスの情報を設定
  info_.m_AmbientColor = *info->m_AmbientColor->GetNative();
  info_.m_ColorShaderColor = *info->m_ColorShaderColor->GetNative();
  info_.m_IsAmbientColorEnabled = info->m_IsAmbientColorEnabled;
  info_.m_IsColorShaderEnabled = info->m_IsColorShaderEnabled;
  info_.m_LightVector = *info->m_LightVector->GetNative();
  info_.m_Rotation = *info->m_Rotation->GetNative();
  info_.m_Scale = *info->m_Scale->GetNative();
  info_.m_Translate = *info->m_Translate->GetNative();
  info_.m_CameraMatrix = *info->m_CameraMatrix->GetNative();
  info_.m_FieldOfView = info->m_FieldOfView;
  info_.m_NearClip = info->m_NearClip;
  info_.m_FarClip = info->m_FarClip;
  info_.m_IsPlayingAnimation = info->m_IsPlayingAnimation;
  info_.m_PlayingFrame = info->m_PlayingFrame;
  info_.m_StopMotionIndex = info->m_StopMotionIndex;
  info_.m_StopMotionFrame = info->m_StopMotionFrame;
  info_.m_StopPrevMotionIndex = info->m_StopPrevMotionIndex;
  info_.m_StopPrevMotionFrame = info->m_StopPrevMotionFrame;
  info_.m_StopInterpWeight = info->m_StopInterpWeight;
  info_.m_IsBackgroundVisible = info->m_IsBackgroundVisible;

  // アニメーションのフレームを設定
  for (s32 i = 0; i < info->m_MotionIndices->Length; ++i)
  {
    info_.m_MotionIndices[i] = info->m_MotionIndices[i];
    info_.m_MotionStartFrames[i] = info->m_MotionStartFrames[i];
    info_.m_MotionEndFrames[i] = info->m_MotionEndFrames[i];
    info_.m_MotionInterpFrames[i] = info->m_MotionInterpFrames[i];
  }

  // ループモーションのインデックスを設定
  for (s32 i = 0; i < gfl2::animation::OverridePartsType::NumberOf; ++i)
  {
    info_.m_OverrideMotionIndices[i] = info->m_OverrideMotionIndices[i];
  }

  // 模様設定(2016.01.26現在パッチールのみ)
  info_.m_MarkFrame1 = info->m_MarkFrame1;
  info_.m_MarkFrame2 = info->m_MarkFrame2;
  info_.m_MarkFrame3 = info->m_MarkFrame3;
  info_.m_MarkFrame4 = info->m_MarkFrame4;

  // ファイルに書き込んで出力
  FILE* fp = NULL;
  fopen_s(&fp, infoDatPath_.c_str(), "wb");
  fwrite(&info_, sizeof(info_), 1, fp);
  fclose(fp);
}

/**
 * @brief 操作対象を補間スロットに切り替える：ビューアー用
 *
 * @param isTypeInterp 操作対象が補間スロットか？
 */
void PokemonModel::SetAnimationControllTargetInterp(bool isTargetInterp)
{
  this->GetNative()->SetAnimationControllTargetInterp(isTargetInterp);
}

/**
 * @brief 歩き用速度を取得：ポケモン用の拡張データ
 *
 * @param prevFrame このフレームから
 * @param nextFrame このフレームまでの変化量を取得
 * @param interpWeight 補間率
 *
 * @return 移動速度ベクトル
 */
gfl2::clr::math::Vector^ PokemonModel::GetWalkSpeedManual(f32 stepFrame, f32 prevFrame, f32 nextFrame, f32 interpWeight)
{
  gfl2::math::Vector3 speed = this->GetNative()->GetWalkSpeedManual(stepFrame, prevFrame, nextFrame, interpWeight);
  return gcnew gfl2::clr::math::Vector(speed);
}

/**
 * @brief ライトセット番号の設定
 * @param lightSetNo ライトセット番号
 */
void PokemonModel::SetLightSetNo(u32 lightSetNo)
{
  m_pNative->SetLightSetNo(lightSetNo);
}

/**
 * @brief ライトの方向設定
 * @brief direction 方向ベクトル
 */
void PokemonModel::SetLightDirection(gfl2::clr::math::Vector^ direction)
{
  m_pNative->SetLightDirection(gfl2::math::Vector3(direction->x, direction->y, direction->z));
}

/**
 * @brief ライトの方向取得
 * @brief direction 方向ベクトル
 */
gfl2::clr::math::Vector^ PokemonModel::GetLightDirection()
{
  gfl2::math::Vector3 dir = m_pNative->GetLightDirection();
  return gcnew gfl2::clr::math::Vector(dir.x, dir.y, dir.z, 1.0f);
}

/**
 * @brief gfblocファイルからEfffectNodeを生成
 * @param gfblocFilePath ファイルパス
 * @return <gfl2::clr::renderingengine::scenegraph::instance::LocatorInstanceNode^>^ エフェクトロケータのノードの配列
 */
array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>^ PokemonModel::LoadEffectLocatorInstanceNodes(System::String^ gfblocFilePath)
{
  s32 size;
  std::string gfblocFilePath_ = gfl2::clr::util::String::GetNativeString(gfblocFilePath);
  m_EffectGfblocRowBinaryData = gfl2::clr::util::Util::ReadFile(gfblocFilePath_.c_str(), &size );

  gfl2::renderingengine::scenegraph::resource::GfBinaryLocData gfbLocData;
  gfbLocData.SetLocData(m_EffectGfblocRowBinaryData);

  m_pEffectLocatorResourceNode = 
    gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), &gfbLocData );

  m_pNative->AttachEffectLocatorInstanceNodeTopWithLocatorResourceNode(
    gfl2::gfx::GLMemory::GetAllocator(),
    m_pEffectLocatorResourceNode
    );

  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pEffectLocatorInstanceNodeTop;
  pEffectLocatorInstanceNodeTop = m_pNative->GetEffectLocatorInstanceNodeTop();

  u32 childCount = pEffectLocatorInstanceNodeTop->GetChildCount();
  array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>^ effectLocatorInstanceNodes;
  effectLocatorInstanceNodes = gcnew array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>(childCount + 1);
  
  // 親のノードをまず設定
  u32 index = 0;
  const c8* nodeName = pEffectLocatorInstanceNodeTop->GetNodeName();
  const c8* parentName = pEffectLocatorInstanceNodeTop->GetParentName();
  const gfl2::math::Vector3 translate = pEffectLocatorInstanceNodeTop->GetLocalTranslate();
  const gfl2::math::Quaternion rotation = pEffectLocatorInstanceNodeTop->GetLocalRotation();
  const gfl2::math::Vector3 scale = pEffectLocatorInstanceNodeTop->GetLocalScale();
  System::String^ nodeName_ = gcnew System::String(nodeName);
  System::String^ parentName_ = gcnew System::String(parentName);
  gfl2::clr::math::Vector^ translate_ = gcnew gfl2::clr::math::Vector(translate.x, translate.y, translate.z);
  gfl2::clr::math::Quaternion^ rotation_ = gcnew gfl2::clr::math::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w);
  gfl2::clr::math::Vector^ scale_ = gcnew gfl2::clr::math::Vector(scale.x, scale.y, scale.z);
  effectLocatorInstanceNodes[index] = gfl2::clr::renderingengine::scenegraph::instance::InstanceCreator::CreateViewerLocatorInstanceNode(nodeName_, parentName_);
  effectLocatorInstanceNodes[index]->SetTranslate(translate_);
  effectLocatorInstanceNodes[index]->SetRotation(rotation_);
  effectLocatorInstanceNodes[index]->SetScale(scale_);
  AttachLocatorNode(effectLocatorInstanceNodes[index]);
  ++index;

  for (u32 i = 0; i < childCount; ++i)
  {
    gfl2::renderingengine::scenegraph::DagNode* dagNode = pEffectLocatorInstanceNodeTop->GetChild(i);
    gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* locatorInstanceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode>();
    nodeName = locatorInstanceNode->GetNodeName();
    parentName = locatorInstanceNode->GetParentName();
    const gfl2::math::Vector3 trans = locatorInstanceNode->GetLocalTranslate();
    const gfl2::math::Quaternion rot = locatorInstanceNode->GetLocalRotation();
    const gfl2::math::Vector3 scl = locatorInstanceNode->GetLocalScale();
    nodeName_ = gcnew System::String(nodeName);
    parentName_ = gcnew System::String(parentName);
    gfl2::clr::math::Vector^ trans_ = gcnew gfl2::clr::math::Vector(trans.x, trans.y, trans.z);
    gfl2::clr::math::Quaternion^ rot_ = gcnew gfl2::clr::math::Quaternion(rot.x, rot.y, rot.z, rot.w);
    gfl2::clr::math::Vector^ scl_ = gcnew gfl2::clr::math::Vector(scl.x, scl.y, scl.z);
    effectLocatorInstanceNodes[index] = gfl2::clr::renderingengine::scenegraph::instance::InstanceCreator::CreateViewerLocatorInstanceNode(nodeName_, parentName_);
    effectLocatorInstanceNodes[index]->SetTranslate(trans_);
    effectLocatorInstanceNodes[index]->SetRotation(rot_);
    effectLocatorInstanceNodes[index]->SetScale(scl_);
    AttachLocatorNode(effectLocatorInstanceNodes[index]);
    ++index;
  }

  return effectLocatorInstanceNodes;
}

/**
 * @brief gfblocファイルからEfffectNodeを生成
 * @param gfblocFilePath ファイルパス
 * @return <gfl2::clr::renderingengine::scenegraph::instance::LocatorInstanceNode^>^ エフェクトロケータのノードの配列
 */
array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>^ PokemonModel::LoadPokedecoLocatorInstanceNodes(System::String^ gfblocFilePath)
{
  s32 size;
  std::string gfblocFilePath_ = gfl2::clr::util::String::GetNativeString(gfblocFilePath);
  m_PokedecoGfblocRowBinaryData = gfl2::clr::util::Util::ReadFile(gfblocFilePath_.c_str(), &size );

  gfl2::renderingengine::scenegraph::resource::GfBinaryLocData gfbLocData;
  gfbLocData.SetLocData(m_PokedecoGfblocRowBinaryData);

  m_pPokedecoLocatorResourceNode = 
    gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), &gfbLocData );

  m_pNative->AttachPokedecoLocatorInstanceNodeTopWithLocatorResourceNode(
    gfl2::gfx::GLMemory::GetAllocator(),
    m_pPokedecoLocatorResourceNode
    );

  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pPokedecoLocatorInstanceNodeTop;
  pPokedecoLocatorInstanceNodeTop = m_pNative->GetPokedecoLocatorInstanceNodeTop();

  u32 childCount = pPokedecoLocatorInstanceNodeTop->GetChildCount();
  array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>^ effectLocatorInstanceNodes;
  effectLocatorInstanceNodes = gcnew array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>(childCount + 1);
  
  u32 index = 0;

  const c8* nodeName = pPokedecoLocatorInstanceNodeTop->GetNodeName();
  const c8* parentName = pPokedecoLocatorInstanceNodeTop->GetParentName();
  const gfl2::math::Vector3 translate = pPokedecoLocatorInstanceNodeTop->GetLocalTranslate();
  System::String^ nodeName_ = gcnew System::String(nodeName);
  System::String^ parentName_ = gcnew System::String(parentName);
  gfl2::clr::math::Vector^ translate_ = gcnew gfl2::clr::math::Vector(translate.x, translate.y, translate.z);
  effectLocatorInstanceNodes[index] = gfl2::clr::renderingengine::scenegraph::instance::InstanceCreator::CreateViewerLocatorInstanceNode(nodeName_, parentName_);
  effectLocatorInstanceNodes[index]->SetTranslate(translate_);
  AttachLocatorNode(effectLocatorInstanceNodes[index]);
  ++index;

  for (u32 i = 0; i < childCount; ++i)
  {
    gfl2::renderingengine::scenegraph::DagNode* dagNode = pPokedecoLocatorInstanceNodeTop->GetChild(i);
    gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* locatorInstanceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode>();
    nodeName = locatorInstanceNode->GetNodeName();
    parentName = locatorInstanceNode->GetParentName();
    const gfl2::math::Vector3 trans = locatorInstanceNode->GetLocalTranslate();
    nodeName_ = gcnew System::String(nodeName);
    parentName_ = gcnew System::String(parentName);
    gfl2::clr::math::Vector^ trans_ = gcnew gfl2::clr::math::Vector(trans.x, trans.y, trans.z);
    effectLocatorInstanceNodes[index] = gfl2::clr::renderingengine::scenegraph::instance::InstanceCreator::CreateViewerLocatorInstanceNode(nodeName_, parentName_);
    effectLocatorInstanceNodes[index]->SetTranslate(trans_);
    AttachLocatorNode(effectLocatorInstanceNodes[index]);
    ++index;
  }

  return effectLocatorInstanceNodes;
}

/**
 * @brief レアカラーテクスチャの作成
 */
void PokemonModel::CreateColorShaderTextures()
{
  m_pNative->CreateColorShaderTextures(gfl2::clr::gfx::GFGL::GetGLAllocator());
}

// JointAndLocatorNodeVisitorに対してのVisitor
class JointAndLocatorNodeVisitor
{
public:
  JointAndLocatorNodeVisitor() : m_AvailableNodeCount(0)
  {
    Initialize();
  }
  virtual ~JointAndLocatorNodeVisitor() {}

  void Initialize()
  {
    m_NodeNameString = "";
  }

  void Visit( gfl2::renderingengine::scenegraph::instance::TransformNode* pNode )
  {
    class LocalFunc
    {
    public:
      static const c8* GetNodeName( gfl2::renderingengine::scenegraph::DagNode* pParentNode )
      {
        static const c8* parentName = "null";

        gfl2::renderingengine::scenegraph::instance::JointInstanceNode    *pParentJoint = pParentNode->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>();
        gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode  *pParentLocator = pParentNode->SafeCast<gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode>();
        gfl2::renderingengine::scenegraph::instance::ModelInstanceNode    *pParentModel = pParentNode->SafeCast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode>();

        if ( pParentJoint )
        {
          parentName = pParentJoint->GetNodeName();
        }
        else if ( pParentLocator )
        {
          parentName = pParentLocator->GetNodeName();
        }
        else if ( pParentModel )
        {
          parentName = pParentModel->GetNodeName();
        }

        return parentName;
      }
    };

    gfl2::renderingengine::scenegraph::instance::JointInstanceNode      *pJointNode = pNode->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>();
    gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode    *pLocatorNode = pNode->SafeCast<gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode>();

    if ( pJointNode )
    {// ジョイントの場合
      const c8* nodeName = pJointNode->GetNodeName();
      gfl2::renderingengine::scenegraph::DagNode* pParentNode = pJointNode->GetParent();
      const c8* parentName = "null";
      if ((pParentNode != NULL) && (m_AvailableNodeCount > 0))
      {
        parentName = LocalFunc::GetNodeName( pParentNode );
      }
      m_NodeNameString += "ParentName:";
      m_NodeNameString += parentName;
      m_NodeNameString += ";";
      m_NodeNameString += "NodeName:";
      m_NodeNameString += nodeName;
      m_NodeNameString += ";";
      m_NodeNameString += "Type:";
      m_NodeNameString += "Joint";
      m_NodeNameString += "|";
      ++m_AvailableNodeCount;
    }
    else if ( pLocatorNode )
    {// ロケータの場合
      const c8* nodeName = pLocatorNode->GetNodeName();
      gfl2::renderingengine::scenegraph::DagNode* pParentNode = pLocatorNode->GetParent();
      const c8* parentName = "null";
      if ((pParentNode != NULL) && (m_AvailableNodeCount > 0))
      {
        parentName = LocalFunc::GetNodeName( pParentNode );
      }
      m_NodeNameString += "ParentName:";
      m_NodeNameString += parentName;
      m_NodeNameString += ";";
      m_NodeNameString += "NodeName:";
      m_NodeNameString += nodeName;
      m_NodeNameString += ";";
      m_NodeNameString += "Type:";
      m_NodeNameString += "Locator";
      m_NodeNameString += "|";
      ++m_AvailableNodeCount;
    }
  }

  std::string m_NodeNameString;

private:
  u32 m_AvailableNodeCount;
};

/**
 * @brief ノード名を集めたTreeNodeの作成
 * @return System::Windows::Forms::TreeNode^ 作成したツリーノード
 */
System::Windows::Forms::TreeNode^ PokemonModel::CreateModelNameTreeNode()
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = m_pNative->GetModelInstanceNode();
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::instance::TransformNode, JointAndLocatorNodeVisitor> visitor;
  visitor.Initialize();
  gfl2::renderingengine::scenegraph::Traverser::Traverse<gfl2::renderingengine::scenegraph::instance::TransformNode, JointAndLocatorNodeVisitor>(pModelInstanceNode, &visitor);
  
  System::String^ nodeNameString = gcnew System::String(visitor.m_NodeNameString.c_str());
  if (System::String::IsNullOrEmpty(nodeNameString))
  {
    m_RootTreeNode = gcnew System::Windows::Forms::TreeNode(nodeNameString);
    return m_RootTreeNode;
  }
  nodeNameString = nodeNameString->Remove(nodeNameString->Length - 1);

  array<System::String^>^ nodeNameArray = nodeNameString->Split('|');

  array<System::String^>^ nodeNames = nodeNameArray[0]->Split(';');
  System::String^ parentName = nodeNames[0]->Split(':')[1];
  System::String^ nodeName = nodeNames[1]->Split(':')[1];
  System::String^ type = nodeNames[2]->Split(':')[1];
  m_RootTreeNode = gcnew System::Windows::Forms::TreeNode(nodeName);

  for (s32 i = 1; i < nodeNameArray->Length; ++i)
  {
    nodeNames = nodeNameArray[i]->Split(';');
    parentName = nodeNames[0]->Split(':')[1];
    nodeName = nodeNames[1]->Split(':')[1];
    type = nodeNames[2]->Split(':')[1];
    System::Windows::Forms::TreeNode^ parentTreeNode = FindTreeNodeByName(m_RootTreeNode, parentName);
    if (parentTreeNode == nullptr)
    {
      continue;
    }

    System::Windows::Forms::TreeNode^ node = gcnew System::Windows::Forms::TreeNode(nodeName);
    node->BackColor = s_TreeNodeBackColorDictionary[type];
    parentTreeNode->Nodes->Add(node);
  }

  return m_RootTreeNode;
}

System::Windows::Forms::TreeNode^ PokemonModel::FindTreeNodeByName(
  System::Windows::Forms::TreeNode^ treeNode,
  System::String^ name)
{
  if (treeNode->Text == name)
  {
    return treeNode;
  }

  for (s32 i = 0; i < treeNode->Nodes->Count; ++i)
  {
    System::Windows::Forms::TreeNode^ node = PokemonModel::FindTreeNodeByName(treeNode->Nodes[i], name);
    if (node != nullptr)
    {
      return node;
    }
  }

  return nullptr;
}

/**
 * @brief ツリーノードの取得
 * return System::Windows::Forms::TreeNode^ 作成したツリーノード
 */
System::Windows::Forms::TreeNode^ PokemonModel::GetModelNameTreeNode()
{
  return m_RootTreeNode;
}

/**
 * @brief 注視処理更新： ※シーンのトラバース後に呼んでください
 */
void PokemonModel::UpdateInterest()
{
  if (m_pInterestPosition == NULL)
  {
    return;
  }

  m_pNative->UpdateInterest();
}

/**
 * @brief 注視位置設定
 *
 * @param pPosition 注視位置：NULLなら終了
 */
void PokemonModel::SetInterestPosition(gfl2::clr::math::Vector^ pPosition)
{
  if (pPosition == nullptr)
  {
    if (m_pInterestPosition)
    {
      delete m_pInterestPosition;
      m_pInterestPosition = NULL;
    }

    m_pNative->SetInterestPosition(NULL);
    return;
  }

  if (m_pInterestPosition == NULL)
  {
    m_pInterestPosition = new gfl2::math::Vector3();
  }
  *m_pInterestPosition = pPosition->GetNativeVector3();
  m_pNative->SetInterestPosition(m_pInterestPosition);
}

/**
 * @brief 注目タイプ設定
 *
 * @param index インデックス
 * @param type 注目タイプ
 */
void PokemonModel::SetInterestType(u32 index, gfl2::clr::renderingengine::InterestType::Enum type)
{
  m_pNative->SetInterestType(index, static_cast<gfl2::renderingengine::InterestType::Enum>(type));
}

/**
 * @brief 注目タイプ取得
 *
 * @param index インデックス
 *
 * @return 注目タイプを返す
 */
gfl2::renderingengine::InterestType PokemonModel::GetInterestType(u32 index)
{
  return m_pNative->GetInterestType(index);
}

/**
 * @brief 注目速度設定
 *
 * @param index インデックス
 * @param speed 注目速度
 */
void PokemonModel::SetInterestSpeed(u32 index, f32 speed)
{
  m_pNative->SetInterestSpeed(index, speed);
}

/**
 * @brief 注目速度取得
 *
 * @param index インデックス
 *
 * @return 注目速度を返す
 */
f32 PokemonModel::GetInterestSpeed(u32 index)
{
  return m_pNative->GetInterestSpeed(index);
}

/**
 * @brief 角度制限：最小値設定（ラジアン）
 *
 * @param index インデックス
 * @param low 角度制限：最小値
 */
void PokemonModel::SetInterestLimitAngleLow(u32 index, f32 low)
{
  m_pNative->SetInterestLimitAngleLow(index, low);
}

/**
 * @brief 角度制限：最小値取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最小値を返す
 */
f32 PokemonModel::GetInterestLimitAngleLow(u32 index)
{
  return m_pNative->GetInterestLimitAngleLow(index);
}

/**
 * @brief 角度制限：最大値設定（ラジアン）
 *
 * @param index インデックス
 * @param high 角度制限：最大値
 */
void PokemonModel::SetInterestLimitAngleHigh(u32 index, f32 high)
{
  m_pNative->SetInterestLimitAngleHigh(index, high);
}

/**
 * @brief 角度制限：最大値取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最大値を返す
 */
f32 PokemonModel::GetInterestLimitAngleHigh(u32 index)
{
  return m_pNative->GetInterestLimitAngleHigh(index);
}

/**
 * @brief 角度制限：最小値２設定（ラジアン）
 *
 * @param index インデックス
 * @param low 角度制限：最小値
 */
void PokemonModel::SetInterestLimitAngleLow2(u32 index, f32 low)
{
  m_pNative->SetInterestLimitAngleLow2(index, low);
}

/**
 * @brief 角度制限：最小値２取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最小値２を返す
 */
f32 PokemonModel::GetInterestLimitAngleLow2(u32 index)
{
  return m_pNative->GetInterestLimitAngleLow2(index);
}

/**
 * @brief 角度制限：最大値２設定（ラジアン）
 *
 * @param index インデックス
 * @param high 角度制限：最大値
 */
void PokemonModel::SetInterestLimitAngleHigh2(u32 index, f32 high)
{
  m_pNative->SetInterestLimitAngleHigh2(index, high);
}

/**
 * @brief 角度制限：最大値２取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最大値２を返す
 */
f32 PokemonModel::GetInterestLimitAngleHigh2(u32 index)
{
  return m_pNative->GetInterestLimitAngleHigh2(index);
}

/**
 * @brief 注目解除にかかるフレーム数を設定
 *
 * @param index インデックス
 * @param resetFrameCount 注目解除にかかるフレーム数
 */
void PokemonModel::SetInterestResetFrameCount(u32 index, s32 resetFrameCount)
{
  m_pNative->SetInterestResetFrameCount(index, resetFrameCount);
}

/**
 * @brief 注目解除にかかるフレーム数を取得
 *
 * @param index インデックス
 *
 * @return 注目解除にかかるフレーム数を返す
 */
s32 PokemonModel::GetInterestResetFrameCount(u32 index)
{
  return m_pNative->GetInterestResetFrameCount(index);
}

void PokemonModel::UpdateSpring()
{
  m_pNative->UpdateSpring();
}

void PokemonModel::ResetMotionConfig(gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pMotionConfigNode)
{
  m_pNative->InitializeInterest(gfl2::clr::gfx::GFGL::GetGLAllocator(), pMotionConfigNode->GetNative());
  m_pNative->InitializeSpring(GetPokemonModelHeap());
}

/**
 * @brief PokemonModelMaterialAttributeを生成
 *
 * @return モデルが持っている全てのマテリアルの情報をマテリアル名をキーとした連想配列で返す
 */
Dictionary<System::String^, PokemonModelMaterialAttribute^>^ PokemonModel::CreatePokemonModelMaterialAttributes()
{
  array<System::String^>^ cullModeNames = gcnew array<System::String^>(static_cast<s32>(gfl2::gfx::CullMode::NumberOf));
  cullModeNames[static_cast<s32>(gfl2::gfx::CullMode::None)] = "None";
  cullModeNames[static_cast<s32>(gfl2::gfx::CullMode::CW)] = "CW";
  cullModeNames[static_cast<s32>(gfl2::gfx::CullMode::CCW)] = "CCW";
  u32 materialNum = m_pNative->GetMaterialCount();

  Dictionary<System::String^, PokemonModelMaterialAttribute^>^ pokemonModelMaterialAttributes = gcnew Dictionary<System::String^, PokemonModelMaterialAttribute^>();
  for (u32 i = 0; i < materialNum; ++i)
  {
    const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = m_pNative->GetModelInstanceNode()->GetMaterialInstanceNode(i);
    const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam;
    pAttributeParam = pMaterialInstanceNode->GetAttributeParam();
    const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode* pMaterialResourceNode;
    pMaterialResourceNode = pMaterialInstanceNode->GetMaterialResourceNode();
    const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::UserData &rUserData = pMaterialResourceNode->GetUserData();
    const c8* materialName_ = pMaterialResourceNode->GetName();
    System::String^ materialName = gcnew System::String(materialName_);

    PokemonModelMaterialAttribute^ pokemonModelMaterialAttribute = gcnew PokemonModelMaterialAttribute();
    pokemonModelMaterialAttribute->m_IdEdgeNo = rUserData.m_EdgeID;
    pokemonModelMaterialAttribute->m_LightSetNo = pAttributeParam->m_LightSetNo;
    pokemonModelMaterialAttribute->m_CullModeName = cullModeNames[static_cast<s32>(pAttributeParam->m_CullMode)];
    pokemonModelMaterialAttributes->Add(materialName, pokemonModelMaterialAttribute);
  }

  return pokemonModelMaterialAttributes;
}

/**
 * @brief 模様を固定するフレーム数の指定
 * @param u8 markingFrame1 Loop01の固定フレーム
 * @param u8 markingFrame2 Loop02の固定フレーム
 * @param u8 markingFrame3 Loop03の固定フレーム
 * @param u8 markingFrame4 Loop04の固定フレーム
 */
void PokemonModel::SetMarkingFrames(u8 markingFrame1, u8 markingFrame2, u8 markingFrame3, u8 markingFrame4)
{
  m_pNative->SetMarkingFrames(markingFrame1, markingFrame2, markingFrame3, markingFrame4);
}

/**
 * @brief animationBitで指定したアニメーションがループモーションに含まれているかどうかの確認
 * @param loopIndex ループモーションのインデックス(0～3)
 * @return true/false ジョイントアニメーションを持っている/持っていない
 */
bool PokemonModel::HasJointAnimationInLoopMotion(u32 loopIndex)
{
  return m_pNative->HasJointAnimationInLoopMotion(loopIndex) ? true : false;
}

System::String^ PokemonModel::GetMaterialName(s32 materialIndex)
{
  const char* pMaterialName = m_pNative->GetModelInstanceNode()->GetMaterialInstanceNode(materialIndex)->GetName();
  return gcnew System::String(pMaterialName);
}

s32 PokemonModel::GetMaterialIndex(System::String^ materialName)
{
  for (u32 i = 0; i < this->GetMaterialCount(); ++i)
  {
    if (materialName == this->GetMaterialName(i))
    {
      return i;
    }
  }

  return -1;
}

gfl2::clr::math::Vector^ PokemonModel::GetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 slotNo)
{
  gfl2::math::Vector2 scale = m_pNative->GetMaterialTextureCoordinatorScaleUV(materialIndex, slotNo);
  return gcnew gfl2::clr::math::Vector(scale.x, scale.y , 0);
}

void PokemonModel::SetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 slotNo, gfl2::clr::math::Vector^ scale)
{
  gfl2::math::Vector2 scale_native(scale->x, scale->y);
  m_pNative->SetMaterialTextureCoordinatorScaleUV(materialIndex, slotNo, scale_native);
}

f32 PokemonModel::GetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 slotNo)
{
  return m_pNative->GetMaterialTextureCoordinatorRotateUV(materialIndex, slotNo);
}

void PokemonModel::SetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 slotNo, f32 rotate)
{
  m_pNative->SetMaterialTextureCoordinatorRotateUV(materialIndex, slotNo, rotate);
}

gfl2::clr::math::Vector^ PokemonModel::GetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 slotNo)
{
  gfl2::math::Vector2 translate = m_pNative->GetMaterialTextureCoordinatorTranslateUV(materialIndex, slotNo);
  return gcnew gfl2::clr::math::Vector(translate.x, translate.y , 0);
}

void PokemonModel::SetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 slotNo, gfl2::clr::math::Vector^ translate)
{
  gfl2::math::Vector2 translate_native(translate->x, translate->y);
  m_pNative->SetMaterialTextureCoordinatorTranslateUV(materialIndex, slotNo, translate_native);
}

}}}