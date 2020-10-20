#include <model/include/gfl2_PokemonModel.h>
#include <math/include/gfl2_EulerRotation.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
// カラーシェーダ用テクスチャ作成に必要
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
// ライトノードの作成に必要
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <debug/include/gfl2_Assert.h>

// バネ制御フラグ
#define POKEMON_MODEL_SPRING_FLAG 1

namespace poke_3d { namespace model {

const c8* PokemonModel::KeyAnimationSlotFunc::s_pKeyAttributeNames[poke_3d::model::PokemonModel::KeySoundAttribute::NumberOf] =
{
  "AK_EffectStart01",
  "AK_ButuriStart01",
  "AK_SEStart01",
  "AK_SEStart02",
  "AK_SEStart03",
  "AK_SEStart04",
  "AK_PartsSkel01",
  "AK_PartsMaterial01",
  "AK_PartsVisible01",
  "AK_PartsShader01",
};

//!< @brief ループモーションの種類
const BaseModel::AnimationBit PokemonModel::s_LoopAnimationBit[poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM] =
{
  BaseModel::ANIMATION_BIT_JOINT,
  BaseModel::ANIMATION_BIT_MATERIAL,
  BaseModel::ANIMATION_BIT_VISIBILITY
};

#if defined(GF_PLATFORM_WIN32)
LPDIRECTSOUND8 PokemonModel::KeyAnimationSlotFunc::lpDS = NULL;
LPDIRECTSOUNDBUFFER PokemonModel::KeyAnimationSlotFunc::lpPrimary = NULL;
LPDIRECTSOUNDBUFFER PokemonModel::KeyAnimationSlotFunc::lpSecondary = NULL;

bool PokemonModel::KeyAnimationSlotFunc::InitializeKeySound(LPWSTR soundFilePath)
{
  HRESULT ret;
  WAVEFORMATEX wf;

  ret = DirectSoundCreate8(NULL, &lpDS, NULL);
  if ( FAILED(ret) )
  {
    return false;
  }
  ret = lpDS->SetCooperativeLevel(gfl2::gfx::GFGL::GetHwnd(), DSSCL_EXCLUSIVE | DSSCL_PRIORITY);
  if ( FAILED(ret) )
  {
    return false;
  }

  // プライマリサウンドバッファの作成
  DSBUFFERDESC dsdesc;
  ZeroMemory( &dsdesc,sizeof(DSBUFFERDESC) );
  dsdesc.dwSize = sizeof( DSBUFFERDESC );
  dsdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
  dsdesc.dwBufferBytes = 0;
  dsdesc.lpwfxFormat = NULL;
  ret = lpDS->CreateSoundBuffer(&dsdesc, &lpPrimary, NULL);
  if( FAILED(ret) )
  {
      return false;
  }

  // プライマリバッファのステータスを決定
  wf.cbSize = sizeof( WAVEFORMATEX );
  wf.wFormatTag = WAVE_FORMAT_PCM;
  wf.nChannels = 1;           // 1(モノラル), 2(ステレオ)
  wf.nSamplesPerSec = 44100;  // 周波数
  wf.wBitsPerSample = 8;      // 量子化ビット数
  wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
  wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
  ret = lpPrimary->SetFormat(&wf);
  if( FAILED(ret) )
  {
      return false;
  }

  // セカンダリバッファの作成
  CreateSoundBuffer(&lpSecondary, soundFilePath);

  return true;
}

void PokemonModel::KeyAnimationSlotFunc::TerminateKeySound()
{
  if (lpSecondary)
  {
    lpSecondary->Release();
    lpSecondary = NULL;
  }

  if (lpPrimary)
  {
    lpPrimary->Release();
    lpPrimary = NULL;
  }

  if (lpDS)
  {
    lpDS->Release();
    lpDS = NULL;
  }
}

bool PokemonModel::KeyAnimationSlotFunc::CreateSoundBuffer(LPDIRECTSOUNDBUFFER* dsb, LPWSTR file)
{
  HRESULT ret;
  MMCKINFO mSrcWaveFile;
  MMCKINFO mSrcWaveFmt;
  MMCKINFO mSrcWaveData;
  LPWAVEFORMATEX wf;

  // WAVファイルをロード
  HMMIO hSrc;
  hSrc = mmioOpen( file, NULL, MMIO_ALLOCBUF|MMIO_READ|MMIO_COMPAT );
  if( !hSrc )
  {
    return false;
  }

  // 'WAVE'チャンクチェック
  ZeroMemory( &mSrcWaveFile,sizeof(mSrcWaveFile) );
  ret = mmioDescend( hSrc,&mSrcWaveFile,NULL,MMIO_FINDRIFF );
  if( mSrcWaveFile.fccType!=mmioFOURCC('W','A','V','E') )
  {
    mmioClose( hSrc,0 );
    return false;
  }

  // 'fmt 'チャンクチェック
  ZeroMemory( &mSrcWaveFmt,sizeof(mSrcWaveFmt) );
  ret = mmioDescend( hSrc,&mSrcWaveFmt,&mSrcWaveFile,MMIO_FINDCHUNK );
  if( mSrcWaveFmt.ckid!=mmioFOURCC('f','m','t',' ') )
  {
    mmioClose( hSrc,0 );
    return false;
  }

  // ヘッダサイズの計算
  int iSrcHeaderSize = mSrcWaveFmt.cksize;
  if( iSrcHeaderSize<sizeof(WAVEFORMATEX) )
  {
      iSrcHeaderSize=sizeof(WAVEFORMATEX);
  }

  // ヘッダメモリ確保
  wf = (LPWAVEFORMATEX)malloc( iSrcHeaderSize );
  if( !wf )
  {
    mmioClose( hSrc,0 );
    return false;
  }

  ZeroMemory( wf,iSrcHeaderSize );

  // WAVEフォーマットのロード
  ret = mmioRead( hSrc,(char*)wf,mSrcWaveFmt.cksize );
  if( FAILED(ret) )
  {
    free( wf );
    mmioClose( hSrc,0 );
    return false;
  }

  // fmtチャンクに戻る
  mmioAscend( hSrc,&mSrcWaveFmt,0 );

  // dataチャンクを探す
  for( ; ; )
  {
    // 検索
    ret = mmioDescend( hSrc,&mSrcWaveData,&mSrcWaveFile,0 );
    if( FAILED(ret) )
    {
      free( wf );
      mmioClose( hSrc,0 );
      return false;
    }

    if( mSrcWaveData.ckid==mmioStringToFOURCC(L"data", 0) )
    {
      break;
    }

    // 次のチャンクへ
    ret = mmioAscend( hSrc,&mSrcWaveData,0 );
  }


  // サウンドバッファの作成
  DSBUFFERDESC dsdesc;
  ZeroMemory( &dsdesc,sizeof(DSBUFFERDESC) );
  dsdesc.dwSize = sizeof( DSBUFFERDESC );
  dsdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC | DSBCAPS_LOCDEFER;
  dsdesc.dwBufferBytes = mSrcWaveData.cksize;
  dsdesc.lpwfxFormat = wf;
  dsdesc.guid3DAlgorithm = DS3DALG_DEFAULT;
  ret = lpDS->CreateSoundBuffer( &dsdesc,dsb,NULL );
  if( FAILED(ret) )
  {
    free( wf );
    mmioClose( hSrc,0 );
    return false;
  }

  // ロック開始
  LPVOID pMem1,pMem2;
  DWORD dwSize1,dwSize2;
  ret = (*dsb)->Lock( 0,mSrcWaveData.cksize,&pMem1,&dwSize1,&pMem2,&dwSize2,0 );
  if( FAILED(ret) ) {
    free( wf );
    mmioClose( hSrc,0 );
    return false;
  }

  // データ書き込み
  mmioRead( hSrc,(char*)pMem1,dwSize1 );
  mmioRead( hSrc,(char*)pMem2,dwSize2 );

  // ロック解除
  (*dsb)->Unlock( pMem1,dwSize1,pMem2,dwSize2 );

  // ヘッダ用メモリを開放
  free( wf );

  // WAVを閉じる
  mmioClose( hSrc,0 );

  return true;
}
#endif

void PokemonModel::KeyAnimationSlotFunc::KeyCallback(const c8* pKeyName, f32 value, void* pUserData)
{
  c8 buf[256];
#if defined(GF_PLATFORM_WIN32)
  sprintf_s(buf, "%s, %f\n", pKeyName, value);
#else
  std::sprintf(buf, "%s, %f\n", pKeyName, value);
#endif

  poke_3d::model::PokemonModel::KeyCallBackInfo* pKeyCallBackInfo = (poke_3d::model::PokemonModel::KeyCallBackInfo*)pUserData;

  // 文字が一致している場合のみフラグを立てる
  for (u32 i = 0; i < poke_3d::model::PokemonModel::KeySoundAttribute::NumberOf; ++i)
  {
    if (pKeyCallBackInfo->m_KeySoundMatchFlags[i])
    {// 既にフラグが立っているなら無視
      continue;
    }

    pKeyCallBackInfo->m_KeySoundMatchFlags[i] = 
      (std::strcmp(PokemonModel::KeyAnimationSlotFunc::s_pKeyAttributeNames[i], pKeyName) == 0);
  }
  
#if defined(GF_PLATFORM_WIN32)
  for (u32 i = 0; i < poke_3d::model::PokemonModel::KeySoundAttribute::NumberOf; ++i)
  {
    if (pKeyCallBackInfo->m_KeySoundAttributes[i] && pKeyCallBackInfo->m_KeySoundMatchFlags[i])
    {// フラグが立ってて文字列が一致していたら鳴らす
      lpSecondary->Play(0, 0, 0);
      break;
    }
  }
#endif

  // AK_PartsSkel01
  // AK_PartsVisible01
  // AK_PartsMaterial01
  // AK_PartsShader01
  // の4つのフラグ制御を行い, 再生のON/OFFを決める
  for (s32 i = poke_3d::model::PokemonModel::KeySoundAttribute::AK_PartsSkel01; i < poke_3d::model::PokemonModel::KeySoundAttribute::NumberOf; ++i)
  {
    if (std::strcmp(pKeyName, s_pKeyAttributeNames[i]) != 0)
    {
      continue;
    }

    s32 index = i - poke_3d::model::PokemonModel::KeySoundAttribute::AK_PartsSkel01;
    pKeyCallBackInfo->m_LoopAnimationFlags[index] = static_cast<b32>(value);
  }
}

PokemonModel::PokemonModel() : 
  BaseModel(),
#if defined POKEMONMODEL_PATCHEEL_EXCEPTION
  m_IsPatcheel(false),
#endif
  m_CmHeight(0.0f),
  m_AdjustHeight(0.0f),
  m_MotionNum(0),
  m_ColorTextureNum(0),
  m_RareColorConstantSettingInfoNum(0),
  m_pLightNode(NULL),
  m_pLightSetNode(NULL),
  m_pIdModel(NULL),
  m_pShadowModel(NULL),
  m_pSpringController(NULL),
  m_IsEnabledColorShader(false),
  m_InterpColor(gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 0.0f)),
  m_IsEnabledAmbientColor(false),
  m_AmbientColor(gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 0.0f)),
  m_pEffectLocatorResourceNode(NULL),
  m_pEffectLocatorInstanceNodeTop(NULL),
  m_pPokedecoLocatorResourceNode(NULL),
  m_pPokedecoLocatorInstanceNodeTop(NULL)
{
  // 上書き用のモーションのインデックスおよびスロットを初期化
  for (u32 i = 0; i < gfl2::animation::OverridePartsType::NumberOf; ++i)
  {
    m_OverridePartsMotionIndices[i] = -1;
    m_OverridePartsMotionSlots[i] = -1;
  }

  for (u32 i = 0; i < sizeof(m_MarkingFrames) / sizeof(m_MarkingFrames[0]); ++i)
  {
    m_MarkingFrames[i] = 0;
  }

  for (u32 i = 0; i < COLORTEXTURE_MAX; ++i)
  {
    m_OriginalColorTextureResourceNodes[i] = NULL;
    m_ColorShaderTextureResourceNodes[i] = NULL;
  }

  for (s32 i = 0; i < MOTION_MAX; ++i)
  {
    m_pMotionResourceNodes[i] = NULL;
  }

  for (s32 i = 0; i < MATERIAL_MAX; ++i)
  {
    m_RareColorConstantSettingInfos[i].m_OriginalColor = RareColorConstantOriginal();
    m_RareColorConstantSettingInfos[i].m_ColorOffset = RareColorConstantOffset();
  }

  m_pKeyCallBackInfo.m_pPokemonModel = NULL;
}

