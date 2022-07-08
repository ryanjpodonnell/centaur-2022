#include <Arduino.h>
#include "AttractState.h"
#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "BaseMode.h"
#include "BonusLightShow.h"
#include "CountdownBonusState.h"
#include "Debug.h"
#include "Display.h"
#include "GameMode.h"
#include "HertzLogging.h"
#include "Lamps.h"
#include "OrbMode1.h"
#include "OrbMode2.h"
#include "OrbMode3.h"
#include "OrbMode4.h"
#include "PinballMachineBase.h"
#include "RestartGame.h"
#include "SelfTestAndAudit.h"
#include "SkillShotMode.h"
#include "Sound.h"
#include "UnstructuredPlay.h"

#include "PlayerState.h"
#include "MachineState.h"

#ifndef SHARED_VARIABLES_H
#define SHARED_VARIABLES_H
#define DEBUG_MESSAGES 1

extern Attract          g_attract;
extern Base             g_base;
extern BonusLightShow   g_bonusLightShow;
extern CountdownBonus   g_countdownBonus;
extern Debug            g_debug;
extern DisplayHelper    g_displayHelper;
extern GameMode         g_gameMode;
extern HertzLogging     g_hertzLogging;
extern LampsHelper      g_lampsHelper;
extern MachineState     g_machineState;
extern OrbMode1         g_orbMode1;
extern OrbMode2         g_orbMode2;
extern OrbMode3         g_orbMode3;
extern OrbMode4         g_orbMode4;
extern RestartGame      g_restartGame;
extern SelfTestAndAudit g_selfTestAndAudit;
extern SkillShot        g_skillShot;
extern SoundHelper      g_soundHelper;
extern UnstructuredPlay g_unstructuredPlay;

#endif
