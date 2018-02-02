#include "OpenGL.h"

#include <string>
#include <vector>

// Includes to create the cube
#include "ISceneNode.h"
#include "ModelManager.h"
#include "DrawableProxy.h"
#include "DrawableCubeSolution.h"
#include "DrawableSphereSolution.h"
#include "DrawableFloorSolution.h"

// Material includes
#include "SolidColorMaterialSolution.h"
#include "ShadedMaterial.h"
#include "MaterialProxy.h"
#include "MaterialManager.h"
#include "ShaderConstantMaterial.h"
#include "Color.h"

#include "BlankTexture2D.h"
#include "TextureBinding.h"
#include "TextureBindManager.h"
#include "TextureBindManagerOpenGL.h"
#include "TextureDataImage.h"
#include "SamplerApplicator.h"
#include "SimpleShaderMaterial.h"
#include "TexParam2DNoMipMap.h"
#include "TexParam2DMipMap.h"

// Includes for the camera
#include "ExaminerCameraNode.h"
#include "PerspectiveTransformSolution.h"
#include "LookAtTransformSolution.h"
#include "ShaderConstantModelView.h"

// Lights
#include "PointLight.h"
#include "DirectionalLight.h"
#include "LightManager.h"
#include "ShaderConstantLights.h"

#include "RenderTargetProxy.h"
#include "RenderTarget.h"
#include "RenderManager.h"
#include "ClearFrameCommand.h"
#include "SwapCommand.h"

// Includes for walking the scene graph
#include "DebugRenderVisitor.h"
#include "PrintSceneVisitor.h"
// 1108
#include "PaintController.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "DrawableObject.h"
#include "DrawableBrush.h"
#include "DrawableObjectwireframe.h"

// 12/1
#include "IDepthBuffer.h"
#include "DepthBuffer.h"
#include "BlankDepthTexture2D.h"
#include "MatrixStack.h"
// Interaction
std::vector<IMouseHandler*> mouseHandlers;
std::vector<IKeyboardHandler*> keyboardHandlers;


using namespace Crawfis::Graphics;
using namespace Crawfis::Math;
using namespace std;


ISceneNode* rootSceneNode;
// 11/13
ISceneNode* rootSceneNode2;
ISceneNode* rootSceneNode3;
ISceneNode* rootSceneNode4;

// 11/28
ISceneNode* renderSceneNode;

IVisitor* renderVisitor;
// 11/29
IVisitor* DepthrenderVisitor;

ExaminerCameraNode* examiner;
// 11/13
ExaminerCameraNode* examiner2;
ExaminerCameraNode* examiner3;
ExaminerCameraNode* examiner4;

// 11/29
ExaminerCameraNode* render_examiner;

// 11/15
PaintController *paint;
// 11/16
// create an empty texture
ITextureDataObject* brushtexture;

// 11/24
static DrawableBrush* brush;

// 11/26
// brushPath
std::vector<glm::vec3> Crawfis::Graphics::PaintController::brushPath;


// 12/01
Matrix4 DepthMVP;

int windowGUID;
int windowWidth;
int windowHeight; 

void CreateGLUTWindow(std::string windowTitle)
{
	windowWidth = 800;
	windowHeight = 600;
	glutInitDisplayMode(GLUT_RGB);
	// 12/03
	//glutInitDisplayMode(GLUT_DEPTH);

	glutInitWindowSize(windowWidth, windowHeight);
	windowGUID = glutCreateWindow(windowTitle.c_str());
}

void InitializeOpenGLExtensions()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		throw "Error initializing GLEW";
	}

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void InitializeDevices()
{
	CreateGLUTWindow("OpenGL lab3 Su");
	InitializeOpenGLExtensions();
	glDisable(GL_CULL_FACE);
}