PokemonModel::~PokemonModel()
{
}

/**
 * @brief 作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pModelResource ベースモデルリソース
 * @param pMotionConfigResource モーションコンフィグリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool PokemonModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  BaseModel::Create(pGfxAllocator, pAnimationHeap, pModelResource, pMotionConfigResource);
  Initialize(pGfxAllocator, pAnimationHeap);
  return true;
}

/**
 * @brief ジョイント共有モデルを作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pModelResource ベースモデルリソース
 * @param pJointReferenceModel ジョイント共有モデル
 * @param pMotionConfigResource モーションコンフィグリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool PokemonModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
  BaseModel* pJointReferenceModel,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  BaseModel::Create(pGfxAllocator, pAnimationHeap, pModelResource, pJointReferenceModel, pMotionConfigResource);
  Initialize(pGfxAllocator, pAnimationHeap);
  return true;
}

/**
 * @brief 作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pModelInstance モデルインスタンス
 * @param pMotionConfigResource モーションコンフィグリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool PokemonModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  BaseModel::Create(pGfxAllocator, pAnimationHeap, pModelInstance, pMotionConfigResource);
  Initialize(pGfxAllocator, pAnimationHeap);
  return true;
}

/**
 * @brief 破棄
 */
void PokemonModel::Destroy()
{
  if ( m_pDrawEnvNode )
  {
    for (u32 i = 0; i < COLORTEXTURE_MAX; ++i)
    {
      GFL_SAFE_DELETE( m_ColorShaderTextureResourceNodes[i] );
    }

    delete m_pDrawEnvNode;
    m_pDrawEnvNode = NULL;
    //delete m_pLightSetNode;// 必要ない. DrawEnvNodeの子となっているので開放済み
    //delete m_pDirectionalLightNode;// 必要ない. LightSetNodeの子となっているので開放済み
  }

  if (m_pEffectLocatorInstanceNodeTop)
  {
    delete m_pEffectLocatorInstanceNodeTop;
    m_pEffectLocatorInstanceNodeTop = NULL;
  }

  GFL_SAFE_DELETE(m_pSpringController);

  BaseModel::Destroy();
}

