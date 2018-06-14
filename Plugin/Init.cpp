/////////////////////////////////////////////////////////////
// UD1414 Assignment 2: Maya Plug-in in C++
// Fredrik Linde TA15
// Init.cpp
/////////////////////////////////////////////////////////////

#include <iostream>
#include "maya_includes.h"

#pragma comment(lib, "Shared.lib")
#include <vector>
#include <Shared\ComLib.h>

using namespace std;

// Callback manager class to help with callbacks
#pragma region CallbackManagerClass

class CallbackManager {

public:

	CallbackManager();
	~CallbackManager();

	static void Shutdown();
	static void RemoveCallback(MCallbackId id);

	static void RegisterCallbacks();
	static void IterateScene();

	// Creates a timer
	static MStatus CreateTimerCallback(float period);

	// Takes a node to check for name changes on
	static MStatus CreateNameChangedCallback(MObject &node, MMessage::MNodeStringFunction func);

	// Takes a string to specify what node type to check after if added to the scene
	static MStatus CreateAddedNodeCallback(MString type);
	static MStatus CreateRemoveNodeCallback(MString type);

	// Special case for attribute changed callback on created mesh
	static MStatus CreateChangedAttributeMeshCallback(MObject &node, MNodeMessage::MAttr2PlugFunction func);

	// Takes a node to look for attribute changes on and a function pointer responsible for handling that change
	static MStatus CreateChangedAttributeCallback(MObject &node, MNodeMessage::MAttr2PlugFunction func);

	// Creates a callback for topology changes
	static MStatus CreateTopologyChangedCallback(MObject &node, MMessage::MNodeFunction func);

	// Post Render callback for camera updates
	static MStatus CreatePostRenderCallback(MString panelName, MUiMessage::MStringFunction func);

	// Transform callback functions
	static void attributeMeshTransformCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);
	static void attributeLightTransformCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);

	// Temporary callback for added mesh nodes
	static void attributeMeshAddedCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);

	static void attributeVertexCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);
	static void attributeMaterialCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);
	static void attributeLightCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);

	// Topology callback function
	static void topologyChangedCallback(MObject &node, void *clientData);

	// Post render function
	static void postrenderCallback(const MString& panelName, void * data);

	static void meshAdded(MFnMesh& mesh);
	static void lightAdded(MFnPointLight& light);

	// Transformation update functions
	static void meshTransformUpdate(MFnTransform& transform);
	static void lightTransformUpdate(MFnTransform& transform);

	// Recursive transform update for parent/child transformations
	static void RecursiveTransformUpdate(MFnTransform& transform);

	// Create functions
	static void CreateVertexData(MFnMesh& mesh, vector<Vertex> &vertices);
	static void CreateMaterialData(MObject &shaderNode, MsgMaterial& material);

private:

	// Timer function
	static void timerCallback(float elapsedTime, float lastTime, void* clientData);

	// Added and removed node callback functions
	static void addedCallback(MObject &node, void *clientData);
	static void removedCallback(MObject &node, void *clientData);

	// Name change callback functions
	static void nameMeshCallback(MObject &node, const MString &str, void *clientData);
	static void nameLightCallback(MObject &node, const MString &str, void *clientData);
	static void nameMaterialCallback(MObject &node, const MString &str, void *clientData);

	// Helper functions for indexing and getting material data
	static MIntArray GetLocalIndex(MIntArray & getVertices, MIntArray & getTriangle);
	static MObject findShader(MObject& setNode);
	static MObject getShader(MFnMesh& mesh);

	static MCallbackIdArray myCallbackArray;
	static ComLib comLib;
	static MCallbackId meshCreatedID;

};

// When working with static member variables in a class, we can't initialize them in the class. We can only initialize them outside the class in the cpp.
// This is because there can only be one instance of them in the current project
MCallbackIdArray CallbackManager::myCallbackArray;
ComLib CallbackManager::comLib("MyFileMappingObject", 100 * 1 << 20, TYPE::CONSUMER);
MCallbackId CallbackManager::meshCreatedID;

CallbackManager::CallbackManager() {


}

CallbackManager::~CallbackManager() {


}

void CallbackManager::Shutdown() {
	
	MMessage::removeCallbacks(myCallbackArray);
	comLib.Unmap();
}

void CallbackManager::RemoveCallback(MCallbackId id) {

	MMessage::removeCallback(id);
}

void CallbackManager::RegisterCallbacks() {

	MStatus status = MS::kSuccess;

	// Create timer
	status = CreateTimerCallback(5);

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}

	// Check callbacks for meshes
	status = CreateAddedNodeCallback("mesh");

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}

	// Check callbacks for transforms
	status = CreateAddedNodeCallback("transform");

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}

	// Check callbacks for cameras
	status = CreateAddedNodeCallback("camera");

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}

	// Check callbacks for cameras
	status = CreateAddedNodeCallback("light");

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}

	// Check callbacks for cameras
	CreatePostRenderCallback("modelPanel4", CallbackManager::postrenderCallback);

	// Check callbacks for removing meshes
	status = CreateRemoveNodeCallback("mesh");

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}

	// Check callbacks for removing point lights
	status = CreateRemoveNodeCallback("light");

	if (status != MS::kSuccess) {

		MGlobal::displayInfo(status.errorString());
	}
}

void CallbackManager::IterateScene() {

	// Iterate the scene at start to find already existing meshes and lights

	MStatus status = MS::kSuccess;

	// Iterate scene for meshes
	MItDag meshIt(MItDag::kDepthFirst, MFn::kMesh, &status);

	if (status == MS::kSuccess) {

		while (!meshIt.isDone())
		{

			// attach the function set to the object
			MFnMesh mesh(meshIt.item());

			// only want non-history items
			if (!mesh.isIntermediateObject()) {

				// Add the mesh
				CallbackManager::meshAdded(mesh);
			}

			// get next mesh
			meshIt.next();

		}

	}

	else {

		MGlobal::displayInfo(status.errorString());
	}

	// Iterate scene for lights
	MItDag lightIt(MItDag::kDepthFirst, MFn::kPointLight, &status);

	if (status == MS::kSuccess) {

		while (!lightIt.isDone())
		{

			// attach the function set to the object
			MFnPointLight pointLight(lightIt.item());

			// only want non-history items
			if (!pointLight.isIntermediateObject()) {

				// Add the point light
				CallbackManager::lightAdded(pointLight);
			}

			// get next mesh
			lightIt.next();

		}

	}

	else {

		MGlobal::displayInfo(status.errorString());
	}
}

