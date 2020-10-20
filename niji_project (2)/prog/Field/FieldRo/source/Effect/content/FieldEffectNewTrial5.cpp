//======================================================================
/**
 * @file FieldEffectNewTrial5.cpp
 * @date 2016/12/13 21:39:39
 * @author kawazoe_shinichi
 * @brief 電気試練用エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "PokeTool/include/PokeModel.h"

// niji
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptTrialModel.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/Fieldmap.h"
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include "Field/FieldRos/FieldGimmickShirenDenki/include/FieldGimmickShirenDenki.h"
// ref
#include "niji_reference_files/script/FieldPawnTypes.h"



GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @brief  コンストラクタ
*/
EffectNewTrial5::EffectNewTrial5(void)
: IEffectBase()
, m_pEffectHeap     (NULL)
, m_pEffectSystem   (NULL)
, m_pEffectHandle   (NULL)
, m_EffectResorceID (-1)
, m_Idx(-1)
// @fix cov_ctr[12525]:未初期化変数対応
, m_pParentObject( NULL )
{
}

/**
*  @brief  デストラクタ
*/
EffectNewTrial5::~EffectNewTrial5(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  初期化
*/
void EffectNewTrial5::Initialize(SetupData &setupData)
{
  m_vPosition         = setupData.position;
  m_pEffectHeap       = setupData.pEffectHeap;
  m_pEffectSystem     = setupData.pEffectSystem;
  m_pEffectHandle     = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle;  
  m_pParentObject     = setupData.pParentObject;
}

/**
*  @brief  破棄
*/
bool EffectNewTrial5::Terminate(void)
{
  End();
  GFL_SAFE_DELETE(m_pEffectHandle);
  if (m_pEffectSystem && m_pEffectHeap)
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap   = NULL;
  }
  return true;
}

/**
*  @brief  エフェクトを再生する
*/
void EffectNewTrial5::CreateEffect(s32 idx)
{
  // 再生済みを破棄する
  if (m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive())
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
    m_pEffectHandle->Invalidate();
  }

  m_Idx = idx;

  switch (idx)
  {
  case Field::FieldScript::EFF_S5_LINE:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_01_LINE;
    break;
  case Field::FieldScript::EFF_S5_STRONG_LINE:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_02_STR_LINE;
    break;
  case Field::FieldScript::EFF_S5_STRONG_SLANT:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_03_STR_SLANT;
    break;
  case Field::FieldScript::EFF_S5_Z:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_Z;
    break;
  case Field::FieldScript::EFF_S5_KUWA_CHARGE:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CHARGE;
    break;
  case Field::FieldScript::EFF_S5_KUWA_CHARGE_WEAK:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CHARGE;
    break;

  case Field::FieldScript::EFF_S5_TOGE_CHARGE:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_CHARGE;
    break;
  case Field::FieldScript::EFF_S5_NUSHI_CHARGE:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_CHARGE;
    break;
  case Field::FieldScript::EFF_S5_TOGE_END:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_END;
    break;
  case Field::FieldScript::EFF_S5_NUSHI_END:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_END;
    break;
  case Field::FieldScript::EFF_S5_WEAK_SLANT:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_04_WEAK_SLANT;
    break;
  case Field::FieldScript::EFF_S5_KUWA_CRASH:
    m_EffectResorceID = EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CRASH;
    break;
  default:
    m_EffectResorceID = -1;
  }

  if (m_EffectResorceID != -1)
  {
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, m_EffectResorceID);
  }
  
}

/**
*  @brief  再生
*/
void EffectNewTrial5::Start(void)
{
  if (!IsAlive() && (m_EffectResorceID != -1))
  {
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, m_EffectResorceID);
  }
}

