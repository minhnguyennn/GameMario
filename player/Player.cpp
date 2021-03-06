#include "../Device.h"
#include "../SceneManager.h"
#include "Player.h"

Texture* Player::_playerTexture = nullptr;
std::vector<GameObject::GameObjectType> Player::_bonusItems;

void Player::_ParseSprites(std::string line) {
	_animatedSprite.ParseSprites(line, _playerTexture);
}

void Player::_HandleCurrencies() {
	if (_lives > _MAX_LIVES) {
		_lives = _MAX_LIVES;
	}

	if (_coins > _MAX_COINS) {
		_coins = _MAX_COINS;
	}

	if (_score > _MAX_SCORE) {
		_score = _MAX_SCORE;
	}
}

void Player::_HanldeStageEnd() {
	if (_triggeredStageEnd) {
		if (!_hasBossItem) {
			_acceleration = _MIN_ACCEL;
		}

		_isHolding = false;
		
		if (_sceneRemainingTime > 0) {
			for (unsigned int i = 0; i < _sceneRemainingTime; ++i) {
				_score += 50;
			}
			_sceneRemainingTime = 0;
		}
		
	}
}

void Player::_HandleBonusItems() {
	if (_bonusItems.empty()) {
		return;
	}

	unsigned int shroomCards = 0;
	unsigned int flowerCards = 0;
	unsigned int starCards = 0;

	for (auto& item : _bonusItems) {
		switch (item) {
			case GameObjectType::GAMEOBJECT_TYPE_REDMUSHROOM:
				++shroomCards;
				break;
			case GameObjectType::GAMEOBJECT_TYPE_FLOWER:
				++flowerCards;
				break;
			case GameObjectType::GAMEOBJECT_TYPE_STAR:
				++starCards;
				break;
		}
	}

	const unsigned int MAX_ITEMS = 3;
	if (_bonusItems.size() == MAX_ITEMS) {
		if (shroomCards == MAX_ITEMS) {
			_lives += 2;
		}
		else if (flowerCards == MAX_ITEMS) {
			_lives += 3;
		}
		else if (starCards == MAX_ITEMS) {
			_lives += 5;
		}
		else {
			_lives += 1;
		}
		
		_bonusItems.clear();
	}
}

void Player::_HandleMovementMap() {
	if (abs(_position.x - _lastPos.x) >= _MAX_TRAVEL_DISTANCE || abs(_position.y - _lastPos.y) >= _MAX_TRAVEL_DISTANCE) {
		_velocity = { 0.0f, 0.0f };
		_lastPos = _position;
		if (_mapNodePos.x != 0.0f && _mapNodePos.y != 0.0f) {
			_position = _mapNodePos;
		}
	}
}

void Player::_HandleMovementGame() {
	if (_isOnGround) {
		_gravity = 0.0025f;
	}

	//Variable jump height	
	if (Device::IsKeyDown(DIK_SPACE)) {
		if (_gravity > _MAX_GRAVITY) {
			_gravity -= 0.0005f;
		}
		else if (_gravity <= _MAX_GRAVITY) {
			_gravity = _MAX_GRAVITY;
		}
	}
	else {
		if (_gravity < 0.0025f) {
			_gravity += 0.0005f;
		}
	}

	//Skid
	if (_acceleration < _ACCEL_THRESHOLD && _velocity.x != 0.0f) {
		if (_normal.x == -1) {
			if (Device::IsKeyDown(DIK_RIGHTARROW)) {
				_acceleration = 0.0499f;
			}
		}
		else if (_normal.x == 1) {
			if (Device::IsKeyDown(DIK_LEFTARROW)) {
				_acceleration = 0.0499f;
			}
		}
	}

	if (Device::IsKeyDown(DIK_LEFTARROW)) {
		MoveLeft();
	}
	else if (Device::IsKeyDown(DIK_RIGHTARROW)) {
		MoveRight();
	}
	else {
		//Slippery feel when the player stops
		if (_acceleration <= 0.5f) {
			_velocity.x = 0.0f;
			_acceleration = 0.5f;
		}
		else if (_acceleration > 0.5f) {
			_acceleration -= 0.06f;
		}
	}

	if (Device::IsKeyDown(DIK_LEFTARROW) || Device::IsKeyDown(DIK_RIGHTARROW)) {
		//GOTTA GO FAAAST
		if (Device::IsKeyDown(DIK_LSHIFT) && (_isOnGround || !IsFlying())) {
			if (_acceleration < _MAX_ACCEL) {
				_acceleration += 0.03f;
			}
		}
		else {
			if (_acceleration < _MIN_ACCEL) {
				_acceleration += 0.03f;
			}
			else if (_acceleration > _MIN_ACCEL) {
				_acceleration -= 0.02f;
			}
		}
	}

	//Float a bit longer when flying
	if (_acceleration >= _ACCEL_THRESHOLD) {
		_gravity = 0.0013f;
	}
}

