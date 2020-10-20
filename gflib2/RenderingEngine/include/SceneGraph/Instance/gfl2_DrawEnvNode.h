#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_DRAWENVNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_DRAWENVNODE_H_INCLUDED

#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightSetNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CameraNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <math/include/gfl2_MathAABB.h>


//kawa20150507
#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRCommandBufferManager.h>
#endif  // GF_PLATFORM_CTR


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
class ModelInstanceNode;
class MaterialInstanceNode;

class DrawEnvNode : public NodeTrait<DrawEnvNode, CullingObjectNode>
{
#if defined(GF_PLATFORM_CTR)
  static const u32 TABLE_COMMAND_BUFFER_SIZE = ( nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM + 4 ) * 4   +   8+8;  // 参照テーブルの要素数*4バイト   +   ダミーコマンド+キックコマンド(16の倍数にしてある)
#endif

public:

	friend class InstanceCreator;

	static const u32																CameraMax = 32;
	static const u32																LightSetMax = 32;
	static const u32																LinkMax = 32;
  static const u32                                FogMax = resource::DrawEnvResourceNode::FOG_NUM;  //kawa20150514

	struct DepthOfFieldParam{
		DepthOfFieldParam();
		f32					m_Near;
		f32					m_Far;
		f32					m_Scale;
	};


  //kawa20150507
  //! @brief  フォグのテーブルに影響を及ぼす外部パラメータ
  class FogOuterParam
  {
  public:
    f32            wScale;          //!< WScaleの値  // WScaleはカメラからcameraWScaleを取得したほうがいいか？
                                    //   0のときZバッファ、0以外のときWバッファ
                                    //   Wスケールの値は、透視射影のときは1/cameraFar、平行射影のときは1。

    f32            cameraNear;      //!< カメラのニアクリップ距離
    f32            cameraFar;       //!< カメラのファークリップ距離

    f32            depthRangeNear;  //!< 深度バッファの範囲のニア値(wScaleが0以外のとき0)
    f32            depthRangeFar;   //!< 深度バッファの範囲のファー値(wScaleが0以外のとき1)

    math::Matrix44 projMatrix;      //!< 射影行列

  public:
    FogOuterParam(void)
      : wScale(0.0f),
        cameraNear(1.0f),
        cameraFar(10000.0f),
        depthRangeNear(0.0f),
        depthRangeFar(1.0f),
        projMatrix(math::Matrix44::GetIdentity())
    {}
  };


	virtual ~DrawEnvNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::DrawEnvNode") ) );
		return hash;
	}

	void ReadDrawEnvResourceNode( const resource::DrawEnvResourceNode &rDrawEnvResourceNode );

	inline void AddLightSet( LightSetNode *pNode ){ m_LightSets[ pNode->GetSetNo() ] = pNode; AddChild(pNode); }
	inline void RemoveLightSet( LightSetNode *pNode ){ m_LightSets[ pNode->GetSetNo() ] = NULL; RemoveChild(pNode); }
	LightSetNode* GetLightSet( s32 setNo );

	inline void AddCamera( CameraNode *pNode ){ m_CameraContainer.push_back( pNode ); AddChild(pNode); }
	inline void RemoveCamera( CameraNode *pNode ){ m_CameraContainer.remove( pNode ); RemoveChild(pNode); }
	inline u32 GetCameraNum(){ return m_CameraContainer.size(); }
	inline CameraNode* GetCamera( u32 no ){ return m_CameraContainer[no]; }

	inline u32 GetLinkEnvNum() const { return m_LinkDrawEnvContainer.size(); }
	inline const DrawEnvNode* GetLinkDrawEnvNode( u32 no ) const { return m_LinkDrawEnvContainer[no]; }

	inline void SetFrontDOF( DepthOfFieldParam dof ){ m_FrontDOF = dof; }
	inline void SetBackDOF( DepthOfFieldParam dof ){ m_BackDOF = dof ; }
	inline void SetPriority( u32 value ){ m_Priority = value; }
	inline void SetIsLinkEnv( u32 value ){ m_IsLinkEnv = value; }

  inline void SetFogEnable( bool enable ){ m_FogEnable = enable; }
  inline void SetFogNearLength( f32 value ){ m_FogNearLength = value; }
  inline void SetFogFarLength( f32 value ){ m_FogFarLength = value; }
  inline void SetFogStrength( f32 value )
  {
    if(value>2.0f)      m_FogStrength = 2.0f;
    else if(value<0.0f) m_FogStrength = 0.0f;
    else                m_FogStrength = value;
  }
  inline void SetFogColor( const math::Vector4& col ){ m_FogColor = col; }

  inline void SetGlareEnable( bool enable ){ m_GlareEnable = enable; }
  inline void SetGlareReduceNum( u32 value ){ m_GlareReduceNum = value; }
  inline void SetGlareStrength( f32 value ){ m_GlareStrength = value; }
	inline void SetGlareRange( f32 value ){ m_GlareRange = value; }
	inline void SetGlareIntensity( f32 value ){ m_GlareIntensity = value; }
  inline void SetGlareWeight( const math::Vector4& weight ){ m_GlareWeight = weight; }

	inline DepthOfFieldParam	GetFrontDOF() const { return m_FrontDOF; }
	inline DepthOfFieldParam	GetBackDOF() const { return m_BackDOF; }
	inline u32								GetPriority() const { return m_Priority; }
	inline u32								GetIsLinkEnv() const { return m_IsLinkEnv; }

  inline bool               GetFogEnable() const { return m_FogEnable; }
  inline f32								GetFogNearLength() const { return m_FogNearLength; }
  inline f32								GetFogFarLength() const { return m_FogFarLength; }
  inline f32                GetFogStrength() const { return m_FogStrength; }        
  inline math::Vector4				GetFogColor() const { return m_FogColor; }

  inline bool               GetGlareEnable() const { return m_GlareEnable; }
  inline u32                GetGlareReduceNum() const { return m_GlareReduceNum; }
  inline f32                GetGlareStrength() const { return m_GlareStrength; }
	inline f32								GetGlareRange() const { return m_GlareRange; }
	inline f32								GetGlareIntensity() const { return m_GlareIntensity; }
  inline math::Vector4       GetGlareWeight() const { return m_GlareWeight; }


  //kawa20150507
  //! @brief  フォグのコマンドを更新する
  //! @param[in]  param  外部パラメータ
  //! @note  CTR版でしか処理を行いません。Win32版では何もしません。
  void UpdateFog(const FogOuterParam& param);

  //! @brief  マテリアルのフォグを更新する
  //! @param[in]  pModelInstanceNode  更新するモデル(この関数内でマテリアルが書き換えられます)。
  //!                                 DrawEnvNodeのAABBに含まれていない場合は何もしません。
  //! @return  DrawEnvNodeのAABBに含まれており、書き換えた場合はtrueを返す。
  //! @note  Win32版でしか処理を行いません。CTR版では何もしません。
  bool UpdateMaterialFog(ModelInstanceNode* pModelInstanceNode) const;

  //! @brief  マテリアルのフォグを更新する
  //! @param[in]  pMaterialInstanceNode  更新するマテリアル(この関数内でマテリアルが書き換えられます)。
  //!                                    DrawEnvNodeのAABBに含まれているか否かの判定は行えないので、必ず書き換えます。
  //! @note  Win32版でしか処理を行いません。CTR版では何もしません。
  void UpdateMaterialFog(MaterialInstanceNode* pMaterialInstanceNode) const;

  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  inline gfx::ctr::CTRCommandBufferManager<TABLE_COMMAND_BUFFER_SIZE>* GetFogTableCommandCache(void) { return m_pFogTableCommandCache; }
  inline gfx::ctr::CTRCommandBufferManager<16>* GetFogColorCommandCache(void) { return m_pFogColorCommandCache; }
#endif  // GF_PLATFORM_CTR




protected:
	DrawEnvNode();

private:

	c8									m_EnvTextureFilePath[Constant::FLAT_RESOURCE_NAME_MAX];
	
	DepthOfFieldParam		m_FrontDOF;
	DepthOfFieldParam		m_BackDOF;

	u32									m_Priority;
	u32									m_IsLinkEnv;

  bool                m_FogEnable;
	f32									m_FogNearLength;
	f32									m_FogFarLength;
  f32                 m_FogStrength;  // 0.0fのときフォグが全く掛からない。1.0fのとき標準の掛かり方。2.0fのときフォグが全体に掛かっている。
  math::Vector4				m_FogColor;

  bool                m_GlareEnable;
  u32                 m_GlareReduceNum;
  f32                 m_GlareStrength;
	f32									m_GlareRange;
	f32									m_GlareIntensity;
  math::Vector4        m_GlareWeight;

	LightSetNode																					*m_LightSets[LightSetMax];
	util::FixedSizeContainer<CameraNode*,CameraMax>				m_CameraContainer;
	util::FixedSizeContainer<DrawEnvNode*,LinkMax>				m_LinkDrawEnvContainer;										//! 隣接DrawEnv。補間時に用いる。


  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  // フォグのコマンド
 
  //! @brief  フォグのパラメータ
  class FogParam
  {
  public:
    math::Vector4   color;
    f32            nearLength;
    f32            farLength;
    f32            strength;
    FogOuterParam  outerParam;
  public:
    FogParam(void)
      : color(math::Vector4::GetZero()),
        nearLength(0.0f),
        farLength(0.0f),
        strength(0.0f),
        outerParam()
    {}
  };

  // フォグの可変コマンド
  void allocFogMutableCommandCache(void);
  void freeFogMutableCommandCache(void);
  FogParam  m_FogMutableCommandParam;
  bool      m_IsFogMutableCommandCreated;
  gfx::ctr::CTRCommandBufferManager<TABLE_COMMAND_BUFFER_SIZE> *m_pFogTableCommandCache;
  gfx::ctr::CTRCommandBufferManager<16> *m_pFogColorCommandCache;
#endif  // GF_PLATFORM_CTR


	//他に入るパラメーター
	//Fog
	//Camera near far
	//EnvMap
	//Wind
	//ポスト処理パラメーター DOF、HDR等


};

}}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif

