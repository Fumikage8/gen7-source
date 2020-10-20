#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_CollisionMeshResourceNode_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_CollisionMeshResourceNode_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_MathAABB.h>

#define TEST_NIJI 1

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class CollisionTriangle
{
public:

  static f32 Vec2Cross(const math::Vector4& a, const math::Vector4& b)
  {
    return (a.z * b.x) - (b.z * a.x);
  }

  bool IntersectsXZ(f32* out_pHeight, const math::Vector4& position) const
  {
    // トライアングル内にいるか判定
    math::Vector4 v_0p = position - m_Positions[0];
    math::Vector4 v_01 = m_Positions[1] - m_Positions[0];

    if (Vec2Cross(v_0p, v_01) > 0.0f)
    {
      return false;
    }
    
    math::Vector4 v_1p = position - m_Positions[1];
    math::Vector4 v_12 = m_Positions[2] - m_Positions[1];

    if (Vec2Cross(v_1p, v_12) > 0.0f)
    {
      return false;
    }

    math::Vector4 v_2p = position - m_Positions[2];
    math::Vector4 v_20 = m_Positions[0] - m_Positions[2];

    if (Vec2Cross(v_2p, v_20) > 0.0f)
    {
      return false;
    }

    // 高さを計算
    *out_pHeight = m_Positions[0].y - (((m_Normal.x * v_0p.x) + (m_Normal.z * v_0p.z)) / m_Normal.y); 
    return true;
  }

  math::Vector4 GetClosestPoint(const math::Vector4& position) const
  {
    math::Vector4 v_0p = position - m_Positions[0];
    math::Vector4 v_01 = m_Positions[1] - m_Positions[0];
    f32 v_01Length = v_01.Length3();
    math::Vector4 v_01N = v_01 / v_01Length;
    f32 v_01Dot = v_01N.Dot3(v_0p);
    f32 rate0 = v_01Dot / v_01Length;
    if (rate0 < 0.0f) rate0 = 0.0f;
    if (rate0 > 1.0f) rate0 = 1.0f;
    math::Vector4 p0 = m_Positions[0] + v_01 * rate0;

    math::Vector4 v_1p = position - m_Positions[1];
    math::Vector4 v_12 = m_Positions[2] - m_Positions[1];
    f32 v_12Length = v_12.Length3();
    math::Vector4 v_12N = v_12 / v_12Length;
    f32 v_12Dot = v_12N.Dot3(v_1p);
    f32 rate1 = v_12Dot / v_12Length;
    if (rate1 < 0.0f) rate1 = 0.0f;
    if (rate1 > 1.0f) rate1 = 1.0f;
    math::Vector4 p1 = m_Positions[1] + v_12 * rate1;

    math::Vector4 v_2p = position - m_Positions[2];
    math::Vector4 v_20 = m_Positions[0] - m_Positions[2];
    f32 v_20Length = v_20.Length3();
    math::Vector4 v_20N = v_20 / v_20Length;
    f32 v_20Dot = v_20N.Dot3(v_2p);
    f32 rate2 = v_20Dot / v_20Length;
    if (rate2 < 0.0f) rate2 = 0.0f;
    if (rate2 > 1.0f) rate2 = 1.0f;
    math::Vector4 p2 = m_Positions[2] + v_20 * rate2;

    math::Vector4 sub0 = position - p0;
    math::Vector4 sub1 = position - p1;
    math::Vector4 sub2 = position - p2;

    f32 l0 = sub0.Dot3(sub0);
    f32 l1 = sub1.Dot3(sub1);
    f32 l2 = sub2.Dot3(sub2);

    math::Vector4 result = p0;
    f32 resultL = l0;
    if (resultL > l1)
    {
      result = p1;
      resultL = l1;
    }
    if (resultL > l2)
    {
      result = p2;
      resultL = l2;
    }

    return result;
  }

  /**
   * @brief 線分との当たり判定
   * @param [out] pOutIntersection 交点の座標(当たっていなければ何も入れない)
   * @param [in] st,ed 線分の始点と終点
   * @retval true 当たっている
   * @retval false 当たっていない
   */
  bool IntersectsRay( math::Vector4* pOutIntersection, const math::Vector4& st, const math::Vector4& ed) const
  {
    math::Vector4 v1 = st - m_Positions[0];
    math::Vector4 v2 = ed - m_Positions[0];

    // 線分と平面が当たっているか
    if( v1.Dot3( m_Normal) * v2.Dot3( m_Normal) > 0.0f)
    {
      return false;
    }

    // 交点を計算
    f32 d1 = math::FAbs( m_Normal.Dot3( v1)) / m_Normal.Length3();
    f32 d2 = math::FAbs( m_Normal.Dot3( v2)) / m_Normal.Length3();
    if( d1 + d2 == 0.0f )
    {
      return false;
    }
    f32 rate = d1 / (d1 + d2);
    math::Vector4 intersection = m_Positions[0] + (v1 * (1.0f - rate) + v2 * rate);

    // 交点がポリゴン内に含まれているか
    math::Vector4 v10 = m_Positions[1] - m_Positions[0];
    math::Vector4 vi0 = intersection - m_Positions[0];
    math::Vector4 cross1 = v10.Cross( vi0);

    math::Vector4 v21 = m_Positions[2] - m_Positions[1];
    math::Vector4 vi1 = intersection - m_Positions[1];
    math::Vector4 cross2 = v21.Cross( vi1);

    math::Vector4 v02 = m_Positions[0] - m_Positions[2];
    math::Vector4 vi2 = intersection - m_Positions[2];
    math::Vector4 cross3 = v02.Cross( vi2);

    if( cross1.Dot3( cross2) < 0.0f || cross1.Dot3( cross3) < 0.0f)
    {
      return false;
    }

#if 1
    // 各辺の延長線上に交点が含まれる場合があるので、辺内に交点があるかもチェック
    if (cross1.Dot3(cross1) < 0.0001f) // 適当な誤差
    {
      f32 l1 = v10.Length3();
      math::Vector4 n1 = v10 / l1; // 正規化
      f32 e1 = n1.Dot3(vi0) / l1;  // 射影して辺の長さに対する割合を求める
      if (e1 < 0.0f || e1 > 1.0f)
      {
        return false;
      }
    }

    if (cross2.Dot3(cross2) < 0.0001f) // 適当な誤差
    {
      f32 l2 = v21.Length3();
      math::Vector4 n2 = v21 / l2; // 正規化
      f32 e2 = n2.Dot3(vi1) / l2;  // 射影して辺の長さに対する割合を求める
      if (e2 < 0.0f || e2 > 1.0f)
      {
        return false;
      }
    }

    if (cross3.Dot3(cross3) < 0.0001f) // 適当な誤差
    {
      f32 l3 = v02.Length3();
      math::Vector4 n3 = v02 / l3; // 正規化
      f32 e3 = n3.Dot3(vi2) / l3;  // 射影して辺の長さに対する割合を求める
      if (e3 < 0.0f || e3 > 1.0f)
      {
        return false;
      }
    }
#endif

    // 当たってる
    *pOutIntersection = intersection;
    return true;
  }

  /**
   * @brief 線分との当たり判定 交点が辺上だと無効とする版
   * @param [out] pOutIntersection 交点の座標(当たっていなければ何も入れない)
   * @param [in] st,ed 線分の始点と終点
   * @retval 1  当たっている
   * @retval 0  当たっていない
   * @retval -1 計測に失敗
   */
  s32 IntersectsRayWithoutEdge( math::Vector4* pOutIntersection, const math::Vector4& st, const math::Vector4& ed) const
  {
#if 0
    math::Vector4 v1 = st - m_Positions[0];
    math::Vector4 v2 = ed - m_Positions[0];

    // 線分と平面が当たっているか
    if( v1.Dot3( m_Normal) * v2.Dot3( m_Normal) > 0.0f)
    {
      return 0;
    }

    // 交点を計算
    f32 d1 = math::FAbs( m_Normal.Dot3( v1)) / m_Normal.Length3();
    f32 d2 = math::FAbs( m_Normal.Dot3( v2)) / m_Normal.Length3();
    if( d1 + d2 == 0.0f )
    {
      return 0;
    }
    f32 rate = d1 / (d1 + d2);
    math::Vector4 intersection = m_Positions[0] + (v1 * (1.0f - rate) + v2 * rate);

    // 交点が返上にあれば無効にする
    {
      const f32 THRESHOLD = 0.0001f;

      // 辺ベクトル と 交点から頂点へのベクトル が平行であるなら交点は返上にある
      {
        math::Vector4 v1 = m_Positions[0] - intersection;
        math::Vector4 v2 = m_Positions[0] - m_Positions[1];
        v1 = v1.Normalize();
        v2 = v2.Normalize();
        math::Vector4 v3 = v1.Cross( v2 );
        if( v3.Length3() <= THRESHOLD )
        {
          return -1;
        }
      }
      {
        math::Vector4 v1 = m_Positions[1] - intersection;
        math::Vector4 v2 = m_Positions[1] - m_Positions[2];
        v1 = v1.Normalize();
        v2 = v2.Normalize();
        math::Vector4 v3 = v1.Cross( v2 );
        if( v3.Length3() <= THRESHOLD )
        {
          return -1;
        }
      }
      {
        math::Vector4 v1 = m_Positions[2] - intersection;
        math::Vector4 v2 = m_Positions[2] - m_Positions[0];
        v1 = v1.Normalize();
        v2 = v2.Normalize();
        math::Vector4 v3 = v1.Cross( v2 );
        if( v3.Length3() <= THRESHOLD )
        {
          return -1;
        }
      }
    }

    // 交点がポリゴン内に含まれているか
    math::Vector4 v10 = m_Positions[1] - m_Positions[0];
    math::Vector4 vi0 = intersection - m_Positions[0];
    math::Vector4 cross1 = v10.Cross( vi0);

    math::Vector4 v21 = m_Positions[2] - m_Positions[1];
    math::Vector4 vi1 = intersection - m_Positions[1];
    math::Vector4 cross2 = v21.Cross( vi1);

    math::Vector4 v02 = m_Positions[0] - m_Positions[2];
    math::Vector4 vi2 = intersection - m_Positions[2];
    math::Vector4 cross3 = v02.Cross( vi2);

    if( cross1.Dot3( cross2) <= 0.0f || cross1.Dot3( cross3) <= 0.0f)
    {
      return 0;
    }

    // 当たってる
    *pOutIntersection = intersection;

    return 1;
#endif
    b32 isHit = this->IntersectsRay(pOutIntersection, st, ed);

    // 交点が返上にあれば無効にする
    if (isHit)
    {
      const f32 THRESHOLD = 0.0001f;

      // 辺ベクトル と 交点から頂点へのベクトル が平行であるなら交点は返上にある
      {
        math::Vector4 v1 = m_Positions[0] - *pOutIntersection;
        math::Vector4 v2 = m_Positions[0] - m_Positions[1];
        math::Vector4 v3 = v1.Cross(v2);
        if (v3.Length3() <= THRESHOLD)
        {
          pOutIntersection = NULL;
          return -1;
        }
      }
      {
        math::Vector4 v1 = m_Positions[1] - *pOutIntersection;
        math::Vector4 v2 = m_Positions[1] - m_Positions[2];
        math::Vector4 v3 = v1.Cross(v2);
        if (v3.Length3() <= THRESHOLD)
        {
          pOutIntersection = NULL;
          return -1;
        }
      }
      {
        math::Vector4 v1 = m_Positions[2] - *pOutIntersection;
        math::Vector4 v2 = m_Positions[2] - m_Positions[0];
        math::Vector4 v3 = v1.Cross(v2);
        if (v3.Length3() <= THRESHOLD)
        {
          pOutIntersection = NULL;
          return -1;
        }
      }
    }

    return isHit;

  }

  /**
   * @brief 線分との当たり判定 表裏も判定する版
   * @param [out] pOutIntersection 交点の座標(当たっていなければ何も入れない)
   * @param [in] st,ed 線分の始点と終点
   * @retval true 当たっている
   * @retval false 当たっていない
   */
  bool IntersectsRayEx( math::Vector4* pOutIntersection, const math::Vector4& st, const math::Vector4& ed) const
  {
    // レイが裏面から伸びていたら当たっていないとする
    math::Vector4 v1 = ed - st;
    if( v1.Dot3( m_Normal) > 0.0f)
    {
      return false;
    }

    return this->IntersectsRay( pOutIntersection, st, ed);
  }

public:

  math::Vector4 m_Positions[3];
  math::Vector4 m_Normal;
  s32          m_GroundAttribute;
#if TEST_NIJI
    u8         m_vertexColorG[3];
#endif
};

class CollisionMeshResourceNodeFactory;

class CollisionMeshResourceNode : public NodeTrait<CollisionMeshResourceNode, ResourceNode>
{
public:
	friend class CollisionMeshResourceNodeFactory;

	
	virtual ~CollisionMeshResourceNode();

	void SetData(const void* data, int size);

  const CollisionTriangle* GetCollisionTriangle(u32 index) const
  {
    return &m_pCollisionTriangleList[index];
  }

  u32 GetCollisionTriangleCount() const
  {
    return m_CollisionTriangleCount;
  }

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::CollisionMeshResourceNode") ) );
		return hash;
	}

protected:
	CollisionMeshResourceNode();

private:
	
  CollisionTriangle*  m_pCollisionTriangleList;
  u32                 m_CollisionTriangleCount;
};

}}}}

#endif

