//============================================================================================
/**
 * GAME FREAK inc.
 * @file    gfl_math_random.cpp
 * @brief   乱数関数
            2011.8.19にM系乱数に変更
 * @author  k.ohno
 * @date    2010.12.14
 */
//============================================================================================

#include <string.h>
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_math_random.h>

#if defined(GF_PLATFORM_WIN32)
#include <ctime>
#include <string.h>
#include <random>
#elif defined(GF_PLATFORM_CTR)
#include "nn/nstd.h"
#include "nn/fnd.h"
#include "nn/cfg.h"
#include "nn/crypto.h"

#endif


namespace gfl2{
namespace math{

/**
 * @name     Randomコンストラクタ
 * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
 *           hasしたときに勝手にコンストラクタが走るのを避けたいためです
 */
Random::Random(void)
{
#if GFL_DEBUG
  _bInit = false;
#endif//GFL_DEBUG
}

/**
 * @name     Random::Initialize
 * @brief    TinyMTによる乱数を初期化します
 *           種はエントロピーとSHA1で初期化します
 */
void Random::Initialize(void)
{
  Initialize( CreateGeneralSeed() );
#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name     Random::Initialize
 * @brief    TinyMTによる乱数を初期化します
 * @param    const u32 seed 初期値として設定する乱数の種
 */
void Random::Initialize(const u32 seed)
{
  u32 seed2 = seed & 0xffffffff;

  mtinymt32.mat1=0x8f7011ee;
  mtinymt32.mat2=0xfc78ff1f;
  mtinymt32.tmat=0x3793fdff;

  tinymt32_init(&mtinymt32, seed2);

#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name       Random::Initialize
 * @brief      TinyMTによる乱数を初期化します
 * @param[in]  const tinymt32_t* pContext  初期値として設定するコンテキスト
 */
void Random::Initialize(const tinymt32_t* pContext)
{
  SetContext(pContext);
#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name     Random::Initialize
 * @brief    TinyMTによる乱数を初期化します
 * @param    const Random::State Resumeする内部状態
 */
void Random::Initialize(const Random::State state)
{
  // キー設定
  mtinymt32.mat1=0x8f7011ee;
  mtinymt32.mat2=0xfc78ff1f;
  mtinymt32.tmat=0x3793fdff;

  mtinymt32.status[0] = state.status[0];
  mtinymt32.status[1] = state.status[1];
  mtinymt32.status[2] = state.status[2];
  mtinymt32.status[3] = state.status[3];
#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name     Random::Next
 * @brief    TinyMTによる32bit乱数取得関数
 * @return   u32 32bitの乱数値
 */
u32 Random::Next(void)
{
#if GFL_DEBUG  //デバッグ時は初期化してないとASSERT
  GFL_ASSERT(_bInit);
  //本番ではいらない
#endif

  return tinymt32_generate_uint32(&mtinymt32);
}

/**
 * @name     Random::Next
 * @brief    TinyMTによる32bit乱数取得関数 範囲指定
 * @param    const u32 maxValue  最大値（含まれない）
 * @return   u32 32bitのr乱数値
 */
u32 Random::Next(const u32 maxValue)
{
  return (Next() % maxValue);
}

/**
 *  @name        Random::GetContext
 *  @brief       コンテキストを得る
 *  @param[out]  tinymt32_t* コンテキスト
 */
void Random::GetContext(tinymt32_t* pmt)
{
#if defined(GF_PLATFORM_WIN32)
  memcpy(pmt,&mtinymt32,sizeof(tinymt32_t));
#elif defined(GF_PLATFORM_CTR)
	nn::nstd::MemCpy(pmt,&mtinymt32,sizeof(tinymt32_t));
#endif

}

/**
 *  @name       Random::SetComtext
 *  @brief      コンテキストを与える
 *  @param[in]  const tinymt32_t* コンテキスト
 */
void Random::SetContext(const tinymt32_t *pmt)
{
#if defined(GF_PLATFORM_WIN32)
	memcpy(&mtinymt32 , pmt ,sizeof(tinymt32_t));
#elif defined(GF_PLATFORM_CTR)
	nn::nstd::MemCpy(&mtinymt32 , pmt ,sizeof(tinymt32_t));
#endif

}

/**
 *  @name   Random::CreateGeneralSeed
 *  @brief  種の生成 チック値と現在時刻をSHA256でハッシュ化し生成
 *  @return u32 乱数の種
 */
u32 Random::CreateGeneralSeed(void)
{
#if defined(GF_PLATFORM_CTR)
  struct SeedKey{
    u64 tick;
    nn::fnd::TimeSpan now;
  }; // key storage type

  SeedKey key;
  key.tick = nn::os::Tick::GetSystemCurrent();
  key.now = nn::fnd::DateTime::GetNow()
    - nn::fnd::DateTime() - nn::cfg::GetUserTimeOffset();

  c8 buf[nn::crypto::Sha256Context::HASH_SIZE]; // <- outbuf of sha256

  nn::crypto::Sha256Context ctx;
  ctx.Initialize();
  ctx.Update(&key, sizeof(key));
  ctx.GetHash(&buf);
  ctx.Finalize();

  s32 res; // result value
  memcpy(&res, buf, sizeof(res));
#else
  std::random_device rd; // C++11
  u32 res = rd();
#endif
  ARAKI_PRINT("seed : %d\n", res);
  return res;
}

/**
 *  @name   Random::SaveState
 *  @brief  内部状態の保存 16byte
 *  @return const Random::State Resumeに必要な内部状態
 */
const Random::State Random::SaveState()
{
  Random::State state;
  state.status[0] = mtinymt32.status[0];
  state.status[1] = mtinymt32.status[1];
  state.status[2] = mtinymt32.status[2];
  state.status[3] = mtinymt32.status[3];
  return state;
}

/**
 * @name     Random64コンストラクタ
 * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
 *           hasしたときに勝手にコンストラクタが走るのを避けたいためです
 */
Random64::Random64(void)
{
#if GFL_DEBUG
  _bInit = false;
#endif//GFL_DEBUG
}

/**
 * @name     Random64::Initialize
 * @brief    TinyMTによる乱数を初期化します
 * @param    const u64 seed 初期値として設定する乱数の種
 */
void Random64::Initialize(const u64 seed)
{

  // https://github.com/MersenneTwister-Lab/TinyMT/blob/master/readme-jp.html からコピー
  // MTToolBoxを使ってちゃんと設定したい
  mtinymt64.mat1=0xfa051f40;
  mtinymt64.mat2=0xffd0fff4;
  mtinymt64.tmat=0x58d02ffeffbfffbc;

  tinymt64_init(&mtinymt64, seed);

#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name     Random64::Initialize
 * @brief    TinyMTによる乱数を初期化します
 * @param    const tinymt64_t* pContext  初期値として設定するコンテキスト
 */
void Random64::Initialize(const tinymt64_t* pContext)
{
  SetContext(pContext);
#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name     Random64::Initialize
 * @brief    TinyMT64による乱数を初期化します
 * @param    Random64::State Resumeする内部状態
 */
void Random64::Initialize(Random64::State state)
{
  // キー設定
  mtinymt64.mat1=0xfa051f40;
  mtinymt64.mat2=0xffd0fff4;
  mtinymt64.tmat=0x58d02ffeffbfffbc;

  mtinymt64.status[0] = state.status[0];
  mtinymt64.status[1] = state.status[1];
#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG
}

/**
 * @name     Random64::Next
 * @brief    TinyMTによる64bit乱数を取得します
 * @return   u64 64bitの乱数値
 */
u64 Random64::Next(void)
{
#if GFL_DEBUG  //デバッグ時は初期化してないとASSERT
  GFL_ASSERT(_bInit);
  //本番ではいらない
#endif

  return tinymt64_generate_uint64(&mtinymt64);
}

/**
 * @name     Random64::Next
 * @brief    TinyMTによる64bit乱数取得関数 範囲指定
 * @param    const u64 maxValue  最大値（含まれない）
 * @return   u64 [0, maxValue - 1]上の乱数値
 */
u64 Random64::Next(const u64 maxValue)
{
  return (Next() % maxValue);
}

/**
 *  @name       Random64::GetContext
 *  @brief      コンテキストを得る
 *  @param[out] tinymt64_t* コンテキスト
 */
void Random64::GetContext(tinymt64_t* pmt)
{
#if defined(GF_PLATFORM_WIN32)
  memcpy(pmt,&mtinymt64,sizeof(tinymt64_t));
#elif defined(GF_PLATFORM_CTR)
  nn::nstd::MemCpy(pmt,&mtinymt64,sizeof(tinymt64_t));
#endif
}

/**
 *  @name       Random64::SetContext
 *  @brief      コンテキストを与える
 *  @param[in]  const tinymt64_t* コンテキスト
 */
void Random64::SetContext(const tinymt64_t *pmt)
{
#if defined(GF_PLATFORM_WIN32)
  memcpy(&mtinymt64, pmt, sizeof(tinymt64_t));
#elif defined(GF_PLATFORM_CTR)
  nn::nstd::MemCpy(&mtinymt64, pmt, sizeof(tinymt64_t));
#endif
}

/**
 *  @name   Random64::SaveState
 *  @brief  内部状態の保存 (16byte)
 *  @return const Random64::State Resumeに必要な内部状態
 */
const Random64::State Random64::SaveState()
{
  Random64::State state;
  state.status[0] = mtinymt64.status[0];
  state.status[1] = mtinymt64.status[1];
  return state;
}

/**
 * @name     SFMTMTRandomコンストラクタ
 * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
 *           hasしたときに勝手にコンストラクタが走るのを避けたいためです
 */
SFMTRandom::SFMTRandom(void)
{
#if GFL_DEBUG
  _bInit = false;
#endif//GFL_DEBUG
}

/**
 * @name     SFMTRandom::Initialize
 * @brief    SFMTによる32bit乱数を初期化します
 *           種はエントロピーとSHA1で初期化します
 * @return   種に使用した変数を返します これを使えば同じ乱数を生成します
 */
u32 SFMTRandom::Initialize(void)
{
  u32 seed32 = Random::CreateGeneralSeed();

  Initialize( seed32 );
  return seed32;
}

/**
 * @name     SFMTRandom::Initialize
 * @brief    SFMTによる乱数系列を初期化します
 * @param    seed 初期値として設定する乱数の種
 */
void SFMTRandom::Initialize(const u32 seed)
{
#if GFL_DEBUG
  _bInit = true;
#endif//GFL_DEBUG

  sfmt_init_gen_rand(&msfmt, seed);
}

/**
 * @name     SFMTRandom::Next
 * @brief    SFMTによる32bit乱数取得関数
 * @return   u32 32bitの乱数値
 */
u32 SFMTRandom::Next(void)
{
  return sfmt_genrand_uint32(&msfmt);
}

/**
 * @name     SFMTRandom::Next
 * @brief    SFMTによる32bit乱数取得関数 範囲指定
 * @param    const u32 maxValue  最大値（含まれない）
 * @return   u32 32bitの乱数値
 */
u32 SFMTRandom::Next(const u32 maxValue)
{
  return (Next() % maxValue);
}

/**
 * @name     SFMTRandom::Next
 * @brief    SFMTによる64bit乱数取得関数
 * @return   u64 64bitの乱数値
 */
u64 SFMTRandom::Next64(void)
{
  return sfmt_genrand_uint64(&msfmt);
}

/**
 * @name     SFMTRandom::Next
 * @brief    SFMTによる64bit乱数取得関数 範囲指定
 * @param    const u64 maxValue  最大値（含まれない）
 * @return   u64 64bitの乱数値
 */
u64 SFMTRandom::Next64(const u64 maxValue)
{
  return (Next64() % maxValue);
}

}
}
