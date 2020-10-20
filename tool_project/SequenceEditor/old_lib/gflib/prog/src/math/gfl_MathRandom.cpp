//============================================================================================
/**
 *	GAME FREAK inc.
 * @file    gfl_MathRandom.cpp
 * @brief   乱数関数
            2011.8.19にM系乱数に変更
            2012.9.25にCTRSDKに変更
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

//------------------------TinyMT  2の127乗-1


//------------------------------------------------------------------
/**
 * @name     Randomコンストラクタ
 * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
             hasしたときに勝手にコンストラクタが走るのを避けたいためです
 */
//------------------------------------------------------------------

Random::Random(void)
{
}

//------------------------------------------------------------------
/**
 * @name     RandomInitialize
 * @brief    TinyMTによる乱数を初期化します
             種はエントロピーとSHA1で初期化します
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
 * @brief    TinyMTによる乱数を初期化します
 * @param    seed    初期値として設定する乱数の種
 */
//------------------------------------------------------------------
void Random::Initialize(const u32 seed)
{
  mTinymt.Initialize(seed);
}

//------------------------------------------------------------------
/**
 * @name     RandomInitialize
 * @brief    TinyMTによる乱数を初期化します
 * @param    seed    初期値として設定する乱数の種
 */
//------------------------------------------------------------------
void Random::Initialize(const u64 seed)
{
  mTinymt.Initialize((const u32*)&seed, 2);
}


//------------------------------------------------------------------
/**
* @name     RandomInitialize
* @brief    TinyMTによる乱数を初期化します
* @param    pContext    初期値として設定するコンテキスト
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
 * @brief    TinyMT乱数
 * @return   u32型 32bitのランダム値
 */
//------------------------------------------------------------------
u32 Random::Next(void)
{
  return mTinymt.GenerateRandomU32();
}

//------------------------------------------------------------------
/**
 * @name     Next64
 * @brief    TinyMT乱数
 * @return   u64型 64bitのランダム値
 */
//------------------------------------------------------------------
u64 Random::Next64(void)
{
  return mTinymt.GenerateRandomU64();
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    TinyMT乱数 範囲指定
 * @param    maxValue  最大（含まれない）
 * @return   u32型 32bitのランダム値
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
 *  @brief  コンテキストを得る
 *  @param  コンテキスト
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
 *  @brief  コンテキストを与える
 *  @param  コンテキスト
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
 *  @brief  任天堂推奨 種の生成
 *  @return u32型の乱数の種
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
  // 2000-01-01 00:00:00.000 からの経過時間から、
  // UserTimeOffset を除いたもの
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
 * @name     MTRandomコンストラクタ
 * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
             hasしたときに勝手にコンストラクタが走るのを避けたいためです
 */
//------------------------------------------------------------------

MTRandom::MTRandom(void)
{
}

//------------------------------------------------------------------
/**
 * @name     Initialize
 * @brief    MTによる32bit乱数を初期化します
                種はCreateGeneralSeedで作成
 * @return   種に使用した変数を返します これを使えば同じ乱数を生成します
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
 * @brief    MTによる32bit乱数を初期化します
 * @param    seed    初期値として設定する乱数の種
 */
//------------------------------------------------------------------
void MTRandom::Initialize(const u32 seed)
{
  mMT.Initialize( seed );
}

//------------------------------------------------------------------
/**
 * @name     Initialize
 * @brief    MTによる64bit乱数を初期化します
 * @param    seed    初期値として設定する乱数の種
 */
//------------------------------------------------------------------
void MTRandom::Initialize(const u64 seed)
{
  mMT.Initialize( (const u32*)&seed,2 );
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    MTによる32bit乱数取得関数
 * @return   uint型 32bitのランダム値
 */
//------------------------------------------------------------------
u32 MTRandom::Next(void)
{
  return mMT.GenerateRandomU32();
}

//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    MTによる64bit乱数取得関数
 * @return   uint型 64bitのランダム値
 */
//------------------------------------------------------------------
u64 MTRandom::Next64(void)
{
  return mMT.GenerateRandomU64();
}


//------------------------------------------------------------------
/**
 * @name     Next
 * @brief    MTによる32bit乱数取得関数 範囲指定
 * @param    maxValue  最大（含まれない）
 * @return   uint型 32bitのランダム値
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
