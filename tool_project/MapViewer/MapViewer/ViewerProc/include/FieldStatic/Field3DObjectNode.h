//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		Field3DObjectNode.h
 *	@brief  フィールド3Dオブジェクト基底ノード
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_3DOBJECT_NODE_H__ )
#define __FIELD_3DOBJECT_NODE_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <renderingengine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include "ViewerProc/include/FieldStatic/FieldNode.h"

GFL_NAMESPACE_BEGIN(Field);

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
 *  @class フィールド基底ノード
 */
class IField3DObjectNode : public gfl2::renderingengine::scenegraph::NodeTrait<IField3DObjectNode,FieldNode>
{
public:

  /**
   * @class 特定の操作を抽象化
   *
   * IField3DObjectNodeを継承した先の処理で、
   * 外部公開する処理に関しては、
   * IHandleを継承したクラスで作成する。
   *
   * ・アニメーション制御
   * ・特定の情報の取得
   *
   * サポート外の型が引数にわたってきていないかのチェックは必ず行なう。
   */
  class IHandle
  {
  public:
    IHandle(){}
    virtual ~IHandle(){}

    /**
     *  @brief 操作可能なオブジェクトかチェック
     */
    virtual bool IsSupport( const IField3DObjectNode & node ) const = 0;

    /**
     * @brief Handle内部でSafeCastしてアニメーションの終了等を待つ。
     */
    virtual bool IsEnd( const IField3DObjectNode * node ) const = 0;
  };

  class ISetHandle : public IHandle
  {
  public:
    ISetHandle(){}
    virtual ~ISetHandle(){}
    /**
     * @brief Handle内部でSafeCastしてアニメーションの制御等を行なう。
     */
    virtual void Handle( IField3DObjectNode * node ) = 0;
  };
  class IGetHandle : public IHandle
  {
  public:
    IGetHandle(){}
    virtual ~IGetHandle(){}

    /**
     * @brief Handle内部でSafeCastしてアニメーションの制御等を行なう。
     */
    virtual void Handle( const IField3DObjectNode * node ) = 0;
  };


public:
  virtual ~IField3DObjectNode()
  {
  }

	static gfl2::util::FnvHash GetTypeHash()
	{
		return gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::FieldNode::Field3DObjectNode") ) );
	}

  /**
   * @brief 表示ON・OFF
   */
  virtual void SetVisible( bool isVisible ) = 0;
  virtual bool GetVisible( void ) const = 0;

  /**
   * @brief SRTコントロール
   */
  virtual void SetLocalSRT( const gfl2::math::SRT & srt ) = 0;
  virtual const gfl2::math::SRT & GetLocalSRT() const = 0;

  /**
   * @brief モデルインスタンスを取得
   */
  virtual u32 GetModelInstanceNum() const = 0;
  virtual const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetModelInstance( int index = 0 ) const = 0;

  //! 特定の操作を行なう。
  void Set( ISetHandle * handle )
  {
    if( handle->IsSupport( *this ) )
    {
      handle->Handle( this );
    }
    else
    {
      GFL_ASSERT( 0 );
    }

  }
  //! 特定の値の取得を行なう。
  void Get( IGetHandle * handle ) const
  {
    if( handle->IsSupport( *this ) )
    {
      handle->Handle( this );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //! 操作の完了を待つ
  bool IsEnd( const IHandle & handle ) const
  {
    if( handle.IsSupport( *this ) )
    {
      return handle.IsEnd( this );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
    return true;
  }


  //! モデルインスタンスの遅延解放は自由に実装
  /**! 削除リクエストを受けたモデルインスタンスを解放
   * -実行タイミング　フィールドプロセスの先頭で実行します。
   * -制限　　　　　　PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
   */
  virtual void ExecuteTerminateRequest(void) = 0;
  //! 安全に破棄できる常態かチェックする。
  virtual bool IsCanTerminate(void) = 0;
  /*　例）
   void TerminateRequest()
   {
      m_TerminateModelInstance = true;
      m_pModelInstance->SetVisible(false); // 表示OFF
   }

   virtual void ExecuteTerminateRequest()
   {
     if( m_TerminateModelInstance )
     {
       GFL_DELETE m_pModelInstance; // インスタンスを破棄
       m_TerminateModelInstance = false;
     }
   }
   */


protected:
  IField3DObjectNode()
  {
  }
};

GFL_NAMESPACE_END(Field);


#endif	// __FIELD_3DOBJECT_NODE_H__