MObject CallbackManager::findShader(MObject& setNode) {

	MFnDependencyNode fnNode(setNode);

	// We must go through the surfaceShader plug to find the connected shaders of this mesh
	MPlug shaderPlug = fnNode.findPlug("surfaceShader");

	// Shader plug should never be null for a valid shading group node set!
	if (!shaderPlug.isNull()) {

		MPlugArray connectedPlugs;
		bool asSource = false;
		bool asDestination = true;

		//  Find the shading node for the given shading group set node in the plug
		shaderPlug.connectedTo(connectedPlugs, asDestination, asSource);

		if (connectedPlugs.length() != 1) {

			MGlobal::displayError("Error getting shader");

		}

		else {

			// We're only interested in the currently assigned shader
			return connectedPlugs[0].node();

		}
	}

	// If not even the default lambert shader was found, return a null object
	// The returned node must be controlled outside the function call!
	MGlobal::displayInfo("No shader was found in the given shading group node");
	return MObject::kNullObj;
}

MObject CallbackManager::getShader(MFnMesh& mesh) {

	MObjectArray sets;
	MObjectArray comps;
	MObject shaderNode;
	unsigned int instanceNum = mesh.dagPath().instanceNumber();

	// The documentation states the following about this way of reaching the shader node

	// "Returns all the sets connected to the specified instance of this mesh.
	// For each set in the "sets" array there is a corresponding entry in the "comps" array which are all the components in that set.
	// If the entire object is in a set, then the corresponding entry in the comps array will have no elements in it".

	// Note: This method will only work with a MFnMesh function set which has been initialized with an MFn::kMesh.

	// The set we're after in this case is the shading node group for the mesh. Within that, we can find the assigned shader

	if (!mesh.getConnectedSetsAndMembers(instanceNum, sets, comps, true)) {

		MGlobal::displayError("ERROR: MFnMesh::getConnectedSetsAndMembers");

	}

	// There will always be a default lambert shader in the Maya scene that automatically is assigned to any created mesh
	// With that said, there will always be an initial shading group node in the scene
	if (sets.length())
	{

		MObject set = sets[0];
		MObject comp = comps[0];
		MStatus status;

		// From the given shading group node set where the shader is expected to be found, we look for the plug "surfaceShader"
		shaderNode = findShader(set);
	}

	return shaderNode;
}

MStatus CallbackManager::CreateTimerCallback(float period) {

	MStatus status = MS::kSuccess;

	MCallbackId timerId = MTimerMessage::addTimerCallback(
		period,
		timerCallback,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(timerId) == MS::kSuccess) {


		}

		else {

			return MS::kFailure;
		}
	}

	return status;
}

MStatus CallbackManager::CreateNameChangedCallback(MObject &node, MMessage::MNodeStringFunction func) {

	MStatus status = MS::kSuccess;

	// Registers a callback for name changes on the given node
	MCallbackId nameId = MNodeMessage::addNameChangedCallback(
		node,
		func,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(nameId) == MS::kSuccess) {


		}
	}

	return status;

}

MStatus CallbackManager::CreateAddedNodeCallback(MString type) {

	MStatus status = MS::kSuccess;

	// For this callback we want to be able to check whenever a node of the given type was added to the scene
	MCallbackId addedId = MDGMessage::addNodeAddedCallback(
		addedCallback,
		type,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(addedId) == MS::kSuccess) {


		}
	}

	return status;
}

MStatus CallbackManager::CreateRemoveNodeCallback(MString type) {

	MStatus status = MS::kSuccess;

	// For this callback we want to be able to check whenever a node of the given type was removed from the scene
	MCallbackId removedId = MDGMessage::addNodeRemovedCallback(
		removedCallback,
		type,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(removedId) == MS::kSuccess) {


		}
	}

	return status;
}

MStatus CallbackManager::CreateChangedAttributeMeshCallback(MObject &node, MNodeMessage::MAttr2PlugFunction func) {

	MStatus status = MS::kSuccess;

	// For this callback, we want to use a temporary functions to gather data from a mesh when it's added to the scene
	meshCreatedID = MNodeMessage::addAttributeChangedCallback(
		node,
		func,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(meshCreatedID) == MS::kSuccess) {


		}
	}

	return status;
}

MStatus CallbackManager::CreateChangedAttributeCallback(MObject &node, MNodeMessage::MAttr2PlugFunction func) {

	MStatus status = MS::kSuccess;

	// For this callback, we want the node to track attribute changes on and the function pointer to handle this change
	MCallbackId attributeId = MNodeMessage::addAttributeChangedCallback(
		node,
		func,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(attributeId) == MS::kSuccess) {


		}
	}

	return status;
}

MStatus CallbackManager::CreateTopologyChangedCallback(MObject &node, MMessage::MNodeFunction func) {

	MStatus status = MS::kSuccess;

	// For this callback, we want to track changes in a mesh node's topology
	MCallbackId topologyId = MPolyMessage::addPolyTopologyChangedCallback(
		node,
		func,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(topologyId) == MS::kSuccess) {


		}
	}

	return status;
}

MStatus CallbackManager::CreatePostRenderCallback(MString panelName, MUiMessage::MStringFunction func) {

	MStatus status = MS::kSuccess;

	// Callback used to capture the viewport matrices
	MCallbackId viewportID = MUiMessage::add3dViewPostRenderMsgCallback(
		panelName,
		func,
		NULL,
		&status
	);

	if (status == MS::kSuccess) {

		if (myCallbackArray.append(viewportID) == MS::kSuccess) {


		}
	}

	return status;
}

void CallbackManager::timerCallback(float elapsedTime, float lastTime, void* clientData) {

	MString msg("Elapsed time: ");
	msg += elapsedTime;
	MGlobal::displayInfo(msg);
}

