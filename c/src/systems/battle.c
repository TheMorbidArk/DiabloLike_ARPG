#include "battle.h"
#include "../tic80.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 战斗单位
static BattleUnit player_unit = {100, 100, 10, 5, 10, "Player"};
static BattleUnit enemy_unit = {50, 50, 8, 3, 8, "Enemy"};

// 战斗状态
static BattleState battle_state = BATTLE_MENU_SELECT;
static BattleResult battle_result = BATTLE_RESULT_NONE;
static int menu_selection = 0;
static int animation_timer = 0;
static int battle_message_timer = 0;
static char battle_message[64] = {0};

// 敌人类型
static int current_enemy_type = 0;

// 敌人行动完成标志
static int enemy_action_completed = 0;
// 当前动画来源（0-玩家行动，1-敌人行动）
static int animation_source = 0;

// UI绘制辅助函数
static void draw_hp_bar(int x, int y, int current, int max, int width, int color) {
    float filled_f = (float)current / max * width;
    int filled = (int)filled_f;
    rectb(x, y, width + 4, 14, COLOR_WHITE);
    rect(x + 2, y + 2, (int8_t)filled, 10, (int8_t)color);

    // 绘制HP数值
    char hp_text[16];
    sprintf(hp_text, "%d/%d", current, max);
    print(hp_text, x + width / 2 - 16, y + 15, COLOR_WHITE, 0, 1, 0);
}

static void draw_battle_ui() {
    // 绘制玩家信息
    print("PLAYER", 10, 10, COLOR_WHITE, 0, 1, 0);
    draw_hp_bar(10, 25, player_unit.current_hp, player_unit.max_hp, 80, COLOR_GREEN);

    // 绘制敌人信息
    print(enemy_unit.name, WIDTH - 90, 10, COLOR_WHITE, 0, 1, 0);
    draw_hp_bar(WIDTH - 90, 25, enemy_unit.current_hp, enemy_unit.max_hp, 80, COLOR_RED);

    // 绘制战斗信息
    if (battle_message_timer > 0) {
        rectb(5, HEIGHT - 25, WIDTH - 10, 20, COLOR_WHITE);
        print(battle_message, WIDTH / 2 - 64, HEIGHT - 20, COLOR_WHITE, 0, 1, 0);
    }
}

static void draw_battle_menu() {
    const char* menu_items[] = {
        "ATTACK",
        "DEFEND",
        "HEAL",
        "RUN"
    };

    int start_y = BATTLE_MENU_Y;

    for (int i = 0; i < BATTLE_ACTION_COUNT; i++) {
        int y = start_y + i * BATTLE_MENU_ITEM_H;

        // 绘制菜单选项
        if (i == menu_selection) {
            rect(BATTLE_MENU_X - 4, y - 2, 100, BATTLE_MENU_ITEM_H - 2, COLOR_LIGHT_BLUE);
            print(">", BATTLE_MENU_X + 2, y + 1, COLOR_WHITE, 0, 1, 0);
            print(menu_items[i], BATTLE_MENU_X + 12, y + 1, COLOR_WHITE, 0, 1, 0);
        } else {
            print(menu_items[i], BATTLE_MENU_X + 12, y + 1, COLOR_GREY, 0, 1, 0);
        }
    }
}

static void draw_enemy_sprite() {
    // 简单绘制敌人精灵（红色球）
    int enemy_x = WIDTH / 2 - 8;
    int enemy_y = 50;

    // 绘制敌人
    circ(enemy_x + 8, enemy_y + 8, 7, COLOR_RED);
    circ(enemy_x + 6, enemy_y + 6, 2, COLOR_WHITE); // 高光
    circ(enemy_x + 10, enemy_y + 10, 2, COLOR_DARK_GREY); // 阴影

    // 如果受伤，闪烁效果
    if (animation_timer > 0 && animation_timer % 10 < 5) {
        circ(enemy_x + 8, enemy_y + 8, 7, COLOR_WHITE);
    }
}

static void calculate_damage(int attacker_atk, int defender_def, int* min_dmg, int* max_dmg) {
    *min_dmg = (attacker_atk - defender_def / 2) / 2;
    *max_dmg = (attacker_atk - defender_def / 2);

    if (*min_dmg < 1) *min_dmg = 1;
    if (*max_dmg < 2) *max_dmg = 2;
}

