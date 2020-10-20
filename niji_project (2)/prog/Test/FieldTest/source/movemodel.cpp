//=================================================================================================
/**
 * @file  movemodel.cpp
 * @brief
 * @date 2014.02.24
 * @author tamada@gamefreak.co.jp
 */
//=================================================================================================
#include "../include/movemodel.h"
#include "../include/field_transform.h"
#include "../include/movemodel_define.h"
#include "../include/movemodel_manager.h"
#include "../include/chara_rsc_manager.h"
#include "../include/FieldActionCmdBase.h"
#include "../include/FieldActionCmdManager.h"

#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Animation/include/gfl2_AnimationController.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include "GameSys/include/GameManager.h"
#include "test/FieldTest/include/FieldSystem.h"
#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"
#include "test/FieldTest/include/FieldTestUtility.h"

// 新コリジョンシステム
#include "../include/Collision/BaseActor.h"
#include "../include/Collision/StaticActor.h"
#include "../include/Collision/DynamicActor.h"
#include "../include/Collision/CollisionSolver3D.h"

using namespace gfl2::renderingengine;
using namespace gfl2::animation;

namespace fld{ namespace mmodel {

static bool IKCallback(gfl2::math::Vector3* out_pPosition, gfl2::math::Vector3* out_pNormal, const gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode, void* pUserData)
{
  const c8* pNodeName = pJointNode->GetNodeName();
  if (strcmp(pNodeName, "LFoot") == 0)
  {
    return false;
  }

  *out_pPosition = pJointNode->GetWorldMatrix().GetElemPosition();
  //out_pPosition->y = 10.0f;

  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
MoveModel::MoveModel( gfl2::heap::HeapBase * p_heap )
: m_exists(false)
, m_isRegistSceneGraph(false)
, m_animeNo(-1)
, m_MoveBit(0)
, m_pMotionConfigResourceRootNode(NULL)
, m_pModelInstanceNode(NULL)
, m_pMouthMotionResourceRootNode( NULL )
, m_pEyeMotionResourceRootNode( NULL )
, m_pMotionConfigInstanceNode(NULL)
, m_pStaticCollisionResourceRootNode(NULL)
, m_animationUpdateFrame(1)
, m_animationUpdateCnt(1)
, m_animationStepFrame(1.0f)
, m_floorNormal(0.0f,1.0f,0.0f)
, m_pParent(NULL)
, m_ParentJointIndex(-1)
, m_lod(LOD_NONE)
{

  // @todo 処理負荷計測用一時処理(ごめんなさい)、ずっと補間する
  m_IsInterpLoop = false;

  m_transform = GFL_NEW( p_heap ) FieldTransform;
  
  for( u32 i=0 ; i<LOD_MAX ; i++ )
  {
    m_pModelResourceRootNode[i] = NULL;
  }
  for( u32 i=0 ; i<chara::MOTION_ID_MAX ; i++ )
  {
    m_pMotionResourceRootNode[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
MoveModel::~MoveModel()
{
  GFL_SAFE_DELETE( m_transform );
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  p_header  リクエスト
*  @return 動作コード
*/
//-----------------------------------------------------------------------------
bool MoveModel::Setup( const MoveModelHeader * p_header )
{
  // リクエストを保持
  m_Header      = *p_header;
  m_exists      = true;
  m_isVisible   = true;
  m_isAnimation = true;
  m_isInterest  = false;
  m_isIk        = false;

  // モデルリソースの読み込み
  m_pModelResourceRootNode[LOD_LOW]  = RscManager::getInstance()->GetLowModelResource ( m_Header.rsc_id );  
  m_pModelResourceRootNode[LOD_HIGH] = RscManager::getInstance()->GetHighModelResource( m_Header.rsc_id );  

  // モーションリソースの読み込み
  m_pMotionResourceRootNode[chara::MOTION_ID_WAIT]        = RscManager::getInstance()->GetMotionResource( m_Header.rsc_id , chara::MOTION_ID_WAIT );
  m_pMotionResourceRootNode[chara::MOTION_ID_WALK]        = RscManager::getInstance()->GetMotionResource( m_Header.rsc_id , chara::MOTION_ID_WALK );
  m_pMotionResourceRootNode[chara::MOTION_ID_WALK_FAST]   = RscManager::getInstance()->GetMotionResource( m_Header.rsc_id , chara::MOTION_ID_WALK_FAST );
  m_pMotionResourceRootNode[chara::MOTION_ID_RUN]         = RscManager::getInstance()->GetMotionResource( m_Header.rsc_id , chara::MOTION_ID_RUN );
  m_pMotionResourceRootNode[chara::MOTION_ID_TSURI_START] = RscManager::getInstance()->GetMotionResource( m_Header.rsc_id , chara::MOTION_ID_TSURI_START );

  // 口パク 
  m_pMouthMotionResourceRootNode = RscManager::getInstance()->GetMouthMotionResource( m_Header.rsc_id );

  // 目パチ
  m_pEyeMotionResourceRootNode = RscManager::getInstance()->GetEyeMotionResource( m_Header.rsc_id );

  // モーションコンフィグリソースの読み込み
  m_pMotionConfigResourceRootNode = RscManager::getInstance()->GetMotionConfigResource( m_Header.rsc_id , chara::MOTION_ID_RUN );


  // 最初はHIGHモデルで表示
  requestChangeLod( LOD_HIGH );
  UpdateLod();

  // 出現時は待機モーションにする
  if( m_pMotionResourceRootNode[chara::MOTION_ID_WAIT] )
  {
    ChangeAnimation(chara::MOTION_ID_WAIT,true,0.0f,1.0f);
  }

  // 衝突されるコリジョン
  m_pStaticCollisionResourceRootNode = RscManager::getInstance()->GetStaticCollisionResource( m_Header.rsc_id );
  if( m_pStaticCollisionResourceRootNode )
  {
    gfl2::heap::HeapBase *pAppHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
    m_pStaticCollisionModel = GFL_NEW(pAppHeap) gfl2::collision::CollisionModel();
    m_pStaticCollisionModel->Initialize( m_pStaticCollisionResourceRootNode );
  }

  // スケール
  m_transform->SetScale( gfl2::math::Vector(1.0f,1.0f,1.0f) );
  
  // ムーブコードの初期化  
  MoveCodeBase::InitializeMoveCodeWork( &m_moveCodeWork, this );
  m_MoveCode  = fld::mmodel::MoveModelManager::GetMoveCode( m_Header.move_code );
  m_MoveCode->InitProcess( &m_moveCodeWork );

  // アクションコマンドの初期化
  field::mmodel::ActionCmdBase::InitializeAcmdWork( &m_AcmdWork, this );  


  // @todo 計測用仮コード
  {
    // 口パク
    if( m_pMouthMotionResourceRootNode )
    {
      // マテリアルアニメーション
      gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot( 1 );
      if( pMaterialSlot )
      {
        pMaterialSlot->SetAnimationResource( m_pMouthMotionResourceRootNode );
        pMaterialSlot->SetLoop( true );
      }
    }

    // 目パチ
    if( m_pEyeMotionResourceRootNode )
    {
      // マテリアルアニメーション
      gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot( 2 );
      if( pMaterialSlot )
      {
        pMaterialSlot->SetAnimationResource( m_pEyeMotionResourceRootNode );
        pMaterialSlot->SetLoop( true );
        pMaterialSlot->SetStepFrame( 0.5f );
      }
    }
  }


  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void MoveModel::Clear( void )
{
  RemoveChild();

  GFL_SAFE_DELETE( m_pModelInstanceNode );
  GFL_SAFE_DELETE( m_pMotionConfigInstanceNode );
  GFX_SAFE_DELETE( m_pStaticCollisionModel );
  m_isRegistSceneGraph = false;
  m_exists = false;
}

//----------------------------------------------------------------------------
/**
*  @brief  マネージャーを登録
*
*  @param  p_manager  登録する
*  @return 動作コード
*/
//-----------------------------------------------------------------------------
bool MoveModel::activate(MoveModelManager * p_manager)
{
  m_pManager = p_manager;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  シーングラフに自身のモデルインスタンスノードを登録する(トラバース有効)
*/
//-----------------------------------------------------------------------------
void MoveModel::addSceneGraph( void )
{
  if( !isRegistSceneGraph() )
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( getModelInstanceNode() );
    m_isRegistSceneGraph = true;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  シーングラフから自身のモデルインスタンスノードを登録解除する(トラバース無効)
*/
//-----------------------------------------------------------------------------
void MoveModel::removeSceneGraph( void ) 
{
  if( isRegistSceneGraph() )
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( getModelInstanceNode() );
    m_isRegistSceneGraph = false; 
  }
}

//----------------------------------------------------------------------------
// ■ 階層構造、親子関係
/**
* @brief 親モデルの設定
*
* @param pParent 親になるモデル
* @param jointIndex 接続するジョイント番号：-1でモデル自体に接続
*/
//----------------------------------------------------------------------------
void MoveModel::SetParent(MoveModel* pParent, s32 jointIndex)
{
  // 親がNULLなら親子関係解除
  if (pParent == NULL)
  {
    m_pModelInstanceNode->RemoveFromParent();
    m_pParent = NULL;
    m_ParentJointIndex = -1;
    return;
  }

  if (jointIndex < 0)
  {
    // モデル自体に接続
    pParent->m_pModelInstanceNode->AddChild(m_pModelInstanceNode);
  }
  else
  {
    // ジョイントに接続
    pParent->m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->AddChild(m_pModelInstanceNode);
  }

  // BaseModel側でも保持
  m_pParent = pParent;
  m_ParentJointIndex = jointIndex;
}

//----------------------------------------------------------------------------
// ■ 階層構造、親子関係
/**
* @brief 子モデルの切り離し
*/
//----------------------------------------------------------------------------
void MoveModel::RemoveChild(void)
{
  // 自身のモデルインスタンスを親にしているものから登録を外す
  GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  if( game_manager->GetFieldSystem() )
  {
    fld::mmodel::MoveModelManager* pMoveModelManager = game_manager->GetFieldSystem()->GetMoveModelManager();
    if( pMoveModelManager )
    {
      for( u32 i=0; i<fld::mmodel::MMDL_MAX ; i++ )
      {
        MoveModel* pMoveModel = pMoveModelManager->GetMoveModel((fld::mmodel::MMDL_ID)i);
        if( pMoveModel && pMoveModel->GetParent() == this )
        {
          // アンスレッドセーフ
          pMoveModel->SetParent(NULL);
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
* @brief 親モデルの取得
*
* @return 親モデルを返す
*/
//----------------------------------------------------------------------------
MoveModel* MoveModel::GetParent() const
{
  return m_pParent;
}

//----------------------------------------------------------------------------
/**
* @brief 親モデルの接続先のジョイント番号取得
*
* @return 親モデルの接続先のジョイント番号を返す、-1でモデル自体に接続
*/
//----------------------------------------------------------------------------
s32 MoveModel::GetParentJointIndex() const
{
  return m_ParentJointIndex;
}

//----------------------------------------------------------------------------
/**
*  @brief  更新
*/
//-----------------------------------------------------------------------------
void MoveModel::update( void )
{
  if ( m_exists == false )
  {
    return;
  }

  // @todo 計測処理一時対応、何もしない
  if( isAnimation() == false )
  {
    m_pModelInstanceNode->SetVisible( isVisible() );
    return;
  }

  // 地面チェック
  GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  fld::mmodel::MoveModelManager* pMoveModelManager = game_manager->GetFieldSystem()->GetMoveModelManager();
  if (pMoveModelManager)
  {
    const MoveModel* pYuka = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_CITY_YUKA );
    if( pYuka && pYuka->GetTransform() )
    {
      // レイキャスト用コールバッククラスを作成
      gfl2::math::Vector start = GetTransform()->GetTranslation() + gfl2::math::Vector(0.0f,50000.0f,0.0f);
      gfl2::math::Vector end   = GetTransform()->GetTranslation() - gfl2::math::Vector(0.0f,50000.0f,0.0f);
      Test::FieldTest::customCollisionCallback_Intersection callbackIntersection;

      pYuka->GetStaticCollision()->GetIntersectionEx(start,end,NULL,&callbackIntersection);

      // 取得した一番近い接点から高さを取得します
      if( callbackIntersection.IsHit() )
      {
        Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA hitData = callbackIntersection.GetHitData();
        gfl2::math::Vector newPos = GetTransform()->GetTranslation();
        newPos.y = hitData.intersection.y;
        GetTransform()->SetTranslation(newPos);
        GetTransform()->Update();

        gfl2::math::Vector      translate = m_transform->GetTranslation() + m_transform->GetEffectTranslation();
        gfl2::math::Quaternion  rotation  = m_transform->GetQuaternion();
        gfl2::math::Vector      scale     = m_transform->GetScale();
        m_pModelInstanceNode->SetLocalTranslate (gfl2::math::Vector3(translate.x, translate.y, translate.z));
        m_pModelInstanceNode->SetLocalRotation  (rotation);    
        m_pModelInstanceNode->SetLocalScale     (gfl2::math::Vector3(scale.x, scale.y, scale.z));
      }
    }
  }

  // ムーブコードかアクションコマンドの実行(アクションコマンドが優先)
  updateMoveCodeOrActionCmd();

  // LOD切り替えリクエストの処理
  UpdateLod();

  // 描画オンorオフ
  if( isVisible() )
  {
    m_pModelInstanceNode->SetVisible( true );
  }
  else
  {
    m_pModelInstanceNode->SetVisible( false );
  }

  // アニメーションを更新するフレームかどうか取得
  b32 isAnimationUpdateFrame = false;
  m_animationUpdateCnt--;
  if( m_animationUpdateCnt <= 0 )
  {
    isAnimationUpdateFrame = true;
    m_animationUpdateCnt   = m_animationUpdateFrame;
  }

  // アニメーション更新
  bool is_animation_update = false;
  if( isAnimation()                           //アニメーションするかどうか
   && isAnimationUpdateFrame                  //アニメーションを更新するフレームかどうか
   && m_animeNo != -1                         //-1ならばChangeAnimationが一度も呼ばれていない
   && m_pMotionResourceRootNode[m_animeNo] )  //再生しようとするアニメーションのリソースがない
  {
    updateAnimationStepFrame();
    m_AnimationController.Update();
    is_animation_update = true;
  }
  // @todo Tポーズでも口は動かせる！
  else if( isAnimation() && isAnimationUpdateFrame && ( m_pMouthMotionResourceRootNode || m_pEyeMotionResourceRootNode ) )
  {
    m_AnimationController.Update();
    is_animation_update = true;
  }

  // インスタンスノードのSRT更新
  if( m_transform->IsDirty() )
  {// <- Dirtyフラグがm_transform->updateでクリアされるので注意
    gfl2::math::Vector      translate = m_transform->GetTranslation() + m_transform->GetEffectTranslation();
    gfl2::math::Quaternion  rotation  = m_transform->GetQuaternion();
    gfl2::math::Vector      scale     = m_transform->GetScale();
    m_pModelInstanceNode->SetLocalTranslate (gfl2::math::Vector3(translate.x, translate.y, translate.z));
    m_pModelInstanceNode->SetLocalRotation  (rotation);    
    m_pModelInstanceNode->SetLocalScale     (gfl2::math::Vector3(scale.x, scale.y, scale.z));
  }

  m_transform->Update();

  // ジョイントのポスト処理
  if( isAnimationUpdateFrame )
  {
    // IK
    if( m_animeNo != -1 && m_pMotionConfigResourceRootNode && m_isIk )
    {
      m_JointController.GetIKController()->Update();
    }
  }

  // 注視
  if( m_animeNo != -1 && m_pMotionConfigResourceRootNode && m_isInterest )
  {
    InterestController* pInterestController = m_JointController.GetInterestController();
    pInterestController->SetInterestPosition(&m_targetInterest);
    pInterestController->Update();
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  LODを変更
*
*  @param  lod          変更したいレベル
*  @return 成否
*/
//-----------------------------------------------------------------------------
b32 MoveModel::UpdateLod(void)
{
  if( m_requestLod == m_lod || !m_pModelResourceRootNode[m_requestLod] )
  {
    return false;
  }

  RemoveChild();

  // モデルインスタンスノードを初期化
  GFL_SAFE_DELETE( m_pModelInstanceNode );
  m_isRegistSceneGraph = false;             // 解放時にデフォルトでシーングラフから登録解除されます
  {
    char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "MoveModel Lod resId[%d]",  m_Header.rsc_id );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[m_requestLod] );
    m_pModelInstanceNode->SetNodeName( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
  }

  m_isRegistSceneGraph = true;              // CreateModelInstanceNode時にデフォルトでシーングラフに登録されます

  // アニメーションコントローラーを初期化
  m_AnimationController.Finalize();
  gfl2::animation::AnimationController::InitDescription desc;
  desc.jointSlotCount      = 1;
  desc.materialSlotCount   = 3;
  desc.visibilitySlotCount = 1;
  desc.useJointInterp      = true;
  m_AnimationController.Initialize( m_pModelInstanceNode, desc );

  // ジョイントコントローラーを初期化
  if( m_pMotionConfigResourceRootNode )
  {
    // motionConfigInstanceNodeはモデルインスタンスノードの削除時に同時に削除されています
    m_JointController.Finalize();
    m_pMotionConfigInstanceNode = scenegraph::instance::InstanceCreator::CreateMotionConfigInstanceNode(m_pMotionConfigResourceRootNode);
    m_JointController.Initialize(m_pModelInstanceNode, m_pMotionConfigInstanceNode);

    // IKのコールバックを登録
    m_JointController.GetIKController()->SetCallback(IKCallback, this);
  }

  m_lod = m_requestLod;

  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  マネージャーを登録
*
*  @param  id        アニメーションID
*  @param  loopFlag    ループするかどうか
*  @param  interpFrame    補完フレーム
*  @param  stepFrame    再生するフレームスピード
*  @return 成否
*/
//-----------------------------------------------------------------------------
b32 MoveModel::ChangeAnimation( unsigned int id, bool loopFlag, f32 interpFrame, f32 stepFrame)
{
  // 補完中なら無視
  if (m_AnimationController.IsInterp())
  {
    return false;
  }
  bool isInterp = interpFrame > 0.0f;

  // ジョイントアニメーション
  gfl2::animation::JointAnimationSlot* pSlot = (isInterp) ? m_AnimationController.GetJointInterpSlot() : m_AnimationController.GetJointSlot();
  pSlot->SetAnimationResource(m_pMotionResourceRootNode[id]);
  pSlot->SetLoop(loopFlag);

  // マテリアルアニメーション
  gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot();
  pMaterialSlot->SetAnimationResource(m_pMotionResourceRootNode[id]);
  pMaterialSlot->SetLoop(loopFlag);

  // ビジビリティアニメーション
  gfl2::animation::VisibilityAnimationSlot* pVisibilitySlot = m_AnimationController.GetVisibilitySlot();
  pVisibilitySlot->SetAnimationResource(m_pMotionResourceRootNode[id]);
  pVisibilitySlot->SetLoop(loopFlag);

  if (isInterp)
  {
    m_AnimationController.InterpRequest(interpFrame);
  }

  m_animeNo = id;
  m_animationStepFrame = stepFrame;
  m_animationUpdateCnt = 1;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  モーション更新を何フレームに一回にするか考慮したステップフレームに上書き
*/
//-----------------------------------------------------------------------------
void MoveModel::updateAnimationStepFrame(void)
{
  // 補完中なら無視
  if (m_AnimationController.IsInterp())
  {
    return;
  }

  // m_animationUpdateFrameだけアニメーション更新がされないので、それを考慮したステップフレームに
  f32 stepFrame = m_animationStepFrame * (f32)m_animationUpdateFrame;

  // ジョイントアニメーション
  gfl2::animation::JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
  pSlot->SetStepFrame(stepFrame);

  // マテリアルアニメーション
  gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot();
  pMaterialSlot->SetStepFrame(stepFrame);

  // ビジビリティアニメーション
  gfl2::animation::VisibilityAnimationSlot* pVisibilitySlot = m_AnimationController.GetVisibilitySlot();
  pVisibilitySlot->SetStepFrame(stepFrame);
  return;
}

//----------------------------------------------------------------------------
/**
*  @brief  床コリジョンでのコールバック関数
*/
//-----------------------------------------------------------------------------
void MoveModel::rayCallbackFunc(Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA* pHitData)
{
  m_floorNormal = pHitData->pTriangle->m_Normal;
}

//----------------------------------------------------------------------------
/**
*  @brief  LODを更新 @todo 処理負荷計測用一時処理
*
*  @param  lod          変更したいレベル
*  @return 成否
*/
//-----------------------------------------------------------------------------
b32  MoveModel::UpdateLodTest( LOD lod )
{
  m_requestLod = lod;
  if( UpdateLod() )
  {
    // 出現時は待機モーションにする
    if( m_pMotionResourceRootNode[chara::MOTION_ID_WAIT] )
    {
      ChangeAnimation(chara::MOTION_ID_WAIT,true,0.0f,1.0f);
    }

    // @todo 計測用仮コード
    {
      // 口パク
      if( m_pMouthMotionResourceRootNode )
      {
        // マテリアルアニメーション
        gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot( 1 );
        if( pMaterialSlot )
        {
          pMaterialSlot->SetAnimationResource( m_pMouthMotionResourceRootNode );
          pMaterialSlot->SetLoop( true );
        }
      }

      // 目パチ
      if( m_pEyeMotionResourceRootNode )
      {
        // マテリアルアニメーション
        gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_AnimationController.GetMaterialSlot( 2 );
        if( pMaterialSlot )
        {
          pMaterialSlot->SetAnimationResource( m_pEyeMotionResourceRootNode );
          pMaterialSlot->SetLoop( true );
          pMaterialSlot->SetStepFrame( 0.5f );
        }
      }
    }

    // 位置を更新
    // <- Dirtyフラグがm_transform->updateでクリアされるので注意
    gfl2::math::Vector      translate = m_transform->GetTranslation() + m_transform->GetEffectTranslation();
    gfl2::math::Quaternion  rotation  = m_transform->GetQuaternion();
    gfl2::math::Vector      scale     = m_transform->GetScale();
    m_pModelInstanceNode->SetLocalTranslate (gfl2::math::Vector3(translate.x, translate.y, translate.z));
    m_pModelInstanceNode->SetLocalRotation  (rotation);    
    m_pModelInstanceNode->SetLocalScale     (gfl2::math::Vector3(scale.x, scale.y, scale.z));

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  指定LODに変更できるかチェックする @todo 処理負荷計測用一時処理
*
*  @param  lod          変更したいレベル
*  @return 成否(同じLODでも駄目)
*/
//-----------------------------------------------------------------------------
b32  MoveModel::CheckChangeLodTest( LOD lod ) const
{
  if( lod == m_lod || !m_pModelResourceRootNode[lod] )
  {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  DynamicActorからのコールバック関数
*
*  @param  衝突データ
*/
//-----------------------------------------------------------------------------
void MoveModel::hitDynamicCallbackFunc(void* pUserData)
{
}

//----------------------------------------------------------------------------
/**
*  @brief  StaticActorからのコールバック関数
*
*  @param  衝突データ
*/
//-----------------------------------------------------------------------------
void MoveModel::hitStaticCallbackFunc(void* pUserData)
{
}

//*****************************************************************************
// アクションコマンド関連
//*****************************************************************************

//----------------------------------------------------------------------------
/**
 *  @brief  動作更新
 */
//-----------------------------------------------------------------------------
void MoveModel::updateMoveCodeOrActionCmd( void )
{
  m_AcmdUsed = false;

  // アクションコマンドが終わっていたら、MoveCode実行 
  if(!this->CheckMoveBit(fld::mmodel::MOVEBIT_ACMD))
  {
    m_MoveCode->MainProcess( &m_moveCodeWork );
  }
  
  // 優先順位は、アクションコマンド > サブ操作　＞　メイン操作
  if( this->CheckMoveBit(fld::mmodel::MOVEBIT_ACMD) )
  {    
    // アクションコマンドを実行
    field::mmodel::ActionCmdManager::ActionCmd( &m_AcmdWork );

    // 実行
    m_AcmdUsed = true;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  アクションコマンドの設定
 *
 *  @param  acmd    アクションコマンド
 */
//-----------------------------------------------------------------------------
void MoveModel::SetAcmd( Acmd acmd , gfl2::math::Quaternion qua )
{
  // アクションコマンド設定
  field::mmodel::ActionCmdBase::SetAcmd( &m_AcmdWork, acmd , qua );
  this->setAcmdClearWork( acmd );
}

//----------------------------------------------------------------------------
/**
 *  @brief  アクションコマンド設定時にクリアするデータ
 */
//-----------------------------------------------------------------------------
void MoveModel::setAcmdClearWork( Acmd acmd )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  アクションコマンドの強制停止
 */
//-----------------------------------------------------------------------------
void MoveModel::ClearAcmd( void )
{
  // アクションコマンド設定
  field::mmodel::ActionCmdBase::ClearAcmdWork( &m_AcmdWork );
}

//----------------------------------------------------------------------------
/**
 *  @brief  アクションコマンドの実行が可能か？
 *
 *  @retval true    可能
 *  @retval false   不可能
 */
//-----------------------------------------------------------------------------
bool MoveModel::IsAcmdUse( void ) const 
{
  if( field::mmodel::ActionCmdBase::IsAcmdWorkEnd( &m_AcmdWork ) == false ){ //コマンド中
    return false;
  }
  
  return( true );
}

//----------------------------------------------------------------------------
/**
 *  @brief  アクションコマンドの実行が行われたか？ (実行後1フレームを検知することに使用）
 *
 *  @retval true    実行された。
 *  @retval false   実行していない。
 *
 *  1Frameで終わるアクションコマンドが実行されていたのかを検知する為に使用。
 */
//-----------------------------------------------------------------------------
bool MoveModel::IsAcmdUsed( void ) const 
{
  if( field::mmodel::ActionCmdBase::IsAcmdWorkEnd( &m_AcmdWork ) == false ){ //コマンド中
    return true;
  }

  if( m_AcmdUsed ){
    return true;
  }
  
  return( false );
}

//----------------------------------------------------------------------------
/**
 *  @brief  アクションコマンドの実行完了待ち
 *
 *  @retval true    完了
 *  @retval false   実行中
 */
//-----------------------------------------------------------------------------
bool MoveModel::IsAcmdEnd( void ) const
{
  // アクションコマンド実行完了まち
  return field::mmodel::ActionCmdBase::IsAcmdWorkEnd( &m_AcmdWork );
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作ビット　ON
 *
 *  @param  bit   ビット
 */
//-----------------------------------------------------------------------------
void MoveModel::onMoveBit( MoveBit bit )
{
  m_MoveBit |= bit;
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作ビットOFF
 *  
 *  @param  bit   ビット
 */
//-----------------------------------------------------------------------------
void MoveModel::offMoveBit( MoveBit bit )
{
  m_MoveBit &= ~bit;
}

//----------------------------------------------------------------------------
/**
 *  @brief  動作ビットのチェック
 *
 *  @param  bit チェックするビット
 *
 *  @retval true    たっている
 *  @retval false   たっていない
 */
//-----------------------------------------------------------------------------
bool MoveModel::CheckMoveBit( MoveBit bit) const
{
  if( (m_MoveBit & bit) == bit )
  {
    return true;
  }
  return false;
}

}; //end of namespace mmodel
}; //end of namespace fld
