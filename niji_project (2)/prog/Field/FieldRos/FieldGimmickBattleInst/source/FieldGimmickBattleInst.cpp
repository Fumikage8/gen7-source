//==============================================================================
/**
 * @file	FieldGimmickBattleInst.cpp
 * @brief	フィールドギミック：バトル施設
 * @author	onoue_masayuki
 * @date	2015/01/28
 */
// =============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"

// ジョインフェスタ動的動作モデルロードシステム
#include <ro/include/gfl2_RoManager.h>
#include "system/include/HeapDefine.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstModelManager.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"

#include "Field/FieldRos/FieldGimmickBattleInst/include/FieldGimmickBattleInst.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"

GFL_NAMESPACE_BEGIN( Field )


// コンストラクタ
FieldGimmickBattleInst::FieldGimmickBattleInst( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap, u32 zoneId ) :
  FieldGimmick(pMap)
  ,m_pFieldDeviceHeap(pHeap)
  ,m_pModelManager(NULL)
  ,m_initializeSeq(INITIALIZE_SEQ_START)
  ,m_finalizeSeq(FINALIZE_SEQ_SAVE_TRANSFORM)
  ,m_status(Closed)
  ,m_zoneID(zoneId)
{
}

// デストラクタ
FieldGimmickBattleInst::~FieldGimmickBattleInst()
{
}

/**
*  @biref  ステータスをセットする
  */
void FieldGimmickBattleInst::SetStatus(GimmickStatus status)
{ 
  m_status = status; 
  ONOUE_PRINT("BattleInst Gimmick Status = %d\n",status); 
}

/**
 *  @brief  生成
 */
void FieldGimmickBattleInst::Create( void )
{
  ONOUE_PRINT("FieldGimmickBattleInst::Create\n");
}

void FieldGimmickBattleInst::UpdateCreate( void )
{
  GameSys::GameManager* pGameManager = m_pFieldmap->GetGameManager();
  if( IsExistBattleInst() )
  {
    OpenGimmick();
  }
}

bool FieldGimmickBattleInst::IsExistBattleInst()
{
  GameSys::GameManager* pGameManager = m_pFieldmap->GetGameManager();
  return (pGameManager->GetBattleInstManager() != NULL) && pGameManager->GetBattleInstManager()->IsCreated();
}

/**
 *  @biref  生成処理が完了しているか
 *  @return バトル施設内のみ最初から生成
 */
bool FieldGimmickBattleInst::IsCreated( void )
{
  GameSys::GameManager* pGameManager = m_pFieldmap->GetGameManager();
  if( IsExistBattleInst() )
  {
    // 施設内の場合は生成の完了をまつ
    return m_status == Open;
  }else{
    // 施設外の場合は生成の必要がない
    return true;
  }
}

/**
*  @biref  ギミックをオープンする
*  @return 初期化処理が不要な場合にtrue
*/
bool FieldGimmickBattleInst::OpenGimmick( void )
{
  // 閉じていたらセットアップスタート
  if( m_status == Closed )
  {
    m_initializeSeq = INITIALIZE_SEQ_START;
    SetStatus( Setup );
  }

  // セットアップ以外の状態なら完了
  if( m_status != Setup )
  {
    return true;
  }

  ONOUE_PRINT("FieldGimmickBattleInst::Setup %d \n", m_initializeSeq);

  BattleInst::BattleInstManager* inst = m_pFieldmap->GetGameManager()->GetBattleInstManager();

  // フィールドキャラクターのリソースはAreaに必要な物(EventEditorではいちされている）のみ
  // 読み込むように変更しました。　なので、JFで使用しているリソースをここで読み込みます。
  // ジョインフェスタで使用するリソースの読み込みをリクエスト
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = m_pFieldmap->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  // charcterIdが0と1(=CHARA_MODEL_ID_PC0001_00とCHARA_MODEL_ID_PC0002_00)のモデルリソースノードがないと、
  // その性別の着せ替えがセットアップ出来ない。

  switch( m_initializeSeq ){
  case INITIALIZE_SEQ_START:                   //! 初期化最初
    {
      // 各システムインスタンス作成
      u32 modelCount = BattleInst::BattleInstDefine::BINST_MODEL_DISP_MAX;
      if( m_zoneID == Z_F0401BT0101)
      {
        // 受付は1人に制限
        modelCount = 1;
      }
      ONOUE_PRINT("バトル施設 動的に配置できるモデル数: %d\n",modelCount);
      m_pModelManager = GFL_NEW( m_pFieldDeviceHeap ) BattleInst::BattleInstModelManager( m_pFieldDeviceHeap , modelCount );
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
        m_initializeSeq = INITIALIZE_SEQ_RECOVER_START; 
      }
    }
    break;

  case INITIALIZE_SEQ_RECOVER_START:
    if( !IsExistBattleInst() )
    {
      m_initializeSeq = INITIALIZE_SEQ_END; 
    }else{
      m_initializeSeq = INITIALIZE_SEQ_END; // 未復帰モデルが無い場合は終了 

      // 未復帰モデルがあれば、リクエストを投げる
      for (int i = 0; i < BattleInst::BattleInstDefine::BINST_MODEL_DISP_MAX; i++ )
      {
        BattleInst::BattleInst::MoveModelData* p_data = inst->GetModelData(i);
        if( p_data->isUsing && !m_pModelManager->IsSetupEnd(p_data->index) )
        {
          m_pModelManager->RequestReload(p_data);
          m_initializeSeq = INITIALIZE_SEQ_RECOVER_LOAD_WAIT; 
          break;
        }
      }
    }
    break;

  case INITIALIZE_SEQ_RECOVER_LOAD_WAIT:
    if( m_pModelManager->IsRequestEnd(false) )
    {
      m_initializeSeq = INITIALIZE_SEQ_RECOVER_START; 
    }
    break;

  case INITIALIZE_SEQ_END:                     //! 初期化完了
    SetStatus( Open );
    // 処理なし
    break;
  }
    
  return false;
}

