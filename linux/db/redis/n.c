#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<hiredis/hiredis.h>

/*宏定义*/
#define SID_MAX_LENGHT 16
#define NAME_MAX_LENGHT 16
#define MAJOR_MAX_LENGHT 64
#define CMD_MAX_LENGHT  256
#define REDIS_SERVER_IP "134.208.6.61"
#define REDIS_SERVER_PORT 10000
/*结构体定义*/
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

/*函数*/
STU_RESULT addStu(Stu_Info_Struct *stu);/*添加信息*/
/*执行命令*/
STU_RESULT exeRedisIntCmd(char *cmd);
STU_RESULT quryStuBySid(char *sid);
STU_RESULT exeRedisStrCmd(char *cmd);
/**************************************
函数名:addStu
函数功能:添加学生信息
输入参数:stu 学生信息结构指针
输出参数:
返回值:STU_RESULT  成功或失败
************************************/
STU_RESULT addStu(Stu_Info_Struct *stu)
{
    char cmd[CMD_MAX_LENGHT] = {0};
    /*检查入参*/
    if(NULL == stu)
    {
        printf("NULL pointer");
        return FAILURE;
    }
    /*组装redis命令*/
    snprintf(cmd,CMD_MAX_LENGHT,"hset stu:%s name %s gender %d major %s",
            stu->sid,stu->name,stu->gender,stu->major);

    /*执行redis命令*/
    if(FAILURE == exeRedisIntCmd(cmd))
    {
        printf("add student %s,%s,%d,%s failure\n",stu->sid,stu->name,stu->gender,stu->major);
        return FAILURE;
    }
    printf("add student %s,%s,%d,%s success\n",stu->sid,stu->name,stu->gender,stu->major);
    return SUCCESS;
}
/**************************************
函数名:exeRedisIntCmd
函数功能:执行redis 返回值为int类型命令
输入参数:cmd  redis命令
输出参数:redis 返回结构
返回值:STU_RESULT
*************************************/
STU_RESULT exeRedisIntCmd(char *cmd)
{
    /*检查入参*/
    if(NULL == cmd)
    {
        printf("NULL pointer");
        return FAILURE;
    }
    /*连接redis*/
    redisContext *context = redisConnect(REDIS_SERVER_IP,REDIS_SERVER_PORT);
    if(context->err)
    {
        redisFree(context);
        printf("%d connect redis server failure:%s\n",__LINE__, context->errstr);
        return FAILURE;
    }
    printf("connect redis server success\n");

    /*执行redis命令*/
    redisReply *reply = (redisReply *)redisCommand(context, cmd);
    if(NULL == reply)
    {
        printf("%d execute command:%s failure\n",__LINE__,cmd);
        redisFree(context);
        return FAILURE;
     }
    //返回执行结果为整型的命令,只有状态为REDIS_REPLY_INTEGER,并且INTEGER是大于0时,才表示这种类型的命令执行成功
    if(!(reply->type == REDIS_REPLY_INTEGER && reply->integer > 0))
    {
        printf("%d execute command:%s failure\n",__LINE__, cmd);
        freeReplyObject(reply);
        redisFree(context);
        return FAILURE;
    }

    freeReplyObject(reply);
    redisFree(context);
    printf("%d execute command:%s success\n",__LINE__,cmd);
    return SUCCESS;

}
/**************************************
函数名:quryStuBySid
函数功能:通过sid查询学生信息
输入参数:cmd  redis命令
输出参数:redis 返回结构
返回值:STU_RESULT
*************************************/
STU_RESULT queryStuBySid(char *sid)
{
    char cmd[CMD_MAX_LENGHT] = {0};
    /*入参检查*/
    if(NULL == sid)
    {
        printf("%d NULL pointer\n",__LINE__);
        return FAILURE;
    }
    /*组装执行命令*/
    snprintf(cmd,CMD_MAX_LENGHT,"HGETALL stu:%s",sid);
    if(FAILURE == exeRedisStrCmd(cmd))
    {
        printf("%d query stu failue",__LINE__);
        return FAILURE;
    }
    return SUCCESS;
}
STU_RESULT exeRedisStrCmd(char *cmd)
{

    /*检查入参*/
    if(NULL == cmd)
    {
        printf("NULL pointer");
        return FAILURE;
    }

    /*连接redis*/
    redisContext *context = redisConnect(REDIS_SERVER_IP,REDIS_SERVER_PORT);
    if(context->err)
    {
        redisFree(context);
        printf("%d connect redis server failure:%s\n",__LINE__, context->errstr);
        return FAILURE;
    }
    printf("connect redis server success\n");

    /*执行redis命令*/
    redisReply *reply = (redisReply *)redisCommand(context, cmd);
    if(NULL == reply)
    {
        printf("%d execute command:%s failure\n",__LINE__,cmd);
        redisFree(context);
        return FAILURE;
     }
    //返回执行结果为整型的命令,只有状态为REDIS_REPLY_INTEGER,并且INTEGER是大于0时,才表示这种类型的命令执行成功
    if(!(reply->type == REDIS_REPLY_ARRAY && reply->elements > 0))
    {
        printf("%d execute command:%s failure\n",__LINE__, cmd);
        freeReplyObject(reply);
        redisFree(context);
        return FAILURE;

    }

    printf("%d,%lu\n",reply->type,reply->elements);
    int i = 0;
    for(i=0;i < reply->elements;i++)
    {
        if(i%2 ==0)
        {
            printf("%s:",reply->element[i]->str);
        }
        else
        {

            printf("%s\n",reply->element[i]->str);
        }
    }
    freeReplyObject(reply);
    redisFree(context);
    return SUCCESS;
}
int main(int argc,char *argv[])
{
    Stu_Info_Struct stu =
    {
     "01",
     "hu",
    1,
    "CS"
    };

    //addStu(&stu);
    queryStuBySid("01");
    return 0;
}