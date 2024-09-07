#ifndef __TEST_H__
#define __TEST_H__

#define SID_MAX_LENGHT 16
#define NAME_MAX_LENGHT 16
#define MAJOR_MAX_LENGHT 64
#define CMD_MAX_LENGHT  256
#define REDIS_SERVER_IP "134.208.6.61"
#define REDIS_SERVER_PORT 10000

typedef struct Stu_Info_Struct
{
	char sid[SID_MAX_LENGHT];
	char name[NAME_MAX_LENGHT];
	int gender;//0 male,1 female
	char major[MAJOR_MAX_LENGHT];
}Stu_Info_Struct;

typedef enum STU_RESULT
{
    SUCCESS=0,
    FAILURE=1
}STU_RESULT;

STU_RESULT addStu(redisContext *context, Stu_Info_Struct *stu);/*添加信息*/
STU_RESULT exeRedisIntCmd(redisContext *context, char *cmd);
STU_RESULT quryStuBySid(redisContext *context, char *sid);
STU_RESULT exeRedisStrCmd(redisContext *context, char *cmd);
#endif
