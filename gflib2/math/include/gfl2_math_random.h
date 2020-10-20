#ifndef __GFL_MATH_RANDOM_H__
#define __GFL_MATH_RANDOM_H__
//============================================================================================
/**
 *  GAME FREAK inc.
 *  @file gfl_math_random.h
 *  @brief  乱数管理
 *  @author k.ohno
 *  @date 2010.12.14
 *
 */
//============================================================================================


#include <math/include/gfl2_tinymt32.h>
#include <math/include/gfl2_tinymt64.h>
#include <math/include/SFMT/SFMT.h>

namespace gfl2{
namespace math{

  //------------------------------------------------------------------
  /**
   * @class    Random
   * @brief    TinyMTによる32bit乱数系列のラッパークラス
   * @detail   インスタンスを作成した後、いずれかのInitialize関数を呼んで下さい
   *           乱数系列の周期：2^127-1  状態空間：127bit
   */
  //------------------------------------------------------------------
  class Random {

    GFL_FORBID_COPY_AND_ASSIGN(Random); //コピーコンストラクタ＋代入禁止

  public:
    /**
     * @struct  State
     * @brief   Randomクラスの状態を復元する状態変数
     */
    struct State
    {
      u32 status[4];
    };

  private:
#if GFL_DEBUG  //デバッグ時は初期化してないとASSERT
    bool _bInit;
#endif

  private:
    static const int LOW_ENTROPY_DATA_SIZE = 32; ///< エントロピーをとってくるバッファの必要サイズ

    
  private:
    tinymt32_t mtinymt32;  ///< TinyMT

    
  public:
    /**
     * @name     Randomコンストラクタ
     * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
     *           hasしたときに勝手にコンストラクタが走るのを避けたいためです
     */
    Random(void);

    /**
     * @name     Random::Initialize
     * @brief    TinyMTによる32bit乱数系列を初期化します
     *           種はエントロピーとSHA1で初期化します
     */
    void Initialize(void);

    /**
     * @name     Random::Initialize
     * @brief    TinyMTによる32bit乱数系列を初期化します
     * @param    const u32 seed    初期値として設定する乱数の種
     */
    void Initialize(const u32 seed);

    /**
     * @name     Random::Initialize
     * @brief    TinyMTによる32bit乱数系列を初期化します
     * @param    const tinymt32_t* pContext  初期値として設定するコンテキスト
     */
    void Initialize(const tinymt32_t* pContext);
 
    /**
     * @name     Random::Initialize
     * @brief    TinyMTによる32bit乱数系列を初期化します
     * @param    const Random::State state Resumeする内部状態
     */
    void Initialize(const State state);

    /**
     * @name     Random::Next
     * @brief    TinyMTによる32bit乱数取得関数
     * @return   u32 32bitの乱数値
     */
    u32 Next(void);

    /**
     * @name     Random::Next
     * @brief    TinyMTによる32bit乱数取得関数 範囲指定
     * @param    const u32 maxValue  最大値（含まれない）
     * @return   u32 32bitの乱数値
     */
    u32 Next(const u32 maxValue);

    /**
     *  @name       Random::GetContext
     *  @brief      コンテキストを得る
     *  @param[out] tinymt32_t* コンテキスト
     */
    void GetContext(tinymt32_t* pContext);

    /**
     *  @name       Random::GetContext
     *  @brief      コンテキストを与える
     *  @param[in]  const tinymt32_t* コンテキスト
     */
    void SetContext(const tinymt32_t* pContext);

    /**
     *  @name   Random::CreateGeneralSeed
     *  @brief  種の生成 チック値と現在時刻をSHA256でハッシュ化し生成
     *  @return u32 乱数の種
     */
    static u32 CreateGeneralSeed(void);

    /**
     *  @name   Random::SaveState
     *  @brief  内部状態の保存
     *  @return const Random::State Resumeに必要な内部状態
     */
    const State SaveState();

  };

  //------------------------------------------------------------------
  /**
   * @class    Random64
   * @brief    TinyMTによる64bit乱数系列のラッパークラス
   * @detail   インスタンスを作成した後、いずれかのInitialize関数を呼んで下さい
   *           乱数系列の周期：2^127-1  状態空間：127bit
   */
  //------------------------------------------------------------------
  class Random64 {

    GFL_FORBID_COPY_AND_ASSIGN(Random64); //コピーコンストラクタ＋代入禁止

  public:
    /**
     * @struct  State
     * @brief   Random64クラスの状態を復元する状態変数
     */
    struct State
    {
      u64 status[2];
    };

  private:
#if GFL_DEBUG  //デバッグ時は初期化してないとASSERT
    bool _bInit;
#endif

