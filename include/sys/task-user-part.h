#pragma once
#include "../kernel/shared.h"
#include "../kernel/task/user-part"

static const char         *task_command_line = (const char *)TASK_CMDLINE_ADDR;
static const TaskUserPart *task_user_part    = (TaskUserPart *)TASK_USER_PART_ADDR;
