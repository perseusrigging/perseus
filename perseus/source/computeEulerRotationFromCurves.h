#ifndef _computeEulerRotationFromCurves_h
#define _computeEulerRotationFromCurves_h

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MEulerRotation.h>

#define EPSILON             0.001
#define ROTATE_ORDER_XYZ        0
#define ROTATE_ORDER_YZX        1
#define ROTATE_ORDER_ZXY        2
#define ROTATE_ORDER_XZY        3
#define ROTATE_ORDER_YXZ        4
#define ROTATE_ORDER_ZYX        5
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void computeEulerRotationFromCurves(MDagPath curveDagPath, MDagPath curveAimDagPath,double parameter, MPoint &position, MPoint &aimPosition, MVector &normal, MVector &tangent,MVector &aim , double3 rotation,double &paramU, MObject theCurve=MObject::kNullObj, MObject theCurveB = MObject::kNullObj);

#endif

