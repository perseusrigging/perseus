#include "prSimplePairBlend.h"
extern double PI;
extern double DPI;
static const MTypeId TYPE_ID = MTypeId(0x0013be03);
static const MString TYPE_NAME = "prSimplePairBlend";
MObject prSimplePairBlend::inRot; 
MObject prSimplePairBlend::rotateX;
MObject prSimplePairBlend::rotateY;
MObject prSimplePairBlend::rotateZ;
MObject prSimplePairBlend::outRot;
MObject prSimplePairBlend::outRotateX;
MObject prSimplePairBlend::outRotateY;
MObject prSimplePairBlend::outRotateZ;
MObject prSimplePairBlend::pitchYawRoll;
MObject prSimplePairBlend::pitch;
MObject prSimplePairBlend::yaw;
MObject prSimplePairBlend::roll;
MObject prSimplePairBlend::rotOrder;
MObject prSimplePairBlend::forward;
MObject prSimplePairBlend::backward;
MObject prSimplePairBlend::upward;
MObject prSimplePairBlend::downward;
MObject prSimplePairBlend::intensity;
MObject prSimplePairBlend::forwardIntensity;
MObject prSimplePairBlend::backwardIntensity;
MObject prSimplePairBlend::upwardIntensity;
MObject prSimplePairBlend::downwardIntensity;
MObject prSimplePairBlend::twistIntensity;
MObject prSimplePairBlend::blend;

MAngle setRangePairB(const MAngle& inputVal, MAngle driverMin, MAngle driverMax, MAngle min, MAngle max)
{
    MAngle output;
    if (inputVal.value() < driverMin.value())
    {
        output = min;
    }
    else if (inputVal.value() > driverMax.value())
    {
        output = max;
    }
    else
    {

        double oldRange = driverMax.value() - driverMin.value();
        double newRange = max.value() - min.value();
        MAngle currentVal = inputVal.value() - driverMin.value();
        double scale = currentVal.value() / oldRange;
        output = (scale * newRange) + min.value();
    }
    return output;
}

prSimplePairBlend::prSimplePairBlend()
{
}

prSimplePairBlend::~prSimplePairBlend()
{
}

void* prSimplePairBlend::creator()
{
    return new prSimplePairBlend();
}

MEulerRotation::RotationOrder getEulerRotationOrderB(short value)
{
    switch (value)
    {
    case 0:
        return MEulerRotation::kXYZ;
    case 1:
        return MEulerRotation::kXYZ;
    case 2:
        return MEulerRotation::kYZX;
    case 3:
        return MEulerRotation::kZXY;	
    case 4:
        return MEulerRotation::kXZY;	
    case 5:
        return MEulerRotation::kYXZ;	
    case 6:
        return MEulerRotation::kZYX;	
    default:
        return MEulerRotation::kXYZ;
    }
}

MStatus prSimplePairBlend::compute(const MPlug& plug, MDataBlock& data)
{
    if (( plug == backward || plug == forward || plug == upward || plug == downward || plug == outRot || plug == inRot || plug == pitchYawRoll || (plug.isChild() && plug.parent() == pitchYawRoll) || (plug.isChild() && plug.parent() == outRot) || (plug.isChild() && plug.parent() == inRot)))
    {
        MStatus status;
        float twistIntensityVal = data.inputValue(twistIntensity).asFloat();
        float blendVal = data.inputValue(blend).asFloat();
        MDataHandle inRotationData = data.inputValue(inRot);
        MVector inRotation = inRotationData.asVector();
        MDataHandle orderHandle = data.inputValue(rotOrder);
        short rotOrder = orderHandle.asShort();
        MEulerRotation::RotationOrder  eulerRotOrder = getEulerRotationOrderB(rotOrder);
        MQuaternion quat = MEulerRotation(inRotation[0], inRotation[1], inRotation[2], eulerRotOrder).asQuaternion();
        MVector X_VEC(1, 0, 0);
        MVector Y_VEC(0, 1, 0);
        MVector Z_VEC(0, 0, 1);
        MVector qVec = X_VEC.rotateBy(quat);
        MQuaternion bendQ = MQuaternion(X_VEC, qVec);
        MQuaternion xQ = quat * bendQ.inverse();
        double b = (X_VEC * qVec) + 1.0;
        double x = (atan2(xQ[0], xQ[3]) * 2.0);
        if (x > PI)
        {
            x = x - DPI;
        }
        else if (x < -PI)
        {
            x = x + DPI;
        }
        double y = atan2(Z_VEC * qVec, b) * -2.0;
        double z = atan2(Y_VEC * qVec, b) * 2.0;
        double half = x * 0.0f;
        double f = sin(half);
        double h = tan(-blendVal * y * blendVal);
        double v = tan(blendVal * z * blendVal);
        MQuaternion quat2 = MQuaternion(X_VEC[0] * f, X_VEC[1] * f, X_VEC[2] * f, cos(half));
        f = 2. / (h * h + v * v + 1.0);
        quat2 *= MQuaternion(X_VEC, X_VEC * (f - 1.0) + Y_VEC * (v * f) + Z_VEC * (h * f));
        MEulerRotation euler(0.0f, 0.0f, 0.0f, eulerRotOrder);
        euler *= quat2;
        MEulerRotation outputRotation(euler.x, euler.y, euler.z, eulerRotOrder);
        MEulerRotation outputPitchYawRoll(x, y, z, eulerRotOrder);
        double intensityVal = data.inputValue(intensity).asDouble();
        double forwardIntensityVal = data.inputValue(forwardIntensity).asDouble();
        double backwardIntensityVal = data.inputValue(backwardIntensity).asDouble();
        double upIntensityVal = data.inputValue(upwardIntensity).asDouble();
        double downwardIntensityVal = data.inputValue(downwardIntensity).asDouble();
        MDataHandle outputRot = data.outputValue(outRot);
        outputRot.set(outputRotation.x*(1.0- twistIntensityVal)+(outputPitchYawRoll.x* twistIntensityVal), outputRotation.y, outputRotation.z);
        MDataHandle outputDriver = data.outputValue(pitchYawRoll);
        outputDriver.set(outputPitchYawRoll.y, outputPitchYawRoll.z, outputPitchYawRoll.x);
        MAngle normalForward = setRangePairB(outputPitchYawRoll.z, -30, 0.0, -0.2, 0.0);
        MDataHandle outputForward = data.outputValue(forward);
        double normalForwardDeg = normalForward.value() * RAD2DEG * forwardIntensityVal * intensityVal;
        outputForward.set(normalForwardDeg);
        MAngle normalBackward = setRangePairB(outputPitchYawRoll.z, 0.0, 30.0, 0.0, 0.2);
        MDataHandle outputBackward = data.outputValue(backward);
        double normalBackwardDeg = normalBackward.value() * RAD2DEG * backwardIntensityVal * intensityVal;
        outputBackward.set(normalBackwardDeg);
        MAngle normalUpward = setRangePairB(outputPitchYawRoll.y, -30, 0.0, 0.2, 0.0);
        MDataHandle outputUpward = data.outputValue(upward);
        double normalUpwardDeg = normalUpward.value() * RAD2DEG * upIntensityVal * intensityVal;
        outputUpward.set(normalUpwardDeg);
        MAngle normalDownward = setRangePairB(outputPitchYawRoll.y, 0, 30.0, 0.0, -0.2);
        MDataHandle outputDownward = data.outputValue(downward);
        double normalDownwardDeg = normalDownward.value() * RAD2DEG * downwardIntensityVal * intensityVal;
        outputDownward.set(normalDownwardDeg);
        outputForward.set(normalForwardDeg);
        outputBackward.set(normalBackwardDeg);
        outputUpward.set(normalUpwardDeg);
        outputDownward.set(normalDownwardDeg);
        data.setClean(plug);
    }
    else
    {
        return MS::kUnknownParameter;
    }
    return MS::kSuccess;
}

MStatus prSimplePairBlend::initialize()
{
    MStatus status;
    MFnNumericAttribute nAttr;
    MFnMatrixAttribute mAttr;
    MFnEnumAttribute eAttr;
    MFnUnitAttribute uAttr;

    blend = nAttr.create("blend", "blend", MFnNumericData::kFloat, 0.5f);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(blend);

    forward = nAttr.create("forward", "forward", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(forward);

    backward = nAttr.create("backward", "backward", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(backward);

    upward = nAttr.create("upward", "upward", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(upward);

    downward = nAttr.create("downward", "downward", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(downward);

    intensity = nAttr.create("intensity", "intensity", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    nAttr.setKeyable(true);
    addAttribute(intensity);

    forwardIntensity = nAttr.create("forwardIntensity", "forwardIntensity", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    nAttr.setKeyable(true);
    addAttribute(forwardIntensity);

    backwardIntensity = nAttr.create("backwardIntensity", "backwardIntensity", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    nAttr.setKeyable(true);
    addAttribute(backwardIntensity);

    upwardIntensity = nAttr.create("upwardIntensity", "upwardIntensity", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    nAttr.setKeyable(true);
    addAttribute(upwardIntensity);

    downwardIntensity = nAttr.create("downwardIntensity", "downwardIntensity", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    nAttr.setKeyable(true);
    addAttribute(downwardIntensity);

    rotateX = uAttr.create("rotateX", "rx", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(rotateX);

    rotateY = uAttr.create("rotateY", "ry", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(rotateY);

    rotateZ = uAttr.create("rotateZ", "rz", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(rotateZ);

    inRot = nAttr.create("inRotation", "inRot", rotateX, rotateY, rotateZ);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(inRot);

    outRotateX = uAttr.create("outRotateX", "outRx", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(outRotateX);

    outRotateY = uAttr.create("outRotateY", "outRy", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(outRotateY);

    outRotateZ = uAttr.create("outRotateZ", "outRz", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(outRotateZ);

    outRot = nAttr.create("outRotation", "outRot", outRotateX, outRotateY, outRotateZ);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(outRot);

    pitch = uAttr.create("pitch", "pitch", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(pitch);

    yaw = uAttr.create("yaw", "yaw", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(yaw);

    roll = uAttr.create("roll", "roll", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setStorable(true);
    uAttr.setWritable(true);
    uAttr.setReadable(true);
    addAttribute(roll);

    pitchYawRoll = nAttr.create("pitchYawRoll", "pitchYawRoll", pitch, yaw, roll);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(pitchYawRoll);

    rotOrder = eAttr.create("rotationOrder", "rotationOrder");
    eAttr.setReadable(true);
    eAttr.setStorable(true);
    eAttr.setWritable(true);
    eAttr.addField("xyz", 0);
    eAttr.addField("yzx", 1);
    eAttr.addField("zxy", 2);
    eAttr.addField("xzy", 3);
    eAttr.addField("yxz", 4);
    eAttr.addField("zyx", 5);
    addAttribute(rotOrder);

    twistIntensity = nAttr.create("twistIntensity", "twistIntensity", MFnNumericData::kFloat, 0.5);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(twistIntensity);

    attributeAffects(inRot, outRot);
    attributeAffects(rotOrder, outRot);
    attributeAffects(blend, outRot);
    attributeAffects(twistIntensity, outRot);

    attributeAffects(inRot, pitchYawRoll);
    attributeAffects(rotOrder, pitchYawRoll);
    attributeAffects(blend, pitchYawRoll);

    attributeAffects(inRot, forward);
    attributeAffects(rotOrder, forward);
    attributeAffects(forwardIntensity, forward);
    attributeAffects(intensity, forward);
    attributeAffects(blend, forward);
    attributeAffects(twistIntensity, forward);

    attributeAffects(inRot, backward);
    attributeAffects(rotOrder, backward);
    attributeAffects(backwardIntensity, backward);
    attributeAffects(intensity, backward);
    attributeAffects(blend, backward);
    attributeAffects(twistIntensity, backward);

    attributeAffects(inRot, upward);
    attributeAffects(rotOrder, upward);
    attributeAffects(upwardIntensity, upward);
    attributeAffects(intensity, upward);
    attributeAffects(blend, upward);
    attributeAffects(twistIntensity, upward);

    attributeAffects(inRot, downward);
    attributeAffects(rotOrder, downward);
    attributeAffects(downwardIntensity, downward);
    attributeAffects(intensity, downward);
    attributeAffects(blend, downward);
    attributeAffects(twistIntensity, downward);

    return MS::kSuccess;
}

MTypeId prSimplePairBlend::GetTypeId()
{
    return(TYPE_ID);
}

MString prSimplePairBlend::GetTypeName()
{
    return(TYPE_NAME);
}