void CreateLights()
{
	PointLight* pointLight = new PointLight("light0-pt");
	pointLight->setPosition(Vector3(-2, 5, -1));
	LightManager::Instance()->SetLight(0, pointLight);
	DirectionalLight* dirLight = new DirectionalLight("light1-dir");
	//dirLight->setColor(Colors::IndianRed);
	//dirLight->setDirection(Vector3(10, 1, 1));
	dirLight->setDirection(Vector3(10, 1, 1));
	LightManager::Instance()->SetLight(1, dirLight);
}
void CreateGoldMaterial()
{
	VertexRoutine* vertexRoutine = new VertexRoutine("..\\Media\\Shaders\\VertexLight.glsl");
	FragmentRoutine* fragmentRoutine = new FragmentRoutine("..\\Media\\Shaders\\SolidColorSolution.frag");
	
	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(vertexRoutine, fragmentRoutine);

	Color gold(0.8314f, 0.6863f, 0.2169f, 1.0f);
	ShadedMaterial* shinyGold = new ShadedMaterial(shaderProgram);
	shinyGold->setAmbientReflection(0.01f*gold);
	shinyGold->setDiffuseReflection(0.7f*gold);
	shinyGold->setSpecularReflection(0.25f*gold);
	shinyGold->setShininess(100.0f);

	ShaderConstantMaterial* materialConstant = new ShaderConstantMaterial("frontMaterial");
	materialConstant->setValue(shinyGold);
	ShaderConstantLights* lightConstant = new ShaderConstantLights();
	ShaderConstantCollection* shaderConstantList = new ShaderConstantCollection();
	shaderConstantList->AddConstant(materialConstant);
	shaderConstantList->AddConstant(lightConstant);
	shaderProgram->setShaderConstant(shaderConstantList);

	MaterialManager::Instance()->RegisterMaterial("ShinyGold", shinyGold);
}

