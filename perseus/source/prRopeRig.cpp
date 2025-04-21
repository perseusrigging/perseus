
#include "prRopeRig.h"
#include "computeEulerRotationFromCurves.h"
static const MTypeId TYPE_ID = MTypeId(0x0013be04);
static const MString TYPE_NAME = "prRopeRig";

MObject prRopeRig::aInCurve;
MObject prRopeRig::aInDivision;
MObject prRopeRig::aLength;
MObject prRopeRig::aSize;
MObject prRopeRig::aPosition;
MObject prRopeRig::aMaxStretch;
MObject prRopeRig::aMinStretch;
MObject prRopeRig::aGlobalScale;
MObject prRopeRig::aDegree;
MObject prRopeRig::aSpans;
MObject prRopeRig::aRebuild;
MObject prRopeRig::aEndTwist;
MObject prRopeRig::aMidTwist;
MObject prRopeRig::aStartTwist;
MObject prRopeRig::aRotation;
MObject prRopeRig::aRotationX;
MObject prRopeRig::aRotationY;
MObject prRopeRig::aRotationZ;
MObject prRopeRig::aTranslate;
MObject prRopeRig::aTranslateX;
MObject prRopeRig::aTranslateY;
MObject prRopeRig::aTranslateZ;
MObject prRopeRig::aScale;
MObject prRopeRig::aScaleX;
MObject prRopeRig::aScaleY;
MObject prRopeRig::aScaleZ;
MObject prRopeRig::aRoll;
MObject prRopeRig::aInCurveAim;
MObject prRopeRig::aStartBias;
MObject prRopeRig::aEndBias;
MObject prRopeRig::aPushVector;
MObject prRopeRig::aPushVectorRadius;
MObject prRopeRig::aPushVectorStrength;
MObject prRopeRig::aCollisionMesh;
MObject prRopeRig::aCollisionMeshStrength;
MObject prRopeRig::aOutCurve;
MObject prRopeRig::aOutCurveAim;
MObject prRopeRig::springIntensity;
MObject prRopeRig::springStiffness;
MObject prRopeRig::springDamping;
MObject prRopeRig::aTime;
MObject prRopeRig::aStartFrame;
MObject prRopeRig::springRamp;
MObject prRopeRig::springElastic;
MObject prRopeRig::expandRamp;
MObject prRopeRig::aExpandStrength;
MObject prRopeRig::shrinkRamp;
MObject prRopeRig::aShrinkStrength;
MObject prRopeRig::bulgeRamp;
MObject prRopeRig::aBulgeStrengthX;
MObject prRopeRig::aBulgeStrengthY;
MObject prRopeRig::aBulgeStrengthZ;
MObject prRopeRig::aBulgeOffset;

prRopeRig::prRopeRig()
{
}

prRopeRig::~prRopeRig()
{
}

void* prRopeRig::creator()
{
    return new prRopeRig();
}

double setRange(const double& inputVal, double driverMin, double driverMax, double min, double max)
{
    double output;
    if (inputVal < driverMin)
    {
        output = min;
    }
    else if (inputVal > driverMax)
    {
        output = max;
    }
    else
    {
        double oldRange = driverMax - driverMin;
        double newRange = max - min;
        double currentVal = inputVal - driverMin;
        double scale = currentVal / oldRange;
        output = (scale * newRange) + min;
    }
    return output;
}

void prRopeRig::postConstructor()
{
    MStatus status;
    MObject thisNode = this->thisMObject();
    postConstructor_init_curveRamp(thisNode, springRamp, 0, 0.0f, 0.0f, 3);
    postConstructor_init_curveRamp(thisNode, springRamp, 1, 1.0f, 1.0f, 3);

    postConstructor_init_curveRamp(thisNode, shrinkRamp, 0, 0.0f, 0.0f, 3);
    postConstructor_init_curveRamp(thisNode, shrinkRamp, 1, 0.25f, 0.75f, 3);
    postConstructor_init_curveRamp(thisNode, shrinkRamp, 2, 0.5f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, shrinkRamp, 3, 0.75f, 0.75f, 3);
    postConstructor_init_curveRamp(thisNode, shrinkRamp, 4, 1.0f, 0.0f, 3);

    postConstructor_init_curveRamp(thisNode, expandRamp, 0, 0.0f, 0.0f, 3);
    postConstructor_init_curveRamp(thisNode, expandRamp, 1, 0.25f, 0.75f, 3);
    postConstructor_init_curveRamp(thisNode, expandRamp, 2, 0.5f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, expandRamp, 3, 0.75f, 0.75f, 3);
    postConstructor_init_curveRamp(thisNode, expandRamp, 4, 1.0f, 0.0f, 3);

    postConstructor_init_curveRamp(thisNode, bulgeRamp, 0, 0.0f, 0.0f, 3);
    postConstructor_init_curveRamp(thisNode, bulgeRamp, 1, 0.25f, 0.0f, 3);
    postConstructor_init_curveRamp(thisNode, bulgeRamp, 2, 0.5f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, bulgeRamp, 3, 0.75f, 0.0f, 3);
    postConstructor_init_curveRamp(thisNode, bulgeRamp, 4, 1.0f, 0.0f, 3);
}