/**
*  @brief  停止
*/
void EffectNewTrial5::End(void)
{
  if (m_pEffectHandle->GetEmitterSet())
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

  /**
  *  @brief  停止
  */
void EffectNewTrial5::Fade(void)
{
  if (m_pEffectHandle->GetEmitterSet())
  {
    m_pEffectHandle->GetEmitterSet()->Fade();
  }
}

/**
*  @brief  再生確認
*/
bool EffectNewTrial5::IsAlive(void)
{
  if (!m_pEffectHandle->GetEmitterSet())             { return false; }
  if (!m_pEffectHandle->GetEmitterSet()->IsAlive())  { return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  更新処理
*/
void EffectNewTrial5::updateNormal(void)
{
  switch (m_Idx)
  {
  case Field::FieldScript::EFF_S5_KUWA_CHARGE_WEAK:
  case Field::FieldScript::EFF_S5_KUWA_CHARGE:
  {
    GameSys::GameManager * pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
    FieldGimmickShirenDenki* gimmick_shiren_denki = static_cast<FieldGimmickShirenDenki*>(gimmick);

    if (gimmick_shiren_denki)
    {
      poke_3d::model::BaseModel* pModel = gimmick_shiren_denki->GetModel(0); // @note 決めうち注意
      if (pModel)
      {
        s32 joint_index = pModel->GetJointIndex("kuwa03");
        if (joint_index != -1)
        {
          f32 scale = 1.0f;

          if (m_Idx == Field::FieldScript::EFF_S5_KUWA_CHARGE_WEAK)
          {
            scale = 0.5f;
          }

          gfl2::math::Vector3 pos(0.0f, -50.3f, -128.0f);
          gfl2::math::Matrix34 mat = pModel->GetJointWorldMatrix( joint_index );
          pos = mat.TransformCoord( pos );
          setPosition(pos, scale);
        }
      }
    }
    break;
  }

  case Field::FieldScript::EFF_S5_KUWA_CRASH:
  {
    GameSys::GameManager * pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
    FieldGimmickShirenDenki* gimmick_shiren_denki = static_cast<FieldGimmickShirenDenki*>(gimmick);

    if (gimmick_shiren_denki)
    {
      poke_3d::model::BaseModel* pModel = gimmick_shiren_denki->GetModel(0); // @note 決めうち注意
      if (pModel)
      {
        s32 joint_index = pModel->GetJointIndex("kuwa03");
        if (joint_index != -1)
        {
          gfl2::math::Vector3 pos(0.0f, 0.0f, 0.0f);
          gfl2::math::Matrix34 mat = pModel->GetJointWorldMatrix( joint_index );
          pos = mat.TransformCoord( pos );
          setPosition(pos);
        }
      }
    }
    break;
  }

  case Field::FieldScript::EFF_S5_TOGE_END:
  case Field::FieldScript::EFF_S5_TOGE_CHARGE:
  {
    Field::IField3DObjectNode *pParent = m_pParentObject;
    if (pParent->SafeCast<Field::MoveModel::FieldMoveModel>() != NULL)
    {
      Field::MoveModel::FieldMoveModel *pMoveModel = pParent->SafeCast<Field::MoveModel::FieldMoveModel>();
      poke_3d::model::BaseModel *pBaseModel = pMoveModel->GetCharaDrawInstance();
      if (pBaseModel)
      {
        s32 joint_index = pBaseModel->GetJointIndex("Waist");
        if (joint_index != -1)
        {
          gfl2::math::Vector3 pos = pBaseModel->GetJointWorldPosition(joint_index);
          setPosition(pos, 0.5f);
        }
      }
      else
      {
        // m_SRT.scale = gfl2::math::Vector3::GetZero();
      }
    }
    break;
  }

  case  Field::FieldScript::EFF_S5_NUSHI_END:
  case  Field::FieldScript::EFF_S5_NUSHI_CHARGE:
  {
    GameSys::GameManager * p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    Field::FieldScript::FieldScriptSystem *p_sys = p_gman->GetFieldScriptSystem();

    Field::TrialModel::FieldTrialModel* pTrialModel = p_sys->GetScriptTrialModel()->GetTrialMode(0);
    if (pTrialModel)
    {
      s32 joint_index = pTrialModel->GetPokeModel()->GetJointIndex("Waist");
#if 0
      s32 jointCount = p_sys->GetScriptTrialModel()->GetTrialMode(0)->GetPokeModel()->GetModelInstanceNode()->GetJointNum();
      for (u32 i = 0; i < jointCount; ++i)
      {
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJoint = p_sys->GetScriptTrialModel()->GetTrialMode(0)->GetPokeModel()->GetModelInstanceNode()->GetJointInstanceNode(i);

        //if (hash != pJoint->GetNameHash())
        //{
        //  continue;
        //}
        GFL_PRINT("%s\n", pJoint->GetName());
      }
#endif
      if (joint_index != -1)
      {
        gfl2::math::Vector3 pos = p_sys->GetScriptTrialModel()->GetTrialMode(0)->GetPokeModel()->GetJointWorldPosition(joint_index);
        setPosition(pos, 0.5f * 2.7f);
      }
    }
    break;
  }

  }

}

void EffectNewTrial5::setPosition(const gfl2::math::Vector3& rPos, f32 scale )
{
  // エミッター
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    // trans
    gfl2::math::Vector3 glPos( rPos );
    nw::math::VEC3      nwPos( glPos.x, glPos.y, glPos.z );

    // rotate
    f32 x, y, z;
    x = y = z = 0.0f;
    // quat.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot  ( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(scale,scale,scale);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
