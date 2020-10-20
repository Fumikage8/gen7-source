//===================================================================
/**
 * @file    FieldKusaModel.h
 * @brief   草モデルクラス
 * @author  ishiguro_masateru
 * @date    2015.05.23
 */
//===================================================================

#if !defined( KUSA_MODEL_H_INCLUDED )
#define KUSA_MODEL_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Math.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( KusaModel )


/**
 * @class 草モデルクラス
 */
class FieldKusaModel : public gfl2::renderingengine::scenegraph::NodeTrait<FieldKusaModel,
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode>
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldKusaModel); //コピーコンストラクタ＋代入禁止
public:

	static const f32							GrassSizeX;//75.0f
	static const f32							GrassSizeY;//75.0f
  static const f32							GrassXOffSet;//45.0f
	static const f32							KusaErrY;//-5000.0f

	static const int							GrassXMax = 56;
	static const int							GrassYMax = 56;
	static const int							GridTotalNum = GrassXMax * GrassYMax;

  static const int							BlockGrassXNum = 14;
	static const int							BlockGrassYNum = 14;
  static const int							BlockIncludeMax = BlockGrassXNum * BlockGrassYNum;//１９６フラグ

  static const int							BlockXNum = GrassXMax / BlockGrassXNum;
	static const int							BlockYNum = GrassYMax / BlockGrassYNum;
  static const int							BlockMax = BlockXNum * BlockYNum;

	static const u32						s_KusaMax = 196;

	struct KusaGrid{
		u32					grid_x;
		u32					grid_y;
	};

	static FieldKusaModel* CreateKusaModelInstanceNode( gfl2::gfx::IGLAllocator* pAllocator,
																											gfl2::renderingengine::scenegraph::resource::ResourceNode* pSrc,
																											gfl2::renderingengine::scenegraph::resource::ResourceNode* pTextureShaderNodeRoot);

	virtual ~FieldKusaModel();

	void InitKusaShader( gfl2::gfx::IGLAllocator* pAllocator );

	void SetShapeDrawFlag( s32 grid_x, s32 grid_y, b32 flag );
	void SetShapeDrawFlag( KusaGrid grid, b32 flag ){ SetShapeDrawFlag( grid.grid_x, grid.grid_y, flag ); }

	KusaGrid GetKusaGrid( gfl2::math::Vector3 worldPos ) const;//<- ワールドマトリックスの逆行列を使うのでトラバース後に使ってください。
	KusaGrid GetKusaGrid( gfl2::math::Vector4 worldPos ) const { return GetKusaGrid( gfl2::math::Vector3(worldPos.x, worldPos.y, worldPos.z) ); }

	gfl2::math::Vector3 GetKusaWorldPos( s32 grid_x, s32 grid_y ) const;
	gfl2::math::Vector3 GetKusaWorldPos( KusaGrid grid ) const { return GetKusaWorldPos( grid.grid_x, grid.grid_y ); }


protected:

	class FieldKusaShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
	{
	public:

		struct FieldKusaConstants
		{
			static const u32			s_VisibleRegNum	 = (s_KusaMax/4) + 1;//50

			gfl2::math::Vector	cVisible[s_VisibleRegNum];

			static const gfl2::gfx::ConstantBufferElement* GetConstantBufferElement()
			{
				static const gfl2::gfx::ConstantBufferElement element[] = {
					{ gfl2::gfx::ConstantType::Vector, s_VisibleRegNum },
					gfl2::gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
				};
				return element;
			}

			void Set( const gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode *pMaterial );
		};

		FieldKusaShaderDriver();
		virtual ~FieldKusaShaderDriver();

		virtual b32 Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag *pMeshDrawTag ) const;
		void SetShapeDrawFlag( s32 index, b32 flag );

		gfl2::renderingengine::renderer::IShaderDriver									*m_pOrigDriver;
		gfl2::gfx::ConstantBuffer																				*m_FieldKusaConstantsBuffer;
		FieldKusaConstants																							m_FieldKusaConstants;
	};

	FieldKusaModel();

	FieldKusaShaderDriver																							*m_FieldKusaShaderDriverMtx[BlockXNum][BlockYNum];
};

GFL_NAMESPACE_END( KusaModel )
GFL_NAMESPACE_END( Field )

#endif // KUSA_MODEL_H_INCLUDED
