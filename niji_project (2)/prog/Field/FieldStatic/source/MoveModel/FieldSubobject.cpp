//=================================================================================================
/**
*  @file   FieldSubobject.cpp
*  @brief  フィールド用 サブオブジェクト
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h" 
#include "Field/FieldStatic/include/FieldNode.h"
#include "../../include/MoveModel/FieldSubobject.h"
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "niji_conv_header/field/chara/ob0001_00.h"
#include "niji_conv_header/field/chara/ob0204_00.h"
#include "niji_conv_header/field/chara/pc0001_00_spmot_ev0500.h"

// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldSubobject::FieldSubobject()
:m_pLocalAllocator     (NULL)
,m_isSetup             (false)
,m_pCharaDrawInstance  (NULL)
,m_isDeleteRequest     (false)
,m_deleteState         (0)
,m_isCulling           (false)
,m_isRegistToPipeline  (false)
,m_pParentNode         (NULL)
,m_pMyRenderingPipeLine(NULL)
{
  m_header.Init();
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldSubobject::~FieldSubobject()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pHeap                    ヒープ
*  @param  pMyRenderingPipeLine     レンダリングパイプライン
*  @param  pCharaModelFactory       キャラリソースファクトリー
*  @param  pFieldSubobject          作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldSubobject::Setup(gfl2::heap::HeapBase*               pHeap,
                           Field::MyRenderingPipeLine*         pMyRenderingPipeLine,
                           poke_3d::model::CharaModelFactory*  pCharaModelFactory,
                           const FieldSubobjectHeader*         pFieldSubobjectResource)
{
  // リソースが既に作成されているかどうか
  if( m_isSetup )
  {
    return;
  }
  m_header               = *pFieldSubobjectResource;
  m_isSetup              = true;
  m_pLocalAllocator      = GFL_NEW( pHeap ) System::nijiAllocator( pHeap );
  m_pMyRenderingPipeLine = pMyRenderingPipeLine;

  // 着せ替えなし 描画インスタンスを作成
  m_pCharaDrawInstance  = GFL_NEW( pHeap ) poke_3d::model::CharaModel();
  bool result = pCharaModelFactory->CreateModel( m_pLocalAllocator, pHeap, m_pCharaDrawInstance , pFieldSubobjectResource->characterId );
  GFL_ASSERT(result);

  // 描画パイプラインに登録
  AddEdgeRenderingTarget();

  // BindPose
  m_pCharaDrawInstance->ChangeAnimationBindpose();
}

//----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void FieldSubobject::Clear(void)
{
  // 描画パイプラインから削除
  RemoveEdgeRenderingTarget();

  // 描画インスタンスの破棄
  GFL_SAFE_DELETE( m_pCharaDrawInstance );

  // アロケータ削除
  GFL_SAFE_DELETE( m_pLocalAllocator );

  m_isSetup            = false;
  m_isDeleteRequest    = false;
  m_deleteState        = 0;
  m_isCulling          = false;
  m_isRegistToPipeline = false;
  m_pParentNode        = NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  更新
*/
//-----------------------------------------------------------------------------
void FieldSubobject::Update(void)
{
  if( m_isSetup && m_isDeleteRequest )
  {
    m_deleteState++;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  SRT/アニメーション更新(自身を保持している動作モデルの座標決定後に必要な処理を行う)
*/
//-----------------------------------------------------------------------------
void FieldSubobject::Attach(b32 isVisible,s32 animationId,f32 animationFrame,u32 parentCharaId)
{
  // 未初期化or削除中なら処理を中断
  if( !m_isSetup || m_isDeleteRequest )
  {
    return;
  }

  // 親がバインドポーズなら処理を中断
  if( animationId == -1 )
  {
    return;
  }

  // モンスターボールだけ40キャラ以上にアタッチされるためハードコーディングでリソース作成を簡略化
  if( m_header.characterId==CHARA_MODEL_ID_OB0204_00 )
  {
    // 親がボールポンポンを再生している場合はキャラにあった専用のアニメーションを作成
    if( animationId == P2_BASE_FI129_POSE01 )
    {
      if( m_pCharaDrawInstance->GetAnimationId() == -1 )
      {
             if( parentCharaId == CHARA_MODEL_ID_TR0010_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0010_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0010_01 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0010_01_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0011_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0011_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0011_01 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0011_01_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0012_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0012_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0017_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0017_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0018_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0018_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0077_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0077_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0078_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0078_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0079_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0079_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0080_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0080_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0081_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0081_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0082_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0082_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0083_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0083_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0084_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0084_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0085_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0085_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0086_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0086_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0087_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0087_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0088_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0088_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0089_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0089_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0090_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0090_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0092_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0092_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0093_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0093_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0094_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0094_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0095_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0095_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0096_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0096_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0097_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0097_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0098_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0098_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0099_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0099_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0100_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0100_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0101_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0101_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0102_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0102_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0103_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0103_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0104_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0104_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0105_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0105_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0106_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0106_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0107_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0107_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0108_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0108_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0109_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0109_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0110_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0110_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0111_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0111_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0112_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0112_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0113_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0113_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0114_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0114_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0115_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0115_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0116_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0116_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0117_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0117_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0118_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0118_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR0119_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR0119_00_FI129_POSE01 ); }
        else if( parentCharaId == CHARA_MODEL_ID_TR1007_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR1007_00_FI129_POSE01 ); }//momiji追加
        else if( parentCharaId == CHARA_MODEL_ID_TR1008_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR1008_00_FI129_POSE01 ); }//momiji追加
        else if( parentCharaId == CHARA_MODEL_ID_TR1009_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0204_00_TR1009_00_FI129_POSE01 ); }//momiji追加
      }
      m_pCharaDrawInstance->SetVisible(isVisible);
    }
    // 親がボールポンポンを再生していない場合は無効化
    else
    {
      if( m_pCharaDrawInstance->GetAnimationId() >= 0 )
      {
        m_pCharaDrawInstance->ChangeAnimationBindpose();
      }
      m_pCharaDrawInstance->SetVisible(false);
    }
  }
  // レポートのペンとノート
  else if( (m_header.characterId==CHARA_MODEL_ID_OB0028_00_PC0001_00) ||
           (m_header.characterId==CHARA_MODEL_ID_OB0028_00_PC0002_00) )
  {
    // Visibleは引き継ぐがアニメーションは引き継がない
    m_pCharaDrawInstance->SetVisible(isVisible);
    m_pCharaDrawInstance->UpdateAnimation();
    return ;
  }
  // ライドギア
  else if( m_header.characterId==CHARA_MODEL_ID_OB0001_00 )
  {
    if( animationId == P1_BASE_EV0500_RIDECALL )
    {
      if( m_pCharaDrawInstance->GetAnimationId() == -1 )
      {
            if( parentCharaId == CHARA_MODEL_ID_PC0001_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0001_00_PC0001_00_EV0500_RIDECALL ); }
       else if( parentCharaId == CHARA_MODEL_ID_PC0002_00 ){ m_pCharaDrawInstance->ChangeAnimation( OB0001_00_PC0002_00_EV0500_RIDECALL ); }
      }
    }
    m_pCharaDrawInstance->SetVisible(isVisible);
  }
  // それ以外のサブオブジェクト
  else
  {
    // 親のモーションと同期させる
    if( animationId != m_pCharaDrawInstance->GetAnimationId() )
    {
      m_pCharaDrawInstance->ChangeAnimation( animationId );
    }
    m_pCharaDrawInstance->SetVisible(isVisible);
  }

  // ハウのZリング処理
  if (parentCharaId == CHARA_MODEL_ID_TR0003_00_OB0021_00 && m_header.characterId == CHARA_MODEL_ID_OB0021_00_TR0003_00)
  {
    // フラグが立っていなければ消す
    Field::EventWork * evwk = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if (!evwk->CheckEventFlag(SYS_FLAG_VISIBLE_TR0003_RING))
    {
      m_pCharaDrawInstance->SetVisible(false);
    }
  }

  // 親とフレームを一致させる
  m_pCharaDrawInstance->SetAnimationStepFrame(0.0f);
  m_pCharaDrawInstance->SetAnimationFrame(animationFrame);
  m_pCharaDrawInstance->UpdateAnimation();
}

void FieldSubobject::PostUpdate(b32 isVisible, u32 parentCharaId)
{
  // @note momiji対応:ハウのリング対応のためFieldMoveModelManager::PostUpdateで行っていた内容を関数化
  
  // ハウのZリング処理
  if (parentCharaId == CHARA_MODEL_ID_TR0003_00_OB0021_00 && m_header.characterId == CHARA_MODEL_ID_OB0021_00_TR0003_00)
  {
    // フラグが立っていなければ消す
    Field::EventWork * evwk = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if (!evwk->CheckEventFlag(SYS_FLAG_VISIBLE_TR0003_RING))
    {
      isVisible = false;
    }
  }
  
  m_pCharaDrawInstance->SetVisible(isVisible);
}

//-----------------------------------------------------------------------------
/**
*  @brief  削除リクエストを発行
*/
//-----------------------------------------------------------------------------
void FieldSubobject::EnableDeleteRequest(void)
{ 
  if( !m_isDeleteRequest )
  { 
    m_isDeleteRequest = true;
    m_deleteState     = 0; 
    m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_pCharaDrawInstance->GetModelInstanceNode() );
  } 
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリングを有効
*/
//-----------------------------------------------------------------------------
void FieldSubobject::EnableCulling(void)
{
  // 自身を保持している動作モデルが削除された状態で呼ばないでください
  RemoveEdgeRenderingTarget();
  RemoveParentNode();
  m_isCulling = true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリングを無効
*/
//-----------------------------------------------------------------------------
void FieldSubobject::DisableCulling(void)
{
  // 自身を保持している動作モデルが削除された状態で呼ばないでください
  AddEdgeRenderingTarget();
  ReSetParentNode();
  m_isCulling = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  描画パイプラインに自身のキャラモデルを追加
*/
//-----------------------------------------------------------------------------
void FieldSubobject::AddEdgeRenderingTarget(void)
{
  // 既に登録されている
  if( m_isRegistToPipeline )
  {
    return;
  }

  m_pMyRenderingPipeLine->AddEdgeRenderingTarget( m_pCharaDrawInstance->GetModelInstanceNode() );
  m_isRegistToPipeline = true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  描画パイプラインから自身のキャラモデルを削除
*/
//-----------------------------------------------------------------------------
void FieldSubobject::RemoveEdgeRenderingTarget(void)
{
  if( !m_isRegistToPipeline )
  {
    return;
  }

  m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_pCharaDrawInstance->GetModelInstanceNode() );
  m_isRegistToPipeline = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  モデルインスタンスノードを親から登録を外す
*/
//-----------------------------------------------------------------------------
void FieldSubobject::RemoveParentNode(void)
{
  if( !m_pParentNode )
  {
    m_pParentNode = m_pCharaDrawInstance->GetModelInstanceNode()->GetParent();
    m_pCharaDrawInstance->GetModelInstanceNode()->RemoveFromParent();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  モデルインスタンスノードを親に再設定する
*/
//-----------------------------------------------------------------------------
void FieldSubobject::ReSetParentNode(void)
{
  if( m_pParentNode )
  {
    m_pParentNode->AddChild( m_pCharaDrawInstance->GetModelInstanceNode() );
    m_pParentNode = NULL;
  }
}

//================================================================================================================
// 親のインターフェースの実装部
//================================================================================================================

//-----------------------------------------------------------------------------
/**
*  @brief 表示ON・OFFを設定
*
*  @param isVisible  表示ON・OFF
*/
//-----------------------------------------------------------------------------
void FieldSubobject::SetVisible(bool isVisible)
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    m_pCharaDrawInstance->SetVisible(isVisible);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  表示ON・OFFを取得
*
*  @return 表示ON・OFF
*/
//-----------------------------------------------------------------------------
bool FieldSubobject::GetVisible(void) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return m_pCharaDrawInstance->IsVisible() ? true : false;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief SRTを設定
*
*  @param srt 設定したいSRT
*/
//-----------------------------------------------------------------------------
void FieldSubobject::SetLocalSRT(const gfl2::math::SRT & srt)
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    m_pCharaDrawInstance->SetScale( srt.scale );
    m_pCharaDrawInstance->SetRotationQuat( srt.rotation );
    m_pCharaDrawInstance->SetPosition( srt.translate );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  SRTを取得
*
*  @return SRT
*/
//-----------------------------------------------------------------------------
const gfl2::math::SRT& FieldSubobject::GetLocalSRT(void) const
{
  GFL_ASSERT_MSG( m_pCharaDrawInstance                          , "モデルがセットアップされていない状態でLocalSrtを取得することはできません" );
  GFL_ASSERT_MSG( m_pCharaDrawInstance->GetModelInstanceNode()  , "モデルがセットアップされていない状態でLocalSrtを取得することはできません" );
  return m_pCharaDrawInstance->GetModelInstanceNode()->GetLocalSRT();
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンス数を取得
*
*  @return モデルインスタンス数
*/
//-----------------------------------------------------------------------------
u32 FieldSubobject::GetModelInstanceNum(void) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンスを取得
*
*  @return モデルインスタンス
*/
//-----------------------------------------------------------------------------
const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * FieldSubobject::GetModelInstance(int index) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return m_pCharaDrawInstance->GetModelInstanceNode();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンスの遅延解放は自由に実装
*
*  @note:
*   削除リクエストを受けたモデルインスタンスを解放
*   -実行タイミング  フィールドプロセスの先頭で実行します。
*   -制限            PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
*/
//-----------------------------------------------------------------------------
void FieldSubobject::ExecuteTerminateRequest(void)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  安全に破棄できる常態かチェック
*
*  @return 安全に解放できるかどうか
*/
//-----------------------------------------------------------------------------
bool FieldSubobject::IsCanTerminate(void)
{
  return true;
}

}; //end of namespace MoveModel
}; //end of namespace Field
