#include "prSquash.h"
MString nodeTypeMT("prSquash");
static const MTypeId TYPE_IDMT = MTypeId(0x0013be00);
static const MString TYPE_NAMEMT = "prSquash";


#define CHECK_ERROR(stat, msg) \
    if (!stat) { \
        MString errorMsg("[" + nodeTypeMT + "] " + __FILE__ + " " + __LINE__ + ": " msg); \
        MGlobal::displayError(errorMsg); \
        cerr << errorMsg << endl; \
        return MS::kFailure; \
    }

#define CHECK_ERROR_NO_RETURN(stat, msg) \
    if (!stat) { \
        MString errorMsg("[" + nodeTypeMT + "] " + __FILE__ + " " + __LINE__ + ": " msg); \
        MGlobal::displayError(errorMsg); \
        cerr << errorMsg << endl; \
    }

// The user Context
const double EPSILON = 0.0001;


typedef struct
{
    int start;
    int end;
    MPointArray* points;
    MPointArray* originalPoints;
    MFloatArray* weights;
    float env;
    int width;
    int numTasks;
    float xMid;
    float yMid;
    float zMid;
    int octaves;
    float lacunarity;
    float persistence;
    MMatrix* upLocRotsInv;
    MMatrix* upLocRots;
    MPoint bBoxMinB;
    MPoint bBoxMaxB;
    float initialLength;
    int rampMemValue;
    double twistVals;
    MVector minusSquashVec;
    double minusUpLocPos;
    float lengthValSquash;
    float lengthValStretch;
    MFloatArray expandMap;
    MFloatArray shrinkMap;
    MFloatArray stretchMap;
    MFloatArray squashMap;
    float  bendStrengthVal;
    float bendXTemp;
    float bendZTemp;
    float localGlobalWeightVal;
    float xStrength;
    float zStrength;
    std::array<float, 10000> arrExpandMTF;
    std::array<float, 10000> arrShrinkMTF;
    std::array<float, 10000> arrBendMTF;
    std::array<float, 10000> arrDeformerMTF;
    std::array<float, 10000> arrTwistMTF;
    std::array<float, 10000> arrStretchMTF;
    std::array<float, 10000> arrSquashMTF;
} SharedData;

typedef struct
{
    int id;
    SharedData* sharedData;
} ThreadData;

MObject prSquash::aPerGeometry;
MObject prSquash::locatorWorldSpace;
MObject prSquash::locatorUpWorldSpace;
MObject prSquash::expandRamp;
MObject prSquash::expandStrength;
MObject prSquash::shrinkRamp;
MObject prSquash::shrinkStrength;
MObject prSquash::aexpandMap;
MObject prSquash::ashrinkMap;
MObject prSquash::astretchMap;
MObject prSquash::asquashMap;
MObject prSquash::bendStrength;
MObject prSquash::twistStrength;
MObject prSquash::twist;
MObject prSquash::aWorldMatrix;
int  prSquash::isInitialized;
MObject prSquash::bendRamp;
MObject prSquash::deformerRamp;
MObject prSquash::twistRamp;
MObject prSquash::stretchRamp;
MObject prSquash::squashRamp;
MObject prSquash::boundingMin;
MObject prSquash::boundingMax;
MObject prSquash::stretchBendWeight;
MObject prSquash::numTasks;
MStringArray prSquash::menuItems;
MObject prSquash::deformerArray;
MObject prSquash::twistArray;
MObject prSquash::expandArray;
MObject prSquash::shrinkArray;
MObject prSquash::bendArray;
MObject prSquash::stretchArray;
MObject prSquash::squashArray;
MObject prSquash::xStrength;
MObject prSquash::zStrength;
MObject prSquash::positionArray;
MObject prSquash::localGlobalWeight;
MObject prSquash::initialHeight;

//constructor
prSquash::prSquash()
{
    //init thread pool
    cerr << "[" << TYPE_NAMEMT << "] Initializing thread pool" << endl;
    MStatus stat = MThreadPool::init();
    CHECK_ERROR_NO_RETURN(stat, "Unable to create thread pool\n");
}

//destructor
prSquash::~prSquash()
{
    //release thread pool
    cerr << "[" << TYPE_NAMEMT << "] Releasing thread pool" << endl;
    MThreadPool::release();
}

