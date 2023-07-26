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
#include "prSquashCmd.h"

static const MString MEL_COMMAND = "prSquash";
MString prSquashCmd::nodeName;
MSelectionList selectList;

prSquashCmd::prSquashCmd() :
    MPxCommand()
{
}

prSquashCmd::~prSquashCmd()
{
}

MStatus prSquashCmd::doIt(const MArgList& args)
{
    MStatus status;
    MGlobal::getActiveSelectionList(selectList);
    MDoubleArray posB;
    if (selectList.length() > 0)
    {
        MGlobal::executeCommand(MString("exactWorldBoundingBox;"), posB, false, false);
        float posX = (posB[0] + posB[3]) * .5;
        float posY = (posB[1] + posB[4]) * .5;
        float posZ = (posB[2] + posB[5]) * .5;
        float posYB= (posB[4] - posB[1]) * .5;
        MString componentMode = "string $components[];string $initialSel[];$initialSel= `ls - sl`;string $objList[] = `ls - hl`;$compMod = size($objList);if ($compMod != 0){$components =`polyListComponentConversion - tv`;select $objList;}";
        MGlobal::executeCommand(componentMode, false, false);
        MStringArray result;
        MGlobal::executeCommand(MString("deformer - type prSquash;"), result, true, true);
        MString name = result[0];
        nodeName = name;
        componentMode = "string $obj;string $prSqName = \"" + name + "\";string $vtx;if ($compMod != 0){for ($obj in $objList){$vtx = $obj + \".vtx[*]\";percent - v 0.0 $prSqName $vtx;}";
        componentMode += "percent - v 1.0 $prSqName $components;select -r $initialSel;}";
        MGlobal::executeCommand(componentMode, false, false);
        MString prSquash_root = name+"_root.translate";
        MString prSquash_loc = name+"_locShape.localPositionY";
        MString prSquash_locScale = name+"_locShape.localScale";
        MString prSquash_Min = name + ".boundingMin";
        MString prSquash_Max = name + ".boundingMax";
        MString prSquash_Height = name + ".initialHeight";
        MString prSquash_Global = name + ".localGlobalWeight";
        MGlobal::executeCommand(MString("setAttr \"") + prSquash_root + MString("\" ") + posX + MString(" ") + posY + MString(" ") + posZ + MString(";"), true, true);
        MGlobal::executeCommand(MString("setAttr \"") + prSquash_loc + MString("\" ")  + posYB + MString(";"), true, true);
        MGlobal::executeCommand(MString("setAttr \"") + prSquash_locScale + MString("\" ") + posYB + MString(" ") + posYB + MString(" ") + posYB + MString(";"), true, true);
        MGlobal::executeCommand(MString("setAttr \"") + prSquash_Min + MString("\" ") + posB[0] + MString(" ") + posB[1] + MString(" ") + posB[2] + MString(";"), true, true);
        MGlobal::executeCommand(MString("setAttr \"") + prSquash_Max + MString("\" ") + posB[3] + MString(" ") + posB[4] + MString(" ") + posB[5] + MString(";"), true, true);
       MString chkLattice = "string $selShape[]=`listRelatives -c`;if(size($selShape)!=0){string $selType=`objectType $selShape[0]`;if($selType==\"lattice\"){";
        chkLattice += MString("setAttr \"") + prSquash_Global + MString("\" 1;}}");
        MGlobal::executeCommand(chkLattice, false, true);    
    }
    return(MS::kSuccess);
}

MStatus prSquashCmd::undoIt()
{
    for (int i = 0; i < selectList.length(); i++)
    {
        MObject selectedObjs;
        selectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        if (selectedObjs.apiType() == MFn::kTransform)
        {
            MPlug plugs = selObj.findPlug("scaleX", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("scaleY", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("scaleZ", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("translateX", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("translateY", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("translateZ", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("rotateX", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("rotateY", false);
            plugs.setLocked(false);
            plugs = selObj.findPlug("rotateZ", false);
            plugs.setLocked(false);
        }
    }
    return(MS::kSuccess);
}

MStatus prSquashCmd::redoIt()
{
    MStatus status;
    MGlobal::executeCommand(MString("parent "+ nodeName+"_loc "+ nodeName+"_root; setAttr " + nodeName + "_loc.t 0 0 0; "), true, true);
    for (int i = 0; i < selectList.length(); i++)
    {
        MObject selectedObjs;
        selectList.getDependNode(i, selectedObjs);
        MFnDependencyNode selObj(selectedObjs);
        if (selectedObjs.apiType() == MFn::kTransform)
        {

            MPlug plugs = selObj.findPlug("scaleX", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("scaleY", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("scaleZ", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("translateX", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("translateY", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("translateZ", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("rotateX", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("rotateY", false);
            plugs.setLocked(true);
            plugs = selObj.findPlug("rotateZ", false);
            plugs.setLocked(true);
        }
    }
    return(MS::kSuccess);
}

bool prSquashCmd::isUndoable() const
{
    return(true);
}

void* prSquashCmd::Creator()
{
    return(new prSquashCmd());
}

MString prSquashCmd::CommandName()
{
    return(MEL_COMMAND);
}

MSyntax prSquashCmd::CreateSyntax()
{
    MSyntax syntax;
    return(syntax);
}
