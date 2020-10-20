//======================================================================
/**
* @file FieldEffectFureaiIconBase.cpp
* @date 2017/02/13 13:07:19
* @author kawazoe_shinichi
* @brief ふれあい表現用のアイコンエフェクト
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// gfl2
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>
// sound
#include "Sound/include/Sound.h"
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectFureaiIconBase.h"
#include "niji_conv_header/field/effect/exclamation_icon1.h"
// ref
#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  コンストラクタ
 */
 EffectFureaiIconBase::EffectFureaiIconBase(void)
 :EffectBallonIcon()
 , m_pMotionResourceRootNode_Open(NULL)
 , m_pMotionResourceRootNode_Close(NULL)
 , m_type(0)
 , m_eCurrentState(STATE_NONE)
 , m_eMode(MODE_NORMAL)
 , m_FrameCount(0)
 , m_MaxWaitFrame(24)
 , m_IsVisible(true)
{
  m_nSeID = 0;
}

/**
 *  @brief  デストラクタ
 */
EffectFureaiIconBase::~EffectFureaiIconBase(void)
{
}

/**
 *  @brief  初期化
 */
void EffectFureaiIconBase::Initialize(EffectFureaiIconBase::SetupData &setupData)
{
  m_pFieldRootNode = setupData.baseSetup.pRootNode;
  m_pCameraManager = setupData.baseSetup.pCameraManager;
  m_pRenderingPipeLine = setupData.baseSetup.pPipeLine;
  m_bPosMode = setupData.baseSetup.bPosMode;
  m_vPosition = setupData.baseSetup.vPosition;

  // アロケータ作成
  m_pAllocator = GFL_NEW(setupData.baseSetup.pHeap) gfl2::util::GLHeapAllocator(setupData.baseSetup.pHeap);
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize(setupData.baseSetup.pResource);
  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData(GetModelDataID());
  createModel(pModelData);
  m_Model.Create(m_pAllocator, setupData.baseSetup.pHeap, m_pModelInstanceNode);
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData(GetMotionDataID());
  void *pOpen = binLinkerAccessor.GetData(BL_IDX_EXCLAMATION_ICON1_MAP_OPEN_GFBMOT);
  void *pClose = binLinkerAccessor.GetData(BL_IDX_EXCLAMATION_ICON1_MAP_CLOSE_GFBMOT);
  createMotion(pMotionData);
  createMotion2(pOpen, pClose);
  // ジョイント
  jointParent(setupData.baseSetup.pParentObject);
  // 描画登録
  m_pFieldRootNode->AddAfterTransChild(m_pModelInstanceNode);
  m_pRenderingPipeLine->AddEdgeRenderingTarget(m_pModelInstanceNode);

  m_type = setupData.type;
  // Se
  if (IsPlaySe() && setupData.type != 0)
  {
    switch (setupData.type)
    {
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1: Sound::PlaySE(SEQ_SE_FLD_0428); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2: Sound::PlaySE(SEQ_SE_FLD_0428); break; // @note 確認
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_HEART1: Sound::PlaySE(SEQ_SE_FLD_0430); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1: Sound::PlaySE(SEQ_SE_FLD_0443); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1: Sound::PlaySE(SEQ_SE_FLD_215); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1: Sound::PlaySE(SEQ_SE_FLD_0429); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_TEN1: Sound::PlaySE(SEQ_SE_FLD_0427); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1: Sound::PlaySE(SEQ_SE_FLD_0431); break;
    }
  }

  m_IsVisible = true;
}

/**
 *  @brief  破棄
 */