Player::Player() {
	_scale = D3DXVECTOR2(-1.0f, 1.0f);
	_renderPriority = 0;

	_runSpeed = 0.09f;
	_jumpSpeed = 0.327f;
	_bounceSpeed = 0.4f;
	_gravity = 0.0025f;
	_acceleration = 0.5f;

	_health = 1;

	_lives = 3;
	_coins = 0;
	_score = 0;

	_fireballsCount = 0;

	_upVector = 1.0f;

	_heldEntity = nullptr;

	_flyTime = 6000;
	_inPipeTime = 2000;
	_attackTime = 150;
	_fireballCoolDownTime = 2500;
	_invulnerableTime = 1000;

	_bonusItems.reserve(3);
}

Player::~Player() {}

unsigned int Player::GetNextSceneID() const {
	return _nextSceneID;
}

RECTF Player::GetBoundingBox(int index) const {
	return GameObject::GetBoundingBox(_health >= 2 && !isInMap && !_isCrouching);
}

Entity* Player::GetHeldEntity() const {
	return _heldEntity;
}

void Player::GetSceneRemainingTime(unsigned int sceneTime) {
	_sceneRemainingTime = sceneTime;
}

void Player::SetUpVector(float upVector) {
	if (upVector == 0.0f) {
		return;
	}
	
	_upVector = upVector;
}

bool Player::TriggeredStageEnd() const {
	return _triggeredStageEnd;
}

bool Player::WentIntoPipe() const {
	return _wentIntoPipe;
}

bool Player::IsFlying() const {
	return _flyStart != 0;
}

bool Player::IsInPipe() const {
	return _inPipeStart != 0;
}

bool Player::IsAttacking() const {
	return _attackStart != 0;
}

bool Player::IsOnFireballCoolDown() const {
	return _fireballCoolDownStart != 0;
}

bool Player::IsInvulnerable() const {
	return _invulnerableStart != 0;
}

void Player::StartFlyTimer() {
	_flyStart = static_cast<DWORD>(GetTickCount64());
}

void Player::StartInPipeTimer() {
	_inPipeStart = static_cast<DWORD>(GetTickCount64());
}

void Player::StartAttackTimer() {
	_attackStart = static_cast<DWORD>(GetTickCount64());
}

void Player::StartFireballCoolDownTimer() {
	_fireballCoolDownStart = static_cast<DWORD>(GetTickCount64());
}

void Player::StartInvulnerableTimer() {
	_invulnerableStart = static_cast<DWORD>(GetTickCount64());
}

void Player::HandleStates() {
	if (isInMap) {	
		_HandleMovementMap();
	}
	else {
		_HandleMovementGame();
	}
}

void Player::OnKeyUpMap(int keyCode) {
	//Stub
}

void Player::OnKeyUpGame(int keyCode) {
	if (keyCode == DIK_DOWNARROW) {
		_isCrouching = false;

		if (_health > 1 && _isOnGround && !IsInPipe()) {
			_isOnGround = false;
			_position.y = ceil(_position.y - _CROUCH_HEIGHT_ADJUST);
		}
	}
	else if (keyCode == DIK_S) {
		_isHolding = false;
	}
}

void Player::OnKeyDownMap(int keyCode) {
	const float MAP_RUN_SPEED = 0.08f;

	if (keyCode == DIK_UPARROW) {
		_velocity.y = -MAP_RUN_SPEED;
	}
	else if (keyCode == DIK_LEFTARROW) {
		_velocity.x = -MAP_RUN_SPEED;
	}
	else if (keyCode == DIK_DOWNARROW) {
		_velocity.y = MAP_RUN_SPEED;
	}
	else if (keyCode == DIK_RIGHTARROW) {
		_velocity.x = MAP_RUN_SPEED;
	}
}

void Player::OnKeyDownGame(int keyCode) {
	if (keyCode == DIK_LEFTARROW) {
		_normal.x = -1.0f;
	}
	else if (keyCode == DIK_RIGHTARROW) {
		_normal.x = 1.0f;
	}
	else if (keyCode == DIK_DOWNARROW) {
		_isCrouching = true;

		if (_health > 1 && _isOnGround && !IsInPipe()) {
			_position.y = ceil(_position.y + _CROUCH_HEIGHT_ADJUST);
		}
	}
	else if (keyCode == DIK_S) {
		_isHolding = true;
		//Fireball attack
		if (_health == 3 && !_isCrouching) {
			if (_fireballsCount < _FIREBALLS_LIMIT) {
				
				++_fireballsCount;

				if (_fireballsCount == _FIREBALLS_LIMIT) {
					StartFireballCoolDownTimer();
				}
			}
		}

		//Tail attack
		if (_health == 4 && !IsAttacking()) {
			StartAttackTimer();
		}
	}
	else if (keyCode == DIK_SPACE) {
		SlowFall();
		RunFly();
		Jump();
	}
}

void Player::ParseData(
	std::string dataPath, 
	Texture*& texture, 
	std::vector<std::string> extraData) 
{
	if (_playerTexture == nullptr) {
		_playerTexture = texture;
	}
	Entity::ParseData(dataPath, texture, extraData);
}

void Player::TakeDamage() {
	if (!IsInvulnerable()) {
		_originalVel = { _velocity.x, _isOnGround ? _velocity.y : -_bounceSpeed };

		StartInvulnerableTimer();

		if (_health > 2) {
			_health = 2;
		}
		else {
			--_health;

			if (_health < 1) {
				--_lives;
			}
		}
	}
}

void Player::MoveLeft() {
	_scale = D3DXVECTOR2(1.0f, 1.0f);
	_velocity.x = -_runSpeed * _acceleration;

	OutputDebugStringA("[PLAYER] Move left\n");
}

void Player::MoveRight() {
	_scale = D3DXVECTOR2(-1.0f, 1.0f);
	_velocity.x = _runSpeed * _acceleration;

	OutputDebugStringA("[PLAYER] Move right\n");
}

void Player::Jump() {
	if (_isOnGround) {
		_velocity.y = -_jumpSpeed;
		_isOnGround = false;

		OutputDebugStringA("[PLAYER] Jump\n");
	}
}

void Player::RunFly() {
	if (_health == 4) {
		if (_acceleration >= _ACCEL_THRESHOLD || IsFlying()) {
			OutputDebugStringA("[PLAYER] Flying\n");

			if (_isOnGround && !IsFlying()) {
				_isOnGround = false;
				StartFlyTimer();
			}
			
			const float FLOAT_MODIFIER = 0.66f;
			_velocity.y = -_jumpSpeed * FLOAT_MODIFIER;
		}
	}
}

void Player::SlowFall() {
	if (_health == 4 && !_isOnGround) {
		const float SLOW_MODIFIER = 0.2f;
		_velocity.y *= SLOW_MODIFIER;

		OutputDebugStringA("[PLAYER] Falling\n");
	}
}

void Player::HandleCollisionResult(
	LPCOLLISIONEVENT result, 
	D3DXVECTOR2& minTime, 
	D3DXVECTOR2& offset, 
	D3DXVECTOR2& normal, 
	D3DXVECTOR2& relativeDistance) 
{}