//main task method for a single thread
MThreadRetVal threadTask(void* data)
{
    ThreadData* threadData = static_cast<ThreadData*>(data);
    const SharedData* sharedData = threadData->sharedData;
    const int sharedStart = sharedData->start;
    const int sharedEnd = sharedData->end;
    MPointArray* sharedPoints = sharedData->points;
    MPointArray* originalPoints = sharedData->originalPoints;
    const MFloatArray* sharedWeights = sharedData->weights;
    std::array<float, 10000> arrExpandMTT= sharedData->arrExpandMTF;
    std::array<float, 10000> arrShrinkMTT = sharedData->arrShrinkMTF;
    std::array<float, 10000> arrBendMTT = sharedData->arrBendMTF;
    std::array<float, 10000> arrDeformerMTT = sharedData->arrDeformerMTF;
    std::array<float, 10000> arrTwistMTT = sharedData->arrTwistMTF;
    std::array<float, 10000> arrStretchMTT = sharedData->arrStretchMTF;
    std::array<float, 10000> arrSquashMTT = sharedData->arrSquashMTF;
    const MFloatArray& expandMap = sharedData->expandMap;
    const MFloatArray& shrinkMap = sharedData->shrinkMap;
    const MFloatArray& stretchMap = sharedData->stretchMap;
    const MFloatArray& squashMap = sharedData->squashMap;
    const float sharedEnv = sharedData->env;
    const int sharedWidth = sharedData->width;
    const float xMid = sharedData->xMid;
    const float yMid = sharedData->yMid;
    const float zMid = sharedData->zMid;
    const MMatrix* upLocRotsInv = sharedData->upLocRotsInv;
    const MMatrix* upLocRots = sharedData->upLocRots;
    const MPoint bBoxMinB = sharedData->bBoxMinB;
    const MPoint bBoxMaxB = sharedData->bBoxMaxB;
    const float initialLength = sharedData->initialLength;
    const int rampMemValue = sharedData->rampMemValue;
    const double twistVals = sharedData->twistVals;
    const MVector minusSquashVec = sharedData->minusSquashVec;
    const double minusUpLocPos = sharedData->minusUpLocPos;
    const float lengthValSquash = sharedData->lengthValSquash;
    const float lengthValStretch = sharedData->lengthValStretch;
    const float  bendStrengthVal = sharedData->bendStrengthVal;
    const float  localGlobalWeightVal = sharedData->localGlobalWeightVal;
    const float bendXTemp = sharedData->bendXTemp;
    const float bendZTemp = sharedData->bendZTemp;
    const float xStrength = sharedData->xStrength;
    const float zStrength = sharedData->zStrength;
    //get range of ids to work on
    const int threadStartId = sharedStart + threadData->id * sharedWidth;
    const int threadEndId = threadStartId + sharedWidth;
    float noiseInput[3];
    float envTimesWeight;
    MPoint* pos;
    MPoint* originalPos;
    int i;
    float findRamp;
    float disFromLow;
    int findRampInt;
    float expandValue;
    float shrinkValue;
    float bendValue;
    float deformerValue;
    float twistValue;
    float twistTimesWeight;
    float stretchValue;
    float squashValue;
    float cosTwist;
    float sinTwist;
    float posTwist;
    float squashStretchVal;
    MVector squashVec;
    float envBendStrength;
    float bendX;
    float bendZ;
    float tempY;
    float tempX;
    float tempZ;
    float bendVal;
    float cosBend;
    float sinBend;
    float scale;
    float bendPI;
    float posX;
    float posY;
    float posZ;
    for (i = threadStartId; i < threadEndId && i <= sharedEnd; ++i)
    {
        pos = &((*sharedPoints)[i]);
        pos->x -= xMid;
        pos->y -= yMid;
        pos->z -= zMid;
        *pos *= *upLocRotsInv;
        originalPos = &((*originalPoints)[i]);
        originalPos->x -= xMid;
        originalPos->y -= yMid;
        originalPos->z -= zMid;
        *originalPos *= *upLocRotsInv;
        envTimesWeight = sharedEnv * (*sharedWeights)[i];
        tempY = originalPos->y* (1 - localGlobalWeightVal) + pos->y* localGlobalWeightVal;
        findRamp = (tempY - bBoxMinB.y);
        disFromLow = pos->y * localGlobalWeightVal +originalPos->y * (1 - localGlobalWeightVal) - bBoxMinB.y; 
        findRamp /= initialLength;
        findRampInt = int(findRamp * (rampMemValue - 1));
        if (findRampInt > (rampMemValue - 1))
        {
            findRampInt = (rampMemValue - 1);
        }
        else if (findRampInt < 0)
        {
            findRampInt = 0;
        }
        expandValue = arrShrinkMTT[findRampInt];
        shrinkValue = arrExpandMTT[findRampInt];
        bendValue = arrBendMTT[findRampInt];
        deformerValue = arrDeformerMTT[findRampInt];
        twistValue = arrTwistMTT[findRampInt];
        stretchValue = arrStretchMTT[findRampInt];
        squashValue = arrSquashMTT[findRampInt];
        twistTimesWeight = twistVals * findRamp * deformerValue * twistValue;
        twistTimesWeight *= envTimesWeight;
        cosTwist = cos(twistTimesWeight);
        sinTwist = sin(twistTimesWeight);
        posTwist = (pos->x) * cosTwist;
        posTwist -= ((pos->z) * sinTwist);
        pos->z = (pos->z) * cosTwist;
        pos->z += (pos->x) * sinTwist;
        pos->x = posTwist;
        tempY = pos->y;
        pos->x = pos->x;
        pos->z = pos->z;
        // stretch function 
        squashVec.x = pos->x - minusSquashVec.x;
        squashVec.y = 0.0 - minusSquashVec.y;
        squashVec.z = pos->z - minusSquashVec.z;
        if (minusUpLocPos  > EPSILON)
        { 
        pos->y += minusUpLocPos * disFromLow * envTimesWeight * deformerValue * stretchMap[i] * stretchValue;
        }
        else
        {
            pos->y += minusUpLocPos * disFromLow * envTimesWeight * deformerValue * squashMap[i] * squashValue;
        }
        // squash function 
        squashStretchVal = lengthValSquash * expandValue * expandMap[i] * deformerValue;
        squashStretchVal += lengthValStretch * shrinkValue * shrinkMap[i] * deformerValue;
        squashStretchVal *= envTimesWeight;
        pos->x += (squashVec.x) * squashStretchVal * xStrength;
        pos->z += (squashVec.z ) * squashStretchVal * zStrength;
        envBendStrength = envTimesWeight * bendStrengthVal * deformerValue;
        bendX = bendXTemp * envBendStrength;
        bendZ = bendZTemp * envBendStrength;
        tempY = pos->y;
        tempX = pos->x;
        tempZ = pos->z;

        if (abs(bendX) > EPSILON)
        {
            bendVal = pos->y - bBoxMinB.y + (bendValue - 1) * findRamp * initialLength;
            bendVal *= bendX;
            scale = 1.0f / bendX;
            bendPI = (float)PI - bendVal;
            cosBend = cos(bendPI);
            sinBend = sin(bendPI);
            posX = scale * cosBend;
            posX += scale;
            posX -= pos->x * cosBend;
            posY = scale * sinBend;
            posY -= pos->x * sinBend;
            pos->x += (posX - pos->x);
            posY += bBoxMinB.y - (bendValue - 1) * findRamp * initialLength;
            pos->y += (posY - pos->y) * envTimesWeight * 1.0;
        }
        else
        {
            pos->y = tempY;
            pos->x = tempX;
        }

        tempY = pos->y;
        if (abs(bendZ) > EPSILON)
        {
            bendVal = pos->y - bBoxMinB.y + (bendValue - 1) * findRamp * initialLength;
            bendVal *= bendZ;
            scale = 1.0f / bendZ;
            bendPI = (float)PI - bendVal;
            cosBend = cos(bendPI);
            sinBend = sin(bendPI);
            posZ = scale * cosBend;
            posZ += scale;
            posZ -= (pos->z * cosBend);
            posY = scale * sinBend;
            posY -= pos->z * sinBend;
            posY += bBoxMinB.y - (bendValue - 1) * findRamp * initialLength;
            pos->z += posZ - pos->z;
            pos->y += (posY - pos->y) * envTimesWeight;
        }
        else
        {
            pos->y = tempY;
            pos->z = tempZ;
        }
        //convert back to local space
        * pos *= *upLocRots;
        pos->x += xMid;
        pos->y += yMid;
        pos->z += zMid;
    }
    return static_cast<MThreadRetVal>(0);
}
//task creation and execution method
void createTasksAndExecute(void* data, MThreadRootTask* root)
{
    SharedData* sharedData = static_cast<SharedData*>(data);

    //store local variables
    const int numTasks = sharedData->numTasks;

    //create array to store thread data for each task
    ThreadData* threadData = new ThreadData[numTasks];

    //create tasks
    int i;
    for (i = 0; i < numTasks; ++i)
    {
        threadData[i].id = i;
        threadData[i].sharedData = sharedData;
        MThreadPool::createTask(threadTask, static_cast<void*>(&threadData[i]), root);
    }

    //execute tasks in parallel region and wait for all to finish
    MThreadPool::executeAndJoin(root);

    //delete array memory
    delete[] threadData;
}




