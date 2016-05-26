#include "Util.h"

// print info for the given profile
void Util::printProfile(const Ptr<Profile> &profile)
{
    Ptr<Point3D> maxPoint = profile->boundingBox()->maxPoint();
    Ptr<Point3D> minPoint = profile->boundingBox()->minPoint();
    XTRACE(L"profile : (%p)\n", profile.get());
    XTRACE(L"--------------------\n");
    XTRACE(L"max point : (%lf, %lf, %lf)\n", maxPoint->x(), maxPoint->y(), maxPoint->z());
    XTRACE(L"min point : (%lf, %lf, %lf)\n", minPoint->x(), minPoint->y(), minPoint->z());
}

// print info for all profiles in the given sketch
void Util::printProfiles(const Ptr<Sketch>& sketch)
{
    Ptr<Profiles> profiles = sketch->profiles();
    Ptr<Profile> profile;

    for (unsigned int i = 0; i < profiles->count(); i++)
    {
        profile = profiles->item(i);
        printProfile(profile);
    }
}

// check the test point to see if it's on the face
// https://forums.autodesk.com/t5/api-and-scripts/checking-whether-a-2d-point-lies-within-a-flat-surface-profile/m-p/6304081
bool Util::isPointOnSurface(const Ptr<Point3D>& testPoint, const Ptr<BRepFace>& plane)
{
    bool isOk = false;
    Ptr<Point3D> planePoint;

    Ptr<SurfaceEvaluator> surfaceEval = plane->evaluator();
    Ptr<Point2D> param;

    isOk = surfaceEval->getParameterAtPoint(testPoint, param);
    if (!isOk) return false;

    isOk = surfaceEval->getPointAtParameter(param, planePoint);
    if (!isOk) return false;

    XTRACE(L"test point : (%lf, %lf, %lf)\n", testPoint->x(), testPoint->y(), testPoint->z());
    XTRACE(L"plane point : (%lf, %lf, %lf)\n", planePoint->x(), planePoint->y(), planePoint->z());

    if (testPoint->isEqualTo(planePoint))
    {
        if (surfaceEval->isParameterOnFace(param))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


Ptr<Vector3D> Util::findScaleVector(const Ptr<SketchLine>& line, double length)
{
    Ptr<Point3D> sp = line->startSketchPoint()->geometry();
    Ptr<Point3D> ep = line->endSketchPoint()->geometry();

    Ptr<Vector3D> v1 = sp->vectorTo(ep);
    v1->normalize();
    v1->scaleBy(length);

    return v1;
}

Ptr<ObjectCollection> Util::filterMatchingFeatures(const Ptr<Component>& comp, const Ptr<BoundingBox3D>& box)
{
    Ptr<ObjectCollection> featureCollection = ObjectCollection::create();
    Ptr<Features> features = comp->features();
    if (!features)
        return featureCollection;

    Ptr<ExtrudeFeatures> extrudeFeatures = features->extrudeFeatures();
    if (!extrudeFeatures)
        return featureCollection;

    for (unsigned int i = 0; i < extrudeFeatures->count(); i++)
    {
        bool featureMatches = true;
        Ptr<ExtrudeFeature> feature = extrudeFeatures->item(i);
        Ptr<TimelineObject> timeObject = feature->timelineObject();
        Ptr<BRepFaces> faces = feature->faces();
        XTRACE(L"feature %i (%i faces): (%p)\ntimeline index: %i\n", i, faces->count(), feature.get(), timeObject->index());

        if (faces->count() == 0)
        {
            continue;
        }

        for (unsigned int j = 0; (j < faces->count()) && featureMatches; j++)
        {
            Ptr<BRepFace> face = faces->item(j);
            XTRACE(L"\tface %i: (%p)\n", j, face.get());

            if (!face)
            {
                featureMatches = false;
                break;
            }

#if 0
            Ptr<SketchLine> line;
            line = m_gapSketch->sketchCurves()->sketchLines()->addByTwoPoints(
                m_gapSketch->modelToSketchSpace(maxP),
                m_gapSketch->modelToSketchSpace(minP)
                );
            line->isConstruction(true);
#endif

            if (!face->boundingBox()->intersects(box))
            {
                featureMatches = false;
            }
        }

        if (featureMatches)
        {
            XTRACE(L"feature %i : intersects with gap box!\n", i);
            featureCollection->add(feature);
        }
    }

    return featureCollection;
}

// compare the bounding boxes of two profiles and see if they are equal
bool Util::profileBoxesEqual(const Ptr<Profile> &p1, const Ptr<Profile> &p2)
{
    Ptr<Point3D> p1MinPoint = p1->boundingBox()->minPoint();
    Ptr<Point3D> p1MaxPoint = p1->boundingBox()->maxPoint();

    Ptr<Point3D> p2MinPoint = p2->boundingBox()->minPoint();
    Ptr<Point3D> p2MaxPoint = p2->boundingBox()->maxPoint();

    bool retVal = p1MinPoint->isEqualTo(p2MinPoint) && p1MaxPoint->isEqualTo(p2MaxPoint);
    return retVal;
}

// compare each profile in a sketch and try to match against
// the provided border profile
Ptr<ObjectCollection> Util::stripBorderProfile(const Ptr<Sketch>& sketch, const Ptr<Profile> &borderProfile)
{
    Ptr<ObjectCollection> profileCollection = ObjectCollection::create();
    Ptr<Profiles> profiles = sketch->profiles();
    Ptr<Profile> profile;

    for (unsigned int i = 0; i < profiles->count(); i++)
    {
        profile = profiles->item(i);
        printProfile(profile);

        if (!profileBoxesEqual(profile, borderProfile))
        {
            profileCollection->add(profile);
        }
    }

    return profileCollection;
}
