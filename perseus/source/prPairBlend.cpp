#include "prPairBlend.h"
double PI = 3.14159265358979323846; /* pi */
double DPI = PI * 2.0;

#define ROTATE_ORDER_XYZ        0
#define ROTATE_ORDER_YZX        1
#define ROTATE_ORDER_ZXY        2
#define ROTATE_ORDER_XZY        3
#define ROTATE_ORDER_YXZ        4
#define ROTATE_ORDER_ZYX        5
static const MTypeId TYPE_ID = MTypeId(0x0013be02);
static const MString TYPE_NAME = "prPairBlend";

MObject prPairBlend::inRot; 
MObject prPairBlend::rotateX;
MObject prPairBlend::rotateY;
MObject prPairBlend::rotateZ;
MObject prPairBlend::outRot;
MObject prPairBlend::outRotateX;
MObject prPairBlend::outRotateY;
MObject prPairBlend::outRotateZ;
MObject prPairBlend::pitchYawRoll;
MObject prPairBlend::pitch;
MObject prPairBlend::yaw;
MObject prPairBlend::roll;
MObject prPairBlend::rotOrder;
MObject prPairBlend::forward;
MObject prPairBlend::backward;
MObject prPairBlend::upward;
MObject prPairBlend::downward;
MObject prPairBlend::intensity;
MObject prPairBlend::forwardIntensity;
MObject prPairBlend::backwardIntensity;
MObject prPairBlend::upwardIntensity;
MObject prPairBlend::downwardIntensity;
MObject prPairBlend::springIntensity;
MObject prPairBlend::springStiffness;
MObject prPairBlend::springDamping;
MObject prPairBlend::blend;
MObject prPairBlend::twistIntensity;
MObject prPairBlend::aTime;
MObject prPairBlend::aStartFrame;
MObject prPairBlend::tzSpring;
MObject prPairBlend::tySpring;

MAngle setRangePair(const MAngle& inputVal, MAngle driverMin, MAngle driverMax, MAngle min, MAngle max)
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

prPairBlend::prPairBlend()
{
}

prPairBlend::~prPairBlend()
{
}

void* prPairBlend::creator()
{
    return new prPairBlend();
}

MEulerRotation::RotationOrder getEulerRotationOrder(short value)
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

