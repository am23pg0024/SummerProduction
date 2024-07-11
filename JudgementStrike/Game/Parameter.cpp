#include "UnlimitedLib/UnlimitedLib.h"
#include "Parameter.h"
#include "json.hpp"
#include "Player.h"
#include "GameManager.h"
#include <fstream>

using json = nlohmann::json;

static PARAM param;

const PARAM* GetParameter() { return &param; }

void SaveParam()
{
}

void LoadParam()
{
	// JSONファイル読み込み
	std::ifstream ifs("Data/parameter.json");
	json params;
	ifs >> params;

	// 各種パラメータ読み込み
	param.timeLimit = params["timeLimit"];

	param.player.speed = params["PLAYER"]["speed"];
	param.player.speed2 = params["PLAYER"]["speed2"];
	param.player.speed3 = params["PLAYER"]["speed3"];
	param.player.CHARGE_CHECK_TIME = params["PLAYER"]["CHARGE_CHECK_TIME"];
	param.player.CHARGING_TIME = params["PLAYER"]["CHARGING_TIME"];
	param.player.MAX_INVINCIBLE_TIME = params["PLAYER"]["MAX_INVINCIBLE_TIME"];
	param.player.MAX_COOL_TIME = params["PLAYER"]["MAX_COOL_TIME"];
	param.player.AVOID_DIST = params["PLAYER"]["AVOID_DIST"];

	param.bullet.speed = params["BULLET"]["speed"];

	param.shieldRecovery.startTime = params["SHIELD_RECOVERY"]["startTime"];
	param.shieldRecovery.interval = params["SHIELD_RECOVERY"]["interval"];
	param.shieldRecovery.num = params["SHIELD_RECOVERY"]["num"];
	
	json::iterator itr = params["ENERGY_SYSTEM"].begin();
	for (int i = 0; i < 5; i++) {
		param.energyParams[i].maxGauge = itr.value()["maxGauge"];
		param.energyParams[i].swordPower = itr.value()["swordPower"];
		param.energyParams[i].swordLength = itr.value()["swordLength"];
		param.energyParams[i].swordAngle = itr.value()["swordAngle"];
		param.energyParams[i].gunPower = itr.value()["gunPower"];
		param.energyParams[i].gunScale = itr.value()["gunScale"];
		param.energyParams[i].cGunPower = itr.value()["cGunPower"];
		itr++;
	}

	param.enemy.params[0].hp = params["ENEMY"]["NORMAL"]["hp"];
	param.enemy.params[0].speed = params["ENEMY"]["NORMAL"]["speed"];
	param.enemy.params[0].power = params["ENEMY"]["NORMAL"]["power"];
	param.enemy.params[0].hitPower = params["ENEMY"]["NORMAL"]["hitPower"];

	param.enemy.params[1].hp = params["ENEMY"]["HIGH"]["hp"];
	param.enemy.params[1].speed = params["ENEMY"]["HIGH"]["speed"];
	param.enemy.params[1].power = params["ENEMY"]["HIGH"]["power"];
	param.enemy.params[1].hitPower = params["ENEMY"]["HIGH"]["hitPower"];

	param.enemy.params[2].hp = params["ENEMY"]["EXPLOSION"]["hp"];
	param.enemy.params[2].speed = params["ENEMY"]["EXPLOSION"]["speed"];
	param.enemy.params[2].power = params["ENEMY"]["EXPLOSION"]["power"];
	param.enemy.params[2].hitPower = params["ENEMY"]["EXPLOSION"]["hitPower"];

	param.enemy.maxSpawn = params["ENEMY"]["maxSpawn"];
	param.enemy.maxBlinkTime = params["ENEMY"]["maxBlinkTime"];

	param.explosion.power = params["EXPLOSION"]["power"];
	param.explosion.scale = params["EXPLOSION"]["scale"];

	param.wave.spawnTime = params["WAVE"]["spawnTime"];

	itr = params["WAVE"].begin();
	for (int i = 0; i < 4; i++) {
		param.wave.params[i].spawnNum = itr.value()["spawnNum"];
		param.wave.params[i].border = itr.value()["border"];
		param.wave.params[i].highRate = itr.value()["highRate"];
		param.wave.params[i].explRate = itr.value()["explRate"];
		itr++;
	}
}
