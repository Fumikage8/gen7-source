//======================================================================
/**
 * @file Rail_Part.cpp
 * @date 2016/10/11 19:52:39
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include "./Directions.h"
#include "./Part.h"
#include "./PartStorage.h"
#include "./Section.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Rail{
// static function
// Connection Matrix
static gfl2::math::Matrix34 ConnectionMatrix(
            const Directions& from,
            const Directions& to);
// constructor
Part::Part()
      : m_pStorage(NULL),
        m_Shape(PART_SHAPE__SIZE),
        m_PointNumber(0),
        m_Radius(0.0f),
        m_pRenderingPipeline(NULL),
        m_pModel(NULL),
        m_IsVisible(false)
{
  for(u32 i = 0; i < POINT_NUMBER_MAX; ++i)
  {
    m_PointList[i] = gfl2::math::Vector3::GetZero();
    m_CumulativeLength[i] = 0;
  }
  for(u32 i = 0; i < POINT_NUMBER_MAX - 1; ++i)
  {
    m_VerticalDirectionList[i] = gfl2::math::Vector3::GetZero();
  }
}
// Initialize
void Part::Initialize(
            PartStorage *pStorage,
            RenderingPipeline* pRenderingPipeline)
{
  LUNA_SOL_DIVE_ASSERT(m_pStorage == NULL);
  m_pStorage = pStorage;
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_Shape = m_pStorage->Shape();
  m_PointNumber = m_pStorage->PointNumber();
  LUNA_SOL_DIVE_ASSERT(m_PointNumber <= POINT_NUMBER_MAX);
  for(u32 i = 0; i < m_pStorage->PointNumber(); ++i)
  {
    m_PointList[i] = m_pStorage->Point(i);
  }
  m_Radius = m_pStorage->Radius();
  m_VerticalDirectionList[0] = m_pStorage->BeginVerticalDirection();
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
}
// Set Vertical Direction
void Part::SetVerticalDirection(
            const gfl2::math::Vector3& beginVerticalDirection)
{
  gfl2::math::Vector3 verticalDirection = beginVerticalDirection;
  for(u32 i = 0; i < m_PointNumber - 1; ++i)
  {
    const gfl2::math::Vector3 displacement = Point(i + 1) - Point(i);
    LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(displacement.Length()));
    const gfl2::math::Vector3 direction  = displacement.Normalize();
    const gfl2::math::Vector3 vector =
                verticalDirection - direction * direction.Dot(verticalDirection);
    LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(vector.Length()));
    verticalDirection = vector.Normalize();
    LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(verticalDirection.Length(), 1.0f));
    LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(verticalDirection.Dot(direction)));
    m_VerticalDirectionList[i] = verticalDirection;
  }
}
// Set Cumulative Length
void Part::SetCumulativeLength()
{
  f32 value = 0.0f;
  m_CumulativeLength[0] = value;
  for(u32 i = 0; i < m_PointNumber - 1; ++i)
  {
    value += (Point(i + 1) - Point(i)).Length();
    m_CumulativeLength[i + 1] = value;
  }
}
// Terminate
void Part::Terminate()
{
  // non-visualize Model
  if(IsVisible())
  {
    NonVisualize();
  }
  // unset Model
  if(m_pModel != NULL)
  {
    UnsetModel();
  }
  // reset parameter
  m_Shape = PART_SHAPE__SIZE;
  m_PointNumber = 0;
  for(u32 i = 0; i < POINT_NUMBER_MAX; ++i)
  {
    m_PointList[i] = gfl2::math::Vector3::GetZero();
    m_CumulativeLength[i] = 0;
  }
  for(u32 i = 0; i < POINT_NUMBER_MAX - 1; ++i)
  {
    m_VerticalDirectionList[i] = gfl2::math::Vector3::GetZero();
  }
  m_Radius = 0;
  m_RotationMatrix = gfl2::math::Matrix34();
  m_IsVisible = false;
  // unset pointer
  m_pStorage = NULL;
  m_pRenderingPipeline = NULL;
}
// check if the Part is valid
bool Part::IsValid() const
{
  return m_Shape != PART_SHAPE__SIZE
          && ! gfl2::math::IsAlmostZero(EndVerticalDirection().Length());
}
// Update
void Part::Update()
{
  // update animation
  if(IsVisible())
  {
    m_pModel->UpdateAnimation();
  }
}
// Shape
const PartShape& Part::Shape() const
{
  return m_Shape;
}
// Point Number
const u32& Part::PointNumber() const
{
  return m_PointNumber;
}
// Point
const gfl2::math::Vector3& Part::Point(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber);
  return m_PointList[index];
}
// Tangent Vector
gfl2::math::Vector3 Part::TangentVector(
            const u32& index) const
{
  const gfl2::math::Vector3 vector =
              (index == 0)
              ? Point(index + 1) - Point(index)
              : (index == m_PointNumber - 1)
              ? Point(index) - Point(index - 1)
              : Point(index + 1) - Point(index - 1);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(vector.Length()));
  return vector.Normalize();
}
// Normal Vector
gfl2::math::Vector3 Part::NormalVector(
            const u32& index) const
{
  if(index == 0 || index == m_PointNumber - 1)
  {// Begin Point or End Point
    return gfl2::math::Vector3::GetZero();
  }
  else
  {
    const gfl2::math::Vector3 vector =
            TangentVector(index + 1) - TangentVector(index - 1);
    return ! gfl2::math::IsAlmostZero(vector.Length())
                ? vector.Normalize()
                : gfl2::math::Vector3::GetZero();
  }
}
// Direction
gfl2::math::Vector3 Part::Direction(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber - 1);
  const gfl2::math::Vector3 displacement = Point(index + 1) - Point(index);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(displacement.Length()));
  return displacement.Normalize();
}
// Vertical Direction
const gfl2::math::Vector3& Part::VerticalDirection(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber - 1);
  const gfl2::math::Vector3& vector = m_VerticalDirectionList[index];
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(vector.Length(), 1.0f));
  return vector;
}
// Horizontal Direction
gfl2::math::Vector3 Part::HorizontalDirection(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber - 1);
  const gfl2::math::Vector3 vector = Direction(index).Cross(VerticalDirection(index));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(vector.Length(), 1.0f));
  return vector;
}
// Radius
const f32& Part::Radius() const
{
  return m_Radius;
}
// Begin Point
const gfl2::math::Vector3& Part::BeginPoint() const
{
  return Point(0);
}
// End Point
const gfl2::math::Vector3& Part::EndPoint() const
{
  return Point(m_PointNumber - 1);
}
// Begin Direction
gfl2::math::Vector3 Part::BeginDirection() const
{
  return TangentVector(0);
}
// End Direction
gfl2::math::Vector3 Part::EndDirection() const
{
  return TangentVector(m_PointNumber -1);
}
// Begin Vertical Direction
const gfl2::math::Vector3& Part::BeginVerticalDirection() const
{
  return VerticalDirection(0);
}
// End Vertical Direction
const gfl2::math::Vector3& Part::EndVerticalDirection() const
{
  return VerticalDirection(m_PointNumber - 2);
}
// Begin Horizontal Direction
gfl2::math::Vector3 Part::BeginHorizontalDirection() const
{
  return HorizontalDirection(0);
}
// End Horizontal Direction
gfl2::math::Vector3 Part::EndHorizontalDirection() const
{
  return HorizontalDirection(m_PointNumber - 2);
}
// Begin Directions
Directions Part::BeginDirections() const
{
  return Directions(
              Point(0),
              Direction(0),
              VerticalDirection(0));
}
// End Directions
Directions Part::EndDirections() const
{
  return Directions(
              Point(m_PointNumber - 1),
              Direction(m_PointNumber - 2),
              VerticalDirection(m_PointNumber - 2));
}
// Intermediate Directions
Directions Part::IntermediateDirections(
            const f32& travelDistance) const
{
  const u32 sectionIndex = SectionIndex(travelDistance);
  if(sectionIndex == 0xffffffff)
  {
    return Directions();
  }
  const gfl2::math::Vector3 Location =
              Point(sectionIndex)
              + Direction(sectionIndex) * (travelDistance - CumulativeLength(sectionIndex));
  return Directions(
              Location,
              Direction(sectionIndex),
              VerticalDirection(sectionIndex));
}
// Cumulative Length
const f32& Part::CumulativeLength(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber);
  return m_CumulativeLength[index];
}
// Total Length
const f32& Part::TotalLength() const
{
  return CumulativeLength(m_PointNumber - 1);
}
// Section Index (Invalid -> 0xffffffff)
u32 Part::SectionIndex(
            const f32& travelDistance) const
{
  for(u32 i = 0; i < m_PointNumber - 1; ++i)
  {
    if(m_CumulativeLength[i] <= travelDistance
                && travelDistance <= m_CumulativeLength[i + 1])
    {
      return i;
    }
  }
  return 0xffffffff;
}
// Create Section
Section Part::CreateSection(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber -1);
  return Section(
              index,
              Point(index),
              NormalVector(index),
              Point(index + 1),
              NormalVector(index + 1),
              m_Radius,
              VerticalDirection(index));
}
// Translation
void Part::Translation(
            const gfl2::math::Vector3& displacement)
{
  LUNA_SOL_DIVE_ASSERT(m_PointList != NULL);
  for(u32 i = 0; i < m_PointNumber; ++i)
  {
    m_PointList[i] += displacement;
  }
  if(m_pModel != NULL)
  {
    m_pModel->SetPosition(BeginPoint());
  }
}
// Rotation
void Part::Rotation(
            const f32& angle)
{
  const gfl2::math::Quaternion rotation(
              BeginDirection(),
              - angle);
  m_VerticalDirectionList[0] = rotation.Transform(BeginVerticalDirection());
}
// Connect To
void Part::ConnectTo(
            const Directions& directions)
{
  LUNA_SOL_DIVE_ASSERT(m_PointList != NULL);
  m_RotationMatrix = ConnectionMatrix(
              directions,
              BeginDirections());
  // convert Geometric Parameter
  const gfl2::math::Vector3 zeroPoint = BeginPoint();
  for(u32 i = 0; i < m_PointNumber; ++i)
  {
    m_PointList[i] =
            m_RotationMatrix * (m_PointList[i] - zeroPoint)
            + zeroPoint + directions.Point();
  }
  SetVerticalDirection(directions.Vertical());
  SetCumulativeLength();
  // check
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero((directions.Point() - BeginPoint()).Length()));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(
              (directions.Forward() - BeginDirection()).Length(),
             1.0e-4f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(
              (directions.Vertical() - BeginVerticalDirection()).Length(),
             1.0e-4f));
}
// Connect To (with Part ver)
void Part::ConnectTo(
            const Part& previousPart)
{
  ConnectTo(previousPart.EndDirections());
}
// Set Model
void Part::SetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel == NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  poke_3d::model::BaseModel* const pModel = m_pStorage->GetModelPtr();
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  m_pModel = pModel;
}
// Unset Model
void Part::UnsetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_pStorage->ReleaseModelPtr(m_pModel);
  m_pModel = NULL;
}
// Whether this Part is visible or not
bool Part::IsVisible() const
{
  return m_IsVisible;
}
// Visualize the Model
void Part::Visualize()
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  if(m_pModel != NULL)
  {
    LUNA_SOL_DIVE_ASSERT(IsValid());
    // translation & rotation
    m_pModel->SetPosition(BeginPoint());
    gfl2::math::Quaternion rotationQuaternon;
    rotationQuaternon.MatrixToQuaternion(m_RotationMatrix);
    m_pModel->SetRotationQuat(rotationQuaternon);
    // visualize
    m_IsVisible = true;
#if PM_DEBUG
    if(! Debug::Parameter::Get().modelVisualizeRail) return;
#endif// PM_DEBUG
    m_pRenderingPipeline->AddObject(
                m_pModel->GetModelInstanceNode(),
                true);
  }
}
// Non-Visualize the Model
void Part::NonVisualize()
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  if(m_pModel != NULL)
  {
    m_IsVisible = false;
    m_pRenderingPipeline->RemoveObject(
                m_pModel->GetModelInstanceNode());
  }
}
// Adjusted Direction
Directions AdjustedDirections(
            const Part& part,
            const f32& travelDistance,
            const f32& radiusRate,
            const f32& angle)
{
  LUNA_SOL_DIVE_ASSERT(part.IsValid());
  const u32 sectionIndex = part.SectionIndex(travelDistance);
  // check if this section index is Valid
  if(sectionIndex == 0xffffffff)
  {
    return Directions();
  }
  const Section section = part.CreateSection(sectionIndex);
  // calculate Internal Coordinates
  const f32 z = travelDistance - part.CumulativeLength(sectionIndex);
  LUNA_SOL_DIVE_ASSERT(z > -EPSILON);
  const InternalCoordinates point = AdjustedLocation(
              section,
              z,
              radiusRate,
              angle);
  // Next & Previous Section Directions
  const Directions nextSectionDirections =
          (sectionIndex == part.PointNumber() - 2)
          ? part.EndDirections()
          : Directions(
                    part.Point(sectionIndex + 1),
                    part.Direction(sectionIndex + 1),
                    part.VerticalDirection(sectionIndex + 1));
  const Directions previousSectionDirections =
          (sectionIndex == 0)
          ? part.BeginDirections()
          : Directions(
                      part.Point(sectionIndex),
                      part.Direction(sectionIndex - 1),
                      part.VerticalDirection(sectionIndex - 1));
  return AdjustedDirections(
              section,
              point,
              nextSectionDirections,
              previousSectionDirections);
}
// static function
// Connection Matrix
static gfl2::math::Matrix34 ConnectionMatrix(
            const Directions& from,
            const Directions& to)
{
  LUNA_SOL_DIVE_ASSERT(from.IsValid());
  LUNA_SOL_DIVE_ASSERT(to.IsValid());
  const gfl2::math::Matrix34 fromMatrix = gfl2::math::Matrix34(
              from.Horizontal(),
              from.Vertical(),
              from.Forward())
              .SetTranslationVector(gfl2::math::Vector3());
  const gfl2::math::Matrix34 toMatrix = gfl2::math::Matrix34(
              to.Horizontal(),
              to.Vertical(),
              to.Forward())
              .SetTranslationVector(gfl2::math::Vector3());
  return fromMatrix.Transpose() * toMatrix;
}
}// namespace Rail
}// namespace LunaSolDive
