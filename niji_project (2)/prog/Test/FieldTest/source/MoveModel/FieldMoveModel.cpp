//=================================================================================================
/**
*  @file   FieldMoveModel.cpp
*  @brief  フィールド用 動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveModel/FieldMoveModel.h"
#include "../../include/MoveCode/FieldMoveCode_define.h"
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeBase.h"
#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "../../include/ActionCommand/FieldActionCommandManager.h"
#include "../../include/ActionCommand/FieldActionCommandBase.h"
#include "../../include/Collision/BaseShape.h"

// RTTIデバッグシステム
#include "../../include/RttiDebug/RttiEditor.h"
#include "../../include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModel::FieldMoveModel()
: m_isSetup(false)
, m_test (10)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModel::~FieldMoveModel()
{
  Clear();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pFieldMoveModelHeader  作成リクエスト
*  @return trueは成功、falseは失敗
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::Setup(const FieldMoveModelHeader* pFieldMoveModelHeader)
{
  m_isSetup = true;

  // ムーブコードワークの初期化  
  Field::MoveModel::FieldMoveCodeManager::InitializeMoveCodeWork( &m_moveCodeWork, this );
  m_MoveCode  = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( pFieldMoveModelHeader->moveCodeId );
  m_MoveCode->InitProcess( &m_moveCodeWork );

  // アクションコマンドワークの初期化
  Field::MoveModel::FieldActionCommandManager::InitializeActionCommandWork( &m_ActionCommandWork, this );  

  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::Clear(void)
{
  m_isSetup = false;

  m_dynamicActor.Clear();
}

//-----------------------------------------------------------------------------
/**
*  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::PrevUpdate(void)
{
  // アニメーション更新
  UpdateAnimation();

  // アクションコマンドが終わっていたら、動作コードを実行 
  if     (!this->CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD) )
  {
    m_MoveCode->MainProcess( &m_moveCodeWork );
  }
  // アクションコマンドを実行( 優先順位は、アクションコマンド>サブ操作＞メイン操作 )
  else if( this->CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD) )
  {    
    Field::MoveModel::FieldActionCommandManager::UpdateActionCommand( &m_ActionCommandWork );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  更新(コリジョンシーンへの座標/姿勢の登録を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::Update(void)
{
  // コリジョンシステムに伝える
  gfl2::math::Vector3    pos  ( GetPosition()     );
  gfl2::math::Quaternion quat ( GetRotationQuat() );
  m_dynamicActor.setPosition  ( pos  );
  m_dynamicActor.setQuaternion( quat );
  m_dynamicActor.getCollisionResult()->init();
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(コリジョンシーンからの結果を適用し最終的な座標/姿勢の決定を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::PostUpdate(void)
{
  // コリジョンシステムから結果を受け取る
  gfl2::math::Vector3    pos ( m_dynamicActor.getPosition()   );
  gfl2::math::Quaternion quat( m_dynamicActor.getQuaternion() );
  SetPosition    ( pos );
  SetRotationQuat( quat );
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビット  ON
*
*  @param  fieldMoveModelStateBit  ビット
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::onStateBit(FieldMoveModelStateBit fieldMoveModelStateBit)
{
  m_StateBit |= fieldMoveModelStateBit;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビットOFF
*  
*  @param  fieldMoveModelStateBit  ビット
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::offStateBit(FieldMoveModelStateBit fieldMoveModelStateBit)
{
  m_StateBit &= ~fieldMoveModelStateBit;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビットのチェック
*
*  @param  fieldMoveModelStateBit  チェックするビット
*  @return trueはbitOn、falseはbitOff
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::CheckStateBit(FieldMoveModelStateBit fieldMoveModelStateBit) const
{
  if( (m_StateBit & fieldMoveModelStateBit) == fieldMoveModelStateBit )
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの設定
*
*  @param  actionCommand  アクションコマンド
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommand(FieldActionCommandId actionCommand)
{
  FieldActionCommandManager::SetActionCommand( &m_ActionCommandWork, actionCommand );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの強制停止
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearActionCommand(void)
{
  FieldActionCommandManager::ClearActionCommandWork( &m_ActionCommandWork );
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  static bool isInit = false;
  if( isInit )
  {
    return;
  }
  isInit = true;

  RTTI_START_REGIST( 512 );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isSetup      , L"初期化済み" , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_dynamicActor , L"アクター:" );
  RTTI_END_REGIST( FieldMoveModel );

#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタに自身のインスタンスを登録
*
*  @param  エディタに表示したい名前
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::CreateRttiEditorPage(c16* pDispName)
{
#if USE_RTTI_DEBUG

  // RTTIエディタに自身のプロパティを登録
  RegistRtti();
  m_dynamicActor.RegistRtti();
  m_dynamicActor.getShape()->RegistRtti();

  // RTTIエディタに自身のインスタンス内容を表示する
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::RttiEditor::getInstance()->createEditorFromInstace( dispName , &typeid( FieldMoveModel ) , this );
  }
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状]" );
    gfl2::rttidebug::RttiEditor::getInstance()->createEditorFromInstace( dispName , &typeid( *m_dynamicActor.getShape() ) ,  m_dynamicActor.getShape() );
  }

#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタから自身のインスタンスを削除
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::DeleteRttiEditorPage(void)
{
#if USE_RTTI_DEBUG

  gfl2::rttidebug::RttiEditor::getInstance()->deleteEditorFromInstace( this );
  gfl2::rttidebug::RttiEditor::getInstance()->deleteEditorFromInstace( &m_dynamicActor );
  gfl2::rttidebug::RttiEditor::getInstance()->deleteEditorFromInstace( m_dynamicActor.getShape() );

#endif
}

}; //end of namespace MoveModel
}; //end of namespace Field
