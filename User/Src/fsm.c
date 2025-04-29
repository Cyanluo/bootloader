#include <stdint.h>
#include <fsm.h>
#include <string.h>

static uint32_t fsm_state;
FSM_State_Convert fsm_state_set[MAX_TASK_NUM];

void fsm_init(void)
{
	fsm_state = INIT_STATE;

	for (uint32_t i = 0; i < MAX_TASK_NUM; i++)
	{
		fsm_state_set[i].pre_state = IDLE_STATE;
		fsm_state_set[i].state = IDLE_STATE;
		fsm_state_set[i].r_state = IDLE_STATE;
		fsm_state_set[i].condition = NULL;
		fsm_state_set[i].argc = 0;
		fsm_state_set[i].argv = NULL;
		fsm_state_set[i].action = NULL;
		fsm_state_set[i].r_argc = 0;
		fsm_state_set[i].r_argv = NULL;
		fsm_state_set[i].r_action = NULL;
	}
}

void set_state(const enum FSM_STATE state)
{
	fsm_state = state;
}

enum FSM_STATE get_state(void)
{
	return fsm_state;
}

void fsm_run(void)
{
	for (uint32_t i = 0; i < MAX_TASK_NUM; i++)
	{
		if (fsm_state_set[i].action != NULL && fsm_state_set[i].condition != NULL)
		{
			if ( (fsm_state_set[i].pre_state == IDLE_STATE || fsm_state_set[i].pre_state == fsm_state) )
			{
				if (fsm_state_set[i].condition())
				{
					fsm_state = fsm_state_set[i].state;
					fsm_state_set[i].action(fsm_state_set[i].argc, fsm_state_set[i].argv);
				}
				else if (fsm_state_set[i].r_action != NULL)
				{
					fsm_state = fsm_state_set[i].r_state;
					fsm_state_set[i].r_action(fsm_state_set[i].r_argc, fsm_state_set[i].r_argv);
				}
			}
		}
	}
}

void add_fsm_state(FSM_State_Convert_Ptr fsm_state_convert)
{
	for (uint32_t i = 0; i < MAX_TASK_NUM; i++)
	{
		if (fsm_state_set[i].action == NULL && fsm_state_set[i].condition == NULL)
		{
			memcpy(&fsm_state_set[i], fsm_state_convert, sizeof(FSM_State_Convert));
			break;
		}
	}
}

void nop_action(uint16_t argc, char* argv[])
{

}
