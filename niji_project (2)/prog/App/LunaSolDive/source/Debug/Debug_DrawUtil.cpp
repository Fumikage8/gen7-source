//======================================================================
/**
 * @file Debug_DrawUtil.cpp
 * @date 2017/02/28 12:09:32
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include <gfx/include/gfl2_DrawUtil.h>
#include "System/include/GflUse.h"
#include "./DrawUtil.h"
#include "../Assert.h"
#include "../Camera/Base.h"
#include "../Gimmick/Collision/Cylinder.h"
#include "../Gimmick/Collision/Hemisphere.h"
#include "../Rail/Directions.h"
#include "../Rail/Section.h"

namespace LunaSolDive{
namespace Debug{
// parameter
static const u32 CIRCLE_PARTITION_NUMBER = 8;
// static function
// Triangle
void DrawTriangle(
            const gfl2::math::Vector3& target,
            const f32& size,
            const Rail::Directions& cameraDirections,
            const gfl2::math::Vector4& color);
// Circle
void DrawCircle(
            const gfl2::math::Vector3& center,
            const gfl2::math::Vector3& axis,
            const gfl2::math::Vector3& polarAxis,
            const f32& radius,
            const gfl2::math::Vector4& color);
// Tube
void DrawTube(
            const gfl2::math::Vector3& center,
            const gfl2::math::Vector3& axis,
            const gfl2::math::Vector3& polarAxis,
            const f32& radius,
            const f32& height,
            const gfl2::math::Vector4& color);
// Rail::Section
void DrawRailSection(
            const Rail::Section& target,
            const gfl2::math::Vector4& beginColor,
            const gfl2::math::Vector4& endColor);
// Gimmick::Collision::Cylinder
void DrawCylinder(
            const Gimmick::Collision::Cylinder& target,
            const gfl2::math::Vector4& color);
// Gimmick::Collision::Hemispere
void DrawHemisphere(
            const Gimmick::Collision::Hemisphere& target,
            const gfl2::math::Vector4& color);
// constructor
DrawUtil::DrawUtil()
{
  m_Setting.active = false;
  m_Setting.input_point = false;
  m_Setting.present_point = false;
  m_Setting.collision_point = false;
  m_Setting.gimmick_energy_collision = false;
  m_Setting.gimmick_obstacle_collision = false;
  m_Setting.gimmick_distortion_collision = false;
}
// destructor
DrawUtil::~DrawUtil()
{
  Terminate();
}
// Initialize
void DrawUtil::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  m_Data.Initialize(pHeap);
}
// Terminate
void DrawUtil::Terminate()
{
  m_Setting.active = false;
  m_Setting.input_point = false;
  m_Setting.present_point = false;
  m_Setting.collision_point = false;
  m_Setting.gimmick_energy_collision = false;
  m_Setting.gimmick_obstacle_collision = false;
  m_Setting.gimmick_distortion_collision = false;
  m_Data.Terminate();
}
// Draw
void DrawUtil::Draw(
            const gfl2::gfx::CtrDisplayNo& displayNo,
            const Camera::Base& camera)
{
  if(! m_Setting.active) return;
  if(displayNo != gfl2::gfx::CtrDisplayNo::LEFT) return;
  // display setting
  const gfl2::gfx::DisplayDeviceHandle& displayHandle = System::GflUse::m_HandleList[displayNo];
  const u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  const u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);
  // begin scene
  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::GFGL::SetRenderTarget(
              gfl2::gfx::GFGL::GetBackBuffer(displayHandle));
  gfl2::gfx::GFGL::SetViewport(
              0,
              0,
              displayWidth,
              displayHeight);
  gfl2::gfx::GFGL::SetScissor(
              0,
              0,
              displayWidth,
              displayHeight);
  // set projection matrix
  gfl2::gfx::DrawUtil::SetProjectionMatrix(
              camera.ProjectionMatrix(displayNo));
  // set view matrix
  gfl2::gfx::DrawUtil::SetViewMatrix(
              camera.ViewMatrix());
  // drawing
  gfl2::gfx::DrawUtil::SetModelMatrix(
              gfl2::math::Matrix44::GetIdentity());
  gfl2::gfx::DrawUtil::SetMaterialAlpha(0.3f);
  // draw implement
  DrawImpl(camera);
  // end scene
  gfl2::gfx::GFGL::EndScene();
}
// Draw Implement
void DrawUtil::DrawImpl(
            const Camera::Base& camera)
{
  // color
  const gfl2::math::Vector4 red = gfl2::math::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  const gfl2::math::Vector4 green = gfl2::math::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  const gfl2::math::Vector4 blue = gfl2::math::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  // input point
  if(m_Setting.input_point)
  {
    DrawTriangle(
                Data().InputPoint(),
                20.0f,
                camera.Directions(),
                red);
  }
  // present point
  if(m_Setting.present_point)
  {
    DrawTriangle(
                Data().PresentLocation().Point(),
                20.0f,
                camera.Directions(),
                green);
  }
  // collision point
  if(m_Setting.collision_point)
  {
    DrawTriangle(
                Data().CollisionPoint(),
                20.0f,
                camera.Directions(),
                blue);
  }
  // Rail::Seciton
  if(m_Setting.rail_section)
  {
    for(u32 i = 0; i < Data().RailSectionListSize(); ++i)
    {
      DrawRailSection(
                  Data().RailSection(i),
                  green,
                  blue);
    }
  }
  // Gimmick::Energy Collision
  if(m_Setting.gimmick_energy_collision)
  {
    for(u32 i = 0; i < Data().GimmickEnergyCollisionListSize(); ++i)
    {
      DrawCylinder(
                  Data().GimmickEnergyCollision(i),
                  red);
    }
  }
  // Gimmick::Obstacle Collision
  if(m_Setting.gimmick_obstacle_collision)
  {
    for(u32 i = 0; i < Data().GimmickObstacleCollisionListSize(); ++i)
    {
      DrawCylinder(
                  Data().GimmickObstacleCollision(i),
                  blue);
    }
  }
  // Gimmick::Distortion Collision
  if(m_Setting.gimmick_distortion_collision)
  {
    for(u32 i = 0; i < Data().GimmickDistortionCollisionListSize(); ++i)
    {
      DrawHemisphere(
                  Data().GimmickDistortionCollision(i),
                  green);
    }
  }
}
// Data
DrawUtilData& DrawUtil::Data()
{
  return m_Data;
}
// static function
// Traiangle
void DrawTriangle(
            const gfl2::math::Vector3& target,
            const f32& size,
            const Rail::Directions& cameraDirections,
            const gfl2::math::Vector4& color)
{
  const u32 VERTEX_NUMBER = 3;
  gfl2::gfx::DrawUtil::VertexFormat vertexFormatList[VERTEX_NUMBER];
  for(u32 i = 0; i < VERTEX_NUMBER; ++i)
  {
    const gfl2::math::Quaternion rotation(
                cameraDirections.Forward(),
                PI2 * i / VERTEX_NUMBER);
    vertexFormatList[i].pos =
                target
                + rotation.Transform(cameraDirections.Vertical()) * size;
    vertexFormatList[i].color = color;
  }
  gfl2::gfx::DrawUtil::BeginRender();
  gfl2::gfx::DrawUtil::DrawTriangles(
              vertexFormatList,
              VERTEX_NUMBER,
              false);
  gfl2::gfx::DrawUtil::EndRender();
}
// Circle
void DrawCircle(
            const gfl2::math::Vector3& center,
            const gfl2::math::Vector3& axis,
            const gfl2::math::Vector3& polarAxis,
            const f32& radius,
            const gfl2::math::Vector4& color)
{
  // plot
  gfl2::math::Vector3 pointList[CIRCLE_PARTITION_NUMBER + 1];
  for(u32 i = 0; i <= CIRCLE_PARTITION_NUMBER; ++i)
  {
    const gfl2::math::Quaternion rotation(
                axis,
                PI2 * i / CIRCLE_PARTITION_NUMBER);
    pointList[i] = center + rotation.Transform(polarAxis) * radius;
  }
  // draw
  const u32 vertexFormatListSize = 3 * CIRCLE_PARTITION_NUMBER;
  gfl2::gfx::DrawUtil::VertexFormat vertexFormatList[vertexFormatListSize];
  for(u32 i = 0; i < vertexFormatListSize; ++i)
  {
    vertexFormatList[i].color = color;
  }
  u32 index = 0;
  for(u32 i = 0; i < CIRCLE_PARTITION_NUMBER; ++i)
  {
    vertexFormatList[index++].pos = pointList[i];
    vertexFormatList[index++].pos = pointList[i + 1];
    vertexFormatList[index++].pos = center;
  }
  LUNA_SOL_DIVE_ASSERT(index == vertexFormatListSize);
  gfl2::gfx::DrawUtil::BeginRender();
  gfl2::gfx::DrawUtil::DrawTriangles(
              vertexFormatList,
              vertexFormatListSize,
              false);
  gfl2::gfx::DrawUtil::EndRender();
}
// Tube
void DrawTube(
            const gfl2::math::Vector3& center,
            const gfl2::math::Vector3& axis,
            const gfl2::math::Vector3& polarAxis,
            const f32& radius,
            const f32& height,
            const gfl2::math::Vector4& color)
{
  // plot
  gfl2::math::Vector3 beginPointList[CIRCLE_PARTITION_NUMBER + 1];
  gfl2::math::Vector3 endPointList[CIRCLE_PARTITION_NUMBER + 1];
  for(u32 i = 0; i <= CIRCLE_PARTITION_NUMBER; ++i)
  {
    const gfl2::math::Quaternion rotation(
                axis,
                PI2 * i / CIRCLE_PARTITION_NUMBER);
    beginPointList[i] = center + rotation.Transform(polarAxis) * radius;
    endPointList[i] = beginPointList[i] + axis * height;
  }
  // draw
  const u32 vertexFormatListSize = 6 * CIRCLE_PARTITION_NUMBER;
  gfl2::gfx::DrawUtil::VertexFormat vertexFormatList[vertexFormatListSize];
  for(u32 i = 0; i < vertexFormatListSize; ++i)
  {
    vertexFormatList[i].color = color;
  }
  u32 index = 0;
  for(u32 i = 0; i < CIRCLE_PARTITION_NUMBER; ++i)
  {
    vertexFormatList[index++].pos = beginPointList[i];
    vertexFormatList[index++].pos = beginPointList[i + 1];
    vertexFormatList[index++].pos = endPointList[i];
    vertexFormatList[index++].pos = endPointList[i];
    vertexFormatList[index++].pos = endPointList[i + 1];
    vertexFormatList[index++].pos = beginPointList[i + 1];
  }
  LUNA_SOL_DIVE_ASSERT(index == vertexFormatListSize);
  gfl2::gfx::DrawUtil::BeginRender();
  gfl2::gfx::DrawUtil::DrawTriangles(
              vertexFormatList,
              vertexFormatListSize,
              false);
  gfl2::gfx::DrawUtil::EndRender();
}
// Rail::Section
void DrawRailSection(
            const Rail::Section& target,
            const gfl2::math::Vector4& beginColor,
            const gfl2::math::Vector4& endColor)
{
  if(! target.IsValid()) return;
  gfl2::gfx::DrawUtil::VertexFormat vertexFormatList[6 * CIRCLE_PARTITION_NUMBER];
  for(u32 i = 0; i < CIRCLE_PARTITION_NUMBER; ++i)
  {
    vertexFormatList[6 * i + 0].pos =
            target.BeginCylindricSection(1.0f, PI2 * i / CIRCLE_PARTITION_NUMBER);
    vertexFormatList[6 * i + 0].color = beginColor;
    vertexFormatList[6 * i + 1].pos =
            target.BeginCylindricSection(1.0f, PI2 * (i + 1) / CIRCLE_PARTITION_NUMBER);
    vertexFormatList[6 * i + 1].color = beginColor;
    vertexFormatList[6 * i + 2].pos =
            target.EndCylindricSection(1.0f, PI2 * i / CIRCLE_PARTITION_NUMBER);
    vertexFormatList[6 * i + 2].color = endColor;
    vertexFormatList[6 * i + 3].pos =
            target.EndCylindricSection(1.0f, PI2 * i / CIRCLE_PARTITION_NUMBER);
    vertexFormatList[6 * i + 3].color = endColor;
    vertexFormatList[6 * i + 4].pos =
            target.EndCylindricSection(1.0f, PI2 * (i + 1) / CIRCLE_PARTITION_NUMBER);
    vertexFormatList[6 * i + 4].color = endColor;
    vertexFormatList[6 * i + 5].pos =
            target.BeginCylindricSection(1.0f, PI2 * (i + 1) / CIRCLE_PARTITION_NUMBER);
    vertexFormatList[6 * i + 5].color = beginColor;
  }
  gfl2::gfx::DrawUtil::BeginRender();
  gfl2::gfx::DrawUtil::DrawTriangles(
              vertexFormatList,
              6 * CIRCLE_PARTITION_NUMBER,
              false);
  gfl2::gfx::DrawUtil::EndRender();
}
// Gimmick::Collision::Cylinder
void DrawCylinder(
            const Gimmick::Collision::Cylinder& target,
            const gfl2::math::Vector4& color)
{
  // bottom
  DrawCircle(
              target.Location(),
              target.Axis(),
              target.PolarAxis(),
              target.Radius(),
              color);
  // side
  DrawTube(
              target.Location(),
              target.Axis(),
              target.PolarAxis(),
              target.Radius(),
              target.Height(),
              color);
  // top
  DrawCircle(
              target.Location() + target.Axis() * target.Height(),
              target.Axis(),
              target.PolarAxis(),
              target.Radius(),
              color);
}
// Gimmick::Collision::Hemispere
void DrawHemisphere(
            const Gimmick::Collision::Hemisphere& target,
            const gfl2::math::Vector4& color)
{
  // bottom
  DrawCircle(
              target.Location(),
              target.Axis(),
              target.PolarAxis(),
              target.Radius(),
              color);
  // plot
  const u32 stageNumber = 3;
  LUNA_SOL_DIVE_ASSERT(stageNumber != 0);
  gfl2::math::Vector3 pointList[stageNumber][CIRCLE_PARTITION_NUMBER + 1];
  for(u32 i = 0; i < stageNumber; ++i)
  {
    const gfl2::math::Quaternion polarRotationQuaternion(
                target.PolarAxis().Cross(target.Axis()),
                (PI * i) / (2.0f * (stageNumber + 1)));
    const f32 azimuthAngleOffset =
            PI2 * i / (2.0f * CIRCLE_PARTITION_NUMBER);
    for(u32 k = 0; k <= CIRCLE_PARTITION_NUMBER; ++k)
    {
      const f32 azimuthAngle =
              PI2 * k / CIRCLE_PARTITION_NUMBER
              + azimuthAngleOffset;
      const gfl2::math::Quaternion azimuthRotationQuaternion(
                  target.Axis(),
                  azimuthAngle);
      pointList[i][k] =
              target.Location()
              + azimuthRotationQuaternion.Transform(
                          polarRotationQuaternion.Transform(target.PolarAxis()))
                * target.Radius();
    }
  }
  // vertex format
  const u32 vertexFormatListSize = 3 * (2 * (stageNumber - 1) + 1) * CIRCLE_PARTITION_NUMBER;
  gfl2::gfx::DrawUtil::VertexFormat vertexFormatList[vertexFormatListSize];
  for(u32 i = 0; i < vertexFormatListSize; ++i)
  {
    vertexFormatList[i].color = color;
  }
  u32 index = 0;
  for(u32 i = 0; i < stageNumber - 1; ++i)
  {
    for(u32 k = 0; k < CIRCLE_PARTITION_NUMBER; ++k)
    {
      vertexFormatList[index++].pos = pointList[i][k];
      vertexFormatList[index++].pos = pointList[i][k + 1];
      vertexFormatList[index++].pos = pointList[i + 1][k];
      vertexFormatList[index++].pos = pointList[i + 1][k];
      vertexFormatList[index++].pos = pointList[i + 1][k + 1];
      vertexFormatList[index++].pos = pointList[i][k + 1];
    }
  }
  for(u32 i = 0; i < CIRCLE_PARTITION_NUMBER; ++i)
  {
    vertexFormatList[index++].pos = pointList[stageNumber - 1][i];
    vertexFormatList[index++].pos = pointList[stageNumber - 1][i + 1];
    vertexFormatList[index++].pos = target.Location() + target.Axis() * target.Radius();
  }
  LUNA_SOL_DIVE_ASSERT(index == vertexFormatListSize);
  // draw
  gfl2::gfx::DrawUtil::BeginRender();
  gfl2::gfx::DrawUtil::DrawTriangles(
              vertexFormatList,
              vertexFormatListSize,
              false);
  gfl2::gfx::DrawUtil::EndRender();
}
}// namespace Debug
}// namespace LunaSolDive
#endif// PM_DEBUG
