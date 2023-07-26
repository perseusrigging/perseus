#pragma once
#ifndef prSimplePairBlend_H
#define prSimplePairBlend_H
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MVector.h>
#include <iostream>
#include <cmath>
#include <maya/MAngle.h>
#include <maya/MAnimControl.h>
#define RAD2DEG 57.2958
#define DEG2RAD 0.0174532862

class prSimplePairBlend : public MPxNode
{
public:
    prSimplePairBlend();
    virtual				~prSimplePairBlend();
    static  void* creator();
    virtual MStatus     compute(const MPlug& plug, MDataBlock& data);
    static  MStatus		initialize();
    static MTypeId GetTypeId();
    static MString GetTypeName();
    static  MObject     inRot;
    static  MObject         rotateX;
    static  MObject         rotateY;
    static  MObject         rotateZ;
    static MObject rotOrder;
    static MObject outRot;
    static  MObject         outRotateX;
    static  MObject         outRotateY;
    static  MObject         outRotateZ;
    static MObject pitchYawRoll;
    static MObject pitch;
    static MObject yaw;
    static MObject roll;
    static MObject upward;
    static MObject downward;
    static MObject forward;
    static MObject backward;
    static MObject intensity;
    static MObject forwardIntensity;
    static MObject backwardIntensity;
    static MObject upwardIntensity;
    static MObject downwardIntensity;
    static MObject twistIntensity;
    bool _initialized;
    static MObject blend;
};

#endif