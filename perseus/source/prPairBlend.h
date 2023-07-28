#ifndef prPairBlend_H
#define prPairBlend_H
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

class prPairBlend : public MPxNode
{
public:
    prPairBlend();
    virtual				~prPairBlend();
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
    static MObject springIntensity;
    static MObject springStiffness;
    static MObject springDamping;
    static MObject aTime;
    bool _initialized;
    MTime _previousTime;
    MAngle _currentUpwardPosition;
    MAngle _previousUpwardPosition;
    MAngle _currentBackwardPosition;
    MAngle _previousBackwardPosition;
    MAngle _currentDownwardPosition;
    MAngle _previousDownwardPosition;
    MAngle _currentForwardPosition;
    MAngle _previousForwardPosition;
    static MObject twistIntensity;
    static  MObject aStartFrame;
    static MObject blend;
    static MObject tzSpring;
    static MObject tySpring;
};

#endif