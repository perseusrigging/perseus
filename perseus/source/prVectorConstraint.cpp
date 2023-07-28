#include "prVectorConstraint.h"

static const MTypeId TYPE_ID = MTypeId(0x0013be01);
static const MString TYPE_NAME = "prVectorConstraint";

MObject prVectorConstraint::matrix; 
MObject prVectorConstraint::worldMatrix; 
MObject prVectorConstraint::drivenPMatrix;
MObject prVectorConstraint::rotation;
MObject prVectorConstraint::rotateX;
MObject prVectorConstraint::rotateY;
MObject prVectorConstraint::rotateZ;
MObject prVectorConstraint::offsetRotation;
MObject prVectorConstraint::offsetRotateX;
MObject prVectorConstraint::offsetRotateY;
MObject prVectorConstraint::offsetRotateZ;
MObject prVectorConstraint::thisMObj;
MObject prVectorConstraint::origVec;
MObject prVectorConstraint::aimVec;
MObject prVectorConstraint::aimVecX;
MObject prVectorConstraint::aimVecY;
MObject prVectorConstraint::aimVecZ;
MObject prVectorConstraint::origVecX;
MObject prVectorConstraint::origVecY;
MObject prVectorConstraint::origVecZ;
MObject prVectorConstraint::driverAngle;
MObject prVectorConstraint::driverTranslate;
MObject prVectorConstraint::blendAngle;
MObject prVectorConstraint::blendTranslate;
MObject prVectorConstraint::driverAngleWeight;
MObject prVectorConstraint::driverTranslateWeight;
MObject prVectorConstraint::driverMin;
MObject prVectorConstraint::driverMax;
MObject prVectorConstraint::min;
MObject prVectorConstraint::max;
MObject prVectorConstraint::springIntensity;
MObject prVectorConstraint::springStiffness;
MObject prVectorConstraint::springDamping;
MObject prVectorConstraint::aTime;
MObject prVectorConstraint::aStartFrame;
MObject prVectorConstraint::radius;
MObject prVectorConstraint::radiusPower;
MObject prVectorConstraint::falloffPower;

prVectorConstraint::prVectorConstraint() 
{
}

prVectorConstraint::~prVectorConstraint() 
{
}

void* prVectorConstraint::creator()
{ 
    return new prVectorConstraint(); 
}

MAngle setRange(const MAngle& inputVal, MAngle driverMin, MAngle driverMax, MAngle min, MAngle max)
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

prVectorConstraint::SchedulingType prVectorConstraint::schedulingType() const
{
    return kParallel;
}

