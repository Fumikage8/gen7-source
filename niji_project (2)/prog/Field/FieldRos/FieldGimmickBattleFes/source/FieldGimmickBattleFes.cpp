//==============================================================================
/**
* @file	FieldGimmickBattleFes.cpp
* @brief	フィールドギミック：バトルフェス ※BattleInstから移植
* @author	munakata_kai
* @date	2016/11/26
*/
// =============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"

#include <gfx/include/gfl2_types.h>   // for displayNo
#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRGL.h>
#endif

// ジョインフェスタ動的動作モデルロードシステム
#include "system/include/HeapDefine.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesModelManager.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesMsgResourceManager.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/FieldGimmickBattleFes.h"
// staticmodel
#include <niji_conv_header\field\static_model\static_model_id.h>

GFL_NAMESPACE_BEGIN( Field ) 

// コンストラクタ
FieldGimmickBattleFes::FieldGimmickBattleFes(gfl2::heap::HeapBase* pHeap, Fieldmap *pMap)
  :FieldGimmick(pMap)
  ,m_pFieldDeviceHeap(pHeap)
  ,m_pModelManager(NULL)
  ,m_pResourceManager(NULL)
  ,m_pJoinFestaRoModule(NULL)
  ,m_pKisekaeDataRoModule(NULL)
  ,m_initializeSeq(INITIALIZE_SEQ_START)
  ,m_finalizeSeq(FINALIZE_SEQ_TERMINATE)
  ,m_status(Closed)
{
  // リソースマネージャ用ヒープ作成
  {
    gfl2::heap::NwAllocator*      pNULLNwAllocator = NULL;
    gfl2::util::GLHeapAllocator*  pNULLGLHeapAllocator = NULL;
    m_ResourceManagerHeap.Create(
      m_pFieldDeviceHeap,     //  gfl2::heap::HeapBase        * heap,
      m_pFieldDeviceHeap,     //  gfl2::heap::HeapBase        * devHeap,
      pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * heapAllocator,
      pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * devHeapAllocator,
      pNULLGLHeapAllocator,   //  gfl2::util::GLHeapAllocator * glHeapAllocator,
      pNULLGLHeapAllocator    //  gfl2::util::GLHeapAllocator * devGLHeapAllocator
      );   /*  参照Create  */
  }
}

// デストラクタ
FieldGimmickBattleFes::~FieldGimmickBattleFes()
{
  m_ResourceManagerHeap.Delete();
}

/**
*  @brief  生成
*/
void FieldGimmickBattleFes::Create()
{
  //  DLLを読み込み
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  // 着せ替えデータを参照するのにdll読みが必要
  m_pKisekaeDataRoModule = roMgr->LoadModule(pGameManager->GetAsyncFileManager(), "KisekaeData.cro");
  roMgr->StartModule(m_pKisekaeDataRoModule, true);

  //// ジョインフェスタDLL
  m_pJoinFestaRoModule = roMgr->LoadModule(pGameManager->GetAsyncFileManager(), "JoinFesta.cro");
  roMgr->StartModule(m_pJoinFestaRoModule, true);

  BattleFes::BattleFesManager* pBattleFesManager = pGameManager->GetGameData()->GetBattleFesManager();
  pBattleFesManager->Create(pGameManager);
  // リソースマネージャ
  m_pResourceManager = GFL_NEW(m_pFieldDeviceHeap) BattleFes::BattleFesMsgResourceManager(&m_ResourceManagerHeap);
  // 初期化リクエスト関連
  {
    // リソース
    m_pResourceManager->RequestLoad();
    m_pResourceManager->ObserveLoad();
  }

}

void FieldGimmickBattleFes::UpdateCreate()
{
  OpenGimmick();
}

/**
 *  @biref  生成処理が完了しているか
 *  @return バトル施設内のみ最初から生成
 */
bool FieldGimmickBattleFes::IsCreated()
{
  return (m_status == Open);
}

/**
*  @biref  ギミックをオープンする
*  @return 初期化処理が不要な場合にtrue
*/
bool FieldGimmickBattleFes::OpenGimmick()
{
  // 閉じていたらセットアップスタート
  if( m_status == Closed )
  {
    m_initializeSeq = INITIALIZE_SEQ_START;
    setStatus( Setup );
  }

  // セットアップ以外の状態なら完了
  if( m_status != Setup )
  {
    return true;
  }

  MUNAKATA_PRINT("FieldGimmickBattleFes::Setup %d \n", m_initializeSeq);

  // フィールドキャラクターのリソースはAreaに必要な物(EventEditorではいちされている）のみ
  // 読み込むように変更しました。　なので、JFで使用しているリソースをここで読み込みます。
  // ジョインフェスタで使用するリソースの読み込みをリクエスト
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = m_pFieldmap->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  // charcterIdが0と1(=CHARA_MODEL_ID_PC0001_00とCHARA_MODEL_ID_PC0002_00)のモデルリソースノードがないと、
  // その性別の着せ替えがセットアップ出来ない。

  switch( m_initializeSeq ){
  case INITIALIZE_SEQ_START:                   //! 初期化最初
    {
      // リソース読み込み完了待ち
      if (!m_pResourceManager->ObserveLoad())
      {
        break;
      }
      // リソースマネージャーを登録
      m_pFieldmap->GetGameManager()->GetGameData()->GetBattleFesManager()->SetMsgResourceManager(m_pResourceManager);

      // 各システムインスタンス作成
      m_pModelManager = GFL_NEW( m_pFieldDeviceHeap ) BattleFes::BattleFesModelManager( m_pFieldDeviceHeap );
      m_initializeSeq = INITIALIZE_SEQ_CHARA_LOAD_1;
    }
    /* break through */

  case INITIALIZE_SEQ_CHARA_LOAD_1:            //! キャラクターリソースロード
    {
      pFieldCharaModelManager->LoadStaticAsync(CHARA_MODEL_ID_PC0001_00);
      m_initializeSeq = INITIALIZE_SEQ_CHARA_WAIT_1; 
    }
    /* break through */

  case INITIALIZE_SEQ_CHARA_WAIT_1:            //! キャラクターリソースロード待ち
    {
      if( pFieldCharaModelManager->IsLoaded(CHARA_MODEL_ID_PC0001_00) )
      {
        // キャラクターリソースノードの初期化
        pFieldCharaModelManager->SetupModel(CHARA_MODEL_ID_PC0001_00);
        m_initializeSeq = INITIALIZE_SEQ_CHARA_LOAD_2; 
      }
    }
    break;

  case INITIALIZE_SEQ_CHARA_LOAD_2:            //! キャラクターリソースロード
    {
      pFieldCharaModelManager->LoadStaticAsync(CHARA_MODEL_ID_PC0002_00);
      m_initializeSeq = INITIALIZE_SEQ_CHARA_WAIT_2; 
    }
    /* break through */

  case INITIALIZE_SEQ_CHARA_WAIT_2:            //! キャラクターリソースロード待ち
    {
      if( pFieldCharaModelManager->IsLoaded(CHARA_MODEL_ID_PC0002_00) )
      {
        // キャラクターリソースノードの初期化
        pFieldCharaModelManager->SetupModel(CHARA_MODEL_ID_PC0002_00);
        m_initializeSeq = INITIALIZE_SEQ_END;
      }
    }
    break;

  case INITIALIZE_SEQ_END:                     //! 初期化完了
    setStatus( Open );
    // 処理なし
    break;
  }
    
  return false;
}

/**
 *  @biref  ギミックをクローズする
 *  @return 完了でTreu
 */
bool FieldGimmickBattleFes::CloseGimmick()
{
  // 開いていたらクローズ
  if( m_status == Open )
  {
    m_finalizeSeq = FINALIZE_SEQ_TERMINATE;
    setStatus(Closing);
  }

  // クローズ中以外であれば完了
  if( m_status != Closing )
  {
    return true;
  }

  MUNAKATA_PRINT("FieldGimmickBattleFes::Finalize %d \n", m_finalizeSeq);

  switch( m_finalizeSeq ){
  case FINALIZE_SEQ_TERMINATE:
    {
      // 破棄待ち
      if( m_pModelManager->Terminate() )
      {
        GFL_SAFE_DELETE(m_pModelManager);
        m_finalizeSeq = FINALIZE_SEQ_END; 
      }
    }
    break;
  case FINALIZE_SEQ_END:
    {
      if (m_pResourceManager->ReleaseData())
      {
        setStatus(Closed);
        return true;
      }
    }
  }

  return false;
}

/**
*  @biref  破棄更新
*  @return 破棄が完了したかどうか
*
*  破棄が完了するまでマイフレーム呼出がなされる関数
*/
bool FieldGimmickBattleFes::Finalize()
{
  return CloseGimmick();
}

