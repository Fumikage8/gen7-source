#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_TRANSFORMNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_TRANSFORMNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceNode.h>

#include <math/include/gfl2_SRT.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { 
	
class SceneGraphManager;

namespace instance {

class InstanceCreator;

class TransformNode : public NodeTrait<TransformNode, InstanceNode>
{
public:

	//! フラグの列挙型
	class TransformFlag {
	public:
		enum {
      LocalDirty            = (1 << 0),
      WorldDirty            = (1 << 1),
      NonInheritanceScale   = (1 << 2),
      JointNode             = (1 << 3),
      BillboardY        = (1 << 4),
      BillboardXY        = (1 << 5),
		};
	};

  static TransformNode* Create( gfx::IGLAllocator* allocator )
  {
    return GLNew(allocator) TransformNode();
  }

	friend class InstanceCreator;
	friend class SceneGraphManager;

	virtual ~TransformNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode") ) );
		return hash;
	}

	virtual void AddChild( DagNode* child);

	//inline void SetLocalMatrix( const math::Matrix34& mat, b32 fDirty = true ){ m_LocalMatrix = mat; SetLocalTransformDirty(fDirty); }
	//inline const math::Matrix34& GetLocalMatrix() const { return m_LocalMatrix; }

	inline void SetWorldMatrix( const math::Matrix34& mat, b32 fDirty = false )
	{
		if ( fDirty )
		{
			SetWorldTransformDirty(fDirty);//強制更新
		}
		else
		{
			if ( m_WorldMatrix[0][3] != mat[0][3] || m_WorldMatrix[1][3] != mat[1][3] || m_WorldMatrix[2][3] != mat[2][3] ) 
				SetWorldTransformDirty(true);
		}

		m_WorldMatrix = mat;
	}
	inline const math::Matrix34& GetWorldMatrix() const { return m_WorldMatrix; }
	inline const math::Matrix34* GetWorldMatrixPtr() const { return &m_WorldMatrix; }

	void SetTransformDirty( b32 f )
  {    
    SetLocalTransformDirty( f );    
    SetWorldTransformDirty( f );
  }

	void SetLocalTransformDirty( b32 f ){    
    if (f) {
      this->SetTransformFlag(TransformFlag::LocalDirty);
    } else {
      this->ResetTransformFlag(TransformFlag::LocalDirty);
    }
  }
	inline b32 IsLocalTransformDirty() const { return this->GetTransformFlag(TransformFlag::LocalDirty); }

	virtual void SetWorldTransformDirty( b32 f ){
    if(IsBillboard()) f = true; // Always dirty for Billboard!
    if (f) {
      this->SetTransformFlag(TransformFlag::WorldDirty);
    } else {
      this->ResetTransformFlag(TransformFlag::WorldDirty);
    }
  }
	inline b32 IsWorldTransformDirty() const { return this->GetTransformFlag(TransformFlag::WorldDirty); }

  // ローカルSRTの一括取得
  const math::SRT& GetLocalSRT() const
  {
    return m_LocalSRT;
  }
  // ローカルSRTの一括設定
  void SetLocalSRT(const math::SRT& localSRT, b32 fDirty = true)
  {
    SetLocalTransformDirty(fDirty);
    m_LocalSRT = localSRT;
  }

  // ローカルスケールの取得
  const math::Vector3& GetLocalScale() const
  {
    return m_LocalSRT.scale;
  }
  // ローカルスケールの設定
  void SetLocalScale(const math::Vector3& localScale, b32 fDirty = true)
  {
    SetLocalTransformDirty(fDirty);
    m_LocalSRT.scale = localScale;
  }

  // ローカルローテーションの取得
  const math::Quaternion& GetLocalRotation() const
  {
    return m_LocalSRT.rotation;
  }
  // ローカルローテーションの設定
  void SetLocalRotation(const math::Quaternion& localRotation, b32 fDirty = true)
  {
    SetLocalTransformDirty(fDirty);
    m_LocalSRT.rotation = localRotation;
  }

  // ローカルトランスレートの取得
  const math::Vector3& GetLocalTranslate() const
  {
    return m_LocalSRT.translate;
  }
  // ローカルトランスレートの設定
  void SetLocalTranslate(const math::Vector3& localTranslate, b32 fDirty = true)
  {
    SetLocalTransformDirty(fDirty);
    m_LocalSRT.translate = localTranslate;
  }

  // 親のスケールを引き継がないフラグ：Mayaの行列準拠
  void SetNonInheritanceScale(b32 f) {
    if (f) {
      this->SetTransformFlag(TransformFlag::NonInheritanceScale);
    } else {
      this->ResetTransformFlag(TransformFlag::NonInheritanceScale);
    }
  }
  b32 IsNonInheritanceScale() { return this->GetTransformFlag(TransformFlag::NonInheritanceScale); }

  void SetBillboardType(int type) {
    if (type==1) // Y
    {
      this->SetTransformFlag(TransformFlag::BillboardY);
    } 
    else if(type==2) // XY
    {
      this->SetTransformFlag(TransformFlag::BillboardXY);
    }    
    else // NONE
    {
      this->ResetTransformFlag(TransformFlag::BillboardY);
      this->ResetTransformFlag(TransformFlag::BillboardXY);
    }
  }
  b32 IsBillboardY() { return this->GetTransformFlag(TransformFlag::BillboardY); }
  b32 IsBillboardXY() { return this->GetTransformFlag(TransformFlag::BillboardXY); }
  b32 IsBillboard() { return(IsBillboardY() || IsBillboardXY() );}

  // JointNodeフラグ取得
  b32 IsJointNode() const
  {
    return this->GetTransformFlag(TransformFlag::JointNode);
  }

protected:

  void SetTransformFlag(u32 flag)
  {
    m_TransformFlags |= flag;
  }
  void ResetTransformFlag(u32 flag)
  {
    m_TransformFlags &= ~flag;
  }
  b32 GetTransformFlag(u32 flag) const
  {
    return m_TransformFlags & flag;
  }

protected:
	TransformNode();

  math::SRT                           m_LocalSRT;
	math::Matrix34                      m_WorldMatrix;

  u32                                 m_TransformFlags;
};

}}}}

#endif

