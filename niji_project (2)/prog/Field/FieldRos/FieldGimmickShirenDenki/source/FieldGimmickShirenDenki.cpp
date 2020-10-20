//=================================================================================================
/**
*  @file FieldGimmickShirenDenki.cpp
*  @brief フィールドギミック 電気試練
*  @author ikuta_junya
*  @date 2016.02.17
*/
//=================================================================================================

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

// field
#include "Field/FieldRos/FieldGimmickShirenDenki/include/FieldGimmickShirenDenki.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "arc_def_index/arc_def.h"

// reference
//#include <niji_reference_files/script/FieldPawnTypes.h>
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaScrollStopManager.h"

#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaScrollStopManager.h"

// フィールドレジデント
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// @todo ヒープ分割を行う
#include "niji_conv_header/field/gimmick/shiren_denki/gimmick_shiren_denki_mot.h"

#include "Field/FieldRo/include/Collision/BaseShape.h"
GFL_NAMESPACE_BEGIN(Field)


static const gfl2::math::Vector3 KUWAGA_COL_POS_01(722.455f, 0.0f, 587.377f);
static const gfl2::math::Vector3 KUWAGA_COL_SIZE_01(70.0f, 100.0f, 120.0f);
static const gfl2::math::Vector3 KUWAGA_COL_POS_02(722.455f, 0.0f, 587.377f + 80.0f );
static const gfl2::math::Vector3 KUWAGA_COL_SIZE_02(70.0f - 25.0f, 100.0f, 120.0f );

class FieldGimmickShirenDenkiCameraCallBack : public Camera::Controller::ControllerTypeAreaCallBack
{
public:
  
  FieldGimmickShirenDenkiCameraCallBack( Fieldmap *pFieldmap )
  {
    m_pFieldmap = pFieldmap;
  }

  virtual void SetAfter(Camera::Controller::ControllerTypeArea* pThis )
  {
    if ( pThis->GetNowAreaDataNo() == 1 )
    {
      m_pFieldmap->GetCameraManager()->GetScrollStopManager()->SetInDataInvalidFlag(1, true );
    }
    else
    {
      m_pFieldmap->GetCameraManager()->GetScrollStopManager()->SetInDataInvalidFlag(1, false);
    }
  }

  virtual void AreaChangeAfter(Camera::Controller::ControllerTypeArea* pThis, u32 areaDataNo)
  {
    if ( areaDataNo == 1 )
    {
      m_pFieldmap->GetCameraManager()->GetScrollStopManager()->SetInDataInvalidFlag(1, true);
    }
    else
    {
      m_pFieldmap->GetCameraManager()->GetScrollStopManager()->SetInDataInvalidFlag(1, false);
    }
  }

  /**
   *  @brief  内部補間前処理
  */
  virtual void InSideInterpolationPrev(Camera::Controller::ControllerTypeArea* pThis, gfl2::math::Vector3* pCamera, gfl2::math::Vector3* pTarget)
  {
    //m_pFieldmap->GetCameraManager()->GetScrollStopManager()
    gfl2::math::Vector3 workTargetPosition( *pTarget );
    if (m_pFieldmap->GetCameraManager()->GetScrollStopManager()->Execute( *pTarget, pTarget))
    {
      *pCamera += (*pTarget - workTargetPosition);
    }
    else
    {
    }
  }


private:
  Fieldmap *m_pFieldmap;
};

/**
 *  @brief コンストラクタ
 *  @param pHeap ヒープ
 *  @param pFieldmap フィールドマップ
 */
FieldGimmickShirenDenki::FieldGimmickShirenDenki(gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap)
  : FieldGimmick(pFieldmap), m_InitializeState(0), m_FinalizeState(0), m_pFieldDeviceHeap(pHeap), m_pFileReadManager(NULL), m_pModelResource(NULL), m_pModel(NULL), m_pModelActor(NULL)

  // @fix cov_ctr[12531]:初期化追加
, m_pResourceBuff( NULL )
{
  m_pAllocator = GFL_NEW(pHeap) System::nijiAllocator(pHeap);
  m_pCameraCallBack = GFL_NEW(pHeap) FieldGimmickShirenDenkiCameraCallBack( pFieldmap );
}

/**
 *  @brief デストラクタ
 */
