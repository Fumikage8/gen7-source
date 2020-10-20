//============================================================================================
/**
 *	GAME FREAK inc.
 * @file    gfl_MathRandom.cpp
 * @brief   �����֐�
            2011.8.19��M�n�����ɕύX
            2012.9.25��CTRSDK�ɕύX
 * @author  k.ohno
 * @date    2010.12.14
 */
//============================================================================================

#include <nn/math.h>
#include <nn/cfg.h>
#include "gfl_Std.h"
#include "gfl_Heap.h"
#include "gfl_Common.h"
#include "math/gfl_MathRandom.h"



namespace gfl{
namespace math{

//------------------------TinyMT  2��127��-1


//------------------------------------------------------------------
/**
 * @name     Random�R���X�g���N�^
 * @brief    �������܂���B Initialize���Ă΂Ȃ���Next�̂Ƃ���ASSERT�łƂ܂�܂�
             has�����Ƃ��ɏ���ɃR���X�g���N�^������̂�����������߂ł�
 */
//------------------------------------------------------------------

Random::Random(void)
{
}

//------------------------------------------------------------------
/**
 * @name     RandomInitialize
 * @brief    TinyMT�ɂ�闐�������������܂�
             ��̓G���g���s�[��SHA1�ŏ��������܂�
 */
//------------------------------------------------------------------
u32 Random::Initialize(void)
{
  u32 seed = CreateGeneralSeed();
  mTinymt.Initialize(seed);
  return seed;
}

//------------------------------------------------------------------
/**
 * @name     RandomInitialize
 * @brief    TinyMT�ɂ�闐�������������܂�
 * @param    seed    �����l�Ƃ��Đݒ肷�闐���̎�
 */
//------------------------------------------------------------------
void Random::Initialize(const u32 seed)
{
  mTinymt.Initialize(seed);
}

//------------------------------------------------------------------
/**
 * @name     RandomInitialize
 * @brief    TinyMT�ɂ�闐�������������܂�
 * @param    seed    �����l�Ƃ��Đݒ肷�闐���̎�
 */
//------------------------------------------------------------------
void Random::Initialize(const u64 seed)
{
  mTinymt.Initialize((const u32*)&seed, 2);
}


//------------------------------------------------------------------
/**
* @name     RandomInitialize
* @brief    TinyMT�ɂ�闐�������������܂�
* @param    pContext    �����l�Ƃ��Đݒ肷��R���e�L�X�g
*/
//------------------------------------------------------------------
void Random::Initialize(const u32* pContext)
{
  mTinymt.RestoreState((const nn::math::TinyMt::State*)pContext);
}
void Random::Initialize(const Context* pContext)
{
  mTinymt.RestoreState((const nn::math::TinyMt::State*)pContext);
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    TinyMT����
 * @return   u32�^ 32bit�̃����_���l
 */
//------------------------------------------------------------------
u32 Random::Next(void)
{
  return mTinymt.GenerateRandomU32();
}

//------------------------------------------------------------------
/**
 * @name     Next64
 * @brief    TinyMT����
 * @return   u64�^ 64bit�̃����_���l
 */
//------------------------------------------------------------------
u64 Random::Next64(void)
{
  return mTinymt.GenerateRandomU64();
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    TinyMT���� �͈͎w��
 * @param    maxValue  �ő�i�܂܂�Ȃ��j
 * @return   u32�^ 32bit�̃����_���l
 */
//------------------------------------------------------------------
u32 Random::Next(const u32 maxValue)
{
  if(0xFFFF > maxValue){
    return mTinymt.GenerateRandomN(maxValue);
  }
  return (Next() % maxValue);
}


//----------------------------------------------------------------------------
/**
 *  @brief  �R���e�L�X�g�𓾂�
 *  @param  �R���e�L�X�g
 */
//----------------------------------------------------------------------------
void Random::GetContext(u32* pmt)
{
  mTinymt.SaveState((nn::math::TinyMt::State*)pmt);
}

void Random::GetContext(Context* pmt)
{
  mTinymt.SaveState((nn::math::TinyMt::State*)pmt);
}



//----------------------------------------------------------------------------
/**
 *  @brief  �R���e�L�X�g��^����
 *  @param  �R���e�L�X�g
 */
//----------------------------------------------------------------------------
void Random::SetContext(const u32* pmt)
{
  mTinymt.RestoreState((const nn::math::TinyMt::State*)pmt);
}

void Random::SetContext(const Context* pmt)
{
  mTinymt.RestoreState((const nn::math::TinyMt::State*)pmt);
}



//----------------------------------------------------------------------------
/**
 *  @brief  �C�V������ ��̐���
 *  @return u32�^�̗����̎�
 */
//----------------------------------------------------------------------------

u32 Random::CreateGeneralSeed(void)
{
#if 0
  nn::fnd::DateTime date = nn::fnd::DateTime::GetNow();
  nn::fnd::TimeSpan spm = nn::cfg::GetUserTimeOffset();

  u32 seed1;
  u32 seed2;

  seed1 = date.GetYear() * date.GetMilliSecond()  + date.GetMonth() * date.GetDay() *  date.GetHour() *  date.GetMinute() * date.GetSecond();
  seed2 = seed1 + spm.GetNanoSeconds() * spm.GetMicroSeconds() * spm.GetMilliSeconds() + spm.GetSeconds() * spm.GetMinutes() * spm.GetHours() + spm.GetDays();

  Random mt;

  mt.Initialize(seed2);
  seed1 = mt.Next();
  NN_LOG("seedenn %d\n",seed1);
  return seed1;

#endif
  // 2000-01-01 00:00:00.000 ����̌o�ߎ��Ԃ���A
  // UserTimeOffset ������������
  nn::fnd::TimeSpan now = nn::fnd::DateTime::GetNow()  - nn::fnd::DateTime() - nn::cfg::GetUserTimeOffset();
  u32 seedmaster = static_cast<u32>(now.GetMilliSeconds()); 

  NN_LOG("seedmaster %d\n",seedmaster);
  return seedmaster;
}

Random::~Random()
{
  mTinymt.Finalize();
}



//------------------------------------------------------------------
/**
 * @name     MTRandom�R���X�g���N�^
 * @brief    �������܂���B Initialize���Ă΂Ȃ���Next�̂Ƃ���ASSERT�łƂ܂�܂�
             has�����Ƃ��ɏ���ɃR���X�g���N�^������̂�����������߂ł�
 */
//------------------------------------------------------------------

MTRandom::MTRandom(void)
{
}

//------------------------------------------------------------------
/**
 * @name     Initialize
 * @brief    MT�ɂ��32bit���������������܂�
                ���CreateGeneralSeed�ō쐬
 * @return   ��Ɏg�p�����ϐ���Ԃ��܂� ������g���Γ��������𐶐����܂�
 */
//------------------------------------------------------------------
u32 MTRandom::Initialize(void)
{
  u32 seed32 = Random::CreateGeneralSeed();
  mMT.Initialize( seed32 );
  return seed32;
}


//------------------------------------------------------------------
/**
 * @name     Initialize
 * @brief    MT�ɂ��32bit���������������܂�
 * @param    seed    �����l�Ƃ��Đݒ肷�闐���̎�
 */
//------------------------------------------------------------------
void MTRandom::Initialize(const u32 seed)
{
  mMT.Initialize( seed );
}

//------------------------------------------------------------------
/**
 * @name     Initialize
 * @brief    MT�ɂ��64bit���������������܂�
 * @param    seed    �����l�Ƃ��Đݒ肷�闐���̎�
 */
//------------------------------------------------------------------
void MTRandom::Initialize(const u64 seed)
{
  mMT.Initialize( (const u32*)&seed,2 );
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    MT�ɂ��32bit�����擾�֐�
 * @return   uint�^ 32bit�̃����_���l
 */
//------------------------------------------------------------------
u32 MTRandom::Next(void)
{
  return mMT.GenerateRandomU32();
}

//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    MT�ɂ��64bit�����擾�֐�
 * @return   uint�^ 64bit�̃����_���l
 */
//------------------------------------------------------------------
u64 MTRandom::Next64(void)
{
  return mMT.GenerateRandomU64();
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    MT�ɂ��32bit�����擾�֐� �͈͎w��
 * @param    maxValue  �ő�i�܂܂�Ȃ��j
 * @return   uint�^ 32bit�̃����_���l
 */
//------------------------------------------------------------------
u32 MTRandom::Next(u32 maxValue)
{
  if(0xFFFF > maxValue){
    return mMT.GenerateRandomN(maxValue);
  }
  return (Next() % maxValue);
}


MTRandom::~MTRandom()
{
  mMT.Finalize();
}


}
}
