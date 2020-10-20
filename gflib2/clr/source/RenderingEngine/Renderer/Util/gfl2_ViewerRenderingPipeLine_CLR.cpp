#include <clr/include/RenderingEngine/Renderer/Util/gfl2_ViewerRenderingPipeLine_CLR.h>
#include <clr/include/util/gfl2_String_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace renderer { namespace util {

ViewerRenderingPipeLine::ViewerRenderingPipeLine(
  System::String^ colorShaderPlaneFilePath,
  System::String^ colorShaderPlaneVertexShaderPath,
  System::String^ colorShaderPlanePixelShaderPath,
  System::String^ colorShaderPlaneTexturePath,
  System::String^ edgeCheckPlaneFilePath,
  System::String^ edgeCheckPlaneVertexShaderPath,
  System::String^ edgeCheckPlanePixelShaderPath,
  System::String^ edgeCheckPlaneTexturePath,
  System::String^ edgeGfmdlPath,
  array<System::String^>^ edgeShaderPaths,
  array<System::String^>^ edgeTexturePaths,
  System::String^ gfbmdlPathNormal,
  array<System::String^>^ shaderPathsNormal,
  System::String^ texturePathNormal,
  System::String^ gfbmdlPathId,
  array<System::String^>^ shaderPathsId,
  System::String^ texturePathId,
  System::String^ dofPlaneFilePath,
  System::String^ dofPlaneVertexShaderPath,
  System::String^ dofFrontPlanePixelShaderPath,
  System::String^ dofBackPlanePixelShaderPath,
  System::String^ dofPlaneTexturePath,
  u32 screenWidth,
  u32 screenHeight,
  gfl2::clr::gfx::Surface^ surface)
{
  std::string colorShaderPlaneFilePath_;
  colorShaderPlaneFilePath_ = gfl2::clr::util::String::GetNativeString(colorShaderPlaneFilePath);

  std::string colorShaderPlaneVertexShaderPath_;
  colorShaderPlaneVertexShaderPath_ = gfl2::clr::util::String::GetNativeString(colorShaderPlaneVertexShaderPath);

  std::string colorShaderPlanePixelShaderPath_;
  colorShaderPlanePixelShaderPath_ = gfl2::clr::util::String::GetNativeString(colorShaderPlanePixelShaderPath);

  std::string colorShaderPlaneTexturePath_;
  colorShaderPlaneTexturePath_ = gfl2::clr::util::String::GetNativeString(colorShaderPlaneTexturePath);

  std::string edgeCheckPlaneFilePath_;
  edgeCheckPlaneFilePath_ = gfl2::clr::util::String::GetNativeString(edgeCheckPlaneFilePath);

  std::string edgeCheckPlaneVertexShaderPath_;
  edgeCheckPlaneVertexShaderPath_ = gfl2::clr::util::String::GetNativeString(edgeCheckPlaneVertexShaderPath);

  std::string edgeCheckPlanePixelShaderPath_;
  edgeCheckPlanePixelShaderPath_ = gfl2::clr::util::String::GetNativeString(edgeCheckPlanePixelShaderPath);

  std::string edgeCheckPlaneTexturePath_;
  edgeCheckPlaneTexturePath_ = gfl2::clr::util::String::GetNativeString(edgeCheckPlaneTexturePath);

  std::string edgeGfmdlPath_;
  edgeGfmdlPath_ = gfl2::clr::util::String::GetNativeString(edgeGfmdlPath);

  const c8** edgeShaderPath_ = new const c8*[edgeShaderPaths->Length];
  std::string* edgeShaderPathString = new std::string[edgeShaderPaths->Length];
  for (s32 i = 0; i < edgeShaderPaths->Length; ++i)
  {
    edgeShaderPathString[i] = gfl2::clr::util::String::GetNativeString(edgeShaderPaths[i]);
    edgeShaderPath_[i] = edgeShaderPathString[i].c_str();
  }

  const c8** edgeTexturePath_ = new const c8*[edgeTexturePaths->Length];
  std::string* edgeTexturePathString = new std::string[edgeTexturePaths->Length];
  for (s32 i = 0; i < edgeTexturePaths->Length; ++i)
  {
    edgeTexturePathString[i] = gfl2::clr::util::String::GetNativeString(edgeTexturePaths[i]);
    edgeTexturePath_[i] = edgeTexturePathString[i].c_str();
  }

  std::string gfbmdlPathNormal_;
  gfbmdlPathNormal_ = gfl2::clr::util::String::GetNativeString(gfbmdlPathNormal).c_str();

  const c8** shaderPathsNormal_ = new const c8*[shaderPathsNormal->Length];
  std::string* shaderPathsNormalString = new std::string[shaderPathsNormal->Length];
  for (s32 i = 0; i < shaderPathsNormal->Length; ++i)
  {
    shaderPathsNormalString[i] = gfl2::clr::util::String::GetNativeString(shaderPathsNormal[i]);
    shaderPathsNormal_[i] = shaderPathsNormalString[i].c_str();
  }

  std::string texturePathNormal_;
  texturePathNormal_ = gfl2::clr::util::String::GetNativeString(texturePathNormal);

  std::string gfbmdlPathId_;
  gfbmdlPathId_ = gfl2::clr::util::String::GetNativeString(gfbmdlPathId);

  const c8** shaderPathsId_ = new const c8*[shaderPathsId->Length];
  std::string* shaderPathsIdString = new std::string[shaderPathsId->Length];
  for (s32 i = 0; i < shaderPathsId->Length; ++i)
  {
    shaderPathsIdString[i] = gfl2::clr::util::String::GetNativeString(shaderPathsId[i]);
    shaderPathsId_[i] = shaderPathsIdString[i].c_str();
  }

  std::string texturePathId_;
  texturePathId_ = gfl2::clr::util::String::GetNativeString(texturePathId).c_str();

  std::string dofPlaneFilePath_;
  dofPlaneFilePath_ = gfl2::clr::util::String::GetNativeString(dofPlaneFilePath);

  std::string dofPlaneVertexShaderPath_;
  dofPlaneVertexShaderPath_ = gfl2::clr::util::String::GetNativeString(dofPlaneVertexShaderPath);

  std::string dofFrontPlanePixelShaderPath_;
  dofFrontPlanePixelShaderPath_ = gfl2::clr::util::String::GetNativeString(dofFrontPlanePixelShaderPath);

  std::string dofBackPlanePixelShaderPath_;
  dofBackPlanePixelShaderPath_ = gfl2::clr::util::String::GetNativeString(dofBackPlanePixelShaderPath);

  std::string dofPlaneTexturePath_;
  dofPlaneTexturePath_ = gfl2::clr::util::String::GetNativeString(dofPlaneTexturePath);


  m_pNative = new gfl2::renderingengine::renderer::util::ViewerRenderingPipeLine(
    colorShaderPlaneFilePath_.c_str(),
    colorShaderPlaneVertexShaderPath_.c_str(),
    colorShaderPlanePixelShaderPath_.c_str(),
    colorShaderPlaneTexturePath_.c_str(),
    edgeCheckPlaneFilePath_.c_str(),
    edgeCheckPlaneVertexShaderPath_.c_str(),
    edgeCheckPlanePixelShaderPath_.c_str(),
    edgeCheckPlaneTexturePath_.c_str(),
    edgeGfmdlPath_.c_str(),
    edgeShaderPath_,
    edgeShaderPaths->Length,
    edgeTexturePath_,
    edgeTexturePaths->Length,
    gfbmdlPathNormal_.c_str(),
    shaderPathsNormal_,
    shaderPathsNormal->Length,
    texturePathNormal_.c_str(),
    gfbmdlPathId_.c_str(),
    shaderPathsId_,
    shaderPathsId->Length,
    texturePathId_.c_str(),
    dofPlaneFilePath_.c_str(),
    dofPlaneVertexShaderPath_.c_str(),
    dofFrontPlanePixelShaderPath_.c_str(),
    dofBackPlanePixelShaderPath_.c_str(),
    dofPlaneTexturePath_.c_str(),
    screenWidth,
    screenHeight,
    surface->GetNative());

  delete[] edgeShaderPath_;
  delete[] edgeTexturePath_;
  delete[] shaderPathsNormal_;
  delete[] shaderPathsId_;

  delete[] edgeShaderPathString;
  delete[] edgeTexturePathString;
  delete[] shaderPathsNormalString;
  delete[] shaderPathsIdString;
}