/**
 * @brief ループアニメーションの設定
 * @param index ループのインデックス(0〜3)
 * @param enabled 有効/無効
 * @param animationBit 設定するアニメーションのビット
 */
void PokemonModel::SetUpLoopMotion(s32 index, b32 enabled, AnimationBit animationBit)
{
  // ループモーションの設定
  // SetOverridePartsMotionIndex()で設定されたモーションのインデックスとスロットを取得
  // その値を利用してChangeAnimation()でモーションの切り替え
  s32 loopIndex = gfl2::animation::OverridePartsType::Loop01 + index;
  s32 overridePartsMotionIndex = m_OverridePartsMotionIndices[loopIndex];
  s32 overridePartsMotionSlot = m_OverridePartsMotionSlots[loopIndex];
  if (overridePartsMotionIndex < 0)
  {
    return;
  }

  ChangeAnimationSmoothManualByResourceNode(
    (enabled) ? m_pMotionResourceNodes[overridePartsMotionIndex] : NULL,
    (enabled) ? m_pMotionResourceNodes[overridePartsMotionIndex] : NULL,
    overridePartsMotionSlot,
    animationBit);
}

/**
 * @brief ループアニメーションのフレーム設定
 * @param index ループのインデックス(0〜3)
 * @param animationBit 設定するアニメーションのビット
 */
void PokemonModel::SetLoopMotionFrame(s32 index, AnimationBit animationBit)
{
  s32 loopIndex = gfl2::animation::OverridePartsType::Loop01 + index;
  s32 overridePartsMotionSlot = m_OverridePartsMotionSlots[loopIndex];
  SyncAnimationFrameAndStepFrame(overridePartsMotionSlot, animationBit);
}

void PokemonModel::SetEnabledPlaySoundFlag(u32 index, b32 enabled)
{
  m_pKeyCallBackInfo.m_KeySoundAttributes[index] = enabled; 
}

b32 PokemonModel::GetKeySoundMatchFlag(u32 index)
{
  return (m_pKeyCallBackInfo.m_KeySoundMatchFlags[index] != 0);
}

void PokemonModel::ClearKeySoundMatchFlags()
{
  for (s32 i = 0; i < poke_3d::model::PokemonModel::KeySoundAttribute::NumberOf; ++i)
  {
    m_pKeyCallBackInfo.m_KeySoundMatchFlags[i] = false;
  }
}

b32 PokemonModel::GetPlaySoundFlag(u32 index)
{
  return (m_pKeyCallBackInfo.m_KeySoundAttributes[index] != 0);
}

/**
 * @brief 指定したインデックスのコールバックのフラグを取得
 * @param akPartsIndex 0 : AK_PartsSkel01, 1 : AK_PartsMaterial01, 2 : AK_PartsVisible01
 * @return true / false
 */
b32 PokemonModel::GetLoopAnimationFlag(u32 akPartsIndex)
{
  return (m_pKeyCallBackInfo.m_LoopAnimationFlags[akPartsIndex] != 0);
}

/**
 * @brief ループアニメーションフラグをリセット
 * @param akPartsIndex 0 : AK_PartsSkel01, 1 : AK_PartsMaterial01, 2 : AK_PartsVisible01
 */
void PokemonModel::ResetLoopAnimationFlag(u32 akPartsIndex)
{
  m_pKeyCallBackInfo.m_LoopAnimationFlags[akPartsIndex] = true;
}

b32* PokemonModel::GetPlaySoundFlagBuffer()
{
  return m_pKeyCallBackInfo.m_KeySoundAttributes;
}

/**
 * @brief キーアニメーションの更新
 */
void PokemonModel::UpdateKeyAnimation()
{
  BaseModel::UpdateAnimation(poke_3d::model::PokemonModel::ANIMATION_BIT_KEY);
}

/**
 * @brief アニメーションの更新
 *
 * @param interpWeight 補間率を指定したい場合は0.0〜1.0の範囲で設定。デフォルトは自動で補間率を更新
 */
f32 PokemonModel::UpdateAnimation(f32 interpWeight)
{
  ResetMaterialAttribute();
  ResetToRareConstantColor();

  // ループモーションが4つ以上のときの制御
  SetUpMarkings();

  if (interpWeight < 0.0f)
  {
    interpWeight = BaseModel::UpdateAnimation((poke_3d::model::PokemonModel::AnimationBit)(poke_3d::model::PokemonModel::ANIMATION_BIT_ALL & ~poke_3d::model::PokemonModel::ANIMATION_BIT_KEY));
  }
  else
  {
    interpWeight = BaseModel::UpdateAnimationManual(interpWeight, (poke_3d::model::PokemonModel::AnimationBit)(poke_3d::model::PokemonModel::ANIMATION_BIT_ALL & ~poke_3d::model::PokemonModel::ANIMATION_BIT_KEY));
  }

  ChangeToRareConstantColor();

  // アニメーション更新後にコンスタントカラーを更新
  // (アニメーション再生でコンスタントカラーを更新することがあるためここでの更新は必須)
  if (m_IsEnabledColorShader)
  {
    UpdateColorShaderConstantColor();
  }

  // アニメーション更新後に環境光を更新
  if (m_IsEnabledAmbientColor)
  {
    UpdateAmbientConstantColor();
  }

  return interpWeight;
}

void PokemonModel::AddRareConstantColor(
  s32 materialNo,
  s32 constantColorIndex,
  gfl2::math::Vector4 colorOffset)
{
  if (materialNo < 0 || constantColorIndex < 0)
  {
    return;
  }

  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
  pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterialInstanceNode->GetAttributeParam();

  RareColorConstantOriginal rareColorConstantOriginal(rAttributeParam.m_ConstantColor[constantColorIndex], static_cast<u8>(materialNo));
  m_RareColorConstantSettingInfos[m_RareColorConstantSettingInfoNum].m_OriginalColor = rareColorConstantOriginal;

  RareColorConstantOffset rareColorConstantOffset(colorOffset, static_cast<u8>(constantColorIndex));
  m_RareColorConstantSettingInfos[m_RareColorConstantSettingInfoNum].m_ColorOffset = rareColorConstantOffset;

  ++m_RareColorConstantSettingInfoNum;
}

/**
 * @brief レアカラー用の定数カラーに変更
 */
void PokemonModel::ChangeToRareConstantColor()
{
  for (u32 i = 0; i < m_RareColorConstantSettingInfoNum; ++i)
  {
    RareColorConstantSettingInfo& info = m_RareColorConstantSettingInfos[i];
    if (!info.m_OriginalColor.IsAvailableMaterialIndex())
    {
      return;
    }

    if (!info.m_ColorOffset.IsAvailableConstantColorIndex())
    {
      return;
    }

    // ゴルーグの体の模様などは専用のオフセットがあるので加算して対応
    u32 materialNo = info.m_OriginalColor.GetMaterialIndex();
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterialInstanceNode->GetAttributeParam();

    // レアカラー用の定数カラーで更新
    // アルファ値はレアカラーのコンスタントの設定にないので省略
    u32 index = info.m_ColorOffset.GetConstantColorIndex();
    gfl2::gfx::ColorU8 destinationColor = info.m_ColorOffset.AddColorOffset(rAttributeParam.m_ConstantColor[index]);
    rAttributeParam.m_ConstantColor[index] = destinationColor;
  }
}

/**
 * @brief レアカラー用の定数カラーに変更
 */
void PokemonModel::ResetToRareConstantColor()
{
  for (u32 i = 0; i < m_RareColorConstantSettingInfoNum; ++i)
  {
    RareColorConstantSettingInfo& info = m_RareColorConstantSettingInfos[i];
    if (!info.m_OriginalColor.IsAvailableMaterialIndex())
    {
      return;
    }

    if (!info.m_ColorOffset.IsAvailableConstantColorIndex())
    {
      return;
    }

    // ゴルーグの体の模様などは専用のオフセットがあるので加算して対応
    u32 materialNo = info.m_OriginalColor.GetMaterialIndex();
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterialInstanceNode->GetAttributeParam();

    // レアカラー用の定数カラーで更新
    gfl2::gfx::ColorU8 originalColor32Bit = info.m_OriginalColor.GetColor();
    u32 index = info.m_ColorOffset.GetConstantColorIndex();
    rAttributeParam.m_ConstantColor[index].r = originalColor32Bit.r;
    rAttributeParam.m_ConstantColor[index].g = originalColor32Bit.g;
    rAttributeParam.m_ConstantColor[index].b = originalColor32Bit.b;
  }
}

gfl2::renderingengine::scenegraph::resource::ResourceNode* PokemonModel::GetMotionResourceNode(s32 index) const
{
  return m_pMotionResourceNodes[index];
}

void PokemonModel::SetMotionResourceNode(s32 index, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  m_pMotionResourceNodes[index] = pResourceNode;
}

void PokemonModel::SetHeight(const f32 cmHeight, const f32 adjustHeight)
{
  m_CmHeight = cmHeight;
  m_AdjustHeight = adjustHeight;
  f32 scale = CalculateAdjustScale();
  BaseModel::SetScale(scale, scale, scale);
}

f32 PokemonModel::CalculateAdjustScale()
{
  GFL_ASSERT(m_CmHeight > 0.0f);
  return m_AdjustHeight / m_CmHeight;
}

void PokemonModel::CreateColorShaderTextures(gfl2::gfx::IGLAllocator* pGfxAllocator)
{
  // テクスチャ生成用のファクトリを取得
  gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
    gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
    gfl2::renderingengine::scenegraph::resource::IResourceData::Type("texture")
    );

  // カラーテクスチャを収集してカラーテクスチャを生成
  m_ColorTextureNum = 0;

  u32 materialNum = this->GetModelInstanceNode()->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);

    if (!ContainsColorShaderTextureInMaterial(pMaterialInstanceNode))
    {
      continue;
    }

    for (u32 textureNo = 0; textureNo <= gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode::Texture2; ++textureNo)
    {
      if (!IsColorShaderTextureInMaterial(pMaterialInstanceNode, static_cast<gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode>(textureNo)))
      {
        continue;
      }

      const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode;
      pOriginalTextureResourceNode = pMaterialInstanceNode->GetTextureResourceNode(textureNo);
      const gfl2::gfx::Texture* pTexture = pOriginalTextureResourceNode->GetTexture();

			gfl2::renderingengine::scenegraph::resource::TextureResourceNode*			pVramTexture = NULL;

			//すでにＶＲＡＭに乗っているかのチェックを差し戻し用ポインタを見て調べる。
			for (u32 colTexNo = 0; colTexNo < m_ColorTextureNum; ++colTexNo)
			{
				if ( m_OriginalColorTextureResourceNodes[colTexNo] == pOriginalTextureResourceNode )
				{//配列に積まれているということは、すにでＶＲＡＭに転送している。
					pVramTexture = m_ColorShaderTextureResourceNodes[colTexNo];
					break;
				}
			}

			if ( pVramTexture == NULL )
			{//見つからなかったのでまだＶＲＡＭに積まれていない。
				m_OriginalColorTextureResourceNodes[m_ColorTextureNum] = pOriginalTextureResourceNode;

				// カラーテクスチャを作成
				gfl2::gfx::Texture::TextureDesc desc = pTexture->GetTextureDesc();
				desc.format = gfl2::gfx::Format::R5G6B5;

				gfl2::gfx::Texture* pColorShaderTexture = gfl2::gfx::GFGL::CreateTexture(
          pGfxAllocator,
					desc.width,
					desc.height,
					desc.mipLevels,
					gfl2::gfx::Usage::RenderTarget,  // テクスチャはレンダーターゲットとして使う.
					desc.format,
					desc.pool);

				gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
					reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

				m_ColorShaderTextureResourceNodes[m_ColorTextureNum] = 
					reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(
					pTextureResourceNodeFactory->CreateResourceNode(
          pGfxAllocator,
					m_OriginalColorTextureResourceNodes[m_ColorTextureNum]->GetNodeName(),
					pColorShaderTexture)
					);

				pVramTexture = m_ColorShaderTextureResourceNodes[m_ColorTextureNum];
				++m_ColorTextureNum;
			}

      // カラーシェーダのテクスチャリソースに差し替え
      pMaterialInstanceNode->SetTextureResourceNode(textureNo, pVramTexture);
      
    }
  }
}

void PokemonModel::SetEnabledColorShaderTexture(b32 enable)
{
  m_IsEnabledColorShader = enable;
  if (!enable)
  {
    m_InterpColor.x = 0.0f;
    m_InterpColor.y = 0.0f;
    m_InterpColor.z = 0.0f;
    m_InterpColor.w = 0.0f;
  }
}

/**
 * @brief カラーシェーダテクスチャの有効/無効の取得
 * @return b32 有効/無効
 */
b32 PokemonModel::IsEnabledColorShaderTexture()
{
  return m_IsEnabledColorShader;
}

/**
 * @brief 環境光の設定の有効/無効の切り替え
 * @param enable 有効/無効
 */
void PokemonModel::SetEnabledAmbientColor(b32 enable)
{
  m_IsEnabledAmbientColor = enable;
}

/**
 * @brief 環境光の設定の有効/無効の取得
 * @return b32 有効/無効
 */
b32 PokemonModel::IsEnabledAmbientColor()
{
  return m_IsEnabledAmbientColor;
}

void PokemonModel::SetKeyCallBackInfo()
{
  SetCallback(&m_pKeyCallBackInfo, &poke_3d::model::PokemonModel::KeyAnimationSlotFunc::KeyCallback);
}