MStatus prPairBlend::compute(const MPlug& plug, MDataBlock& data)
{

    if ((plug == tySpring || plug == tzSpring || plug == backward || plug == forward || plug == upward || plug == downward || plug == outRot || plug == inRot || plug == pitchYawRoll || (plug.isChild() && plug.parent() == pitchYawRoll) || (plug.isChild() && plug.parent() == outRot) || (plug.isChild() && plug.parent() == inRot)))
    {
        MStatus status;
        float twistIntensityVal = data.inputValue(twistIntensity).asFloat();
        float blendVal = data.inputValue(blend).asFloat();
        int startFrame = data.inputValue(aStartFrame).asInt();
        MDataHandle inRotationData = data.inputValue(inRot);
        MVector inRotation = inRotationData.asVector();
        MDataHandle orderHandle = data.inputValue(rotOrder);
        short rotOrder = orderHandle.asShort();
        MEulerRotation::RotationOrder  eulerRotOrder = getEulerRotationOrder(rotOrder);
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
        outputRot.set(outputRotation.x * (1.0 - twistIntensityVal) + (outputPitchYawRoll.x * twistIntensityVal), outputRotation.y, outputRotation.z);
        MDataHandle outputDriver = data.outputValue(pitchYawRoll);
        outputDriver.set(outputPitchYawRoll.y, outputPitchYawRoll.z, outputPitchYawRoll.x);
        MAngle normalForward = setRangePair(outputPitchYawRoll.z, -30, 0.0, -0.2, 0.0);
        MDataHandle outputForward = data.outputValue(forward);
        double normalForwardDeg = normalForward.value() * RAD2DEG * forwardIntensityVal * intensityVal;
        outputForward.set(normalForwardDeg);
        MAngle normalBackward = setRangePair(outputPitchYawRoll.z, 0.0, 30.0, 0.0, 0.2);
        MDataHandle outputBackward = data.outputValue(backward);
        double normalBackwardDeg = normalBackward.value() * RAD2DEG * backwardIntensityVal * intensityVal;
        outputBackward.set(normalBackwardDeg);
        MAngle normalUpward = setRangePair(outputPitchYawRoll.y, -30, 0.0, 0.2, 0.0);
        MDataHandle outputUpward = data.outputValue(upward);
        double normalUpwardDeg = normalUpward.value() * RAD2DEG * upIntensityVal * intensityVal;
        outputUpward.set(normalUpwardDeg);
        MAngle normalDownward = setRangePair(outputPitchYawRoll.y, 0, 30.0, 0.0, -0.2);
        MDataHandle outputDownward = data.outputValue(downward);
        double normalDownwardDeg = normalDownward.value() * RAD2DEG * downwardIntensityVal * intensityVal;
        outputDownward.set(normalDownwardDeg);
        MTime currentTime = data.inputValue(aTime, &status).asTime();
        double intensity = data.inputValue(springIntensity, &status).asDouble();
        double damping = data.inputValue(springDamping, &status).asDouble();
        double stiffness = data.inputValue(springStiffness, &status).asDouble();
        double newNormalUpwardDeg;
        double newNormalBackwardDeg;
        double newNormalDownwardDeg;
        double newNormalForwardDeg;
        MAngle velocity;
        MAngle goalForce;
        double tzSpringVal;
        double tySpringVal;
        MDataHandle outputTzSpring = data.outputValue(tzSpring);
        MDataHandle outputTySpring = data.outputValue(tySpring);

        if (intensity > 0.009)
        {
            if (_initialized == false) {

                _previousUpwardPosition = normalUpwardDeg;
                _currentUpwardPosition = normalUpwardDeg;
                _previousDownwardPosition = normalDownwardDeg;
                _currentDownwardPosition = normalDownwardDeg;
                _previousForwardPosition = normalForwardDeg;
                _currentForwardPosition = normalForwardDeg;
                _previousBackwardPosition = normalBackwardDeg;
                _currentBackwardPosition = normalBackwardDeg;
                _previousTime = currentTime;
                _initialized = true;
            }
            double timeDifference = currentTime.value() - _previousTime.value();
            if (timeDifference > 1.0 || timeDifference < 0.0 || currentTime.value() < startFrame) {
                _initialized = false;
                _previousUpwardPosition = normalUpwardDeg;
                _currentUpwardPosition = normalUpwardDeg;
                _previousDownwardPosition = normalDownwardDeg;
                _currentDownwardPosition = normalDownwardDeg;
                _previousForwardPosition = normalForwardDeg;
                _currentForwardPosition = normalForwardDeg;
                _previousBackwardPosition = normalBackwardDeg;
                _currentBackwardPosition = normalBackwardDeg;
                _previousTime = currentTime;
            }
            velocity = (_currentForwardPosition.value() - _previousForwardPosition.value()) * (1.0 - damping);
            newNormalForwardDeg = _currentForwardPosition.value() + velocity.value();
            goalForce = (normalForwardDeg - newNormalForwardDeg) * stiffness;
            newNormalForwardDeg = newNormalForwardDeg + goalForce.value();
            _previousForwardPosition = _currentForwardPosition;
            _currentForwardPosition = newNormalForwardDeg;
            newNormalForwardDeg = normalForwardDeg + ((newNormalForwardDeg - normalForwardDeg) * intensity);

            velocity = (_currentBackwardPosition.value() - _previousBackwardPosition.value()) * (1.0 - damping);
            newNormalBackwardDeg = _currentBackwardPosition.value() + velocity.value();
            goalForce = (normalBackwardDeg - newNormalBackwardDeg) * stiffness;
            newNormalBackwardDeg = newNormalBackwardDeg + goalForce.value();
            _previousBackwardPosition = _currentBackwardPosition;
            _currentBackwardPosition = newNormalBackwardDeg;
            newNormalBackwardDeg = normalBackwardDeg + ((newNormalBackwardDeg - normalBackwardDeg) * intensity);

            velocity = (_currentUpwardPosition.value() - _previousUpwardPosition.value()) * (1.0 - damping);
            newNormalUpwardDeg = _currentUpwardPosition.value() + velocity.value();
            goalForce = (normalUpwardDeg - newNormalUpwardDeg) * stiffness;
            newNormalUpwardDeg = newNormalUpwardDeg + goalForce.value();
            _previousUpwardPosition = _currentUpwardPosition;
            _currentUpwardPosition = newNormalUpwardDeg;
            newNormalUpwardDeg = normalUpwardDeg + ((newNormalUpwardDeg - normalUpwardDeg) * intensity);

            velocity = (_currentDownwardPosition.value() - _previousDownwardPosition.value()) * (1.0 - damping);
            newNormalDownwardDeg = _currentDownwardPosition.value() + velocity.value();
            goalForce = (normalDownwardDeg - newNormalDownwardDeg) * stiffness;
            newNormalDownwardDeg = newNormalDownwardDeg + goalForce.value();
            _previousDownwardPosition = _currentDownwardPosition;
            _currentDownwardPosition = newNormalDownwardDeg;
            newNormalDownwardDeg = normalDownwardDeg + ((newNormalDownwardDeg - normalDownwardDeg) * intensity);

            tzSpringVal = ((newNormalUpwardDeg - normalUpwardDeg) * upIntensityVal) + ((newNormalDownwardDeg - normalDownwardDeg) * downwardIntensityVal);
            tySpringVal = ((newNormalForwardDeg - normalForwardDeg) * forwardIntensityVal) + ((newNormalBackwardDeg - normalBackwardDeg) * backwardIntensityVal);
            _previousTime = currentTime;
        }
        else
        {
            newNormalForwardDeg = normalForwardDeg;
            newNormalBackwardDeg = normalBackwardDeg;
            newNormalUpwardDeg = normalUpwardDeg;
            newNormalDownwardDeg = normalDownwardDeg;
            tzSpringVal = 0.0;
            tySpringVal = 0.0;
        }
        outputForward.set(normalForwardDeg);
        outputBackward.set(normalBackwardDeg);
        outputUpward.set(normalUpwardDeg);
        outputDownward.set(normalDownwardDeg);
        outputTzSpring.set(tzSpringVal);
        outputTySpring.set(tySpringVal);
        data.setClean(plug);
    }
    else
    {
        return MS::kUnknownParameter;
    }
    return MS::kSuccess;
}

