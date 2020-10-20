#include <Animation/include/gfl2_MaterialAnimationSlot.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <math/include/gfl2_Quaternion.h>


using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
MaterialAnimationSlot::MaterialAnimationSlot() :
  AnimationSlotBase(),
	m_pMaterialDataSetList(NULL),
  m_MaterialDataSetCount(0)
{
}

/**
 * @brief デストラクタ
 */
MaterialAnimationSlot::~MaterialAnimationSlot()
{
}

/**
 * @brief マテリアルを適用
 *
 * @param materialIndex マテリアルインデックス
 */
void MaterialAnimationSlot::ApplyMaterial(u32 materialIndex)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // マテリアルが無いなら無視
  if (m_pMaterialDataSetList == NULL)
  {
    return;
  }

  scenegraph::instance::MaterialInstanceNode* pMaterialNode = m_pModelNode->GetMaterialInstanceNode(materialIndex);
  MaterialDataSet* pMaterialDataSet = m_pMaterialDataSetList[materialIndex];

  // メモリ確保されていないなら無視
  if (pMaterialDataSet == NULL)
  {
    return;
  }

  // UVアニメーションの反映
  for (u32 animSlotNo = 0; animSlotNo < pMaterialDataSet->m_UvAnimSlotCount; ++animSlotNo)
  {
    MaterialDataSet::SlotData* pSlotData = &pMaterialDataSet->m_SlotDataList[animSlotNo];

    scenegraph::resource::MaterialResourceNode::TextureInfo* pTextureInfo = pMaterialNode->GetTextureInfo(pSlotData->m_pUvAnimBody->textureSlotNo);

    f32* valueList[] =
    {
      &pTextureInfo->m_Attribute.m_ScaleU,
      &pTextureInfo->m_Attribute.m_ScaleV,
      &pTextureInfo->m_Attribute.m_Rotate,
      &pTextureInfo->m_Attribute.m_TranslateU,
      &pTextureInfo->m_Attribute.m_TranslateV,
    };

    for (u32 curveIndex = 0; curveIndex < MaterialDataSet::UV_INDEX_COUNT; ++curveIndex)
    {
      CurvePlayer* pCurvePlayer = &pSlotData->m_UvCurvePlayerList[curveIndex];
      if (pCurvePlayer->m_type != CURVE_KEY_TYPE_BASEPOSE)
      {
        pCurvePlayer->GetValue(valueList[curveIndex], (s32)m_CurrentFrame); // @attention UVアニメは整数フレームのみ
      }
    }
  }

  // テクスチャパターンアニメーションの反映
  for (u32 animSlotNo = 0; animSlotNo < pMaterialDataSet->m_TexturePatternAnimSlotCount; ++animSlotNo)
  {
    MaterialDataSet::SlotData* pSlotData = &pMaterialDataSet->m_SlotDataList[animSlotNo];

    // 前回のキーと同じか？
    if (pSlotData->m_pTexturePatternLastKey != NULL)
    {
      if ((pSlotData->m_pTexturePatternLastKey->frame <= m_CurrentFrame) && (m_CurrentFrame < pSlotData->m_TexturePatternLastKeyNextFrame))
      {
        pMaterialNode->SetTextureResourceNode(pSlotData->m_pTexturePatternAnimBody->textureSlotNo, pSlotData->m_pTexturePatternLastTexture);
        break;
      }
    }

    //@fix GFNMCat[5279] : キーが一つの場合に一回もループに入らないのでkeyIndexをsingedに変更
    for (s32 keyIndex = pSlotData->m_pTexturePatternAnimBody->keyCount - 1; keyIndex >= 0U; --keyIndex)
    {
      const scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody::Key* pKey = &pSlotData->m_pTexturePatternAnimBody->keyList[keyIndex];

      if (pKey->frame > m_CurrentFrame)
      {
        continue;
      }

      // テクスチャ名取得
      const scenegraph::resource::AnimationResourceNode::Name* pAnimTextureName = m_pTextureIdList;
      uptr texIdAddr = reinterpret_cast<uptr>(pAnimTextureName);
      for (u32 texId = 0; texId < pKey->value; ++texId)
      {
        texIdAddr += pAnimTextureName->length + sizeof(u8);
        pAnimTextureName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(texIdAddr);
      }

      // オリジナルのテクスチャから対応するテクスチャを探す
      for (u32 originalTextureNo = 0; originalTextureNo < pMaterialNode->GetTextureNumOriginal(); ++originalTextureNo)
      {
        const scenegraph::resource::TextureResourceNode* pTexture = pMaterialNode->GetTextureResourceNodeOriginal(originalTextureNo);

        if (!pAnimTextureName->IsSameName(pTexture->GetName()))
        {
          continue;
        }

        pMaterialNode->SetTextureResourceNode(pSlotData->m_pTexturePatternAnimBody->textureSlotNo, pTexture);

        // 今回の値を保存
        pSlotData->m_pTexturePatternLastKey = pKey;
        pSlotData->m_pTexturePatternLastTexture = pTexture;
        pSlotData->m_TexturePatternLastKeyNextFrame = (keyIndex < pSlotData->m_pTexturePatternAnimBody->keyCount - 1)
          ? (++pKey)->frame
          : 0xFFFF;
        break;
      }

      break;
    }
  }

  // シェーダーアニメーションの反映
  for (u32 animColorNo = 0; animColorNo < pMaterialDataSet->m_ColorAnimCount; ++animColorNo)
  {
    MaterialDataSet::ColorData* pColorData = &pMaterialDataSet->m_ColorDataList[animColorNo];

    gfx::ColorU8* pConstantColor = &pMaterialNode->GetAttributeParam()->m_ConstantColor[pColorData->m_pColorAnimBody->constantColorNo];

    u8* valueList[] =
    {
      &pConstantColor->r,
      &pConstantColor->g,
      &pConstantColor->b,
      &pConstantColor->a,
    };

    for (u32 curveIndex = 0; curveIndex < MaterialDataSet::COLOR_INDEX_COUNT; ++curveIndex)
    {
      CurvePlayer* pCurvePlayer = &pColorData->m_ColorCurvePlayerList[curveIndex];
      if (pCurvePlayer->m_type != CURVE_KEY_TYPE_BASEPOSE)
      {
        float       f32Value;
        pCurvePlayer->GetValue(&f32Value, m_CurrentFrame);
        f32Value = math::Clamp(f32Value, 0.0f, 1.0f); // u8にキャストするので0.0f～1.0fの範囲にクランプしておく
        *valueList[curveIndex] = static_cast<u8>(f32Value * 255.0f);
      }
    }
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void MaterialAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  // マテリアルが無いなら終了
  if (m_pModelNode->GetMaterialNum() == 0)
  {
    return;
  }

  GFL_ASSERT(pHeap != NULL);
  m_pMaterialDataSetList = GFL_NEW_ARRAY(pHeap) MaterialDataSet*[m_pModelNode->GetMaterialNum()];
  m_MaterialDataSetCount = m_pModelNode->GetMaterialNum();
  for (u32 i = 0; i < m_pModelNode->GetMaterialNum(); ++i)
  {
    m_pMaterialDataSetList[i] = NULL;
  }
}

/**
 * @brief 派生クラスでの終了
 */
void MaterialAnimationSlot::FinalizeDerived()
{
  // マテリアルデータセットリスト開放
  if (m_pMaterialDataSetList == NULL)
  {
    return;
  }

  for (u32 i = 0; i < m_MaterialDataSetCount; ++i)
  {
    GFL_SAFE_DELETE(m_pMaterialDataSetList[i]);
  }
  GFL_SAFE_DELETE_ARRAY(m_pMaterialDataSetList);
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void MaterialAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // マテリアルが無ければ無視
  if (m_pMaterialDataSetList == NULL)
  {
    return;
  }

  // ＵＶアニメーションセットアップ
  this->SetupUvAnimation(pHeap);

  // テクスチャパターンアニメーションセットアップ
  this->SetupTexturePatternAnimation(pHeap);

  // シェーダーアニメーションセットアップ
  this->SetupShaderAnimation(pHeap);
}

/**
 * @brief ＵＶアニメーションセットアップ
 *
 * @param pHeap ヒープ
 */
void MaterialAnimationSlot::SetupUvAnimation(gfl2::heap::HeapBase* pHeap)
{
  // 全テクスチャパターンアニメーション初期化
  for (u32 materialNo = 0; materialNo < m_pModelNode->GetMaterialNum(); ++materialNo)
  {
    MaterialDataSet* pMaterialDataSet = m_pMaterialDataSetList[materialNo];

    if (pMaterialDataSet == NULL)
    {
      continue;
    }

    pMaterialDataSet->m_UvAnimSlotCount = 0;
  }

  if (m_pResourceNode->m_pUvAnimHeader == NULL)
  {
    return;
  }

  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pUvAnimHeader)
    + sizeof(scenegraph::resource::AnimationResourceNode::UvAnimHeader) - sizeof(u32)
    + (sizeof(u32) * m_pResourceNode->m_pUvAnimHeader->materialCount);
  uptr bodyAddr = nameAddr + m_pResourceNode->m_pUvAnimHeader->nameBufferSize;
  for (u32 animMaterialNo = 0; animMaterialNo < m_pResourceNode->m_pUvAnimHeader->materialCount; ++animMaterialNo)
  {
    u32 animSlotCount = m_pResourceNode->m_pUvAnimHeader->textureSlotCountList[animMaterialNo];

    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // 対応するマテリアルを探す
    MaterialDataSet* pMaterialDataSet = this->CreateMaterialDataSet(pHeap, pName);
    if (pMaterialDataSet == NULL)
    {
      // 見つからない時はアニメーションデータがずれない様に空回し
      for (u32 animSlotNo = 0; animSlotNo < animSlotCount; ++animSlotNo)
      {
        const scenegraph::resource::AnimationResourceNode::UvAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::UvAnimBody*>(bodyAddr);
        bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::UvAnimBody) - sizeof(u32);
      }

      continue;
    }

    // スロット
    pMaterialDataSet->m_UvAnimSlotCount = animSlotCount;
    for (u32 animSlotNo = 0; animSlotNo < animSlotCount; ++animSlotNo)
    {
      // アニメーションデータ解決
      const scenegraph::resource::AnimationResourceNode::UvAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::UvAnimBody*>(bodyAddr);
      bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::UvAnimBody) - sizeof(u32);

      uptr curveAddr = reinterpret_cast<uptr>(pBody->data);

      MaterialDataSet::SlotData* pSlotData = &pMaterialDataSet->m_SlotDataList[animSlotNo];
      pSlotData->m_pUvAnimBody = pBody;

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < MaterialDataSet::UV_INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &pSlotData->m_UvCurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }
    }
  }
}

/**
 * @brief テクスチャパターンアニメーションセットアップ
 *
 * @param pHeap ヒープ
 */
void MaterialAnimationSlot::SetupTexturePatternAnimation(gfl2::heap::HeapBase* pHeap)
{
  // 全テクスチャパターンアニメーション初期化
  for (u32 materialNo = 0; materialNo < m_pModelNode->GetMaterialNum(); ++materialNo)
  {
    MaterialDataSet* pMaterialDataSet = m_pMaterialDataSetList[materialNo];

    if (pMaterialDataSet == NULL)
    {
      continue;
    }

    pMaterialDataSet->m_TexturePatternAnimSlotCount = 0;
  }

  if (m_pResourceNode->m_pTexturePatternAnimHeader == NULL)
  {
    return;
  }

  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pTexturePatternAnimHeader)
    + sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimHeader) - sizeof(u32)
    + (sizeof(u32) * m_pResourceNode->m_pTexturePatternAnimHeader->materialCount);
  uptr texIdAddr = nameAddr + m_pResourceNode->m_pTexturePatternAnimHeader->nameBufferSize;
  uptr bodyAddr = texIdAddr + m_pResourceNode->m_pTexturePatternAnimHeader->texIdBufferSize;

  // テクスチャＩＤリスト
  m_pTextureIdList = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(texIdAddr);
  for (u32 animMaterialNo = 0; animMaterialNo < m_pResourceNode->m_pTexturePatternAnimHeader->materialCount; ++animMaterialNo)
  {
    u32 animSlotCount = m_pResourceNode->m_pTexturePatternAnimHeader->textureSlotCountList[animMaterialNo];

    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // 対応するマテリアルを探す
    MaterialDataSet* pMaterialDataSet = this->CreateMaterialDataSet(pHeap, pName);
    if (pMaterialDataSet == NULL)
    {
      // 見つからない時はアニメーションデータがずれない様に空回し
      for (u32 animSlotNo = 0; animSlotNo < animSlotCount; ++animSlotNo)
      {
        const scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody*>(bodyAddr);
        bodyAddr += (pBody->keyCount * sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody::Key)) + sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody) - sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody::Key);
      }

      continue;
    }

    // スロット
    pMaterialDataSet->m_TexturePatternAnimSlotCount = animSlotCount;
    for (u32 animSlotNo = 0; animSlotNo < animSlotCount; ++animSlotNo)
    {
      // アニメーションデータ解決
      const scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody*>(bodyAddr);
      bodyAddr += (pBody->keyCount * sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody::Key)) + sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody) - sizeof(scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody::Key);

      MaterialDataSet::SlotData* pSlotData = &pMaterialDataSet->m_SlotDataList[animSlotNo];
      pSlotData->m_pTexturePatternAnimBody = pBody;

      pSlotData->m_pTexturePatternLastKey = NULL;
      pSlotData->m_pTexturePatternLastTexture = NULL;
    }
  }
}

