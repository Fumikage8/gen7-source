//===================================================================
/**
* @file    BattleFesSave.cpp
* @brief   �o�g���t�F�X�T�[�N���̃f�[�^�Ǘ�
* @author  munakata_kai
* @date    2016/11/15
*/
//===================================================================
#include <Savedata/include/BattleFesSave.h>
#include <util/include/gfl2_std_string.h>
#include <System/include/PokemonVersion.h>
#include "Savedata/include/MiscSave.h"
// msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "System/include/HeapDefine.h"
// �t�F�X�t�@�����p
#include <arc_index/message.gaix>
#include <arc_index/battle_tree_trainer.gaix>

// ��^���pmsgId
#include  <niji_conv_header/message/msg_jf_phrase.h>

#include <Print/include/PrintSystem.h>

#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "System/include/DressUp.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/BattleFes/BattleFesDebug.h"
#endif

GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------------------------------------
/**
* @brief    �R���X�g���N�^
*/
//--------------------------------------------------------------------------------------------
BattleFesSave::BattleFesSave()
{
  STATIC_ASSERT(sizeof(BattleFesData) == 924);
  Reset();
}


//--------------------------------------------------------------------------------------------
/**
* @brief    �f�X�g���N�^
*/
//--------------------------------------------------------------------------------------------
BattleFesSave::~BattleFesSave()
{
  Reset();
}


//--------------------------------------------------------------------------------------------
/**
* @brief   �S�f�[�^�����Z�b�g����
*/
//--------------------------------------------------------------------------------------------
void BattleFesSave::Reset()
{
  gfl2::std::MemClear(&m_battleFesData, sizeof(BattleFesData));
}

//------------------------------------------------------------------------------------------
/**
* @brief    �ǂݍ��񂾃f�[�^���Z�b�g����֐�
*
* @param    pData    �擪�̃|�C���^
*/
//------------------------------------------------------------------------------------------
// @override
void BattleFesSave::SetData(void* pData)
{
  gfl2::std::MemCopy(pData, reinterpret_cast<void*>(&m_battleFesData), sizeof(BattleFesData));
}


//------------------------------------------------------------------------------------------
/**
* @brief    �f�[�^�̐擪�|�C���^�𓾂�֐�
*
* @return  �擪�̃|�C���^
*/
//------------------------------------------------------------------------------------------
// @override
void* BattleFesSave::GetData()
{
  return reinterpret_cast<void*>(&m_battleFesData);
}


//------------------------------------------------------------------------------------------
/**
* @brief    �f�[�^�T�C�Y
*
* @return  �o�C�g
*/
//------------------------------------------------------------------------------------------
// @override
size_t BattleFesSave::GetDataSize()
{
  return sizeof(BattleFesData);
}

//--------------------------------------------------------------------------
/**
* @brief �Z�[�u�f�[�^�̃N���A
* @param heap�@�N���A�Ɏg�p����e���|�����q�[�v
*/
//--------------------------------------------------------------------------
void BattleFesSave::Clear(gfl2::heap::HeapBase * heap)
{
  Reset();
  InitRentalPokemonData(heap);
  ClearBattleTreeData();
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    ClearPartnerData(index, heap);
  }
  ClearTrainerData();
}

//--------------------------------------------------------------------------
/**
* @brief �o�g���Z�[�u�f�[�^�̃N���A
* @param heap�@�N���A�Ɏg�p����e���|�����q�[�v
*/
//--------------------------------------------------------------------------
void BattleFesSave::ClearBattleData(gfl2::heap::HeapBase * heap)
{
  InitRentalPokemonData(heap);
  ClearBattleTreeData();
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    ClearPartnerData(index, heap);
  }
  ClearTrainerData();
}

//-----------------------------------------------------------------------------
/**
* @brief   ������
*
* @param   heap  �q�[�v
*/
//-----------------------------------------------------------------------------
void BattleFesSave::InitRentalPokemonData(gfl2::heap::HeapBase* heap)
{
  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(heap->GetLowerHandle()) pml::pokepara::CoreParam(heap->GetLowerHandle(), MONSNO_NULL, 0, 0);
  // �����̃|�P�����f�[�^��������
  cp->Serialize_Core(m_battleFesData.rentalPokemon.pokemonCoreParam);
  m_battleFesData.isRegistPoke = false;
  GFL_DELETE cp;
}