MStatus prPairBlend::initialize()
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

    aStartFrame = nAttr.create("startFrame", "startFrame", MFnNumericData::kInt, 0);
    nAttr.setKeyable(true);
    addAttribute(aStartFrame);

    springIntensity = nAttr.create("springIntensity", "springIntensity", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springIntensity);

    springStiffness = nAttr.create("springStiffness", "springStiffness", MFnNumericData::kDouble, 0.25);
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

    aTime = uAttr.create("time", "time", MFnUnitAttribute::kTime, 0.0f);
    addAttribute(aTime);

    forward = nAttr.create("forward", "forward", MFnNumericData::kDouble, 1.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(forward);

    tzSpring = nAttr.create("tzSpring", "tzSpring", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(tzSpring);

    tySpring = nAttr.create("tySpring", "tySpring", MFnNumericData::kDouble, 0.0);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    addAttribute(tySpring);

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
    attributeAffects(springIntensity, forward);
    attributeAffects(aStartFrame, forward);
    attributeAffects(rotOrder, forward);
    attributeAffects(forwardIntensity, forward);
    attributeAffects(intensity, forward);
    attributeAffects(blend, forward);
    attributeAffects(twistIntensity, forward);

    attributeAffects(inRot, backward);
    attributeAffects(springIntensity, backward);
    attributeAffects(aStartFrame, backward);
    attributeAffects(rotOrder, backward);
    attributeAffects(backwardIntensity, backward);
    attributeAffects(intensity, backward);
    attributeAffects(blend, backward);
    attributeAffects(twistIntensity, backward);

    attributeAffects(inRot, upward);
    attributeAffects(springIntensity, upward);
    attributeAffects(aStartFrame, upward);
    attributeAffects(rotOrder, upward);
    attributeAffects(upwardIntensity, upward);
    attributeAffects(intensity, upward);
    attributeAffects(blend, upward);
    attributeAffects(twistIntensity, upward);

    attributeAffects(inRot, downward);
    attributeAffects(springIntensity, downward);
    attributeAffects(aStartFrame, downward);
    attributeAffects(rotOrder, downward);
    attributeAffects(downwardIntensity, downward);
    attributeAffects(intensity, downward);
    attributeAffects(blend, downward);
    attributeAffects(twistIntensity, downward);


    attributeAffects(springIntensity, tzSpring);
    attributeAffects(aStartFrame, tzSpring);
    attributeAffects(inRot, tzSpring);
    attributeAffects(rotOrder, tzSpring);
    attributeAffects(upwardIntensity, tzSpring);
    attributeAffects(downwardIntensity, tzSpring);
    attributeAffects(intensity, tzSpring);
    attributeAffects(blend, tzSpring);
    attributeAffects(twistIntensity, tzSpring);

    attributeAffects(springIntensity, tySpring);
    attributeAffects(aStartFrame, tySpring);
    attributeAffects(inRot, tySpring);
    attributeAffects(rotOrder, tySpring);
    attributeAffects(upwardIntensity, tySpring);
    attributeAffects(downwardIntensity, tySpring);
    attributeAffects(intensity, tySpring);
    attributeAffects(blend, tySpring);
    attributeAffects(twistIntensity, tySpring);

    return MS::kSuccess;
}

MTypeId prPairBlend::GetTypeId()
{
    return(TYPE_ID);
}

MString prPairBlend::GetTypeName()
{
    return(TYPE_NAME);
}