//main deform method
MStatus prSquash::deform(MDataBlock& dataBlock,
    MItGeometry& geomIter,
    const MMatrix& localToWorldMat,
    unsigned int multiIndex)
{
    MStatus stat = MS::kSuccess;
    MDataHandle envDataHandle = dataBlock.inputValue(envelope, &stat);
    CHECK_ERROR(stat, "Unable to get envelope data handle\n");
    float env = envDataHandle.asFloat();
    if (EPSILON >= env)
    {
        return stat;
    }

    int  rampMemValue = 10000;
    MDataHandle locatorWorldSpaceDataHandle = dataBlock.inputValue(locatorWorldSpace, &stat);
    CHECK_ERROR(stat, "Unable to get locatorWorldSpace data handle\n");
    MMatrix locatorWorldSpaceMat = locatorWorldSpaceDataHandle.asMatrix();

    MDataHandle locatorUpWorldSpaceDataHandle = dataBlock.inputValue(locatorUpWorldSpace, &stat);
    CHECK_ERROR(stat, "Unable to get locatorUpWorldSpace data handle\n");
    MMatrix locatorUpWorldSpaceMat = locatorUpWorldSpaceDataHandle.asMatrix();

    MObject oThis = thisMObject();
    MRampAttribute curveExpandAttribute(oThis, shrinkRamp, &stat);
    CHECK_ERROR(stat, "Unable to get speed data handle\n");
    float rampShrinkPosition = 0.25f, curveSpeedRampValue;

    MRampAttribute curveShrinkAttribute(oThis, expandRamp, &stat);
    CHECK_ERROR(stat, "Unable to get scale data handle\n");
    float rampExpandPosition = 0.25f, curveScaleRampValue;

    MRampAttribute curveBendAttribute(oThis, bendRamp, &stat);
    CHECK_ERROR(stat, "Unable to get bend data handle\n");
    float bendRampition = 0.25f, curveBendRampValue;

    MRampAttribute curveTwistAttribute(oThis, twistRamp, &stat);
    CHECK_ERROR(stat, "Unable to get twist data handle\n");
    float twistRampition = 0.25f, curveTwistRampValue;

    MRampAttribute curveDeformerAttribute(oThis, deformerRamp, &stat);
    CHECK_ERROR(stat, "Unable to get stretch data handle\n");
    float deformerRampPosition = 0.25f, curveDeformerRampValue;

    MRampAttribute curveStretchAttribute(oThis, stretchRamp, &stat);
    CHECK_ERROR(stat, "Unable to get stretch data handle\n");
    float stretchRampPosition = 0.25f, curveStretchRampValue;

    MRampAttribute curveSquashAttribute(oThis, squashRamp, &stat);
    CHECK_ERROR(stat, "Unable to get stretch data handle\n");
    float squashRampPosition = 0.25f, curveSquashRampValue;

    MDataHandle expandStrengthData = dataBlock.inputValue(expandStrength, &stat);
    CHECK_ERROR(stat, "Unable to get expandStrength data handle\n");
    float expandStrengthVal = expandStrengthData.asFloat();
 
    MDataHandle shrinkStrengthData = dataBlock.inputValue(shrinkStrength, &stat);
    CHECK_ERROR(stat, "Unable to get shrinkStrength data handle\n");
    float shrinkStrengthVal = shrinkStrengthData.asFloat();
    shrinkStrengthVal *= 0.5;
  
    MDataHandle stretchBendWeightData = dataBlock.inputValue(stretchBendWeight, &stat);
    CHECK_ERROR(stat, "Unable to get stretchBendWeight data handle\n");
    float stretchBendWeightVal = stretchBendWeightData.asFloat();

    MDataHandle localGlobalWeightData = dataBlock.inputValue(localGlobalWeight, &stat);
    CHECK_ERROR(stat, "Unable to get localGlobalWeight data handle\n");
    float localGlobalWeightVal = localGlobalWeightData.asFloat();

    float  bendStrengthVal = dataBlock.inputValue(bendStrength, &stat).asFloat();
    float  twistStrengthVal = dataBlock.inputValue(twistStrength, &stat).asFloat();
    MAngle  twistVal = dataBlock.inputValue(twist, &stat).asAngle();
    bendStrengthVal *= 2;

    float  xStrengthVal = dataBlock.inputValue(xStrength, &stat).asFloat();
    float  zStrengthVal = dataBlock.inputValue(zStrength, &stat).asFloat();
    float envelopeValue = dataBlock.inputValue(envelope).asFloat();


    MArrayDataHandle expandArrayDataHandle = dataBlock.inputValue(expandArray, &stat);
    CHECK_ERROR(stat, "Unable to get expandArray data handle\n");

    MArrayDataHandle shrinkArrayDataHandle = dataBlock.inputValue(shrinkArray, &stat);
    CHECK_ERROR(stat, "Unable to get shrinkArray data handle\n");

    MArrayDataHandle twistArrayDataHandle = dataBlock.inputValue(twistArray, &stat);
    CHECK_ERROR(stat, "Unable to get twistArray data handle\n");

    MArrayDataHandle bendArrayDataHandle = dataBlock.inputValue(bendArray, &stat);
    CHECK_ERROR(stat, "Unable to get bendArray data handle\n");

    MArrayDataHandle deformerArrayDataHandle = dataBlock.inputValue(deformerArray, &stat);
    CHECK_ERROR(stat, "Unable to get deformerArray data handle\n");


    MArrayDataHandle stretchArrayDataHandle = dataBlock.inputValue(stretchArray, &stat);
    CHECK_ERROR(stat, "Unable to get stretchArray data handle\n");

    MArrayDataHandle squashArrayDataHandle = dataBlock.inputValue(squashArray, &stat);
    CHECK_ERROR(stat, "Unable to get squashArray data handle\n");

    MDataHandle boundingMinDataHandle = dataBlock.inputValue(boundingMin, &stat);
    CHECK_ERROR(stat, "Unable to get boundingMin data handle\n");

    MPoint boundingMinValB = boundingMinDataHandle.asFloat3();

    MDataHandle boundingMaxDataHandle = dataBlock.inputValue(boundingMax, &stat);
    CHECK_ERROR(stat, "Unable to get boundingMax data handle\n");
    MPoint boundingMaxValB = boundingMaxDataHandle.asFloat3();

    //get attribute values
    MDataHandle numTasksDataHandle = dataBlock.inputValue(numTasks, &stat);
    CHECK_ERROR(stat, "Unable to get numTasks data handle\n");
    int numTasks = numTasksDataHandle.asInt();
    MPoint bBoxMin = boundingMinValB;
    MPoint bBoxMax = boundingMaxValB;

    ////// define variables
    float length;
    float weight;
    MPoint pos;
    float noiseInput[3];
    float envTimesWeight;
    float disFromLow;
    MVector normalVec;
    MVector squashVec;
    float initialLength = 1.0;
    float tempY;
    float lengthVal;
    int i = 0;
    float  twistTimesWeight;
    float cosTwist;
    float sinTwist;
    float posTwist;
    float lengthValSquash;
    float lengthValStretch;
    double minusUpLocPos;
    MVector minusSquashVec;
    float findRamp;
    float bendX;
    float bendZ;
    float bendVal;
    float cosBend;
    float sinBend;
    float scale;
    float posX;
    float posZ;
    float posY;
    float bendXTemp;
    float bendZTemp;
    int findRampInt = 0;
    float tempX;
    float tempZ;
    float squashStretchVal;
    float envBendStrength;
    float bendPI;
    int ii;
    float angleValX;
    float angleValZ;
    float xMid;
    float yMid;
    float zMid;
    MMatrix localToWorldMatRev;
    MVector upLocPos;
    MVector lowLocPos;
    double twistVals;
    float expandValue;
    float shrinkValue;
    float bendValue;
    float deformerValue;
    float twistValue;
    float stretchValue;
    float squashValue;
    MDGModifier Modifier;
    float  rampUnit = 1.0 / float(rampMemValue);
    MObject thisObj = thisMObject();
    MFnDependencyNode thisFn(thisObj);
    MArrayDataHandle hGeo = dataBlock.inputArrayValue(aPerGeometry);
    stat = JumpToElement(hGeo, multiIndex);
    CHECK_MSTATUS_AND_RETURN_IT(stat);
    MDataHandle hPerGeometry = hGeo.inputValue();
    MMatrix matrix = hPerGeometry.child(aWorldMatrix).asMatrix();
    MArrayDataHandle positionArrayDataHandle = hPerGeometry.child(positionArray);
    //get current object name
    MString thisObjName = thisFn.name(&stat);
    int boolTest = 0;
    if (!initialized_[multiIndex]) {
        // Initialize the point states
        initialized_[multiIndex] = true;
        dirtyMap_[multiIndex] = true;
        _rampBool = 0;
    }
        expandValue = 0.0;
        shrinkValue = 0.0;
        bendValue = 0.0;
        deformerValue = 0.0;
        twistValue = 0.0;
        stretchValue = 0.0;
        squashValue = 0.0;

        float a;
        for (ii = 0; ii < 10; ii++)
        {
            curveExpandAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            expandValue+= a;

            curveShrinkAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            shrinkValue+= a;

            curveBendAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            bendValue += a;

            curveDeformerAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            deformerValue+= a;

            curveTwistAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            twistValue+= a;

            curveStretchAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            stretchValue += a;

            curveSquashAttribute.getValueAtPosition((ii * 0.1), a, &stat);
            squashValue += a;
        }
 
        float rampBool = expandValue + shrinkValue + bendValue + deformerValue + twistValue + rampUnit + stretchValue + squashValue;
        if (rampBool != _rampBool)
        {
            _rampBool = rampBool;
            for (ii = 0; ii < rampMemValue; ii++)
            {
                curveExpandAttribute.getValueAtPosition((ii * rampUnit), expandValue, &stat);
                arrExpandMTF[ii]= expandValue;

                curveShrinkAttribute.getValueAtPosition((ii * rampUnit), shrinkValue, &stat);
                arrShrinkMTF[ii] = shrinkValue;

                curveBendAttribute.getValueAtPosition((ii * rampUnit), bendValue, &stat);
                arrBendMTF[ii] = bendValue;

                curveDeformerAttribute.getValueAtPosition((ii * rampUnit), deformerValue, &stat);
                arrDeformerMTF[ii] = deformerValue;

                curveTwistAttribute.getValueAtPosition((ii * rampUnit), twistValue, &stat);
                arrTwistMTF[ii] = twistValue;

                curveStretchAttribute.getValueAtPosition((ii * rampUnit), stretchValue, &stat);
                arrStretchMTF[ii] = stretchValue;

                curveSquashAttribute.getValueAtPosition((ii * rampUnit), squashValue, &stat);
                arrSquashMTF[ii] = squashValue;

                stat = JumpToElement(expandArrayDataHandle, ii);
                expandArrayDataHandle.inputValue().setFloat(expandValue);

                stat = JumpToElement(shrinkArrayDataHandle, ii);
                shrinkArrayDataHandle.inputValue().setFloat(shrinkValue);

                stat = JumpToElement(twistArrayDataHandle, ii);
                twistArrayDataHandle.inputValue().setFloat(twistValue);

                stat = JumpToElement(bendArrayDataHandle, ii);
                bendArrayDataHandle.inputValue().setFloat(bendValue);

                stat = JumpToElement(deformerArrayDataHandle, ii);
                deformerArrayDataHandle.inputValue().setFloat(deformerValue);

                stat = JumpToElement(stretchArrayDataHandle, ii);
                stretchArrayDataHandle.inputValue().setFloat(stretchValue);

                stat = JumpToElement(squashArrayDataHandle, ii);
                squashArrayDataHandle.inputValue().setFloat(squashValue);
            }

        }


    MFloatArray& weights = weights_[multiIndex];
    MIntArray& membership = membership_[multiIndex];
    MFloatArray& expandMap = expandMap_[multiIndex];
    MFloatArray& shrinkMap = shrinkMap_[multiIndex];
    MFloatArray& stretchMap = stretchMap_[multiIndex];
    MFloatArray& squashMap = squashMap_[multiIndex];

    if (dirtyMap_[multiIndex] || geomIter.count() != membership.length()) {
        expandMap.setLength(geomIter.count());
        shrinkMap.setLength(geomIter.count());
        stretchMap.setLength(geomIter.count());
        squashMap.setLength(geomIter.count());
        membership.setLength(geomIter.count());
        weights.setLength(geomIter.count());
        MArrayDataHandle hexpandMap = hPerGeometry.child(aexpandMap);
        MArrayDataHandle hshrinkMap = hPerGeometry.child(ashrinkMap);
        MArrayDataHandle hstretchMap = hPerGeometry.child(astretchMap);
        MArrayDataHandle hsquashMap = hPerGeometry.child(asquashMap);
        ii = 0;
        for (geomIter.reset(); !geomIter.isDone(); geomIter.next(), ii++) {
            // Squash map
            stat = JumpToElement(hexpandMap, geomIter.index());
            CHECK_MSTATUS_AND_RETURN_IT(stat);
            expandMap[ii] = hexpandMap.inputValue().asFloat();
            expandMap[ii] = expandMap[ii];

            // Shrink map
            stat = JumpToElement(hshrinkMap, geomIter.index());
            CHECK_MSTATUS_AND_RETURN_IT(stat);
            shrinkMap[ii] = hshrinkMap.inputValue().asFloat();
            shrinkMap[ii] = shrinkMap[ii];

            // Stretch map
            stat = JumpToElement(hstretchMap, geomIter.index());
            CHECK_MSTATUS_AND_RETURN_IT(stat);
            stretchMap[ii] = hstretchMap.inputValue().asFloat();
            stretchMap[ii] = stretchMap[ii];

            // Squash map
            stat = JumpToElement(hsquashMap, geomIter.index());
            CHECK_MSTATUS_AND_RETURN_IT(stat);
            squashMap[ii] = hsquashMap.inputValue().asFloat();
            squashMap[ii] = squashMap[ii];

            // Weight map
            membership[ii] = geomIter.index();
            weights[ii] = weightValue(dataBlock, multiIndex, geomIter.index());
        }
        dirtyMap_[multiIndex] = false;
    }
    //////////////
    //precompute some transformation matrices
    localToWorldMatRev = localToWorldMat.inverse();
    upLocPos = MTransformationMatrix(locatorUpWorldSpaceMat).getTranslation(MSpace::kTransform);

    initialLength = bBoxMax.y - bBoxMin.y;

    upLocPos.y += EPSILON;
    float origUpLocY = upLocPos.y;
    length = upLocPos.y;
    lengthVal = (length / initialLength);
    expandStrengthVal = expandStrengthVal;
    shrinkStrengthVal = shrinkStrengthVal;
    lengthValStretch = std::max(0.0f, std::min(lengthVal, lengthVal));
    lengthValStretch *= (-shrinkStrengthVal);
    lengthValSquash = std::max(-lengthVal, std::min(lengthVal, 0.0f));
    lengthValSquash *= expandStrengthVal;
    squashVec = MVector(0, 1, 0);
    minusUpLocPos = upLocPos.y / initialLength;
    // initilize bend variables
    bendStrengthVal = (bendStrengthVal);
    length = std::max(0.02f, std::min(float(upLocPos.y), float(upLocPos.y)));
    angleValX = upLocPos.x / (initialLength + length * stretchBendWeightVal);
    bendX = angleValX * (1.0 / (length * stretchBendWeightVal + initialLength));
    angleValZ = upLocPos.z / (initialLength + length * stretchBendWeightVal);
    bendZ = angleValZ * (1.0 / (length * stretchBendWeightVal + initialLength));
    bendXTemp = bendX;
    bendZTemp = bendZ;
    // initilize twist variables
    twistVals = twistVal.value() * (-twistStrengthVal);
    xMid = (bBoxMax.x + bBoxMin.x) * 0.5;
    zMid = (bBoxMax.z + bBoxMin.z) * 0.5;
    yMid = (bBoxMax.y + bBoxMin.y) * 0.5;
    MPoint bBoxMinB;
    MPoint bBoxMaxB;
    bBoxMinB.x = bBoxMin.x - xMid;
    bBoxMinB.y = bBoxMin.y - yMid;
    bBoxMinB.z = bBoxMin.z - zMid;
    bBoxMaxB.x = bBoxMax.x - xMid;
    bBoxMaxB.y = bBoxMax.y - yMid;
    bBoxMaxB.z = bBoxMax.z - zMid;
    minusSquashVec = MVector((bBoxMaxB.x + bBoxMinB.x) * 0.5, 0.0, (bBoxMaxB.z + bBoxMinB.z) * 0.5);

    MMatrix childMat;
    childMat.setToIdentity();
    childMat[3][0] = xMid;
    childMat[3][1] = yMid;
    childMat[3][2] = zMid;
    double3 rot;
    MMatrix upLocRots = locatorWorldSpaceMat * localToWorldMat.inverse();
    upLocRots *= childMat.inverse();
    MMatrix upLocRotsInv = upLocRots.inverse();
    MPointArray refPoints;
    unsigned int numPoint;
    MPointArray finalPoints;
    geomIter.allPositions(refPoints);
    int array = positionArrayDataHandle.elementCount();
    numPoint = refPoints.length();
    i = 0;
    int currentCount = geomIter.count();
    MPoint tempPos;
    if (array < 2 || array != currentCount)
    {
        refPointss_[multiIndex] = refPoints;
        for (i == 0; i < numPoint; i++)
        {
            tempPos = refPointss_[multiIndex][i] * localToWorldMat;
            stat = JumpToElement(positionArrayDataHandle, i);
            positionArrayDataHandle.inputValue().set3Double(tempPos.x, tempPos.y, tempPos.z);
        }
    }
    finalPoints.setLength(numPoint);
    MPoint test = MPoint(0, 0, 0, 1);
    float initialHeightVal=(test* localToWorldMat)[1];
    for (i == 0; i < numPoint; i++)
    {
        stat = JumpToElement(positionArrayDataHandle, i);
        finalPoints[i] = positionArrayDataHandle.inputValue().asDouble3();
        finalPoints[i].y += upLocRots[3][1]+ initialHeightVal;
    }
    //read all points
    MPointArray points;
    geomIter.allPositions(points);
    int numPoints = points.length();

    //pack data into a struct
    SharedData sharedData;
    sharedData.start = 0;
    sharedData.end = points.length() - 1;
    sharedData.points = &points;
    sharedData.originalPoints = &finalPoints;
    sharedData.weights = &weights;
    sharedData.numTasks = numTasks;
    sharedData.width = static_cast<int>(std::ceil((sharedData.end - sharedData.start + 1.0) / numTasks));
    sharedData.env = envelopeValue;
    sharedData.xMid = xMid;
    sharedData.yMid = yMid;
    sharedData.zMid = zMid;
    sharedData.upLocRotsInv = &upLocRotsInv;
    sharedData.upLocRots = &upLocRots;
    sharedData.bBoxMinB = bBoxMinB;
    sharedData.bBoxMaxB = bBoxMaxB;
    sharedData.initialLength = initialLength;
    sharedData.rampMemValue = rampMemValue;
    sharedData.twistVals = twistVals;
    sharedData.minusSquashVec = minusSquashVec;
    sharedData.minusUpLocPos = minusUpLocPos;
    sharedData.lengthValSquash = lengthValSquash;
    sharedData.lengthValStretch = lengthValStretch;
    sharedData.expandMap = expandMap;
    sharedData.shrinkMap = shrinkMap;
    sharedData.stretchMap = stretchMap;
    sharedData.squashMap = squashMap;
    sharedData.bendStrengthVal = bendStrengthVal;
    sharedData.bendXTemp = bendXTemp;
    sharedData.bendZTemp = bendZTemp;
    sharedData.localGlobalWeightVal = localGlobalWeightVal;
    sharedData.xStrength = xStrengthVal;
    sharedData.zStrength = zStrengthVal;
    sharedData.arrExpandMTF = arrExpandMTF;
    sharedData.arrShrinkMTF = arrShrinkMTF;
    sharedData.arrBendMTF = arrBendMTF;
    sharedData.arrDeformerMTF = arrDeformerMTF;
    sharedData.arrTwistMTF = arrTwistMTF;
    sharedData.arrStretchMTF = arrStretchMTF;
    sharedData.arrSquashMTF = arrSquashMTF;
    //create new parallel region and start off the multi-threading functions
    MThreadPool::newParallelRegion(createTasksAndExecute, static_cast<void*>(&sharedData));
    //set all points
    geomIter.setAllPositions(points);
    return stat;
}

