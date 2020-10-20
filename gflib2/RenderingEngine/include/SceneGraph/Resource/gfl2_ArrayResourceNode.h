#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_ARRAYRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_ARRAYRESOURCENODE_H_INCLUDED

#include "gfl2_ResourceNode.h"

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

  class ShaderResourceNode;
  class TextureResourceNode;

  /**
    @brief  配列で保持するノード
  */
  class ArrayResourceNode : public NodeTrait<ArrayResourceNode, ResourceNode>
  {
    friend class ResourceVisitor;

    enum Type {
      Shader,
      Texture,

      Max
    };

  public:
    static util::FnvHash GetTypeHash()
    {
		  static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ArrayResourceNode") ) );
		  return hash;
    }

    static ArrayResourceNode *Create(gfx::IGLAllocator *pAllocator)
    {
      gfx::GLMemory::SetInstanceAllocator(pAllocator);
      ArrayResourceNode *node = new ArrayResourceNode();
      gfx::GLMemory::ClearInstanceAllocator();
      return node;
    }

    virtual ~ArrayResourceNode();
    virtual void AddChild(DagNode* node);

    void Freeze(gfx::IGLAllocator *pAllocator);
    bool IsFreezed() const { return m_isFreezed; }

    template<typename T>
    const T **GetResourceNode(u32 *nodeCount) const;

  protected:
    ArrayResourceNode();

    const ResourceNode **GetResourceNode(Type type, u32 *nodeCount) const
    {
      if(nodeCount)
      {
        *nodeCount = m_nodeCount[type];
      }
      return m_nodes[type];
    }

  private:
    u32 m_nodeCount[Max];
    const ResourceNode **m_nodes[Max];

    bool m_isFreezed;
  };

  template<>
  inline const ShaderResourceNode **ArrayResourceNode::GetResourceNode(u32 *nodeCount) const
  {
    return reinterpret_cast<const ShaderResourceNode **>(GetResourceNode(Shader, nodeCount));
  }

  template<>
  inline const TextureResourceNode **ArrayResourceNode::GetResourceNode(u32 *nodeCount) const
  {
    return reinterpret_cast<const TextureResourceNode **>(GetResourceNode(Texture, nodeCount));
  }

}}}}

#endif
