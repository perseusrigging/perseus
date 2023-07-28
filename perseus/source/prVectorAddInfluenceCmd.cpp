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
#include "prVectorAddInfluenceCmd.h"

static const MString MEL_COMMAND = "prVectorAddInfluence";
MString prVectorAddInfluenceCmd::nodeName;
MSelectionList jntSelectList;

prVectorAddInfluenceCmd::prVectorAddInfluenceCmd() :
    MPxCommand()
{
}

prVectorAddInfluenceCmd::~prVectorAddInfluenceCmd()
{
}

MStatus prVectorAddInfluenceCmd::doIt(const MArgList& args)
{
    MStatus status;
    MGlobal::getActiveSelectionList(jntSelectList);
    MDoubleArray posB;
    MString AimFix= "sel=cmds.ls(sl=1)\nsize=len(sel)\n";
    AimFix += "rootJnt=sel[size - 1]\n";
    AimFix += "for i in range(0, (size - 1)) :\n    cmds.select(sel[i], sel[size - 1], r = 1)\n    cmds.prVectorConstraint()\n    ";
    AimFix += "prVConst='prVectorConstraint_'+rootJnt+'_'+sel[i]\n    cmds.connectAttr(rootJnt+'.driverMin', prVConst+'.driverMin', f=1)\n    cmds.connectAttr(rootJnt+'.driverMax', prVConst+'.driverMax', f=1)\n    cmds.connectAttr(rootJnt+'.min', prVConst+'.min', f=1)\n    cmds.connectAttr(rootJnt+'.max', prVConst+'.max', f=1)\n    ";
    AimFix += "cmds.connectAttr(rootJnt+'.startFrame', prVConst+'.startFrame', f=1)\n    cmds.connectAttr(rootJnt+'.springIntensity', prVConst+'.springIntensity', f=1)\n    cmds.connectAttr(rootJnt+'.springStiffness', prVConst+'.springStiffness', f=1)\n    cmds.connectAttr(rootJnt+'.springDamping', prVConst+'.springDamping', f=1)\n    ";
    AimFix += "cmds.connectAttr(rootJnt+'.outputTranslateWeight', prVConst+'.outputTranslateWeight', f=1)\n    cmds.connectAttr(rootJnt+'.outputRotateWeight', prVConst+'.outputRotateWeight', f=1)\n    ";
    AimFix += "cmds.connectAttr(rootJnt+'.radiusPower', prVConst+'.radiusPower', f=1)\n    cmds.connectAttr(rootJnt+'.falloffPower', prVConst+'.falloffPower', f=1)\n";
    if (jntSelectList.length() > 0)
    {
        MGlobal::executePythonCommand(MString(AimFix), false, true);
    }
    return(redoIt());
}

MStatus prVectorAddInfluenceCmd::undoIt()
{
    for (int i = 0; i < jntSelectList.length(); i++)
    {

        MObject selectedObjs;
        jntSelectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
    }

    return(MS::kSuccess);
}

MStatus prVectorAddInfluenceCmd::redoIt()
{
    MStatus status;
    for (int i = 0; i < jntSelectList.length(); i++)
    {

        MObject selectedObjs;
        jntSelectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
    }
    return(MS::kSuccess);
}

bool prVectorAddInfluenceCmd::isUndoable() const
{
    return(true);
}

void* prVectorAddInfluenceCmd::Creator()
{
    return(new prVectorAddInfluenceCmd());
}

MString prVectorAddInfluenceCmd::CommandName()
{
    return(MEL_COMMAND);
}

MSyntax prVectorAddInfluenceCmd::CreateSyntax()
{
    MSyntax syntax;
    return(syntax);
}
