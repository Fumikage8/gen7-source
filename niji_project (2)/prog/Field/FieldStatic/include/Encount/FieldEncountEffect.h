//======================================================================
/**
 * @file FieldEncountEffect.h
 * @date 2015/09/11 12:37:38
 * @author saita_kazuki
 * @brief 戦闘導入演出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 *
 * ・演出の種類を増やす
 * １、EffectKindに定義を増やしてください
 * ２、その演出用の初期化関数、実行前関数を定義してください
 * ３、cppのINITIALIZE_FUNC、START_PRE_FUNCに関数を追加してください
 * ４、cppのgetDemoIDメソッドのswitch文に、その演出のIDを動的ヘッダシステムのマクロを介して返すようcase文を足してください。
       IDは<arc_index/demo_seq_script.gaix>に定義されています
 * ５、EffectKindに追加した定義をバトル側のbattle_effect.xlsxのFieldEffIDで参照できるようにしてください。
       BATTLE_SETUP_PARAM::btlEffData::enceff_fldにEffectKindに対応した値が格納され、このクラスに渡ってきます。
       battle_effect.xlsxのコンバートは有泉さんが担当しています。
 */
//======================================================================

#if !defined __FIELD_ENCOUNT_EFFECT_H_INCLUDED__
#define __FIELD_ENCOUNT_EFFECT_H_INCLUDED__
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <ro/include/gfl2_RoManager.h>
#include <math/include/gfl2_Vector4.h>


// 前方参照
namespace AppLib { namespace Fade {
  class DemoFade;
}}
namespace System {
  class nijiAllocator;
}
namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {
  class ResourceNode;
}}}}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

/**
 * @class EncountEffect
 * @brief 戦闘導入演出
 */
class EncountEffect
{
  GFL_FORBID_COPY_AND_ASSIGN( EncountEffect );

public:

  // フラッシュのデフォルト値
  static const u32  DEFAULT_FLASH_TOTAL_SYNC  = 8;      ///< フラッシュの合計フレーム
  static const u32  DEFAULT_FLASH_NUM         = 2;      ///< フラッシュの回数
  static const s32  DEFAULT_FLASH_INTERVAL    = -2;     ///< フラッシュの間隔

  /**
   * @brief エフェクト種類
   */
  enum EffectKind
  {
    EFFECT_KIND_DEFAULT,    ///< デフォルト

    EFFECT_KIND_MAX,
    EFFECT_KIND_INVALID = EFFECT_KIND_MAX,
  };

  /**
   * @brief オプション
   */
  enum Option
  {
    OPTION_NONE                   = 0,          ///< なし
  };

  /**
   * @brief 初期化情報
   */
  struct Description
  {
    Description()
      : effectKind( EFFECT_KIND_INVALID)
      , option( 0)
    {}

    EffectKind      effectKind;
    u32             option;
  };

public:

  /**
   * @brief コンストラクタ
   */
  EncountEffect();

  /**
   * @brief デストラクタ
   */
  virtual ~EncountEffect();

  /**
   * @brief 初期化
   * @param pParent 親ヒープ
   * @param desc 初期化情報
   */
  void Initialize( gfl2::heap::HeapBase* pParent, const Description& desc);

  /**
   * @brief 破棄
   */
  void Terminate();

  /**
   * @brief 再生前準備
   * @note キャプチャー処理の後で呼ぶ
   */
  void Setup();

  /**
   * @brief 開始
   * @param isRequestFlash trueでフラッシュを開始
   * @param totalSync 合計時間
   * @param num 回数
   * @param interval 間隔
   */
  void Start( bool isRequestFlash = true, u32 totalSync = DEFAULT_FLASH_TOTAL_SYNC, u32 flashNum = DEFAULT_FLASH_NUM, s32 flashInterval = DEFAULT_FLASH_INTERVAL );

  /**
   * @brief シーケンス再生が開始しているか
   * @param isRequestFlash trueでフラッシュ更新も待つ
   * @retval true 開始している
   * @retval false まだ開始していない
   */
  bool IsStart( bool isRequestFlash = true );