/**
*  @brief  破棄
*/
void FieldGimmickBattleFes::Delete()
{
  GFL_SAFE_DELETE(m_pResourceManager);
  BattleFes::BattleFesManager* pBattleFesManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleFesManager();
  pBattleFesManager->SetMsgResourceManager(NULL);
  pBattleFesManager->Delete();

  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
  roMgr->DisposeModule(m_pJoinFestaRoModule);
  roMgr->DisposeModule(m_pKisekaeDataRoModule);
}

/**
*  @brief  Ready中1度だけ呼び出し
*/
void FieldGimmickBattleFes::Ready()
{
  
}

/**
*  @brief  更新
*  Fieldmap::Updateにて呼出
*  キャラクタ前更新よりも手前で呼び出される
*/
void FieldGimmickBattleFes::UpdateFirst()
{
}

/**
*  @brief  更新
*  Fieldmap::Updateにて呼出
*  UpdateNormalが終了してから呼び出される
*/
void FieldGimmickBattleFes::UpdateLast()
{
}
/**
*  @brief  更新
*  Fieldmap::UpdateNormalにて呼び出される
*/
void FieldGimmickBattleFes::CalculateAfter()
{
}

/**
*  @brief  特殊ステップチェック
*/
bool FieldGimmickBattleFes::MoveRequestCheck(const EventCheckRequest *cpReq)
{
  return false;
}

/**
*  @brief  イベント起動チェック
*/
bool FieldGimmickBattleFes::EventRequestCheck(const EventCheckRequest *cpReq)
{
  return false;
}

/**
*  @brief  現在バトルフェス専用関数　フレームバッファの画像データをモデルのテクスチャにコピーする
*/
void FieldGimmickBattleFes::TransferTexture(gfl2::gfx::CtrDisplayNo displayNo)
{
#if defined(GF_PLATFORM_CTR)
  if (displayNo != gfl2::gfx::CtrDisplayNo::LEFT)
  {
    return;
  }
  const gfl2::gfx::Texture* pMonitorTexture = NULL;
  StaticModel::StaticModel* pStaticModel = m_pFieldmap->GetStaticModelManager()->GetStaticModelIDStaticModel(STATIC_MODEL_ID_W_F0601JF0103_BM_MONITOR01);
  GFL_ASSERT_STOP(pStaticModel);
  //BaseModel
  poke_3d::model::BaseModel* pBaseModel = pStaticModel->GetBaseModel(pStaticModel->GetNowModelType());
  GFL_ASSERT(pBaseModel);
  // マテリアル名から検索
  const c8 target_material_name[] = { "w_f0601jf0103_movie01" };
  s32 material_index = pBaseModel->GetMaterialIndex(target_material_name);
  if (material_index != -1)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pBaseModel->GetModelInstanceNode()->GetMaterialInstanceNode(material_index);
    // テクスチャ名から検索
    // リソースが固まったのでテクスチャ検索番号を固定化
    u32 texture_index = 0;
    const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureNode = pMaterialNode->GetTextureResourceNode(texture_index);
    const u32 MATERIAL_ATTRIBUTE_NAME_DATA_MAX = 64;
    const c8 target_texture_name[MATERIAL_ATTRIBUTE_NAME_DATA_MAX] = { "w_f0601jf0103_movie01.tga" };
    if (gfl2::std::StringCmp(pTextureNode->GetName(), target_texture_name, MATERIAL_ATTRIBUTE_NAME_DATA_MAX) == 0)
    {
      pMonitorTexture = pTextureNode->GetTexture();
    }
    else
    {
      pMonitorTexture = NULL;
    }
  }

  if (pMonitorTexture == NULL)
  {
    GFL_ASSERT(0);
    return;
  }

  const gfl2::gfx::ctr::CTRTexture* pCtrCopyColorBufferTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pMonitorTexture);
  uptr destinationAddress = pCtrCopyColorBufferTexture->GetVirtualAddr();
  gfl2::gfx::GFGL::AddTransferColorBufferCommand(reinterpret_cast<void*>(destinationAddress));
#endif
}

// private
/**
*  @biref  ステータスをセットする
*/
void FieldGimmickBattleFes::setStatus(GimmickStatus status)
{
  m_status = status;
  MUNAKATA_PRINT("BattleFes Gimmick Status = %d\n", status);
}

GFL_NAMESPACE_END( Field )
