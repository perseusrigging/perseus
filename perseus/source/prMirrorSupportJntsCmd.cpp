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
#include "prMirrorSupportJntsCmd.h"


static const MString MEL_COMMANDS = "prMirrorSupportJnts";
MString prMirrorSupportJntsCmd::nodeNameS;
MSelectionList jntsSelectListC;
prMirrorSupportJntsCmd::prMirrorSupportJntsCmd() :
    MPxCommand()
{
}
prMirrorSupportJntsCmd::~prMirrorSupportJntsCmd()
{
}

MStatus prMirrorSupportJntsCmd::doIt(const MArgList& args)
{
    MStatus status;
    MGlobal::getActiveSelectionList(jntsSelectListC);
    MDoubleArray posB;
    MString jntOnCrv = "import maya.cmds as cmds\ndef mirrorSupportJnts():\n    jnts=cmds.ls(sl=1)\n    attr='tx'\n    jnt=jnts[0]\n";
    jntOnCrv += "    for a in 'xyz':\n        plug=cmds.listConnections(jnt+'.t'+a,p=1)\n        if plug!=None:\n            attr='t'+a\n            srcPlug=plug\n";
    jntOnCrv += "    cmds.duplicate(jnt,n=jnt+'_neg')\n    cmds.createNode('multiplyDivide',n=jnt+'_mult_neg')\n    cmds.connectAttr(srcPlug[0],jnt+'_mult_neg.input1X')\n";
    jntOnCrv += "    cmds.setAttr(jnt+'_mult_neg.input2X',-1.0)\n    cmds.connectAttr(jnt+'_mult_neg.outputX',jnt+'_neg.'+attr)\n";
    jntOnCrv += "jnts=cmds.ls(sl=1)\nfor jnt in jnts:\n    cmds.select(jnt,r=1)\n    mirrorSupportJnts()";
    if (jntsSelectListC.length() > 0)
    {
        MGlobal::executePythonCommand(MString(jntOnCrv), false, true);
    }
    return(MS::kSuccess);
}

MStatus prMirrorSupportJntsCmd::undoIt()
{
    for (int i = 0; i < jntsSelectListC.length(); i++)
    {
        MString crvName;
        MObject selectedObjs;
        jntsSelectListC.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
        crvName = srcDag.name();
        MString jntOnCrv = "import maya.cmds as cmds\n";
        jntOnCrv += "try:\n    cmds.delete('" + crvName + "+'_neg'')\nexcept:\n    pass\n";
        jntOnCrv += "try:\n    cmds.delete('" + crvName + "_mult_neg')\nexcept:\n    pass\n";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

MStatus prMirrorSupportJntsCmd::redoIt()
{
    MStatus status;
    MString crvName;
    for (int i = 0; i < jntsSelectListC.length(); i++)
    {
        MObject selectedObjs;
        jntsSelectListC.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
        crvName = srcDag.name();
        MString jntOnCrv = "import maya.mel as mm\ncmds.select('" + crvName + "', r=1)\nmm.eval('prMirrorSupportJnts;')";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

bool prMirrorSupportJntsCmd::isUndoable() const
{
    return(true);
}

void* prMirrorSupportJntsCmd::Creator()
{
    return(new prMirrorSupportJntsCmd());
}

MString prMirrorSupportJntsCmd::CommandName()
{
    return(MEL_COMMANDS);
}

MSyntax prMirrorSupportJntsCmd::CreateSyntax()
{
    MSyntax syntax;
    return(syntax);
}
