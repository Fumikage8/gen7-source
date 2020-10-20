//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectEmitterSet.h
 *	@brief  エミッターセット　エミッターコントローラー
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __GFL2_EFFECT_EMITTERSET_H__ )
#define __GFL2_EFFECT_EMITTERSET_H__
#pragma once

#include <math/include/gfl2_math.h>

#include <nw/eft/eft_Emitter.h>
#include <nw/eft/eft_EmitterSet.h>
#include <nw/eft/eft_Particle.h>

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Effect )


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @brief エミッターコントローラー再定義
 *
 *  詳細な操作はNW_Cafe NW_CTRの物を使用
 *  基本的なインターフェースはCTR/Cafe共通
 */
typedef nw::eft::EmitterController EmitterController;

/**
 *  @brief エミッターインスタンス再定義
 *
 *  詳細な操作はNW_Cafe NW_CTRの物を使用
 *  基本的なインターフェースはCTR/Cafe共通
 */
typedef nw::eft::EmitterInstance EmitterInstance;

/**
 *  @brief パーティクルインスタンス再定義
 *
 *  詳細な操作はNW_Cafe NW_CTRの物を使用
 *  基本的なインターフェースはCTR/Cafe共通
 */
typedef nw::eft::PtclInstance PtclInstance;



/**
 *  @brief エミッターセット再定義
 *
 *  詳細な操作はNW_Cafe NW_CTRの物を使用
 */
class EmitterSet : public nw::eft::EmitterSet
{
  GFL_FORBID_COPY_AND_ASSIGN(EmitterSet); //コピーコンストラクタ＋代入禁止

public:
  /**
   *  @brief コンストラクタ
   */
  EmitterSet() : nw::eft::EmitterSet(){};


