#include "QCWorld.hpp"

#include <AssetLibrary/Model.hpp>
#include "AssetLibrary/Image.hpp"

#include <array>
#include <random>

inline float rand2() {
	return (rand() / float(RAND_MAX)) * 2 - 1;
}

QCWorld::QCWorld(inl::gxeng::GraphicsEngine* graphicsEngine) {
	using namespace inl::gxeng;

	m_graphicsEngine = graphicsEngine;

	// Create scene and camera
	m_worldScene.reset(m_graphicsEngine->CreateScene("World"));
	//m_sun.SetColor({1.0f, 0.63f, 0.46f});
	//m_sun.SetDirection({ 0.8f, -0.7f, -0.15f });
	m_sun.SetColor({1.0f, 0.9f, 0.85f});
	m_sun.SetDirection({ 0.8f, -0.7f, -0.9f });
	m_worldScene->SetSun(&m_sun);
	m_camera.reset(m_graphicsEngine->CreateCamera("WorldCam"));
	m_camera->SetTargeted(true);
	m_camera->SetTarget({ 0, 0, 0 });
	m_camera->SetPosition({ 0, -8, 3 });
	m_camera->SetUpVector({0, 0, 1});

	inl::asset::CoordSysLayout coordSysLayout = {
		inl::asset::AxisDir::POS_X, inl::asset::AxisDir::POS_Z, inl::asset::AxisDir::NEG_Y
	};

	// Create terrain mesh
	{
		inl::asset::Model model("assets\\terrain.fbx");

		auto modelVertices = model.GetVertices<Position<0>, Normal<0>, TexCoord<0>>(0, coordSysLayout);
		std::vector<unsigned> modelIndices = model.GetIndices(0);

		m_terrainMesh.reset(m_graphicsEngine->CreateMesh());
		m_terrainMesh->Set(modelVertices.data(), modelVertices.size(), modelIndices.data(), modelIndices.size());
	}

	// Create terrain texture
	{
		using PixelT = Pixel<ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR>;
		inl::asset::Image img("assets\\terrain.jpg");

		m_terrainTexture.reset(m_graphicsEngine->CreateImage());
		m_terrainTexture->SetLayout(img.GetWidth(), img.GetHeight(), ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR);
		m_terrainTexture->Update(0, 0, img.GetWidth(), img.GetHeight(), img.GetData(), PixelT::Reader());
	}

	// Create QC mesh
	{
		inl::asset::Model model("assets\\quadcopter.fbx");

		auto modelVertices = model.GetVertices<Position<0>, Normal<0>, TexCoord<0>>(0, coordSysLayout);
		std::vector<unsigned> modelIndices = model.GetIndices(0);

		m_quadcopterMesh.reset(m_graphicsEngine->CreateMesh());
		m_quadcopterMesh->Set(modelVertices.data(), modelVertices.size(), modelIndices.data(), modelIndices.size());
	}

	// Create QC texture
	{
		using PixelT = Pixel<ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR>;
		inl::asset::Image img("assets\\axes.jpg");

		m_axesTexture.reset(m_graphicsEngine->CreateImage());
		m_axesTexture->SetLayout(img.GetWidth(), img.GetHeight(), ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR);
		m_axesTexture->Update(0, 0, img.GetWidth(), img.GetHeight(), img.GetData(), PixelT::Reader());
	}

	// Create axes mesh
	{
		inl::asset::Model model("assets\\axes.fbx");

		auto modelVertices = model.GetVertices<Position<0>, Normal<0>, TexCoord<0>>(0, { inl::asset::AxisDir::POS_Z, inl::asset::AxisDir::POS_Y, inl::asset::AxisDir::POS_X });
		std::vector<unsigned> modelIndices = model.GetIndices(0);

		m_axesMesh.reset(m_graphicsEngine->CreateMesh());
		m_axesMesh->Set(modelVertices.data(), modelVertices.size(), modelIndices.data(), modelIndices.size());
	}

	// Create axes texture
	{
		using PixelT = Pixel<ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR>;
		inl::asset::Image img("assets\\quadcopter.jpg");

		m_quadcopterTexture.reset(m_graphicsEngine->CreateImage());
		m_quadcopterTexture->SetLayout(img.GetWidth(), img.GetHeight(), ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR);
		m_quadcopterTexture->Update(0, 0, img.GetWidth(), img.GetHeight(), img.GetData(), PixelT::Reader());
	}

	// Create tree mesh
	{
		inl::asset::Model model("assets\\pine_tree.fbx");

		auto modelVertices = model.GetVertices<Position<0>, Normal<0>, TexCoord<0>>(0, coordSysLayout);
		std::vector<unsigned> modelIndices = model.GetIndices(0);

		m_treeMesh.reset(m_graphicsEngine->CreateMesh());
		m_treeMesh->Set(modelVertices.data(), modelVertices.size(), modelIndices.data(), modelIndices.size());
	}

	// Create tree texture
	{
		using PixelT = Pixel<ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR>;
		inl::asset::Image img("assets\\pine_tree.jpg");

		m_treeTexture.reset(m_graphicsEngine->CreateImage());
		m_treeTexture->SetLayout(img.GetWidth(), img.GetHeight(), ePixelChannelType::INT8_NORM, 3, ePixelClass::LINEAR);
		m_treeTexture->Update(0, 0, img.GetWidth(), img.GetHeight(), img.GetData(), PixelT::Reader());
	}

	// Create tree material
	{
		m_treeMaterial.reset(m_graphicsEngine->CreateMaterial());
		m_treeShader.reset(m_graphicsEngine->CreateMaterialShaderGraph());
		std::unique_ptr<inl::gxeng::MaterialShaderEquation> mapShader(m_graphicsEngine->CreateMaterialShaderEquation());
		std::unique_ptr<inl::gxeng::MaterialShaderEquation> diffuseShader(m_graphicsEngine->CreateMaterialShaderEquation());

		mapShader->SetSourceName("bitmap_color_2d.mtl");
		diffuseShader->SetSourceName("simple_diffuse.mtl");

		std::vector<std::unique_ptr<inl::gxeng::MaterialShader>> nodes;
		nodes.push_back(std::move(mapShader));
		nodes.push_back(std::move(diffuseShader));
		m_treeShader->SetGraph(std::move(nodes), { {0, 1, 0} });
		m_treeMaterial->SetShader(m_treeShader.get());

		(*m_treeMaterial)[0] = m_treeTexture.get();
	}

	// Create checker texture
	{
		using PixelT = Pixel<ePixelChannelType::INT8_NORM, 4, ePixelClass::LINEAR>;
		std::vector<PixelT> imgData = {
			{220, 32, 32, 255},
			{32, 220, 22, 255},
			{32, 32, 220, 255},
			{64, 64, 64, 255}
		};

		m_checkerTexture.reset(m_graphicsEngine->CreateImage());
		m_checkerTexture->SetLayout(2, 2, ePixelChannelType::INT8_NORM, 4, ePixelClass::LINEAR);
		m_checkerTexture->Update(0, 0, 2, 2, imgData.data(), PixelT::Reader());
	}

	// Set up terrain
	m_terrainEntity.reset(m_graphicsEngine->CreateMeshEntity());
	m_terrainEntity->SetMesh(m_terrainMesh.get());
	m_terrainEntity->SetTexture(m_terrainTexture.get());
	m_terrainEntity->SetPosition({ 0,0,0 });
	m_terrainEntity->SetRotation({ 1,0,0,0 });
	m_terrainEntity->SetScale({ 1,1,1 });
	m_worldScene->GetMeshEntities().Add(m_terrainEntity.get());

	// Set up copter
	m_quadcopterEntity.reset(m_graphicsEngine->CreateMeshEntity());
	m_quadcopterEntity->SetMesh(m_quadcopterMesh.get());
	m_quadcopterEntity->SetTexture(m_quadcopterTexture.get());
	m_quadcopterEntity->SetPosition({ 0,0,3 });
	m_quadcopterEntity->SetRotation({ 1,0,0,0 });
	m_quadcopterEntity->SetScale({ 1,1,1 });
	m_worldScene->GetMeshEntities().Add(m_quadcopterEntity.get());

	// Set up axes
	m_axesEntity.reset(m_graphicsEngine->CreateMeshEntity());
	m_axesEntity->SetMesh(m_axesMesh.get());
	m_axesEntity->SetTexture(m_axesTexture.get());
	m_axesEntity->SetPosition({ 0,0,3 });
	m_axesEntity->SetRotation({ 1,0,0,0 });
	m_axesEntity->SetScale({ 1,1,1 });
	//m_worldScene->GetMeshEntities().Add(m_axesEntity.get());

	// Set up trees
	AddTree({ 2, 2, 0 });
	AddTree({ 11, 6, 0 });
	AddTree({ 13, 8, 0 });
	AddTree({ 10.5f, 8.5f, 0 });
	AddTree({ -17, -12, 0 });
	AddTree({ 8, 14, 0 });
	AddTree({ 9, -12, 0 });

	// Set up simulation
	m_rigidBody.SetPosition({0, 0, 1});
	m_rigidBody.SetRotation({ 1, 0, 0, 0 });

	// copter parameters
	float m = 2;
	float Ixx = 0.026;
	float Iyy = 0.024;
	float Izz = 0.048;
	mathfu::Matrix3x3f I = {
		Ixx, 0, 0,
		0, Iyy, 0,
		0, 0, Izz };
	m_rigidBody.SetMass(m);
	m_rigidBody.SetInertia(I);
	m_rigidBody.SetGravity({ 0, 0, -9.81f });
	m_controller.SetInertia(I);
}