void CallbackManager::nameMeshCallback(MObject &node, const MString &str, void *clientData) {

	// Define message header
	MsgHeader header = { MAYA_MSG_TYPE::MESH_RENAMED };

	// MFnDependencyNode allows the creation and manipulation of dependency graph nodes
	MFnDependencyNode depNode = node;

	// We get the old name from the string
	MString oldName = str;

	// Print the names
	MGlobal::displayInfo("New name to the object: " + depNode.name());
	MGlobal::displayInfo("Object that triggered the callback: " + oldName);

	// Construct the mesh message
	MsgMesh msgMesh;

	// Copy the new name and the old name
	strcpy_s(msgMesh.name, depNode.name().asChar());
	strcpy_s(msgMesh.oldName, oldName.asChar());

	MFnDagNode dagNode = node;

	// Print the path
	MGlobal::displayInfo("Path: " + dagNode.fullPathName());

	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	memcpy(msg + msgLength, &msgMesh, sizeof(MsgMesh));
	msgLength += sizeof(MsgMesh);

	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::nameLightCallback(MObject &node, const MString &str, void *clientData) {

	// Define message header
	MsgHeader header = { MAYA_MSG_TYPE::LIGHT_RENAMED };

	// MFnDependencyNode allows the creation and manipulation of dependency graph nodes
	MFnDependencyNode depNode = node;

	// We get the old name from the string
	MString oldName = str;

	// Print the names
	MGlobal::displayInfo("New name to the object: " + depNode.name());
	MGlobal::displayInfo("Object that triggered the callback: " + oldName);

	// Construct the light message
	MsgLight msgLight;

	// Copy the new name and the old name
	strcpy_s(msgLight.name, depNode.name().asChar());
	strcpy_s(msgLight.oldName, oldName.asChar());

	MFnDagNode dagNode = node;

	// Print the path
	MGlobal::displayInfo("Path: " + dagNode.fullPathName());

	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	memcpy(msg + msgLength, &msgLight, sizeof(MsgLight));
	msgLength += sizeof(MsgLight);

	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::nameMaterialCallback(MObject &node, const MString &str, void *clientData) {

	// Define message header
	MsgHeader header = { MAYA_MSG_TYPE::MATERIAL_RENAMED };

	// MFnDependencyNode allows the creation and manipulation of dependency graph nodes
	MFnDependencyNode depNode = node;

	// We get the old name from the string
	MString oldName = str;

	// Print the names
	MGlobal::displayInfo("New name to the object: " + depNode.name());
	MGlobal::displayInfo("Object that triggered the callback: " + oldName);

	// Construct the material message
	MsgMaterial msgMaterial;

	// Copy the new name and the old name
	strcpy_s(msgMaterial.name, depNode.name().asChar());
	strcpy_s(msgMaterial.oldName, oldName.asChar());

	MFnDagNode dagNode = node;

	// Print the path
	MGlobal::displayInfo("Path: " + dagNode.fullPathName());

	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	memcpy(msg + msgLength, &msgMaterial, sizeof(MsgMaterial));
	msgLength += sizeof(MsgMaterial);

	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::addedCallback(MObject &node, void *clientData) {

	// First validate the node types
	// Then add callbacks for the verified node

	// Check if the added node is a mesh
	if (node.apiType() == MFn::Type::kMesh) {

		MStatus status = MS::kSuccess;

		// Get the dag node
		MFnDagNode dagNode(node, &status);

		if (status == MS::kSuccess) {

			MGlobal::displayInfo("The mesh " + dagNode.name() + " has been added!");

			// Create an attribute changed callback to use in order to wait for the mesh to be ready
			CreateChangedAttributeMeshCallback(node, attributeMeshAddedCallback);

		}

		else {

			MGlobal::displayInfo(status.errorString());
		}

	}

	// Check if the added node is a point light
	if (node.apiType() == MFn::Type::kPointLight) {

		MStatus status = MS::kSuccess;

		// Get the dag node
		MFnDagNode dagNode(node, &status);

		if (status == MS::kSuccess) {

			MGlobal::displayInfo("The point light " + dagNode.name() + " has been added!");

			MFnPointLight light(node, &status);

			if (status == MS::kSuccess) {

				lightAdded(light);
			}

			else {

				MGlobal::displayInfo(status.errorString());
			}
		}

		else {

			MGlobal::displayInfo(status.errorString());
		}
	}

}

void CallbackManager::removedCallback(MObject &node, void *clientData) {

	// First validate the node types
	// Then send messages holding the name and type of the removed object

	// Check if the added node is a mesh
	if (node.apiType() == MFn::Type::kMesh) {

		MStatus status = MS::kSuccess;

		// Get the dag node
		MFnDagNode dagNode(node, &status);

		if (status == MS::kSuccess) {

			MGlobal::displayInfo("The mesh " + dagNode.name() + " has been removed!");

			// Create mesh message header
			MsgHeader header = { MAYA_MSG_TYPE::MESH_REMOVED };

			// Create the mesh message
			MsgMesh msgMesh;

			// Get mesh name
			strcpy_s(msgMesh.name, dagNode.name().asChar());

			size_t msgSize = 10 * 1 << 20;
			char* msg = new char[msgSize];
			size_t msgLength = 0;

			// Memcpy header
			memcpy(msg, &header, sizeof(MsgHeader));
			msgLength += sizeof(MsgHeader);

			// Memcpy MsgMesh
			memcpy(msg + msgLength, &msgMesh, sizeof(MsgMesh));
			msgLength += sizeof(MsgMesh);

			comLib.send(msg, msgLength);

		}

		else {

			MGlobal::displayInfo(status.errorString());
		}
	}

	if (node.apiType() == MFn::Type::kPointLight) {

		MStatus status = MS::kSuccess;

		// Get the dag node
		MFnDagNode dagNode(node, &status);

		if (status == MS::kSuccess) {

			MGlobal::displayInfo("The pointlight " + dagNode.name() + " has been removed!");

			// Create light message header
			MsgHeader header = { MAYA_MSG_TYPE::LIGHT_REMOVED };

			// Create the mesh message
			MsgLight msgLight;

			// Get light name
			strcpy_s(msgLight.name, dagNode.name().asChar());

			size_t msgSize = 10 * 1 << 20;
			char* msg = new char[msgSize];
			size_t msgLength = 0;

			// Memcpy header
			memcpy(msg, &header, sizeof(MsgHeader));
			msgLength += sizeof(MsgHeader);

			// Memcpy MsgMesh
			memcpy(msg + msgLength, &msgLight, sizeof(MsgLight));
			msgLength += sizeof(MsgLight);

			comLib.send(msg, msgLength);
		}

		else {

			MGlobal::displayInfo(status.errorString());
		}
	}
}

void CallbackManager::attributeMeshAddedCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData) {

	MStatus status = MS::kSuccess;

	// Make an MObject from the plug node
	MObject object(plug.node());

	// Create mesh
	MFnMesh mesh(object, &status);

	if (status == MS::kSuccess) {

		// Add the mesh
		meshAdded(mesh);

		// Unregister the callback
		RemoveCallback(meshCreatedID);

	}

	// If mesh failed to create, it wasn't ready
	else {

		MGlobal::displayInfo(status.errorString());
	}
}

void CallbackManager::attributeMeshTransformCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData) {

	// Check if attribute was set
	if (msg & MNodeMessage::kAttributeSet) {

		MStatus status = MS::kSuccess;

		// Create transform from the MObject
		MFnTransform transform(plug.node(), &status);

		if (status == MS::kSuccess) {

			RecursiveTransformUpdate(transform);
		}

		else {

			MGlobal::displayInfo(status.errorString());
		}

	}

	else {

		return;
	}
}

void CallbackManager::attributeLightTransformCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData) {

	// Check if attribute was set
	if (msg & MNodeMessage::kAttributeSet) {

		MStatus status = MS::kSuccess;

		// Create transform from the MObject
		MFnTransform transform(plug.node(), &status);

		if (status == MS::kSuccess) {

			lightTransformUpdate(transform);

		}

		else {

			MGlobal::displayInfo(status.errorString());
		}

	}

	else {

		return;
	}
}

