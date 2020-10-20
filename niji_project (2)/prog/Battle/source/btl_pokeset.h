#if !defined( __BTL_POKESET_H__ )
#define __BTL_POKESET_H__
//=============================================================================================
/**
 * @file    btl_PokeSet.h
 * @brief   ポケモンXY バトルシステム 特定目的のポケモンパラメータセット管理
 * @author  taya
 *
 * @date  2011.01.07  作成
 */
//=============================================================================================
#pragma once

#include "./btl_common.h"
#include "./btl_pospoke.h"
#include "./btl_PokeParam.h"

GFL_NAMESPACE_BEGIN(btl)

class ServerFlow;
class BTL_POKEPARAM;

class PokeSet {
  GFL_FORBID_COPY_AND_ASSIGN( PokeSet );

public:
  PokeSet( void ){ };
  ~PokeSet(){ };

  enum DamageType {

    DMGTYPE_NONE = 0,   ///< ダメージ記録なし
    DMGTYPE_REAL,       ///< ダメージ記録：実体ヒット
    DMGTYPE_MIGAWARI,   ///< ダメージ記録：みがわりヒット

  };

  /**
   *  初期化
   */
  void Clear( void );

  /**
   *  ポケモン１体登録（ダメージ記録を伴う）
   */
  void AddWithDamage( BTL_POKEPARAM* bpp, u16 damage, bool fMigawariHit );

  /**
   *  ポケモン１体登録
   */
  void Add( BTL_POKEPARAM* bpp );

  /**
   *  ポケモン１体除外
   */
  void Remove( BTL_POKEPARAM* bpp );

  /**
   *  ポケモンデータ取得
   */
  BTL_POKEPARAM* Get( u32 idx ) const;

  /**
   *  生きてるポケモンだけ
   */
  u32 CopyAlive( PokeSet* dst ) const;

  /**
   *  味方だけコピー
   */
  u32 CopyFriends( const MainModule& mainModule, const BTL_POKEPARAM* bpp, PokeSet* dst ) const;

  /**
   *  相手だけコピー
   */
  u32 CopyEnemys( const MainModule& mainModule, const BTL_POKEPARAM* bpp, PokeSet* dst ) const;

  /**
   *  死んでる or 場に居ないのを除外
   */
  void RemoveDisablePoke( const PosPoke& pospoke );

  /**
   *  素早さ順ソート
   */
  void SortByAgility( ServerFlow* flowWk );

  /**
   *  順番アクセス開始
   */
  void SeekStart( void );

  /**
   *  順番アクセス（NULLが返ったら修了）
   */
   BTL_POKEPARAM*  SeekNext( void );

  /**
   *  ダメージ記録取得（実体・みがわりとも）
   */
   bool GetDamage( const BTL_POKEPARAM* bpp, u32* damage ) const;

  /**
   *  ダメージ記録取得（実体のみ）
   */
   bool GetDamageReal( const BTL_POKEPARAM* bpp, u32* damage ) const;

  /**
   *  現在登録されているポケモン総数を取得
   */
   u32 GetCount( void ) const;

  /**
   *  初期化後、記録されたポケモン総数を取得
   */
   u32 GetCountMax( void ) const;

  /**
   *
   */
   void SetDefaultTargetCount( u8 posCnt );

  /**
   *  本来は１体以上いたハズのターゲットが現在は0になってしまったケースをチェック
   */
   bool IsRemovedAll( void ) const;

   /**
    * 受けたダメージタイプを取得
    */
   DamageType  GetDamageType( const BTL_POKEPARAM* bpp ) const;

private:
  BTL_POKEPARAM*  m_bpp[ BTL_POS_NUM ];
  u16             m_damage[ BTL_POS_NUM ];
  u16             m_migawariDamage[ BTL_POS_NUM ];
  u8              m_damageType[ BTL_POS_NUM ];
  u16             m_sortWork[ BTL_POS_NUM ];
  u8              m_count;
  u8              m_countMax;
  u8              m_getIdx;
  u8              m_targetPosCount;


};  // class PokeSet

GFL_NAMESPACE_END(btl)

#endif // __BTL_POKESET_H__
