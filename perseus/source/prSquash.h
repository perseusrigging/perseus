#pragma once
#define NOMINMAX
#include <maya/MTypeId.h>
#include <maya/MGlobal.h>
#include <maya/MTypes.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MItGeometry.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MDagModifier.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFloatArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnData.h>
#include <maya/MThreadPool.h>
#include <maya/MFnLattice.h>
#include <maya/MFnLatticeData.h>
#include <maya/MFnLatticeDeformer.h>
#include <maya/MTime.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MBoundingBox.h>
#include <math.h>
#include <algorithm>
#include <maya/MAngle.h>
#include <stdio.h>
#include <map>
#include <array>
#include <maya/MThreadPool.h>
//#include <maya/MPxGPUDeformer.h>
//#include <maya/MEvaluationNode.h>
//#include <maya/MGPUDeformerRegistry.h>
//#include <maya/MOpenCLInfo.h>
//#include <clew/clew_cl.h>

#define PI 3.14159265359


class prSquash : public MPxDeformerNode
{

public:
	prSquash();
	virtual ~prSquash();
	virtual MStatus deform(MDataBlock& dataBlock,
		MItGeometry& geomIter,
		const MMatrix& localToWorldMat,
		unsigned int multiIndex);
	virtual MStatus accessoryNodeSetup(MDagModifier& dagMod);
	virtual MObject& accessoryAttribute() const;
	virtual MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray);
	static void* creator();
	static MStatus initialize();
	void postConstructor();
	MStatus postConstructor_init_curveRamp(MObject& nodeObj,
		MObject& rampObj,
		int index,
		float position,
		float value,
		int interpolation);

public:
	static MTypeId GetTypeId();
	static MString GetTypeName();
	static MObject numTasks;
	static MObject aAutoKickGPU;
	static MStringArray menuItems;
	static MObject locatorWorldSpace;
	static MObject locatorUpWorldSpace;
	static MObject expandRamp;
	static MObject expandStrength;
	static MObject shrinkRamp;
	static MObject shrinkStrength;
	static  MObject aexpandMap;
	static  MObject ashrinkMap;
	static  MObject astretchMap;
	static  MObject asquashMap;
	static  MObject aPerGeometry;
	static  MObject bendStrength;
	static  MObject twistStrength;
	static  MObject twist;
	static  MObject stretchBendWeight;
	static  MObject aWorldMatrix;
	static MObject bendRamp;
	static MObject deformerRamp;
	static MObject stretchRamp;
	static MObject squashRamp;
	static MObject twistRamp;
	static MObject boundingMin;
	static MObject boundingMax;
	static  MObject deformerArray;
	static MObject twistArray;
	static  MObject expandArray;
	static  MObject shrinkArray;
	static  MObject bendArray;
	static  MObject stretchArray;
	static  MObject squashArray;
	static MObject xStrength;
	static MObject zStrength;
	static MObject positionArray;
	static  MObject localGlobalWeight;
	static  MObject initialHeight;
private:
	MStatus JumpToElement(MArrayDataHandle& hArray, unsigned int index);
	MStatus GetInputMesh(MDataBlock& data, unsigned int geomIndex, MObject* oInputMesh);
	// Store everything per input geometry
	std::map<unsigned int, bool> initialized_;
	std::map<unsigned int, bool> dirtyMap_;
	std::map<unsigned int, MIntArray> membership_;
	std::map<unsigned int, MFloatArray> weights_;
	std::map<unsigned int, MFloatArray> expandMap_;
	std::map<unsigned int, MFloatArray> shrinkMap_;
	std::map<unsigned int, MFloatArray> stretchMap_;
	std::map<unsigned int, MFloatArray> squashMap_;
	static int  isInitialized;
	std::array<float, 10000> arrExpandMTF;
	std::array<float, 10000> arrShrinkMTF;
	std::array<float, 10000> arrBendMTF;
	std::array<float, 10000> arrDeformerMTF;
	std::array<float, 10000> arrTwistMTF;
	std::array<float, 10000> arrStretchMTF;
	std::array<float, 10000> arrSquashMTF;
	std::map<unsigned int, MPointArray> refPointss_;
	float _rampBool;
};