void QCWorld::UpdateWorld(float elapsed) {
	// Update QC heading
	m_rotorInfo.heading += 2.0f*((int)m_rotorInfo.rotateLeft - (int)m_rotorInfo.rotateRight)*elapsed;

	// Update simulation
	bool controller = true;
	if (!controller) {
		mathfu::Vector4f rpm = m_rotorInfo.RPM(m_rotor);
		mathfu::Vector3f force;
		mathfu::Vector3f torque;
		m_rotor.SetRPM(rpm, force, torque);
		m_rigidBody.Update(elapsed, force, torque);
	}
	else {
		mathfu::Quaternionf orientation = m_rotorInfo.Orientation();
		mathfu::Quaternionf q = m_rigidBody.GetRotation();
		mathfu::Vector3f force;
		mathfu::Vector3f torque;
		mathfu::Vector4f rpm;
		float lift = 2.0f * 9.81 + 5.f*((int)m_rotorInfo.ascend - (int)m_rotorInfo.descend);
		m_controller.Update(orientation, lift, q, m_rigidBody.GetAngularVelocity(), elapsed, force, torque);
		m_rotor.SetTorque(force, torque, rpm);
		m_rotor.SetRPM(rpm, force, torque);
		m_rigidBody.Update(elapsed, force, torque);
	}

	// Move quadcopter entity
	m_quadcopterEntity->SetPosition(m_rigidBody.GetPosition());
	m_quadcopterEntity->SetRotation(m_rigidBody.GetRotation());

	m_axesEntity->SetPosition(m_quadcopterEntity->GetPosition());
	m_axesEntity->SetRotation(m_rotorInfo.Orientation());

	// Follow copter with camera
	mathfu::Vector3f frontDir = m_rigidBody.GetRotation() * mathfu::Vector3f{ 0,1,0 };
	mathfu::Vector3f upDir = m_rigidBody.GetRotation() * mathfu::Vector3f{ 0,0,1 };
	frontDir.z() = 0;
	upDir.z() = 0;
	mathfu::Vector3f viewDir = (5*frontDir.LengthSquared() > upDir.LengthSquared()) ? frontDir.Normalized() : upDir.Normalized();
	m_camera->SetTarget(m_rigidBody.GetPosition());
	m_camera->SetPosition(m_rigidBody.GetPosition() + (-viewDir * 1.5 + mathfu::Vector3f{ 0,0,-lookTilt }).Normalized() * 1.5f);
}