FieldGimmickShirenDenki::~FieldGimmickShirenDenki()
{
  GFL_SAFE_DELETE(m_pCameraCallBack);
  GFL_SAFE_DELETE(m_pAllocator);
}

/**
 *  @brief  生成
 */
void FieldGimmickShirenDenki::Create(void)
{
  //Param
  {
    m_pFileReadManager = m_pFieldmap->GetGameManager()->GetAsyncFileManager();
  }

  //モンスターボール：リソースだけ事前準備
  {
    // ARCファイルオープン
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      {
        openReq.arcId = ARCID_FIELD_GIMMICK_SHIREN_DENKI;
        openReq.heapForFile = m_pFieldDeviceHeap;
        openReq.heapForArcSrc = m_pFieldDeviceHeap;
        openReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
      }

      m_pFileReadManager->AddArcFileOpenReq(openReq);
    }

    // ファイル読み込みリクエスト
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      {
        loadReq.arcId = ARCID_FIELD_GIMMICK_SHIREN_DENKI;
        loadReq.datId = 0;
        loadReq.ppBuf = &m_pResourceBuff;
        loadReq.heapForBuf = m_pFieldDeviceHeap;
        loadReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
        loadReq.heapForCompressed = m_pFieldDeviceHeap->GetLowerHandle();
        loadReq.pBufSize = NULL;
        loadReq.align = 128;
      }

      m_pFileReadManager->AddArcFileLoadDataReq(loadReq);
    }
  }

  // エフェクト
  {
    Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
    pEffectManager->LoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01, m_pFieldDeviceHeap );
    pEffectManager->LoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02, m_pFieldDeviceHeap );
  }

  m_InitializeState = 0;

}
void FieldGimmickShirenDenki::UpdateCreate(void)
{
  switch (m_InitializeState)
  {
  case 0:
  {
    // 読み込み完了待ち
    if (!m_pFileReadManager->IsArcFileOpenFinished(ARCID_FIELD_GIMMICK_SHIREN_DENKI))
    {
      break;
    }
    if (!m_pFileReadManager->IsArcFileLoadDataFinished(&m_pResourceBuff)){
      break;
    }

    // エフェクトセットアップ
    {
      Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
      if (!pEffectManager->IsLoadedEffectResource(Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01))
      {
        break;
      }
      if (!pEffectManager->IsLoadedEffectResource(Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02))
      {
        break;
      }
    }

    m_BinLinker.Initialize(m_pResourceBuff);
    gfl2::fs::BinLinkerAccessor binWork;
    binWork.Initialize(m_BinLinker.GetData(0));

    // モデル作成
    {

      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
      resourceData.SetModelData(static_cast<c8*>(binWork.GetData(0)));
      m_pModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &resourceData);
    }

    // モーション作成
    {
      c8* pAnime = reinterpret_cast<c8*>(binWork.GetData(1));
      m_AnimPackList.Initialize(m_pFieldDeviceHeap, 1);
      m_AnimPackList.LoadData(0, m_pAllocator, m_pFieldDeviceHeap, pAnime);
    }

    // モデル生成
    {
      m_pModel = GFL_NEW(m_pFieldDeviceHeap) poke_3d::model::BaseModel();
      m_pModel->Create(m_pAllocator, m_pFieldDeviceHeap, m_pModelResource);
      m_pModel->SetPosition(gfl2::math::Vector3(0.0f, 0.0f, 0.0f));

      //モーション設定
      m_pModel->SetAnimationPackList(&m_AnimPackList);

      //リソース登録
      //GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->AddResourceChild(m_pModelResource_Birthday[i]);

      //描画登録
      m_pFieldmap->GetUpperRenderingPipeLine()->AddEdgeRenderingTarget(m_pModel->GetModelInstanceNode());
    }

    // コリジョン生成
    {
      gfl2::math::Vector3 pos( 722.455f, 0.0f, 587.377f);
      gfl2::math::Vector3 size( 70.0f, 100.0f, 120.0f );
      gfl2::math::Vector3 x( 1.0f, 0.0f, 0.0f );
      gfl2::math::Vector3 z( 0.0f, 0.0f, 1.0f );
      gfl2::math::Quaternion quaternion( 0.0f, 0.0f, 0.0f, 1.0f );

      BaseCollisionScene* pScene = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetCollisionScene(FieldResident::COLLISION_SCENE_STATIC_MODEL);
      m_pModelActor = pScene->CreateStaticActor(m_pAllocator, m_pFieldDeviceHeap);
      m_pModelActor->CreateBox(pos, x, z, size.x, size.y, size.z );
      pScene->RegistStaticActor(m_pModelActor);
    }

    // エフェクトセットアップ
    {
      Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
      pEffectManager->SetupDynamicEffectResource(Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01, m_pFieldDeviceHeap);
      pEffectManager->SetupDynamicEffectResource(Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02, m_pFieldDeviceHeap);
    }

    m_InitializeState = 1;
  } // case 0
  }
}
bool FieldGimmickShirenDenki::IsCreated(void)
{
  return ( m_InitializeState == 1);
}