  /**
   *  
   *  gfl2のMath版での座標系操作をstaticで用意
   *
   */
  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。行列からスケールを取り除く計算が入りますので、なるべく setMtxFast を使用するようにして下さい。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    mtx         エミッタ行列。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFMtx( nw::eft::EmitterSet* pEmitterSet, const math::Matrix34 &mtx )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    pEmitterSet->SetMtx( nw_mtx );
  }

  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。引数にスケール込みの行列を渡した場合表示が壊れます。どうしてもスケール込の行列を扱うときは setMtx をコールして下さい。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    mtx         エミッタ行列。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFMtxFast( nw::eft::EmitterSet* pEmitterSet, const math::Matrix34 &mtx )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    pEmitterSet->SetMtxFast( nw_mtx );
  }

  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。この関数はスケール抜きの行列と、それとは別にスケールを指定できます。mtxにスケール込みの行列を渡した場合表示が壊れます。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    mtx         エミッタ行列。
  //! @param[in]    scale       スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFMtxFast( nw::eft::EmitterSet* pEmitterSet, const math::Matrix34 &mtx , f32 scale )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    pEmitterSet->SetMtxFast( nw_mtx, scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。この関数はスケール抜きの行列と、それとは別にスケールを指定できます。mtxにスケール込みの行列を渡した場合表示が壊れます。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    mtx         エミッタ行列。
  //! @param[in]    scale       スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFMtxFast( nw::eft::EmitterSet* pEmitterSet, const math::Matrix34 &mtx , const nw::math::VEC3 &scale )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    pEmitterSet->SetMtxFast( nw_mtx, scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置を指定してエミッタ行列設定します。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    pos         座標。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFPos( nw::eft::EmitterSet* pEmitterSet, const math::Vector3 &pos )
  {
    pEmitterSet->SetPos( nw::math::VEC3( pos.GetX(), pos.GetY(), pos.GetZ() ) );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置とスケールを指定してエミッタ行列設定します。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    pos         座標。
  //! @param[in]    scale       スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFPos( nw::eft::EmitterSet* pEmitterSet, const math::Vector3 &pos , f32 scale )
  {
    pEmitterSet->SetPos( nw::math::VEC3( pos.GetX(), pos.GetY(), pos.GetZ() ), scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置とスケールを指定してエミッタ行列設定します。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[in]    pos         座標。
  //! @param[in]    scale       スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  static void SetGFPos( nw::eft::EmitterSet* pEmitterSet, const math::Vector3 &pos , const nw::math::VEC3 &scale )
  {
    pEmitterSet->SetPos( nw::math::VEC3( pos.GetX(), pos.GetY(), pos.GetZ() ), scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置を指定してエミッタ行列を取得します。
  //!
  //! @param[in]    pEmitterSet エミッタセットのポインタ。
  //! @param[out]   pos         位置座標を格納する変数.
  //---------------------------------------------------------------------------
  static void GetGFPos( nw::eft::EmitterSet* pEmitterSet, math::Vector3 &pos )
  {
    nw::math::VEC3 nw_pos;
    pEmitterSet->GetPos(nw_pos);
    pos.SetX(nw_pos.x);
    pos.SetY(nw_pos.y);
    pos.SetZ(nw_pos.z);
  }

  /**
   *  
   *  座標系操作のみgfl2のMath版を用意
   *
   */
  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。行列からスケールを取り除く計算が入りますので、なるべく setMtxFast を使用するようにして下さい。
  //!
  //! @param[in]    mtx     エミッタ行列。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFMtx( const math::Matrix34 &mtx )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    this->SetMtx( nw_mtx );
  }

  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。引数にスケール込みの行列を渡した場合表示が壊れます。どうしてもスケール込の行列を扱うときは setMtx をコールして下さい。
  //!
  //! @param[in]    mtx     エミッタ行列。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFMtxFast( const math::Matrix34 &mtx )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    this->SetMtxFast( nw_mtx );
  }

  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。この関数はスケール抜きの行列と、それとは別にスケールを指定できます。mtxにスケール込みの行列を渡した場合表示が壊れます。
  //!
  //! @param[in]    mtx     エミッタ行列。
  //! @param[in]    scale   スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFMtxFast( const math::Matrix34 &mtx , f32 scale )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    this->SetMtxFast( nw_mtx, scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        エミッタ行列を設定します。この関数はスケール抜きの行列と、それとは別にスケールを指定できます。mtxにスケール込みの行列を渡した場合表示が壊れます。
  //!
  //! @param[in]    mtx     エミッタ行列。
  //! @param[in]    scale   スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFMtxFast( const math::Matrix34 &mtx , const nw::math::VEC3 &scale )
  {
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
    this->SetMtxFast( nw_mtx, scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置を指定してエミッタ行列設定します。
  //!
  //! @param[in]    pos     座標。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFPos( const math::Vector3 &pos )
  {
    this->SetPos( nw::math::VEC3( pos.GetX(), pos.GetY(), pos.GetZ() ) );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置とスケールを指定してエミッタ行列設定します。
  //!
  //! @param[in]    pos     座標。
  //! @param[in]    scale   スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFPos( const math::Vector3 &pos , f32 scale )
  {
    this->SetPos( nw::math::VEC3( pos.GetX(), pos.GetY(), pos.GetZ() ), scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置とスケールを指定してエミッタ行列設定します。
  //!
  //! @param[in]    pos     座標。
  //! @param[in]    scale   スケール。
  //!
  //! @return       なし。
  //---------------------------------------------------------------------------
  void SetGFPos( const math::Vector3 &pos , const nw::math::VEC3 &scale )
  {
    this->SetPos( nw::math::VEC3( pos.GetX(), pos.GetY(), pos.GetZ() ), scale );
  }

  //---------------------------------------------------------------------------
  //! @brief        位置を指定してエミッタ行列を取得します。
  //!
  //! @param[out]   pos     位置座標を格納する変数.
  //---------------------------------------------------------------------------
  void GetGFPos( math::Vector3 &pos ) const
  {
    nw::math::VEC3 nw_pos;
    this->GetPos(nw_pos);
    pos.SetX(nw_pos.x);
    pos.SetY(nw_pos.y);
    pos.SetZ(nw_pos.z);
  }


};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( gfl2 )

#endif	// __GFL2_EFFECT_EMITTERSET_H__