//-----------------------------------------------------------------------------
/**
* @brief   CoreParam���Z�b�g
*
* @param   pp  �Z�b�g����CoreParam
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetRentalPokemonData(const pml::pokepara::CoreParam* cp)
{
  GFL_ASSERT_STOP(cp->IsNull() == false);
  cp->Serialize_Core(&m_battleFesData.rentalPokemon);
  m_battleFesData.isRegistPoke = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   �����̃����^���|�P�����f�[�^�̎擾 CoreParam���擾
* @param[in] pml::pokepara::CoreParam* cp �擾�ꏊ
* @param[out] bool �擾����/�擾���s
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::GetRentalPokemonData(pml::pokepara::CoreParam* cp) const
{
  if(m_battleFesData.isRegistPoke)
  {
    cp->Deserialize_Core(&m_battleFesData.rentalPokemon);
  }
  return (m_battleFesData.isRegistPoke != 0);  ///< u8 -> bool WarnningC4800����.
}

//-----------------------------------------------------------------------------
/**
* @brief �����̃����^���|�P�����f�[�^�̗L��
* @param[out] bool true : �f�[�^�L�� false : �f�[�^����
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsExistRentalPokemonData() const
{
  return (m_battleFesData.isRegistPoke != 0);  ///< u8 -> bool WarnningC4800����.
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[�f�[�^�̓o�^
* @param[in]
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeData(const BattleFes::BATTLE_TREE_DATA battle_tree_data)
{
  SetBattleTreeID(battle_tree_data.pokeID);
  SetBattleTreeSex(battle_tree_data.Sex);
  SetBattleTreeTokusei(battle_tree_data.Tokusei);
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[�h�c�̓o�^
* @param[in] u16 battle_tree_id
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeID(const u16 battle_tree_id)
{
  GFL_ASSERT_STOP(battle_tree_id < BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX);
  m_battleFesData.battle_tree_id = battle_tree_id;
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[�h�c�̎擾
* @param[in] index
* @param[out] u16 battle_tree_id�@0�ُ͈�
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetBattleTreeID() const
{
  return m_battleFesData.battle_tree_id;
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[���ʂ̓o�^
* @param[in] u8 sex
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeSex(const u8 battle_tree_sex)
{
  GFL_ASSERT_STOP(battle_tree_sex < BattleFes::BattleFesDefine::POKEMON_SEX_END);
  m_battleFesData.battle_tree_sex = battle_tree_sex;
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[���ʂ̎擾
* @param[out] u8 sex
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetBattleTreeSex() const
{
  return m_battleFesData.battle_tree_sex;
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[�����̓o�^
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetBattleTreeTokusei(const u8 battle_tree_tokusei)
{
  GFL_ASSERT_STOP(battle_tree_tokusei < BattleFes::BattleFesDefine::POKEMON_TOKUSEI_END);
  m_battleFesData.battle_tree_tokusei = battle_tree_tokusei;
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[�����̎擾
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetBattleTreeTokusei() const
{
  return m_battleFesData.battle_tree_tokusei;
}

//-----------------------------------------------------------------------------
/**
* @brief   �o�g���c���[�֘A�f�[�^�폜
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearBattleTreeData()
{
  m_battleFesData.battle_tree_id = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
  m_battleFesData.battle_tree_sex = 0;
  m_battleFesData.battle_tree_tokusei = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���Ԃ̃����^���|�P�����f�[�^�̏�����
* @param[in] index 0~1
* @param[in] heap  �q�[�v
*/
//-----------------------------------------------------------------------------
void BattleFesSave::InitPartnerPokemonData(const u32 index, gfl2::heap::HeapBase* heap)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(heap->GetLowerHandle()) pml::pokepara::CoreParam(heap->GetLowerHandle(), MONSNO_NULL, 0, 0);
  // ���Ԃ̃|�P�����f�[�^��������
  cp->Serialize_Core(m_battleFesData.partner[index].rentalPokemon.pokemonCoreParam);
  m_battleFesData.partner[index].isRegistPoke = false;
  GFL_DELETE cp;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���Ԃ̃����^���|�P�����f�[�^�̓o�^ CoreParam���Z�b�g
* @param[in] index 0~1
* @param[in] pml::pokepara::CoreParam* cp
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerPokemonData(const u32 index, const pml::pokepara::CoreParam* cp)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(cp->IsNull() == false);
  cp->Serialize_Core(&m_battleFesData.partner[index].rentalPokemon);
  m_battleFesData.partner[index].isRegistPoke = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���Ԃ̃����^���|�P�����f�[�^�̎擾 CoreParam���擾
* @param[in] index 0~1
* @param[in] pml::pokepara::CoreParam* cp
*/
//-----------------------------------------------------------------------------
void BattleFesSave::GetPartnerPokemonData(const u32 index, pml::pokepara::CoreParam* cp)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistPoke);
  cp->Deserialize_Core(&m_battleFesData.partner[index].rentalPokemon);
}