void CreateTexturedMaterial()
{
	// texture for normal map for step7_shaderprogram
	ITextureDataObject* camelTexture = new TextureDataImage("../Media/Textures/camel_normals.png", GL_RGB);
	camelTexture->setTextureParams(&TexParam2DMipMap::Instance);
	SamplerApplicator* uniformBinding2 = new SamplerApplicator("normaltexture");
	TextureBinding* binding2 = TextureBindManager::Instance()->CreateBinding(camelTexture, uniformBinding2);
/////////////////////////////////////////////////////////////////
	// 11/16
	// texture for the brush - unfinished step7_shaderprogram is using
	//ITextureDataObject* brushtexture = new BlankTexture2D(1024, 1024);
	// 11/26
	// ITextureDataObject* brushtexture = Createrendertexture();
	ITextureDataObject* brushtexture = new BlankTexture2D(1024, 1024, Color(1, 1, 1, 1), GL_RGB);
	brushtexture->setTextureParams(&TexParam2DMipMap::Instance);
	SamplerApplicator* uniformBinding3 = new SamplerApplicator("diffusetexture");
	TextureBinding* binding3 = TextureBindManager::Instance()->CreateBinding(brushtexture, uniformBinding3);
//////////////////////////////////////////////////////////////////////////////////
	// 11/12 step7
	VertexRoutine* step7_vs = new VertexRoutine("..\\Media\\Shaders\\step7.vert");
	FragmentRoutine* step7_fs = new FragmentRoutine("..\\Media\\Shaders\\step7.frag");
	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(step7_vs, step7_fs);
	SimpleShaderMaterial* texturedMaterial = new SimpleShaderMaterial(shaderProgram);
	texturedMaterial->setShaderConstant(uniformBinding2);
	texturedMaterial->setShaderConstant(uniformBinding3);
	texturedMaterial->AddTexture(binding2);
	texturedMaterial->AddTexture(binding3);
/////////////////////////////////////////////////////////////////////////////////
	// step 1 for scene 2
	VertexRoutine * step1_vs = new VertexRoutine("..\\Media\\Shaders\\step1.vert");
	FragmentRoutine* step1_fs = new FragmentRoutine("..\\Media\\Shaders\\step1.frag");
	IShaderProgram* step1_shaderProgram = new ShaderProgramWithMatrices(step1_vs, step1_fs);
	SimpleShaderMaterial* texturedMaterial2 = new SimpleShaderMaterial(step1_shaderProgram);
////////////////////////////////////////////////////////////////////////
	// colorful uv plot for scene 3
	VertexRoutine* uv_vs = new VertexRoutine("..\\Media\\Shaders\\uv.vert");
	FragmentRoutine* uv_fs = new FragmentRoutine("..\\Media\\Shaders\\uv.frag");
	IShaderProgram* uv_shaderProgram = new ShaderProgramWithMatrices(uv_vs, uv_fs);
	SimpleShaderMaterial* texturedMaterial3 = new SimpleShaderMaterial(uv_shaderProgram);
////////////////////////////////////////////////////////////////////////////////////
	// step3_shaderprogram
	ITextureDataObject* imageTexture = new TextureDataImage("../Media/Textures/UVGrid.jpg", GL_RGB);
	imageTexture->setTextureParams(&TexParam2DMipMap::Instance);
	SamplerApplicator* uniformBinding = new SamplerApplicator("texture");
	TextureBinding* binding = TextureBindManager::Instance()->CreateBinding(imageTexture, uniformBinding);
	VertexRoutine* step3_vs = new VertexRoutine("..\\Media\\Shaders\\step3.vert");
	FragmentRoutine* step3_fs = new FragmentRoutine("..\\Media\\Shaders\\step3.frag");
	IShaderProgram* step3_shaderProgram = new ShaderProgramWithMatrices(step3_vs, step3_fs);
	SimpleShaderMaterial* texturedMaterial4 = new SimpleShaderMaterial(step3_shaderProgram);
	texturedMaterial4->setShaderConstant(uniformBinding);
	texturedMaterial4->AddTexture(binding);
////////////////////////////////////////////////////////////////////////////////
	ShadedMaterial* white = new ShadedMaterial(shaderProgram);
	white->setAmbientReflection(0.0f*Colors::White);
	white->setDiffuseReflection(0.75f*Colors::White);
	white->setSpecularReflection(0.25f*Colors::White);
	white->setShininess(10.0f);
	ShaderConstantMaterial* materialConstant = new ShaderConstantMaterial("frontMaterial");
	materialConstant->setValue(white);
	ShaderConstantLights* lightConstant = new ShaderConstantLights();
	ShaderConstantCollection* shaderConstantList = new ShaderConstantCollection();
	shaderConstantList->AddConstant(materialConstant);
	shaderConstantList->AddConstant(lightConstant);
	shaderConstantList->AddConstant(uniformBinding2);
	shaderConstantList->AddConstant(uniformBinding3);
	shaderProgram->setShaderConstant(shaderConstantList);
/////////////////////////////////////////////////////////////////////////////////////
	// light for step3_program
	ShadedMaterial* white2 = new ShadedMaterial(step3_shaderProgram);
	white2->setAmbientReflection(0.0f*Colors::White);
	white2->setDiffuseReflection(0.75f*Colors::White);
	white2->setSpecularReflection(0.25f*Colors::White);
	white2->setShininess(10.0f);
	ShaderConstantMaterial* materialConstant2 = new ShaderConstantMaterial("frontMaterial");
	materialConstant2->setValue(white2);
	ShaderConstantLights* lightConstant2 = new ShaderConstantLights();
	ShaderConstantCollection* shaderConstantList2 = new ShaderConstantCollection();
	shaderConstantList2->AddConstant(materialConstant2);
	shaderConstantList2->AddConstant(lightConstant2);
	step3_shaderProgram->setShaderConstant(shaderConstantList2);
/////////////////////////////////////////////////////////////////////////////////
	MaterialManager::Instance()->RegisterMaterial("Textured", texturedMaterial);
	MaterialManager::Instance()->RegisterMaterial("step1_Textured", texturedMaterial2);
	MaterialManager::Instance()->RegisterMaterial("uv_Textured", texturedMaterial3);
	MaterialManager::Instance()->RegisterMaterial("step3_Textured", texturedMaterial4);
}

// 11/28
void CreateDepth() {
	VertexRoutine* depth_vs = new VertexRoutine("..\\Media\\Shaders\\depth.vert");
	FragmentRoutine* depth_fs = new FragmentRoutine("..\\Media\\Shaders\\depth.frag");
	IShaderProgram* depth_shaderProgram = new ShaderProgramWithMatrices(depth_vs, depth_fs);
	ShadedMaterial* depth = new ShadedMaterial(depth_shaderProgram);
	MaterialManager::Instance()->RegisterMaterial("Depth_Textured", depth);
}