void PokemonModel::SetCallback(void* pUserData, gfl2::animation::KeyAnimationCallback* pCallback)
{
  SetKeyAnimationCallback(pUserData, pCallback);
}

/**
 * @brief animationBitで指定したアニメーションがループモーションに含まれているかどうかの確認
 * @param loopIndex ループモーションのインデックス(0～3)
 * @return true/false ジョイントアニメーションを持っている/持っていない
 */
b32 PokemonModel::HasJointAnimationInLoopMotion(u32 loopIndex)
{
  const u32 loopMotionMax = static_cast<u32>(gfl2::animation::OverridePartsType::Loop04 - gfl2::animation::OverridePartsType::Loop01) + 1;
  // ループのモーションはloop01～loop04の4つのため, それ以上の値が来ていたら止める.
  GFL_ASSERT(loopIndex < loopMotionMax);

  // ループモーションに該当するモーションリソースノードを取得
  u32 loopMotionResourceNodeIndex = m_OverridePartsMotionIndices[loopIndex + gfl2::animation::OverridePartsType::Loop01];
  gfl2::renderingengine::scenegraph::DagNode* pLoopMotionResourceNode = m_pMotionResourceNodes[loopMotionResourceNodeIndex];
  if (!pLoopMotionResourceNode)
  {// インデックスで指定されたノードがなければfalse
    return false;
  }

  // animationBitで指定されたアニメーションを持っているかどうかを確認する
  // PokemonModelクラスでm_pMotionResourceNodesを使用しているのは, 
  // C++/CLIのPokemonModel_CLRと, nijiのPokeModelのみ.
  // m_pMotionResourceNodesにはモーションのデータしか入っていないことは,
  // 2016/04/13時点で確認済みのため, SafeCast<>でAnimationResourceNodeにキャスト.
  // 子供にぶら下がっている可能性がある. 子ノードがある場合はそちらを取得
  if (pLoopMotionResourceNode->GetChildCount() > 0)
  {
    pLoopMotionResourceNode = pLoopMotionResourceNode->GetChild();
  }
  gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimationResourceNode = NULL;
  pAnimationResourceNode = pLoopMotionResourceNode->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();

  if (!pAnimationResourceNode)
  {// キャストに失敗したらAnimationResourceNodeではないのでfalse
    return false;
  }

  return pAnimationResourceNode->HasJointAnimation();
}

/**
 * @brief モデル生成後の初期化
 */
void PokemonModel::Initialize(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap)
{
  // 各種変数の初期化
  m_CmHeight = 0.0f;
  m_AdjustHeight = 0.0f;
  m_MotionNum = 0;
  m_ColorTextureNum = 0;
  m_RareColorConstantSettingInfoNum = 0;
  m_pLightNode = NULL;
  m_pLightSetNode = NULL;
  m_pIdModel = NULL;
  m_pShadowModel = NULL;
  m_IsEnabledColorShader = false;
  m_InterpColor = gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  m_IsEnabledAmbientColor = false;
  m_AmbientColor = gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  m_pEffectLocatorResourceNode = NULL;
  m_pEffectLocatorInstanceNodeTop = NULL;

  for (u32 i = 0; i < gfl2::animation::OverridePartsType::NumberOf; ++i)
  {
    m_OverridePartsMotionIndices[i] = -1;
    m_OverridePartsMotionSlots[i] = -1;
  }

  for (u32 i = 0; i < COLORTEXTURE_MAX; ++i)
  {
    m_OriginalColorTextureResourceNodes[i] = NULL;
    m_ColorShaderTextureResourceNodes[i] = NULL;
  }

  for (s32 i = 0; i < MOTION_MAX; ++i)
  {
    m_pMotionResourceNodes[i] = NULL;
  }

  for (s32 i = 0; i < MATERIAL_MAX; ++i)
  {
    m_RareColorConstantSettingInfos[i].m_OriginalColor = RareColorConstantOriginal();
    m_RareColorConstantSettingInfos[i].m_ColorOffset = RareColorConstantOffset();
  }

  for (s32 i = 0; i < poke_3d::model::PokemonModel::KeySoundAttribute::NumberOf; ++i)
  {
    m_pKeyCallBackInfo.m_KeySoundMatchFlags[i] = false;
    m_pKeyCallBackInfo.m_KeySoundAttributes[i] = false;
  }
  for (s32 i = 0; i < sizeof(m_pKeyCallBackInfo.m_LoopAnimationFlags) / sizeof(m_pKeyCallBackInfo.m_LoopAnimationFlags[0]); ++i)
  {
    m_pKeyCallBackInfo.m_LoopAnimationFlags[i] = true;
  }

  m_pKeyCallBackInfo.m_pPokemonModel = this;
  this->GetModelInstanceNode()->SetUpDrawTag();

  // 各種カラーテクスチャを検索して保存
  u32 materialNum = this->GetModelInstanceNode()->GetMaterialNum();
  GFL_ASSERT((materialNum <= poke_3d::model::PokemonModel::MATERIAL_MAX) && "PokemonModel::Initialize() doesn't work correctly. The number of materials should be less than 16.");
  // 元のマテリアル情報を保持
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
    m_OriginalAttributeParams[materialNo] = *pMaterialInstanceNode->GetAttributeParam();
  }

  s16 lightSetNo = m_OriginalAttributeParams[0].m_LightSetNo;
  for (u32 i = 1; i < materialNum; ++i)
  {// ライトセット番号が一致しているかを調べる.
    GFL_ASSERT(lightSetNo == m_OriginalAttributeParams[i].m_LightSetNo);
  }

  // DrawEnvNodeの作成
  m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(pGfxAllocator);

  // AABBの設定
  m_pDrawEnvNode->SetAABB(
    gfl2::math::AABB(
      gfl2::math::Vector4(-50000.0f, -50000.0f, -50000.0f),
      gfl2::math::Vector4(50000.0f, 50000.0f, 50000.0f)
    )
  );

  // ライトとライトセットのノードを作成
  m_pLightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(pGfxAllocator);
  m_pLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pGfxAllocator);
  m_pLightNode->SetLightType(gfl2::renderingengine::LightType::Directional);
  gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 0.0f, 10000000.0f);
  m_pLightNode->SetPosition(position);
  gfl2::math::Vector3 direction = -position.Normalize();
  m_pLightNode->SetDirection(direction);
  m_pLightNode->SetColor(gfl2::math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  m_pLightSetNode->SetSetNo(lightSetNo);        // いったん0を設定.
  m_pLightSetNode->AddLight(m_pLightNode);

  if (lightSetNo == 2)
  {// 2はフィールド用モデルなのでAmbientを追加
    gfl2::renderingengine::scenegraph::instance::LightNode* pAmbientLightNode;
    pAmbientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pGfxAllocator);
    pAmbientLightNode->SetLightType(gfl2::renderingengine::LightType::Ambient);
    pAmbientLightNode->SetColor(gfl2::math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_pLightSetNode->AddLight(pAmbientLightNode);
  }

  m_pDrawEnvNode->AddLightSet(m_pLightSetNode); //DrawEnvNodeの子となる

  this->InitializeSpring(pAnimationHeap);

#if defined POKEMONMODEL_PATCHEEL_EXCEPTION
  // パッチールの文字列を検索し, フラグを有効化
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = GetModelInstanceNode();
  u32 jointNum = pModelInstanceNode->GetJointNum();
  if (jointNum == 0)
  {// ジョイントを使用していないモデルを考慮してここで弾いておく
    return;
  }

  // Topのジョイントは0番なので, 0番のジョイントの名前を取得. pm0327_00かを調べる.
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pTopJointInstanceNode = pModelInstanceNode->GetJointInstanceNode(0);
  const c8* topJointName = pTopJointInstanceNode->GetName();
  m_IsPatcheel = ::std::strcmp(topJointName, "pm0327_00") == 0;
#endif
}

/**
 * @brief カラーシェーダ用定数の更新
 */
void PokemonModel::UpdateColorShaderConstantColor()
{
  u32 materialNum = this->GetModelInstanceNode()->GetMaterialNum();
  // gfl_G3dH3dModel.hの以下の機能に相当
  // void H3dPokemonShaderModel::changeConstantColor( const changeInfo &rInfo )
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterialInstanceNode->GetAttributeParam();
    const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::UserData& rUserData = pMaterialInstanceNode->GetMaterialResourceNode()->GetUserData();

    if (ContainsColorShaderTextureInMaterial(pMaterialInstanceNode))
    {
      if ( rAttributeParam.m_UseShadowTexture )
      {
        // 影の色の変更をConstant[0]に移行
        rAttributeParam.m_ConstantColor[0].a = static_cast<u8>( m_InterpColor.w * 255.0f );
      }
      else
      {
        gfl2::gfx::Color      f32Color = rAttributeParam.m_ConstantColor[0].GetColor();

        //平均化した影の値を算出
        f32 shadowColor = 0.0f;
        for (u32 i = 0; i < 3; ++i)
        {// r, g, bの3回ぶんループ
          shadowColor += f32Color[i];
        }
        shadowColor /= 3.0f;

        // 影の色の変更をConstant[0]に行う.
        for (u32 i = 0; i < 3; ++i)
        {// r, g, bの3回ぶんループ
          f32Color[i] = f32Color[i] * (1.0f - m_InterpColor.w);
          f32Color[i] += shadowColor * m_InterpColor.w;
        }
        rAttributeParam.m_ConstantColor[0] = f32Color;
      }
    }

    for (u32 constantNo = 0; constantNo < gfl2::renderingengine::scenegraph::resource::CombinerAttribute::ConstantMax; ++constantNo)
    {
      if (rUserData.m_BakeConstants[constantNo] == gfl2::renderingengine::scenegraph::resource::TextureBakeType::None)
      {// Noneは影響が無いのでスルー
        continue;
      }

      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
      pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterialInstanceNode->GetAttributeParam();
      gfl2::math::Vector4 originalColor = rAttributeParam.m_ConstantColor[constantNo].GetColor();

      if (rUserData.m_BakeConstants[constantNo] == gfl2::renderingengine::scenegraph::resource::TextureBakeType::AtLight)
      {
        continue;
      }

      f32 colorBrightness = (originalColor.x + originalColor.y + originalColor.z) / 3.0f;
      f32 brightnessShift = static_cast<f32>( m_OriginalAttributeParams[2].m_ConstantColor[0].a ) / 255.0f;    // コンスタント2を輝度値計算に用いている
      gfl2::math::Vector4 effectColor;

      f32 a = colorBrightness;
      f32 b = 1.0f;
      f32 c = 1.0f - brightnessShift;
      f32 lerp = m_InterpColor.w;
      colorBrightness = ( a * c ) + ( b * (1-c) );

      effectColor.x = colorBrightness * m_InterpColor.x * lerp;
      effectColor.y = colorBrightness * m_InterpColor.y * lerp;
      effectColor.z = colorBrightness * m_InterpColor.z * lerp;

      originalColor.x *= (1.0f - lerp);
      originalColor.y *= (1.0f - lerp);
      originalColor.z *= (1.0f - lerp);

      effectColor.x += originalColor.x;
      effectColor.y += originalColor.y;
      effectColor.z += originalColor.z;
      effectColor.w = originalColor.w;

      rAttributeParam.m_ConstantColor[constantNo] = effectColor;
    }
  }
}

/**
 * @brief 環境光用定数の更新
 */
void PokemonModel::UpdateAmbientConstantColor()
{
  u32 materialNum = this->GetModelInstanceNode()->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* pParam = pMaterialInstanceNode->GetAttributeParam();
    pParam->m_ConstantColor[5] = m_AmbientColor;    // [5]が環境光用の定数バッファ
  }
}

void PokemonModel::ResetMaterialAttribute()
{
  u32 materialNum = this->GetModelInstanceNode()->GetMaterialNum();
  // カラーシェーダ無効の場合は, ResourceNodeから本来のConstantColor[0]を取得して元に戻す
  // @fixme : マテリアル情報をリセットする関数をつくってそちらにまとめる.
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = this->GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rInstanceAttributeParam = *pMaterialInstanceNode->GetAttributeParam();
    rInstanceAttributeParam = m_OriginalAttributeParams[materialNo];
  }
}

/**
 * @brief 模様を固定する
 */
void PokemonModel::SetUpMarkings()
{
#if defined POKEMONMODEL_PATCHEEL_EXCEPTION
  if (!m_IsPatcheel)
  {// パッチールでなければ模様は固定しない
    return;
  }
#endif

  // ループが4つ設定されていなければ何もしない
  for (u32 type = gfl2::animation::OverridePartsType::Loop01; type <= gfl2::animation::OverridePartsType::Loop04; ++type)
  {
    if (m_OverridePartsMotionIndices[type] < 0)
    {
      return;
    }
  }

  // メンバとして持つ指定フレームにて固定
  for (u32 type = gfl2::animation::OverridePartsType::Loop01; type <= gfl2::animation::OverridePartsType::Loop04; ++type)
  {
    u8 fixedFrame = m_MarkingFrames[type - gfl2::animation::OverridePartsType::Loop01];

    // 通常のモーションが0番に入っているのはGFModelViewerの仕様
    // nijiで仕様しているPokeModelは0に通常のモーション, 1に原点移動用のモーションがあるため,
    // SetOverridePartsMotionIndex()で指定したスロットを指定する
    u32 slotIndex = m_OverridePartsMotionSlots[type];

    // リソースのインデックスを取得
    s32 loopMotionIndex = m_OverridePartsMotionIndices[type];

    ChangeAnimationSmoothManualByResourceNode(m_pMotionResourceNodes[loopMotionIndex], m_pMotionResourceNodes[loopMotionIndex], slotIndex, BaseModel::ANIMATION_BIT_MATERIAL);

    // 補間用スロットの設定
    SetAnimationControllTargetInterp(true);
    SetAnimationFrame(static_cast<f32>(fixedFrame), slotIndex, BaseModel::ANIMATION_BIT_MATERIAL);

    // 通常用スロットの設定
    SetAnimationControllTargetInterp(false);
    SetAnimationFrame(static_cast<f32>(fixedFrame), slotIndex, BaseModel::ANIMATION_BIT_MATERIAL);
  }
}

