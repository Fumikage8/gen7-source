#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_AnimationResourceNode_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_AnimationResourceNode_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class AnimationResourceNodeFactory;

class AnimationResourceNode : public NodeTrait<AnimationResourceNode, ResourceNode>
{
public:

	friend class AnimationResourceNodeFactory;

	virtual ~AnimationResourceNode();

	void SetData( const void* data, u32 size );

	u32 GetFrameSize() const { return m_pAnimTop->frameSize; }
	math::AABB GetAABB() const
  {
    return math::AABB(
      math::Vector4(m_pAnimTop->aabb[0], m_pAnimTop->aabb[1], m_pAnimTop->aabb[2], 1.0f),
      math::Vector4(m_pAnimTop->aabb[3], m_pAnimTop->aabb[4], m_pAnimTop->aabb[5], 1.0f)); 
  }

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::AnimationResourceNode") ) );
		return hash;
	}

  static bool IsGfbmotSupportedVersion(u32 version);

  bool HasJointAnimation() const
  {
    return m_pJointAnimHeader != NULL;
  }

  bool HasBlendShapeAnimation() const
  {
    return false;
  }

  bool HasMaterialAnimation() const
  {
    return
      (m_pUvAnimHeader != NULL) ||
      (m_pTexturePatternAnimHeader != NULL) ||
      (m_pShaderAnimHeader != NULL);
  }

  bool HasVisibilityAnimation() const
  {
    return m_pVisibilityAnimHeader != NULL;
  }

  bool HasKeyAnimation() const
  {
    return m_pKeyAnimHeader != NULL;
  }

  bool HasEnvLightAnimation() const
  {
    return m_pEnvLightAnimHeader != NULL;
  }

  bool HasEnvCameraAnimation() const
  {
    return m_pEnvCameraAnimHeader != NULL;
  }

  //kawa20150514
  bool HasEnvFogAnimation() const
  {
    return m_pEnvFogAnimHeader != NULL;
  }

  bool HasEnvGlareAnimation() const
  {
    return m_pEnvGlareAnimHeader != NULL;
  }

  u32 GetFileNameHash() const;

protected:
	AnimationResourceNode();

public:

  struct AnimType
  {
    enum
    {
      Top,
      Joint,
      BlendShape,
      Uv,
      TexturePattern,
      Shader,
      Visibility,
      Key,
      EnvLight,
      EnvCamera,
      EnvFog,    //kawa20150514
      EnvGlare,  //kawa20150514
    };
  };

  struct Name
  {
    u8 length;
    char name[1];

    bool IsSameName(const c8* pName) const
    {
      // 文字数が同じか？
      u32 nameLength = ::std::strlen(pName);
      if (this->length != nameLength)
      {
        return false;
      }

      // 文字列が全て一致しているか？
      return (::std::memcmp(this->name, pName, this->length) == 0);
    }
  };

  struct DataHeader
  {
    u32 type;
    u32 size;
    u32 offset;
  };

  struct AnimData
  {
    u32 version;
    u32 dataCount;
    DataHeader dataHeaderList[1];
  };

  struct AnimTop
  {
    u32 frameSize;
    u16 loopFlag;
    u16 dataLoopFlag; // データ的に最初と最後のフレームが一致しているか？
    f32 aabb[6];
    u32 fileNameHash;
  };

  struct JointAnimHeader
  {
    u32 jointCount; // アニメーションしているジョイント数
    u32 nameBufferSize; // 名前バッファサイズ
  };

  struct JointAnimBody
  {
    u32 flags; // SRT各3bitずつ
    u32 dataSize;
    u32 data[1];
  };

  struct UvAnimHeader
  {
    u32 materialCount;
    u32 nameBufferSize; // 名前バッファサイズ

    u32 textureSlotCountList[1];
  };

  struct UvAnimBody
  {
    u32 textureSlotNo;
    u32 flags; // SRT各3bitずつ
    u32 dataSize;
    u32 data[1];
  };

  struct TexturePatternAnimHeader
  {
    u32 materialCount;
    u32 nameBufferSize; // 名前バッファサイズ
    u32 texIdCount;
    u32 texIdBufferSize;

    u32 textureSlotCountList[1];
  };

  struct TexturePatternAnimBody
  {
    struct Key
    {
      u16 frame;
      u16 value;
    };

    u32 textureSlotNo;
    u32 keyCount;
    Key keyList[1];
  };

  struct ShaderAnimHeader
  {
    u32 materialCount;
    u32 nameBufferSize; // 名前バッファサイズ

    u32 constantColorCountList[1];
  };

  struct ShaderAnimBody
  {
    u32 constantColorNo;
    u32 flags; // SRT各3bitずつ
    u32 dataSize;
    u32 data[1];
  };

  struct VisibilityAnimHeader
  {
    u32 meshCount;
    u32 nameBufferSize; // 名前バッファサイズ
  };

  struct VisibilityAnimBody
  {
    u8 visibleFlags[1];
  };

  struct KeyAnimHeader
  {
    u32 attributeCount;
  };

  struct KeyAnimBody
  {
    struct Key
    {
      s32 frame;
      f32 value;
    };

    u32 keyCount;
    Key keyList[1];
  };

  struct EnvLightAnimHeader
  {
    u32 lightCount;
    u32 nameBufferSize;
  };

  struct EnvLightAnimBody
  {
    u32 lightSetNo;
    u32 lightType;

    u64 flags; // SRT各3bitずつ
    u32 dataSize;
    u32 pad;
    u64 data[1];
  };

  struct EnvCameraAnimHeader
  {
    u32 cameraCount;
    u32 nameBufferSize;
  };

  struct EnvCameraAnimBody
  {
    u64 flags; // SRT各3bitずつ
    u32 dataSize;
    u32 data[1];
  };

  //kawa20150514
  struct EnvFogAnimHeader
  {
    u32 fogCount;
    u32 pad;
  };

  struct EnvFogAnimBody
  {
    u32 fogNo;
    u32 pad;
    u64 flags; // 各3bitずつ
    u32 dataSize;
    u32 data[1];
  };

  struct EnvGlareAnimHeader
  {
    u32 dummy;  // ヘッダーなし。EnvGlareAnimBodyから始まります。
  };

  struct EnvGlareAnimBody
  {
    u64 flags; // 各3bitずつ
    u32 dataSize;
    u32 data[1];
  };

public:

  const AnimData* m_pAnimData;

  // Top
  const AnimTop* m_pAnimTop;

  // Joint
  const JointAnimHeader* m_pJointAnimHeader;

  //BlendShape,

  // Uv
  const UvAnimHeader* m_pUvAnimHeader;

  //TexturePattern,
  const TexturePatternAnimHeader* m_pTexturePatternAnimHeader;

  //Shader
  const ShaderAnimHeader* m_pShaderAnimHeader;

  //Visibility,
  const VisibilityAnimHeader* m_pVisibilityAnimHeader;

  // Key
  const KeyAnimHeader* m_pKeyAnimHeader;

  // EnvLight,
  const EnvLightAnimHeader* m_pEnvLightAnimHeader;

  // EnvCamera,
  const EnvCameraAnimHeader* m_pEnvCameraAnimHeader;

  //kawa20150514
  // EnvFog
  const EnvFogAnimHeader* m_pEnvFogAnimHeader;

  // EnvGlare
  const EnvGlareAnimHeader* m_pEnvGlareAnimHeader;
};

}}}}

#endif
