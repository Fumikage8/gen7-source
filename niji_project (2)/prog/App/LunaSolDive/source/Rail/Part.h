//======================================================================
/**
 * @file Part.h
 * @date 2016/10/11 19:52:09
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__PART_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__PART_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Vector3.h>
#include "./PartShape.h"

// forward declaration
namespace poke_3d{
namespace model{
class BaseModel;
}// namespace model
}// namespace poke_3d

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Rail{
// forward declaration
class Directions;
class PartStorage;
class Section;

class Part
{
  GFL_FORBID_COPY_AND_ASSIGN(Part);
public:
  // the Maximum number of the size of the Point List
  enum {POINT_NUMBER_MAX = 0x20};
  // constructor
  Part();
  // Initialize
  void Initialize(
              PartStorage* pStorage,
              RenderingPipeline* pRenderingPipeline);
  // Terminate
  void Terminate();
  // check if the Part is valid
  bool IsValid() const;
  // Update
  void Update();
  // Shape
  const PartShape& Shape() const;
  // Point Number
  const u32& PointNumber() const;
  // Point
  const gfl2::math::Vector3& Point(
              const u32& index) const;
  // Tangent Vector
  gfl2::math::Vector3 TangentVector(
              const u32& index) const;
  // Normal Vector
  gfl2::math::Vector3 NormalVector(
              const u32& index) const;
  // Direction
  gfl2::math::Vector3 Direction(
              const u32& index) const;
  // Vertical Direction
  const gfl2::math::Vector3& VerticalDirection(
              const u32& index) const;
  // Horizontal Direction
  gfl2::math::Vector3 HorizontalDirection(
              const u32& index) const;
  // Radius
  const f32& Radius() const;
  // Begin Point
  const gfl2::math::Vector3& BeginPoint() const;
  // End Point
  const gfl2::math::Vector3& EndPoint() const;
  // Begin Direction
  gfl2::math::Vector3 BeginDirection() const;
  // End Direction
  gfl2::math::Vector3 EndDirection() const;
  // Begin Vertical Direction
  const gfl2::math::Vector3& BeginVerticalDirection() const;
  // End Vertical Direction
  const gfl2::math::Vector3& EndVerticalDirection() const;
  // Begin Horizontal Direction
  gfl2::math::Vector3 BeginHorizontalDirection() const;
  // End Horizontal Direction
  gfl2::math::Vector3 EndHorizontalDirection() const;
  // Begin Directions
  Directions BeginDirections() const;
  // End Directions
  Directions EndDirections() const;
  // Intermediate Directions
  Directions IntermediateDirections(
              const f32& travelDistance) const;
  // Cumulative Length
  const f32& CumulativeLength(
              const u32& index) const;
  // Total Length
  const f32& TotalLength() const;
  // Section Index (Invalid -> 0xffffffff)
  u32 SectionIndex(
              const f32& travelDistance) const;
  // Create Section
  Section CreateSection(
              const u32& index) const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
  // Rotation
  void Rotation(
              const f32& angle);
  // Connect to
  void ConnectTo(
              const Directions& directions);
  // Connect To (with Part ver)
  void ConnectTo(
              const Part& previousPart);
  // Set Model
  void SetModel();
  // Unset Model
  void UnsetModel();
  // Whether this Part is visible or not
  bool IsVisible() const;
  // Visualize the Model
  void Visualize();
  // Non-Visualize the Model
  void NonVisualize();
private:
  // Set Vertical Direction
  void SetVerticalDirection(
              const gfl2::math::Vector3& beginVerticalDirection);
  // Set Cumulative Length
  void SetCumulativeLength();
  // Storage pointer
  PartStorage* m_pStorage;
  // Shape
  PartShape m_Shape;
  // Center Point Number
  u32 m_PointNumber;
  // Center Point List
  gfl2::math::Vector3 m_PointList[POINT_NUMBER_MAX];
  // Vertical Direction
  gfl2::math::Vector3 m_VerticalDirectionList[POINT_NUMBER_MAX - 1];
  // Radius
  f32 m_Radius;
  // Cumulative Length
  f32 m_CumulativeLength[POINT_NUMBER_MAX];
  // Rotation Matrix for Connection
  gfl2::math::Matrix34 m_RotationMatrix;
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Whether this Part is visible or not
  bool m_IsVisible;
};
// Adjusted Direction
Directions AdjustedDirections(
            const Part& part,
            const f32& travelDistance,
            const f32& radiusRate,
            const f32& angle);
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__PART_H_INCLUDED__