//-----------------------------------------------------------------------------
/**
* @brief   ���Ԃ��o�^����Ă��邩
* @param[in] index
* @param[out] bool
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsPartner(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  if (m_battleFesData.partner[index].isRegistName &&
      m_battleFesData.partner[index].isRegistIcon &&
      m_battleFesData.partner[index].isRegistMegaRing &&
      m_battleFesData.partner[index].isRegistZenryokuRing &&
      m_battleFesData.partner[index].isRegistBall &&
      m_battleFesData.partner[index].isRegistSex  &&
      m_battleFesData.partner[index].isRegistPoke && 
      m_battleFesData.partner[index].isRegistHello &&
      m_battleFesData.partner[index].isRegistEmotion &&
      m_battleFesData.partner[index].isRegistRegionCode &&
      m_battleFesData.partner[index].isRegistRomCode &&
      m_battleFesData.partner[index].isRegistLanguageId &&
      m_battleFesData.partner[index].isRegistAddressId &&
      m_battleFesData.partner[index].isRegistRank)
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���Ԃ̖��O�o�^
* @param[in] index
* @param[in] gfl2::str::STRCODE*
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerName(const u32 index, const gfl2::str::STRCODE* name)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  gfl2::std::MemCopy(name, m_battleFesData.partner[index].name, sizeof(m_battleFesData.partner[index].name));
  m_battleFesData.partner[index].isRegistName = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���Ԃ̖��O�擾
* @param[in] index
* @param[in] gfl2::str::STRCODE* (�Q�U�o�C�g�ȏ�̃T�C�Y�K�{)
*/
//-----------------------------------------------------------------------------
void BattleFesSave::GetPartnerName(const u32 index, gfl2::str::STRCODE* name) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistName);
  gfl2::std::MemCopy(m_battleFesData.partner[index].name, name, sizeof(m_battleFesData.partner[index].name));
}