void CallbackManager::attributeVertexCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData) {

	// Check if attribute was set
	if (msg & MNodeMessage::kAttributeSet) {

		MStatus status = MS::kSuccess;

		// Was the attribute a point?
		MPlug vertexPlug = MFnDependencyNode(plug.node()).findPlug("pnts", &status);

		if (status == MS::kSuccess) {

			// Declare status check
			status = MS::kSuccess;

			// Get the index of the vertex that has been modified
			int index = plug.logicalIndex();

			// In some rare cases, the logic index could be -1 and we want to check this as well
			if (index != -1) {

				// Make an MObject from the plug node
				MObject object(plug.node());

				// Create mesh
				MFnMesh mesh(object, &status);

				if (status == MS::kSuccess) {

					// Print which vertex on the mesh was modifed
					//MGlobal::displayInfo(MString() + "Vertex " + index + " on " + mesh.name() + " was modified");

					// Get the vertex from the mesh
					MPoint vertex;
					mesh.getPoint(index, vertex);

					// Print vertex
					//MGlobal::displayInfo(MString() + vertex.x + vertex.y + vertex.z);

					// Send mesh message header
					MsgHeader header = { MAYA_MSG_TYPE::MESH_TOPOLOGY_CHANGED };

					MsgMesh msgMesh;

					// Get mesh name
					strcpy_s(msgMesh.name, mesh.name().asChar());

					// Get vertices
					vector<Vertex>vertices;
					CreateVertexData(mesh, vertices);

					// Get vertex count
					msgMesh.vertexCount = vertices.size();

					size_t msgSize = 10 * 1 << 20;
					char* msg = new char[msgSize];
					size_t msgLength = 0;

					// Memcpy header
					memcpy(msg, &header, sizeof(MsgHeader));
					msgLength += sizeof(MsgHeader);

					// Memcpy MsgMesh
					memcpy(msg + msgLength, &msgMesh, sizeof(MsgMesh));
					msgLength += sizeof(MsgMesh);

					// Memcpy vertices
					memcpy(msg + msgLength, vertices.data(), sizeof(Vertex) * msgMesh.vertexCount);
					msgLength += sizeof(Vertex) * msgMesh.vertexCount;

					comLib.send(msg, msgLength);

				}

				else {

					MGlobal::displayInfo(status.errorString());
				}

			}

		}

	}

	else {

		return;
	}
}

void CallbackManager::attributeMaterialCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData) {

	// Check if attribute was set
	if (msg & MNodeMessage::kAttributeSet) {

		MStatus status = MS::kSuccess;

		// Declare rgb array and cosine power
		float rgb[3];
		float cosinePower = 16.0f;

		// If the material is Phong, we want the cosine power
		if (plug.node().hasFn(MFn::kPhong))
		{
			MPlug cosinePowerPlug = MFnDependencyNode(plug.node()).findPlug("cosinePower", &status);
			if (status == MS::kSuccess)
			{
				MGlobal::displayInfo("Found cosine power");
				float cosPower = 0.0f;
				cosinePowerPlug.getValue(cosPower);
				cosinePower = cosPower;
			}

			else {

				MGlobal::displayInfo(status.errorString());
			}
		}

		// Most importantly, we require the color from the material
		MPlug colorPlug = MFnDependencyNode(plug.node()).findPlug("color", &status);
		if (status == MS::kSuccess)
		{

			// Send mesh message header
			MsgHeader header = { MAYA_MSG_TYPE::MATERIAL_CHANGED };

			// Create the material message 
			MsgMaterial msgMaterial;

			// Get material name
			MStringArray matName;
			plug.name().split('.', matName);

			// Provide the material name
			strcpy_s(msgMaterial.materialName, matName[0].asChar());

			// Get diffuse
			MObject data;
			colorPlug.getValue(data);
			MFnNumericData val(data);
			val.getData(rgb[0], rgb[1], rgb[2]);
			msgMaterial.diffuse.x = (float)rgb[0];
			msgMaterial.diffuse.y = (float)rgb[1];
			msgMaterial.diffuse.z = (float)rgb[2];
			msgMaterial.diffuse.w = cosinePower;

			size_t msgSize = 10 * 1 << 20;
			char* msg = new char[msgSize];
			size_t msgLength = 0;

			// Memcpy header
			memcpy(msg, &header, sizeof(MsgHeader));
			msgLength += sizeof(MsgHeader);

			// Memcpy MsgMesh
			memcpy(msg + msgLength, &msgMaterial, sizeof(MsgMaterial));
			msgLength += sizeof(MsgMaterial);

			comLib.send(msg, msgLength);

		}

		else {

			MGlobal::displayInfo(status.errorString());
		}

	}

	else {

		return;
	}
}

void CallbackManager::attributeLightCallback(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData) {

	// Check if attribute was set
	if (msg & MNodeMessage::kAttributeSet) {

		MStatus status = MS::kSuccess;

		// Was the attribute a color?
		MPlug colorPlug = MFnDependencyNode(plug.node()).findPlug("color", &status);

		if (status == MS::kSuccess)
		{

			// Make an MObject from the plug node
			MObject object(plug.node());

			// Create point light
			MFnPointLight light(object, &status);

			if (status == MS::kSuccess) {

				// Create the light message 
				MsgLight msgLight;

				// Get light name
				MStringArray lightName;
				plug.name().split('.', lightName);

				// Provide the light name
				strcpy_s(msgLight.name, lightName[0].asChar());

				// Get color
				MColor lightColor = light.color(&status);
				
				// Initialize light message color
				msgLight.Color.x = lightColor.r;
				msgLight.Color.y = lightColor.g;
				msgLight.Color.z = lightColor.b;
				msgLight.Color.w = light.intensity();

				size_t msgSize = 10 * 1 << 20;
				char* msg = new char[msgSize];
				size_t msgLength = 0;

				// Construct message header
				MsgHeader header = { MAYA_MSG_TYPE::LIGHT_COLOR_CHANGED };

				// Memcpy header
				memcpy(msg, &header, sizeof(MsgHeader));
				msgLength += sizeof(MsgHeader);

				// Memcpy MsgLight
				memcpy(msg + msgLength, &msgLight, sizeof(MsgLight));
				msgLength += sizeof(MsgLight);

				// Send the message
				comLib.send(msg, msgLength);

				delete msg;

			}

			else {

				MGlobal::displayInfo(status.errorString());
			}

		}

		else {

			MGlobal::displayInfo(status.errorString());
		}

	}

	else {

		return;
	}
}

void CallbackManager::topologyChangedCallback(MObject &node, void *clientData) {

	//MGlobal::displayInfo("Topology was changed!");

	MStatus status = MS::kSuccess;

	MFnMesh mesh(node, &status);

	if (status == MS::kSuccess) {

		// Send mesh message header
		MsgHeader header = { MAYA_MSG_TYPE::MESH_TOPOLOGY_CHANGED };

		// Send actual mesh message
		MsgMesh msgMesh;

		// Get mesh name
		strcpy_s(msgMesh.name, mesh.name().asChar());

		// Get vertices
		vector<Vertex>vertices;
		CreateVertexData(mesh, vertices);

		// Get vertex count
		msgMesh.vertexCount = vertices.size();

		size_t msgSize = 10 * 1 << 20;
		char* msg = new char[msgSize];
		size_t msgLength = 0;

		// Memcpy header
		memcpy(msg, &header, sizeof(MsgHeader));
		msgLength += sizeof(MsgHeader);

		// Memcpy MsgMesh
		memcpy(msg + msgLength, &msgMesh, sizeof(MsgMesh));
		msgLength += sizeof(MsgMesh);

		// Memcpy vertices
		memcpy(msg + msgLength, vertices.data(), sizeof(Vertex) * msgMesh.vertexCount);
		msgLength += sizeof(Vertex) * msgMesh.vertexCount;

		comLib.send(msg, msgLength);

		delete msg;
	}

	else {

		MGlobal::displayInfo(status.errorString());
	}
}

