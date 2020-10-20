//======================================================================
/**
 * @file Rail_TravelDistanceCounter.cpp
 * @date 2016/11/04 19:30:58
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./TravelDistanceCounter.h"

namespace LunaSolDive{
namespace Rail{
// constructor
TravelDistanceCounter::TravelDistanceCounter()
    : m_Offset(0.0f),
      m_FromInitialPartToPresentPart(0.0f),
      m_FromPresentPartToPresentSection(0.0f),
      m_FromPresentSectionToPresentPoint(0.0f){}
// Initialize
void TravelDistanceCounter::Initialize(
            const f32& offset)
{
  m_Offset = offset;
  m_FromInitialPartToPresentPart = 0.0f;
  m_FromPresentPartToPresentSection = 0.0f;
  m_FromPresentSectionToPresentPoint = 0.0f;
}
// Terminate
void TravelDistanceCounter::Terminate()
{
  m_Offset = 0.0f;
  m_FromInitialPartToPresentPart = 0.0f;
  m_FromPresentPartToPresentSection = 0.0f;
  m_FromPresentSectionToPresentPoint = 0.0f;
}
// Travel Distance: Total
f32 TravelDistanceCounter::Total() const
{
  return m_FromInitialPartToPresentPart
              + m_FromPresentPartToPresentSection
              + m_FromPresentSectionToPresentPoint
              + m_Offset;
}
// Travel Distance: Present Part
f32 TravelDistanceCounter::PresentPart() const
{
  return m_FromPresentPartToPresentSection
              + m_FromPresentSectionToPresentPoint;
}
// Add Part Length
void TravelDistanceCounter::AddPartLength(
            const f32& partLength)
{
  m_FromInitialPartToPresentPart += partLength;
  m_FromPresentPartToPresentSection = 0.0f;
  m_FromPresentSectionToPresentPoint = 0.0f;
}
// Add Section Length
void TravelDistanceCounter::AddSectionLength(
            const f32& sectionLength)
{
  m_FromPresentPartToPresentSection += sectionLength;
  m_FromPresentSectionToPresentPoint = 0.0f;
}
// Update Present Section Length
void TravelDistanceCounter::UpdatePresentSection(
            const f32& sectionLength)
{
  m_FromPresentSectionToPresentPoint = sectionLength;
}
}// namespace Rail
}// namespace LunaSolDive