/**
 * @brief シェーダーアニメーションセットアップ
 *
 * @param pHeap ヒープ
 */
void MaterialAnimationSlot::SetupShaderAnimation(gfl2::heap::HeapBase* pHeap)
{
  // 全コンスタントカラー初期化
  for (u32 materialNo = 0; materialNo < m_pModelNode->GetMaterialNum(); ++materialNo)
  {
    MaterialDataSet* pMaterialDataSet = m_pMaterialDataSetList[materialNo];

    if (pMaterialDataSet == NULL)
    {
      continue;
    }

    pMaterialDataSet->m_ColorAnimCount = 0;
  }

  if (m_pResourceNode->m_pShaderAnimHeader == NULL)
  {
    return;
  }

  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pShaderAnimHeader)
    + sizeof(scenegraph::resource::AnimationResourceNode::ShaderAnimHeader) - sizeof(u32)
    + (sizeof(u32) * m_pResourceNode->m_pShaderAnimHeader->materialCount);
  uptr bodyAddr = nameAddr + m_pResourceNode->m_pShaderAnimHeader->nameBufferSize;
  for (u32 animMaterialNo = 0; animMaterialNo < m_pResourceNode->m_pShaderAnimHeader->materialCount; ++animMaterialNo)
  {
    u32 animConstantColorCount = m_pResourceNode->m_pShaderAnimHeader->constantColorCountList[animMaterialNo];

    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // 対応するマテリアルを探す
    MaterialDataSet* pMaterialDataSet = this->CreateMaterialDataSet(pHeap, pName);
    if (pMaterialDataSet == NULL)
    {
      // 見つからない時はアニメーションデータがずれない様に空回し
      for (u32 animConstantColorNo = 0; animConstantColorNo < animConstantColorCount; ++animConstantColorNo)
      {
        const scenegraph::resource::AnimationResourceNode::ShaderAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::ShaderAnimBody*>(bodyAddr);
        bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::ShaderAnimBody) - sizeof(u32);
      }

      continue;
    }

    // コンスタントカラー
    pMaterialDataSet->m_ColorAnimCount = animConstantColorCount;
    for (u32 animConstantColorNo = 0; animConstantColorNo < animConstantColorCount; ++animConstantColorNo)
    {
      // アニメーションデータ解決
      const scenegraph::resource::AnimationResourceNode::ShaderAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::ShaderAnimBody*>(bodyAddr);
      bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::ShaderAnimBody) - sizeof(u32);

      uptr curveAddr = reinterpret_cast<uptr>(pBody->data);

      MaterialDataSet::ColorData* pColorData = &pMaterialDataSet->m_ColorDataList[animConstantColorNo];
      pColorData->m_pColorAnimBody = pBody;

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < MaterialDataSet::COLOR_INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &pColorData->m_ColorCurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }
    }
  }
}

/**
 * @brief 名前からマテリアルデータセットを検索
 *
 * @param pHeap ヒープ
 */
MaterialDataSet* MaterialAnimationSlot::CreateMaterialDataSet(gfl2::heap::HeapBase* pHeap, const scenegraph::resource::AnimationResourceNode::Name* pName)
{
  // 対応するマテリアルを探す
  for (u32 materialNo = 0; materialNo < m_pModelNode->GetMaterialNum(); ++materialNo)
  {
    if (pName->IsSameName(m_pModelNode->GetMaterialInstanceNode(materialNo)->GetName()))
    {
      if (m_pMaterialDataSetList[materialNo] == NULL)
      {
        // 一度でもAllocate()で確保されていたら動的確保を禁止するためにpHeapはNULLになる。
        // ここで引っかかる場合はAllocate()処理を見直す必要がある。
        GFL_ASSERT(pHeap != NULL);
        m_pMaterialDataSetList[materialNo] = GFL_NEW(pHeap) MaterialDataSet();
      }

      return m_pMaterialDataSetList[materialNo];
    }
  }

  return NULL;
}

}}