//-----------------------------------------------------------------------------
/**
* @brief   ��^�����A�o�^
* @param[in] index
* @param[in] u16
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerHello(const u32 index, const u16 patternHello)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(patternHello < msg_jf_phrase_max);
  m_battleFesData.partner[index].patternHello = patternHello;
  m_battleFesData.partner[index].isRegistHello = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ��^�����A�擾
* @param[in] index
* @param[out] u16
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetPartnerHello(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistHello);
  return m_battleFesData.partner[index].patternHello;
}

//-----------------------------------------------------------------------------
/**
* @brief   ��^�������o�^
* @param[in] index
* @param[in] u16
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerEmotion(const u32 index, const u16 patternEmotion)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(patternEmotion < msg_jf_phrase_max);
  m_battleFesData.partner[index].patternEmotion = patternEmotion;
  m_battleFesData.partner[index].isRegistEmotion = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ��^�������擾
* @param[in] index
* @param[out] u16
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetPartnerEmotion(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistEmotion);
  return m_battleFesData.partner[index].patternEmotion;
}

//-----------------------------------------------------------------------------
/**
* @brief   �{�[��������ނ̓o�^
* @param[in] index
* @param[in] u8 (0~7)
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerBallThrowType(const u32 index, const u8 ballThrowType)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(ballThrowType < Savedata::Misc::BALL_THROW_TYPE_NUM);
  m_battleFesData.partner[index].ballThrowType = ballThrowType;
  m_battleFesData.partner[index].isRegistBall = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   �{�[��������ނ̎擾
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerBallThrowType(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistBall);
  return m_battleFesData.partner[index].ballThrowType;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���K�����O�t���O�̓o�^
* @param[in] index
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerMegaRing(const u32 index, const u8 has_mega_ring)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(has_mega_ring <= static_cast<u8>(true));
  m_battleFesData.partner[index].has_mega_ring = has_mega_ring;
  m_battleFesData.partner[index].isRegistMegaRing = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���K�����O�t���O�̎擾
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerMegaRing(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistMegaRing);
  return m_battleFesData.partner[index].has_mega_ring;
}

//-----------------------------------------------------------------------------
/**
* @brief   �����傭�����O�t���O�̓o�^
* @param[in] index
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerZenryokuRing(const u32 index, const u8 has_zenryoku_ring)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(has_zenryoku_ring <= static_cast<u8>(true));
  m_battleFesData.partner[index].has_zenryoku_ring = has_zenryoku_ring;
  m_battleFesData.partner[index].isRegistZenryokuRing = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   �����傭�����O�t���O�̎擾
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerZenryokuRing(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistZenryokuRing);
  return m_battleFesData.partner[index].has_zenryoku_ring;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���ʂ̓o�^
* @param[in] index
* @param[in] u8
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerSex(const u32 index, const u8 sex)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(sex <= PM_FEMALE);
  m_battleFesData.partner[index].sex = sex;
  m_battleFesData.partner[index].isRegistSex = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   ���ʂ̎擾
* @param[in] index
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerSex(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistSex);
  return m_battleFesData.partner[index].sex;
}

//-----------------------------------------------------------------------------
/**
* @brief ���Ԃ̃����^���|�P�����f�[�^�̗L��
* @param[in] index 0~1
* @param[in] pml::pokepara::CoreParam* cp
* @param[out] bool true : �f�[�^�L�� false : �f�[�^����
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsExistPartnerPokemonData(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  return (m_battleFesData.partner[index].isRegistPoke != 0);
}

//-----------------------------------------------------------------------------
/**
* @brief ���Ԃ��ő�l�����邩�ۂ�
* @param[out] bool true : �ő�l���o�^����Ă��� false : �󂫂�����
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsFullPartner()
{
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    if (!IsPartner(index))
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
* @brief �w��̒��ԃf�[�^���폜
* @param[in] index
* @param[in] heap �N���A�Ɏg�p����e���|�����q�[�v
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearPartnerData(const u32 index, gfl2::heap::HeapBase* heap)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  InitPartnerPokemonData(index, heap);
  gfl2::std::MemClear(&m_battleFesData.partner[index].name, sizeof(m_battleFesData.partner[index].name));
  m_battleFesData.partner[index].patternHello = 0;
  m_battleFesData.partner[index].patternEmotion = 0;

  m_battleFesData.partner[index].isRegistName = false;
  m_battleFesData.partner[index].isRegistIcon = false;
  m_battleFesData.partner[index].isRegistMegaRing = false;
  m_battleFesData.partner[index].isRegistZenryokuRing = false;
  m_battleFesData.partner[index].isRegistBall = false;
  m_battleFesData.partner[index].isRegistSex = false;
  m_battleFesData.partner[index].isRegistPoke = false;
  m_battleFesData.partner[index].isRegistHello = false;

  m_battleFesData.partner[index].isRegistEmotion = false;
  m_battleFesData.partner[index].isFesFun = false;
  m_battleFesData.partner[index].isRegistRegionCode = false;
  m_battleFesData.partner[index].isRegistRomCode = false;
  m_battleFesData.partner[index].isRegistLanguageId = false;
  m_battleFesData.partner[index].isRegistAddressId = false;
  m_battleFesData.partner[index].isRegistRank = false;

  m_battleFesData.partner[index].regionCode = 0;
  m_battleFesData.partner[index].romCode = 0;
  m_battleFesData.partner[index].languageId = 0;
  m_battleFesData.partner[index].addressId = 0;

  m_battleFesData.partner[index].ballThrowType = 0;
  m_battleFesData.partner[index].has_mega_ring = 0;
  m_battleFesData.partner[index].has_zenryoku_ring = 0;
  m_battleFesData.partner[index].sex = 0;
  gfl2::std::MemClear(&m_battleFesData.partner[index].icon, sizeof(m_battleFesData.partner[index].icon));
}

//-----------------------------------------------------------------------------
/**
* @brief   �t�F�X�t�@���t���O�i�s�q�t�d�j�ݒ�
* @param[in] index
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerFesFun(const u32 index)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].isFesFun = true;
}

//-----------------------------------------------------------------------------
/**
* @brief   �t�F�X�t�@���t���O�̎擾
* @param[in] index
* @param[out] bool
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsFesFun(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  return (m_battleFesData.partner[index].isFesFun != 0);
}

//-----------------------------------------------------------------------------
/**
* @brief ���̃o�g���Z�b�g�J�E���g�擾�i�O�`�T�j
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetNowBattleSetCount() const
{
  return static_cast<u8>(m_battleFesData.winCount / BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
}

//-----------------------------------------------------------------------------
/**
* @brief �����N�A�b�v�ɕK�v�ȃo�g���Z�b�g�J�E���g�擾�i�P�`�T�j
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetRankUpBattleSetCount() const
{
  const u8 RANKUP_BATTLE_SET_COUNT_MAX = 5;
  u8 setCount = gfl2::math::clamp(static_cast<u8>((m_battleFesData.rank / BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE) + 1), static_cast<u8>(1), RANKUP_BATTLE_SET_COUNT_MAX);
  return setCount;
}

//-----------------------------------------------------------------------------
/**
* @brief �A�����擾�i�O�`�Q�j
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetWinCount() const
{
  return (m_battleFesData.winCount % BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
}

//-----------------------------------------------------------------------------
/**
* @brief �A�����J�E���g�A�b�v�i�{�P�j
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddWinCount()
{
  m_battleFesData.winCount++;
  m_battleFesData.winCount = gfl2::math::clamp(m_battleFesData.winCount, static_cast<u32>(0), static_cast<u32>(GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX));
#if PM_DEBUG
  BattleFesDebug::SetWinCount();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief �A�����O�N���A
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ZeroClearWinCount()
{
  m_battleFesData.winCount = 0;
#if PM_DEBUG
  BattleFesDebug::SetWinCount();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief �A�����N���A�i���O�ɂ���킯�ł͂Ȃ��j
*        0~2��0
*        3~5��3
*        6~9��6
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearWinCount()
{
  m_battleFesData.winCount = GetNowBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX;
#if PM_DEBUG
  BattleFesDebug::SetWinCount();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief �S�����񐔎擾
* @param[out] u16
*/
//-----------------------------------------------------------------------------
u16 BattleFesSave::GetTotalRandCount() const
{
  return m_battleFesData.totalRandCount;
}

//-----------------------------------------------------------------------------
/**
* @brief �S�����񐔃N���A
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearTotalRandCount()
{
  m_battleFesData.totalRandCount = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief �S�����񐔍X�V
*/
//-----------------------------------------------------------------------------
void BattleFesSave::UpdateTotalRandCount()
{
  m_battleFesData.totalRandCount += m_battleFesData.activeRandCount;
  const u16 max = 0xffff;
  m_battleFesData.totalRandCount = gfl2::math::clamp(m_battleFesData.totalRandCount, static_cast<u16>(0), max);
  ClearActiveRandCount();
}

//-----------------------------------------------------------------------------
/**
* @brief �����I���ꂽ�|�P�����Ɏg�p���������񐔃A�b�v�i�{�P�j
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddActiveRandCount()
{
  ++m_battleFesData.activeRandCount;
  const u16 max = 0xffff;
  m_battleFesData.activeRandCount = gfl2::math::clamp(m_battleFesData.activeRandCount, static_cast<u16>(0), max);
}

//-----------------------------------------------------------------------------
/**
* @brief �����I���ꂽ�|�P�����Ɏg�p���������񐔃N���A
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearActiveRandCount()
{
  m_battleFesData.activeRandCount = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief �����N�񐔎擾
* @param[out] u8
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetRank() const
{
  return m_battleFesData.rank;
}

//-----------------------------------------------------------------------------
/**
* @brief �����N�A�b�v�i�{�P�j
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddRank()
{
  m_battleFesData.rank++;
  m_battleFesData.rank = gfl2::math::clamp(m_battleFesData.rank, static_cast<u32>(0), static_cast<u32>(BattleFes::BattleFesDefine::BFES_RANK_MAX));
#if PM_DEBUG
  BattleFesDebug::SetRank();
#endif
}

//-----------------------------------------------------------------------------
/**
* @brief ���ɂ��X�V
*/
//-----------------------------------------------------------------------------
void BattleFesSave::UpdateBattleFesDayEvent()
{
  // �A�����Ă��Ȃ��Ȃ�o�g���t�F�X�f�[�^�폜
  if (GetWinCount() == 0)
  {
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    // @fix GFMMCat[809]�O�����O�ɂ��肩�����𗘗p���Ă���t�őI�ԑO�ɂO�����߂���ƒ��I�����|�P�����������Ă��܂�
    // �����̃|�P�����͏����Ȃ�
    for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
    {
      ClearPartnerData(index, pHeap);
    }
    ClearTrainerData();
  }
  ClearTotalRandCount();
  ClearActiveRandCount();
  ClearUseShop();
}

