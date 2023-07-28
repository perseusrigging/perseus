#include "prVectorConstraint.h"
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
#include "prVectorConstraintCmd.h"
#include <maya/MFnMatrixData.h>

static const MString MEL_COMMAND = "prVectorConstraint";
MString prVectorConstraintCmd::nodeName;

prVectorConstraintCmd::prVectorConstraintCmd() :
    MPxCommand()
{
}

prVectorConstraintCmd::~prVectorConstraintCmd()
{
}

MStatus prVectorConstraintCmd::doIt(const MArgList& args)
{
    MStatus status;

    MArgDatabase argData(syntax(), args, &status);
    if (!status)
    {
        MGlobal::displayError("Error parsing command arguments");
        return(status);
    }

    MSelectionList selectionList;
    status = argData.getObjects(selectionList);
    if (!status)
    {
        MGlobal::displayError("Failed to get selection list");
        return(status);
    }

    if (selectionList.length() != 2)
    {
        MGlobal::displayError("Command requires two transform nodes");
        return(MS::kInvalidParameter);
    }
    MObject selectedObjs[4];
    MFnDependencyNode eyeNode;
    selectionList.getDependNode(0, selectedObjs[0]);
    selectionList.getDependNode(1, selectedObjs[1]);
    MFnDependencyNode srcFn(selectedObjs[1]);
    MFnDependencyNode desFn(selectedObjs[0]);
    MFnDagNode srcDag(selectedObjs[1]);
    MFnDagNode desDag(selectedObjs[0]);
    MFnDagNode parentObj(srcDag.parent(0));
    MFnDagNode parentDesObj(desDag.parent(0));
    MFnDagNode ppDag(parentObj.parent(0));
    MDagPath srcPath = srcDag.dagPath();
    MDagPath desPath = desDag.dagPath();
    mShapeObj = mDGModifier.createNode(prVectorConstraint::GetTypeName());
    nodeName = MString("prVectorConstraint_") + srcDag.name() + MString("_") + desDag.name();
    mDagModifier.renameNode(mShapeObj, nodeName);
    MPlug srcWMatPlug = srcFn.findPlug(MString("worldMatrix"), false).elementByLogicalIndex(0);
    MObject srcWMatObj;
    srcWMatPlug.getValue(srcWMatObj);
    MFnMatrixData srcWMatData(srcWMatObj);
    MMatrix srcWMat = srcWMatData.matrix(&status);
    MVector srcPos;
    srcPos[0] = srcWMat(3, 0);
    srcPos[1] = srcWMat(3, 1);
    srcPos[2] = srcWMat(3, 2);
    MPlug desWMatPlug = desFn.findPlug(MString("worldMatrix"), false).elementByLogicalIndex(0);
    MObject desWMatObj;
    desWMatPlug.getValue(desWMatObj);
    MFnMatrixData desWMatData(desWMatObj);
    MMatrix desWMat = desWMatData.matrix(&status);
    MVector desPos;
    desPos[0] = desWMat(3, 0);
    desPos[1] = desWMat(3, 1);
    desPos[2] = desWMat(3, 2);
    double dis=(srcPos - desPos).length();
    mTempObj = mDagModifier.createNode("transform");
    MFnDependencyNode nodeTmpFn;
    nodeTmpFn.setObject(mTempObj);
    hlprName = desDag.name() + MString("_vectorHlpr");
    nodeTmpFn.setName(hlprName);
    MFnDependencyNode nodeFn;
    nodeFn.setObject(mShapeObj);
    MObject child;
    child = MFnDagNode(selectedObjs[1]).child(0);
    MFnDagNode childDag(child);
    MDagPath childPath = childDag.dagPath();
    MObject childB;
    MObject childC;
    childB = MFnDagNode(selectedObjs[0]).child(0);
    MFnDagNode childBDag(childB);
    MVector eyeAim = MVector(0.0, -1.0, 0.0);
    MVector eyeUp = MVector(1.0, 0.0, 0.0);
    MFnTransform transformFn;
    MDoubleArray pos;
    MGlobal::executeCommand(MString("xform -q -rp -ws \"") + srcDag.fullPathName() + MString("\";"), pos, false, true);
    MVector eyePivotPOS = MVector(pos[0], pos[1], pos[2]);
    MGlobal::executeCommand(MString("xform -q -rp -ws \"") + childDag.fullPathName() + MString("\";"), pos, false, true);
    MVector targetPivotPOS = MVector(pos[0], pos[1], pos[2]);
    MGlobal::executeCommand(MString("xform -q -rp -ws \"") + childBDag.fullPathName() + MString("\";"), pos, false, true);
    MVector desPivotPOS = MVector(pos[0], pos[1], pos[2]);
    bool newJnt = false;
    MFnDagNode nodeTmpDagB;
    MFnDependencyNode nodeTmpFnB;
    MString jntName;
    if (ppDag.name() != "world")
    {
        MGlobal::executeCommand(MString("parent - w ") + parentObj.name() + MString(" ; "), false, true);
    }
    srcDag.removeChild(child);
    MEulerRotation identityEulerRot(0, 0, 0, MEulerRotation::kXYZ);
    MQuaternion identityQuatRot(0, 0, 0, 1);
    transformFn.setObject(child);
    transformFn.setRotation(identityEulerRot);
    transformFn.setRotateOrientation(identityQuatRot, MSpace::kWorld, 0);
    MFnDependencyNode childFnBB(child);
    MPlug srcPlug = childFnBB.findPlug("jointOrient", false);
    MPlug plug_x = srcPlug.child(0);
    MPlug plug_y = srcPlug.child(1);
    MPlug plug_z = srcPlug.child(2);
    mDagModifier.newPlugValueFloat(plug_x, 0.0);
    mDagModifier.newPlugValueFloat(plug_y, 0.0);
    mDagModifier.newPlugValueFloat(plug_z, 0.0);
    mDagModifier.doIt();

    transformFn.setObject(selectedObjs[1]);
    transformFn.setRotation(identityEulerRot);
    transformFn.setRotateOrientation(identityQuatRot, MSpace::kWorld, 0);
    srcPlug = srcDag.findPlug("jointOrient", false);
    plug_x = srcPlug.child(0);
    plug_y = srcPlug.child(1);
    plug_z = srcPlug.child(2);
    mDagModifier.newPlugValueFloat(plug_x, 0.0);
    mDagModifier.newPlugValueFloat(plug_y, 0.0);
    mDagModifier.newPlugValueFloat(plug_z, 0.0);
    MGlobal::executeCommand(MString("setAttr ") + srcDag.fullPathName() + MString(".rotateAxis 0 0 0;"), false, true);
    mDagModifier.doIt();

    transformFn.setObject(parentObj.object());
    transformFn.setRotation(identityEulerRot);
    transformFn.setRotateOrientation(identityQuatRot, MSpace::kWorld, 0);
    srcPlug = parentObj.findPlug("jointOrient", false);
    plug_x = srcPlug.child(0);
    plug_y = srcPlug.child(1);
    plug_z = srcPlug.child(2);
    mDagModifier.newPlugValueFloat(plug_x, 0.0);
    mDagModifier.newPlugValueFloat(plug_y, 0.0);
    mDagModifier.newPlugValueFloat(plug_z, 0.0);
    mDagModifier.doIt();

    MVector aimVector = (targetPivotPOS - eyePivotPOS);
    MVector eyeU = aimVector.normal();
    MVector up = MGlobal::upAxis();
    MVector worldUp = MVector(up.x, up.y, up.z);
    MVector eyeV = worldUp;
    MVector eyeW = (eyeU ^ eyeV).normal();
    eyeV = eyeW ^ eyeU;
    MQuaternion quaternion;
    MQuaternion quaternionU = MQuaternion(eyeAim, eyeU);
    quaternion = quaternionU;
    transformFn.setObject(srcDag.object());

    transformFn.setObject(parentObj.object());
    mDagModifier.doIt();

    transformFn.setRotateOrientation(quaternion, MSpace::kObject, 0);
    parentObj.addChild(selectedObjs[1]);
    mDagModifier.doIt();

    MGlobal::executeCommand(MString("xform - ws - t ") + eyePivotPOS[0] + MString(" ") + eyePivotPOS[1] + MString(" ") + eyePivotPOS[2] + MString("\"") + srcDag.fullPathName() + MString("\";"), false, true);
    transformFn.setObject(selectedObjs[1]);
    mDagModifier.doIt();
    srcDag.addChild(child);
    mDagModifier.doIt();
    MGlobal::executeCommand(MString("xform - ws - t ") + targetPivotPOS[0] + MString(" ") + targetPivotPOS[1] + MString(" ") + targetPivotPOS[2] + MString("\"") + childDag.fullPathName() + MString("\";"), false, true);
    mDagModifier.doIt();

    selectionList.clear();
    mDGModifier.doIt();
    if (ppDag.name() != "world")
    {
        cout << "parentObj " << parentObj.name() << endl;
        if (newJnt == false)
        {
            MGlobal::executeCommand(MString("parent ") + parentObj.fullPathName() + MString(" ") + ppDag.fullPathName() + MString(";"), false, true);
        }
    }
    mDGModifier.doIt();

    MFnDependencyNode childFnB(childB);
    MPlug childPlugB = childFnB.findPlug("translate", false);
    float txB = 0, tyB = 0, tzB = 0;
    MPlug plug_xB = childPlugB.child(0);
    MPlug plug_yB = childPlugB.child(1);
    MPlug plug_zB = childPlugB.child(2);
    plug_xB.getValue(txB);
    plug_yB.getValue(tyB);
    plug_zB.getValue(tzB);
    MString aimStr = MString("Y");
    if (abs(txB) > abs(tyB) && abs(txB) > abs(tzB))
    {
        aimStr = MString("Y");
    }
    else if (abs(tyB) > abs(txB) && abs(tyB) > abs(tzB))
    {
        aimStr = MString("X");
    }
    else if (abs(tzB) > abs(txB) && abs(tzB) > abs(tyB))
    {
        aimStr = MString("Y");
    }
    parentObj.addChild(mTempObj);
    MGlobal::executeCommand(MString("xform - ws - t ") + desPivotPOS[0] + MString(" ") + desPivotPOS[1] + MString(" ") + desPivotPOS[2] + MString("\"") + hlprName + MString("\";"), false, true);
    mDagModifier.doIt();

    srcPlug = srcFn.findPlug(MString("matrix"), false);
    MPlug matPlug = nodeFn.findPlug(MString("matrix"), false);
    MPlug rotPlug = nodeFn.findPlug(MString("outputRotate"), false);
    MPlug origSrcPlug = nodeTmpFn.findPlug(MString("translate"), false);
    MPlug desPlug = desFn.findPlug((MString("rotate") + aimStr), false);
    MPlug blendAnglePlug = nodeFn.findPlug(MString("inputRotate"), false);
    MPlug srcRotPlug;
    MPlug translatePlug = nodeFn.findPlug(MString("outputTranslate"), false); 
    MPlug translateDesPlug = desFn.findPlug((MString("translateZ")), false);
    MPlug blendTranslatePlug = nodeFn.findPlug(MString("inputTranslate"), false);
    MPlug srcTranslatePlug;
    MPlug srcWorldMatPlug = srcFn.findPlug(MString("worldMatrix"), false).elementByLogicalIndex(0);
    MPlug worldMatPlug = nodeFn.findPlug(MString("worldMatrix"), false);
    MPlug desPWorldMatPlug = desFn.findPlug(MString("parentMatrix"), false).elementByLogicalIndex(0);
    MPlug pWorldMatPlug = nodeFn.findPlug(MString("drivenParentMatrix"), false);
    MPlug radiusPlug = nodeFn.findPlug(MString("radius"), false);
    mDagModifier.newPlugValueFloat(radiusPlug, dis);
    status = mDagModifier.connect(srcWorldMatPlug, worldMatPlug);
    status = mDagModifier.connect(desPWorldMatPlug, pWorldMatPlug);
    mDagModifier.doIt();

    if (desPlug.isConnected())
    {
        MGlobal::executeCommand(MString("setAttr -lock false ") + desPlug.name() + MString("; "), false, true);
        srcRotPlug = desPlug.source();
        status = mDagModifier.disconnect(srcRotPlug, desPlug);
        status = mDagModifier.connect(srcRotPlug, blendAnglePlug);
    }
    if (desPlug.isLocked())
    {
        MGlobal::executeCommand(MString("setAttr -lock false ") + desPlug.name() + MString("; "), false, true);
    }

    if (translateDesPlug.isConnected())
    {
        MGlobal::executeCommand(MString("setAttr -lock false ") + translateDesPlug.name() + MString("; "), false, true);
        srcTranslatePlug = translateDesPlug.source();
        status = mDagModifier.disconnect(srcTranslatePlug, translateDesPlug);
        status = mDagModifier.connect(srcTranslatePlug, blendTranslatePlug);
    }
    if (translateDesPlug.isLocked())
    {
        MGlobal::executeCommand(MString("setAttr -lock false ") + translateDesPlug.name() + MString("; "), false, true);
    }

    status = mDagModifier.connect(srcPlug, matPlug);
    status = mDagModifier.connect(rotPlug, desPlug);
    status = mDagModifier.connect(translatePlug, translateDesPlug);
    MPlug plug_txHlpr = origSrcPlug.child(0);
    MPlug plug_tyHlpr = origSrcPlug.child(1);
    MPlug plug_tzHlpr = origSrcPlug.child(2);
    plug_txHlpr.getValue(txB);
    plug_tyHlpr.getValue(tyB);
    plug_tzHlpr.getValue(tzB);
    mDagModifier.doIt();
    mDagModifier.deleteNode(mTempObj);
    mDagModifier.doIt();

    MVector drivenVec = MVector(txB, 0, tzB);
    MVector driverVec = MVector(0.0, 0.0, -1);
    MQuaternion offsetQuat = MQuaternion(driverVec.normal(), drivenVec.normal(), 1.0);
    MVector offsetVec = offsetQuat.asEulerRotation().asVector();
    MPlug offsetRotYPlug = nodeFn.findPlug(MString("offsetRotateY"), false);
    mDagModifier.newPlugValueDouble(offsetRotYPlug, offsetVec[1]);
    MGlobal::executeCommand(MString("connectAttr time1.outTime ") + nodeFn.name() + MString(".time ;"), false, true);
    mDagModifier.doIt();
    return(redoIt());
}

MStatus prVectorConstraintCmd::undoIt()
{
    MGlobal::executeCommandOnIdle(MString("delete ") + nodeName + MString("; "), false);
    return(mDagModifier.undoIt());
    return(mDGModifier.undoIt());
}

MStatus prVectorConstraintCmd::redoIt()
{
    mDGModifier.doIt();
    mDagModifier.doIt();
    MFnDependencyNode shapeFn(mShapeObj);
    MFnDagNode tempShapeFnB(mTempObjB);
    MFnDagNode tempShapeFn(mTempObj);
    shapeFn.setName(nodeName);
    return(MS::kSuccess);
}

bool prVectorConstraintCmd::isUndoable() const
{
    return(true);
}

void* prVectorConstraintCmd::Creator()
{
    return(new prVectorConstraintCmd());
}

MString prVectorConstraintCmd::CommandName()
{
    return(MEL_COMMAND);
}

MSyntax prVectorConstraintCmd::CreateSyntax()
{
    MSyntax syntax;
    syntax.setObjectType(MSyntax::kSelectionList);
    syntax.useSelectionAsDefault(true);
    return(syntax);
}

MString prVectorConstraintCmd::GetShapeName(const MString& transformName)
{
    const int substringStart = prVectorConstraint::GetTypeName().length();
    const int substringEnd = transformName.length() - 1;

    MString number = transformName.substring(substringStart, substringEnd);

    return(prVectorConstraint::GetTypeName() + "Shape" + number);
}
