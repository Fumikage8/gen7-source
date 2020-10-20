#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ArrayResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

  class ResourceVisitor
  {
  public:
    ResourceVisitor() :
      m_root(NULL)
    {
      memset(m_nodeCount, 0, sizeof(m_nodeCount));
    }

    void Visit(DagNode *node)
    {
      if(node->IsKindOf<ShaderResourceNode>())
      {
        AddNode(static_cast<ResourceNode *>(node), ArrayResourceNode::Shader);
      }
      else if(node->IsKindOf<TextureResourceNode>())
      {
        AddNode(static_cast<ResourceNode *>(node), ArrayResourceNode::Texture);
      }
    }

    void AddNode(ResourceNode *node, ArrayResourceNode::Type type)
    {
      GFL_ASSERT(m_nodeCount[type] < m_root->m_nodeCount[type]);

      /*
        ソートしながら追加している
        最後にまとめて適切なアルゴリズムでソートする方が早いかも？
      */
      u32 hash = node->GetNameHash();

      for(u32 i=0;i<m_nodeCount[type];i++)
      {
        if(hash < m_root->m_nodes[type][i]->GetNameHash())
        {
          for(u32 j=m_nodeCount[type];j>i;j--)
          {
            m_root->m_nodes[type][j] = m_root->m_nodes[type][j-1];
          }
          m_root->m_nodes[type][i] = node;
          m_nodeCount[type]++;
          return;
        }
      }

      m_root->m_nodes[type][m_nodeCount[type]] = node;
      m_nodeCount[type]++;
    }

    u32 m_nodeCount[ArrayResourceNode::Max];
    ArrayResourceNode *m_root;
  };

  /**
    @brief  コンストラクタ
  */
  ArrayResourceNode::ArrayResourceNode() :
    m_isFreezed(false)
  {
    memset(m_nodeCount, 0, sizeof(m_nodeCount));
    memset(m_nodes, 0, sizeof(m_nodes));
  }

  /**
    @brief  デストラクタ
  */
  ArrayResourceNode::~ArrayResourceNode()
  {
    GFL_SAFE_DELETE_ARRAY(m_nodes[0]);
  }

  /**
    @brief  ノード追加
    @param  node 追加する子ノード
  */
  void ArrayResourceNode::AddChild(DagNode *node)
  {
    if(m_isFreezed)
    {
      GFL_ASSERT(0);
      return;
    }

    if(node->IsKindOf<ShaderResourceNode>())
    {
      m_nodeCount[Shader]++;
    }
    else if(node->IsKindOf<TextureResourceNode>())
    {
      m_nodeCount[Texture]++;
    }

    ResourceNode::AddChild(node);
  }

  /**
    ノードを固める
  */
  void ArrayResourceNode::Freeze(gfx::IGLAllocator *pAllocator)
  {
    if(m_isFreezed)
    {
      GFL_ASSERT(0);
      return;
    }

    u32 count = 0;
    for(int i=0;i<Max;i++)
    {
      count += m_nodeCount[i];
    }

    const ResourceNode **nodes = GFL_NEW_ARRAY(pAllocator) const ResourceNode *[count];
    count = 0;
    for(int i=0;i<Max;i++)
    {
      m_nodes[i] = &nodes[count];
      count += m_nodeCount[i];
    }

    Traverser::Visitor<DagNode, ResourceVisitor> visitor;
    visitor.m_root = this;
    Traverser::Traverse(this, &visitor, 0xffffffff);

    m_isFreezed = true;
  }

}}}}
