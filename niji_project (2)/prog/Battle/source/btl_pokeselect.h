//=============================================================================================
/**
 * @file  btl_pokeselect.h
 * @brief ポケモンXY バトルシステム ポケモン選択画面用パラメータ
 * @author  taya
 *
 * @date  2011.03.01  WBより移植
 */
//=============================================================================================
#ifndef __BTL_POKESELECT_H__
#define __BTL_POKESELECT_H__

#include "btl_common.h"
#include "btl_party.h"

GFL_NAMESPACE_BEGIN(btl)


//------------------------------
/**
 *  選択できない理由
 */
//------------------------------
typedef enum {
  BTL_POKESEL_CANT_NONE=0,    ///< 選択できます

  BTL_POKESEL_CANT_DEAD,      ///< 死んでるから
  BTL_POKESEL_CANT_FIGHTING,  ///< 戦ってるから
  BTL_POKESEL_CANT_SELECTED,  ///< 同じターンでもう選ばれたから

}PokeselReason;


//------------------------------------------------------
/**
 *  ポケモン選択画面用パラメータクラス
 */
//------------------------------------------------------
//_ BTL_POKESELECT_PARAM
class PokeSelParam  {
  GFL_FORBID_COPY_AND_ASSIGN( PokeSelParam );

  friend class PokeSelResult;

public:
  PokeSelParam( void ) {};
  ~PokeSelParam() {};

  void          Init( const BTL_PARTY* party, u8 numSelect );
  void          SetProhibitFighting( u8 numCover );
  void          SetProhibit( PokeselReason reason, u8 idx );
  void          DisablePutPosSequence( void );

  u8            GetNumSelect( void ) const;
  bool          IsDisabledPutPosSequence( void ) const; // true が帰ってきたら、繰り出す位置選択をしなくて良い


private:
  const BTL_PARTY*  m_party;        ///< パーティデータ
  u8                m_numSelect;    ///< 選択するポケモン数
  u8                m_bDisabledPutPosSequence;
  u8                m_prohibit[ BTL_PARTY_MEMBER_MAX ];

};  // class PokeSelParam;


//------------------------------------------------------
/**
 *  結果格納用クラス
 */
//------------------------------------------------------
//_ BTL_POKESELECT_RESULT
class  PokeSelResult {
  GFL_FORBID_COPY_AND_ASSIGN( PokeSelResult );

  friend class BTL_CLIENT;

public:
  PokeSelResult( void ) {};
  ~PokeSelResult() {};

  void     Init( const PokeSelParam* param );
  void     Push( u8 outPokeIdx, u8 selPokeIdx );
  void     Pop( void );
  bool     IsCancel( void ) const;
  void     SetCancel(bool flg);
  bool     IsDone( void ) const;
  u8       GetCount( void ) const;
  u8       GetLast( void ) const;
  u8       Get( u8 idx ) const;
  u8       GetSelectMax() const { return m_max; }

  void     SetItemUse( u8 pokeIdx, u16 itemNo, u8 wazaIdx=0 );
  bool     IsItemUse( u8* pokeIdx, u16* itemNo, u8* wazaidx ) const;

private:
  u8      m_selIdx[ BTL_POSIDX_MAX ];     ///< 選択したポケモンの並び順（現在の並び順で先頭を0と数える）
  u8      m_outPokeIdx[ BTL_POSIDX_MAX ]; ///< 入れ替わりに出て行くポケモンの並び順
  u16     m_useItem[ BTL_POSIDX_MAX ];    ///< 使用アイテムナンバー
  u8      m_wazaIdx[ BTL_POSIDX_MAX ];    ///< アイテム使用時の対象ワザIndex
  u8      m_cnt;                          ///< 選択した数
  u8      m_max;                          ///< 選択する数
  u8      m_fCancel;                      ///< キャンセルフラグ

};  // class PokeSelResult



GFL_NAMESPACE_END(btl)
#endif  // #ifndef __BTL_POKESELECT_H__
