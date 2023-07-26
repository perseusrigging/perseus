
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
#include "prVectorJntsOnCrvCmd.h"

static const MString MEL_COMMAND = "prVectorJntsOnCrv";
MString prVectorJntsOnCrvCmd::nodeName;
MSelectionList crvSelectList;
const char* prVectorJntsOnCrvCmd::kDriversShort = "-d";
const char* prVectorJntsOnCrvCmd::kDriversLong = "-drivers";

prVectorJntsOnCrvCmd::prVectorJntsOnCrvCmd() :
    drivers_(1)
{
}

prVectorJntsOnCrvCmd::~prVectorJntsOnCrvCmd()
{
}

MStatus prVectorJntsOnCrvCmd::doIt(const MArgList& args)
{
    MStatus status;
    MArgDatabase argData(syntax(), args);

    if (argData.isFlagSet(kDriversShort)) {
        drivers_ = (int)argData.flagArgumentDouble(kDriversShort, 0, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    MGlobal::getActiveSelectionList(crvSelectList);
    MDoubleArray posB;
    MString jntOnCrv = "import maya.cmds as cmds\nfrom math import pow,sqrt\ndrvnJnts=[]\nname= cmds.ls(sl=1)[0]\nrotation=cmds.getAttr(name+'.r')[0]\ntranslation=cmds.getAttr(name+'.t')[0]\ncmds.setAttr(name+'.r',0,0,0)\ncmds.setAttr(name+'.t',0,0,0)\ncmds.makeIdentity(n=0, s=1, r=0, t=0, apply=True, pn=1)\nbbox=cmds.exactWorldBoundingBox(name)\nh=(abs(bbox[3]-bbox[0])+abs(bbox[4]-bbox[1])+abs(bbox[5]-bbox[2]))/4\n";
    jntOnCrv += "crvShape=cmds.listRelatives(name, s=True)[0]\n";
    jntOnCrv += "loc=";
    jntOnCrv += drivers_;
    jntOnCrv += "\n";
    jntOnCrv += "b = cmds.ls(sl = 1)\na = name\n\ncmds.joint(n = '%s_root' % name)\ncmds.joint(n = '%s_drivenJnt_grp' % name)\ncmds.joint(n = '%s_driver_grp' % name)\n";
    jntOnCrv += "locJtGp = cmds.joint(n='%s_driver%s_rootJntEnv'%(name, 0))\njtA = cmds.joint(locJtGp, n='%s_driver%s_rootJnt'%(name, 0))\njtB = cmds.joint(n='%s_driver%s_rootJntEnd'%(name, 0))\ncmds.setAttr('%s_driver%s_rootJntEnd.ty'%(name, 0), h*-1)\n";
    jntOnCrv += "rootJnt='%s_driver%s_rootJnt'%(name, 0)\ncmds.addAttr(rootJnt, ln='driverMin', max=180, dv=40, at='double', min=-180)\ncmds.setAttr(rootJnt+'.driverMin', e=1, keyable=True)\ncmds.addAttr(rootJnt, ln='driverMax', max=180, dv=130, at='double', min=-180)\ncmds.setAttr(rootJnt+'.driverMax', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='min', max=180, dv=0, at='double', min=0)\ncmds.setAttr(rootJnt+'.min', e=1, keyable=True)\ncmds.addAttr(rootJnt, ln='max', max=180, dv=125, at='double', min=0)\ncmds.setAttr(rootJnt+'.max', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='startFrame', dv=0, at='long')\ncmds.setAttr(rootJnt+'.startFrame', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='springIntensity', max=1, dv=0, at='double', min=0)\ncmds.setAttr(rootJnt+'.springIntensity', e=1, keyable=True)\ncmds.addAttr(rootJnt, ln='springStiffness', max=1, dv=0.25, at='double', min=0)\ncmds.setAttr(rootJnt+'.springStiffness', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='springDamping', max=1, dv=0.01, at='double', min=0)\ncmds.setAttr(rootJnt+'.springDamping', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='outputRotateWeight', max=1, dv=1.0, at='double', min=-1)\ncmds.setAttr(rootJnt+'.outputRotateWeight', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='outputTranslateWeight', max=100, dv=0, at='double', min=-100)\ncmds.setAttr(rootJnt+'.outputTranslateWeight', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='radiusPower', max=10, dv=1, at='double', min=0)\ncmds.setAttr(rootJnt+'.radiusPower', e=1, keyable=True)\n";
    jntOnCrv += "cmds.addAttr(rootJnt, ln='falloffPower', max=10, dv=0.5, at='double', min=0)\ncmds.setAttr(rootJnt+'.falloffPower', e=1, keyable=True)\n";
    jntOnCrv += "cmds.matchTransform('%s_driver_grp'%name, a, pos=1, rot=1)\ncv = cmds.ls('%s.cv[*]'%a, fl=1)\n";
    jntOnCrv += "for i in range(len(cv)):\n    poc = cmds.createNode('pointOnCurveInfo', n='%s%s_pointOnCurveInfo'%(name, i))\n    gp = cmds.joint(n='%s%s_drivenJnt_grp'%(name, i))\n    ";
    jntOnCrv += "cmds.setAttr('%s%s_drivenJnt_grp.drawStyle' % (name, i), 2)\n    ";
    jntOnCrv += "jt = cmds.joint(gp, n='%s%s_drivenJnt'%(name, i), rad=1.0)\n    jtA = cmds.joint(jt, n='%s%s_drivenEnd'%(name, i), rad=1.0)\n    cmds.setAttr('%s.t'%jtA, 0, h*-1, 0)\n    cmds.connectAttr('%s.ws'%crvShape, '%s.ic'%poc)\n    cmds.connectAttr('%s.p'%poc, '%s.t'%gp)\n    ";
    jntOnCrv += "cmds.setAttr('%s.top'%poc, 1)\n    if (cmds.getAttr('%s.form'%crvShape)==0):\n        cmds.setAttr('%s.pr'%poc, 1.0/(len(cv)-1)*i)\n    if (cmds.getAttr('%s.form'%crvShape)==2):\n        cmds.setAttr('%s.pr'%poc, 1.0/len(cv)*i)\n    ";
    jntOnCrv += "cmds.disconnectAttr('%s.p'%poc,'%s.t'%gp)\n    tx=cmds.getAttr('%s.tx'%gp)\n    cmds.setAttr('%s.tx'%gp,tx+.001)\n    cmds.parent(gp, '%s_drivenJnt_grp'%name)\n    cmds.setAttr('%s.r'%gp, 0, 0, 0)\n    ";
    jntOnCrv += "cmds.delete(poc)\n    ";
    jntOnCrv += "aim = cmds.aimConstraint('%s_driver_grp'%name, gp, u=[0, 1, 0], aim=[0, 0, 1], wut='objectrotation', wuo='%s_driver_grp'%name)\n    cmds.delete(aim)\n    cmds.setAttr('%s.rx'%gp, 0)\n    drvnJnts.append(jt)\ncmds.select(drvnJnts,'%s_driver%s_rootJnt'%(name, 0),r=1)\n";  
    jntOnCrv += "sel=cmds.ls(sl=1)\nsize=len(sel)\nif (size)>1:\n    for i in range(0,(size - 1)):\n        cmds.select(sel[i], sel[size - 1], r=1)\n        cmds.prVectorConstraint()\n        "; 
    jntOnCrv += "prVConst='prVectorConstraint_'+rootJnt+'_'+'%s%s_drivenJnt'%(name, i)\n        cmds.connectAttr(rootJnt+'.driverMin', prVConst+'.driverMin', f=1)\n        cmds.connectAttr(rootJnt+'.driverMax', prVConst+'.driverMax', f=1)\n        cmds.connectAttr(rootJnt+'.min', prVConst+'.min', f=1)\n        cmds.connectAttr(rootJnt+'.max', prVConst+'.max', f=1)\n        ";
    jntOnCrv += "cmds.connectAttr(rootJnt+'.startFrame', prVConst+'.startFrame', f=1)\n        ";
    jntOnCrv += "cmds.connectAttr(rootJnt+'.springIntensity', prVConst+'.springIntensity', f=1)\n        cmds.connectAttr(rootJnt+'.springStiffness', prVConst+'.springStiffness', f=1)\n        cmds.connectAttr(rootJnt+'.springDamping', prVConst+'.springDamping', f=1)\n        ";
    jntOnCrv += "cmds.connectAttr(rootJnt+'.radiusPower', prVConst+'.radiusPower', f=1)\n        cmds.connectAttr(rootJnt+'.falloffPower', prVConst+'.falloffPower', f=1)\n        ";
    jntOnCrv += "cmds.connectAttr(rootJnt+'.outputTranslateWeight', prVConst+'.outputTranslateWeight', f=1)\n        cmds.connectAttr(rootJnt+'.outputRotateWeight', prVConst+'.outputRotateWeight', f=1)\n    ";
    jntOnCrv += "cmds.setAttr('%s_drivenJnt_grp.drawStyle' % name, 1)\n";
    jntOnCrv += "cmds.parent('%s_driver_grp' % name, name + '_root')\n";
    jntOnCrv += "cmds.setAttr('%s_driver_grp.overrideEnabled'%name, 1)\ncmds.setAttr('%s_driver_grp.overrideColor'%name, 17)\n";
    jntOnCrv += "import maya.mel as mm\n";
    jntOnCrv += "if loc>1:\n    for i in range(1,loc):\n        ";
    jntOnCrv += "newJnt = '%s_driver%s_rootJnt' % (name, i)\n        newJntGrp=newJnt+'Env'\n        ";
    jntOnCrv += "cmds.duplicate(name+'_driver0_rootJntEnv',n=newJntGrp)\n        cmds.select(newJntGrp,r=1)\n        ";
    jntOnCrv += "mm.eval('searchReplaceNames \"0_rootJnt\" \"%s_rootJnt\" \"hierarchy\";' % i)\n        ";
    jntOnCrv += "cmds.select(drvnJnts, newJnt, r = 1)\n        cmds.prVectorAddInfluence()\n";
    jntOnCrv += "cmds.select(name , r = 1)\ncmds.setAttr(name+'.r',rotation[0],rotation[1],rotation[2])\ncmds.setAttr(name+'.t',translation[0],translation[1],translation[2])";
    if (crvSelectList.length() > 0)
    {
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

MStatus prVectorJntsOnCrvCmd::undoIt()
{
    for (int i = 0; i < crvSelectList.length(); i++)
    {
        MString crvName;
        MObject selectedObjs;
        crvSelectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
         crvName = srcDag.name();
        MString jntOnCrv = "items = cmds.listRelatives('"+ crvName+"', type = 'transform')\ncmds.delete(items)";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

MStatus prVectorJntsOnCrvCmd::redoIt()
{
    MStatus status;
    MString crvName;
    for (int i = 0; i < crvSelectList.length(); i++)
    {
        MObject selectedObjs;
        crvSelectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        MFnDagNode srcDag(selectedObjs);
        crvName = srcDag.name();
        MString jntOnCrv = "import maya.mel as mm\ncmds.select('" + crvName + "', r=1)\nmm.eval('prVectorJntsOnCrv -d "+ drivers_+";')";
        MGlobal::executePythonCommand(MString(jntOnCrv), false, false);
    }
    return(MS::kSuccess);
}

bool prVectorJntsOnCrvCmd::isUndoable() const
{
    return(true);
}

void* prVectorJntsOnCrvCmd::Creator()
{
    return(new prVectorJntsOnCrvCmd());
}

MString prVectorJntsOnCrvCmd::CommandName()
{
    return(MEL_COMMAND);
}

MSyntax prVectorJntsOnCrvCmd::CreateSyntax()
{
    MSyntax syntax;
    syntax.addFlag(kDriversShort, kDriversLong, MSyntax::kLong);
    return(syntax);
}
