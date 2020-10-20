/**
 *  GAME FREAK inc.
 *
 *  @file   FieldGimmick.h
 *  @brief  フィールドのシーケンスに割り込み独自の処理を行うクラス
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

#if !defined(__FIELD_GIMMICK_H__)
#define __FIELD_GIMMICK_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <ro/include/gfl2_RoManager.h>
#include <gfx/include/gfl2_types.h>   // for displayNo

GFL_NAMESPACE_BEGIN( Field );
// 前方宣言
class Fieldmap;
struct EventCheckRequest;

class FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmick );

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldGimmick( Fieldmap *pMap )
    : m_pFieldmap( pMap )
    , m_pModule( NULL )
  {
  }

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldGimmick( void )
  {
    Delete();
  }

  /**
   *  @brief  生成
   */
  virtual void Create( void )
  {
  }
  virtual void UpdateCreate( void )
  {
  }
  virtual bool IsCreated( void )
  {
    return true;
  }
  /**
   *  @brief  破棄
   */
  virtual void Delete( void )
  {
  }
  /**
   *  @biref  破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void )
  {
    return true;
  }

  /**
   *  @brief  Ready中1度だけ呼び出し
   */
  virtual void Ready( void )
  {
  }

  /**
   *  @brief  他のReadyが終わった後に1度だけ呼び出し
   */
  virtual void ReadyAfter( void )
  {
  }
  
  /**
   *  @brief  更新
   *  Fieldmap::Updateにて呼出
   *  キャラクタ前更新よりも手前で呼び出される
   */
  virtual void UpdateFirst( void )
  {
  }
  /**
   *  @brief  更新
   *  Fieldmap::Updateにて呼出
   *  UpdateNormalが終了してから呼び出される
   */
  virtual void UpdateLast( void )
  {
  }
  /**
   *  @brief  更新
   *  Fieldmap::UpdateNormalにて呼び出される
   *  トラバース直前に呼び出される
   */
  virtual void TraverseBefore( void )
  {
  }
  /**
   *  @brief  更新
   *  Fieldmap::UpdateNormalにて呼び出される
   */
  virtual void CalculateAfter( void )
  {
  }

  /**
   *  @brief  特殊ステップチェック
   */
  virtual bool MoveRequestCheck( const EventCheckRequest *cpReq )
  {
    return false;
  }

  /**
   *  @brief  イベント起動チェック
   */
  virtual bool EventRequestCheck( const EventCheckRequest *cpReq )
  {
    return false;
  }

  /**
   *  @brief  モジュールの設定
   */
  void SetModule( gfl2::ro::Module *pModule ){ m_pModule = pModule; }

  /**
   *  @brief  モジュールの取得
   */
  gfl2::ro::Module *GetModule( void ){ return m_pModule; }

  //@momiji追加分
  /**
  *  @brief  現在バトルフェス専用関数　フレームバッファの画像データをモデルのテクスチャにコピーする
  */
  virtual void TransferTexture(gfl2::gfx::CtrDisplayNo displayNo){}

protected:
  Fieldmap  *m_pFieldmap;
  gfl2::ro::Module  *m_pModule;

}; // class FieldGimmick
GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_GIMMICK_H__)
