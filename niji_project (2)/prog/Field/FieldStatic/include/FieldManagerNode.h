//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldManagerNode.h
 *	@brief  フィールドマネージャー基底ノード 
 *	@author	takahashi tomoya
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_MANAGER_NODE_H__ )
#define __FIELD_MANAGER_NODE_H__
#pragma once

#include "Field/FieldStatic/include/FieldNode.h"
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
 *  @class マネージャーノード
 */
class IFieldManagerNode : public FieldNode
{
public:
  /**
   * @class データアクセサー
   * 後々のViewer作成やデバッグ機能実装のために、基本的なマネージャーおよびマネージャーが生成するクラスは依存度を低く保つ。
   *
   * 他のモジュールに依存しそうな場所は、データアクセサーにまとめる。
   */
  class IAccesser
  {
    IAccesser(){}
    virtual ~IAccesser(){}

    /*
      virtual gfl2::heap::HeapBase * GetUseHeapBase() const = 0;
      virtual const XXData & GetXXData() const = 0;
     */
    
  };

public:
  virtual ~ IFieldManagerNode()
  {
  }

  /**
   * @brief ファイル読み込み管理
   */
  virtual void StartFileRead( void ) = 0;
  virtual bool WaitFileRead( void ) = 0;

  /**
   * @brief 使用可能状態にセットアップ
   */
  virtual bool Setup( void ) = 0;


  /**
   * @brief 破棄
   */
  virtual bool Finalize( void ) = 0;

  /**
   * @brief 破棄の必要があるか。
   */
  virtual bool IsExists( void ) const = 0;

protected:
  IFieldManagerNode()
  {
  }
};

GFL_NAMESPACE_END(Field);



#endif	// __FIELD_MANAGER_NODE_H__





