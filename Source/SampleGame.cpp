#include "SampleGame.h"
#include "PlayScene.h"

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

	_register.Set<AssetsManager>(SystemUpdateType::DontCallMe);

	_register.Set<Screen>(SystemUpdateType::DontCallMe);
	_register.Set<MainWindow>(SystemUpdateType::Cycle);

	_register.Set<DirectX11Manager>(SystemUpdateType::Frame);
	_register.Set<Direct2D>(SystemUpdateType::Frame);
	_register.Set<DirectWrite>(SystemUpdateType::Frame);
	_register.Set<MTImGui>(SystemUpdateType::Frame);

	_register.Set<Input>(SystemUpdateType::Frame);

	_register.Set<RigidBodyCP>(SystemUpdateType::Frame, true);
	_register.Set<TransformCP>(SystemUpdateType::Frame, true);
	_register.Set<AudioPlayerCP>(SystemUpdateType::Frame, true);

	_register.Set<SceneSystem>(SystemUpdateType::Frame);

	_register.Set<Time>(SystemUpdateType::Cycle);
	_register.Set<Timer>(SystemUpdateType::Frame);

	_register.Set<Debug>(SystemUpdateType::Cycle);

	_register.Set<Image>(SystemUpdateType::DontCallMe);
	_register.Set<Audio>(SystemUpdateType::Frame);
	_register.Set<OBJ>(SystemUpdateType::Frame);
	_register.Set<Fbx>(SystemUpdateType::Frame);
	_register.Set<Text>(SystemUpdateType::Frame);

	_register.Set<Draw>(SystemUpdateType::DontCallMe);


	// 開始時のシーン
	Game::System<SceneSystem>().Move<PlayScene>();

	//_register<MainWindow>(SystemUpdateType::Cycle);
}