/**
 * @brief マテリアルがカラーシェーダのものを含んでいるか判定
 * @param gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* マテリアルインスタンスノード
 * @return true/false
 */
bool PokemonModel::ContainsColorShaderTextureInMaterial(const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode)
{
  for (u32 textureNo = 0; textureNo <= gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode::Texture2; ++textureNo)
  {
    if (IsColorShaderTextureInMaterial(
      pMaterialInstanceNode,
      (gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode)textureNo))
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief マテリアル内のテクスチャがカラーシェーダの対象かを判定
 * @param gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* マテリアルインスタンスノード
 * @param gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode テクスチャの種類
 * @return true/false
 */
bool PokemonModel::IsColorShaderTextureInMaterial(
  const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode,
  gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode textureNodeType)
{
  u32 textureNum = pMaterialInstanceNode->GetTextureNum();
  if (textureNum == 0)
  {
    return false;
  }

  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::UserData& rUserData = pMaterialInstanceNode->GetMaterialResourceNode()->GetUserData();

  gfl2::renderingengine::scenegraph::resource::TextureBakeType bakeType =
    static_cast<gfl2::renderingengine::scenegraph::resource::TextureBakeType>(rUserData.m_BakeTextures[textureNodeType]);
  if ( (bakeType != gfl2::renderingengine::scenegraph::resource::TextureBakeType::AtAll)
    && (bakeType != gfl2::renderingengine::scenegraph::resource::TextureBakeType::AtEffectCol)
    )
  {// "AtAll"と"AtEffectCol"以外は未使用なので無視
    return false;
  }

  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode;
  pOriginalTextureResourceNode = pMaterialInstanceNode->GetTextureResourceNode(textureNodeType);
  const gfl2::gfx::Texture* pTexture = pOriginalTextureResourceNode->GetTexture();
  if (!pTexture)
  {
    return false;
  }

  return true;
}

/**
 * @brief カラーシェーダ用補間色設定
 */
void PokemonModel::SetInterpColor(const gfl2::math::Vector4& color)
{
  m_InterpColor = color;
}

/**
 * @brief カラーシェーダ用補間色取得
 */
gfl2::math::Vector4 PokemonModel::GetInterpColor()
{
  return m_InterpColor;
}

/**
 * @brief 環境光の有効/無効切り替え
 * @param enabled 有効/無効
 */


/**
 * @brief 環境光の色設定
 * @param color 設定する色
 */
void PokemonModel::SetAmbientColor(const gfl2::math::Vector4& color)
{
  m_AmbientColor = color;
}

/**
 * @brief カラーシェーダ用補間色取得
 */
gfl2::math::Vector4 PokemonModel::GetAmbientColor()
{
  return m_AmbientColor;
}

/**
 * @brief 歩き用速度を取得：ビューアー用：ポケモン用の拡張データ
 *
 * @param stepFrame 再生速度
 * @param prevFrame 補間用：アニメーション０のフレーム
 * @param nextFrame 補間用：アニメーション１のフレーム
 * @param interpWeight 補間率
 *
 * @return 移動速度ベクトル
 */
gfl2::math::Vector3 PokemonModel::GetWalkSpeedManual(f32 stepFrame, f32 prevFrame, f32 nextFrame, f32 interpWeight) const
{
  return this->GetAnimationController()->GetWalkSpeedManual(stepFrame, prevFrame, nextFrame, interpWeight);
}

/**
 * @brief ライトセット番号の設定
 * @param lightSetNo ライトセット番号
 */
void PokemonModel::SetLightSetNo(u32 lightSetNo)
{
  GFL_ASSERT(m_pLightSetNode);
  
  // ノードとアトリビュートのライト番号を設定.
  m_pLightSetNode->SetSetNo(lightSetNo);
  for (s32 i = 0; i < MATERIAL_MAX; ++i)
  {
    m_OriginalAttributeParams[i].m_LightSetNo = lightSetNo;
  }
}

/**
 * @brief ライトの方向設定
 * @brief direction 方向ベクトル
 */
void PokemonModel::SetLightDirection(const gfl2::math::Vector3& direction)
{
  if (GetModelInstanceNode()->GetMaterialNum() == 0)
  {
    return;
  }

  u32 lightSetNo = GetModelInstanceNode()->GetMaterialInstanceNode(0)->GetAttributeParam()->m_LightSetNo;
  m_pDrawEnvNode->GetLightSet(lightSetNo)->GetLightNode(0)->SetDirection(direction);
}

/**
 * @brief ライトの方向取得
 * @brief direction 方向ベクトル
 */
gfl2::math::Vector3 PokemonModel::GetLightDirection()
{
  if (GetModelInstanceNode()->GetMaterialNum() == 0)
  {
    return gfl2::math::Vector3();
  }

  u32 lightSetNo = GetModelInstanceNode()->GetMaterialInstanceNode(0)->GetAttributeParam()->m_LightSetNo;
  gfl2::renderingengine::scenegraph::instance::LightNode* pLightNode;
  pLightNode =  m_pDrawEnvNode->GetLightSet(lightSetNo)->GetLightNode(0);
  return pLightNode->GetDirection();
}

/**
 * @brief ロケータインスタンスノードをリソースノードから設定
 * @param gfl2::gfx::IGLAllocator* pGfxAllocator LocatorInstanceNodeを生成するためのアロケータ
 * @param pLocatorResourceNode 元となるロケータインスタンスノード
 */
void PokemonModel::AttachEffectLocatorInstanceNodeTopWithLocatorResourceNode(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pLocatorResourceNode)
{
  m_pEffectLocatorResourceNode = pLocatorResourceNode;
  m_pEffectLocatorInstanceNodeTop = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLocatorInstanceNode(pGfxAllocator, pLocatorResourceNode);
}

/**
 * @brief ポケデコ用ロケータインスタンスノードをリソースノードから設定
 * @param gfl2::gfx::IGLAllocator* pGfxAllocator LocatorInstanceNodeを生成するためのアロケータ
 * @param pLocatorResourceNode 元となるロケータインスタンスノード
 */
void PokemonModel::AttachPokedecoLocatorInstanceNodeTopWithLocatorResourceNode(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pLocatorResourceNode)
{
  m_pPokedecoLocatorResourceNode = pLocatorResourceNode;
  m_pPokedecoLocatorInstanceNodeTop = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLocatorInstanceNode(pGfxAllocator, pLocatorResourceNode);
}

/**
 * @brief 歩き用速度を取得：ポケモン用の拡張データ
 * @param interpWeight 補間率
 * @return 移動速度ベクトル
 */
gfl2::math::Vector3 PokemonModel::GetWalkSpeed(f32 interpWeight)
{
  return BaseModel::GetWalkSpeed(interpWeight);
}

/**
 * @brief 同名のテクスチャと差し替え
 * @param pTextureResourceNode 差し替えるテクスチャリソースノード
 */
void PokemonModel::ReplaceWithIdTextureResourceNode(gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pIdTextureResourceNode)
{
  u32 materialNum = GetModelInstanceNode()->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
    pMaterialInstanceNode = GetModelInstanceNode()->GetMaterialInstanceNode(materialNo);

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

      const char* originalTextureName = pOriginalTextureResourceNode->GetName();
      const char* idTextureName = pIdTextureResourceNode->GetName();
      if (strcmp(originalTextureName, idTextureName) != 0)
      {
        continue;
      }

      // IDテクスチャの差し替え(ノードを差し替えて実装)
      pMaterialInstanceNode->SetTextureResourceNode(textureNo, pIdTextureResourceNode);
      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* pTextureInfo = pMaterialInstanceNode->GetTextureInfo(textureNo);
      // IDテクスチャを使うときはScale以外の値は全て0に設定して使う.
      pTextureInfo->m_Attribute.m_ScaleU = 1.0f;
      pTextureInfo->m_Attribute.m_ScaleV = 1.0f;
      pTextureInfo->m_Attribute.m_RepeatU = 0.0f;
      pTextureInfo->m_Attribute.m_RepeatV = 0.0f;
      pTextureInfo->m_Attribute.m_TranslateU = 0.0f;
      pTextureInfo->m_Attribute.m_TranslateV = 0.0f;
      pTextureInfo->m_Attribute.m_MagFilter = 0;
      pTextureInfo->m_Attribute.m_MinFilter = 0;
      pTextureInfo->m_Attribute.m_MappingType = 0;
      pTextureInfo->m_Attribute.m_UvSetNo = 0;
    }
  }
}

