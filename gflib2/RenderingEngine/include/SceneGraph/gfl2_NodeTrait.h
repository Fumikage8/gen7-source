#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_NODETRAIT_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_NODETRAIT_H_INCLUDED

#include <util/include/gfl2_FnvHash.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

class Node; // 基底ノード

//! @brief ノードの継承階層
struct Inheritance
{
	static Inheritance Generate( const Inheritance* basetype, util::FnvHash ThistTypeValue );

	util::FnvHash					m_ThistTypeValue;
	const Inheritance*		m_pBaseType;
};

template<class T,class B>
class NodeInheritance;

template<class B>
class NodeInheritance<Node,B> : public B
{
public:
	typedef Node ThisType;
	//typedef B BaseType;//Nodeは基底なのでBaseTypeを定義しない

protected:
	inline NodeInheritance() {}

	static const Inheritance* GetInheritanceStatic()
	{
		return NULL;
	}

	friend class Node;
};

template<class T,class B>
class NodeInheritance : public B
{
public:
	typedef T ThisType;
	typedef B BaseType;

protected:
	inline NodeInheritance() {}

	static const Inheritance* GetInheritanceStatic()
	{
		return &s_Inheritance;
	}

	friend class Node;

private:
	static const Inheritance s_Inheritance;
};

template<class T,class B>
const Inheritance NodeInheritance<T,B>::s_Inheritance = Inheritance::Generate( B::GetInheritanceStatic(), T::GetTypeHash() );

//! @brief ノードの特性を定義するためのクラス
template<class T, class B>
class NodeTrait : public NodeInheritance<T,B>
{
protected:
	
	inline NodeTrait() {}

	NodeTrait( NodeTrait& );
	NodeTrait& operator = ( NodeTrait& );

protected:
	typedef NodeTrait TraitType;

public:
	virtual ~NodeTrait() {}

	//! @brief 継承情報の取得
	virtual const Inheritance* GetInheritance() const
	{
		return NodeInheritance<T,B>::GetInheritanceStatic();
	}

protected:

	friend struct Inheritance;
};

}}}


#endif
