#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MAngle.h>
#include <maya/MPointArray.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MMatrixArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MQuaternion.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshVertex.h>
#include <maya/MRampAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MTypes.h>
#include <algorithm> 
#include <vector>
#include <map>
#include <array>
#define RAD2DEG 57.2958
#define DEG2RAD 0.0174532862
#define PI 3.14159265359


class prRopeRig : public MPxNode

{
public:
    prRopeRig();
    virtual ~prRopeRig();
    static void* creator();
    static MStatus initialize();
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);
    void postConstructor();
    MStatus postConstructor_init_curveRamp(MObject& nodeObj,
        MObject& rampObj,
        int index,
        float position,
        float value,
        int interpolation);
    static MTypeId GetTypeId();
    static MString GetTypeName();
    static MTypeId id;
    static MObject aInCurve;
    static MObject aInCurveAim;
    static MObject aInDivision;
    static MObject aLength;
    static MObject aSize;
    static MObject aPosition;
    static MObject aMaxStretch;
    static MObject aMinStretch;
    static MObject aGlobalScale;
    static MObject aDegree;
    static MObject aSpans;
    static MObject aRebuild;
    static MObject aEndTwist;
    static MObject aStartTwist;
    static MObject aMidTwist;

    static MObject aRoll;

    static MObject aRotation;
    static MObject aRotationX;
    static MObject aRotationY;
    static MObject aRotationZ;

    static MObject aTranslate;
    static MObject aTranslateX;
    static MObject aTranslateY;
    static MObject aTranslateZ;

    static MObject aScale;
    static MObject aScaleX;
    static MObject aScaleY;
    static MObject aScaleZ;

    static MObject aStartBias;
    static MObject aEndBias;

    static MObject aPushVector;
    static MObject aPushVectorRadius;
    static MObject aPushVectorStrength;

    static MObject aCollisionMesh;
    static MObject aCollisionMeshStrength;

    static MObject aOutCurve;
    static MObject aOutCurveAim;

    static  MObject aStartFrame;
    static MObject aTime;
    static MObject springIntensity;
    static MObject springStiffness;
    static MObject springDamping;
    static MObject springRamp;
    static MObject springElastic;

    static MObject expandRamp;
    static MObject aExpandStrength;
    static MObject shrinkRamp;
    static MObject aShrinkStrength;

    static MObject bulgeRamp;
    static MObject aBulgeStrengthX;
    static MObject aBulgeStrengthY;
    static MObject aBulgeStrengthZ;
    static MObject aBulgeOffset;

    

private:
    std::map<unsigned int, bool> _initialized;
    std::array<MTime, 10000> _previousTime;
    std::array<float3, 10000> _currentPosition;
    std::array<float3, 10000> _previousPosition;

    std::map<unsigned int, bool> _initializedB;
    std::array<MTime, 10000> _previousTimeB;
    std::array<float3, 10000> _currentPositionB;
    std::array<float3, 10000> _previousPositionB;

    std::array<float, 10000> arrStretchMTF;

};



