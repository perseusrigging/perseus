#pragma once
#include <maya/MDagModifier.h>
#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDagNode.h>

class prVectorConstraintCmd : public MPxCommand
{
public:
    prVectorConstraintCmd();
    virtual ~prVectorConstraintCmd() override;
    virtual MStatus doIt(const MArgList& args) override;
    virtual MStatus undoIt() override;
    virtual MStatus redoIt() override;
    virtual bool isUndoable() const override;
    static void* Creator();
    static MString CommandName();
    static MSyntax CreateSyntax();
private:
    MString GetShapeName(const MString& transformName);
    MDagModifier mDagModifier;
    MDGModifier mDGModifier;
    MFnDagNode mFnDagNode;
    MObject mTransformObj;
    MObject mShapeObj;
    MObject mTempObj;
    MObject mTempObjB;
    MString hlprName;
    static MString nodeName;
};