//-----------------------------------------------------------------------------
/**
* @brief �o�g���c���[�g���[�i�[�f�[�^�i�|�P�������܂ށj�ݒ�
* param[in] u32 (0~4)
* param[in] const BattleFes::TRAINER_DATA&
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetTrainerData(const u32 index, const BattleFes::TRAINER_DATA trainer)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
  GFL_ASSERT_STOP(trainer.trID < GARC_battle_tree_trainer_DATA_NUM);
  m_battleFesData.trainer[index].trID = trainer.trID;
  m_battleFesData.trainer[index].pokeMakeSeed = trainer.pokeMakeSeed;
  for (u32 poke_count(0); poke_count < BattleFes::BattleFesDefine::POKEMON_MAX; ++poke_count)
  {
    m_battleFesData.trainer[index].battleTreeData[poke_count].pokeID = trainer.battleTreeData[poke_count].pokeID;
    m_battleFesData.trainer[index].battleTreeData[poke_count].Sex = trainer.battleTreeData[poke_count].Sex;
    m_battleFesData.trainer[index].battleTreeData[poke_count].Tokusei = trainer.battleTreeData[poke_count].Tokusei;
  }
}

//-----------------------------------------------------------------------------
/**
* @brief �o�g���c���[�g���[�i�[�f�[�^�i�|�P�������܂ށj�擾
* param[in] u32 (0~4)
* param[out] const BattleFes::TRAINER_DATA&
*/
//-----------------------------------------------------------------------------
const BattleFes::TRAINER_DATA& BattleFesSave::GetTrainerData(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
  return m_battleFesData.trainer[index];
}

//-----------------------------------------------------------------------------
/**
* @brief �o�g���c���[�g���[�i�[�f�[�^�i�|�P�������܂ށj�S�폜
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearTrainerData()
{
  for (u32 tr_count(0); tr_count < BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX; ++tr_count)
  {
    m_battleFesData.trainer[tr_count].trID = GARC_battle_tree_trainer_DATA_NUM;
    m_battleFesData.trainer[tr_count].pokeMakeSeed = 0;
    for (u32 poke_count(0); poke_count < BattleFes::BattleFesDefine::POKEMON_MAX; ++poke_count)
    {
      m_battleFesData.trainer[tr_count].battleTreeData[poke_count].pokeID = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
      m_battleFesData.trainer[tr_count].battleTreeData[poke_count].Sex = 0;
      m_battleFesData.trainer[tr_count].battleTreeData[poke_count].Tokusei = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/**
* @brief ���ԂɃt�F�X�t�@����ǉ�����
* param[in] u32 (0~1)
*/
//-----------------------------------------------------------------------------
void BattleFesSave::AddFesFunPartner(const u32 index, const u32 sex, const pml::pokepara::CoreParam* cp, poke_3d::model::DressUpParam* dressUpParam)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  // �o�^
  SetPartnerPokemonData(index, cp);
  SetPartnerFesFun(index);

  // ���O
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  gfl2::str::MsgData* pMsgData = GFL_NEW_LOW(pHeap) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_jf_window_DAT, pHeap->GetLowerHandle());
  gfl2::str::StrBuf tempStrBuf(256, pHeap->GetLowerHandle());
  // �v���Z�b�gNPC�̖��O�j����
  pMsgData->GetString((1 + sex), tempStrBuf);
  gfl2::str::STRCODE name[::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN];
  tempStrBuf.PutStr(name, ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN);
  GFL_SAFE_DELETE(pMsgData);

  SetPartnerName(index, name);

  // ���ʂ�FEMALE�̏ꍇ�́A�I�t�Z�b�g���炷
  const int offsetSex = (sex == PM_MALE) ? 0 : msg_jf_phrase_max / 2;
  const u32 tblIndex = System::GflUse::GetPublicRand(NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::INIT_PATTENR_TABLE_NUM);
  // ��������
  SetPartnerHello(index, static_cast<u16>(NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg(tblIndex, 0) + offsetSex));
  // ����ǂ�
  SetPartnerEmotion(index, static_cast<u16>(NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg(tblIndex, 2) + offsetSex));

  // �A�C�R��
  Savedata::MyStatus::ICON_DATA icon;
  app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData(sex, &icon);
  icon.dressUpParam.ConvertFromDressUpParam(&icon.dressUpParam, *dressUpParam);
  icon.bDressDefault = false;
  icon.dressUpParam.hasZRing = true;
  icon.dressUpParam.hasBagCharm = true;
  SetPartnerIcon(index, &icon);
  SetPartnerRank(index, 0);

  SetPartnerMegaRing(index, static_cast<u8>(true));
  SetPartnerZenryokuRing(index, static_cast<u8>(true));
  SetPartnerBallThrowType(index, System::GflUse::GetPublicRand(Savedata::Misc::BALL_THROW_TYPE_NUM));
  SetPartnerSex(index, static_cast<u8>(sex));
  // �s�g�p�̂��ߓK��
#if defined(GF_PLATFORM_CTR)
  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  SetPartnerRegionCode(index, pMyStatus->GetRegionCode());
  SetPartnerRomCode(index, pMyStatus->GetRomCode());
  SetPartnerLanguageId(index, pMyStatus->GetPokeLanguageId());
  SetPartnerAddressId(index, pMyStatus->GetSimpleAddressId().id);
#else
  SetPartnerRegionCode(index, 0);
  SetPartnerRomCode(index, 0);
  SetPartnerLanguageId(index, 0);
  SetPartnerAddressId(index, 0);
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief �{�X���ۂ�
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsBoss(const u32 tr_count) const
{
  // BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE-1�Z�b�g�ڂ�FIGHT_TRAINER_MAX��ڂ̓{�X�i�T�J�L���}�[�}�l�j
  // �T�O�����N�̂T�Z�b�g�ڂ�FIGHT_TRAINER_MAX��ڂ̓{�X�i�T�J�L���}�[�}�l�j
  bool isBossSetCount = false;
  if (
      ((m_battleFesData.rank % BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE) / (BattleFes::BattleFesDefine::BATTLE_SET_COUNT_PAUSE - 1) == 1) ||
      m_battleFesData.rank == BattleFes::BattleFesDefine::BFES_RANK_MAX
     )
  {
    isBossSetCount = true;
  }
  
  if (tr_count == BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX - 1 && 
      isBossSetCount &&
      // @fix MMCat[164]�y�t�F�X�T�[�N���z�o�g���G�[�W�F���g�ɂă{�X��1�Z�b�g�ڂ���o�Ă���
      // �Ō�̃o�g���Z�b�g�ł��邩�H��ǉ�
      GetNowBattleSetCount() == (GetRankUpBattleSetCount() - 1))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerRegionCode(const u32 index, const u8 regionCode)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].regionCode = regionCode;
  m_battleFesData.partner[index].isRegistRegionCode = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerRegionCode(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistRegionCode);
  return m_battleFesData.partner[index].regionCode;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerRomCode(const u32 index, const u8 romCode)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].romCode = romCode;
  m_battleFesData.partner[index].isRegistRomCode = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerRomCode(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistRomCode);
  return m_battleFesData.partner[index].romCode;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerLanguageId(const u32 index, const u8 languageId)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].languageId = languageId;
  m_battleFesData.partner[index].isRegistLanguageId = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerLanguageId(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistLanguageId);
  return m_battleFesData.partner[index].languageId;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerAddressId(const u32 index, const u32 addressId)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  m_battleFesData.partner[index].addressId = addressId;
  m_battleFesData.partner[index].isRegistAddressId = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u32 BattleFesSave::GetPartnerAddressId(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistAddressId);
  return m_battleFesData.partner[index].addressId;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerIcon(const u32 index, const Savedata::MyStatus::ICON_DATA* icon)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  gfl2::std::MemCopy(icon, &m_battleFesData.partner[index].icon, sizeof(m_battleFesData.partner[index].icon));
  // ���T���O���X����
  poke_3d::model::DressUpParam dressUpParam;
  System::DressUpParamSaveData::ConvertToDressUpParam(&dressUpParam, m_battleFesData.partner[index].icon.dressUpParam);
  BattleFesTool::GetSunglassDressUpParam(&dressUpParam);
  System::DressUpParamSaveData::ConvertFromDressUpParam(&m_battleFesData.partner[index].icon.dressUpParam, dressUpParam);
  m_battleFesData.partner[index].isRegistIcon = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