void Player::HandleOverlap(Entity* entity) {}

void Player::Update(
	DWORD deltaTime, 
	std::vector<Entity*>* collidableEntities, 
	std::vector<Entity*>* collidableTiles) 
{

	_HandleCurrencies();
	_HanldeStageEnd();
	_HandleBonusItems();

	//----------------------------------------------------------------------------
	//TIMERS
	//----------------------------------------------------------------------------
	if (_health != 4 || (IsFlying() && GetTickCount64() - _flyStart > _flyTime)) {
		_flyStart = 0;
	}

	if (IsInPipe() && GetTickCount64() - _inPipeStart > _inPipeTime) {
		_inPipeStart = 0;
	}

	if (IsAttacking() && GetTickCount64() - _attackStart > _attackTime) {
		_attackStart = 0;
	}

	if (IsOnFireballCoolDown() && GetTickCount64() - _fireballCoolDownStart > _fireballCoolDownTime) {
		_fireballsCount = 0;
		_fireballCoolDownStart = 0;
	}

	if (IsInvulnerable() && GetTickCount64() - _invulnerableStart > _invulnerableTime) {
		_invulnerableStart = 0;

		_velocity = _originalVel;
	}
	//----------------------------------------------------------------------------
	//TIMERS
	//----------------------------------------------------------------------------

	//To show the whole kicking animation
	if (_isNextToShell && GetTickCount64() % 500 == 0) {
		_isNextToShell = false;
	}

	if (_health <= 0 || _isCrouching) {
		_velocity.x = 0.0f;
	}

	if (_triggeredStageEnd && !_hasBossItem) {
		MoveRight();
	}

	if (IsInPipe()) {
		_velocity.y = _health > 1 ? 0.035f : 0.03f;
		_velocity.y *= _normal.y;

		_isHolding = false;
		_velocity.x = 0.0f;

		if (GetTickCount64() - _inPipeStart == _inPipeTime * 0.5f) {
			_wentIntoPipe = !_wentIntoPipe;
			_position = _destination;
			_isOnGround = false;
			_normal.y *= _upVector;
		}
	}

	if (IsInvulnerable()) {
		if (GetTickCount64() - _invulnerableStart >= _invulnerableTime * 0.82f) {
			//Death bounce
			if (_health <= 0) {
				_isOnGround = false;
				_isHolding = false;

				_velocity.y = -_bounceSpeed;
				_gravity = 0.0010f;
			}
		}
		else {
			_velocity = { 0.0f, 0.0f };
		}
	}

	Entity::Update(deltaTime, collidableEntities, collidableTiles);

	if (_heldEntity != nullptr) {
		if (_heldEntity->GetHealth() == 0 || _heldEntity->GetHealth() == 3) {
			_isHolding = false;

			if (_heldEntity->GetHealth() == 3) {
				const float OFFSET_X = 17.0f;
				const float OFFSET_Y = 14.0f;
				_heldEntity->SetPosition(
					{ 
					_position.x + OFFSET_X * _normal.x, 
					_position.y - OFFSET_Y 
					}
				);
			}

			_heldEntity->isBeingHeld = false;
			_heldEntity = nullptr;
			return;
		}
	
		if (_isHolding) {
			D3DXVECTOR2 offset;
			offset.x = IsInPipe() ? 0.0f : 12.0f;
			offset.y = _health == 1 ? 2.0f : -3.0f;

			_heldEntity->SetPosition({ _position.x + offset.x * _normal.x, _position.y - offset.y });
		}
		else {
			_isNextToShell = true;

			_heldEntity->TakeDamage();
			_heldEntity->SetNormal({ -_normal.x, 0.0f });
			_heldEntity->isBeingHeld = false;
			_heldEntity = nullptr;
		}
	}
}

void Player::Render() {
	_animatedSprite.PlaySpriteAnimation("BigRun", _position, _scale);
}

void Player::Release() {
	_animatedSprite.Release();
	_playerTexture = nullptr;
}