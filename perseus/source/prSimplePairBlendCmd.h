#pragma once
#pragma once
#include <maya/MDagModifier.h>
#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDagNode.h>

class prSimplePairBlendCmd : public MPxCommand
{
public:
    prSimplePairBlendCmd();
    virtual ~prSimplePairBlendCmd() override;
    virtual MStatus doIt(const MArgList& args) override;
    virtual MStatus undoIt() override;
    virtual MStatus redoIt() override;
    virtual bool isUndoable() const override;
    static void* Creator();
    static MString CommandName();
    static MSyntax CreateSyntax();
private:
    static MString nodeName;
    MDagModifier mDagModifier;
    MDGModifier mDGModifier;
    MFnDagNode mFnDagNode;
    MObject mShapeObj;
};