bool EffectFureaiIconBase::Terminate(void)
{
  m_pFieldRootNode->RemoveResourceChild(m_pMotionResourceRootNode_Open);
  GFL_SAFE_DELETE(m_pMotionResourceRootNode_Open);
  m_pFieldRootNode->RemoveResourceChild(m_pMotionResourceRootNode_Close);
  GFL_SAFE_DELETE(m_pMotionResourceRootNode_Close);
  return EffectBallonIcon::Terminate();
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectFureaiIconBase::Delete(void)
{
  if (m_type == Field::Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1)
  {
    Sound::StopSE(SEQ_SE_FLD_0431);
  }

  EffectBallonIcon::Delete();
}

/**
 *  @brief  更新
 */
void EffectFureaiIconBase::updateNormal(void)
{
  // @fix MMcat[209]:昼寝アイコンが画面奥の地形内に表示される、対処。ポケモンが自機から遠いorカリング中の際は[ZZZ]アイコンを非表示にする
  if (m_IsVisible != m_Model.IsVisible() )
  {
    m_Model.SetVisible(m_IsVisible);

    // ONになった瞬間は0フレームから再生開始
    if (m_IsVisible)
    {
      if (m_eCurrentState == STATE_OPEN || m_eCurrentState == STATE_WAIT_FOR_ZZZ)
      {
        m_Model.SetAnimationStepFrame(1.0f);
        m_Model.SetAnimationFrame(0.0);
        m_FrameCount = 0;
      }
    }
  }

  f32 currentAnimFrame  = m_Model.GetAnimationFrame();
  f32 maxAnimeFrame     = m_Model.GetAnimationEndFrame();
  b32 isDelReq = false;
  switch (m_eCurrentState)
  {
  case STATE_NONE:  // 初期状態
    
    m_eCurrentState = STATE_OPEN;
    m_Model.ChangeAnimationByResourceNode(m_pMotionResourceRootNode);
    break;
  case STATE_OPEN:  // アイコン出現モーション中
    
    if (maxAnimeFrame <= currentAnimFrame + 0.1f)
    {
      if (m_eMode == MODE_NORMAL)
      {
        // 通常再生の場合は通常待機へ
        m_eCurrentState = STATE_WAIT;
        m_Model.SetAnimationStepFrame(0.0f);    // 一時停止
        m_FrameCount = 0;
      }
      else
      {
        if ( m_type == Field::Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1 )
        {
          m_eCurrentState = STATE_WAIT_FOR_ZZZ;
        }
        else
        {
          // アイコンを表示状態でキープする場合はリクエスト待ちの待機へ
          m_eCurrentState = STATE_WAIT_FOR_REQUEST;
          m_Model.SetAnimationStepFrame(0.0f);    // 一時停止
          m_FrameCount = 0;
        }
      }
    }
    break;
  case STATE_WAIT:  // 指定フレーム分待機中
    if ((m_MaxWaitFrame < m_FrameCount) || (m_eMode == MODE_CLOSE))
    {
      RequestClose();
    }
    m_FrameCount++;
    break;
  case STATE_WAIT_FOR_REQUEST:  // Closeモードへの変更待ち
    if (m_eMode == MODE_CLOSE)
    {
      RequestClose();
    }
    break;
  case STATE_WAIT_FOR_ZZZ:
    {
      if (m_FrameCount > 30 * 1.5 + 10 )
      {
        if (m_eMode == MODE_CLOSE)
        {
          RequestClose();
        }
        else
        {
          if (IsPlaySe())
          {
            Sound::PlaySE(SEQ_SE_FLD_0431);
          }
          m_Model.SetAnimationStepFrame(1.0f);
          m_Model.SetAnimationFrame(1.0);
          m_FrameCount = 0;
        }
      }
      else
      {
        m_FrameCount++;
      }
      break;
    }
  }

  EffectBallonIcon::updateNormal();

  if ( m_eCurrentState == STATE_END && IsAnimationLastFrame())
  {
    // 再生終了したので自滅する
    RequestDelete();
  }
}


/**
*  @brief  スクリプト用のステート取得
*/
u32 EffectFureaiIconBase::GetStateForScript(void)
{
  u32 state = STATE_NONE;
  switch (m_eCurrentState)
  {
  case STATE_OPEN:
    state = Field::FieldScript::EFF_F_ICON_STATE_OPEN;
    break;

  case STATE_WAIT:
  case STATE_WAIT_FOR_REQUEST:
    state = Field::FieldScript::EFF_F_ICON_STATE_KEEP;
    break;

  case STATE_END:
    state = Field::FieldScript::EFF_F_ICON_STATE_CLOSE;
    break;
  }

  return state;
}

/**
*  @brief  Closeリクエスト
*/
void EffectFureaiIconBase::RequestClose(void)
{
  if (m_eCurrentState == STATE_END){ return; }

  m_Model.ChangeAnimationByResourceNode(m_pMotionResourceRootNode_Close);
  m_Model.SetAnimationFrame(0.0f);
  m_eCurrentState = STATE_END;
}

// リソースID取得
u32 EffectFureaiIconBase::GetModelDataID(void)
{
  return BL_IDX_EXCLAMATION_ICON1_MAP_CTR_GFBMDLP;
}
u32 EffectFureaiIconBase::GetMotionDataID(void)
{
  return BL_IDX_EXCLAMATION_ICON1_MAP_GFBMOT;
}

/**
 *  @brief  モーション作成
 */
void EffectFureaiIconBase::createMotion2(void *pOpen, void *pClose)
{
  // Open
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( static_cast<c8*>( pOpen ));
    m_pMotionResourceRootNode_Open =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
    m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode_Open );
  }
  // Close
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( static_cast<c8*>( pClose ));
    m_pMotionResourceRootNode_Close =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
    m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode_Close );
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
