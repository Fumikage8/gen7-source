#pragma once
//=================================================================================================
/**
 @file   movecode_attach.h
 @brief  動作コード　処理管理　基底クラス
 @date   2014.03.17
 @author yy-ikeuchi
 */
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Quaternion.h>
#include "movemodel_define.h"
#include "movecode.h"
#include <xml/include/rapidxml.hpp> // no need to link
#include <heap/include/gfl2_Heap.h>

namespace fld{ namespace mmodel {

//=================================================================================================
// パーツアタッチ(基礎クラス)
//=================================================================================================
class MoveCodeAttach : public MoveCodeBase
{
private:

  // 使用禁止
  MoveCodeAttach( void ){}

public:

  // 動作コードのワーク用構造体
  struct AttachData
  {
    s32                     parentJoint;
    gfl2::math::Vector3     offsetTrans;
    gfl2::math::Quaternion  offsetQua;
  };

  // 同期モーション情報
  struct SyncAnimationData
  {
    s32 parentId;
    s32 childId;
  };

  // 1パーツにおけるアタッチリソース
  struct AttachResource
  {
    s32                 rscId;
    s32                 parentJoint;
    gfl2::math::Vector3 offsetTrans;
    gfl2::math::Vector3 offsetRotYXZ;
    s32                 defaultAnimation;
    SyncAnimationData   syncAnimationData;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  MoveCodeAttach( gfl2::heap::HeapBase * p_heap, c16* pFilePath ){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~MoveCodeAttach(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual MOVE_CODE GetMVCode(void) const = 0;
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess( MoveCodeWork* p_wk ) const{}

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン　
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess( MoveCodeWork* p_wk ) const{}

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  破棄
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void DeleteProcess( MoveCodeWork* p_wk ) const{}

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void ReturnProcess( MoveCodeWork* p_wk ) const{}

protected:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  リソースからアタッチ情報を更新
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void updateAttachDataFromResource( MoveCodeWork* p_wk, AttachResource* pAttachResource, s32 attachResourceCnt ) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  リソースと親のモーションから自身のアニメーションIDを取得
  *
  *  @param  p_wk  動作コードワーク
  *  @return アニメーションID
  */
  //-----------------------------------------------------------------------------
  virtual s32 getAnimationIdFromResource( MoveCodeWork* p_wk, AttachResource* pAttachResource, s32 attachResourceCnt) const;

};  // end of class MoveCodeAttach;
 
//=================================================================================================
// パーツアタッチ(プレイヤー)
//=================================================================================================
class MoveCodeAttachPlayer : public MoveCodeAttach
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  MoveCodeAttachPlayer( gfl2::heap::HeapBase * p_heap, c16* pFilePath );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~MoveCodeAttachPlayer(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual MOVE_CODE GetMVCode(void) const { return MOVE_CODE_ATTACH_PLAYER; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess( MoveCodeWork* p_wk ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン　
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess( MoveCodeWork* p_wk ) const;

};  // end of class MoveCodeAttach;


}; //end of namespace mmodel
}; //end of namespace fld