ISceneNode* CreateRenderScene()
{
	CreateDepth();
	DrawableObject * object = new DrawableObject("..\\Media\\camel_decim.obj");
	ModelManager::Instance()->RegisterModel("Depth_Object", object);
	CreateLights();
	CreateGoldMaterial();
	CreateTexturedMaterial();
	DrawableProxy* objectNode = new DrawableProxy("Depth_Object", "Depth_Object");
	MaterialProxy* materialNode = new MaterialProxy("Depth Material 1", "Depth_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform = new TransformMatrixNodeSolution("DepthObjectSpace1", materialNode);
	objectTransform->Scale(3, 3, 3);
	objectTransform->Translate(0, 1.0, 0);
	// floor
	// 11/30
	
	DrawableFloor* floor = new DrawableFloor(10, 10);
	ModelManager::Instance()->RegisterModel("Depth_Floor", floor);
	DrawableProxy* floorNode = new DrawableProxy("Depth_Floor", "Depth_Floor"); 	// It is okay if they have the same name.
	MaterialProxy* materialNode2 = new MaterialProxy("Floor Material", "Depth_Textured", floorNode);
	TransformMatrixNodeSolution* floorTransform = new TransformMatrixNodeSolution("DepthFloorSpace", materialNode2);
	floorTransform->Scale(10, 1, 10);
	floorTransform->Translate(1.0, -1.1, 0);
	// camel 2
	// object 2
	MaterialProxy* materialNode3 = new MaterialProxy("Depth Material 2", "Depth_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform2 = new TransformMatrixNodeSolution("DepthObjectSpace2", materialNode3);
	objectTransform2->Scale(3, 3, 3);
	objectTransform2->Translate(2.0, 1.0, 0.0);

	
	GroupNode* group = new GroupNode("Render_Pedestal");
	group->AddChild(floorTransform);
	group->AddChild(objectTransform);
	group->AddChild(objectTransform2);
	render_examiner = new ExaminerCameraNode("Render_Examiner", group, true);
	return render_examiner;

}



void CreateShadowTexture() {
	ITextureDataObject* shadowtexture = new BlankTexture2D(1024, 1024, GL_RGB);
//ITextureDataObject* shadowtexture = new BlankDepthTexture2D(1024, 1024);
	shadowtexture->setTextureParams(&TexParam2DMipMap::Instance);
	shadowtexture->Enable();
	shadowtexture->Disable();
	std::vector<ITextureDataObject*>renderTextures;
	renderTextures.push_back(shadowtexture);

IDepthBuffer* depthBuffer = new DepthBuffer(1024, 1024, false);
IRenderTarget* depth = new RenderTarget(renderTextures, depthBuffer);
	//IRenderTarget* depth = new RenderTarget(renderTextures);
	depth->Enable();
	
//////////////////////////////////////////////////////////////////
	ISceneNode * depth_scene = CreateRenderScene();
	RenderManager::Instance()->RegisterRenderTarget("Depth_Screen", depth);
	depth->setEnableCommand(new ClearFrameCommand(Colors::White));
	depth->setDisableCommand(new SwapCommand(true));
	RenderTargetProxy* frameBuffer = new RenderTargetProxy("NOTDisplay", "Depth_Screen", depth_scene);
	renderSceneNode = frameBuffer;
	DepthrenderVisitor = new DebugRenderVisitor();
	renderSceneNode->Accept(DepthrenderVisitor);
/////////////////////////////////////////////////////////////////////////////////
//	// start to use the shadowtexture
	shadowtexture = renderTextures[0];
	shadowtexture->Enable();
	glGenerateMipmap(GL_TEXTURE_2D);
	ITextureDataObject* camelTexture = new TextureDataImage("../Media/Textures/camel_normals.png", GL_RGB);
	camelTexture->setTextureParams(&TexParam2DMipMap::Instance);
	SamplerApplicator* normalSampler = new SamplerApplicator("normaltexture");
	SamplerApplicator* shadowSampler = new SamplerApplicator("shadowMap");
	TextureBinding* normalbinding = TextureBindManager::Instance()->CreateBinding(camelTexture, normalSampler);
	TextureBinding* shadowbinding = TextureBindManager::Instance()->CreateBinding(shadowtexture, shadowSampler);
	VertexRoutine* shadowmap_vs = new VertexRoutine("..\\Media\\Shaders\\shadowmap.vert");
	FragmentRoutine* shadowmap_fs = new FragmentRoutine("..\\Media\\Shaders\\shadowmap.frag");
	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(shadowmap_vs, shadowmap_fs);
	SimpleShaderMaterial* texturedMaterial = new SimpleShaderMaterial(shaderProgram);
	texturedMaterial->AddTexture(normalbinding);
	texturedMaterial->AddTexture(shadowbinding);

	ShadedMaterial* white = new ShadedMaterial(shaderProgram);
	white->setAmbientReflection(0.0f*Colors::White);
	white->setDiffuseReflection(0.75f*Colors::White);
	white->setSpecularReflection(0.25f*Colors::White);
	white->setShininess(10.0f);
	ShaderConstantMaterial* materialConstant = new ShaderConstantMaterial("frontMaterial");
	materialConstant->setValue(white);
	ShaderConstantLights* lightConstant = new ShaderConstantLights();
	ShaderConstantCollection* shaderConstantList = new ShaderConstantCollection();
	shaderConstantList->AddConstant(materialConstant);
	shaderConstantList->AddConstant(lightConstant);
	shaderConstantList->AddConstant(normalSampler);
	shaderConstantList->AddConstant(shadowSampler);
	shaderProgram->setShaderConstant(shaderConstantList);

	MaterialManager::Instance()->RegisterMaterial("Shadow_Textured", texturedMaterial);

/////////////////////////////////////////////////////////////////////////////////////////////
	// floor shadow
	// 12/04
	
	VertexRoutine* floor_vs = new VertexRoutine("..\\Media\\Shaders\\floor.vert");
	FragmentRoutine* floor_fs = new FragmentRoutine("..\\Media\\Shaders\\floor.frag");
	IShaderProgram* floor_shaderProgram = new ShaderProgramWithMatrices(floor_vs, floor_fs);
	SimpleShaderMaterial*  floor_texturedMaterial = new SimpleShaderMaterial(floor_shaderProgram);
	ITextureDataObject* floorTexture = new TextureDataImage("../Media/Textures/UVGrid.jpg", GL_RGB);
	floorTexture->setTextureParams(&TexParam2DMipMap::Instance);
	SamplerApplicator* diffuseSampler = new SamplerApplicator("diffusetexture");
	TextureBinding* diffusebinding = TextureBindManager::Instance()->CreateBinding(floorTexture, diffuseSampler);
	floor_texturedMaterial->AddTexture(diffusebinding);
	floor_texturedMaterial->AddTexture(shadowbinding);
	ShaderConstantCollection* floor_shaderConstantList = new ShaderConstantCollection();
	ShaderConstantMaterial* materialConstant2 = new ShaderConstantMaterial("frontMaterial");
	materialConstant2->setValue(white);
	floor_shaderConstantList->AddConstant(materialConstant2);
	floor_shaderConstantList->AddConstant(lightConstant);
	floor_shaderConstantList->AddConstant(diffuseSampler);
	floor_shaderConstantList->AddConstant(shadowSampler);
	floor_shaderProgram->setShaderConstant(floor_shaderConstantList);
	MaterialManager::Instance()->RegisterMaterial("Floor_Textured", floor_texturedMaterial);
}

ISceneNode* CreateSceneGraph()
{
	// 11/27
	// Create a simple scene
	// Perspective
	// LookAt camera
	// Drawable Cube
	//
	// First, create the models and register them.
	DrawableFloor* floor = new DrawableFloor(10,10);
	ModelManager::Instance()->RegisterModel("Floor", floor);
	DrawableObject * object= new DrawableObject("..\\Media\\camel_decim.obj");
	ModelManager::Instance()->RegisterModel("Object", object);
	DrawableProxy* floorNode = new DrawableProxy("Floor", "Floor"); 	// It is okay if they have the same name.
	DrawableProxy* objectNode = new DrawableProxy("Object", "Object");
	// Add a material
	//SolidColorMaterialSolution* scarlet = new SolidColorMaterialSolution(Colors::White);
	//MaterialManager::Instance()->RegisterMaterial("Scarlet", scarlet);
	CreateLights();
	CreateGoldMaterial();
	CreateTexturedMaterial();
	CreateShadowTexture();
	// object 1
	MaterialProxy* materialNode = new MaterialProxy("Camel Material 1", "Shadow_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform = new TransformMatrixNodeSolution("Camel Space 1", materialNode);
	objectTransform->Scale(3, 3, 3);
	objectTransform->Translate(0, 1.0, 0);

	// floor
	MaterialProxy* materialNode2 = new MaterialProxy("Floor Material", "Floor_Textured", floorNode);
	TransformMatrixNodeSolution* floorTransform = new TransformMatrixNodeSolution("FloorSpace", materialNode2);
	floorTransform->Scale(10, 1, 10);
	//floorTransform->Rotate(1.57f, Vector3(1, 0, 0));
	floorTransform->Translate(1.0, -1.1, 0);

	// object 2
	MaterialProxy* materialNode3 = new MaterialProxy("Camel Material 2", "Shadow_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform2 = new TransformMatrixNodeSolution("Camel Space 2", materialNode3);
	objectTransform2->Scale(3, 3, 3);
	objectTransform2->Translate(2.0, 1.0, 0.0);


	// background
	MaterialProxy* materialNode4 = new MaterialProxy("Floor Material", "step3_Textured", floorNode);
	TransformMatrixNodeSolution* floorTransform2 = new TransformMatrixNodeSolution("FloorSpace", materialNode4);
	floorTransform2->Scale(30, 1, 30);
	floorTransform2->Rotate(1.57f, Vector3(1, 0, 0));
	floorTransform2->Translate(1.0, -1.1, -3);



	GroupNode* group = new GroupNode("Pedestal");
	group->AddChild(floorTransform);
	group->AddChild(floorTransform2);
	group->AddChild(objectTransform);
	group->AddChild(objectTransform2);
	//Vector3 eyePosition = Vector3(-5.0,43.0,-4.998);
	//Vector3 centerOfInterest = Vector3(0,13,0);
	//Vector3 viewUp = Vector3(0,1,0);
	examiner = new ExaminerCameraNode("Eye_Examiner", group);
	// 12/01
	//render_examiner = new ExaminerCameraNode("Light_Examiner", group, true);
	examiner->setWidth(windowWidth);
	examiner->setHeight(windowHeight);
	//ISceneNode* lookAt = new LookAtTransformSolution("LookAt", materialNode, eyePosition, centerOfInterest, viewUp);
	//ISceneNode* rootNode = new PerspectiveTransformSolution("Perspective", lookAt, 30, windowWidth, windowHeight, 0.1, 100);
	return examiner;

}
///////////////////////////////////////////////////

ISceneNode* CreateSceneGraph2()
{
	CreateDepth();
	DrawableObject * object = new DrawableObject("..\\Media\\camel_decim.obj");
	ModelManager::Instance()->RegisterModel("Depth_Object", object);
	CreateLights();
	CreateGoldMaterial();
	CreateTexturedMaterial();
	DrawableProxy* objectNode = new DrawableProxy("Depth_Object", "Depth_Object");
	MaterialProxy* materialNode = new MaterialProxy("Depth Material 1", "Depth_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform = new TransformMatrixNodeSolution("DepthObjectSpace1", materialNode);
	objectTransform->Scale(3, 3, 3);
	objectTransform->Translate(0, 1.0, 0);
	// floor
	// 11/30

	DrawableFloor* floor = new DrawableFloor(10, 10);
	ModelManager::Instance()->RegisterModel("Depth_Floor", floor);
	DrawableProxy* floorNode = new DrawableProxy("Depth_Floor", "Depth_Floor"); 	// It is okay if they have the same name.
	MaterialProxy* materialNode2 = new MaterialProxy("Floor Material", "Depth_Textured", floorNode);
	TransformMatrixNodeSolution* floorTransform = new TransformMatrixNodeSolution("DepthFloorSpace", materialNode2);
	floorTransform->Scale(10, 1, 10);
	floorTransform->Translate(1.0, -1.1, 0);
	// camel 2
	// object 2
	MaterialProxy* materialNode3 = new MaterialProxy("Depth Material 2", "Depth_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform2 = new TransformMatrixNodeSolution("DepthObjectSpace2", materialNode3);
	objectTransform2->Scale(3, 3, 3);
	objectTransform2->Translate(2.0, 1.0, 0.0);


	GroupNode* group = new GroupNode("Render_Pedestal");
	group->AddChild(floorTransform);
	group->AddChild(objectTransform);
	group->AddChild(objectTransform2);
	examiner2 = new ExaminerCameraNode("Render_Examiner", group, true);
	return examiner2;
}



ISceneNode* CreateSceneGraph3()
{
	DrawableObject * object = new DrawableObject("..\\Media\\camel_decim.obj");
	ModelManager::Instance()->RegisterModel("Object", object);
	DrawableProxy* objectNode = new DrawableProxy("Object", "Object");
	CreateLights();
	CreateGoldMaterial();
	CreateTexturedMaterial();
	MaterialProxy* materialNode = new MaterialProxy("Object Material", "uv_Textured", objectNode);
	TransformMatrixNodeSolution* objectTransform = new TransformMatrixNodeSolution("ObjectSpace", materialNode);
	objectTransform->Scale(3, 3, 3);
	objectTransform->Translate(0, 1.0, 0);
	GroupNode* group = new GroupNode("Pedestal");
	group->AddChild(objectTransform);
	examiner3 = new ExaminerCameraNode("Examiner", group);
	examiner3->setWidth(windowWidth);
	examiner3->setHeight(windowHeight);
	return examiner3;
}



ISceneNode* CreateSceneGraph4()
{

	// brush texture and shader
	ITextureDataObject* brushtexture = new BlankTexture2D(1024, 1024, Color(1, 0, 0, 1), GL_RGB);
	brushtexture->setTextureParams(&TexParam2DMipMap::Instance);
	SamplerApplicator* uniformBinding7 = new SamplerApplicator("brushTexture");
	TextureBinding* binding7 = TextureBindManager::Instance()->CreateBinding(brushtexture, uniformBinding7);
	VertexRoutine* brush_vs = new VertexRoutine("..\\Media\\Shaders\\brush.vert");
	GeometryRoutine* brush_gs = new GeometryRoutine("..\\Media\\Shaders\\brush.geom");
	FragmentRoutine* brush_fs = new FragmentRoutine("..\\Media\\Shaders\\brush.frag");
	IShaderProgram* brush_shaderProgram = new ShaderProgramWithMatrices(brush_vs, brush_gs, brush_fs);
	SimpleShaderMaterial* texturedMaterial4 = new SimpleShaderMaterial(brush_shaderProgram);
	texturedMaterial4->setShaderConstant(uniformBinding7);
	texturedMaterial4->AddTexture(binding7);
	MaterialManager::Instance()->RegisterMaterial("brush_Textured", texturedMaterial4);
	// 11/26
	// draw the brush
	
	DrawableBrush * brush = new DrawableBrush(PaintController::brushPath);
	ModelManager::Instance()->RegisterModel("Brush", brush);
	DrawableProxy* brushNode = new DrawableProxy("Brush", "Brush");
	MaterialProxy* materialNode2 = new MaterialProxy("Brush Material", "brush_Textured", brushNode);
	TransformMatrixNodeSolution* brushTransform = new TransformMatrixNodeSolution("BrushSpace", materialNode2);
	
	GroupNode* group = new GroupNode("Pedestal");
	group->AddChild(brushTransform);
	paint = new PaintController("Painter", group);
	examiner4 = new ExaminerCameraNode("Examiner", group);
	// 11/26
	//Createrendertexture();
	return examiner4;
}



//////////////////////////////////////////////////////////////////
void DisplayFrame()
{
	//rootSceneNode->Accept(renderVisitor);
	glEnable(GL_SCISSOR_TEST);
	//examiner->setWidth(windowHeight - 20);
	//examiner->setHeight(windowHeight - 20);

	// left main scene
	glViewport(0, 0, 600, 600);
	glScissor(0, 0, 600, 600);
	rootSceneNode->Accept(renderVisitor);
	// 12/03
	//renderSceneNode->Accept(renderVisitor);

	int viewportSize = 180;
	int sideViewSize = 10;
	// right up scene
	glViewport(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 1 * viewportSize - 1 * sideViewSize, viewportSize, viewportSize);
	glScissor(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 1 * viewportSize - 1 * sideViewSize, viewportSize, viewportSize);
	rootSceneNode2->Accept(renderVisitor);

	// right medium scene
	glViewport(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 2 * viewportSize - 2 * sideViewSize, viewportSize, viewportSize);
	glScissor(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 2 * viewportSize - 2 * sideViewSize, viewportSize, viewportSize);
	//rootSceneNode3->Accept(renderVisitor);
	// right down scene
	glViewport(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 3 * viewportSize - 3 * sideViewSize, viewportSize, viewportSize);
	glScissor(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 3 * viewportSize - 3 * sideViewSize, viewportSize, viewportSize);
	//rootSceneNode4->Accept(renderVisitor);

}

void ReshapeWindow(int newWidth, int newHeight)
{
	windowWidth = newWidth;
	windowHeight = newHeight;
	examiner->setWidth(windowWidth);
	examiner->setHeight(windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glutPostRedisplay();
}

ISceneNode* CreateFrameBuffer(Crawfis::Graphics::ISceneNode * scene)
{
	IRenderTarget* screen = new RenderTarget();
	RenderManager::Instance()->RegisterRenderTarget("Screen", screen);
	screen->setEnableCommand(new ClearFrameCommand(Colors::Scarlet));
	screen->setDisableCommand(new SwapCommand(true));
	RenderTargetProxy* frameBuffer = new RenderTargetProxy("Screen Display", "Screen", scene);
	return frameBuffer;
}



void KeyboardController(unsigned char key, int x, int y)
{
	printf("Key Pressed: %c\n", key);
	std::vector<IKeyboardHandler*>::iterator handlerIterator;
	for (handlerIterator = keyboardHandlers.begin(); handlerIterator != keyboardHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->KeyPress(key, x, y);
	}
	glutPostRedisplay();
}

void NumPadController(int key, int x, int y)
{
	std::vector<IKeyboardHandler*>::iterator handlerIterator;
	for (handlerIterator = keyboardHandlers.begin(); handlerIterator != keyboardHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->NumPadPress(key, x, y);
	}
	glutPostRedisplay();
}

void MousePressController(int button, int state, int ix, int iy)
{
	std::vector<IMouseHandler*>::iterator handlerIterator;
	for (handlerIterator = mouseHandlers.begin(); handlerIterator != mouseHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->MouseEvent(button, state, ix, iy);
	}
	glutPostRedisplay();
}

void MouseMotionController(int ix, int iy)
{
	std::vector<IMouseHandler*>::iterator handlerIterator;
	for (handlerIterator = mouseHandlers.begin(); handlerIterator != mouseHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->MouseMoved(ix, iy);
	}
	glutPostRedisplay();
}

void IdleCallback()
{
}
void InitializeDevIL()
{
	::ilInit();
	::iluInit();
	::ilutInit();
	//::ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	//::ilEnable(IL_ORIGIN_SET);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	InitializeDevices();
	InitializeDevIL();
	TextureBindManagerOpenGL::Init();

	MatrixStack::Init();
	// 11/13
	ISceneNode* scene = CreateSceneGraph();
	rootSceneNode = CreateFrameBuffer(scene);
///////////////////////////////////////////////////////
	// 11/13
	ISceneNode * scene2 = CreateSceneGraph2();
	rootSceneNode2 = CreateFrameBuffer(scene2);

/////////////////////////////////////////////////////////////
	//ISceneNode * scene3 = CreateSceneGraph3();
	//rootSceneNode3 = CreateFrameBuffer(scene3);
///////////////////////////////////////////////////
	// 11/16
	//ISceneNode * scene4 = CreateSceneGraph4();
	//rootSceneNode4 = CreateFrameBuffer(scene4);

	// 11/29
	//ISceneNode *render_scene = CreateRenderScene();
	//renderSceneNode = CreateFrameBuffer(render_scene);

//////////////////////////////////////////////////
	renderVisitor = new DebugRenderVisitor();
	PrintSceneVisitor* printScene = new PrintSceneVisitor();
	rootSceneNode->Accept(printScene);
	rootSceneNode2->Accept(printScene);
	//rootSceneNode3->Accept(printScene);
	//rootSceneNode4->Accept(printScene);

	// 11/30
	//DepthrenderVisitor = new DebugRenderVisitor();
	//renderSceneNode->Accept(DepthrenderVisitor);


	keyboardHandlers.push_back(examiner);
	mouseHandlers.push_back(examiner);

	// 11/24
	//keyboardHandlers.push_back(paint);
	//mouseHandlers.push_back(paint);
///////////////////////////////////////////////////////////////////
	// 12/03
	//keyboardHandlers.push_back(examiner2);
	//mouseHandlers.push_back(examiner2);
	// 11/13
	//keyboardHandlers.push_back(examiner3);
	//mouseHandlers.push_back(examiner3);
	//keyboardHandlers.push_back(examiner4);
	//mouseHandlers.push_back(examiner4);
	

	// 11/29
	//keyboardHandlers.push_back(render_examiner);
	//mouseHandlers.push_back(render_examiner);





	glutDisplayFunc(DisplayFrame);
	glutReshapeFunc(ReshapeWindow);
	glutKeyboardFunc(KeyboardController);
	glutSpecialFunc(NumPadController);
	glutMouseFunc(MousePressController);
	glutMotionFunc(MouseMotionController);

	glutMainLoop();

	return 0;
}