void CallbackManager::postrenderCallback(const MString& panelName, void * data) {

	// Get the view if any for the panel
	M3dView view;
	MStatus status = M3dView::getM3dViewFromModelPanel(panelName, view);
	
	if (status == MS::kSuccess) {

		// Get the view matrix and camera position matrix from the active viewport
		MMatrix viewMatrix;
		MMatrix cameraPos;

		// Get the model view matrix
		view.modelViewMatrix(viewMatrix);

		MDagPath cameraPath;
		view.getCamera(cameraPath);

		MFnCamera fnCamera(cameraPath);

		// Inclusive matrix will return the camera position matrix
		cameraPos = cameraPath.inclusiveMatrix();

		// Convert to XMFLOAT4X4 matrix
		MsgCamera msgCamera;

		// Get camera properties
		msgCamera.isOrtographic = fnCamera.isOrtho();

		msgCamera.fPlane = fnCamera.farClippingPlane();
		msgCamera.nPlane = fnCamera.nearClippingPlane();

		msgCamera.fov = fnCamera.horizontalFieldOfView();
		msgCamera.viewWidth = fnCamera.orthoWidth();

		// Convert from MMatrix to XMFLOAT4X4
		viewMatrix.get(msgCamera.viewMatrix.m);

		msgCamera.position.x = (float)cameraPos(3, 0);
		msgCamera.position.y =	(float)cameraPos(3, 1);
		msgCamera.position.z =	(float)cameraPos(3, 2);
		msgCamera.position.w = (float)cameraPos(3, 3);

		size_t msgSize = 10 * 1 << 20;
		char* msg = new char[msgSize];
		size_t msgLength = 0;

		MsgHeader header = { MAYA_MSG_TYPE::VIEW_CHANGED };

		// Memcpy header
		memcpy(msg, &header, sizeof(MsgHeader));
		msgLength += sizeof(MsgHeader);

		// Memcpy camera message
		memcpy(msg + msgLength, &msgCamera, sizeof(MsgCamera));
		msgLength += sizeof(MsgCamera);

		// Send the message
		comLib.send(msg, msgLength);

		delete msg;

	}

}

