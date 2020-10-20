﻿//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file EncountWork.h
 *  @brief エンカウント常駐パラメータ管理モジュール
 *  @author iwasawa  
 *  @date 2012.07.28
 *
 *  @niji 必要だけど6月アルファは機能実装を一旦スルーしていい。クラス定義だけ作って置いておこう。
 *  エンカウント率は一律10%とかで計算し、上手く作業が進んで余裕があったら、この機能も組み込む
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#if !defined( FIELD_ENCOUNT_WORK_H_INCLUDED )
#define FIELD_ENCOUNT_WORK_H_INCLUDED
#pragma once

// 前方宣言
namespace GameSys{
class GameManager;
class GameData;
class DllReadThread;
}

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// field
#include "Field/FieldRo/include/Encount/FieldEncountData.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"
#include "Field/FieldStatic/include/Encount/FieldEncountEffect.h"

// conv_header
#include <niji_conv_header/field/attribute/attribute_id.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------
class EncountWork
{
  GFL_FORBID_COPY_AND_ASSIGN(EncountWork); //コピーコンストラクタ＋代入禁止
  
public:

  //----------------------------------------------------------------------------
  // 釣り関連定数
  //----------------------------------------------------------------------------
  enum {
    FISHING_CONTINUE_MAX = 9999,  ///< 釣り継続回数最大数 
  };

public:

  // コンストラクタ　デストラクタ
  EncountWork( void );
  ~EncountWork();

  //----------------------------------------------------------------------------
  /**
   *  @brief  初期化処理
   */
  //-----------------------------------------------------------------------------
  void Initialize( void );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  破棄処理
   */
  //-----------------------------------------------------------------------------
  void Terminate( void );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  現在のエンカウント率取得
   */
  //-----------------------------------------------------------------------------
  u32 GetNowEncountProb( const ENCPROB_PATTERN* data ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  現在のエンカウント率パラメータ計算
   *
   *  @param  attr        現在地のアトリビュート
   *  @param  c_move_f    グリッド移動を伴う一歩
   *  @param  c_ex_move_f 波乗り開始時などの特殊イベント一歩
   *  @param  data        エンカウント率制御パターンデータ  
   */
  //-----------------------------------------------------------------------------
  void CalcEncountProb( const bool c_move_f, const bool c_ex_move_f, const ENCPROB_PATTERN* data );

  //----------------------------------------------------------------------------
  /**
   *  @brief  マップチェンジ後の初期状態セット(フィールドが構築されてから呼び出すこと)
   */
  //-----------------------------------------------------------------------------
  void MapChangeUpdate( GameSys::GameManager* p_gman );
 
  //----------------------------------------------------------------------------
  /**
   *  @brief  フィールドバトル発生時(トレーナー・野生戦問わず)に呼び出す初期化
   */
  //-----------------------------------------------------------------------------
  void BattleCallUpdate( Attr attr );

  //----------------------------------------------------------------------------
  /**
   *  @brief  現在のエンカウント率パラメータリセット
   *
   *  @param  after_battle_f 野生戦直後のリセット時はtrue
   */
  //-----------------------------------------------------------------------------
  void EncountProbReset( Attr attr, bool isEncount, bool after_battle_f = false );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣りのクリア
   */
  /* -------------------------------------------------------------------------*/
  void ClearFishingContinueCount( void );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣りの連続回数加算
   *
   * @param add_num 加算する継続数
   */
  /* -------------------------------------------------------------------------*/
  void AddFishingContinueCount( u32 add_num = 1 );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣りの連続回数取得
   */
  /* -------------------------------------------------------------------------*/
  u16 GetFishingContinueCount( void ) const ;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief エンカウントポケモンセットの取得
   */
  /* -------------------------------------------------------------------------*/
  PokeSet& GetPokeSet( void );

#if PM_DEBUG
  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣り継続数をセット
   */
  /* -------------------------------------------------------------------------*/
  void DEBUG_SetFishingContinueCount( u32 cnt );
#endif // PM_DEBUG

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウント率リセットチェック
   *  @param  attr 現在地のアトリビュート
   *  @param  isEncount 現在のアトリビュートでエンカウントできるか
   */
  //-----------------------------------------------------------------------------
  void CheckProbReset( Attr attr, bool isEncount, u8 attrNearBGPattern );

  //----------------------------------------------------------------------------
  /**
   *  @brief  戦闘導入演出インスタンス取得
   */
  //-----------------------------------------------------------------------------
  EncountEffect* GetEncountEffect();

  //-----------------------------------------------------------------------------
  /**
   * @brief 戦闘導入演出で使用するDll読み込み
   * @param pHeap ヒープ
   */
  //-----------------------------------------------------------------------------
  void LoadEncountEffectDll( gfl2::heap::HeapBase* pHeap);

  //-----------------------------------------------------------------------------
  /**
   * @brief 戦闘導入演出で使用するDLL破棄
   */
  //-----------------------------------------------------------------------------
  void DisposeEncountEffectDll();

  //-----------------------------------------------------------------------------
  /**
   * @brief 戦闘導入演出で使用するDLLが読み込まれているか
   * @retval true 読み込まれている
   * @retval false 読み込まれていない
   */
  //-----------------------------------------------------------------------------
  bool IsLoadEncountEffectDll() const ;

  //-----------------------------------------------------------------------------
  /**
   * @brief バトル内でデモ用のDLLを破棄する関数
   */
  //-----------------------------------------------------------------------------
  void DisposeSequenceDemoDll();

  //-----------------------------------------------------------------------------
  /**
   * @brief 戦闘導入演出で使用するDll読み込み スレッド版
   * @param pHeap スレッド用ヒープ
   * @param pHeapDll DLL読み込み用ヒープ
   */
  //-----------------------------------------------------------------------------
  void StartLoadEncountEffectDllThread( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pHeapDll );

  //-----------------------------------------------------------------------------
  /**
   * @brief 戦闘導入演出で使用するDLL読み込み待ち スレッド版
   * @retval true 完了
   * @retval false 実行中
   * @note 必ずStartLoadEncountEffectDllとセットで使用する
   */
  //-----------------------------------------------------------------------------
  bool WaitLoadEncountEffectDllThread() const ;

  //-----------------------------------------------------------------------------
  /**
   * @brief 戦闘導入演出で使用するDLL破棄 スレッド版
   */
  //-----------------------------------------------------------------------------
  void DisposeEncountEffectDllThread();

private:
  u32     m_walkCount;          ///< トータル歩数
  u8      m_intervalCount;      ///< エンカウント率上昇インターバルカウント
  u8      m_move_f:1;           ///< 最終エンカウントから1歩歩いたかどうか？
  u8      m_safety_f:1;         ///< セーフティ処理が有効かどうか？
  u16     m_nowProb;            ///< 現在のエンカウント率
  Attr    m_oldAttr;            ///< 最後に歩いた場所のアトリビュート
  u16     m_fishingContinueCnt; ///< 釣りの連続数
  PokeSet m_pokeSet;            ///< エンカウントポケモンセット
  bool    m_oldIsEncount;       ///< 前回のエンカウントができるかどうかのフラグ


  // 戦闘導入演出関連
  // スクリプト or EventBattleCallで取得され初期化、破棄される
  static const u32    RO_MODULE_NUM = 2;                  ///< 戦闘導入演出Roモジュール数
  static const u32    DEMO_MODULE_INDEX = 1;              ///< デモ用のDLLのINDEX(バトル内でこれだけ破棄するために必要
  EncountEffect       m_encountEffect;                    ///< 戦闘導入演出
  gfl2::ro::Module*   m_pRoModules[ RO_MODULE_NUM ];      ///< Roモジュール
  GameSys::DllReadThread* m_pDllRead;                     ///< DLL読み込みスレッド

private:

  //-----------------------------------------------------------------------------
  /**
   *  @brief  歩数&エンカウント率上昇計算
   */
  //-----------------------------------------------------------------------------
  void AddWalkCountAndProb( const ENCPROB_PATTERN* data );

  void SafeDeleteDllRead();
};

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

#endif  // FIELD_ENCOUNT_WORK_H_INCLUDED

