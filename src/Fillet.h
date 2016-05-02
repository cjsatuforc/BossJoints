#pragma once

#include <Core/Utils.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>

#include <Fusion/Fusion/ModelParameter.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchPoints.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>

#include <Fusion/Sketch/SketchDimensions.h>
#include <Fusion/Sketch/SketchDimension.h>
#include <Fusion/Sketch/GeometricConstraints.h>
#include <Fusion/Sketch/PerpendicularConstraint.h>
#include <Fusion/Sketch/SketchAngularDimension.h>
#include <Fusion/Sketch/SketchLinearDimension.h>
#include <Fusion/Sketch/CoincidentConstraint.h>

#include "FilletBase.h"

using namespace adsk::core;
using namespace adsk::fusion;

class CornerFillet
{
private:
    FilletBase m_filletBase;
    Ptr<Point3D> m_refPoint;
    Ptr<Vector3D> m_thicknessVector;
    Ptr<Vector3D> m_widthVector;

    Ptr<Vector3D> m_halfWidthVector;
    Ptr<Vector3D> m_tinyWidthVector;
    Ptr<Vector3D> m_quarterWidthPerpVector;

    void updateSubVectors();

public:
    CornerFillet(
        const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& widthVector,
        const Ptr<Vector3D>& thicknessVector, const double toolDiameter
    );
    CornerFillet();
    ~CornerFillet();

    void drawSketch(const Ptr<Sketch>& sketch);
};