const Savedata::MyStatus::ICON_DATA* BattleFesSave::GetPartnerIcon(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistIcon);
  return &m_battleFesData.partner[index].icon;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
void BattleFesSave::SetPartnerRank(const u32 index, const u8 rank)
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(rank <= BattleFes::BattleFesDefine::BFES_RANK_MAX);
  m_battleFesData.partner[index].rank = rank;
  m_battleFesData.partner[index].isRegistRank = true;
}

//----------------------------------------------------------------------------
/**
*  @brief
*/
//-----------------------------------------------------------------------------
u8 BattleFesSave::GetPartnerRank(const u32 index) const
{
  GFL_ASSERT_STOP(index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX);
  GFL_ASSERT_STOP(m_battleFesData.partner[index].isRegistRank);
  return m_battleFesData.partner[index].rank;
}

//----------------------------------------------------------------------------
/**
*  @brief ���X�g�p�t���O�擾
*/
//-----------------------------------------------------------------------------
bool BattleFesSave::IsUseShop() const
{
  return (m_battleFesData.isUseShop != 0);
}

//----------------------------------------------------------------------------
/**
*  @brief ���X�g�p
*/
//-----------------------------------------------------------------------------
void BattleFesSave::UseShop()
{
  m_battleFesData.isUseShop = 1;
}

//----------------------------------------------------------------------------
/**
*  @brief �T�J�L�t���OON���}�[�}�l���T�J�L�Ή�
*/
//-----------------------------------------------------------------------------
void BattleFesSave::OpenSakaki()
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  pEventWork->SetEventFlag(SYS_FLAG_BFES_SAKAKI_OPEN);
  // �ŏI�g���[�i�[�`�F�b�N
  u32 tr_count = BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX - 1;
  BattleFes::TRAINER_DATA trainer = GetTrainerData(tr_count);

  // @fix MMCat[197] �o�g���G�[�W�F���g�̘A�����ɁA���C���{�[���P�b�g�c�C�x���g���N���A����ƁA�{�X��́u�}�[�}�l�v���u�T�J�L�v�̃Z���t��b��
  if (trainer.trID == BattleFes::BattleFesDefine::TR_ID_MAMANE)
  {
    // �}�[�}�l�̃f�[�^���Z�[�u���Ȃ���ARR�C�x���g���N���A�����ꍇ�A�}�[�}�l���T�J�L�ɍ쐬������
    trainer.trID = BattleFes::BattleFesDefine::TR_ID_SAKAKI;
    // �|�P�����I��Œ艻�̂��߂̃V�[�h�l�ύX
    trainer.pokeMakeSeed = System::GflUse::GetPublicRand();
    // �|�P�����f�[�^�N���A
    for (u32 poke_count(0); poke_count < BattleFes::BattleFesDefine::POKEMON_MAX; ++poke_count)
    {
      trainer.battleTreeData[poke_count].pokeID = BattleFes::BattleFesDefine::BATTLE_TREE_ID_MAX;
      trainer.battleTreeData[poke_count].Sex = 0;
      trainer.battleTreeData[poke_count].Tokusei = 0;
    }
    // �g���[�i�[�f�[�^�ۑ�
    SetTrainerData(tr_count, trainer);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief ���X�g�p�t���O�N���A
*/
//-----------------------------------------------------------------------------
void BattleFesSave::ClearUseShop()
{
  m_battleFesData.isUseShop = 0;
}

#if PM_DEBUG
u32 BattleFesSave::GetDebugWinCount()
{
  return m_battleFesData.winCount;
}

void BattleFesSave::SetWinCount(const u32 win_count)
{
  m_battleFesData.winCount = win_count;
  m_battleFesData.winCount = gfl2::math::clamp(m_battleFesData.winCount, static_cast<u32>(0), static_cast<u32>(GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX));
}

void BattleFesSave::SetRank(const u32 rank)
{
  m_battleFesData.rank = rank;
  m_battleFesData.rank = gfl2::math::clamp(m_battleFesData.rank, static_cast<u32>(0), static_cast<u32>(BattleFes::BattleFesDefine::BFES_RANK_MAX));
}

void BattleFesSave::SetTotalRandCount(const u16 count)
{
  m_battleFesData.totalRandCount = count;
  const u16 max = 0xffff;
  m_battleFesData.totalRandCount = gfl2::math::clamp(m_battleFesData.totalRandCount, static_cast<u16>(0), max);
}
#endif

GFL_NAMESPACE_END(Savedata)

