
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>
#include <maya/MFnIkJoint.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MFnAnimCurve.h>
#include "prPairBlendCmd.h"
#include "prPairBlend.h"

static const MString MEL_COMMAND = "prPairBlend";
MString prPairBlendCmd::nodeName;

prPairBlendCmd::prPairBlendCmd() :
    MPxCommand()
{
}

prPairBlendCmd::~prPairBlendCmd()
{
}

MStatus prPairBlendCmd::doIt(const MArgList& args)
{
    MStatus status;
    return(redoIt());
}

MStatus prPairBlendCmd::undoIt()
{
    mDGModifier.undoIt();
    mDagModifier.undoIt();
    return(MS::kSuccess);
}

MStatus prPairBlendCmd::redoIt()
{
    MStatus status;
    MFnNumericAttribute nAttr;
    MFnUnitAttribute uAttr;
    MSelectionList selectList;
    MGlobal::getActiveSelectionList(selectList);
    if (selectList.length() == 1)
    {
        MObject selectedObjs[1];
        selectList.getDependNode(0, selectedObjs[0]);
        MFnDagNode srcDag(selectedObjs[0]);
        MDagPath srcPath = srcDag.dagPath();
        nodeName = MString("prPairBlend_") + srcDag.name();
        mShapeObj = mDGModifier.createNode(prPairBlend::GetTypeName());
        mDGModifier.doIt();
        mDagModifier.renameNode(mShapeObj, nodeName);
        MFnDependencyNode srcFn(selectedObjs[0]);
        MFnDependencyNode pairFn(mShapeObj);
        MObject rotate = srcFn.attribute("rotate", &status);
        MObject inRotation = pairFn.attribute("inRotation", &status);
        MObject outRotation = pairFn.attribute("outRotation", &status);
        MObject inRotationOrder = pairFn.attribute("rotationOrder", &status);
        MObject outRotationOrder = srcFn.attribute("rotateOrder", &status);
        status = mDagModifier.connect(selectedObjs[0], rotate, mShapeObj, inRotation);
        status = mDagModifier.connect(selectedObjs[0], outRotationOrder, mShapeObj, inRotationOrder);

        MObject blendAttr;
        blendAttr = nAttr.create("blend", "blend", MFnNumericData::kFloat, 0.5f);
        nAttr.setKeyable(true);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setChannelBox(true);
        mDagModifier.addAttribute(selectedObjs[0], blendAttr);
        mDagModifier.doIt();
        MObject inBlendAttr = srcFn.attribute("blend", &status);
        MObject outBlendAttr = pairFn.attribute("blend", &status);
        status = mDagModifier.connect( selectedObjs[0], inBlendAttr, mShapeObj, outBlendAttr);

        MObject rollAttr;
        rollAttr = uAttr.create("roll", "roll", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setKeyable(true);
        uAttr.setWritable(true);
        uAttr.setStorable(true);
        uAttr.setReadable(true);
        uAttr.setHidden(false);

        mDagModifier.addAttribute(selectedObjs[0], rollAttr);
        mDagModifier.doIt();
        MObject inRollAttr = srcFn.attribute("roll", &status);
        MObject outRollAttr = pairFn.attribute("roll", &status);
        status = mDagModifier.connect(mShapeObj, outRollAttr, selectedObjs[0], inRollAttr);

        MObject yawAttr;
        yawAttr = uAttr.create("yaw", "yaw", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setKeyable(true);
        uAttr.setWritable(true);
        uAttr.setStorable(true);
        uAttr.setReadable(true);
        uAttr.setHidden(false);

        mDagModifier.addAttribute(selectedObjs[0], yawAttr);
        mDagModifier.doIt();
        MObject inYawAttr = srcFn.attribute("yaw", &status);
        MObject outYawAttr = pairFn.attribute("yaw", &status);
        status = mDagModifier.connect(mShapeObj, outYawAttr, selectedObjs[0], inYawAttr);

        MObject pitchAttr;
        pitchAttr = uAttr.create("pitch", "pitch", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setKeyable(true);
        uAttr.setWritable(true);
        uAttr.setStorable(true);
        uAttr.setReadable(true);
        uAttr.setHidden(false);

        mDagModifier.addAttribute(selectedObjs[0], pitchAttr);
        mDagModifier.doIt();
        MObject inPitchAttr = srcFn.attribute("pitch", &status);
        MObject outPitchAttr = pairFn.attribute("pitch", &status);
        status = mDagModifier.connect(mShapeObj, outPitchAttr, selectedObjs[0], inPitchAttr);

        MObject outRotateX = uAttr.create("outRotateX", "outRx", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setStorable(true);
        uAttr.setWritable(true);
        uAttr.setReadable(true);
        uAttr.setKeyable(true);
        mDagModifier.addAttribute(selectedObjs[0], outRotateX);
        mDagModifier.doIt();
        MObject in_outRotateX = srcFn.attribute("outRotateX", &status);
        MObject out_outRotateX = pairFn.attribute("outRotateX", &status);
        status = mDagModifier.connect(mShapeObj, out_outRotateX, selectedObjs[0], in_outRotateX);
        std::cout << status << std::endl;

        MObject outRotateY = uAttr.create("outRotateY", "outRy", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setStorable(true);
        uAttr.setWritable(true);
        uAttr.setReadable(true);
        uAttr.setKeyable(true);
        mDagModifier.addAttribute(selectedObjs[0], outRotateY);
        mDagModifier.doIt();
        MObject in_outRotateY = srcFn.attribute("outRotateY", &status);
        MObject out_outRotateY = pairFn.attribute("outRotateY", &status);
        status = mDagModifier.connect(mShapeObj, out_outRotateY, selectedObjs[0], in_outRotateY);
        std::cout << status << std::endl;

        MObject outRotateZ = uAttr.create("outRotateZ", "outRz", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setStorable(true);
        uAttr.setWritable(true);
        uAttr.setReadable(true);
        uAttr.setKeyable(true);
        mDagModifier.addAttribute(selectedObjs[0], outRotateZ);
        mDagModifier.doIt();
        MObject in_outRotateZ = srcFn.attribute("outRotateZ", &status);
        MObject out_outRotateZ = pairFn.attribute("outRotateZ", &status);
        status = mDagModifier.connect(mShapeObj, out_outRotateZ, selectedObjs[0], in_outRotateZ);
        std::cout << status << std::endl;

        MObject weight_forwardAttr;
        weight_forwardAttr = nAttr.create("forward", "forward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_forwardAttr);
        mDagModifier.doIt();
        MObject in_forwardAttr = srcFn.attribute("forward", &status);
        MObject out_forwardAttr = pairFn.attribute("forward", &status);
        status = mDagModifier.connect(mShapeObj, out_forwardAttr, selectedObjs[0], in_forwardAttr);
        std::cout << status << std::endl;

        MObject weight_backwardAttr;
        weight_backwardAttr = nAttr.create("backward", "backward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_backwardAttr);
        mDagModifier.doIt();
        MObject in_backwardAttr = srcFn.attribute("backward", &status);
        MObject out_backwardAttr = pairFn.attribute("backward", &status);
        status = mDagModifier.connect(mShapeObj, out_backwardAttr, selectedObjs[0], in_backwardAttr);
        std::cout << status << std::endl;

        MObject weight_upwardAttr;
        weight_upwardAttr = nAttr.create("upward", "upward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_upwardAttr);
        mDagModifier.doIt();
        MObject in_upwardAttr = srcFn.attribute("upward", &status);
        MObject out_upwardAttr = pairFn.attribute("upward", &status);
        status = mDagModifier.connect(mShapeObj, out_upwardAttr, selectedObjs[0], in_upwardAttr);
        std::cout << status << std::endl;

        MObject weight_downwardAttr;
        weight_downwardAttr = nAttr.create("downward", "downward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_downwardAttr);
        mDagModifier.doIt();
        MObject in_downwardAttr = srcFn.attribute("downward", &status);
        MObject out_downwardAttr = pairFn.attribute("downward", &status);
        status = mDagModifier.connect(mShapeObj, out_downwardAttr, selectedObjs[0], in_downwardAttr);
        std::cout << status << std::endl;

        MObject weight_intensityAttr;
        weight_intensityAttr = nAttr.create("overallIntensity", "overallIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_intensityAttr);
        mDagModifier.doIt();
        MObject in_intensityAttr = srcFn.attribute("overallIntensity", &status);
        MObject out_intensityAttr = pairFn.attribute("intensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_intensityAttr, mShapeObj, out_intensityAttr);
        std::cout << status << std::endl;

        MObject weight_forwardIntensityAttr;
        weight_forwardIntensityAttr = nAttr.create("forwardIntensity", "forwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_forwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_forwardIntensityAttr = srcFn.attribute("forwardIntensity", &status);
        MObject out_forwardIntensityAttr = pairFn.attribute("forwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_forwardIntensityAttr, mShapeObj, out_forwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_backwardIntensityAttr;
        weight_backwardIntensityAttr = nAttr.create("backwardIntensity", "backwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_backwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_backwardIntensityAttr = srcFn.attribute("backwardIntensity", &status);
        MObject out_backwardIntensityAttr = pairFn.attribute("backwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_backwardIntensityAttr, mShapeObj, out_backwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_upwardIntensityAttr;
        weight_upwardIntensityAttr = nAttr.create("upwardIntensity", "upwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_upwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_upwardIntensityAttr = srcFn.attribute("upwardIntensity", &status);
        MObject out_upwardIntensityAttr = pairFn.attribute("upwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_upwardIntensityAttr, mShapeObj, out_upwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_downwardIntensityAttr;
        weight_downwardIntensityAttr = nAttr.create("downwardIntensity", "downwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_downwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_downwardIntensityAttr = srcFn.attribute("downwardIntensity", &status);
        MObject out_downwardIntensityAttr = pairFn.attribute("downwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_downwardIntensityAttr, mShapeObj, out_downwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_springIntensityAttr;
        weight_springIntensityAttr = nAttr.create("springIntensity", "springIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_springIntensityAttr);
        mDagModifier.doIt();
        MObject in_springIntensityAttr = srcFn.attribute("springIntensity", &status);
        MObject out_springIntensityAttr = pairFn.attribute("springIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_springIntensityAttr, mShapeObj, out_springIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_springStiffnessAttr;
        weight_springStiffnessAttr = nAttr.create("springStiffness", "springStiffness", MFnNumericData::kFloat, 0.25);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_springStiffnessAttr);
        mDagModifier.doIt();
        MObject in_springStiffnessAttr = srcFn.attribute("springStiffness", &status);
        MObject out_springStiffnessAttr = pairFn.attribute("springStiffness", &status);
        status = mDagModifier.connect(selectedObjs[0], in_springStiffnessAttr, mShapeObj, out_springStiffnessAttr);
        std::cout << status << std::endl;

        MObject weight_springDampingAttr;
        weight_springDampingAttr = nAttr.create("springDamping", "springDamping", MFnNumericData::kFloat, 0.01);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_springDampingAttr);
        mDagModifier.doIt();
        MObject in_springDampingAttr = srcFn.attribute("springDamping", &status);
        MObject out_springDampingAttr = pairFn.attribute("springDamping", &status);
        status = mDagModifier.connect(selectedObjs[0], in_springDampingAttr, mShapeObj, out_springDampingAttr);
        std::cout << status << std::endl;

        MObject weight_startFrameAttr;
        weight_startFrameAttr = nAttr.create("startFrame", "startFrame", MFnNumericData::kInt, 0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_startFrameAttr);
        mDagModifier.doIt();
        MObject in_startFrameAttr = srcFn.attribute("startFrame", &status);
        MObject out_startFrameAttr = pairFn.attribute("startFrame", &status);
        status = mDagModifier.connect(selectedObjs[0], in_startFrameAttr, mShapeObj, out_startFrameAttr);
        std::cout << status << std::endl;

        MObject weight_tySpringAttr;
        weight_tySpringAttr = nAttr.create("tySpring", "tySpring", MFnNumericData::kFloat, 0.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_tySpringAttr);
        mDagModifier.doIt();
        MObject in_tySpringAttr = srcFn.attribute("tySpring", &status);
        MObject out_tySpringAttr = pairFn.attribute("tySpring", &status);
        status = mDagModifier.connect(mShapeObj, out_tySpringAttr, selectedObjs[0], in_tySpringAttr);
        std::cout << status << std::endl;

        MObject weight_tzSpringAttr;
        weight_tzSpringAttr = nAttr.create("tzSpring", "tzSpring", MFnNumericData::kFloat, 0.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_tzSpringAttr);
        mDagModifier.doIt();
        MObject in_tzSpringAttr = srcFn.attribute("tzSpring", &status);
        MObject out_tzSpringAttr = pairFn.attribute("tzSpring", &status);
        status = mDagModifier.connect(mShapeObj, out_tzSpringAttr, selectedObjs[0], in_tzSpringAttr);
        std::cout << status << std::endl;
        MGlobal::executeCommand(MString("connectAttr time1.outTime ") + nodeName + MString(".time ;"), false, true);
    }
    if (selectList.length() == 2)
    {
        MObject selectedObjs[2];
        selectList.getDependNode(0, selectedObjs[0]);
        selectList.getDependNode(1, selectedObjs[1]);
        MFnDagNode srcDag(selectedObjs[0]);
        MFnDagNode desDag(selectedObjs[1]);
        MDagPath srcPath = srcDag.dagPath();
        //nodeName = MString("prPairBlend_") + srcDag.name() + MString("_") + desDag.name();
        nodeName = MString("prPairBlend_") + srcDag.name();
        mShapeObj = mDGModifier.createNode(prPairBlend::GetTypeName());
        mDGModifier.doIt();
        mDagModifier.renameNode(mShapeObj, nodeName);
        MFnDependencyNode srcFn(selectedObjs[0]);
        MFnDependencyNode desFn(selectedObjs[1]);
        MFnDependencyNode pairFn(mShapeObj);
        MObject rotate = srcFn.attribute("rotate", &status);
        MObject inRotation = pairFn.attribute("inRotation", &status);
        MObject outRotation = pairFn.attribute("outRotation", &status);
        MObject inRotationOrder = pairFn.attribute("rotationOrder", &status);
        MObject outRotationOrder = srcFn.attribute("rotateOrder", &status);
        MObject inDesRotationOrder = desFn.attribute("rotateOrder", &status);
        MObject desRotate = desFn.attribute("rotate", &status);
        status = mDagModifier.connect(selectedObjs[0], rotate, mShapeObj, inRotation);
        status = mDagModifier.connect(selectedObjs[0], outRotationOrder, mShapeObj, inRotationOrder);
        status = mDagModifier.connect(mShapeObj, outRotation, selectedObjs[1], desRotate);
        status = mDagModifier.connect(mShapeObj, inRotationOrder, selectedObjs[1], inDesRotationOrder);

        MObject outTranslate = srcFn.attribute("translate", &status);
        MObject inTranslate = desFn.attribute("translate", &status);
        status = mDagModifier.connect(selectedObjs[0], outTranslate, selectedObjs[1], inTranslate);
        MObject outScale = srcFn.attribute("scale", &status);
        MObject inScale = desFn.attribute("scale", &status);
        status = mDagModifier.connect(selectedObjs[0], outScale, selectedObjs[1], inScale);
        /// ////////////////
        MObject blendAttr;
        blendAttr = nAttr.create("blend", "blend", MFnNumericData::kFloat, 0.5f);
        nAttr.setKeyable(true);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setChannelBox(true);
        mDagModifier.addAttribute(selectedObjs[0], blendAttr);
        mDagModifier.doIt();
        MObject inBlendAttr = srcFn.attribute("blend", &status);
        MObject outBlendAttr = pairFn.attribute("blend", &status);
        status = mDagModifier.connect(selectedObjs[0], inBlendAttr, mShapeObj, outBlendAttr);

        MObject rollAttr;
        rollAttr = uAttr.create("roll", "roll", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setKeyable(true);
        uAttr.setWritable(true);
        uAttr.setStorable(true);
        uAttr.setReadable(true);
        uAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], rollAttr);
        mDagModifier.doIt();
        MObject inRollAttr = srcFn.attribute("roll", &status);
        MObject outRollAttr = pairFn.attribute("roll", &status);
        status = mDagModifier.connect(mShapeObj, outRollAttr, selectedObjs[0], inRollAttr);

        MObject yawAttr;
        yawAttr = uAttr.create("yaw", "yaw", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setKeyable(true);
        uAttr.setWritable(true);
        uAttr.setStorable(true);
        uAttr.setReadable(true);
        uAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], yawAttr);
        mDagModifier.doIt();
        MObject inYawAttr = srcFn.attribute("yaw", &status);
        MObject outYawAttr = pairFn.attribute("yaw", &status);
        status = mDagModifier.connect(mShapeObj, outYawAttr, selectedObjs[0], inYawAttr);

        MObject pitchAttr;
        pitchAttr = uAttr.create("pitch", "pitch", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setKeyable(true);
        uAttr.setWritable(true);
        uAttr.setStorable(true);
        uAttr.setReadable(true);
        uAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], pitchAttr);
        mDagModifier.doIt();
        MObject inPitchAttr = srcFn.attribute("pitch", &status);
        MObject outPitchAttr = pairFn.attribute("pitch", &status);
        status = mDagModifier.connect(mShapeObj, outPitchAttr, selectedObjs[0], inPitchAttr);
        std::cout << status << std::endl;

        MObject outRotateX = uAttr.create("outRotateX", "outRx", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setStorable(true);
        uAttr.setWritable(true);
        uAttr.setReadable(true);
        uAttr.setKeyable(true);
        mDagModifier.addAttribute(selectedObjs[0], outRotateX);
        mDagModifier.doIt();
        MObject in_outRotateX = srcFn.attribute("outRotateX", &status);
        MObject out_outRotateX = pairFn.attribute("outRotateX", &status);
        status = mDagModifier.connect(mShapeObj, out_outRotateX, selectedObjs[0], in_outRotateX);
        std::cout << status << std::endl;

        MObject outRotateY = uAttr.create("outRotateY", "outRy", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setStorable(true);
        uAttr.setWritable(true);
        uAttr.setReadable(true);
        uAttr.setKeyable(true);
        mDagModifier.addAttribute(selectedObjs[0], outRotateY);
        mDagModifier.doIt();
        MObject in_outRotateY = srcFn.attribute("outRotateY", &status);
        MObject out_outRotateY = pairFn.attribute("outRotateY", &status);
        status = mDagModifier.connect(mShapeObj, out_outRotateY, selectedObjs[0], in_outRotateY);
        std::cout << status << std::endl;

        MObject outRotateZ = uAttr.create("outRotateZ", "outRz", MFnUnitAttribute::kAngle, 0.0);
        uAttr.setStorable(true);
        uAttr.setWritable(true);
        uAttr.setReadable(true);
        uAttr.setKeyable(true);
        mDagModifier.addAttribute(selectedObjs[0], outRotateZ);
        mDagModifier.doIt();
        MObject in_outRotateZ = srcFn.attribute("outRotateZ", &status);
        MObject out_outRotateZ = pairFn.attribute("outRotateZ", &status);
        status = mDagModifier.connect(mShapeObj, out_outRotateZ, selectedObjs[0], in_outRotateZ);
        std::cout << status << std::endl;

        MObject weight_forwardAttr;
        weight_forwardAttr = nAttr.create("forward", "forward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_forwardAttr);
        mDagModifier.doIt();
        MObject in_forwardAttr = srcFn.attribute("forward", &status);
        MObject out_forwardAttr = pairFn.attribute("forward", &status);
        status = mDagModifier.connect(mShapeObj, out_forwardAttr, selectedObjs[0], in_forwardAttr);
        std::cout << status << std::endl;

        MObject weight_backwardAttr;
        weight_backwardAttr = nAttr.create("backward", "backward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_backwardAttr);
        mDagModifier.doIt();
        MObject in_backwardAttr = srcFn.attribute("backward", &status);
        MObject out_backwardAttr = pairFn.attribute("backward", &status);
        status = mDagModifier.connect(mShapeObj, out_backwardAttr, selectedObjs[0], in_backwardAttr);
        std::cout << status << std::endl;

        MObject weight_upwardAttr;
        weight_upwardAttr = nAttr.create("upward", "upward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_upwardAttr);
        mDagModifier.doIt();
        MObject in_upwardAttr = srcFn.attribute("upward", &status);
        MObject out_upwardAttr = pairFn.attribute("upward", &status);
        status = mDagModifier.connect(mShapeObj, out_upwardAttr, selectedObjs[0], in_upwardAttr);
        std::cout << status << std::endl;

        MObject weight_downwardAttr;
        weight_downwardAttr = nAttr.create("downward", "downward", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_downwardAttr);
        mDagModifier.doIt();
        MObject in_downwardAttr = srcFn.attribute("downward", &status);
        MObject out_downwardAttr = pairFn.attribute("downward", &status);
        status = mDagModifier.connect(mShapeObj, out_downwardAttr, selectedObjs[0], in_downwardAttr);
        std::cout << status << std::endl;

        MObject weight_intensityAttr;
        weight_intensityAttr = nAttr.create("overallIntensity", "overallIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_intensityAttr);
        mDagModifier.doIt();
        MObject in_intensityAttr = srcFn.attribute("overallIntensity", &status);
        MObject out_intensityAttr = pairFn.attribute("intensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_intensityAttr, mShapeObj, out_intensityAttr);
        std::cout << status << std::endl;

        MObject weight_forwardIntensityAttr;
        weight_forwardIntensityAttr = nAttr.create("forwardIntensity", "forwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_forwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_forwardIntensityAttr = srcFn.attribute("forwardIntensity", &status);
        MObject out_forwardIntensityAttr = pairFn.attribute("forwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_forwardIntensityAttr, mShapeObj, out_forwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_backwardIntensityAttr;
        weight_backwardIntensityAttr = nAttr.create("backwardIntensity", "backwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_backwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_backwardIntensityAttr = srcFn.attribute("backwardIntensity", &status);
        MObject out_backwardIntensityAttr = pairFn.attribute("backwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_backwardIntensityAttr, mShapeObj, out_backwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_upwardIntensityAttr;
        weight_upwardIntensityAttr = nAttr.create("upwardIntensity", "upwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_upwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_upwardIntensityAttr = srcFn.attribute("upwardIntensity", &status);
        MObject out_upwardIntensityAttr = pairFn.attribute("upwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_upwardIntensityAttr, mShapeObj, out_upwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_downwardIntensityAttr;
        weight_downwardIntensityAttr = nAttr.create("downwardIntensity", "downwardIntensity", MFnNumericData::kFloat, 1.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_downwardIntensityAttr);
        mDagModifier.doIt();
        MObject in_downwardIntensityAttr = srcFn.attribute("downwardIntensity", &status);
        MObject out_downwardIntensityAttr = pairFn.attribute("downwardIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_downwardIntensityAttr, mShapeObj, out_downwardIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_springIntensityAttr;
        weight_springIntensityAttr = nAttr.create("springIntensity", "springIntensity", MFnNumericData::kFloat, 0.0);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_springIntensityAttr);
        mDagModifier.doIt();
        MObject in_springIntensityAttr = srcFn.attribute("springIntensity", &status);
        MObject out_springIntensityAttr = pairFn.attribute("springIntensity", &status);
        status = mDagModifier.connect(selectedObjs[0], in_springIntensityAttr, mShapeObj, out_springIntensityAttr);
        std::cout << status << std::endl;

        MObject weight_springStiffnessAttr;
        weight_springStiffnessAttr = nAttr.create("springStiffness", "springStiffness", MFnNumericData::kFloat, 0.25);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_springStiffnessAttr);
        mDagModifier.doIt();
        MObject in_springStiffnessAttr = srcFn.attribute("springStiffness", &status);
        MObject out_springStiffnessAttr = pairFn.attribute("springStiffness", &status);
        status = mDagModifier.connect(selectedObjs[0], in_springStiffnessAttr, mShapeObj, out_springStiffnessAttr);
        std::cout << status << std::endl;

        MObject weight_springDampingAttr;
        weight_springDampingAttr = nAttr.create("springDamping", "springDamping", MFnNumericData::kFloat, 0.01);
        nAttr.setMin(0.0f);
        nAttr.setMax(1.0f);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_springDampingAttr);
        mDagModifier.doIt();
        MObject in_springDampingAttr = srcFn.attribute("springDamping", &status);
        MObject out_springDampingAttr = pairFn.attribute("springDamping", &status);
        status = mDagModifier.connect(selectedObjs[0], in_springDampingAttr, mShapeObj, out_springDampingAttr);
        std::cout << status << std::endl;

        MObject weight_startFrameAttr;
        weight_startFrameAttr = nAttr.create("startFrame", "startFrame", MFnNumericData::kInt, 0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_startFrameAttr);
        mDagModifier.doIt();
        MObject in_startFrameAttr = srcFn.attribute("startFrame", &status);
        MObject out_startFrameAttr = pairFn.attribute("startFrame", &status);
        status = mDagModifier.connect(selectedObjs[0], in_startFrameAttr, mShapeObj, out_startFrameAttr);
        std::cout << status << std::endl;

        MObject weight_tySpringAttr;
        weight_tySpringAttr = nAttr.create("tySpring", "tySpring", MFnNumericData::kFloat, 0.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_tySpringAttr);
        mDagModifier.doIt();
        MObject in_tySpringAttr = srcFn.attribute("tySpring", &status);
        MObject out_tySpringAttr = pairFn.attribute("tySpring", &status);
        status = mDagModifier.connect(mShapeObj, out_tySpringAttr, selectedObjs[0], in_tySpringAttr);
        std::cout << status << std::endl;

        MObject weight_tzSpringAttr;
        weight_tzSpringAttr = nAttr.create("tzSpring", "tzSpring", MFnNumericData::kFloat, 0.0);
        nAttr.setKeyable(true);
        nAttr.setWritable(true);
        nAttr.setStorable(true);
        nAttr.setReadable(true);
        nAttr.setHidden(false);
        mDagModifier.addAttribute(selectedObjs[0], weight_tzSpringAttr);
        mDagModifier.doIt();
        MObject in_tzSpringAttr = srcFn.attribute("tzSpring", &status);
        MObject out_tzSpringAttr = pairFn.attribute("tzSpring", &status);
        status = mDagModifier.connect(mShapeObj, out_tzSpringAttr, selectedObjs[0], in_tzSpringAttr);
        std::cout << status << std::endl;
        MGlobal::executeCommand(MString("connectAttr time1.outTime ") + nodeName + MString(".time ;"), false, true);
    }
    mDagModifier.doIt();
    return(MS::kSuccess);
}

bool prPairBlendCmd::isUndoable() const
{
    return(true);
}

void* prPairBlendCmd::Creator()
{
    return(new prPairBlendCmd());
}

MString prPairBlendCmd::CommandName()
{
    return(MEL_COMMAND);
}

MSyntax prPairBlendCmd::CreateSyntax()
{
    MSyntax syntax;
    return(syntax);
}