MStatus prRopeRig::postConstructor_init_curveRamp(MObject& nodeObj,
    MObject& rampObj,
    int index,
    float position,
    float value,
    int interpolation)
{
    MStatus status;
    MPlug rampPlug(nodeObj, rampObj);
    MPlug elementPlug = rampPlug.elementByLogicalIndex((unsigned)index, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MPlug positionPlug = elementPlug.child(0, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    status = positionPlug.setFloat(position);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MPlug valuePlug = elementPlug.child(1);
    status = valuePlug.setFloat(value);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MPlug interpolationPlug = elementPlug.child(2);
    interpolationPlug.setInt(interpolation);
    return MS::kSuccess;
}


MStatus prRopeRig::initialize()

{
    MFnNumericAttribute nAttr;
    MFnTypedAttribute nTypeAttr;
    MFnUnitAttribute uAttr;
    MFnMatrixAttribute mAttr;
    MFnMatrixArrayData mArrayData;
    MRampAttribute mrAttr;

    aTime = uAttr.create("time", "time", MFnUnitAttribute::kTime, 0.0f);
    uAttr.setKeyable(true);
    addAttribute(aTime);

    aStartFrame = nAttr.create("startFrame", "startFrame", MFnNumericData::kInt, 0);
    nAttr.setKeyable(true);
    addAttribute(aStartFrame);

    springIntensity = nAttr.create("springIntensity", "springIntensity", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springIntensity);


    springStiffness = nAttr.create("springStiffness", "springStiffness", MFnNumericData::kDouble, 0.025);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springStiffness);


    springDamping = nAttr.create("springDamping", "springDamping", MFnNumericData::kDouble, 0.01);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springDamping);

    springElastic = nAttr.create("springElastic", "springElastic", MFnNumericData::kDouble, 0.5);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springElastic);

    aCollisionMesh = nTypeAttr.create("collisionMesh", "colMesh", MFnMeshData::kMesh);
    addAttribute(aCollisionMesh);

    aRoll = uAttr.create("roll", "roll", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setKeyable(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    addAttribute(aRoll);


    aTranslateX = nAttr.create("translateX", "tx", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    addAttribute(aTranslateX);

    aTranslateY = nAttr.create("translateY", "ty", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    addAttribute(aTranslateY);

    aTranslateZ = nAttr.create("translateZ", "tz", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    addAttribute(aTranslateZ);

    aTranslate = nAttr.create("translate", "t", aTranslateX, aTranslateY, aTranslateZ);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);
    addAttribute(aTranslate);

    aRotationX = uAttr.create("rotationX", "rx", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(false);
    uAttr.setKeyable(false);
    uAttr.setReadable(true);
    uAttr.setWritable(false);
    addAttribute(aRotationX);

    aRotationY = uAttr.create("rotationY", "ry", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(false);
    uAttr.setKeyable(false);
    uAttr.setReadable(true);
    uAttr.setWritable(false);
    addAttribute(aRotationY);

    aRotationZ = uAttr.create("rotationZ", "rz", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(false);
    uAttr.setKeyable(false);
    uAttr.setReadable(true);
    uAttr.setWritable(false);
    addAttribute(aRotationZ);

    aRotation = nAttr.create("rotation", "r", aRotationX, aRotationY, aRotationZ);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);
    addAttribute(aRotation);


    aScaleX = nAttr.create("scaleX", "sx", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    addAttribute(aScaleX);

    aScaleY = nAttr.create("scaleY", "sy", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    addAttribute(aScaleY);

    aScaleZ = nAttr.create("scaleZ", "sz", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    addAttribute(aScaleZ);

    aScale = nAttr.create("scale", "s", aScaleX, aScaleY, aScaleZ);
    nAttr.setStorable(false);
    nAttr.setKeyable(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);
    addAttribute(aScale);


    aStartTwist = uAttr.create("startTwist", "startTwist", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setKeyable(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    addAttribute(aStartTwist);

    aEndTwist = uAttr.create("endTwist", "endTwsit", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setKeyable(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    addAttribute(aEndTwist);

    aMidTwist = uAttr.create("midTwist", "midTwsit", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setKeyable(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    addAttribute(aMidTwist);

    aStartBias = uAttr.create("startBias", "startBias", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setKeyable(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    addAttribute(aStartBias);

    aEndBias = uAttr.create("endBias", "endBias", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setKeyable(true);
    uAttr.setReadable(true);
    uAttr.setWritable(true);
    addAttribute(aEndBias);





    aGlobalScale = mAttr.create("globalScale", "gscale", MFnMatrixAttribute::kDouble);
    mAttr.setStorable(true);
    mAttr.setKeyable(true);
    mAttr.setReadable(true);
    mAttr.setWritable(true);
    addAttribute(aGlobalScale);

    aPosition = nAttr.create("position", "pos", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    addAttribute(aPosition);


    aSize = nAttr.create("size", "size", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(0.01);
    nAttr.setMax(1.0);
    addAttribute(aSize);



    aLength = nAttr.create("length", "len", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    addAttribute(aLength);

    aInCurve = nTypeAttr.create("inputCurve", "ic", MFnData::kNurbsCurve);
    nTypeAttr.setStorable(true);
    nTypeAttr.setKeyable(true);
    nTypeAttr.setChannelBox(true);

    nTypeAttr.setReadable(true);
    nTypeAttr.setWritable(true);
    nTypeAttr.setCached(false);
    addAttribute(aInCurve);


    aInCurveAim = nTypeAttr.create("inputAimCurve", "icAim", MFnData::kNurbsCurve);
    nTypeAttr.setStorable(true);
    nTypeAttr.setKeyable(true);
    nTypeAttr.setReadable(true);
    nTypeAttr.setWritable(true);
    nTypeAttr.setCached(false);
    addAttribute(aInCurveAim);

    aOutCurve = nTypeAttr.create("outputCurve", "oc", MFnData::kNurbsCurve);
    nTypeAttr.setHidden(true);
    addAttribute(aOutCurve);

    aOutCurveAim = nTypeAttr.create("outputCurveAim", "ocAim", MFnData::kNurbsCurve);
    nTypeAttr.setHidden(true);
    addAttribute(aOutCurveAim);


    aInDivision = nAttr.create("division", "div", MFnNumericData::kInt, 6);
    nAttr.setMin(1.0);
    nAttr.setKeyable(true);
    addAttribute(aInDivision);

    aMaxStretch = nAttr.create("maxStretch", "max", MFnNumericData::kDouble, 10.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(1.0);
    addAttribute(aMaxStretch);

    aMinStretch = nAttr.create("minStretch", "min", MFnNumericData::kDouble, 0.1);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    addAttribute(aMinStretch);

    aDegree = nAttr.create("degree", "deg", MFnNumericData::kInt, 3);
    nAttr.setMin(1);
    nAttr.setMax(7);
    nAttr.setKeyable(true);
    addAttribute(aDegree);

    aSpans = nAttr.create("spans", "spans", MFnNumericData::kInt, 4);
    nAttr.setMin(0);
    nAttr.setMax(7);
    addAttribute(aSpans);

    aRebuild = nAttr.create("rebuild", "rbl", MFnNumericData::kBoolean, false);
    nAttr.setKeyable(true);
    addAttribute(aRebuild);


    aPushVector = mAttr.create("pushVector", "pushVector", MFnMatrixAttribute::kDouble);
    mAttr.setStorable(true);
    mAttr.setKeyable(true);
    mAttr.setReadable(true);
    mAttr.setWritable(true);
    addAttribute(aPushVector);

    aPushVectorRadius = nAttr.create("pushVectorRadius", "pushVectorRadius", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    addAttribute(aPushVectorRadius);

    aPushVectorStrength = nAttr.create("pushVectorStrength", "pushVectorStrength", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    addAttribute(aPushVectorStrength);

    aCollisionMeshStrength = nAttr.create("collisionMeshStrength", "collisionMeshStrength", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    addAttribute(aCollisionMeshStrength);

    springRamp = mrAttr.createCurveRamp("springRamp", "springRamp");
    addAttribute(springRamp);

    shrinkRamp = mrAttr.createCurveRamp("shrinkRamp", "shrinkRamp");
    addAttribute(shrinkRamp);

    expandRamp = mrAttr.createCurveRamp("expandRamp", "expandRamp");
    addAttribute(expandRamp);

    aShrinkStrength = nAttr.create("shrinkStrength", "shrinkStrength", MFnNumericData::kFloat, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(-10.0);
    nAttr.setMax(10.0);
    addAttribute(aShrinkStrength);

    aExpandStrength = nAttr.create("expandStrength", "expandStrength", MFnNumericData::kFloat, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(-10.0);
    nAttr.setMax(10.0);
    addAttribute(aExpandStrength);


    aBulgeStrengthX = nAttr.create("bulgeStrengthX", "bulgeStrengthX", MFnNumericData::kFloat, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(-10.0);
    nAttr.setMax(10.0);
    addAttribute(aBulgeStrengthX);

    aBulgeStrengthY = nAttr.create("bulgeStrengthY", "bulgeStrengthY", MFnNumericData::kFloat, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(-10.0);
    nAttr.setMax(10.0);
    addAttribute(aBulgeStrengthY);

    aBulgeStrengthZ = nAttr.create("bulgeStrengthZ", "bulgeStrengthZ", MFnNumericData::kFloat, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(-10.0);
    nAttr.setMax(10.0);
    addAttribute(aBulgeStrengthZ);

    aBulgeOffset = nAttr.create("bulgeOffset", "bulgeOffset", MFnNumericData::kFloat, 0.0);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);
    nAttr.setMin(-10.0);
    nAttr.setMax(10.0);
    addAttribute(aBulgeOffset);

    bulgeRamp = mrAttr.createCurveRamp("bulgeRamp", "bulgeRamp");
    addAttribute(bulgeRamp);


    attributeAffects(aInCurve, aRotation);
    attributeAffects(aInDivision, aRotation);
    attributeAffects(aLength, aRotation);
    attributeAffects(aSize, aRotation);
    attributeAffects(aPosition, aRotation);
    attributeAffects(aMinStretch, aRotation);
    attributeAffects(aMaxStretch, aRotation);
    attributeAffects(aGlobalScale, aRotation);
    attributeAffects(aDegree, aRotation);
    attributeAffects(aStartTwist, aRotation);
    attributeAffects(aEndTwist, aRotation);
    attributeAffects(aMidTwist, aRotation);
    attributeAffects(aRebuild, aRotation);
    attributeAffects(aInCurveAim, aRotation);
    attributeAffects(aRoll, aRotation);
    attributeAffects(aStartBias, aRotation);
    attributeAffects(aEndBias, aRotation);
    attributeAffects(aPushVector, aRotation);
    attributeAffects(aPushVectorRadius, aRotation);
    attributeAffects(aPushVectorStrength, aRotation);
    attributeAffects(aCollisionMesh, aRotation);
    attributeAffects(aCollisionMeshStrength, aRotation);
    attributeAffects(aStartFrame, aRotation);
    attributeAffects(aTime, aRotation);
    attributeAffects(springIntensity, aRotation);
    attributeAffects(springStiffness, aRotation);
    attributeAffects(springDamping, aRotation);
    attributeAffects(springRamp, aRotation);
    attributeAffects(springElastic, aRotation);
    attributeAffects(aInCurve, aTranslate);
    attributeAffects(aInDivision, aTranslate);
    attributeAffects(aLength, aTranslate);
    attributeAffects(aSize, aTranslate);
    attributeAffects(aPosition, aTranslate);
    attributeAffects(aMinStretch, aTranslate);
    attributeAffects(aMaxStretch, aTranslate);
    attributeAffects(aGlobalScale, aTranslate);
    attributeAffects(aDegree, aTranslate);
    attributeAffects(aStartTwist, aTranslate);
    attributeAffects(aEndTwist, aTranslate);
    attributeAffects(aMidTwist, aTranslate);
    attributeAffects(aRebuild, aTranslate);
    attributeAffects(aInCurveAim, aTranslate);
    attributeAffects(aRoll, aTranslate);
    attributeAffects(aStartBias, aTranslate);
    attributeAffects(aEndBias, aTranslate);
    attributeAffects(aPushVector, aTranslate);
    attributeAffects(aPushVectorRadius, aTranslate);
    attributeAffects(aPushVectorStrength, aTranslate);
    attributeAffects(aCollisionMesh, aTranslate);
    attributeAffects(aCollisionMeshStrength, aTranslate);
    attributeAffects(aStartFrame, aTranslate);
    attributeAffects(aTime, aTranslate);
    attributeAffects(springIntensity, aTranslate);
    attributeAffects(springStiffness, aTranslate);
    attributeAffects(springDamping, aTranslate);
    attributeAffects(springRamp, aTranslate);
    attributeAffects(springElastic, aTranslate);
    attributeAffects(aInCurve, aScale);
    attributeAffects(aInDivision, aScale);
    attributeAffects(aLength, aScale);
    attributeAffects(aSize, aScale);
    attributeAffects(aPosition, aScale);
    attributeAffects(aMinStretch, aScale);
    attributeAffects(aMaxStretch, aScale);
    attributeAffects(aGlobalScale, aScale);
    attributeAffects(aDegree, aScale);
    attributeAffects(aStartTwist, aScale);
    attributeAffects(aEndTwist, aScale);
    attributeAffects(aMidTwist, aScale);
    attributeAffects(aRebuild, aScale);
    attributeAffects(aInCurveAim, aScale);
    attributeAffects(aRoll, aScale);
    attributeAffects(aStartBias, aScale);
    attributeAffects(aEndBias, aScale);
    attributeAffects(aPushVector, aScale);
    attributeAffects(aPushVectorRadius, aScale);
    attributeAffects(aPushVectorStrength, aScale);
    attributeAffects(aCollisionMesh, aScale);
    attributeAffects(aCollisionMeshStrength, aScale);
    attributeAffects(aStartFrame, aScale);
    attributeAffects(aTime, aScale);
    attributeAffects(springIntensity, aScale);
    attributeAffects(springStiffness, aScale);
    attributeAffects(springDamping, aScale);
    attributeAffects(springRamp, aScale);
    attributeAffects(springElastic, aScale);
    attributeAffects(aExpandStrength, aScale);
    attributeAffects(aShrinkStrength, aScale);
    attributeAffects(expandRamp, aScale);
    attributeAffects(shrinkRamp, aScale);
    attributeAffects(bulgeRamp, aScale);
    attributeAffects(aBulgeStrengthX, aScale);
    attributeAffects(aBulgeStrengthY, aScale);
    attributeAffects(aBulgeStrengthZ, aScale);
    attributeAffects(aBulgeOffset, aScale);
    attributeAffects(aInCurve, aOutCurve);
    attributeAffects(aInCurveAim, aOutCurveAim);
    attributeAffects(aInDivision, aOutCurve);
    attributeAffects(aLength, aOutCurve);
    attributeAffects(aSize, aOutCurve);
    attributeAffects(aPosition, aOutCurve);
    attributeAffects(aMinStretch, aOutCurve);
    attributeAffects(aMaxStretch, aOutCurve);
    attributeAffects(aGlobalScale, aOutCurve);
    attributeAffects(aDegree, aOutCurve);
    attributeAffects(aStartTwist, aOutCurve);
    attributeAffects(aEndTwist, aOutCurve);
    attributeAffects(aMidTwist, aOutCurve);
    attributeAffects(aRebuild, aOutCurve);
    attributeAffects(aRoll, aOutCurve);
    attributeAffects(aStartBias, aOutCurve);
    attributeAffects(aEndBias, aOutCurve);
    attributeAffects(aPushVector, aOutCurve);
    attributeAffects(aPushVectorRadius, aOutCurve);
    attributeAffects(aPushVectorStrength, aOutCurve);
    attributeAffects(aCollisionMesh, aOutCurve);
    attributeAffects(aCollisionMeshStrength, aOutCurve);
    attributeAffects(aStartFrame, aOutCurve);
    attributeAffects(aTime, aOutCurve);
    attributeAffects(springIntensity, aOutCurve);
    attributeAffects(springStiffness, aOutCurve);
    attributeAffects(springDamping, aOutCurve);
    attributeAffects(springRamp, aOutCurve);
    attributeAffects(springElastic, aOutCurve);
    attributeAffects(aInCurve, aOutCurveAim);
    attributeAffects(aInCurveAim, aOutCurveAim);
    attributeAffects(aInDivision, aOutCurveAim);
    attributeAffects(aLength, aOutCurveAim);
    attributeAffects(aSize, aOutCurveAim);
    attributeAffects(aPosition, aOutCurveAim);
    attributeAffects(aMinStretch, aOutCurveAim);
    attributeAffects(aMaxStretch, aOutCurveAim);
    attributeAffects(aGlobalScale, aOutCurveAim);
    attributeAffects(aDegree, aOutCurveAim);
    attributeAffects(aStartTwist, aOutCurveAim);
    attributeAffects(aEndTwist, aOutCurveAim);
    attributeAffects(aMidTwist, aOutCurveAim);
    attributeAffects(aRebuild, aOutCurveAim);
    attributeAffects(aRoll, aOutCurveAim);
    attributeAffects(aStartBias, aOutCurveAim);
    attributeAffects(aEndBias, aOutCurveAim);
    attributeAffects(aPushVector, aOutCurveAim);
    attributeAffects(aPushVectorRadius, aOutCurveAim);
    attributeAffects(aPushVectorStrength, aOutCurveAim);
    attributeAffects(aCollisionMesh, aOutCurveAim);
    attributeAffects(aCollisionMeshStrength, aOutCurveAim);
    attributeAffects(aStartFrame, aOutCurveAim);
    attributeAffects(aTime, aOutCurveAim);
    attributeAffects(springIntensity, aOutCurveAim);
    attributeAffects(springStiffness, aOutCurveAim);
    attributeAffects(springDamping, aOutCurveAim);
    attributeAffects(springRamp, aOutCurveAim);
    attributeAffects(springElastic, aOutCurveAim);
    return MS::kSuccess;

}


MStatus prRopeRig::compute(const MPlug& plug, MDataBlock& data)

{

    if (((plug.parent() == aRotation || plug == aRotation || plug.parent() == aTranslate || plug == aTranslate ||  plug.parent() == aScale || plug == aScale || plug == aOutCurve || plug == aOutCurveAim)))
    {
        MStatus status;
        int startFrame = data.inputValue(aStartFrame).asInt();
        MTime currentTime = data.inputValue(aTime, &status).asTime();
        double intensity = data.inputValue(springIntensity, &status).asDouble();
        double damping = data.inputValue(springDamping, &status).asDouble();
        double stiffness = data.inputValue(springStiffness, &status).asDouble();
        double elastic = data.inputValue(springElastic, &status).asDouble();

        MObject inCurveAim = data.inputValue(aInCurveAim).asNurbsCurve();
        MObject inCurve = data.inputValue(aInCurve).asNurbsCurve();
        int division = data.inputValue(aInDivision).asInt();
        //division -= 1;
        double inLength = data.inputValue(aLength).asDouble();
        double inScale = data.inputValue(aSize).asDouble();
        //inScale *= 0.1;
        double inPosition = data.inputValue(aPosition).asDouble();
        inPosition *= 0.1;
        MMatrix inGlobalScaleMat = data.inputValue(aGlobalScale).asMatrix();
        MTransformationMatrix inGlobalScaleTrMat = MTransformationMatrix(inGlobalScaleMat);

        MAngle  inStartTwist = data.inputValue(aStartTwist).asAngle();
        MAngle  inEndTwist = data.inputValue(aEndTwist).asAngle();
        MAngle  inMidTwist = data.inputValue(aMidTwist).asAngle();
        MAngle  inStartBias = data.inputValue(aStartBias).asAngle();
        MAngle  inEndBias = data.inputValue(aEndBias).asAngle();
        MAngle  inRoll = data.inputValue(aRoll).asAngle();

        float inExpandStrength = data.inputValue(aExpandStrength).asFloat();
        float inShrinkStrength = data.inputValue(aShrinkStrength).asFloat();
        float inBulgeStrengthX = data.inputValue(aBulgeStrengthX).asFloat();
        float inBulgeStrengthY = data.inputValue(aBulgeStrengthY).asFloat();
        float inBulgeStrengthZ = data.inputValue(aBulgeStrengthZ).asFloat();
        float inBulgeOffset = data.inputValue(aBulgeOffset).asFloat()*0.1;

        MDataHandle inColMesh = data.inputValue(aCollisionMesh, &status);
        MFnMesh colMeshFn = inColMesh.asMesh();

        MStatus stat = MS::kSuccess;
        MObject oThis = thisMObject();
        MRampAttribute curveSpringAttribute(oThis, springRamp);
        MRampAttribute curveExpandAttribute(oThis, expandRamp);
        MRampAttribute curveShrinkAttribute(oThis, shrinkRamp);
        MRampAttribute curveBulgeAttribute(oThis, bulgeRamp);

        MMatrix inPushVectorMat;
        MVector inPushVector=MVector(0.0,0.0,0.0);
        double inPushVectorRadius;
        double inPushVectorStrength;
        MPlug inPushVectorPlug(thisMObject(), aPushVector);
        if (inPushVectorPlug.isConnected()) {
            inPushVectorMat  = data.inputValue(aPushVector).asMatrix();
            inPushVector =MTransformationMatrix(inPushVectorMat).getTranslation(MSpace::kWorld);
            inPushVectorRadius = data.inputValue(aPushVectorRadius).asDouble();
            inPushVectorStrength = data.inputValue(aPushVectorStrength).asDouble();
        }

        MPlug inCollisionMeshPlug(thisMObject(), aCollisionMesh);
        double inCollisionMeshStrength = data.inputValue(aCollisionMeshStrength).asDouble();
        double inCollisionMeshRadius = 1.0;


        MPointArray meshPoints;
        double inGlobalScaleArray[3];
        double* inGlobalScalePtr = inGlobalScaleArray;

        inGlobalScaleTrMat.getScale(inGlobalScalePtr, MSpace::kWorld);
        double inGlobalScale = inGlobalScalePtr[0];

        double inMaxStretch = data.inputValue(aMaxStretch).asDouble() * inGlobalScale;
        double inMinStretch = data.inputValue(aMinStretch).asDouble() * inGlobalScale;


        bool rebuild = data.inputValue(aRebuild).asBool();
        int degree = data.inputValue(aDegree).asInt();
        int spans = data.inputValue(aSpans).asInt();

        MArrayDataHandle rotationArrayHandle = data.outputArrayValue(aRotation, &status);
        MArrayDataBuilder rotationArrayBuilder = rotationArrayHandle.builder();
        MDataHandle rotationElementHandle;

        MArrayDataHandle translateArrayHandle = data.outputArrayValue(aTranslate, &status);
        MArrayDataBuilder translateArrayBuilder = translateArrayHandle.builder();
        MDataHandle translateElementHandle;

        
        MArrayDataHandle scaleArrayHandle = data.outputArrayValue(aScale, &status);
        MArrayDataBuilder scaleArrayBuilder = scaleArrayHandle.builder();
        MDataHandle scaleElementHandle;
        

        MFnNurbsCurve curveFn(inCurve);
        if (inCurve != MObject::kNullObj)
            curveFn.setObject(inCurve);
        double length = curveFn.length();
        int origSpans = curveFn.numSpans();
        int origDegree = curveFn.degree();
        if (rebuild == true)
        {
            inCurve = curveFn.rebuild(origSpans, degree);
            MFnNurbsCurve curveFnB(inCurveAim);
            inCurveAim = curveFnB.rebuild(origSpans, degree);
        }
        MFnNurbsCurve curveFnAim(inCurveAim);
        curveFn.setObject(inCurve);


        double minLength;
        MPoint position, aimPosition, positionB, positionC,closePoint;
        MVector normal, tangent, aim, direction, closeNormal,delta;
        double paramU, distance, parameter, dis, newDis, disParam, newInLength, scaleVal;
        double rotation[3], rotationB[3], locRotation[3], scale[3], fkARotation[3], fkAPreRotation[3];
        float expandValue, shrinkValue, bulgeValue, scaleLength;

        dis = (length / division);
        double resetRotation[3] = { 0.0, 0.0, 0.0 };
        double twistRotation[3] = { 0.0, 0.0, 0.0 };


        MDagPath dummyDagPath;
        MDagPath dummyAimDagPath;

        MTransformationMatrix::RotationOrder  trEulerRotOrder = MTransformationMatrix::kXYZ;
        MEulerRotation::RotationOrder  eulerRotOrder = MEulerRotation::kXYZ;

        MMatrix origMat;
        origMat.setToIdentity();
        MTransformationMatrix currentMTMatrix = MTransformationMatrix(origMat);

        MMatrix previousMat;
        previousMat.setToIdentity();
        MTransformationMatrix previousMTMatrix = MTransformationMatrix(previousMat);

        MMatrix tempMat;
        tempMat.setToIdentity();
        MTransformationMatrix tempMTMatrix = MTransformationMatrix(tempMat);

        MMatrix firstMat;
        firstMat.setToIdentity();
        MTransformationMatrix firstMTMatrix = MTransformationMatrix(tempMat);

        MPoint firstPosition = MPoint(0.0, -0.763, 0.0, 1.0);
        MVector pushVector = MVector({ 0.0,0.0,0.0 });
        MVector previousPos = MVector({ 0.0,0.0,0.0 });
        bool pushVectorBool = false;
        if (inPushVectorPlug.isConnected()) {
            pushVectorBool = true;
        }

        MObject outCurve, outCurveAim;
        float offsetDistance = 1.0;
        MPointArray originalPoints, offsetPoints, controlPoints, offsetPointsB;
        MDoubleArray knotSequences;
        MObject outputCurveObj;
        MFnNurbsCurveData curveDataFn;
        MFnNurbsCurveData curveAimDataFn;
        MFnNurbsCurve outputCurveFn, outCurveFn(outCurve), outputCurveAimFn, outCurveAimFn(outCurveAim);

        curveFn.getCVs(originalPoints, MSpace::kWorld);
        curveFn.getCVs(controlPoints);
        curveFn.getKnots(knotSequences);
        degree = curveFn.degree();
        outCurve = curveDataFn.create();
        MPoint newPosition;
        MPoint velocity;
        MPoint goalForce;
        bool isCollisionConnected = inCollisionMeshPlug.isConnected();
        bool isPushVectorConnected = inPushVectorPlug.isConnected();

        float stretchValue;
        float  rampUnit = 1.0 / float(originalPoints.length());
        for (int i = 0; i < originalPoints.length(); ++i) {
            curveSpringAttribute.getValueAtPosition(rampUnit*i, stretchValue);
            MPoint position = originalPoints[i];
                if (inPushVector.length() > 0 && isPushVectorConnected && abs(inPushVectorStrength) > 0.0001) {
                    MVector pushVec = position - inPushVector;
                    if (pushVec.length() < inPushVectorRadius) {
                        position += pushVec.normal() * inPushVectorStrength * (inPushVectorRadius - pushVec.length());
                    }
            }

            if (intensity > 0.009)
            {

            if (_initialized[i] == false) {
                _previousPosition[i][0] = position.x;
                _previousPosition[i][1] = position.y;
                _previousPosition[i][2] = position.z;
                _currentPosition[i][0] = position.x;
                _currentPosition[i][1] = position.y;
                _currentPosition[i][2] = position.z;
                _previousTime[i] = currentTime;
                _initialized[i] = true;
            }
            MTime timeDifference = currentTime - _previousTime[i];
            if (timeDifference > 1.0 || timeDifference < 0.0 || currentTime.value() < startFrame) {
                _initialized[i] = false;
                _previousPosition[i][0] = position.x;
                _previousPosition[i][1] = position.y;
                _previousPosition[i][2] = position.z;
                _currentPosition[i][0] = position.x;
                _currentPosition[i][1] = position.y;
                _currentPosition[i][2] = position.z;
                _previousTime[i] = currentTime;
            }
            velocity[0] = (_currentPosition[i][0] - _previousPosition[i][0]) * (1.0 - damping);
            velocity[1] = (_currentPosition[i][1] - _previousPosition[i][1]) * (1.0 - damping);
            velocity[2] = (_currentPosition[i][2] - _previousPosition[i][2]) * (1.0 - damping);
            newPosition[0] = _currentPosition[i][0] + velocity[0]*elastic;
            newPosition[1] = _currentPosition[i][1] + velocity[1];
            newPosition[2] = _currentPosition[i][2] + velocity[2];
            goalForce = (position - newPosition) * stiffness;
            newPosition = newPosition + goalForce;
            _previousPosition[i][0] = _currentPosition[i][0];
            _previousPosition[i][1] = _currentPosition[i][1];
            _previousPosition[i][2] = _currentPosition[i][2];
            _currentPosition[i][0] = newPosition[0];
            _currentPosition[i][1] = newPosition[1];
            _currentPosition[i][2] = newPosition[2];
            _previousTime[i] = currentTime;
            position = position + ((newPosition - position) * intensity) * stretchValue;
            }

            offsetPoints.append(position);
        }

        for (int i = 0; i < originalPoints.length(); ++i) {
            curveSpringAttribute.getValueAtPosition(rampUnit * i, stretchValue);
            MPoint position = offsetPoints[i];
            

            if (isCollisionConnected && abs(inCollisionMeshStrength) > 0.0001 ) {
                MPoint closePoint;
                MVector closeNormal;
                colMeshFn.getClosestPointAndNormal(position, closePoint, closeNormal, MSpace::kWorld, NULL, NULL);
                MVector delta = position - closePoint;
                double distance = delta.length();
                double normalDistanceB = setRange(distance, 0.0, inCollisionMeshRadius, 0.0, 1.0);
                double normalDistance = 1.0;
                double weightA = (delta * closeNormal < 0.0) ? 1 - normalDistance : normalDistanceB;
                MVector translateA = weightA * position + (1 - weightA) * closePoint + closeNormal * (1 - weightA);
                translateA = inCollisionMeshStrength * translateA + (1 - inCollisionMeshStrength) * position;
                position = translateA;
            }
            offsetPoints[i] = position;
        }

        outputCurveFn.create(offsetPoints, knotSequences, degree, MFnNurbsCurve::kOpen, false, false, outCurve, &status);
        outputCurveFn.updateCurve();
        data.outputValue(aOutCurve).setMObject(outCurve);
        controlPoints.clear();
        knotSequences.clear();
        offsetPoints.clear();

        curveFnAim.getCVs(controlPoints);
        curveFnAim.getKnots(knotSequences);
        degree = curveFnAim.degree();
        outCurveAim = curveAimDataFn.create();
        curveFnAim.getCVs(originalPoints, MSpace::kWorld);


        for (int i = 0; i < originalPoints.length(); ++i) {
            curveSpringAttribute.getValueAtPosition(rampUnit * i, stretchValue);
            MPoint position = originalPoints[i];
                if (inPushVector.length() > 0 && isPushVectorConnected && abs(inPushVectorStrength) > 0.0001) {
                    MVector pushVec = position - inPushVector;
                    if (pushVec.length() < inPushVectorRadius) {
                        position += pushVec.normal() * inPushVectorStrength * (inPushVectorRadius - pushVec.length());
                    }
            }

            if (intensity > 0.009)
            {
                if (_initialized[i] == false) {
                    _previousPositionB[i][0] = position.x;
                    _previousPositionB[i][1] = position.y;
                    _previousPositionB[i][2] = position.z;
                    _currentPositionB[i][0] = position.x;
                    _currentPositionB[i][1] = position.y;
                    _currentPositionB[i][2] = position.z;
                    _previousTimeB[i] = currentTime;
                    _initializedB[i] = true;
                }
                MTime timeDifference = currentTime - _previousTimeB[i];
                if (timeDifference > 1.0 || timeDifference < 0.0 || currentTime.value() < startFrame) {
                    _initialized[i] = false;
                    _previousPositionB[i][0] = position.x;
                    _previousPositionB[i][1] = position.y;
                    _previousPositionB[i][2] = position.z;
                    _currentPositionB[i][0] = position.x;
                    _currentPositionB[i][1] = position.y;
                    _currentPositionB[i][2] = position.z;
                    _previousTimeB[i] = currentTime;
                }
                velocity[0] = (_currentPositionB[i][0] - _previousPositionB[i][0]) * (1.0 - damping);
                velocity[1] = (_currentPositionB[i][1] - _previousPositionB[i][1]) * (1.0 - damping);
                velocity[2] = (_currentPositionB[i][2] - _previousPositionB[i][2]) * (1.0 - damping);
                newPosition[0] = _currentPositionB[i][0] + velocity[0]*elastic;
                newPosition[1] = _currentPositionB[i][1] + velocity[1];
                newPosition[2] = _currentPositionB[i][2] + velocity[2];
                goalForce = (position - newPosition) * stiffness;
                newPosition = newPosition + goalForce;
                _previousPositionB[i][0] = _currentPositionB[i][0];
                _previousPositionB[i][1] = _currentPositionB[i][1];
                _previousPositionB[i][2] = _currentPositionB[i][2];
                _currentPositionB[i][0] = newPosition[0];
                _currentPositionB[i][1] = newPosition[1];
                _currentPositionB[i][2] = newPosition[2];
                _previousTimeB[i] = currentTime;
                position = position + ((newPosition - position) * intensity) * stretchValue;
            }


            offsetPoints.append(position);
        }
        for (int i = 0; i < originalPoints.length(); ++i) {
            curveSpringAttribute.getValueAtPosition(rampUnit * i, stretchValue);
            MPoint position = offsetPoints[i];
            
            if (isCollisionConnected && abs(inCollisionMeshStrength)>0.0001) {
                MPoint closePoint;
                MVector closeNormal;
                colMeshFn.getClosestPointAndNormal(position, closePoint, closeNormal, MSpace::kWorld, NULL, NULL);
                MVector delta = position - closePoint;
                double distance = delta.length();
                double normalDistanceB = setRange(distance, 0.0, inCollisionMeshRadius, 0.0, 1.0);
                double normalDistance = 1.0;
                double weightA = (delta * closeNormal < 0.0) ? 1 - normalDistance : normalDistanceB;
                MVector translateA = weightA * position + (1 - weightA) * closePoint + closeNormal * (1 - weightA);
                translateA = inCollisionMeshStrength * translateA + (1 - inCollisionMeshStrength) * position;
                position = translateA;
            }
            offsetPoints[i] = position;
        }
 
        outputCurveAimFn.create(offsetPoints, knotSequences, degree, MFnNurbsCurve::kOpen, false, false, outCurveAim, &status);
        outputCurveAimFn.updateCurve();
        data.outputValue(aOutCurveAim).setMObject(outCurveAim);
        controlPoints.clear();
        knotSequences.clear();
        offsetPoints.clear();
 
        length = outputCurveFn.length();
        double stretchFactor = (length / inLength);
        double finalStretchFactor = 1.0;
        if (stretchFactor > 1.0) {
            finalStretchFactor = std::min(inMaxStretch, stretchFactor);
        }
        else if (stretchFactor < 1.0) {
            finalStretchFactor = std::max(inMinStretch, stretchFactor);
        }
        double scaleFactor = inLength / length * finalStretchFactor;
        dis = ((length) / division)* scaleFactor;
        double lengthBB = std::min(length, (inLength * inMaxStretch));
        minLength = std::max(lengthBB, (inLength * inMinStretch));

        
        rampUnit = 1.0 / float(division);
        for (int i = 0; i <= (division); ++i) {  
            newInLength = ((minLength) / division) * i;
            newDis = i * dis * inScale;
            if (i == division)
                newDis = (i - 0.001) * dis * inScale;
            if (i == 0)
                newDis = (i + 0.001) * dis * inScale;

            disParam = outputCurveFn.findParamFromLength(newDis);
            computeEulerRotationFromCurves(dummyDagPath, dummyAimDagPath, disParam + inPosition, position, aimPosition, normal, tangent, aim, rotation, paramU, outCurve, outCurveAim);

            if ((length) < newInLength)
            { 
                computeEulerRotationFromCurves(dummyDagPath, dummyAimDagPath, .95, positionB, aimPosition, normal, tangent, aim, rotationB, paramU, outCurve, outCurveAim);
                direction = (position - positionB).normal();
                direction = direction * ((newInLength - length) / (division / i)) * (1.0);
                position = position + direction;
            }
            
            if (inPushVector.length() > 0 && isPushVectorConnected && abs(inPushVectorStrength) > 0.0001) {
                MVector pushVec = position - inPushVector;
                if (pushVec.length() < inPushVectorRadius) {
                    position += pushVec.normal() * inPushVectorStrength * (inPushVectorRadius - pushVec.length());
                }
            }
            

            if (isCollisionConnected && abs(inCollisionMeshStrength) > 0.0001) {
                MPoint closePoint;
                MVector closeNormal;
                colMeshFn.getClosestPointAndNormal(position, closePoint, closeNormal, MSpace::kWorld, NULL, NULL);
                MVector delta = position - closePoint;
                double distance = delta.length();
                double normalDistanceB = setRange(distance, 0.0, inCollisionMeshRadius, 0.0, 1.0);
                double normalDistance = 1.0;
                double weightA = (delta * closeNormal < 0.0) ? 1 - normalDistance : normalDistanceB;
                MVector translateA = weightA * position + (1 - weightA) * closePoint + closeNormal * (1 - weightA);
                translateA = inCollisionMeshStrength * translateA + (1 - inCollisionMeshStrength) * position;
                position = translateA;
            }

            float bulgeValues[3];
            float GlShrinkScale;
            curveBulgeAttribute.getValueAtPosition((rampUnit* i)+inBulgeOffset, bulgeValue);
            bulgeValues[0]= bulgeValue* inBulgeStrengthX * inGlobalScale;
            bulgeValues[1] = bulgeValue * inBulgeStrengthY * inGlobalScale;
            bulgeValues[2] = bulgeValue * inBulgeStrengthZ * inGlobalScale;
            scaleLength = inGlobalScale * inLength;
            if ((scaleLength) > length)
            {
                curveExpandAttribute.getValueAtPosition(rampUnit * i, expandValue);
                GlShrinkScale= inGlobalScale + ((1.0 - (length / scaleLength)) * inExpandStrength * expandValue) * inGlobalScale;
                scaleVal = bulgeValues[0] + GlShrinkScale;
                scaleVal = std::max(scaleVal, 0.02 * inGlobalScale);
                scale[0] = scaleVal;
                scaleVal = bulgeValues[1] + GlShrinkScale;
                scaleVal = std::max(scaleVal, 0.02 * inGlobalScale);
                scale[1] = scaleVal;
                scaleVal = bulgeValues[2] + GlShrinkScale;
                scaleVal = std::max(scaleVal, 0.02 * inGlobalScale);
                scale[2] = scaleVal;
            }
            else
            {
                curveShrinkAttribute.getValueAtPosition(rampUnit * i, shrinkValue);
                GlShrinkScale = inGlobalScale + ((1.0 - (length / scaleLength)) * inShrinkStrength * shrinkValue) * inGlobalScale;
                scaleVal = bulgeValues[0] + GlShrinkScale;
                scaleVal = std::max(scaleVal, 0.02 * inGlobalScale);
                scale[0] = scaleVal;
                scaleVal = bulgeValues[1] + GlShrinkScale;
                scaleVal = std::max(scaleVal, 0.02 * inGlobalScale);
                scale[1] = scaleVal;
                scaleVal = bulgeValues[2] + GlShrinkScale;
                scaleVal = std::max(scaleVal, 0.02 * inGlobalScale);
                scale[2] = scaleVal;
            }

            int middle = int(division / 2.0);
            int j = ((division / 2) - abs(i - middle)) * 2;        
            currentMTMatrix.setRotation(rotation, trEulerRotOrder);
            currentMTMatrix.setTranslation(position, MSpace::kWorld);
            currentMTMatrix.setScale(scale, MSpace::kWorld);
            twistRotation[2] = (inStartBias.value() * (division - i) / division) + (inEndBias.value() * (i) / (division));
            twistRotation[0] = inRoll.value() + (inStartTwist.value() * (division - i) / division) + (inEndTwist.value() * (i) / (division)+(inMidTwist.value() * (j) / (division)));
            MMatrix twistRotMat = MEulerRotation(twistRotation).asMatrix();
            MTransformationMatrix twistMat = MTransformationMatrix(twistRotMat * currentMTMatrix.asMatrix());
            twistMat.getRotation(rotation, trEulerRotOrder);
            position=twistMat.getTranslation(MSpace::kWorld);
            twistMat.getScale(scale, MSpace::kWorld);
            translateElementHandle = translateArrayBuilder.addElement(i);
            rotationElementHandle = rotationArrayBuilder.addElement(i);
            scaleElementHandle = scaleArrayBuilder.addElement(i);
            translateElementHandle.child(aTranslate).set3Double(position.x, position.y, position.z);
            rotationElementHandle.child(aRotation).set3Double(rotation[0], rotation[1], rotation[2]);
            scaleElementHandle.child(aScale).set3Double(scale[0], scale[1], scale[2]);
        }
        translateArrayHandle.set(translateArrayBuilder);
        rotationArrayHandle.set(rotationArrayBuilder);
        scaleArrayHandle.set(scaleArrayBuilder);
        meshPoints.clear();
        
        data.setClean(plug);
    }
    else
    {
        return MS::kUnknownParameter;
    }

    return MS::kSuccess;
}

MTypeId prRopeRig::GetTypeId()
{
    return(TYPE_ID);
}

MString prRopeRig::GetTypeName()
{
    return(TYPE_NAME);
}

