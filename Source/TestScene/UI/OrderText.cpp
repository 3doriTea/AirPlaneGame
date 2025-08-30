#include "OrderText.h"
#include "../Source/TestScene/EnemyPlane.h"

OrderText::OrderText(const EntityId _playerId, const GameObjectLayer _layer) : GameObject(GameObjectBuilder()
    .SetLayerFlag(_layer)
	.Build()),
	pPlayerTransform_{ &Transform::Get(_playerId) }
{
    orderTexts_ = {
        "‹ß‚­“G‚ª‚¢‚È‚¢‚Ì‚¾I’T‚·‚Ì‚¾",
        "‹ß‚­‚É“G‚ª‚¢‚é‚Ì‚¾I",
    };

    enemyStatus_ = NoEnemy;
    pTextBox_ = nullptr;
}

OrderText::~OrderText()
{
}

void OrderText::Update()
{
	pPlayerTransform_ = &Transform::Get(pPlayerTransform_->GetEntityId());
    static EnemyStatus previousStatus = EnemyStatus::NoEnemy;
    if (enemyStatus_ != previousStatus)
    {
        // ˆÈ‘O‚ÌTextBox‚ª‚ ‚ê‚Î”jŠüiƒƒ‚ƒŠƒŠ[ƒN–h~j
        if (pTextBox_)
        {
            delete pTextBox_;
            pTextBox_ = nullptr;
        }

        switch (enemyStatus_)
        {
        case EnemyStatus::NoEnemy:
            pTextBox_ = Instantiate<TextBox>(orderTexts_[0], 0.08f);
            break;
        case EnemyStatus::NearEnemy:
            pTextBox_ = Instantiate<TextBox>(orderTexts_[1], 0.08f);
            break;
        }
        previousStatus = enemyStatus_;
    }


    std::vector<EnemyPlane*> enemies;
   // FindGameObjects<EnemyPlane>(&enemies);

    for (const auto& enemy : enemies)
    {
        Transform& enemyTransform = Transform::Get(enemy->GetEntityId());
		Vector3 toEnemy = Vector3::Normalize(enemyTransform.position - pPlayerTransform_->position);
        if (toEnemy.Size() < 50.0f)
        {
            enemyStatus_ = EnemyStatus::NearEnemy;
        }
        else
        {
			enemyStatus_ = EnemyStatus::NoEnemy;
        }
    }
}

void OrderText::Draw() const
{
}
