//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.cpp
*  @brief  �t�B�[���h�p ����R�[�h �v���C���[
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "niji_conv_header/field/chara/pc0002_00_not_parts.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h" //for FieldMoveCodeWork
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel_define.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCode_define.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodePlayer.h"
///#include "../../include/ActionCommand/FieldActionCommandManager.h"
///#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "GameSystem/include/GameManager.h"

// RTTI�f�o�b�O�V�X�e��
///#include "../../include/RttiDebug/RttiEditor.h"
///#include "../../include/RttiDebug/RttiManager.h"

///#include "Field/FieldRo/include/Debug/FieldDebugTypes.h"  // for TEMP_CODE_IKUTA_150423

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
extern f32 g_CameraYaw;
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#include <model/include/gfl2_CharaModelFactory.h>


namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
FieldMoveCodePlayer::FieldMoveCodePlayer(void) 
: m_turnSpeed(0.3f)
, m_interpFrameForRunToRunStop(5)
, m_interpFrameForRunToWalk(5)
, m_interpFrameForWalkToRun(5)
{
  // �ҋ@
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionId     = PC0002_00_FI001_WAIT01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].stickLevel   = 0.0f;
  }

  // �E�ѕ���(����)
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].stickLevel   = -1.0f;
  }

  // ����
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionId     = PC0002_00_FI010_WALK01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].stickLevel   = 0.4f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].stickLevel   = -1.0f;
  }

  // ����
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].motionId     = PC0002_00_FI012_RUN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].motionId     = PC0002_00_FI014_RUNSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].stickLevel   =-1.0f;
  }

  // �f�o�b�O����
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].moveSpeed    = 5.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].motionSpeed  = 5.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].motionId     = PC0002_00_FI012_RUN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].motionId     = PC0002_00_FI014_RUNSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].stickLevel   =-1.0f;
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h�����@������
*
*  @param  pMoveCodeWork  ����R�[�h���[�N
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_PLAYER;

  Work* pWork         = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->oldState     = PLAYER_MOVE_MAX;
  pWork->oldoldState  = PLAYER_MOVE_MAX;
  pWork->targetQua    = gfl2::math::Quaternion(0.0f,0.0f,0.0f,0.0f);

#if USE_RTTI_DEBUG
  RegistRtti();
  FieldMoveCodePlayer* pThis = const_cast<FieldMoveCodePlayer*>(this);
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"�v���C���[�R���g���[��" , &typeid( FieldMoveCodePlayer ) , pThis );
#endif

}

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h����  ���C��
*
*  @param  pMoveCodeWork  ����R�[�h���[�N
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // ���L���[�N���擾
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // �L�������f���`��p�C���X�^���X���擾
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // �R���g���[���[�̓��͂��擾
  GameSystem::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*   pVecDevice     = device_manager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  f32                       stickX         = pVecDevice->GetX();
  f32                       stickY         = pVecDevice->GetY();
  f32                       stickLength    = sqrt( stickX*stickX + stickY*stickY );

  // �X�e�B�b�N�̓|�����ʂ��݂ăX�e�[�g��I��
  u32 state = GetStateFromStickLevel( stickLength );

  // �X�e�[�g���󋵂��݂ď㏑��
  state = MainProcessSubOverrideState( pMoveCodeWork , state , pWork->oldState , pWork->oldoldState );

  // �X�e�B�b�N�̓|������������ڕW�p���̍X�V
  if( stickLength > 0.0f )
  {
    // �X�e�B�b�N�̓|�����������擾
    gfl2::math::Vector raw_move       ( stickX, 0.0f, stickY, 0.0f );
    gfl2::math::Vector normal_raw_move( raw_move.Normalize3() );
    float dir = gfl2::math::NormalizeRadianAngle( atan2( normal_raw_move.z, normal_raw_move.x ) + 0.5f * PI );

///#ifdef TEMP_CODE_IKUTA_150423
    // ���J������Y����]�����Ɏ��@����̃X�e�B�b�N���͒l�����H���鏈��
    dir = gfl2::math::NormalizeRadianAngle( dir + Field::Debug::g_CameraYaw );
///#endif //@saito_del

    // �ڕW�p���̍X�V
    gfl2::math::Quaternion qua; 
    qua.RadianYToQuaternion(dir);
    pWork->targetQua = qua;
  }

  // �ڕW�p���̕s���`�F�b�N
  {
    gfl2::math::Vector temp(pWork->targetQua.x,pWork->targetQua.y,pWork->targetQua.z,pWork->targetQua.w);
    if( temp.Length() <= 0.0f )
    { 
      pWork->targetQua = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
    }
  }

  // ���o�[���͂ɂ�铮�샂�f���̉�]
  gfl2::math::Quaternion nowQua( pCharaModelInstance->GetRotationQuat() );
  nowQua.Slerp( pWork->targetQua , gfl2::math::Clamp(m_turnSpeed,0.0f,1.0f) );
  pCharaModelInstance->SetRotationQuat( nowQua );

  // Origin�W���C���g�ɂ��ړ��x�N�g����ϊ�����X�P�[��/�N�H�[�^�j�I����ݒ肷��
  pMoveCodeWork->pModel->SetCustomValueForOrigin( m_PlayerMoveSettingTable[state].moveSpeed , pWork->targetQua );

  // �X�e�[�g�̕ύX
  if ( pWork->oldState != state )
  {
    MainProcessSubChangeState(pMoveCodeWork,state,pWork->oldState,pWork->oldoldState);
  }

  pWork->oldoldState = pWork->oldState;
  pWork->oldState    = state;
}

//----------------------------------------------------------------------------
/**
*  @brief  �A�i���O�X�e�B�b�N�̓|�������X�e�[�g���擾
*
*  @param  stick  �A�i���O�X�e�B�b�N�̓|���
*  @return �X�e�[�g
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::GetStateFromStickLevel( f32 stick ) const
{
  for( s32 i=PLAYER_MOVE_MAX-1 ; i>=0 ; i-- )
  {
    if( m_PlayerMoveSettingTable[i].stickLevel >= 0.0f
     && m_PlayerMoveSettingTable[i].stickLevel <= stick )
    {
      return i;
    }
  }
  return PLAYER_MOVE_WAIT;
}

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h����  ���C���̃T�u�֐�(�X�e�[�g���㏑������^�C�~���O�ŌĂ΂��)
*
*  @param  pMoveCodeWork  ����R�[�h���[�N
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const
{
  // �L�������f���`��p�C���X�^���X���擾
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // �{�^�����͏󋵂��擾
  GameSystem::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::Button*         pButton        = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); 

  s32 state = nowState;

  // "�ҋ@�ȊO������J�n or ���胋�[�v"
  if( state != PLAYER_MOVE_WAIT )
  {
    // �ʏ푖����N��������(����n�߂��������̂͑��胋�[�v��Ԃ���)
    if( pButton->IsHold( gfl2::ui::BUTTON_B ) )
    {
      state = PLAYER_MOVE_RUN_START;
      if( m_PlayerMoveSettingTable[state].stickLevel < 0.0f )
      {
        state = PLAYER_MOVE_RUN_LOOP;
      }
    }
    // �f�o�b�O������N��������(����n�߂��������̂̓f�o�b�O���胋�[�v��Ԃ���)
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      state = PLAYER_MOVE_DEBUG_RUN_START;
      if( m_PlayerMoveSettingTable[state].stickLevel < 0.0f )
      {
        state = PLAYER_MOVE_DEBUG_RUN_LOOP;
      }
    }
  }

  // �O�X��܂ő����Ă��茻��/�O���"�ҋ@���"�̂Ƃ���"����I��"�X�e�[�g�ɏ㏑��
  // note: �X�e�B�b�N�𑖂�Ȃ���t�����ɓ��ꂽ�ہA�ҋ@��Ԃ����܂Ȃ��悤�ɂ��邽�ߑO�X����݂Ă��܂��B
  {
    if(  ( state       == PLAYER_MOVE_WAIT )
      && ( oldState    == PLAYER_MOVE_WAIT )
      && ( oldoldState == PLAYER_MOVE_RUN_START || oldoldState == PLAYER_MOVE_RUN_LOOP) )
    {
      state = PLAYER_MOVE_RUN_END;
    }
    if(  ( state       == PLAYER_MOVE_WAIT )
      && ( oldState    == PLAYER_MOVE_WAIT )
      && ( oldoldState == PLAYER_MOVE_DEBUG_RUN_START || oldoldState == PLAYER_MOVE_DEBUG_RUN_LOOP ) )
    {
      state = PLAYER_MOVE_DEBUG_RUN_END;
    }
  }

  // "����n�߁����胋�[�v" �ɂȂ�悤�ɃX�e�[�g���㏑��
  {
    // �P���Đ����I����"����n��"�X�e�[�g���I��
    if( oldState == PLAYER_MOVE_RUN_START )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_RUN_LOOP;
      }
    }
    // �P���Đ����I����"����n��"�X�e�[�g���I��
    if( oldState == PLAYER_MOVE_DEBUG_RUN_START )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_DEBUG_RUN_LOOP;
      }
    }
  }

  // "����I��聨�ҋ@" �ɂȂ�悤�ɃX�e�[�g���㏑��
  {
    // �P���Đ����I����"����I���"�X�e�[�g���I��
    if( oldState == PLAYER_MOVE_RUN_END )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_RUN_END;
      }
    }
    // �P���Đ����I����"����I���"�X�e�[�g���I��
    if( oldState == PLAYER_MOVE_DEBUG_RUN_END )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_DEBUG_RUN_END;
      }
    }
  }

  // L�L�[��������Ă���ۂ́A���@������X�L�b�v���鏈��
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    state = PLAYER_MOVE_WAIT;
  }

  return state;
}

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h����  ���C���̃T�u�֐�(�X�e�[�g���ς�����Ƃ��ɌĂ΂��)
*
*  @param  pMoveCodeWork  ����R�[�h���[�N
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const
{
  // �L�������f���`��p�C���X�^���X���擾
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // �X�e�[�g�ɂ������f�t�H���g���[�V�������擾
  s32 next_anime_id = m_PlayerMoveSettingTable[nowState].motionId;

  // �ҋ@�A�j���[�V���������͊O������㏑���\
  if( nowState == PLAYER_MOVE_WAIT )
  {
    next_anime_id = (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );
  }

  // @todo
  u32 interpFrame = 5;
  if     ( nowState == PLAYER_MOVE_RUN_END )                                        { interpFrame=m_interpFrameForRunToRunStop; }
  else if( nowState == PLAYER_MOVE_WALK_LOOP && oldState == PLAYER_MOVE_RUN_LOOP  ) { interpFrame=m_interpFrameForRunToWalk;    }
  else if( nowState == PLAYER_MOVE_RUN_LOOP  && oldState == PLAYER_MOVE_WALK_LOOP ) { interpFrame=m_interpFrameForWalkToRun;    }

  // ���[�V�����̕ύX
  pCharaModelInstance->ChangeAnimationSmooth(next_anime_id,interpFrame);
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::RegistRtti(void) const
{
#if USE_RTTI_DEBUG

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST_ATTR( FieldMoveCodePlayer , m_turnSpeed                  , L"���]���x" , gfl2::rttidebug::RttiParamManager::ATTR_ADD_SMALL );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpFrameForRunToRunStop , L"����->����I���̕⊮�t���[��" );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpFrameForRunToWalk    , L"����->�����@�@�@�̕⊮�t���[��" );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpFrameForWalkToRun    , L"����->����@�@�@�̕⊮�t���[��" );
  RTTI_END_REGIST( FieldMoveCodePlayer );

#endif
}

}; //end of namespace MoveModel
}; //end of namespace Field
