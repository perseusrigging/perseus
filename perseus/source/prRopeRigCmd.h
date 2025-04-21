#pragma once
#include <maya/MDagModifier.h>
#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnAnimCurve.h>

class prRopeRigCmd : public MPxCommand
{
public:
    prRopeRigCmd();
    virtual ~prRopeRigCmd() override;
    virtual MStatus doIt(const MArgList& args) override;
    virtual MStatus undoIt() override;
    virtual MStatus redoIt() override;
    virtual bool isUndoable() const override;
    static const char* kDriversShort;
    static const char* kDriversLong;
    static void* Creator();
    static MString CommandName();
    static MSyntax CreateSyntax();
private:
    static MString nodeName;
    int drivers_;
};