/**
 * @brief バネ初期化
 *
 * @param pAnimationHeap アニメーション用ヒープ
 */
void PokemonModel::InitializeSpring(gfl2::heap::HeapBase* pAnimationHeap)
{
#if POKEMON_MODEL_SPRING_FLAG
  // 既に作成済みなら削除
  GFL_SAFE_DELETE(m_pSpringController);

  // バネの初期化
  const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigInstanceNode = 
    static_cast<const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*>(this->GetModelInstanceNode()->GetMotionConfigInstanceNode());

  if (pMotionConfigInstanceNode != NULL)
  {
    m_pSpringController = GFL_NEW(pAnimationHeap) gfl2::animation::SpringController();
    m_pSpringController->Initialize(pAnimationHeap, this->GetModelInstanceNode(), pMotionConfigInstanceNode);
  }
#endif
}

/**
 * @brief バネ更新
 */
void PokemonModel::UpdateSpring()
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->Update();
}

/**
 * @brief バネ制御有効無効を設定
 *
 * @param enable true:バネ制御有効 false:バネ制御無効
 */
void PokemonModel::SetSpringEnable(bool enable)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetEnable(enable);
}

/**
 * @brief バネ制御有効無効を取得
 *
 * @return true:バネ制御有効 false:バネ制御無効
 */
bool PokemonModel::IsSpringEnable() const
{
  if (m_pSpringController == NULL)
  {
    return false;
  }

  return m_pSpringController->IsEnable();
}

/**
 * @brief バネ減衰設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param damping 減衰
 */
void PokemonModel::SetSpringDamping(SpringSide lr, f32 damping)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetDamping(static_cast<gfl2::animation::SpringController::Side>(lr), damping);
}

/**
 * @brief バネ減衰取得
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @return 減衰を返す
 */
f32 PokemonModel::GetSpringDamping(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return 0.0f;
  }

  return m_pSpringController->GetDamping(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネ定数設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param stiffness バネ定数
 */
void PokemonModel::SetSpringStiffness(SpringSide lr, f32 stiffness)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetStiffness(static_cast<gfl2::animation::SpringController::Side>(lr), stiffness);
}

/**
 * @brief バネ定数取得
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @return バネ定数を返す
 */
f32 PokemonModel::GetSpringStiffness(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return 0.0f;
  }

  return m_pSpringController->GetStiffness(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネ質量設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param mass 質量
 */
void PokemonModel::SetSpringMass(SpringSide lr, f32 mass)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetMass(static_cast<gfl2::animation::SpringController::Side>(lr), mass);
}

/**
 * @brief バネ質量取得
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @return 質量を返す
 */
f32 PokemonModel::GetSpringMass(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return 0.0f;
  }

  return m_pSpringController->GetMass(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネ重力設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param gravity 重力
 */
void PokemonModel::SetSpringGravity(SpringSide lr, f32 gravity)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetGravity(static_cast<gfl2::animation::SpringController::Side>(lr), gravity);
}

/**
 * @brief バネ重力取得
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @return 重力を返す
 */
f32 PokemonModel::GetSpringGravity(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return 0.0f;
  }

  return m_pSpringController->GetGravity(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネＹ軸回転だけ行う設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param flag Ｙ軸回転だけならtrue, 全て回転させるならfalse
 */
void PokemonModel::SetSpringRotOnlyY(SpringSide lr, b32 flag)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetRotOnlyY(static_cast<gfl2::animation::SpringController::Side>(lr), flag);
}

/**
 * @brief バネＹ軸回転だけ行うか？
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @return Ｙ軸回転だけならtrue, 全て回転させるならfalse
 */
b32 PokemonModel::IsSpringRotOnlyY(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return false;
  }

  return m_pSpringController->IsRotOnlyY(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネ外力設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param externalForce 外力
 */
void PokemonModel::SetSpringExternalForce(SpringSide lr, const gfl2::math::Vector3& externalForce)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetExternalForce(static_cast<gfl2::animation::SpringController::Side>(lr), externalForce);
}

/**
 * @brief バネ外力取得
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @return 外力を返す
 */
const gfl2::math::Vector3 PokemonModel::GetSpringExternalForce(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return gfl2::math::Vector3();
  }

  return m_pSpringController->GetExternalForce(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネダウンフォース設定
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 * @param downForce ダウンフォース
 */
void PokemonModel::SetSpringDownForce(SpringSide lr, f32 downForce)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->SetDownForce(static_cast<gfl2::animation::SpringController::Side>(lr), downForce);
}

/**
 * @brief バネダウンフォース取得
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 *
 * @return ダウンフォースを返す
 */
f32 PokemonModel::GetSpringDownForce(SpringSide lr) const
{
  if (m_pSpringController == NULL)
  {
    return 0.0f;
  }

  return m_pSpringController->GetDownForce(static_cast<gfl2::animation::SpringController::Side>(lr));
}

/**
 * @brief バネパラメータのリセットリクエスト
 *
 * @param lr SPRING_SIDE_L or SPRING_SIDE_R
 */
void PokemonModel::RequestSpringParameterReset(SpringSide lr)
{
  if (m_pSpringController == NULL)
  {
    return;
  }

  m_pSpringController->RequestParameterReset(static_cast<gfl2::animation::SpringController::Side>(lr));
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
  m_MarkingFrames[0] = markingFrame1;
  m_MarkingFrames[1] = markingFrame2;
  m_MarkingFrames[2] = markingFrame3;
  m_MarkingFrames[3] = markingFrame4;
}

}}