/**
*  @brief  Ready中1度だけ呼び出し
*/
void FieldGimmickShirenDenki::Ready(void)
{
  Field::Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Field::Camera::Controller::ControllerTypeArea* pAreaController = pCameraManager->GetAreaController();
  pAreaController->SetCallBack( m_pCameraCallBack );
}

/**
 *  @biref 破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickShirenDenki::Finalize(void)
{
  switch(m_FinalizeState)
  {
    case 0:
    {
      Field::Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
      Field::Camera::Controller::ControllerTypeArea* pAreaController = pCameraManager->GetAreaController();
      pAreaController->SetCallBack(NULL);

      // 描画解除
      if (m_pModel != NULL)
      {
        m_pFieldmap->GetUpperRenderingPipeLine()->RemoveEdgeRenderingTarget(m_pModel->GetModelInstanceNode());
      }

      // コリジョン破棄
      {
        if (m_pModelActor)
        {
          BaseCollisionScene* pScene = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetCollisionScene(FieldResident::COLLISION_SCENE_STATIC_MODEL);
          pScene->UnRegistStaticActor(m_pModelActor);
          GFL_SAFE_DELETE(m_pModelActor);
        }
      }

      //ARCファイルクローズ
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = ARCID_FIELD_GIMMICK_SHIREN_DENKI;
        closeReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
        m_pFileReadManager->SyncArcFileClose(closeReq);
      }

      m_FinalizeState = 1;
    }
    case 1:
    {
      if (m_pModel)
      {
        if (m_pModel->CanDestroy())
        {
          GFL_SAFE_DELETE(m_pModel);
          GFL_SAFE_DELETE(m_pModelResource);
          m_AnimPackList.Finalize();
        }
        else
        {
          break;
        }
      }

      // 読み込んだファイルの削除
      if(m_pResourceBuff)
      {
        GflHeapFreeMemory(m_pResourceBuff);
        m_pResourceBuff = NULL;
      }

      if (!m_pFileReadManager->IsArcFileCloseFinished(ARCID_FIELD_GIMMICK_SHIREN_DENKI))
      {
        break;
      }
      
      m_FinalizeState = 2;
    }
   
    case 2:
    {
      return true;
    }
  }

  return false;
}

/**
 *  @brief 更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickShirenDenki::UpdateLast(void)
{
  // アニメーション更新
  if (m_pModel)
  {
    m_pModel->UpdateAnimation();
  }

  // コリジョン形状更新
  if (m_pModelActor && m_pModel)
  {
    // @fix MMcat[286]:暴走後のクワガノンの黄色い突起部分に自機がめり込む、対処。クワガノンモデルのモーションが暴走になっている場合、コリジョンを変更するように修正
    if (m_pModel->GetAnimationId() == F0301I1304_BM_OBJ01_EVENT01)
    {
      BoxShape * shape = static_cast<BoxShape*>(m_pModelActor->GetShape());
      shape->m_pos = KUWAGA_COL_POS_02;
      shape->m_e0 = KUWAGA_COL_SIZE_02.x;
      shape->m_e1 = KUWAGA_COL_SIZE_02.y;
      shape->m_height = KUWAGA_COL_SIZE_02.z;
    }
    else
    {
      BoxShape * shape = static_cast<BoxShape*>(m_pModelActor->GetShape());
      shape->m_pos = KUWAGA_COL_POS_01;
      shape->m_e0 = KUWAGA_COL_SIZE_01.x;
      shape->m_e1 = KUWAGA_COL_SIZE_01.y;
      shape->m_height = KUWAGA_COL_SIZE_01.z;
    }
  }
}


GFL_NAMESPACE_END(Field)