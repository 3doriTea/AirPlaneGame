#include "PlayerPilot.h"
#include "PlayerBullet.h"

using namespace mtgb;

PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })

	.Build()),
	pTransform{ Component<Transform>() },
	lockOnSide_{400},
	enemyFrameSideExtents_{15}
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();

	rectCenter_.x = screenSize.x / 2.0f;
	rectCenter_.y = screenSize.y / 2.0f;

	lockOnRect_.x = rectCenter_.x - lockOnSide_ / 2;
	lockOnRect_.width = lockOnSide_;
	lockOnRect_.y = rectCenter_.y - lockOnSide_ / 2;
	lockOnRect_.height = lockOnSide_;

	pTransform->SetParent(_plane);
	lockOnFrame_ = Image::Load("Image/lockOnFrame.png");
	lockOnEnemyFrame_ = Image::Load("Image/lockOnEnemyFrame.png");

	enemyFrameRect_.size = { enemyFrameSideExtents_ * 2,enemyFrameSideExtents_ * 2};
	

	uiParams_.layerFlag = GameObjectLayer::A;
}

PlayerPilot::~PlayerPilot()
{
}

void PlayerPilot::Update()
{
	LockOn();
	if (InputUtil::GetKeyDown(KeyCode::Space))
	{
		LOGIMGUI("Pilot:shoot");
		Shoot();
	}

	MTImGui::Instance().TypedShow(pTransform, "PlayerPilot");
}

void PlayerPilot::Draw() const
{
	//©“®‚Å‘_‚¢‚ğ’è‚ß‚é”ÍˆÍ‚ğ•`‰æ
	const Vector2Int DRAW_SIZE{ lockOnSide_ ,lockOnSide_ };
	Draw::Image(lockOnFrame_, lockOnRect_,uiParams_);

	//‘_‚¢‚ª’è‚Ü‚Á‚Ä‚¢‚é“G‚ğ‹­’²•\¦
	if (lockOnAny)
	{
		Draw::Image(lockOnEnemyFrame_, enemyFrameRect_,uiParams_);
	}
	Draw::ImmediateText("apple", { 0,0 });
}

void PlayerPilot::LockOn()
{
	static std::vector<RectContainsInfo> enemies;
	// TODO : WindowContext‚ğ‚×‚½‘‚«‚Å‚È‚­PlayerPilot‚ª©g‚Ì‚ğ•Û‚·‚é‚æ‚¤‚É!!!
	Game::System<ColliderCP>().RectContains(lockOnRect_, "Enemy", &enemies, WindowContext::First);
	if (enemies.empty())
	{
		lockOnAny = false;
		return;
	}
	lockOnAny = true;

	// ƒ[ƒ‹ƒhÀ•WŒn‚Åˆê”Ô‹ß‚¢“G‚ğ‘_‚¤
	RectContainsInfo& nearestEnemy = enemies.front();
	for (auto& enemy : enemies)
	{
		float enemyDis = (pTransform->position - enemy.worldPos).Size();
		float nearestDis = (pTransform->position - nearestEnemy.worldPos).Size();

		if (enemyDis < nearestDis)
		{
			nearestEnemy = enemy;
		}
	}

	targetInfo_ = nearestEnemy;

	enemyFrameRect_.x = targetInfo_.screenPos.x - enemyFrameSideExtents_;
	enemyFrameRect_.y = targetInfo_.screenPos.y - enemyFrameSideExtents_;
}

void PlayerPilot::Shoot()
{
	Vector3 toTarget = Vector3::Normalize(targetInfo_.worldPos - pTransform->position);
	Quaternion shootDir = Quaternion::LookRotation(toTarget, Vector3::Up());
	Instantiate<PlayerBullet>(pTransform->position + Vector3::Forward() * 1.0f, shootDir);
}
