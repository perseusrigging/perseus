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
#include "prSupportJntsCmd.h"

static const MString MEL_COMMANDS = "prSupportJnts";
MString prSupportJntsCmd::nodeNameS;
MSelectionList jntsSelectList;

prSupportJntsCmd::prSupportJntsCmd() :
    MPxCommand()
{
}

prSupportJntsCmd::~prSupportJntsCmd()
{
}

MStatus prSupportJntsCmd::doIt(const MArgList& args)
{
    MStatus status;
    MGlobal::getActiveSelectionList(jntsSelectList);
    MDoubleArray posB;
    MString jntOnCrv = "import maya.cmds as cmds\ndriverJnt=cmds.ls(sl=1)[0]\ntry:\n    childJnt=cmds.listRelatives(driverJnt,c=1)[0]\nexcept:\n    childJnt=driverJnt\ntx=cmds.getAttr(childJnt+'.tx')\nif isinstance(tx, list):    tx=tx[0]\n";
    jntOnCrv += "ty=cmds.getAttr(childJnt+'.ty')\nif isinstance(ty, list):    ty=ty[0]\ntz=cmds.getAttr(childJnt+'.tz')\nif isinstance(tz, list):    tz=tz[0]\nsize=tx\nif(abs(ty)>abs(tx) and abs(ty)>abs(tz)):\n    size=ty\nelif(abs(tz)>abs(tx) and abs(tz)>abs(ty)):\n    size=tz\n";
    jntOnCrv += "if(abs(size)<0.001):\n    size=1.0;\n";
    jntOnCrv += "parentJnt=cmds.listRelatives(driverJnt,p=1)[0]\n";
    jntOnCrv += "if(abs(ty)>abs(tx) or abs(tz)>abs(tx)):\n    if(abs(ty)>abs(tz) and endJnt==0):\n        offsetJnt=driverJnt+'_hlpr_offset'\n        cmds.joint(n=offsetJnt)\n";
    jntOnCrv += "        cmds.parent(offsetJnt,parentJnt)\n        cmds.joint(n=offsetJnt+'_end')\n        cmds.pointConstraint(childJnt,offsetJnt+'_end')\n        cmds.setAttr(offsetJnt+'.radius',1.5)\n";
    jntOnCrv += "        cmds.joint(offsetJnt,zso=1, ch=1, e=1, oj='xyz', secondaryAxisOrient='yup')\n        cmds.delete(offsetJnt+'_end')\n        cmds.parentConstraint(driverJnt,offsetJnt,mo=1)\n";
    jntOnCrv += "        cmds.scaleConstraint(driverJnt,offsetJnt,mo=1)\n        cmds.select(offsetJnt,r=1)\n        driverJnt=offsetJnt\n";
    jntOnCrv += "rVals=cmds.getAttr(driverJnt+'.r')[0]\nrSum=rVals[0]+rVals[1]+rVals[2]\nif rSum!=0:\n    hlprBaseJnt=driverJnt+'_hlpr_offset'\n    cmds.joint(n=hlprBaseJnt)\n";
    jntOnCrv += "    cmds.parent(hlprBaseJnt,parentJnt)\n    cmds.parentConstraint(driverJnt,hlprBaseJnt,mo=1)\n    cmds.connectAttr(driverJnt+'.s',hlprBaseJnt+'.s',f=1)\nhlprJnt=driverJnt+'_hlpr_root'\ncmds.joint(n=hlprJnt)\n";
    jntOnCrv += "cmds.parent(hlprJnt,parentJnt)\nradius=cmds.getAttr(driverJnt+'.radius')\nblendNode='prPairBlend_'+driverJnt\n";
    jntOnCrv += "try:\n    cmds.delete(blendNode)\nexcept:\n    pass\ncmds.select(driverJnt, hlprJnt, r = 1)\ncmds.prPairBlend()\n";
    jntOnCrv += "cmds.setAttr(driverJnt+'.overallIntensity', size)\nif rSum!=0:\n";
    jntOnCrv += "    cmds.connectAttr(hlprBaseJnt+'.r',blendNode+'.inRotation',f=1)\ncmds.setAttr(hlprJnt+'.radius',radius*2)\ncmds.setAttr(hlprJnt+'.overrideEnabled', 1)\ncmds.setAttr(hlprJnt+'.overrideColor', 17)\n";
    jntOnCrv += "cmds.select(hlprJnt,r=1)\nforwardJnt=driverJnt+'_forwrd'\nbackwardJnt=driverJnt+'_backward'\nupwardJnt=driverJnt+'_upward'\ndownwardJnt=driverJnt+'_downward'\nspringJnt=driverJnt+'_spring'\n";
    jntOnCrv += "cmds.joint(n=forwardJnt)\ncmds.joint(n=backwardJnt)\ncmds.joint(n=upwardJnt)\ncmds.joint(n=downwardJnt)\ncmds.joint(n=springJnt)\n";
    jntOnCrv += "cmds.parent(backwardJnt,upwardJnt,downwardJnt,springJnt,hlprJnt)\ncmds.connectAttr(driverJnt+'.forward',forwardJnt+'.ty',f=1)\n";
    jntOnCrv += "cmds.connectAttr(driverJnt+'.backward',backwardJnt+'.ty',f=1)\ncmds.connectAttr(driverJnt+'.upward',upwardJnt+'.tz',f=1)\n";
    jntOnCrv += "cmds.connectAttr(driverJnt+'.downward',downwardJnt+'.tz',f=1)\ncmds.connectAttr(driverJnt+'.tySpring',springJnt+'.ty',f=1)\ncmds.connectAttr(driverJnt+'.tzSpring',springJnt+'.tz',f=1)\n";
    jntOnCrv += "cmds.connectAttr(driverJnt+'.s',forwardJnt+'.s',f=1)\ncmds.connectAttr(driverJnt+'.s',backwardJnt+'.s',f=1)\ncmds.connectAttr(driverJnt+'.s',upwardJnt+'.s',f=1)\ncmds.connectAttr(driverJnt+'.s',downwardJnt+'.s',f=1)\ncmds.connectAttr(driverJnt+'.s',springJnt+'.s',f=1)\n";

    if (jntsSelectList.length() > 0)
    {
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

MStatus prSupportJntsCmd::undoIt()
{
    for (int i = 0; i < jntsSelectList.length(); i++)
    {
        MString crvName;
        MObject selectedObjs;
        jntsSelectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
        crvName = srcDag.name();
        MString jntOnCrv = "import maya.cmds as cmds\n";
        jntOnCrv += "try:\n    cmds.delete('" + crvName + "_hlpr_offset')\nexcept:\n    pass\n";
        jntOnCrv += "try:\n    cmds.delete('" + crvName + "_hlpr_offset_hlpr_root')\nexcept:\n    pass\n";
        jntOnCrv += "try:\n    cmds.delete('" + crvName + "_hlpr_offset_hlpr_offset')\nexcept:\n    pass\n";
        jntOnCrv += "cmds.delete('" + crvName + "_hlpr_root')\ncmds.delete('prSimplePairBlend_" + crvName + "')\n";
        jntOnCrv += "attrs = 'blend roll yaw pitch outRotateX outRotateY outRotateZ forward backward upward downward overallIntensity forwardIntensity backwardIntensity upwardIntensity downwardIntensity springIntensity springStiffness springDamping startFrame tySpring tzSpring'.split()\n";
        jntOnCrv += "for item in attrs:\n    try:\n         cmds.deleteAttr('" + crvName + "', attribute=item)\n    except:\n        pass";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

MStatus prSupportJntsCmd::redoIt()
{
    MStatus status;
    MString crvName;
    for (int i = 0; i < jntsSelectList.length(); i++)
    {
        MObject selectedObjs;
        jntsSelectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
        crvName = srcDag.name();
        MString jntOnCrv = "import maya.mel as mm\ncmds.select('" + crvName + "', r=1)\nmm.eval('prSupportJnts;')";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

bool prSupportJntsCmd::isUndoable() const
{
    return(true);
}

void* prSupportJntsCmd::Creator()
{
    return(new prSupportJntsCmd());
}

MString prSupportJntsCmd::CommandName()
{
    return(MEL_COMMANDS);
}

MSyntax prSupportJntsCmd::CreateSyntax()
{
    MSyntax syntax;
    return(syntax);
}