void CallbackManager::meshAdded(MFnMesh& mesh) {

	/////////////////////////////////////////////////////////////
	// REGISTER CALLBACKS
	/////////////////////////////////////////////////////////////

	MStatus status = MS::kSuccess;

	MGlobal::displayInfo("The mesh " + mesh.name() + " has been added!");

	// We want to track the name changes for the mesh
	CreateNameChangedCallback(mesh.object(), nameMeshCallback);

	// We want to check if vertices has changed
	CreateChangedAttributeCallback(mesh.object(), attributeVertexCallback);

	// We must get the shader node currently connected to this mesh
	MObject shaderNode = getShader(mesh);

	// We want to check if the material changes its name
	CreateNameChangedCallback(shaderNode, nameMaterialCallback);

	// We want to check if the material has changed
	CreateChangedAttributeCallback(shaderNode, attributeMaterialCallback);

	// We want to check if the topology has changed
	CreateTopologyChangedCallback(mesh.object(), topologyChangedCallback);

	// Create transform from the mest parent
	
	MFnDagNode dagParent = mesh.parent(0);;
	MGlobal::displayInfo("The transform " + dagParent.name() + " has been added!");

	CreateChangedAttributeCallback(dagParent.object(), attributeMeshTransformCallback);

	/////////////////////////////////////////////////////////////
	// CONSTRUCT DATA
	/////////////////////////////////////////////////////////////

	// Construct Mesh message containing name and vertex count
	MsgMesh msgMesh;

	strcpy_s(msgMesh.name, mesh.name().asChar());

	// Get vertices
	vector<Vertex>vertices;
	CreateVertexData(mesh, vertices);

	// Get vertex count
	msgMesh.vertexCount = vertices.size();

	// Construct the material containing diffuse values
	MsgMaterial msgMaterial;

	// Set which mesh the material belong to and provide the material name as well
	strcpy_s(msgMaterial.name, mesh.name().asChar());
	strcpy_s(msgMaterial.materialName, MFnDependencyNode(shaderNode).name().asChar());

	// Get material values
	CreateMaterialData(shaderNode, msgMaterial);

	// Create transform 
	MFnTransform transform(dagParent.object(), &status);

	// Get the dag path
	MDagPath parentPath;
	transform.getPath(parentPath);
	MMatrix worldPos = parentPath.inclusiveMatrix();

	// Assign to mesh transform struct
	MsgTransform msgTransform;

	// Get the world matrix
	worldPos.get(msgTransform.transform.m);

	double Rotation[4];
	double Scale[3];

	// get the transforms local translation 
	MVector Translation = transform.getTranslation(MSpace::kTransform);

	// get the transforms scale
	transform.getScale(Scale);

	// get the transforms rotation as a quaternion
	transform.getRotationQuaternion(Rotation[0], Rotation[1], Rotation[2], Rotation[3], MSpace::kTransform);

	msgTransform.translation.x = Translation.x;
	msgTransform.translation.y = Translation.y;
	msgTransform.translation.z = Translation.z;

	msgTransform.quaternion.x = Rotation[0];
	msgTransform.quaternion.y = Rotation[1];
	msgTransform.quaternion.z = Rotation[2];
	msgTransform.quaternion.w = Rotation[3];

	msgTransform.scale.x = Scale[0];
	msgTransform.scale.y = Scale[1];
	msgTransform.scale.z = Scale[2];

	// Get transform child name (the mesh)
	strcpy_s(msgTransform.childName, mesh.name().asChar());

	/////////////////////////////////////////////////////////////
	// SEND DATA
	/////////////////////////////////////////////////////////////
	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	// Construct message header
	MsgHeader header = { MAYA_MSG_TYPE::NEW_MESH };

	// Memcpy header
	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	// Memcpy MsgMesh
	memcpy(msg + msgLength, &msgMesh, sizeof(MsgMesh));
	msgLength += sizeof(MsgMesh);

	// Memcpy MsgMaterial
	memcpy(msg + msgLength, &msgMaterial, sizeof(MsgMaterial));
	msgLength += sizeof(MsgMaterial);

	// Memcpy vertices
	memcpy(msg + msgLength, vertices.data(), sizeof(Vertex) * msgMesh.vertexCount);
	msgLength += sizeof(Vertex) * msgMesh.vertexCount;

	// Memcpy transform
	memcpy(msg + msgLength, &msgTransform, sizeof(MsgTransform));
	msgLength += sizeof(MsgTransform);

	// Send the message
	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::lightAdded(MFnPointLight& light) {

	/////////////////////////////////////////////////////////////
	// REGISTER CALLBACKS
	/////////////////////////////////////////////////////////////

	MStatus status = MS::kSuccess;

	MGlobal::displayInfo("The light " + light.name() + " has been added!");

	// Check if color has changed
	CreateChangedAttributeCallback(light.object(), attributeLightCallback);

	// Create transform from the light parent
	MFnDependencyNode parent = light.parent(0);
	MGlobal::displayInfo("The transform " + parent.name() + " has been added!");

	// We want to check if the light transformation changes
	CreateChangedAttributeCallback(parent.object(), attributeLightTransformCallback);

	// We want to check if the light name changes
	CreateNameChangedCallback(light.object(), nameLightCallback);

	/////////////////////////////////////////////////////////////
	// CONSTRUCT DATA
	/////////////////////////////////////////////////////////////

	// Create light message
	MsgLight msgLight;

	// Copy light name
	strcpy_s(msgLight.name, light.name().asChar());

	// Create transform from light shape parent
	MFnTransform transform(parent.object(), &status);

	// Get the dag path
	MDagPath parentPath;
	transform.getPath(parentPath);
	MMatrix worldPos = parentPath.inclusiveMatrix();

	// Get color
	MColor lightColor = light.color(&status);

	// Get the translation
	msgLight.Position.x = (float)worldPos(3, 0);
	msgLight.Position.y = (float)worldPos(3, 1);
	msgLight.Position.z = (float)worldPos(3, 2);
	msgLight.Position.w = 70;

	msgLight.Color.x = lightColor.r;
	msgLight.Color.y = lightColor.g;
	msgLight.Color.z = lightColor.b;
	msgLight.Color.w = light.intensity();

	/////////////////////////////////////////////////////////////
	// SEND DATA
	/////////////////////////////////////////////////////////////

	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	// Construct message header
	MsgHeader header = { MAYA_MSG_TYPE::NEW_LIGHT };

	// Memcpy header
	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	// Memcpy MsgLight
	memcpy(msg + msgLength, &msgLight, sizeof(MsgLight));
	msgLength += sizeof(MsgLight);

	// Send the message
	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::meshTransformUpdate(MFnTransform& transform) {

	MStatus status = MS::kSuccess;

	// Get the shape node
	MFnDependencyNode mesh = transform.child(0);

	// Get the dag path
	MDagPath thisDagNode;
	transform.getPath(thisDagNode);
	MMatrix worldPos = thisDagNode.inclusiveMatrix();

	// Assign to mesh transform struct
	MsgTransform msgTransform;

	// Get the world matrix
	worldPos.get(msgTransform.transform.m);

	double Rotation[4];
	double Scale[3];

	// get the transforms local translation 
	MVector Translation = transform.getTranslation(MSpace::kTransform);

	// get the transforms scale
	transform.getScale(Scale);

	// get the transforms rotation as a quaternion
	transform.getRotationQuaternion(Rotation[0], Rotation[1], Rotation[2], Rotation[3], MSpace::kTransform);

	msgTransform.translation.x = Translation.x;
	msgTransform.translation.y = Translation.y;
	msgTransform.translation.z = Translation.z;

	msgTransform.quaternion.x = Rotation[0];
	msgTransform.quaternion.y = Rotation[1];
	msgTransform.quaternion.z = Rotation[2];
	msgTransform.quaternion.w = Rotation[3];

	msgTransform.scale.x = Scale[0];
	msgTransform.scale.y = Scale[1];
	msgTransform.scale.z = Scale[2];

	// Get transform child name
	strcpy_s(msgTransform.childName, mesh.name().asChar());

	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	// Send transform message header
	MsgHeader header = { MAYA_MSG_TYPE::MESH_TRANSFORM_CHANGED };

	// Memcpy header
	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	// Memcpy MsgTransform
	memcpy(msg + msgLength, &msgTransform, sizeof(MsgTransform));
	msgLength += sizeof(MsgTransform);

	// Send the message
	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::RecursiveTransformUpdate(MFnTransform& transform) {

	MStatus status = MS::kSuccess;

	// Make a dag node
	MFnDagNode fn(transform.object());

	// First update the transform
	meshTransformUpdate(transform);

	// Call upon any potential children to update their transforms as well
	for (int i = 0; i != fn.childCount(); ++i) {

		// get a handle to the child
		MObject obj = fn.child(i);

		if (obj.apiType() == MFn::Type::kTransform) {

			MFnTransform childTransform(obj, &status);

			if (status == MS::kSuccess) {

				// Furthermore, check the api type of the child and call appropiate transform update for that object
				MObject childType = transform.child(0, &status);

				if (status == MS::kSuccess) {

					// If the child node is a mesh, only update mesh transformations
					if (childType.apiType() == MFn::Type::kMesh) {

						// Call the recursive function
						RecursiveTransformUpdate(childTransform);

					}

				}

				else {

					MGlobal::displayInfo(status.errorString());
				}
			}

			else {

				MGlobal::displayInfo(status.errorString());
			}
		}

	}
}

void CallbackManager::lightTransformUpdate(MFnTransform& transform) {

	// Create light message
	MsgLight msgLight;

	MFnDependencyNode light = transform.child(0);

	// Get the light name
	strcpy_s(msgLight.name, light.name().asChar());

	// Get the dag path
	MDagPath parentPath;
	transform.getPath(parentPath);
	MMatrix worldPos = parentPath.inclusiveMatrix();

	// Get translation
	msgLight.Position.x = (float)worldPos(3, 0);
	msgLight.Position.y = (float)worldPos(3, 1);
	msgLight.Position.z = (float)worldPos(3, 2);
	msgLight.Position.w = 70;

	size_t msgSize = 10 * 1 << 20;
	char* msg = new char[msgSize];
	size_t msgLength = 0;

	// Send transform message header
	MsgHeader header = { MAYA_MSG_TYPE::LIGHT_TRANSFORM_CHANGED };

	// Memcpy header
	memcpy(msg, &header, sizeof(MsgHeader));
	msgLength += sizeof(MsgHeader);

	// Memcpy MsgLight
	memcpy(msg + msgLength, &msgLight, sizeof(MsgLight));
	msgLength += sizeof(MsgLight);

	// Send the message
	comLib.send(msg, msgLength);

	delete msg;
}

void CallbackManager::CreateVertexData(MFnMesh& mesh, vector<Vertex> &vertices) {

	//  Cache positions for each vertex
	MPointArray meshPoints;
	mesh.getPoints(meshPoints, MSpace::kObject);

	//  Cache normals for each vertex
	MFloatVectorArray  meshNormals;

	// Normals are per-vertex per-face..
	// use MItMeshPolygon::normalIndex() for index
	mesh.getNormals(meshNormals);

	// Get UVSets for this mesh
	MStringArray  UVSets;
	MStatus status = mesh.getUVSetNames(UVSets);

	// Get all UV coordinates for the first UV set (default "map1").
	MFloatArray   u, v;
	mesh.getUVs(u, v, &UVSets[0]);

	// Iterator for mesh polygons
	MItMeshPolygon itPolygon(mesh.object(), &status);

	if (status == MS::kSuccess) {

		//MGlobal::displayInfo("Entered iterator loop");

		for (; !itPolygon.isDone(); itPolygon.next()) {

			//MGlobal::displayInfo("Loop");

			// Get object-relative indices for the vertices in this face.
			MIntArray polygonVertices;
			itPolygon.getVertices(polygonVertices);

			// Get triangulation of this polygon
			int numTriangles;
			itPolygon.numTriangles(numTriangles);

			MPointArray nonTweaked;

			// object-relative vertex indices for each triangle
			MIntArray triangleVertices;

			// face-relative vertex indices for each triangle
			MIntArray localIndex;

			if (numTriangles == 2) {

				status = itPolygon.getTriangles(nonTweaked,
					triangleVertices,
					MSpace::kObject);

				MIntArray firstTriangleVertices;
				MIntArray secondTriangleVertices;

				firstTriangleVertices.setLength(3);
				secondTriangleVertices.setLength(3);

				firstTriangleVertices[0] = triangleVertices[0];
				firstTriangleVertices[1] = triangleVertices[1];
				firstTriangleVertices[2] = triangleVertices[2];

				secondTriangleVertices[0] = triangleVertices[3];
				secondTriangleVertices[1] = triangleVertices[4];
				secondTriangleVertices[2] = triangleVertices[5];

				if (status == MS::kSuccess)

				{

					Vertex v1;
					Vertex v2;
					Vertex v3;

					v1.pos.x = (float)meshPoints[firstTriangleVertices[0]].x;
					v1.pos.y = (float)meshPoints[firstTriangleVertices[0]].y;
					v1.pos.z = (float)meshPoints[firstTriangleVertices[0]].z;

					v2.pos.x = (float)meshPoints[firstTriangleVertices[1]].x;
					v2.pos.y = (float)meshPoints[firstTriangleVertices[1]].y;
					v2.pos.z = (float)meshPoints[firstTriangleVertices[1]].z;

					v3.pos.x = (float)meshPoints[firstTriangleVertices[2]].x;
					v3.pos.y = (float)meshPoints[firstTriangleVertices[2]].y;
					v3.pos.z = (float)meshPoints[firstTriangleVertices[2]].z;

					localIndex = GetLocalIndex(polygonVertices, firstTriangleVertices);

					v1.normal.x = meshNormals[itPolygon.normalIndex(localIndex[0])].x;
					v1.normal.y = meshNormals[itPolygon.normalIndex(localIndex[0])].y;
					v1.normal.z = meshNormals[itPolygon.normalIndex(localIndex[0])].z;

					v2.normal.x = meshNormals[itPolygon.normalIndex(localIndex[1])].x;
					v2.normal.y = meshNormals[itPolygon.normalIndex(localIndex[1])].y;
					v2.normal.z = meshNormals[itPolygon.normalIndex(localIndex[1])].z;

					v3.normal.x = meshNormals[itPolygon.normalIndex(localIndex[2])].x;
					v3.normal.y = meshNormals[itPolygon.normalIndex(localIndex[2])].y;
					v3.normal.z = meshNormals[itPolygon.normalIndex(localIndex[2])].z;

					int firstUVID[3];

					// Get UV values for each vertex within this polygon
					for (int vtxInPolygon = 0; vtxInPolygon < 3; vtxInPolygon++)
					{
						itPolygon.getUVIndex(localIndex[vtxInPolygon],
							firstUVID[vtxInPolygon],
							&UVSets[0]);
					}

					v1.uv.x = u[firstUVID[0]];
					v1.uv.y = v[firstUVID[0]];
					v1.uv.y = 1 - v1.uv.y;

					v2.uv.x = u[firstUVID[1]];
					v2.uv.y = v[firstUVID[1]];
					v2.uv.y = 1 - v2.uv.y;

					v3.uv.x = u[firstUVID[2]];
					v3.uv.y = v[firstUVID[2]];
					v3.uv.y = 1 - v3.uv.y;

					vertices.push_back(v1);
					vertices.push_back(v2);
					vertices.push_back(v3);

					Vertex v4;
					Vertex v5;
					Vertex v6;

					v4.pos.x = (float)meshPoints[secondTriangleVertices[0]].x;
					v4.pos.y = (float)meshPoints[secondTriangleVertices[0]].y;
					v4.pos.z = (float)meshPoints[secondTriangleVertices[0]].z;

					v5.pos.x = (float)meshPoints[secondTriangleVertices[1]].x;
					v5.pos.y = (float)meshPoints[secondTriangleVertices[1]].y;
					v5.pos.z = (float)meshPoints[secondTriangleVertices[1]].z;

					v6.pos.x = (float)meshPoints[secondTriangleVertices[2]].x;
					v6.pos.y = (float)meshPoints[secondTriangleVertices[2]].y;
					v6.pos.z = (float)meshPoints[secondTriangleVertices[2]].z;

					localIndex = GetLocalIndex(polygonVertices, secondTriangleVertices);

					v4.normal.x = meshNormals[itPolygon.normalIndex(localIndex[0])].x;
					v4.normal.y = meshNormals[itPolygon.normalIndex(localIndex[0])].y;
					v4.normal.z = meshNormals[itPolygon.normalIndex(localIndex[0])].z;

					v5.normal.x = meshNormals[itPolygon.normalIndex(localIndex[1])].x;
					v5.normal.y = meshNormals[itPolygon.normalIndex(localIndex[1])].y;
					v5.normal.z = meshNormals[itPolygon.normalIndex(localIndex[1])].z;

					v6.normal.x = meshNormals[itPolygon.normalIndex(localIndex[2])].x;
					v6.normal.y = meshNormals[itPolygon.normalIndex(localIndex[2])].y;
					v6.normal.z = meshNormals[itPolygon.normalIndex(localIndex[2])].z;

					int secondUVID[3];

					// Get UV values for each vertex within this polygon
					for (int vtxInPolygon = 0; vtxInPolygon < 3; vtxInPolygon++)
					{
						itPolygon.getUVIndex(localIndex[vtxInPolygon],
							secondUVID[vtxInPolygon],
							&UVSets[0]);
					}

					v4.uv.x = u[secondUVID[0]];
					v4.uv.y = v[secondUVID[0]];
					v4.uv.y = 1 - v4.uv.y;

					v5.uv.x = u[secondUVID[1]];
					v5.uv.y = v[secondUVID[1]];
					v5.uv.y = 1 - v5.uv.y;

					v6.uv.x = u[secondUVID[2]];
					v6.uv.y = v[secondUVID[2]];
					v6.uv.y = 1 - v6.uv.y;

					vertices.push_back(v4);
					vertices.push_back(v5);
					vertices.push_back(v6);


				}

				else {

					MGlobal::displayInfo(status.errorString());
				}

			}

			else {

				status = itPolygon.getTriangles(nonTweaked,
					triangleVertices,
					MSpace::kObject);

				MIntArray firstTriangleVertices;

				firstTriangleVertices.setLength(3);

				firstTriangleVertices[0] = triangleVertices[0];
				firstTriangleVertices[1] = triangleVertices[1];
				firstTriangleVertices[2] = triangleVertices[2];

				if (status == MS::kSuccess)

				{

					Vertex v1;
					Vertex v2;
					Vertex v3;

					v1.pos.x = (float)meshPoints[firstTriangleVertices[0]].x;
					v1.pos.y = (float)meshPoints[firstTriangleVertices[0]].y;
					v1.pos.z = (float)meshPoints[firstTriangleVertices[0]].z;

					v2.pos.x = (float)meshPoints[firstTriangleVertices[1]].x;
					v2.pos.y = (float)meshPoints[firstTriangleVertices[1]].y;
					v2.pos.z = (float)meshPoints[firstTriangleVertices[1]].z;

					v3.pos.x = (float)meshPoints[firstTriangleVertices[2]].x;
					v3.pos.y = (float)meshPoints[firstTriangleVertices[2]].y;
					v3.pos.z = (float)meshPoints[firstTriangleVertices[2]].z;

					localIndex = GetLocalIndex(polygonVertices, firstTriangleVertices);

					v1.normal.x = meshNormals[itPolygon.normalIndex(localIndex[0])].x;
					v1.normal.y = meshNormals[itPolygon.normalIndex(localIndex[0])].y;
					v1.normal.z = meshNormals[itPolygon.normalIndex(localIndex[0])].z;

					v2.normal.x = meshNormals[itPolygon.normalIndex(localIndex[1])].x;
					v2.normal.y = meshNormals[itPolygon.normalIndex(localIndex[1])].y;
					v2.normal.z = meshNormals[itPolygon.normalIndex(localIndex[1])].z;

					v3.normal.x = meshNormals[itPolygon.normalIndex(localIndex[2])].x;
					v3.normal.y = meshNormals[itPolygon.normalIndex(localIndex[2])].y;
					v3.normal.z = meshNormals[itPolygon.normalIndex(localIndex[2])].z;

					int firstUVID[3];

					// Get UV values for each vertex within this polygon
					for (int vtxInPolygon = 0; vtxInPolygon < 3; vtxInPolygon++)
					{
						itPolygon.getUVIndex(localIndex[vtxInPolygon],
							firstUVID[vtxInPolygon],
							&UVSets[0]);
					}

					v1.uv.x = u[firstUVID[0]];
					v1.uv.y = v[firstUVID[0]];

					v2.uv.x = u[firstUVID[1]];
					v2.uv.y = v[firstUVID[1]];

					v3.uv.x = u[firstUVID[2]];
					v3.uv.y = v[firstUVID[2]];

					vertices.push_back(v1);
					vertices.push_back(v2);
					vertices.push_back(v3);
				}

				else {

					MGlobal::displayInfo(status.errorString());
				}


			}

		}

	}

	else {

		MGlobal::displayInfo(status.errorString());
	}

}

void CallbackManager::CreateMaterialData(MObject &shaderNode, MsgMaterial& material) {

	if (shaderNode != MObject::kNullObj)
	{
		MStatus status = MS::kSuccess;

		float rgb[3];
		float cosinePower = 16.0f;

		MGlobal::displayInfo("Found shader: " + MFnDependencyNode(shaderNode).name());

		// If the material is Phong, we want the cosine power
		if (shaderNode.hasFn(MFn::kPhong))
		{
			MPlug cosinePowerPlug = MFnDependencyNode(shaderNode).findPlug("cosinePower", &status);
			if (status == MS::kFailure)
			{

				float cosPower = 0.0f;
				cosinePowerPlug.getValue(cosPower);
				cosinePower = cosPower;
			}

			else {

				MGlobal::displayInfo(status.errorString());
			}
		}

		// Find the color plug of the shader node
		MPlug colorPlug = MFnDependencyNode(shaderNode).findPlug("color", &status);
		if (status == MS::kSuccess)
		{

			// Iterate the shadernode's color channel for textures
			MItDependencyGraph dgIt(colorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream);

			// If a texture was found, grab the texture path
			if (!dgIt.isDone())
			{
				MObject textureNode = dgIt.thisNode();
				MPlug filenamePlug = MFnDependencyNode(textureNode).findPlug("fileTextureName");
				MString textureName;
				filenamePlug.getValue(textureName);
				MGlobal::displayInfo("Texture Node Name: " + MFnDependencyNode(textureNode).name());
				MGlobal::displayInfo("Texture File Name: " + textureName);

				strcpy_s(material.texturePath, textureName.asChar());

				material.diffuse.x = 1;
				material.diffuse.y = 1;
				material.diffuse.z = 1;
				material.diffuse.w = cosinePower;

			}

			// If no texture was found, grab the color values
			else {

				MGlobal::displayInfo("No texture found on " + MFnDependencyNode(shaderNode).name());

				MObject data;
				colorPlug.getValue(data);
				MFnNumericData val(data);
				val.getData(rgb[0], rgb[1], rgb[2]);

				material.diffuse.x = (float)rgb[0];
				material.diffuse.y = (float)rgb[1];
				material.diffuse.z = (float)rgb[2];
				material.diffuse.w = cosinePower;
			}


		}

		else {

			MGlobal::displayInfo(status.errorString());
		}

	}

}

MIntArray CallbackManager::GetLocalIndex(MIntArray & getVertices, MIntArray & getTriangle) {

	// MItMeshPolygon::getTriangle() returns object-relative vertex indices
	// BUT MItMeshPolygon::normalIndex() and ::getNormal() need face-relative vertex indices! 
	// This helper function converts vertex indices from object-relative to face-relative.

	MIntArray   localIndex;

	unsigned    gv, gt;

	assert(getTriangle.length() == 3);    // We should always handle triangles and nothing else

	for (gt = 0; gt < getTriangle.length(); gt++)
	{
		for (gv = 0; gv < getVertices.length(); gv++)
		{

			if (getTriangle[gt] == getVertices[gv])
			{
				localIndex.append(gv);
				break;
			}
		}

		// if nothing was added, add a default "no match"
		if (localIndex.length() == gt)
			localIndex.append(-1);
	}

	return localIndex;
}

#pragma endregion CallbackManagerClass

EXPORT MStatus initializePlugin(MObject object) {

	// Use MStatus variables to indicate for errors, similiar to HRESULT
	MStatus status = MS::kSuccess;

	// Initialize plugin
	MFnPlugin plugin(object, "Maya Plugin", "1.0", "Any", &status);

	// Register callbacks
	CallbackManager::RegisterCallbacks();

	// Iterate scene at startup
	CallbackManager::IterateScene();

	return status;
}

EXPORT MStatus uninitializePlugin(MObject object) {

	// Initialize the function with the MObject that represents our plugin
	MFnPlugin plugin(object);

	// Deallocate resources
	CallbackManager::Shutdown();

	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}