void battle_init(int enemy_type) {
    current_enemy_type = enemy_type;
    battle_state = BATTLE_MENU_SELECT;
    battle_result = BATTLE_RESULT_NONE;
    menu_selection = 0;
    animation_timer = 0;
    battle_message_timer = 0;

    // 重置单位状态
    player_unit.current_hp = player_unit.max_hp;
    enemy_action_completed = 0;
    animation_source = 0;

    // 根据敌人类型设置敌人属性
    switch(enemy_type) {
        case 0:
            enemy_unit.max_hp = BATTLE_ENEMY_HP;
            enemy_unit.current_hp = BATTLE_ENEMY_HP;
            enemy_unit.attack = BATTLE_ENEMY_ATK;
            enemy_unit.defense = 3;
            strcpy(enemy_unit.name, "Goblin");
            break;
        case 1:
            enemy_unit.max_hp = 70;
            enemy_unit.current_hp = 70;
            enemy_unit.attack = 12;
            enemy_unit.defense = 5;
            strcpy(enemy_unit.name, "Orc");
            break;
        default:
            enemy_unit.max_hp = BATTLE_ENEMY_HP;
            enemy_unit.current_hp = BATTLE_ENEMY_HP;
            enemy_unit.attack = BATTLE_ENEMY_ATK;
            enemy_unit.defense = 3;
            strcpy(enemy_unit.name, "Enemy");
            break;
    }

    // 初始消息
    sprintf(battle_message, "A %s appears!", enemy_unit.name);
    battle_message_timer = 60;
}

void battle_cleanup() {
    // 清理战斗资源
    battle_message_timer = 0;
    animation_timer = 0;
}

void battle_update() {
    // Debug: 显示当前战斗状态
    static BattleState last_state = BATTLE_MENU_SELECT;
    if (battle_state != last_state) {
        char state_str[64];
        sprintf(state_str, "Battle state changed: %d -> %d", last_state, battle_state);
        trace(state_str, COLOR_RED);
        last_state = battle_state;
    }

    // 更新计时器
    if (battle_message_timer > 0) {
        battle_message_timer--;
    }

    if (animation_timer > 0) {
        animation_timer--;
        if (animation_timer == 0) {
            // Debug: 动画结束
            char anim_str[64];
            sprintf(anim_str, "Animation ended, battle_state: %d", battle_state);
            trace(anim_str, COLOR_YELLOW);

            // 动画结束，根据状态进行下一步
            if (battle_state == BATTLE_ANIMATION) {
                if (player_unit.current_hp <= 0) {
                    battle_result = BATTLE_RESULT_DEFEAT;
                    battle_state = BATTLE_RESULT;
                    sprintf(battle_message, "You were defeated!");
                    battle_message_timer = 60;
                } else if (enemy_unit.current_hp <= 0) {
                    battle_result = BATTLE_RESULT_VICTORY;
                    battle_state = BATTLE_RESULT;
                    sprintf(battle_message, "Victory! You won!");
                    battle_message_timer = 60;
                } else {
                    // 检查动画来源
                    if (animation_source == 1) {
                        // 敌人行动后的动画，回到玩家选择
                        battle_state = BATTLE_MENU_SELECT;
                    } else {
                        // 玩家行动后的动画，进入敌人回合
                        battle_state = BATTLE_ENEMY_TURN;
                    }
                }
            }
            return;
        }
    }

    switch (battle_state) {
        case BATTLE_MENU_SELECT:
            // 处理菜单输入
            if (btnp(0, 30, 30)) { // 上
                menu_selection = (menu_selection - 1 + BATTLE_ACTION_COUNT) % BATTLE_ACTION_COUNT;
            }
            if (btnp(1, 30, 30)) { // 下
                menu_selection = (menu_selection + 1) % BATTLE_ACTION_COUNT;
            }
            if (btnp(5, 30, 30)) { // 确认
                battle_execute_player_action((BattleAction)menu_selection);
            }
            break;

        case BATTLE_ENEMY_TURN:
            // Debug: 敌人回合信息
            static int last_anim_timer = 0;
            if (last_anim_timer != animation_timer) {
                char enemy_str[64];
                sprintf(enemy_str, "Enemy turn: timer=%d", animation_timer);
                trace(enemy_str, COLOR_CYAN);
                last_anim_timer = animation_timer;
            }

            // 重置敌人行动完成标志（每次进入敌人回合时重置）
            enemy_action_completed = 0;

            // 敌人AI：只在动画计时器为0时执行
            if (animation_timer == 0) {
                // Debug: 执行敌人行动
                char action_str[64];
                sprintf(action_str, "Executing enemy action");
                trace(action_str, COLOR_CYAN);

                // 重置动画计时器
                animation_timer = 30;

                if (enemy_unit.current_hp <= enemy_unit.max_hp * 0.3 && rand() % 4 == 0) {
                    // 低血量时有概率治疗
                    int heal_amount = rand() % 10 + 5;
                    enemy_unit.current_hp = (enemy_unit.current_hp + heal_amount > enemy_unit.max_hp) ?
                                          enemy_unit.max_hp : enemy_unit.current_hp + heal_amount;
                    sprintf(battle_message, "%s recovers %d HP!", enemy_unit.name, heal_amount);
                } else {
                    // 攻击玩家
                    int min_dmg, max_dmg;
                    calculate_damage(enemy_unit.attack, player_unit.defense, &min_dmg, &max_dmg);
                    int damage = rand() % (max_dmg - min_dmg + 1) + min_dmg;
                    player_unit.current_hp -= damage;
                    sprintf(battle_message, "%s attacks! You take %d damage!", enemy_unit.name, damage);
                }

                // 切换到动画状态
                battle_state = BATTLE_ANIMATION;
                battle_message_timer = 60;
                animation_source = 1; // 标记为敌人行动后的动画

                // 不需要标记敌人行动已完成，因为每次进入敌人回合都会重置
            }
            break;

        case BATTLE_ANIMATION:
            // 等待动画完成
            break;

        case BATTLE_RESULT:
            // 等待玩家按键继续
            if (btnp(5, 30, 30)) {
                battle_end();
            }
            break;

        default:
            break;
    }
}

