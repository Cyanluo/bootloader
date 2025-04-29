#ifndef FSM_H
#define FSM_H

/*
 * 设计需求
 * 1. 方便添加新的状态和对应执行的任务
 * 2. 根据当前状态自动执行相应任务
 * 3. 自动检测状态切换条件并设置对应状态
 */

#define MAX_TASK_NUM 16

enum FSM_STATE
{
  	IDLE_STATE,
	INIT_STATE,
	OTA_DOWNLOADING,
	ISP_DOWNLOADING,
	ISP_FINISH,
	FSM_STATE_NUM
};

typedef uint8_t(*fsm_condition_fn)(void);
typedef void(*fsm_action_fn)(uint16_t argc, char* argv[]);

typedef struct
{
	enum FSM_STATE pre_state; // 如果 pre_state == IDLE_STATE, 表示对当前状态没有要求
	fsm_condition_fn condition;
	enum FSM_STATE state; // 当 condition 返回 true 且当前状态等于 pre_state 时进入的状态
	uint16_t argc;
	char** argv;
	fsm_action_fn action;
	enum FSM_STATE r_state;
	uint16_t r_argc;
	char** r_argv;
	fsm_action_fn r_action;
} FSM_State_Convert, *FSM_State_Convert_Ptr;

void fsm_init(void);
enum FSM_STATE get_state(void);
void set_state(const enum FSM_STATE state);
void fsm_run(void);
void add_fsm_state(FSM_State_Convert_Ptr fsm_state_convert);
void nop_action(uint16_t argc, char* argv[]);

#endif //FSM_H
