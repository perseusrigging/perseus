
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
#include "prRopeRigCmd.h"

static const MString MEL_COMMAND = "prRopeRig";
MString prRopeRigCmd::nodeName;
MSelectionList crvSelectListB;
const char* prRopeRigCmd::kDriversShort = "-d";
const char* prRopeRigCmd::kDriversLong = "-drivers";

prRopeRigCmd::prRopeRigCmd() :
    drivers_(1)
{
}

prRopeRigCmd::~prRopeRigCmd()
{
}

MStatus prRopeRigCmd::doIt(const MArgList& args)
{
    MStatus status;
    MArgDatabase argData(syntax(), args);

    if (argData.isFlagSet(kDriversShort)) {
        drivers_ = (int)argData.flagArgumentDouble(kDriversShort, 0, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    MGlobal::getActiveSelectionList(crvSelectListB);
    MDoubleArray posB;
    MString jntOnCrv = "import maya.cmds as cmds\ncrv,aimCrv=cmds.ls(sl=1)\n";
    jntOnCrv += "jntCount=";
    jntOnCrv += drivers_;
    jntOnCrv += "\n";
    jntOnCrv += "curve_length = cmds.arclen(crv)\nropeNode=crv+'_prRopeRig'\nropeNode=cmds.createNode('prRopeRig',n=ropeNode)\n";
    
    jntOnCrv += "cmds.setAttr(ropeNode+'.length',curve_length)\ncmds.setAttr(ropeNode+'.division',jntCount)\ncrvShape=cmds.listRelatives(crv,type='shape')[0]\naimCrvShape=cmds.listRelatives(aimCrv,type='shape')[0]\n";

    jntOnCrv += "cmds.connectAttr('{0}.worldSpace[0]'.format(crvShape),ropeNode+'.inputCurve')\ncmds.connectAttr('{0}.worldSpace[0]'.format(aimCrvShape),ropeNode+'.inputAimCurve')\n";
    
    jntOnCrv += "cmds.connectAttr('time1.outTime',ropeNode+'.time')\njntGrp=ropeNode+'_jnt_grp'\ncmds.group(n=jntGrp,em=1)\n";
    
    jntOnCrv += "for i in range(jntCount + 1):\n";
    jntOnCrv += "    cmds.select(cl=1)\n    jnt='{0}_{1}_jnt'.format(ropeNode,i)\n    cmds.joint(n=jnt)\n    cmds.connectAttr('{0}.rotation[{1}]'.format(ropeNode,i),'{0}.rotate'.format(jnt,i))\n";
    
    jntOnCrv += "    cmds.connectAttr('{0}.translate[{1}]'.format(ropeNode,i),'{0}.translate'.format(jnt,i))\n    cmds.connectAttr('{0}.scale[{1}]'.format(ropeNode,i),'{0}.scale'.format(jnt,i))\n    cmds.parent(jnt,jntGrp)\n";
    
    jntOnCrv += "loc=ropeNode+'_loc'\ncmds.spaceLocator(n=loc)\ncmds.addAttr(loc, ln='maxStretch', max=10, dv=10, at='double', min=1,keyable=True)\ncmds.addAttr(loc, ln='minStretch', max=1, dv=.1, at='double', min=0,keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='collisionMeshStrength',dv=1.0, at='double',keyable=True)\ncmds.addAttr(loc, ln='startFrame', dv=0, at='long',keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='springIntensity', max=1, dv=0.0, at='double', min=0,keyable=True)\ncmds.addAttr(loc, ln='springStiffness', max=1, dv=.025, at='double', min=0,keyable=True)\ncmds.addAttr(loc, ln='springDamping', max=1, dv=.01, at='double', min=0,keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='springElastic', max=1, dv=.5, at='double', min=0,keyable=True)\ncmds.addAttr(loc, ln='shrinkStrength', max=10, dv=0.0, at='double', min=-10,keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='expandStrength', max=10, dv=0.0, at='double', min=-10,keyable=True)\ncmds.addAttr(loc, ln='bulgeStrengthX', max=10, dv=0.0, at='double', min=-10,keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='bulgeStrengthY', max=10, dv=0.0, at='double', min=-10,keyable=True)\ncmds.addAttr(loc, ln='bulgeStrengthZ', max=10, dv=0.0, at='double', min=-10,keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='bulgeOffset', max=10, dv=0.0, at='double', min=-10,keyable=True)\ncmds.addAttr(loc, ln='position', dv=0.0, at='double', keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='size', max=1, dv=1.0, at='double', min=0.01,keyable=True)\ncmds.addAttr(loc, ln='roll',  dv=0.0, at='doubleAngle',keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='startTwist',  dv=0.0, at='doubleAngle',keyable=True)\ncmds.addAttr(loc, ln='midTwist',  dv=0.0, at='doubleAngle',keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='endTwist',  dv=0.0, at='doubleAngle',keyable=True)\ncmds.addAttr(loc, ln='startBias',  dv=0.0, at='doubleAngle',keyable=True)\ncmds.addAttr(loc, ln='endBias',  dv=0.0, at='doubleAngle',keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='rebuild', at='bool',keyable=True)\n";
    jntOnCrv += "cmds.addAttr(loc, ln='degree',max=7, dv=3, at='long', min=1,keyable=True)\ncmds.addAttr(loc, ln='pushVectorRadius', dv=0.0, at='double', keyable=True)\ncmds.addAttr(loc, ln='pushVectorStrength', dv=1.0, at='double', keyable=True)\ncmds.connectAttr(loc+'.maxStretch',ropeNode+'.maxStretch')\ncmds.connectAttr(loc+'.minStretch',ropeNode+'.minStretch')\n";
    jntOnCrv += "cmds.connectAttr(loc+'.startFrame',ropeNode+'.startFrame')\ncmds.connectAttr(loc+'.springIntensity',ropeNode+'.springIntensity')\ncmds.connectAttr(loc+'.springStiffness',ropeNode+'.springStiffness')\ncmds.connectAttr(loc+'.springElastic',ropeNode+'.springElastic')\ncmds.connectAttr(loc+'.shrinkStrength',ropeNode+'.shrinkStrength')\n";
    jntOnCrv += "cmds.connectAttr(loc+'.expandStrength',ropeNode+'.expandStrength')\ncmds.connectAttr(loc+'.bulgeStrengthX',ropeNode+'.bulgeStrengthX')\ncmds.connectAttr(loc+'.bulgeStrengthY',ropeNode+'.bulgeStrengthY')\ncmds.connectAttr(loc+'.bulgeStrengthZ',ropeNode+'.bulgeStrengthZ')\ncmds.connectAttr(loc+'.bulgeOffset',ropeNode+'.bulgeOffset')\n";
    jntOnCrv += "cmds.connectAttr(loc+'.position',ropeNode+'.position')\n";
    jntOnCrv += "cmds.connectAttr(loc+'.size',ropeNode+'.size')\ncmds.connectAttr(loc+'.roll',ropeNode+'.roll')\ncmds.connectAttr(loc+'.startTwist',ropeNode+'.startTwist')\ncmds.connectAttr(loc+'.midTwist',ropeNode+'.midTwist')\n";
    jntOnCrv += "cmds.connectAttr(loc+'.endTwist',ropeNode+'.endTwist')\ncmds.connectAttr(loc+'.startBias',ropeNode+'.startBias')\ncmds.connectAttr(loc+'.endBias',ropeNode+'.endBias')\ncmds.connectAttr(loc+'.rebuild',ropeNode+'.rebuild')\ncmds.connectAttr(loc+'.degree',ropeNode+'.degree')\n";
    jntOnCrv += "cmds.connectAttr(loc+'.collisionMeshStrength',ropeNode+'.collisionMeshStrength')\ncmds.connectAttr(loc+'.pushVectorRadius',ropeNode+'.pushVectorRadius')\ncmds.connectAttr(loc+'.pushVectorStrength',ropeNode+'.pushVectorStrength')\ncmds.connectAttr(loc+'.worldMatrix[0]',ropeNode+'.globalScale')";
    if (crvSelectListB.length() > 0)
    {
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

MStatus prRopeRigCmd::undoIt()
{
    //for (int i = 0; i < crvSelectListB.length(); i++)
    //{
        MString crvName;
        MObject selectedObjs;
        //crvSelectListB.getDependNode(i, selectedObjs);
        crvSelectListB.getDependNode(0, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
         crvName = srcDag.name();
        MString jntOnCrv = "if cmds.objExists('" + crvName + "_prRopeRig_jnt_grp'):\n    cmds.delete('"+ crvName+"_prRopeRig_jnt_grp');\nif cmds.objExists('" + crvName + "_prRopeRig_loc'):\n    cmds.delete('" + crvName + "_prRopeRig_loc');";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    //}
    return(MS::kSuccess);
}

MStatus prRopeRigCmd::redoIt()
{
    MStatus status;
    MString crvName;
    MString aimCrvName;
    //for (int i = 0; i < crvSelectListB.length(); i++)
    //{
        MObject selectedObjs;
        MObject selectedObjsB;
        //crvSelectListB.getDependNode(i, selectedObjs);
        crvSelectListB.getDependNode(0, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
        crvName = srcDag.name();

        crvSelectListB.getDependNode(1, selectedObjsB);
        MFnDependencyNode selObjB(selectedObjsB);
        MFnDagNode srcDagB(selectedObjsB);
        aimCrvName = srcDagB.name();

        MString jntOnCrv = "import maya.mel as mm\ncmds.select('" + crvName + "','" + aimCrvName + "', r=1)\nmm.eval('prRopeRig -d "+ drivers_+";')";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    //}
    return(MS::kSuccess);
}

bool prRopeRigCmd::isUndoable() const
{
    return(true);
}

void* prRopeRigCmd::Creator()
{
    return(new prRopeRigCmd());
}

MString prRopeRigCmd::CommandName()
{
    return(MEL_COMMAND);
}

MSyntax prRopeRigCmd::CreateSyntax()
{
    MSyntax syntax;
    syntax.addFlag(kDriversShort, kDriversLong, MSyntax::kLong);
    return(syntax);
}