void QCWorld::SetAspectRatio(float ar) {
	m_camera->SetFOVAspect(75.f / 180.f * 3.1419f, ar);
}

void QCWorld::RenderWorld(float elapsed) {
	m_graphicsEngine->Update(elapsed);
}


void QCWorld::AddTree(mathfu::Vector3f position) {
	std::unique_ptr<inl::gxeng::MeshEntity> tree;

	static std::mt19937_64 rne;
	static std::uniform_real_distribution<float> rng{ 0.8f, 1.2f };
	float s = rng(rne);

	tree.reset(m_graphicsEngine->CreateMeshEntity());
	tree->SetMesh(m_treeMesh.get());
	tree->SetMaterial(m_treeMaterial.get());
	tree->SetTexture(m_treeTexture.get());
	tree->SetPosition(position);
	tree->SetRotation({ 1,0,0,0 });
	tree->SetScale({ s,s,s });
	m_worldScene->GetMeshEntities().Add(tree.get());
	m_staticEntities.push_back(std::move(tree));
}




void QCWorld::TiltForward(float set) {
	m_rotorInfo.front = set;
}
void QCWorld::TiltBackward(float set) {
	m_rotorInfo.back = set;
}
void QCWorld::TiltRight(float set) {
	m_rotorInfo.right = set;
}
void QCWorld::TiltLeft(float set) {
	m_rotorInfo.left = set;
}
void QCWorld::RotateRight(float set) {
	m_rotorInfo.rotateRight = set;
}
void QCWorld::RotateLeft(float set) {
	m_rotorInfo.rotateLeft = set;
}
void QCWorld::Ascend(float set) {
	m_rotorInfo.ascend = set;
}
void QCWorld::Descend(float set) {
	m_rotorInfo.descend = set;
}
void QCWorld::IncreaseBase() {
	//m_rotorInfo.baseRpm += 15.f;
}
void QCWorld::DecreaseBase() {
	//m_rotorInfo.baseRpm -= 15.f;
}
void QCWorld::Heading(float set) {
	m_rotorInfo.heading = set;
}
float QCWorld::Heading() const {
	return m_rotorInfo.heading;
}


void QCWorld::IWantSunsetBitches() {
	m_sun.SetColor({1.0f, 0.65f, 0.25f});
	m_sun.SetDirection({ 0.8f, -0.7f, -0.15f });
}