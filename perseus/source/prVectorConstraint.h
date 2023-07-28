#pragma once
#ifndef prVectorConstraint_H
#define prVectorConstraint_H
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MVector.h>
#include <iostream>
#include <cmath>
#include <maya/MAngle.h>
#include<maya/MFnDependencyNode.h>
#include <maya/MAnimControl.h>

#define RAD2DEG 57.2958
#define DEG2RAD 0.0174532862

class prVectorConstraint : public MPxNode
{
public:
						prVectorConstraint();
	virtual				~prVectorConstraint(); 
    virtual SchedulingType schedulingType() const;
	static  void*		creator();
	virtual MStatus     compute( const MPlug& plug, MDataBlock& data );
	static  MStatus		initialize();
    static MTypeId GetTypeId();
    static MString GetTypeName();
    static MObject offsetRotation;
    static  MObject         offsetRotateX;
    static  MObject         offsetRotateY;
    static  MObject         offsetRotateZ;
    static MObject  matrix;  
    static MObject  worldMatrix;  
    static MObject  drivenPMatrix;  
    static MObject origVec;
    static  MObject         origVecX;
    static  MObject         origVecY;
    static  MObject         origVecZ;
    static MObject aimVec;
    static  MObject         aimVecX;
    static  MObject         aimVecY;
    static  MObject         aimVecZ;
    static MObject rotation;
    static  MObject         rotateX;
    static  MObject         rotateY;
    static  MObject         rotateZ;
    static  MObject         driverAngle;
    static  MObject  driverTranslate;
    static  MObject         blendAngle;
    static  MObject         blendTranslate;
    static  MObject         driverAngleWeight;
    static  MObject         driverTranslateWeight;
    static  MObject  thisMObj;
    static  MObject  driverMin;
    static  MObject  driverMax;
    static  MObject  min;
    static  MObject  max;
    static MObject springIntensity;
    static MObject springStiffness;
    static MObject springDamping;
    static  MObject  radius;
    static  MObject  radiusPower;
    static  MObject  falloffPower;
    static MObject aTime;
    bool _initialized;
    MTime _previousTime;
    MAngle _currentAngle;
    MAngle _previousAngle;
    MAngle _currentPosition;
    MAngle _previousPosition;
    static  MObject aStartFrame;
};
#endif