#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_TRAVERSER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_TRAVERSER_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

//-----------------------------------------------------------------------------
/**
 * @class	Traverser
 * @brief	Dagを手繰って処理を施すクラス
 */
class Traverser
{
public:

	static const u32						DEFAULT_DEPTH = 64; // メガレックウザは38階層、十分な余裕を持たせる

	//-----------------------------------------------------------------------------
	/**
	 * @class	Visitor
	 * @brief	トラバース時にノードを受け取り処理するビジター
	 */
	template<class TatgetNodeType, class Implement>
	class Visitor : public Implement
	{
	public:
		Visitor():Implement(){}
		virtual ~Visitor() {}

		void Visit( TatgetNodeType* node )
		{
			GFL_ASSERT(node);
			Implement::Visit( node );
		}
	};

	template<class TatgetNodeType, class Implement>
	inline static void Traverse( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, u32 depth = DEFAULT_DEPTH );

	template<class TatgetNodeType, class Implement>
	inline static void TraverseFast( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, u32 depth = DEFAULT_DEPTH );

	template<class TatgetNodeType, class Implement>
	inline static void TraverseSibling( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, s32 depth );

	template<class TatgetNodeType, class Implement>
	inline static void TraverseSiblingFast( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, s32 depth );
};

template<class TatgetNodeType, class Implement>
inline void Traverser::Traverse( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, u32 depth )
{
	GFL_ASSERT(pTarget);
	GFL_ASSERT(pVisitor);
	TatgetNodeType*		pTType = pTarget->SafeCast<TatgetNodeType>();

	if ( pTType )
		pVisitor->Visit( pTType );

	DagNode		*pChild = pTarget->GetChild();

	if ( depth == 0 )			return;
	--depth;

	if( pChild )
		TraverseSibling<TatgetNodeType,Implement>( pChild, pVisitor, depth );
}

template<class TatgetNodeType, class Implement>
inline void Traverser::TraverseSibling( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, s32 depth )
{
	TatgetNodeType*		pTType = pTarget->SafeCast<TatgetNodeType>();

	{//ターゲットとその兄弟のトラバース
		if ( pTType )
			pVisitor->Visit( pTType );

		DagNode		*pSibling = pTarget->GetSibling();

		while( pSibling )
		{
			pTType = pSibling->SafeCast<TatgetNodeType>();

			if ( pTType )
				pVisitor->Visit( pTType );

			pSibling = pSibling->GetSibling();
		}
	}

	if ( depth == 0 )			return;
	--depth;

	{//ターゲットとその兄弟の子をトラバース
		DagNode		*pChild = NULL;

		pChild = pTarget->GetChild();

		if( pChild )
			TraverseSibling<TatgetNodeType,Implement>( pChild, pVisitor, depth );

		DagNode		*pSibling = pTarget->GetSibling();

		while( pSibling )
		{
			pChild = pSibling->GetChild();

			if( pChild )
				TraverseSibling<TatgetNodeType,Implement>( pChild, pVisitor, depth );

			pSibling = pSibling->GetSibling();
		}

	}
}

template<class TatgetNodeType, class Implement>
inline void Traverser::TraverseFast( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, u32 depth )
{
	GFL_ASSERT(pTarget);
	GFL_ASSERT(pVisitor);
	TatgetNodeType*		pTType = (TatgetNodeType*)pTarget;

	if ( pTType )
		pVisitor->Visit( pTType );

	DagNode		*pChild = pTarget->GetChild();

	if ( depth == 0 )			return;
	--depth;

	if( pChild )
		TraverseSiblingFast<TatgetNodeType,Implement>( pChild, pVisitor, depth );
}

template<class TatgetNodeType, class Implement>
inline void Traverser::TraverseSiblingFast( DagNode* pTarget, Visitor<TatgetNodeType, Implement> *pVisitor, s32 depth )
{
	TatgetNodeType*		pTType = (TatgetNodeType*)pTarget;

	{//ターゲットとその兄弟のトラバース
		if ( pTType )
			pVisitor->Visit( pTType );

		DagNode		*pSibling = pTarget->GetSibling();

		while( pSibling )
		{
			pTType = (TatgetNodeType*)pSibling;

			if ( pTType )
				pVisitor->Visit( pTType );

			pSibling = pSibling->GetSibling();
		}
	}

	if ( depth == 0 )			return;
	--depth;

	{//ターゲットとその兄弟の子をトラバース
		DagNode		*pChild = NULL;

		pChild = pTarget->GetChild();

		if( pChild )
			TraverseSiblingFast<TatgetNodeType,Implement>( pChild, pVisitor, depth );

		DagNode		*pSibling = pTarget->GetSibling();

		while( pSibling )
		{
			pChild = pSibling->GetChild();

			if( pChild )
				TraverseSiblingFast<TatgetNodeType,Implement>( pChild, pVisitor, depth );

			pSibling = pSibling->GetSibling();
		}

	}
}

}}}

#endif

