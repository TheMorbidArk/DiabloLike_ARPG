#ifndef BATTLE_H
#define BATTLE_H

#include "../../core/config.h"

// 战斗单位结构
typedef struct {
    int max_hp;
    int current_hp;
    int attack;
    int defense;
    int speed;
    char name[16];
} BattleUnit;

// 战斗菜单选项
typedef enum {
    BATTLE_ACTION_ATTACK,
    BATTLE_ACTION_DEFEND,
    BATTLE_ACTION_HEAL,
    BATTLE_ACTION_RUN,
    BATTLE_ACTION_COUNT
} BattleAction;

// 战斗结果
typedef enum {
    BATTLE_RESULT_VICTORY,
    BATTLE_RESULT_DEFEAT,
    BATTLE_RESULT_ESCAPE,
    BATTLE_RESULT_NONE
} BattleResult;

// 初始化战斗系统
void battle_init(int enemy_type);

// 清理战斗系统
void battle_cleanup();

// 战斗更新（每帧调用）
void battle_update();

// 战斗渲染
void battle_render();

// 检查战斗是否结束
int battle_is_over();

// 获取战斗结果
BattleResult battle_get_result();

// 获取玩家单位
BattleUnit* battle_get_player();

// 获取敌人单位
BattleUnit* battle_get_enemy();

// 执行玩家行动
void battle_execute_player_action(BattleAction action);

// 设置战斗菜单选择
void battle_set_menu_selection(int selection);

// 获取当前菜单选择
int battle_get_menu_selection();

// 获取当前战斗状态
BattleState battle_get_state();

// 处理战斗结果（返回探索场景）
void battle_end();

#endif