ViewerRenderingPipeLine::!ViewerRenderingPipeLine()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = NULL;
  }
}

void ViewerRenderingPipeLine::ChangeBackgroundVisibility(bool enable)
{
  m_pNative->ChangeBackgroundVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeModelVisibility(bool enable)
{
  m_pNative->ChangeModelVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeWireFrameVisibility(bool enable)
{
  m_pNative->ChangeWireFrameVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeJointVisibility(bool enable)
{
  m_pNative->ChangeJointVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeEffectLocatorVisibility(bool enable)
{
  m_pNative->ChangeEffectLocatorVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeGridVisibility(bool enable)
{
  m_pNative->ChangeGridVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeGaugeVisibility(bool enable)
{
  m_pNative->ChangeGaugeVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeInterestRangeVisibility(bool enable)
{
  m_pNative->ChangeInterestRangeVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeOutLineVisibility(bool enable)
{
  m_pNative->ChangeOutLineVisibility(enable);
}

void ViewerRenderingPipeLine::SetEnabledDof(bool enable)
{
  m_pNative->SetEnabledDof(enable);
}

void ViewerRenderingPipeLine::ChangeFillTexture(s32 index)
{
  m_pNative->ChangeFillTexture(static_cast<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Enum>(index));
}

void ViewerRenderingPipeLine::SetClearColor(clr::gfx::Color^ color)
{
  m_pNative->SetClearColor(*color->GetNative());
}

void ViewerRenderingPipeLine::AddEdgeRenderTarget(clr::poke_3d_lib::PokemonModel^ pModel)
{
  m_pNative->AddEdgeRenderingTarget(pModel->GetNative());
}

void ViewerRenderingPipeLine::RemoveEdgeRenderTarget(clr::poke_3d_lib::PokemonModel^ pModel)
{
  m_pNative->RemoveEdgeRenderingTarget(pModel->GetNative());
}

s32 ViewerRenderingPipeLine::GetCpuTimeAverage()
{
  return m_pNative->GetCpuTimeAverage();
}

s32 ViewerRenderingPipeLine::GetGpuTimeAverage()
{
  return m_pNative->GetGpuTimeAverage();
}

void ViewerRenderingPipeLine::SetModelPosition(gfl2::clr::math::Vector^ pos)
{
  m_pNative->SetModelPosition(pos->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetInnerTubeVisible(bool visible)
{
  m_pNative->SetInnerTubeVisible(visible);
}

void ViewerRenderingPipeLine::SetOuterTubeVisible(bool visible)
{
  m_pNative->SetOuterTubeVisible(visible);
}
void ViewerRenderingPipeLine::SetPokemonSize(gfl2::clr::renderingengine::PokemonSize::Enum size)
{
  m_pNative->SetPokemonSize(static_cast<gfl2::renderingengine::PokemonSize::Enum>(size));
}

void ViewerRenderingPipeLine::ChangeFillScreenType(gfl2::clr::renderingengine::FillScreenConstantType::Enum fillScreenType)
{
  m_pNative->ChangeFillScreenType(static_cast<gfl2::renderingengine::FillScreenConstantType::Enum>(fillScreenType));
}

void ViewerRenderingPipeLine::ChangeDofType(gfl2::clr::renderingengine::DofType::Enum dofType)
{
  m_pNative->ChangeDofType(static_cast<gfl2::renderingengine::DofType::Enum>(dofType));
}

void ViewerRenderingPipeLine::SetFocusPointVisible(bool visible)
{
  m_pNative->SetFocusPointVisible(visible);
}

void ViewerRenderingPipeLine::SetFocusPosition(gfl2::clr::math::Vector^ pos)
{
  m_pNative->SetFocusPosition(pos->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetInterestPositionVisible(bool visible)
{
  m_pNative->SetInterestPositionVisible(visible);
}

void ViewerRenderingPipeLine::SetInterestPosition(gfl2::clr::math::Vector^ pos)
{
  m_pNative->SetInterestPosition(pos->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetInnerTubeColor(gfl2::clr::math::Vector^ col)
{
  m_pNative->SetInnerTubeColor(col->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetOuterTubeColor(gfl2::clr::math::Vector^ col)
{
  m_pNative->SetOuterTubeColor(col->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetIsModelAABBVisible(bool visible)
{
  m_pNative->SetIsModelAABBVisible(visible);
}

void ViewerRenderingPipeLine::SetModelAABBColor(gfl2::clr::math::Vector^ col)
{
  m_pNative->SetModelAABBColor(col->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetLightPositionVisible(bool visible)
{
  m_pNative->SetLightPositionVisible(visible);
}

void ViewerRenderingPipeLine::SetLightPosition(gfl2::clr::math::Vector^ pos)
{
  m_pNative->SetLightPosition(pos->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetModelAABB(gfl2::clr::math::Vector^ min, gfl2::clr::math::Vector^ max)
{
  m_pNative->SetModelAABB(min->GetNativeOldVector(), max->GetNativeOldVector());
}

void ViewerRenderingPipeLine::SetColorShaderTextureColor(clr::poke_3d_lib::PokemonModel^ pokemonModel)
{
  s32 startIndex = pokemonModel->GetColorShaderPassStartIndex();
  u32 passNum = pokemonModel->GetColorTextureNum();
  gfl2::math::Vector4 color = pokemonModel->GetNative()->GetInterpColor();
  m_pNative->SetColorShaderTextureColor(startIndex, passNum, gfl2::math::Vector4(color.x, color.y, color.z, color.w));
}

void ViewerRenderingPipeLine::SetProjectionMatrix(clr::math::Matrix^ mat)
{
  m_pNative->SetProjectionMatrix(*(mat->GetNative()));
}

void ViewerRenderingPipeLine::SetViewMatrix(clr::math::Matrix^ mat)
{
  m_pNative->SetViewMatrix((mat->GetNativeMatrix34()));
}

void ViewerRenderingPipeLine::SetRenderTarget(clr::gfx::Surface^ pSurface)
{
  m_pNative->SetRenderTarget(pSurface->GetNative());
}

void ViewerRenderingPipeLine::SetDepthStencil(clr::gfx::Surface^ pSurface)
{
  m_pNative->SetDepthStencil(pSurface->GetNative());
}

void ViewerRenderingPipeLine::AddColorTexture(clr::poke_3d_lib::PokemonModel^ pokemonModel)
{
  poke_3d::model::PokemonModel* pNativePokemonModel = pokemonModel->GetNative();
  u32 colorShaderTextureNum = pNativePokemonModel->GetColorTextureNum();

  if (colorShaderTextureNum == 0)
  {// カラーテクスチャが1枚も無ければ終了
    return;
  }

  s32 startIndex = static_cast<s32>(m_pNative->GetColorShaderRenderPathNum());
  pokemonModel->SetColorShaderPassStartIndex(startIndex);
  for (u32 i = 0; i < colorShaderTextureNum; ++i)
  {
    const gfl2::gfx::Texture* pOriginalColorTexture = pNativePokemonModel->GetOriginalColorTextureResourceNode(i)->GetTexture();
    const gfl2::gfx::Texture* pColorShaderTexture = pNativePokemonModel->GetColorShaderTextureResourceNode(i)->GetTexture();
    m_pNative->AddColorShaderTexture(pOriginalColorTexture, pColorShaderTexture);
  }
}

void ViewerRenderingPipeLine::RemoveColorTexture(clr::poke_3d_lib::PokemonModel^ pokemonModel)
{
  poke_3d::model::PokemonModel* pNativePokemonModel = pokemonModel->GetNative();
  u32 colorShaderTextureNum = pNativePokemonModel->GetColorTextureNum();
  s32 startIndex = pokemonModel->GetColorShaderPassStartIndex();
  if (startIndex < 0)
  {// 一つもなければ削除できないので終了
    return;
  }

  for (u32 i = 0; i < colorShaderTextureNum; ++i)
  {
    m_pNative->RemoveColorShaderTexture(startIndex);
  }
}

void ViewerRenderingPipeLine::AddDrawableModel(clr::poke_3d_lib::PokemonModel^ pModel)
{
  if (pModel == nullptr)
  {
    m_pNative->AddDrawableModel(NULL);
    return;
  }

  m_pNative->AddDrawableModel(pModel->GetNative());
}

void ViewerRenderingPipeLine::RemoveDrawableModel(clr::poke_3d_lib::PokemonModel^ pModel)
{
  if (pModel == nullptr)
  {
    m_pNative->RemoveDrawableModel(NULL);
    return;
  }

  m_pNative->RemoveDrawableModel(pModel->GetNative());
}

void ViewerRenderingPipeLine::AddBackgroundModel(clr::poke_3d_lib::PokemonModel^ pModel)
{
  if (pModel == nullptr)
  {
    m_pNative->AddBackgroundModel(NULL);
    return;
  }

  m_pNative->AddBackgroundModel(pModel->GetNative());
}

void ViewerRenderingPipeLine::RemoveBackgroundModel(clr::poke_3d_lib::PokemonModel^ pModel)
{
  if (pModel == nullptr)
  {
    m_pNative->RemoveBackgroundModel(NULL);
    return;
  }

  m_pNative->RemoveBackgroundModel(pModel->GetNative());
}

void ViewerRenderingPipeLine::SetCameraDataToBackgroundPath(
  bool useCameraData,
  gfl2::clr::math::Matrix^ projection,
  gfl2::clr::math::Matrix^ view)
{
  m_pNative->SetCameraDataToBackgroundPath(
    useCameraData ? 1 : 0,
    *projection->GetNative(),
    view->GetNativeMatrix34());
}

void ViewerRenderingPipeLine::Update()
{
  m_pNative->Update();
}

void ViewerRenderingPipeLine::Execute()
{
  m_pNative->Execute();
}

}}}}}