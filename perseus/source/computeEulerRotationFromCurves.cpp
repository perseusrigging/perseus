#include "computeEulerRotationFromCurves.h"

void computeEulerRotationFromCurves(MDagPath curveDagPath, MDagPath curveAimDagPath, double parameter, MPoint& position, MPoint& aimPosition, MVector& normal, MVector& tangent, MVector& aim, double3 rotation, double& paramU, MObject theCurve, MObject theCurveB)
{
    MFnNurbsCurve curveFn(curveDagPath);
    if (theCurve != MObject::kNullObj)
        curveFn.setObject(theCurve);
    MFnNurbsCurve curveFnB(curveAimDagPath);
    if (theCurveB != MObject::kNullObj)
        curveFnB.setObject(theCurveB);
    double length = curveFn.length();
    double endParam = curveFn.findParamFromLength(length);
    double newParam = parameter;
    MVector newAim;
    curveFnB.getPointAtParam(parameter, aimPosition, MSpace::kWorld);
    curveFn.getPointAtParam(parameter, position, MSpace::kWorld);
    newAim = (aimPosition - position).normal();
   if (parameter < 0.0)
       newParam = 0.0;
   if (parameter > endParam)
       newParam = endParam;
   tangent = curveFn.tangent(newParam, MSpace::kWorld);
   MVector crossProduct = tangent ^ newAim;
   aim = tangent ^ crossProduct;
   aim= aim.normal();
   tangent = tangent.normal();
   newAim = (aimPosition - position).normal();
   if (parameter < 0.0)
       newParam = 0.0;
   if (parameter > endParam)
       newParam = endParam;
   normal = curveFn.normal(newParam, MSpace::kWorld);
   tangent = curveFn.tangent(newParam, MSpace::kWorld);
   crossProduct = tangent ^ newAim;
   aim = tangent ^ crossProduct;
   normal = aim ^ tangent;
   MMatrix rotationMat;
   rotationMat.setToIdentity();
   rotationMat[0][0] = tangent[0];
   rotationMat[0][1] = tangent[1];
   rotationMat[0][2] = tangent[2];
   rotationMat[1][0] = normal[0];
   rotationMat[1][1] = normal[1];
   rotationMat[1][2] = normal[2];
   rotationMat[2][0] = aim[0];
   rotationMat[2][1] = aim[1];
   rotationMat[2][2] = aim[2];
   MTransformationMatrix startMTMatrix = MTransformationMatrix(rotationMat);
   MTransformationMatrix::RotationOrder  eulerRotOrder = MTransformationMatrix::kXYZ;
   startMTMatrix.getRotation(rotation, eulerRotOrder);
}

