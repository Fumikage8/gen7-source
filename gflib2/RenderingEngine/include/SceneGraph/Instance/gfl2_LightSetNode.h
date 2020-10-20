#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTSETNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTSETNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <math/include/gfl2_MathAABB.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )														\
	class CLASSNAME {                                               \
	public:																                          \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )									            \
		};																                            \
	public:																                          \
		CLASSNAME(){}												                          \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}					        \
		operator Enum () const { return value_; }					            \
	private:															                          \
		Enum value_;													                        \
	};

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class LightSetNode : public NodeTrait<LightSetNode, TransformNode>
{
public:

	friend class InstanceCreator;

	static const u32																LightMax = 32;

	virtual ~LightSetNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::LightSetNode") ) );
		return hash;
	}

	void AddLight( LightNode *pNode );
	void RemoveLight( LightNode *pNode );
	void RemoveAllLight();

	inline u32 GetLightNum() const { return m_LightContainer.size(); }
	LightNode* GetLightNode( s32 no );
	const LightNode* GetLightNode( s32 no ) const;

	void SetSetNo( u32 no ){ m_SetNo = no; }
	u32 GetSetNo() const { return m_SetNo; }

protected:
	LightSetNode();

private:
	u32																										m_SetNo;
	util::FixedSizeContainer<LightNode*,LightMax>					m_LightContainer;
};

}}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif

