#include "SampleGame.h"

#pragma region ゲーム側で追加

#include "PlayScene.h"
#include "TestScene/TestScene.h"
#include "TrailEmitterSystem.h"
#include "CheckTutorialScene/CheckTutorialScene.h"

#pragma endregion

using namespace mtgb;

SampleGame::SampleGame()
{
	
}

SampleGame::~SampleGame()
{
}

void SampleGame::SetupSystems(const RegisterSystemFuncHolder& _register)
{
	//_register(new MainWindow{}, SystemUpdateType::Cycle);

	//_register.function_
	_register.Set<Time>(SystemUpdateType::Cycle);
	_register.Set<Timer>(SystemUpdateType::Frame);

	_register.Set<RenderSystem>(SystemUpdateType::DontCallMe);
	_register.Set<AssetsManager>(SystemUpdateType::DontCallMe);

	_register.Set<Screen>(SystemUpdateType::DontCallMe);
	_register.Set<WindowManager>(SystemUpdateType::Cycle);

	_register.Set<DirectX11Manager>(SystemUpdateType::Frame);
	_register.Set<Direct2D>(SystemUpdateType::Frame);
	_register.Set<DirectWrite>(SystemUpdateType::Frame);
	_register.Set<Input>(SystemUpdateType::DontCallMe);


	_register.Set<WindowContextResourceManager>(SystemUpdateType::DontCallMe);
	Game::System<WindowContextResourceManager>().RegisterResourceTypes<
		WindowResource,
		DXGIResource,
		Direct3DResource,
		Direct2DResource,
		InputResource,
		CameraResource
	>();

	_register.Set<DoubleWindow>(SystemUpdateType::Frame);
	_register.Set<CameraSystem>(SystemUpdateType::Frame);


	_register.Set<ImGuiRenderer>(SystemUpdateType::Frame);


	_register.Set<SceneSystem>(SystemUpdateType::Frame);

	

	_register.Set<Debug>(SystemUpdateType::Cycle);

	_register.Set<Image>(SystemUpdateType::DontCallMe);
	_register.Set<Audio>(SystemUpdateType::Frame);
	_register.Set<OBJ>(SystemUpdateType::Frame);
	_register.Set<Fbx>(SystemUpdateType::Frame);
	_register.Set<Text>(SystemUpdateType::Frame);

	_register.Set<ColliderCP>(SystemUpdateType::Frame, true);
	_register.Set<RigidBodyCP>(SystemUpdateType::Frame, true);
	_register.Set<TransformCP>(SystemUpdateType::Frame, true);
	_register.Set<AudioPlayerCP>(SystemUpdateType::Frame, true);


	_register.Set<TrailEmitterSystem>(SystemUpdateType::Frame, false);


	_register.Set<Draw>(SystemUpdateType::DontCallMe);


	// 開始時のシーン
	//Game::System<SceneSystem>().Move<PlayScene>();
	Game::System<SceneSystem>().Move<TestScene>();
	//Game::System<SceneSystem>().Move<CheckTutorialScene>();

	//_register<MainWindow>(SystemUpdateType::Cycle);
}