void battle_render() {
    cls(COLOR_BLACK);

    // Debug: 显示当前战斗状态
    char debug_str[64];
    sprintf(debug_str, "STATE:%d TIMER:%d MSG:%d", battle_state, animation_timer, battle_message_timer);
    print(debug_str, 2, 2, COLOR_WHITE, 0, 1, 0);

    // Debug: 显示双方HP
    sprintf(debug_str, "P:%d/%d E:%d/%d", player_unit.current_hp, player_unit.max_hp, enemy_unit.current_hp, enemy_unit.max_hp);
    print(debug_str, 2, 12, COLOR_WHITE, 0, 1, 0);

    // 绘制战斗背景
    rect(0, 0, WIDTH, HEIGHT, COLOR_DARK_BLUE);

    // 绘制敌人
    draw_enemy_sprite();

    // 绘制UI
    draw_battle_ui();

    // 绘制菜单（仅在菜单选择状态）
    if (battle_state == BATTLE_MENU_SELECT) {
        draw_battle_menu();
    }

    // 绘制战斗结果
    if (battle_state == BATTLE_RESULT) {
        const char* result_text = (battle_result == BATTLE_RESULT_VICTORY) ? "VICTORY!" : "DEFEATED!";
        int result_color = (battle_result == BATTLE_RESULT_VICTORY) ? COLOR_YELLOW : COLOR_RED;

        // 绘制结果框
        rectb(WIDTH / 2 - 60, HEIGHT / 2 - 30, 120, 60, COLOR_WHITE);
        rect(WIDTH / 2 - 58, HEIGHT / 2 - 28, 116, 56, COLOR_BLACK);
        print(result_text, WIDTH / 2 - 24, HEIGHT / 2 - 10, (int8_t)result_color, 0, 2, 0);
        print("Press A to continue", WIDTH / 2 - 52, HEIGHT / 2 + 10, COLOR_WHITE, 0, 1, 0);
    }
}

int battle_is_over() {
    return battle_state == BATTLE_RESULT && battle_message_timer == 0;
}

BattleResult battle_get_result() {
    return battle_result;
}

BattleUnit* battle_get_player() {
    return &player_unit;
}

BattleUnit* battle_get_enemy() {
    return &enemy_unit;
}

void battle_execute_player_action(BattleAction action) {
    int min_dmg, max_dmg;
    
    // 标记为玩家行动
    animation_source = 0;

    switch (action) {
        case BATTLE_ACTION_ATTACK:
            calculate_damage(player_unit.attack, enemy_unit.defense, &min_dmg, &max_dmg);
            int damage = rand() % (max_dmg - min_dmg + 1) + min_dmg;
            enemy_unit.current_hp -= damage;
            sprintf(battle_message, "You attack! %d damage!", damage);
            animation_timer = 30;
            battle_state = BATTLE_ANIMATION;
            break;

        case BATTLE_ACTION_DEFEND:
            // 防御：本回合受到伤害减半，下次攻击力提升
            sprintf(battle_message, "You defend your position!");
            battle_message_timer = 30;
            animation_timer = 30;
            battle_state = BATTLE_ANIMATION;
            break;

        case BATTLE_ACTION_HEAL:
            {
                int heal_amount = rand() % 15 + 10;
                player_unit.current_hp = (player_unit.current_hp + heal_amount > player_unit.max_hp) ?
                                       player_unit.max_hp : player_unit.current_hp + heal_amount;
                sprintf(battle_message, "You recover %d HP!", heal_amount);
                battle_message_timer = 30;
                animation_timer = 30;
                battle_state = BATTLE_ANIMATION;
            }
            break;

        case BATTLE_ACTION_RUN:
            // 逃跑成功率
            if (rand() % 100 < 50) {
                battle_result = BATTLE_RESULT_ESCAPE;
                sprintf(battle_message, "You escaped safely!");
                battle_state = BATTLE_RESULT;
            } else {
                sprintf(battle_message, "Failed to escape!");
                battle_state = BATTLE_ENEMY_TURN;
            }
            battle_message_timer = 30;
            break;

        default:
            break;
    }
}

void battle_set_menu_selection(int selection) {
    if (selection >= 0 && selection < BATTLE_ACTION_COUNT) {
        menu_selection = selection;
    }
}

int battle_get_menu_selection() {
    return menu_selection;
}

BattleState battle_get_state() {
    return battle_state;
}

void battle_end() {
    // 战斗结束，状态会在main.c中处理
    battle_cleanup();
}