//accessory locator setup method
MStatus prSquash::accessoryNodeSetup(MDagModifier& dagMod)
{
    MFnNumericAttribute nAttr;
    MStatus stat = MS::kSuccess;
    MObject thisObj = thisMObject();

    MSelectionList selectList;
    MGlobal::getActiveSelectionList(selectList);
    MFnDependencyNode thisFn(thisObj);

    //get current object name
    MString thisObjName = thisFn.name(&stat);
    CHECK_ERROR(stat, "Unable to get the name of this deformer node\n");


    //create an accessory locator for user to manipulate a local deformation space
    MObject locRootObj = dagMod.createNode("transform", MObject::kNullObj, &stat);
    CHECK_ERROR(stat, "Unable to create locator node\n");
    stat = dagMod.doIt();
    CHECK_ERROR(stat, "Unable to execute DAG modifications for creating locator\n");


    //rename transform and shape nodes
    stat = dagMod.renameNode(locRootObj, thisObjName + "_root");
    CHECK_ERROR(stat, "Unable to rename locator transform node\n");
    MDagPath locRootDagPath;
    MFnDagNode locRootDagFn(locRootObj);

    MObject locUpObj = dagMod.createNode("locator", MObject::kNullObj, &stat);
    CHECK_ERROR(stat, "Unable to create locatorUp node\n");
    stat = dagMod.doIt();
    CHECK_ERROR(stat, "Unable to execute DAG modifications for creating locator\n");

    //rename transform and shape nodes
    stat = dagMod.renameNode(locUpObj, thisObjName + "_loc");
    CHECK_ERROR(stat, "Unable to rename locator transform node\n");
    MDagPath locUpDagPath;
    MFnDagNode locUpDagFn(locUpObj);
    stat = locUpDagFn.getPath(locUpDagPath);
    CHECK_ERROR(stat, "Unable to get DAG path of locator\n");
    stat = locUpDagPath.extendToShape();
    CHECK_ERROR(stat, "Unable to get shape DAG path from given DAG path\n");
    MObject locUpShapeObj = locUpDagPath.node(&stat);
    CHECK_ERROR(stat, "Unable to get MObject from given locatorUp DAG path\n");
    stat = dagMod.renameNode(locUpShapeObj, thisObjName + "_locShape");
    CHECK_ERROR(stat, "Unable to rename locator shape node\n");

    locRootDagFn.addChild(locUpObj);

    MFnDependencyNode locUpFn(locUpObj);
    MObject bendStrengthAttr;
    bendStrengthAttr = nAttr.create("bendStrength", "bendStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(-3);
    nAttr.setMax(3);
    locUpFn.addAttribute(bendStrengthAttr);
    MObject bendStrengthAttrs = locUpFn.attribute("bendStrength", &stat);
    stat = dagMod.connect(locUpObj, bendStrengthAttrs, thisObj, bendStrength);

    MObject xStrengthAttr;
    xStrengthAttr = nAttr.create("xStrength", "xStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    locUpFn.addAttribute(xStrengthAttr);
    MObject xStrengthAttrs = locUpFn.attribute("xStrength", &stat);
    stat = dagMod.connect(locUpObj, xStrengthAttrs, thisObj, xStrength);

    MObject zStrengthAttr;
    zStrengthAttr = nAttr.create("zStrength", "zStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    locUpFn.addAttribute(zStrengthAttr);
    MObject zStrengthAttrs = locUpFn.attribute("zStrength", &stat);
    stat = dagMod.connect(locUpObj, zStrengthAttrs, thisObj, zStrength);

    MObject twistStrengthAttr;
    twistStrengthAttr = nAttr.create("twistStrength", "twistStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(-3);
    nAttr.setMax(3);
    locUpFn.addAttribute(twistStrengthAttr);
    MObject twistStrengthAttrs = locUpFn.attribute("twistStrength", &stat);
    stat = dagMod.connect(locUpObj, twistStrengthAttrs, thisObj, twistStrength);

    MObject shrinkStrengthAttr;
    shrinkStrengthAttr = nAttr.create("shrinkStrength", "shrinkStrength", MFnNumericData::kFloat, 1.0, &stat);
    CHECK_ERROR(stat, "Unable to create shrinkStrength attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(-10);
    nAttr.setMax(10);
    locUpFn.addAttribute(shrinkStrengthAttr);
    MObject shrinkStrengthAttrs = locUpFn.attribute("shrinkStrength", &stat);
    stat = dagMod.connect(locUpObj, shrinkStrengthAttrs, thisObj, shrinkStrength);

    MObject expandStrengthAttr;
    expandStrengthAttr = nAttr.create("expandStrength", "expandStrength", MFnNumericData::kFloat, 1.0, &stat);
    CHECK_ERROR(stat, "Unable to create expandStrength attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(-10);
    nAttr.setMax(10);
    locUpFn.addAttribute(expandStrengthAttr);
    MObject expandStrengthAttrs = locUpFn.attribute("expandStrength", &stat);
    stat = dagMod.connect(locUpObj, expandStrengthAttrs, thisObj, expandStrength);

    MObject stretchBendWeightAttr;
    stretchBendWeightAttr = nAttr.create("stretchBendWeight", "stretchBendWeight", MFnNumericData::kFloat, 1.0, &stat);
    CHECK_ERROR(stat, "Unable to create stretchBendWeight attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(0);
    nAttr.setMax(1);
    locUpFn.addAttribute(stretchBendWeightAttr);
    MObject stretchBendWeightAttrs = locUpFn.attribute("stretchBendWeight", &stat);
    stat = dagMod.connect(locUpObj, stretchBendWeightAttrs, thisObj, stretchBendWeight);

    MFnDependencyNode locUpShapeFn(locUpShapeObj);
    MPlug tyPlug = locUpFn.findPlug("translateY", false);
    MObject worldUpMatrixAttr = locUpFn.attribute("matrix", &stat);
    CHECK_ERROR(stat, "Unable to get worldUpMatrix attribute for locator\n");
    stat = dagMod.connect(locUpObj, worldUpMatrixAttr, thisObj, locatorUpWorldSpace);
    CHECK_ERROR(stat, "Unable to connect locator worldUpMatrix to deformer locatorWorldSpace\n");
    MObject worldUpRotateYAttr = locUpFn.attribute("rotateY", &stat);
    stat = dagMod.connect(locUpObj, worldUpRotateYAttr, thisObj, twist);
    MFnDependencyNode locRootFn(locRootObj);

    MPlug sxPlug;
    MPlug syPlug;
    MPlug szPlug;
    MObject worldMatrixAttr = locRootFn.attribute("worldMatrix", &stat);
    CHECK_ERROR(stat, "Unable to get worldMatrix attribute for locator\n");
    stat = dagMod.connect(locRootObj, worldMatrixAttr, thisObj, locatorWorldSpace);
    CHECK_ERROR(stat, "Unable to connect locator worldMatrix to deformer locatorWorldSpace\n");
    sxPlug = locUpFn.findPlug("scaleX", false);
    sxPlug.setLocked(true);
    sxPlug.setKeyable(false);
    sxPlug = locUpFn.findPlug("scaleY", false);
    sxPlug.setLocked(true);
    sxPlug.setKeyable(false);
    sxPlug = locUpFn.findPlug("scaleZ", false);
    sxPlug.setLocked(true);
    sxPlug.setKeyable(false);
    sxPlug = locUpFn.findPlug("rotateX", false);
    sxPlug.setLocked(true);
    sxPlug.setKeyable(false);
    sxPlug = locUpFn.findPlug("rotateZ", false);
    sxPlug.setLocked(true);
    sxPlug.setKeyable(false);
    return stat;
}

//accessory attribute method
MObject& prSquash::accessoryAttribute() const
{
    return locatorUpWorldSpace;
}

//creator method
void* prSquash::creator()
{
    return new prSquash();

}

//init method
MStatus prSquash::initialize()
{
    MStatus stat = MS::kSuccess;
    MFnNumericAttribute nAttr;
    MFnMatrixAttribute mAttr;
    MFnUnitAttribute uAttr;
    MRampAttribute mrAttr;
    MFnCompoundAttribute    cAttr;
    //numTasks attr
    numTasks = nAttr.create("numTasks", "nt", MFnNumericData::kInt, 16, &stat);
    CHECK_ERROR(stat, "Unable to create numTasks attribute\n");
    nAttr.setMin(1);
    stat = addAttribute(numTasks);
    CHECK_ERROR(stat, "Unable to add numTasks attribute\n");

    bendStrength = nAttr.create("bendStrength", "bendStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(-3);
    nAttr.setMax(3);
    addAttribute(bendStrength);
    attributeAffects(prSquash::bendStrength, prSquash::outputGeom);


    xStrength = nAttr.create("xStrength", "xStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(0);
    nAttr.setMax(1);
    addAttribute(xStrength);
    attributeAffects(prSquash::xStrength, prSquash::outputGeom);


    zStrength = nAttr.create("zStrength", "zStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(0);
    nAttr.setMax(1);
    addAttribute(zStrength);
    attributeAffects(prSquash::zStrength, prSquash::outputGeom);

    initialHeight = nAttr.create("initialHeight", "initialHeight", MFnNumericData::kDouble, 0.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    addAttribute(initialHeight);
    attributeAffects(prSquash::initialHeight, prSquash::outputGeom);


    twist = uAttr.create("twist", "twist", MFnUnitAttribute::kAngle, 1.0);
    uAttr.setKeyable(true);
    uAttr.setWritable(true);
    uAttr.setStorable(true);
    uAttr.setReadable(true);
    uAttr.setHidden(true);
    uAttr.setMin(-PI);
    uAttr.setMax(PI);
    addAttribute(twist);
    attributeAffects(prSquash::twist, prSquash::outputGeom);

    twistStrength = nAttr.create("twistStrength", "twistStrength", MFnNumericData::kFloat, 1.0);
    nAttr.setKeyable(true);
    nAttr.setWritable(true);
    nAttr.setStorable(true);
    nAttr.setReadable(true);
    nAttr.setMin(-3);
    nAttr.setMax(3);
    addAttribute(twistStrength);
    attributeAffects(prSquash::twistStrength, prSquash::outputGeom);

    shrinkRamp = mrAttr.createCurveRamp("shrinkRamp", "shrinkRamp", &stat);
    CHECK_ERROR(stat, "Unable to create speed attribute\n");
    stat = addAttribute(shrinkRamp);
    CHECK_ERROR(stat, "Unable to add speed attribute\n");
    stat = attributeAffects(prSquash::shrinkRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from speed to outputGeom");

    shrinkStrength = nAttr.create("shrinkStrength", "shrinkStrength", MFnNumericData::kFloat, 1.0, &stat);
    CHECK_ERROR(stat, "Unable to create shrinkStrength attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(-10);
    nAttr.setMax(10);
    stat = addAttribute(shrinkStrength);
    CHECK_ERROR(stat, "Unable to add shrinkStrength attribute\n");
    stat = attributeAffects(prSquash::shrinkStrength, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from shrinkStrength to outputGeom");

    expandRamp = mrAttr.createCurveRamp("expandRamp", "expandRamp", &stat);
    CHECK_ERROR(stat, "Unable to create speed attribute\n");
    stat = addAttribute(expandRamp);
    CHECK_ERROR(stat, "Unable to add scale attribute\n");
    stat = attributeAffects(prSquash::expandRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from scale to outputGeom");

    bendRamp = mrAttr.createCurveRamp("bendRamp", "bendRamp", &stat);
    CHECK_ERROR(stat, "Unable to create bend attribute\n");
    stat = addAttribute(bendRamp);
    CHECK_ERROR(stat, "Unable to add bend attribute\n");
    stat = attributeAffects(prSquash::bendRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from bend to outputGeom");

    twistRamp = mrAttr.createCurveRamp("twistRamp", "twistRamp", &stat);
    CHECK_ERROR(stat, "Unable to create twist attribute\n");
    stat = addAttribute(twistRamp);
    CHECK_ERROR(stat, "Unable to add twist attribute\n");
    stat = attributeAffects(prSquash::twistRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from twist to outputGeom");

    deformerRamp = mrAttr.createCurveRamp("deformerRamp", "deformerRamp", &stat);
    CHECK_ERROR(stat, "Unable to create deformerRamp attribute\n");
    stat = addAttribute(deformerRamp);
    CHECK_ERROR(stat, "Unable to add deformerRamp attribute\n");
    stat = attributeAffects(prSquash::deformerRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from deformerRamp to outputGeom");

    stretchRamp = mrAttr.createCurveRamp("stretchRamp", "stretchRamp", &stat);
    CHECK_ERROR(stat, "Unable to create stretch attribute\n");
    stat = addAttribute(stretchRamp);
    CHECK_ERROR(stat, "Unable to add stretch attribute\n");
    stat = attributeAffects(prSquash::stretchRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from stretch to outputGeom");

    squashRamp = mrAttr.createCurveRamp("squashRamp", "squashRamp", &stat);
    CHECK_ERROR(stat, "Unable to create squash attribute\n");
    stat = addAttribute(squashRamp);
    CHECK_ERROR(stat, "Unable to add squash attribute\n");
    stat = attributeAffects(prSquash::squashRamp, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from squash to outputGeom");

    expandStrength = nAttr.create("expandStrength", "expandStrength", MFnNumericData::kFloat, 1.0, &stat);
    CHECK_ERROR(stat, "Unable to create expandStrength attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(-10);
    nAttr.setMax(10);
    stat = addAttribute(expandStrength);
    CHECK_ERROR(stat, "Unable to add expandStrength attribute\n");
    stat = attributeAffects(prSquash::expandStrength, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from expandStrength to outputGeom");

    stretchBendWeight = nAttr.create("stretchBendWeight", "stretchBendWeight", MFnNumericData::kFloat, 1.0, &stat);
    CHECK_ERROR(stat, "Unable to create stretchBendWeight attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    stat = addAttribute(stretchBendWeight);
    CHECK_ERROR(stat, "Unable to add stretchBendWeight attribute\n");
    stat = attributeAffects(prSquash::stretchBendWeight, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from stretchBendWeight to outputGeom");

    localGlobalWeight = nAttr.create("localGlobalWeight", "localGlobalWeight", MFnNumericData::kFloat, 0.0, &stat);
    CHECK_ERROR(stat, "Unable to create localGlobalWeight attribute\n");
    nAttr.setKeyable(true);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    stat = addAttribute(localGlobalWeight);
    CHECK_ERROR(stat, "Unable to add localGlobalWeight attribute\n");
    stat = attributeAffects(prSquash::localGlobalWeight, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from localGlobalWeight to outputGeom");

    aexpandMap = nAttr.create("expandMap", "expandMap", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);

    ashrinkMap = nAttr.create("shrinkMap", "shrinkMap", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);

    astretchMap = nAttr.create("stretchMap", "stretchMap", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);

    asquashMap = nAttr.create("squashMap", "squashMap", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);

    positionArray = nAttr.create("positionArray", "positionArray", MFnNumericData::k3Double, 0.0, &stat);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);

    aWorldMatrix = mAttr.create("worldMatrix", "worldMatrix");

    aPerGeometry = cAttr.create("perGeometry", "perGeometry", &stat);
    CHECK_ERROR(stat, "Unable to create perGeometry attribute\n");
    cAttr.setArray(true);
    cAttr.setHidden(true);
    cAttr.addChild(aWorldMatrix);
    cAttr.addChild(aexpandMap);
    cAttr.addChild(ashrinkMap);
    cAttr.addChild(astretchMap);
    cAttr.addChild(asquashMap);
    cAttr.addChild(positionArray);
    cAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(aPerGeometry);
    CHECK_ERROR(stat, "Unable to add aPerGeometry attribute\n");
    stat = attributeAffects(aexpandMap, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from aexpandMap to outputGeom");
    stat = attributeAffects(ashrinkMap, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from ashrinkMap to outputGeom");
    stat = attributeAffects(astretchMap, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from astretchMap to outputGeom");
    stat = attributeAffects(asquashMap, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from asquashMap to outputGeom");

    //boundingMin attr
    boundingMin = nAttr.createPoint("boundingMin", "boundingMin", &stat);
    CHECK_ERROR(stat, "Unable to create boundingMin attribute\n");
    nAttr.setDefault(0.0, 0.0, 0.0);
    nAttr.setKeyable(true);
    stat = addAttribute(boundingMin);
    CHECK_ERROR(stat, "Unable to add boundingMin attribute\n");
    stat = attributeAffects(prSquash::boundingMin, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from boundingMin to outputGeom");

    //boundingMax attr
    boundingMax = nAttr.createPoint("boundingMax", "boundingMax", &stat);
    CHECK_ERROR(stat, "Unable to create boundingMax attribute\n");
    nAttr.setDefault(0.0, 1.0, 0.0);
    nAttr.setKeyable(true);
    stat = addAttribute(boundingMax);
    CHECK_ERROR(stat, "Unable to add boundingMax attribute\n");
    stat = attributeAffects(prSquash::boundingMax, prSquash::outputGeom);
    CHECK_ERROR(stat, "Unable to call attributeAffects from boundingMax to outputGeom");


    expandArray = nAttr.create("expandArray", "expandArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(expandArray);

    shrinkArray = nAttr.create("shrinkArray", "shrinkArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(shrinkArray);

    twistArray = nAttr.create("twistArray", "twistArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(twistArray);

    deformerArray = nAttr.create("deformerArray", "deformerArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(deformerArray);

    bendArray = nAttr.create("bendArray", "bendArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(bendArray);

    stretchArray = nAttr.create("stretchArray", "stretchArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(stretchArray);

    squashArray = nAttr.create("squashArray", "squashArray", MFnNumericData::kFloat, 1.0, &stat);
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setArray(true);
    nAttr.setHidden(true);
    nAttr.setUsesArrayDataBuilder(true);
    stat = addAttribute(squashArray);

    locatorWorldSpace = mAttr.create("locatorWorldSpace", "locatorWorldSpace", MFnMatrixAttribute::kDouble, &stat);
    CHECK_ERROR(stat, "Unable to create locatorWorldSpace attribute\n");
    mAttr.setStorable(false);
    mAttr.setHidden(true);
    stat = addAttribute(locatorWorldSpace);
    CHECK_ERROR(stat, "Unable to add locatorWorldSpace attribute\n");
    stat = attributeAffects(prSquash::locatorWorldSpace, prSquash::outputGeom);

    locatorUpWorldSpace = mAttr.create("locatorUpWorldSpace", "locUpsp", MFnMatrixAttribute::kDouble, &stat);
    CHECK_ERROR(stat, "Unable to create locatorUpWorldSpace attribute\n");
    mAttr.setStorable(false);
    mAttr.setHidden(true);
    stat = addAttribute(locatorUpWorldSpace);
    CHECK_ERROR(stat, "Unable to add locatorUpWorldSpace attribute\n");
    stat = attributeAffects(prSquash::locatorUpWorldSpace, prSquash::outputGeom);

    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash weights");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash expandMap");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash shrinkMap");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash stretchMap");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash squashMap");
    return stat;
}

void prSquash::postConstructor()
{
    MStatus status;
    MObject thisNode = this->thisMObject();
    MDagModifier dagMods;

    postConstructor_init_curveRamp(thisNode, bendRamp, 0, 0.0f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, bendRamp, 1, 1.0f, 1.0f, 3);

    postConstructor_init_curveRamp(thisNode, twistRamp, 0, 0.0f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, twistRamp, 1, 1.0f, 1.0f, 3);

    postConstructor_init_curveRamp(thisNode, deformerRamp, 0, 0.0f, 1.0f, 1);
    postConstructor_init_curveRamp(thisNode, deformerRamp, 1, 1.0f, 1.0f, 1);

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

    postConstructor_init_curveRamp(thisNode, stretchRamp, 0, 0.0f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, stretchRamp, 1, 1.0f, 1.0f, 3);

    postConstructor_init_curveRamp(thisNode, squashRamp, 0, 0.0f, 1.0f, 3);
    postConstructor_init_curveRamp(thisNode, squashRamp, 1, 1.0f, 1.0f, 3);
  
}

MStatus prSquash::postConstructor_init_curveRamp(MObject& nodeObj,
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

MStatus prSquash::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray) {
    if (plug == ashrinkMap || plug == aexpandMap ||  plug == astretchMap || plug == asquashMap || plug == weights) {
        unsigned int geomIndex = 0;
        if (plug.isArray()) {
            geomIndex = plug.parent().logicalIndex();
        }
        else {
            geomIndex = plug.array().parent().logicalIndex();
        }
        dirtyMap_[geomIndex] = true;
    }
    return MS::kSuccess;
}

MStatus prSquash::GetInputMesh(MDataBlock& data, unsigned int geomIndex, MObject* oInputMesh) {
    MStatus status;
    MArrayDataHandle hInput = data.outputArrayValue(input, &status);
    status = hInput.jumpToElement(geomIndex);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MDataHandle hInputGeom = hInput.outputValue().child(inputGeom);
    *oInputMesh = hInputGeom.asMesh();
    return status;
}

MStatus prSquash::JumpToElement(MArrayDataHandle& hArray, unsigned int index) {
    MStatus status;
    status = hArray.jumpToElement(index);
    if (MFAIL(status)) {
        MArrayDataBuilder builder = hArray.builder(&status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        builder.addElement(index, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = hArray.set(builder);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = hArray.jumpToElement(index);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }
    return status;
}


MTypeId prSquash::GetTypeId()
{
    return(TYPE_IDMT);
}

MString prSquash::GetTypeName()
{
    return(TYPE_NAMEMT);
}