  /**
   * @brief フィールドキャプチャーが必要か
   * @retval true 必要
   * @retval false 不要
   * @note Initialize後に有効な値が取れます
   */
  bool IsNeedFieldCapture() const ;

  /**
   * @brief フィールドキャプチャーリクエスト
   */
  void RequestFieldCapture();

  /**
   * @brief シーケンスプレイヤーの初期化完了後にシーケンス再生を待機させるモードの設定
   * @param trueにするとシーケンスプレイヤー初期化完了後にシーケンスが再生されなくなる
   * @note デフォルトはfalseです。設定する場合はInitialize後に呼んでください
   * @note trueに設定した場合、明示的にfalseを設定しないとシーケンスが再生されません
   */
  void SetSequenceStartupWaitMode( bool isEnable);

  /**
   * @brief 初期化されているか
   * @retval true 初期化されている
   * @retval false 未初期化
   */
  bool IsInitialized() const ;

  /**
   * @brief 終了待ち
   * @retval true 終了
   * @retval false 未初期化
   */
  bool IsFinish() const ;

  /**
   * @brief 上画面フラッシュ開始
   */
  void StartFlash( u32 totalSync = DEFAULT_FLASH_TOTAL_SYNC, u32 flashNum = DEFAULT_FLASH_NUM, s32 flashInterval = DEFAULT_FLASH_INTERVAL );

  /**
   * @brief 上画面フラッシュ更新
   * @retval true 終了した
   * @retval false 実行中
   */
  bool UpdateFlash();


#if PM_DEBUG
  AppLib::Fade::DemoFade* GetDemoFade() const { return m_pDemoFade; }
#endif // PM_DEBUG

private:

  void initialize();
  void terminate();

  void createHeap( gfl2::heap::HeapBase* pParent);
  void deleteHeap();

  void createDemoFade();
  void deleteDemoFade();

  void createCaptureTextureResourceNode();

  u32 getDemoID( EffectKind kind);

  void requestFadeOutLower();

  void initializeFlashParameter( u32 totalSync, u32 flashNum, s32 flashInterval );
  gfl2::math::Vector4 getFlashColor( f32 alpha ) const ;

  void initializeFunc_Default();
  typedef void (EncountEffect::*InitializeFunc)();

  void startPreFunc_Default();
  typedef void (EncountEffect::*StartPreFunc)();

private:

  // @fix 戦闘導入演出シーケンスで必要な分のメモリサイズに調整
  static const u32                                            HEAP_SIZE_DEMO_SYSTEM = 0x7D000;    ///< デモフェードのシステムヒープサイズ
  static const u32                                            HEAP_SIZE_DEMO_DEVICE = 0x1B0000;   ///< デモフェードのデバイスヒープサイズ
  static const InitializeFunc                                 INITIALIZE_FUNC[ EFFECT_KIND_MAX ]; ///< 種類ごとのデモ開始前処理を行う関数
  static const StartPreFunc                                   START_PRE_FUNC[ EFFECT_KIND_MAX ];  ///< 種類ごとの初期化処理を行う関数

  Description                                                 m_desc;                             ///< 初期化情報
  gfl2::heap::HeapBase*                                       m_pHeap;                            ///< ヒープ
  AppLib::Fade::DemoFade*                                     m_pDemoFade;                        ///< デモフェード
  System::nijiAllocator*                                      m_pAllocator;                       ///< アロケーター
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pTextureResourceNode;             ///< テクスチャリソースノード
  bool                                                        m_isCapture;                        ///< キャプチャーフラグ
  bool                                                        m_isInitialized;                    ///< 初期化されたか
  bool                                                        m_isStartFlash;                     ///< フラッシュが開始されたか
  u32                                                         m_seq;                              ///< シーケンス
  u32                                                         m_flashSync;                        ///< フラッシュフレーム
  u32                                                         m_flashNum;                         ///< フラッシュ回数
  s32                                                         m_flashInterval;                    ///< フラッシュ間隔
  s32                                                         m_flashIntervalCnt;                 ///< フラッシュ間隔カウント
};

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_ENCOUNT_EFFECT_H_INCLUDED__
