#include <maya/MFnPlugin.h>
#include "prVectorConstraint.h"
#include "prVectorConstraintCmd.h"
#include "prSquashCmd.h"
#include "prPairBlend.h"
#include "prPairBlendCmd.h"
#include "prSquash.h"
#include "prVectorAddInfluenceCmd.h"
#include "prVectorJntsOnCrvCmd.h"
#include "prSupportJntsCmd.h"
#include <string>
#include <iostream>
#include "prSimplePairBlend.h"
#include "prSimplePairBlendCmd.h"
#include "prSimpleSupportJntsCmd.h"
#include "prMirrorSupportJntsCmd.h"
#include "prRopeRig.h"
#include "prRopeRigCmd.h"

MString nodeVersion("1.0.2");

MStatus initializePlugin(MObject obj)
{
    MStatus status;

    MFnPlugin fnPlugin(obj, "Mohammad Jafarian - www.perseusrigging.com", nodeVersion.asChar(), "Any");

    status = fnPlugin.registerNode(prRopeRig::GetTypeName(),
        prRopeRig::GetTypeId(),
        prRopeRig::creator,
        prRopeRig::initialize);
    if (!status)
    {
        MGlobal::displayError("Failed to register node: " + prRopeRig::GetTypeName());
        return(status);
    }

    status = fnPlugin.registerCommand(prRopeRigCmd::CommandName(),
        prRopeRigCmd::Creator,
        prRopeRigCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prRopeRigCmd::CommandName());
        return(status);
    }


    status = fnPlugin.registerNode(prSquash::GetTypeName(),
        prSquash::GetTypeId(),
        prSquash::creator,
        prSquash::initialize,
        MPxNode::kDeformerNode);

    if (!status)
    {
        MGlobal::displayError("Failed to register node: " + prSquash::GetTypeName() + "\n");
        return(status);
    }

    status = fnPlugin.registerCommand(prSquashCmd::CommandName(),
        prSquashCmd::Creator,
        prSquashCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prSquashCmd::CommandName());
        return(status);
    }


    status = fnPlugin.registerNode(prVectorConstraint::GetTypeName(),
        prVectorConstraint::GetTypeId(),
        prVectorConstraint::creator,
        prVectorConstraint::initialize);
    if (!status)
    {
        MGlobal::displayError("Failed to register node: " + prVectorConstraint::GetTypeName());
        return(status);
    }


    status = fnPlugin.registerCommand(prVectorConstraintCmd::CommandName(),
        prVectorConstraintCmd::Creator,
        prVectorConstraintCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prVectorConstraintCmd::CommandName());
        return(status);
    }

    status = fnPlugin.registerNode(prPairBlend::GetTypeName(),
        prPairBlend::GetTypeId(),
        prPairBlend::creator,
        prPairBlend::initialize);
    if (!status)
    {
        MGlobal::displayError("Failed to register node: " + prPairBlend::GetTypeName());
        return(status);
    }

    status = fnPlugin.registerCommand(prPairBlendCmd::CommandName(),
        prPairBlendCmd::Creator,
        prPairBlendCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prPairBlendCmd::CommandName());
        return(status);
    }


    status = fnPlugin.registerCommand(prVectorAddInfluenceCmd::CommandName(),
        prVectorAddInfluenceCmd::Creator,
        prVectorAddInfluenceCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prVectorAddInfluenceCmd::CommandName());
        return(status);
    }

    status = fnPlugin.registerCommand(prVectorJntsOnCrvCmd::CommandName(),
        prVectorJntsOnCrvCmd::Creator,
        prVectorJntsOnCrvCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prVectorJntsOnCrvCmd::CommandName());
        return(status);
    }

    status = fnPlugin.registerCommand(prSupportJntsCmd::CommandName(),
        prSupportJntsCmd::Creator,
        prSupportJntsCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prSupportJntsCmd::CommandName());
        return(status);
    }

    status = fnPlugin.registerNode(prSimplePairBlend::GetTypeName(),
        prSimplePairBlend::GetTypeId(),
        prSimplePairBlend::creator,
        prSimplePairBlend::initialize);
    if (!status)
    {
        MGlobal::displayError("Failed to register node: " + prSimplePairBlend::GetTypeName());
        return(status);
    }

    status = fnPlugin.registerCommand(prSimplePairBlendCmd::CommandName(),
        prSimplePairBlendCmd::Creator,
        prSimplePairBlendCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prSimplePairBlendCmd::CommandName());
        return(status);
    }

    status = fnPlugin.registerCommand(prSimpleSupportJntsCmd::CommandName(),
        prSimpleSupportJntsCmd::Creator,
        prSimpleSupportJntsCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prSimpleSupportJntsCmd::CommandName());
        return(status);
    }

    status = fnPlugin.registerCommand(prMirrorSupportJntsCmd::CommandName(),
        prMirrorSupportJntsCmd::Creator,
        prMirrorSupportJntsCmd::CreateSyntax);
    if (!status)
    {
        MGlobal::displayError("Failed to register command: " + prMirrorSupportJntsCmd::CommandName());
        return(status);
    }

    MString about = "Perseus version : 1.0.2\\n\\n";
    about+="Copyright(c) Mohammad Jafarian\\n";
    about += "Copyright(c) perseusrigging.com\\n";
    about += "email : contact@perseusrigging.com\\n\\n";
    about += "Permission is hereby granted, free of charge, to any person obtaining a\\n";
    about += "copy of this softwareand associated documentation files(the \\\"Software\\\"),\\n";
    about += "to deal in the Software without restriction, including without limitation\\n";
    about += "the rights to use, copy, modify, merge, publish, distribute, sublicense,\\n";
    about += "and /or sell copies of the Software, and to permit persons to whom the\\n";
    about += "Software is furnished to do so, subject to the following conditions :\\n";
    about += "The above copyright noticeand this permission notice shall be included in\\n";
    about += "all copies or substantial portions of the Software.\\n\\n";
    about += "THE SOFTWARE IS PROVIDED \\\"AS IS\\\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\\n";
    about += "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\\n";
    about += "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL\\n";
    about += "THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR\\n";
    about += "OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,\\n";
    about += "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE\\n";
    about += "OR OTHER DEALINGS IN THE SOFTWARE.\\n\\n";
    about += "Copyright(c) 2017 - 2025 Mohammad Jafarian\\n";
    about += "email : contact@perseusrigging.com\\n";
    MString prSquashUrl = "string $prSquashUrl = \"system( \\\"start explorer /n,/e, http://www.perseusrigging.com//prsquash\\\")\";";
    prSquashUrl += "string $prVectorUrl = \"system( \\\"start explorer /n,/e, http://www.perseusrigging.com//prvectorConstraint\\\")\";";
    prSquashUrl += "string $prPairBlendUrl = \"system( \\\"start explorer /n,/e, http://www.perseusrigging.com//prpairblend\\\")\";";
    prSquashUrl += "string $prRopeRigUrl = \"system( \\\"start explorer /n,/e, http://www.perseusrigging.com//prroperig\\\")\";";
    MGlobal::executeCommand(prSquashUrl, false, false);


    // Adding menus through C++ API to avoid having to include more complicated MEL/Python script setups for now
    MString addMenuStr = "global proc  prAbout(){confirmDialog -title \"Confirm\" -message \""+about+"\" - button \"Ok\";} global proc  prSkirtRig(){int $text;string $result = `promptDialog -title \"prVectorConstraint\" -message \"How many driver joints do you need ?\"\n -button \"OK\" -button \"Cancel\" -text 1 -defaultButton \"OK\" -cancelButton \"Cancel\" -dismissString \"Cancel\"`; if ($result == \"OK\") {$text = `promptDialog -query -text`;prVectorJntsOnCrv -d $text;}}";
    addMenuStr += "global proc  prRopeRigUI() {int $text; string $result = `promptDialog - title \"prRopeRig\" -message \"How many joints should be placed along the curve? ?\"\n -button \"OK\" -button \"Cancel\" -text 10 -defaultButton \"OK\" -cancelButton \"Cancel\" -dismissString \"Cancel\"`; if ($result == \"OK\") {$text = `promptDialog -query -text`;prRopeRig -d $text;}}";
    addMenuStr += "if(`menu -exists prRigTools`){deleteUI prRigTools;}menu -label \"prRigTools\"-parent MayaWindow prRigTools;menuItem -divider 1 -dividerLabel \"Deform\"; menuItem -label \"Squash Deformer\"-annotation \"Select object(s) to squash\"-command \"prSquash; \";menuItem -divider 1 -dividerLabel \"Vector Constraint\";menuItem -label \"Vector constraint\"-annotation \"Select a curve\"-command \"prSkirtRig(); \";     menuItem - divider 1 - dividerLabel \"Rope Rig\";menuItem -label \"Rope Rig\"-annotation \"Select two curves\"-command \"prRopeRigUI(); \";menuItem -divider 1 -dividerLabel      menuItem -divider 1 -dividerLabel \"Yaw,Pitch,Roll/pairBlend\";    menuItem -label \"pairBlend\"-annotation \"pr pairBlend->select one joint->add yaw / pitch / roll, Two joints->pairBlend\"-command \"prPairBlend; \";     menuItem -label \"addSupportJoints\"-annotation \"pr supportJnts->select one joint->add blend & support Joints\"-command \"prSupportJnts; \";     menuItem -label \"addSimpleSupportJoints\"-annotation \"pr simpleSupportJnts->select one joint->add blend & support Joints\"-command \"prSimpleSupportJnts; \";";
    addMenuStr += "menuItem - label \"addMirrorJoints\"-annotation \"pr mirrorSupportJnts->select one joint->add mirror Joints\"-command \"prMirrorSupportJnts; \";";
    addMenuStr += "menuItem -divider 1 ; menuItem -subMenu true -label \"Help\";menuItem -label \"prSquash Deformer\" -command $prSquashUrl;menuItem -label \"prVector Constraint\" -command $prVectorUrl;menuItem -label \"prPairBlend\" -command $prPairBlendUrl;menuItem -label \"prRoperig\" -command $prRopeRigUrl;";
    addMenuStr += "menuItem -label \"About\" -command \"prAbout();\";setParent - menu ..; ";
    addMenuStr += "setParent - menu prRigTools; menuSet - addMenu prRigTools modelingMenuSet; menuSet - addMenu prRigTools riggingMenuSet; menuSet - addMenu prRigTools animationMenuSet; ";
    MGlobal::executeCommand(addMenuStr, false, false);
    // fix prSquash warning deformer
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash weights");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash expandMap");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash shrinkMap");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash stretchMap");
    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer prSquash squashMap");
    return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status;

    MFnPlugin fnPlugin(obj);

    status = fnPlugin.deregisterCommand(prMirrorSupportJntsCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prMirrorSupportJntsCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterCommand(prSimpleSupportJntsCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prSimpleSupportJntsCmd::CommandName());
        return(status);
    }


    status = fnPlugin.deregisterCommand(prSimplePairBlendCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prSimplePairBlendCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterNode(prSimplePairBlend::GetTypeId());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister node: " + prSimplePairBlend::GetTypeName());
        return(status);
    }

    status = fnPlugin.deregisterCommand(prSupportJntsCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prSupportJntsCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterCommand(prVectorJntsOnCrvCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prVectorJntsOnCrvCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterCommand(prVectorAddInfluenceCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prVectorAddInfluenceCmd::CommandName());
        return(status);
    }


    status = fnPlugin.deregisterCommand(prPairBlendCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prPairBlendCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterNode(prPairBlend::GetTypeId());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister node: " + prPairBlend::GetTypeName());
        return(status);
    }
    status = fnPlugin.deregisterCommand(prVectorConstraintCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prVectorConstraintCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterNode(prVectorConstraint::GetTypeId());
    CHECK_MSTATUS_AND_RETURN_IT(status);



    status = fnPlugin.deregisterCommand(prSquashCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prSquashCmd::CommandName());
        return(status);
    }


    status = fnPlugin.deregisterNode(prSquash::GetTypeId());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister node: " + prSquash::GetTypeName() + "\n");
        return(status);
    }


    status = fnPlugin.deregisterCommand(prRopeRigCmd::CommandName());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister command: " + prRopeRigCmd::CommandName());
        return(status);
    }

    status = fnPlugin.deregisterNode(prRopeRig::GetTypeId());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister node: " + prRopeRig::GetTypeName());
        return(status);
    }


    //fnPlugin.removeMenuItem(prSquash::menuItems);
    MString removeMenuStr = "if(`menu -exists prRigTools`){deleteUI prRigTools;}";
    MGlobal::executeCommand(removeMenuStr, false, false);
    return MS::kSuccess;
}