MStatus prVectorConstraint::compute(const MPlug& plug, MDataBlock& data)
{
    if((plug== driverAngle) || (plug == driverTranslate))
    {
        MStatus status;
        int startFrame = data.inputValue(aStartFrame).asInt();
        MVector offsetRotateV = data.inputValue(offsetRotation, &status).asVector();
        MEulerRotation::RotationOrder  eulerRotOrder = MEulerRotation::kXYZ;
        MEulerRotation offsetRotate(offsetRotateV[0], offsetRotateV[1], offsetRotateV[2], eulerRotOrder);
        MMatrix offsetMatrix =offsetRotate.asMatrix();
        MMatrix offsetInvertMatrix=offsetMatrix.inverse();
        double minVal= data.inputValue(min, &status).asDouble()*DEG2RAD;
        double maxVal = data.inputValue(max, &status).asDouble() * DEG2RAD;
        double driverMinVal = data.inputValue(driverMin, &status).asDouble() * DEG2RAD;
        double driverMaxVal = data.inputValue(driverMax, &status).asDouble() * DEG2RAD;
        double radiusVal = data.inputValue(radius, &status).asDouble();
        CHECK_MSTATUS_AND_RETURN_IT( status );
        MVector aimVector = MVector(0.0, -1.0, 0.0);
        MMatrix childMat;
        childMat.setToIdentity();
        childMat[3][0] = aimVector[0];
        childMat[3][1] = aimVector[1];
        childMat[3][2] = aimVector[2];
        MMatrix driverMatrix = data.inputValue( matrix, &status ).asMatrix();
        CHECK_MSTATUS_AND_RETURN_IT( status );
        MTransformationMatrix driverMTMatrix = MTransformationMatrix(driverMatrix);
        MMatrix drivenPMat = data.inputValue(drivenPMatrix, &status).asMatrix();
        CHECK_MSTATUS_AND_RETURN_IT(status);
        MTransformationMatrix drivenMTPMatrix = MTransformationMatrix(drivenPMat);
        MVector offsetTranslateV = drivenMTPMatrix.getTranslation(MSpace::kWorld);
        MMatrix driverWMat = data.inputValue(worldMatrix, &status).asMatrix();
        CHECK_MSTATUS_AND_RETURN_IT(status);
        MTransformationMatrix driverMTWMatrix = MTransformationMatrix(driverWMat);
        MVector drivenTranslate = driverMTWMatrix.getTranslation(MSpace::kWorld);
        double distance = (drivenTranslate - offsetTranslateV).length();
        double globalScale[3];
        driverMTWMatrix.getScale(globalScale, MSpace::kWorld);
        double scale = (globalScale[0] + globalScale[1] + globalScale[2]) / 3.0;
        double radiusPowerVal = data.inputValue(radiusPower, &status).asDouble()* scale;
        double falloffPowerVal = data.inputValue(falloffPower, &status).asDouble();
        radiusVal *= radiusPowerVal;
        double radiusFalloffVal = radiusVal * falloffPowerVal;
        driverMTMatrix.setRotationOrientation(MQuaternion());
        MMatrix  driverRotation= driverMTMatrix.eulerRotation().asMatrix();
        MMatrix inputMatrix = childMat * driverRotation;
        MMatrix goalMAtrix = inputMatrix * offsetInvertMatrix;
        MTransformationMatrix startMTMatrix = MTransformationMatrix(goalMAtrix);
        MTransformationMatrix localMTMatrix = startMTMatrix;
        MVector endVec = localMTMatrix.getTranslation(MSpace::kWorld);
        MAngle offsetVal = setRange(0.0, driverMinVal, driverMaxVal, minVal, maxVal);
        MQuaternion eulerQuat = MQuaternion(aimVector.normal(), endVec.normal(), 1.0);
        MVector upVec = eulerQuat.asEulerRotation().asVector();
        MAngle aimVal = setRange(upVec[0], driverMinVal, driverMaxVal, minVal, maxVal);
        MAngle outputAngle;
        double outputTranslate;
        if (aimVal.value() > (offsetVal.value()))
        {
            outputAngle = aimVal.value();
        }
        else
        {
            outputAngle = 0.0;
        }
        outputTranslate = outputAngle.value();
        MAngle  blendAng = data.inputValue(blendAngle, &status).asAngle();
        double  blendTrs = data.inputValue(blendTranslate, &status).asDouble();
        double driverAngleMult = data.inputValue(driverAngleWeight, &status).asDouble();
        double driverTranslateMult = data.inputValue(driverTranslateWeight, &status).asDouble();
        double distanceVal = setRange(distance, radiusVal, (radiusVal+ radiusFalloffVal), 1.0, 0.0).value();
        outputAngle = (outputAngle.value() * driverAngleMult) * distanceVal;
        outputTranslate = (outputTranslate * driverTranslateMult) * distanceVal;

        MTime currentTime = data.inputValue(aTime, &status).asTime();
        double intensity = data.inputValue(springIntensity, &status).asDouble();
        double damping = data.inputValue(springDamping, &status).asDouble();
        double stiffness = data.inputValue(springStiffness, &status).asDouble();
        MAngle newAngle;
        double newPosition;
        if (intensity > 0.009)
        {
            if (_initialized == false) {
                _previousAngle = outputAngle;
                _currentAngle = outputAngle;
                _previousPosition = outputTranslate;
                _currentPosition = outputTranslate;
                _previousTime = currentTime;
                _initialized = true;
            }
            double timeDifference = currentTime.value() - _previousTime.value();
            if (timeDifference > 1.0 || timeDifference < 0.0 || currentTime.value() < startFrame) {
                _initialized = false;
                _previousAngle = outputAngle;
                _currentAngle = outputAngle;
                _previousPosition = outputTranslate;
                _currentPosition = outputTranslate;
                _previousTime = currentTime;
            }

            MAngle velocity = (_currentAngle.value() - _previousAngle.value()) * (1.0 - damping);
            newAngle = _currentAngle.value() + velocity.value();
            MAngle goalForce = (outputAngle.value() - newAngle.value()) * stiffness;
            newAngle = newAngle.value() + goalForce.value();

            MAngle velocityT = (_currentPosition.value() - _previousPosition.value()) * (1.0 - damping);
            newPosition = _currentPosition.value() + velocityT.value();
            MAngle goalForceT = (outputTranslate - newPosition) * stiffness;
            newPosition = newPosition + goalForceT.value();

            _previousAngle = _currentAngle;
            _currentAngle = newAngle;
            _previousPosition = _currentPosition;
            _currentPosition = newPosition;
            _previousTime = currentTime;

        newAngle = outputAngle.value() + ((newAngle.value() - outputAngle.value()) * intensity);
        newPosition = outputTranslate + ((newPosition - outputTranslate) * intensity);
            if (newAngle.value() < 0.0)
            {
                newAngle =newAngle.value() *0.5;
            }
            if (newPosition < 0.0f)
            {
                newPosition = newPosition*0.5;
            }
        }
        else
        {
            newAngle = outputAngle;
            newPosition = outputTranslate;

        }
        newAngle = blendAng.value()+ newAngle.value();
        newPosition = abs(blendTrs) + newPosition;
        MDataHandle outputAng = data.outputValue(driverAngle);
        outputAng.set(newAngle);
        double outputTrans = -newPosition;
        MDataHandle outputTran = data.outputValue(driverTranslate);
        outputTran.set(outputTrans);
        data.setClean(plug);
    }
    else
    {
        return MS::kUnknownParameter;
    }
    return MS::kSuccess;
}