  private:
    static const int LOW_ENTROPY_DATA_SIZE = 64; ///< エントロピーをとってくるバッファの必要サイズ

    
  private:
    tinymt64_t mtinymt64;  ///< TinyMT

    
  public:
    /**
     * @name     Random64コンストラクタ
     * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
     *           hasしたときに勝手にコンストラクタが走るのを避けたいためです
     */
    Random64(void);

    /**
     * @name     Random64::Initialize
     * @brief    TinyMTによる64bit乱数系列を初期化します
     * @param    const u64 seed  初期値として設定する乱数の種
     */
    void Initialize(const u64 seed);

    /**
     * @name     Random64::Initialize
     * @brief    TinyMTによる64bit乱数系列を初期化します
     * @param    const tinymt64_t* pContext  初期値として設定するコンテキスト
     */
    void Initialize(const tinymt64_t* pContext);

    /**
     * @name     Random64::Initialize
     * @brief    TinyMTによる64bit乱数系列を初期化します
     * @param    const Random64::State state Resumeする内部状態
     */
    void Initialize(const State state);

    /**
     * @name     Random64::Next
     * @brief    TinyMTによる64bit乱数取得関数
     * @return   u64型 64bitの乱数値
     */
    u64 Next(void);

    /**
     * @name     Random64::Next
     * @brief    TinyMTによる64bit乱数取得関数 範囲指定
     * @param    const u64 maxValue  最大値（含まれない）
     * @return   u64型 64bitの乱数値
     */
    u64 Next(const u64 maxValue);

    /**
     *  @name       Random64::GetContext
     *  @brief      コンテキストを得る
     *  @param[out] tinymt64_t* コンテキスト
     */
    void GetContext(tinymt64_t* pContext);

    /**
     *  @name       Random64::SetContext
     *  @brief      コンテキストを与える
     *  @param[out] const tinymt64_t* コンテキスト
     */
    void SetContext(const tinymt64_t* pContext);

    /**
     *  @name   Random64::SaveState
     *  @brief  内部状態の保存
     *  @return const Random64::State Resumeに必要な内部状態
     */
    const State SaveState();
  };

  //------------------------------------------------------------------
  /**
   * @class    SFMTRandom
   * @brief    SFMTによる32/64bit乱数系列のラッパークラス
   * @detail   インスタンスを作成した後、いずれかのInitialize関数を呼んで下さい
   *           乱数系列の周期：2^19937-1  状態空間：2.5kb
   *           固定パラメータ：SFMT-params19937
   */
  //------------------------------------------------------------------
  class SFMTRandom {

    GFL_FORBID_COPY_AND_ASSIGN(SFMTRandom); //コピーコンストラクタ＋代入禁止

  private:
#if GFL_DEBUG  //デバッグ時は初期化してないとASSERT
    bool _bInit;
#endif

    sfmt_t msfmt;

  public:
    /**
     * @name     SFMTRandomコンストラクタ
     * @brief    何もしません。 Initializeを呼ばないとNextのときにASSERTでとまります
     *           hasしたときに勝手にコンストラクタが走るのを避けたいためです
     */
    SFMTRandom(void);

    /**
     * @name     SFMTRandom::Initialize
     * @brief    SFMTによる乱数系列を初期化します
     * @return   種に使用した変数を返します これを使えば同じ乱数を生成します
     */
    u32 Initialize(void);

    /**
     * @name     SFMTRandom::Initialize
     * @brief    MTによる32bit乱数系列を初期化します
     * @param    const u32 seed  初期値として設定する乱数の種
     */
    void Initialize(const u32 seed);

    /**
     * @name     SFMTRandom::Next
     * @brief    SFMTによる32bit乱数取得関数
     * @return   u32 32bitの乱数値
     */
    u32 Next(void);

    /**
     * @name     SFMTRandom::Next
     * @brief    SFMTによる32bit乱数取得関数 範囲指定
     * @param    const u32 maxValue  最大値（含まれない）
     * @return   u32 32bitの乱数値
     */
    u32 Next(const u32 maxValue);

    /**
     * @name     SFMTRandom::Next
     * @brief    SFMTによる64bit乱数取得関数
     * @return   u64 64bitの乱数値
     */
    u64 Next64(void);

    /**
     * @name     SFMTRandom::Next
     * @brief    SFMTによる64bit乱数取得関数 範囲指定
     * @param    const u64 maxValue  最大値 （含まれない）
     * @return   u64 64bitの乱数値
     */
    u64 Next64(const u64 maxValue);
  };

} // math
} // gfl2

#endif // __GFL_MATH_RANDOM_H__
