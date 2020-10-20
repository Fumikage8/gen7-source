#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_NODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_NODE_H_INCLUDED

//#include <iostream>
#include <memory>
#include <stdio.h>

#include <gfx/include/gfl2_GLMemory.h>
#include "gfl2_NodeTrait.h"

namespace gfl2 { namespace renderingengine { namespace scenegraph {

struct NullType
{
	struct Type {};
};

class NodeName
{
public:
  static const u32					NAME_LENGTH = 64;

  virtual ~NodeName();

  void SetNodeName( const char *name );
  const c8* GetNodeName( void ) const;

protected:
  NodeName();

	c8												m_NameBuff[NAME_LENGTH];
};

//===============================================================
//! ノードを扱うクラス
//===============================================================
class Node : public gfx::GLMemory, public NodeTrait<Node,NullType>
{
public:
	friend class SceneGraphManager;

	//! @brief Tが基底クラスかどうか判定する
	template<class T>
	bool IsKindOf() const
	{
		//thisの持っている継承情報の中に、Ｔの型が存在していればtrueを返す
		const util::FnvHash		targetType = T::GetInheritanceStatic()->m_ThistTypeValue;
		const Inheritance			*pInheritance = this->GetInheritance();

		while( pInheritance )
		{
			if( pInheritance->m_ThistTypeValue == targetType )
			{
				return true;
			}
			pInheritance = pInheritance->m_pBaseType;
		}

		return false;
	}

#ifdef WIN32
	template<>
	bool IsKindOf<Node>() const
	{
		return true;//判定するまでもなく必ずtrue
	}
#endif

	//! @brief Tかどうか判定する
	template<class T> bool IsTypeOf() const
	{
		const util::FnvHash		targetType = T::GetInheritanceStatic()->m_ThistTypeValue;
		const Inheritance			*pInheritance = this->GetInheritance();

		return pInheritance->m_ThistTypeValue == targetType;
	}
	
	//! @brief thisがＴ型、もしくはＴ型の派生型のインスタンスであればキャストしてポインタを返す。要するにdynamic_cast<T>(this)と同じ。
	template<class T>
	T* SafeCast()
	{
		//return ( IsKindOf<T>() ) ? ((T*)this) : NULL;
		return ( IsKindOf<T>() ) ? (static_cast<T*>(this)) : NULL;
	}

	//! @brief thisがＴ型、もしくはＴ型の派生型のインスタンスであればキャストしてポインタを返す。要するにdynamic_cast<T>(this)と同じ。(const版)
	template<class T>
	const T* SafeCast() const
	{
		//return ( IsKindOf<T>() ) ? ((const T*)this) : NULL;
		return ( IsKindOf<T>() ) ? (static_cast<const T*>(this)) : NULL;
	}

public:
	virtual ~Node();

	virtual bool IsTransformType() const;
	//static Node* CreateNode( void );
	static Node* CreateNode( gfx::IGLAllocator* pAllocator );
	
protected:
	Node();
};


}}}

#endif