/**
 *  @biref  ギミックをクローズする
 *  @return 完了でTreu
 */
bool FieldGimmickBattleInst::CloseGimmick( void )
{
  // 開いていたらクローズ
  if( m_status == Open )
  {
    m_finalizeSeq = FINALIZE_SEQ_SAVE_TRANSFORM;
    SetStatus(Closing);
  }

  // クローズ中以外であれば完了
  if( m_status != Closing )
  {
    return true;
  }

  ONOUE_PRINT("FieldGimmickBattleInst::Finalize %d \n", m_finalizeSeq);
  BattleInst::BattleInstManager* inst = m_pFieldmap->GetGameManager()->GetBattleInstManager();

  switch( m_finalizeSeq ){
  case FINALIZE_SEQ_SAVE_TRANSFORM:            //! 位置保存
    {
      if( IsExistBattleInst() )
      {
        for (int i = 0; i < BattleInst::BattleInstDefine::BINST_MODEL_DISP_MAX; i++ )
        {
          BattleInst::BattleInst::MoveModelData* p_data = inst->GetModelData(i);
          if( p_data->isUsing && m_pModelManager->IsSetupEnd(p_data->index) )
          {
            m_pModelManager->UpdateModelData(p_data);
          }
        }
      }
      m_finalizeSeq = FINALIZE_SEQ_TERMINATE; 
    }
    /* break through */
  case FINALIZE_SEQ_TERMINATE:            //! 位置保存
    {
      // 破棄待ち
      if( m_pModelManager->Terminate() )
      {
        m_finalizeSeq = FINALIZE_SEQ_END; 
      }
    }
    break;
  case FINALIZE_SEQ_END:            //! 位置保存
    {
      GFL_SAFE_DELETE( m_pModelManager );
      SetStatus( Closed );
      return true;
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
bool FieldGimmickBattleInst::Finalize( void )
{
  return CloseGimmick();
}

/**
 *  @brief  破棄
 */
void FieldGimmickBattleInst::Delete( void )
{
  ONOUE_PRINT("FieldGimmickBattleInst::Delete\n");

}

/**
 *  @brief  Ready中1度だけ呼び出し
 */
void FieldGimmickBattleInst::Ready( void )
{
  ONOUE_PRINT("FieldGimmickBattleInst::Ready\n");
}
  
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  キャラクタ前更新よりも手前で呼び出される
 */
void FieldGimmickBattleInst::UpdateFirst( void )
{
}

/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickBattleInst::UpdateLast( void )
{
}
/**
 *  @brief  更新
 *  Fieldmap::UpdateNormalにて呼び出される
 */
void FieldGimmickBattleInst::CalculateAfter( void )
{
}

/**
 *  @brief  特殊ステップチェック
 */
bool FieldGimmickBattleInst::MoveRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

/**
 *  @brief  イベント起動チェック
 */
bool FieldGimmickBattleInst::EventRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

GFL_NAMESPACE_END( Field )