MStatus prVectorConstraint::initialize()
{
    MStatus status;
    MFnNumericAttribute nAttr;
    MFnMatrixAttribute mAttr;
    MFnUnitAttribute uAttr;

    aStartFrame = nAttr.create("startFrame", "startFrame", MFnNumericData::kInt, 0);
    nAttr.setKeyable(true);
    addAttribute(aStartFrame);

    matrix = mAttr.create("matrix", "matrix", MFnMatrixAttribute::kDouble);
    mAttr.setWritable(true);
    mAttr.setStorable(true);
    mAttr.setReadable(false);
    mAttr.setConnectable(true);
    mAttr.setHidden(true);
    addAttribute(matrix);

    worldMatrix = mAttr.create("worldMatrix", "worldMatrix", MFnMatrixAttribute::kDouble);
    mAttr.setWritable(true);
    mAttr.setStorable(true);
    mAttr.setReadable(false);
    mAttr.setConnectable(true);
    mAttr.setHidden(true);
    addAttribute(worldMatrix);

    drivenPMatrix = mAttr.create("drivenParentMatrix", "drivenParentMatrix", MFnMatrixAttribute::kDouble);
    mAttr.setWritable(true);
    mAttr.setStorable(true);
    mAttr.setReadable(false);
    mAttr.setConnectable(true);
    mAttr.setHidden(true);
    addAttribute(drivenPMatrix);

    offsetRotateX = uAttr.create("offsetRotateX", "orx", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    uAttr.setHidden(true);
    addAttribute(offsetRotateX);

    offsetRotateY = uAttr.create("offsetRotateY", "ory", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    uAttr.setHidden(true);
    addAttribute(offsetRotateY);

    offsetRotateZ = uAttr.create("offsetRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    uAttr.setHidden(true);
    addAttribute(offsetRotateZ);

    offsetRotation = nAttr.create("offsetRotation", "or", offsetRotateX, offsetRotateY, offsetRotateZ);
    nAttr.setStorable(true);
    nAttr.setWritable(true);
    nAttr.setReadable(true);
    nAttr.setHidden(false);
    addAttribute(offsetRotation);
 
    driverAngle = uAttr.create("outputRotate", "outR", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    addAttribute(driverAngle);

    driverTranslate = nAttr.create("outputTranslate", "outT", MFnNumericData::kDouble, 0.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    addAttribute(driverTranslate);

    blendAngle = uAttr.create("inputRotate", "inR", MFnUnitAttribute::kAngle, 0.0);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    addAttribute(blendAngle);

    blendTranslate = nAttr.create("inputTranslate", "inT", MFnNumericData::kDouble, 0.0);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    addAttribute(blendTranslate);

    driverAngleWeight = nAttr.create("outputRotateWeight", "outRW", MFnNumericData::kDouble, 1.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(-1.0);
    nAttr.setMax(1.0);
    addAttribute(driverAngleWeight);

    driverTranslateWeight = nAttr.create("outputTranslateWeight", "outTW", MFnNumericData::kDouble, 1.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(-100.0);
    nAttr.setMax(100.0);
    addAttribute(driverTranslateWeight);

    driverMax = nAttr.create("driverMax", "driverMax", MFnNumericData::kDouble, 130.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(-180.0);
    nAttr.setMax(180.0);
    addAttribute(driverMax);

    driverMin = nAttr.create("driverMin", "driverMin", MFnNumericData::kDouble, 40.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(-180.0);
    nAttr.setMax(180.0);
    addAttribute(driverMin);

    max = nAttr.create("max", "max", MFnNumericData::kDouble, 125.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(-180.0);
    nAttr.setMax(180.0);
    addAttribute(max);

    min = nAttr.create("min", "min", MFnNumericData::kDouble, 0.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(-180.0);
    nAttr.setMax(180.0);
    addAttribute(min);

    radius = nAttr.create("radius", "radius", MFnNumericData::kDouble, 1.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0);
    addAttribute(radius);

    radiusPower = nAttr.create("radiusPower", "radiusPower", MFnNumericData::kDouble, 1.0);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(10.0);
    addAttribute(radiusPower);
    
    falloffPower = nAttr.create("falloffPower", "falloffPower", MFnNumericData::kDouble, 1.5);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setChannelBox(true);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(10.0);
    addAttribute(falloffPower);

    springIntensity = nAttr.create("springIntensity", "springIntensity", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springIntensity);
    attributeAffects(springIntensity, driverAngle);
    attributeAffects(springIntensity, driverTranslate);

    springStiffness = nAttr.create("springStiffness", "springStiffness", MFnNumericData::kDouble, 0.25);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springStiffness);
    attributeAffects(springStiffness, driverAngle);
    attributeAffects(springStiffness, driverTranslate);

    springDamping = nAttr.create("springDamping", "springDamping", MFnNumericData::kDouble, 0.01);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setChannelBox(true);
    addAttribute(springDamping);
    attributeAffects(springDamping, driverAngle);
    attributeAffects(springDamping, driverTranslate);

    aTime = uAttr.create("time", "time", MFnUnitAttribute::kTime, 0.0f);
    addAttribute(aTime);

    attributeAffects(aTime, driverAngle);
    attributeAffects(aTime, driverTranslate);
    attributeAffects(blendTranslate, driverAngle);
    attributeAffects(blendAngle, driverAngle);
    attributeAffects(driverAngleWeight, driverAngle);
    attributeAffects(matrix, driverAngle); 
    attributeAffects(worldMatrix, driverAngle);
    attributeAffects(driverMax, driverAngle);
    attributeAffects(driverMin, driverAngle);
    attributeAffects(max, driverAngle);
    attributeAffects(min, driverAngle);
    attributeAffects(offsetRotation, driverAngle);
    attributeAffects(drivenPMatrix, driverAngle);
    attributeAffects(aStartFrame, driverAngle);
    attributeAffects(radius, driverAngle);
    attributeAffects(radiusPower, driverAngle);
    attributeAffects(falloffPower, driverAngle);

    attributeAffects(blendTranslate, driverTranslate);
    attributeAffects(blendAngle, driverTranslate);
    attributeAffects(driverAngleWeight, driverTranslate);
    attributeAffects(driverTranslateWeight, driverTranslate);
    attributeAffects(matrix, driverTranslate);
    attributeAffects(worldMatrix, driverTranslate);
    attributeAffects(driverMax, driverTranslate);
    attributeAffects(driverMin, driverTranslate);
    attributeAffects(max, driverTranslate);
    attributeAffects(min, driverTranslate);
    attributeAffects(offsetRotation, driverTranslate);
    attributeAffects(drivenPMatrix, driverTranslate);
    attributeAffects(aStartFrame, driverTranslate);
    attributeAffects(radius, driverTranslate);
    attributeAffects(radiusPower, driverTranslate);
    attributeAffects(falloffPower, driverTranslate);

    return MS::kSuccess;
}

MTypeId prVectorConstraint::GetTypeId()
{
    return(TYPE_ID);
}

MString prVectorConstraint::GetTypeName()
{
    return(TYPE_NAME);
}