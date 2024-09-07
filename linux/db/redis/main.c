#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<hiredis/hiredis.h>
#include "test.h"


STU_RESULT addStu(redisContext *context, Stu_Info_Struct *stu)
{
    char cmd[CMD_MAX_LENGHT] = {0};
    if(NULL == stu)
    {
        printf("NULL pointer");
        return FAILURE;
    }
    snprintf(cmd,CMD_MAX_LENGHT,"hset stu:%s name %s gender %d major %s",
            stu->sid,stu->name,stu->gender,stu->major);

    if(FAILURE == exeRedisIntCmd(context, cmd))
    {
        printf("add student %s,%s,%d,%s failure\n",stu->sid,stu->name,stu->gender,stu->major);
        return FAILURE;
    }
    printf("add student %s,%s,%d,%s success\n",stu->sid,stu->name,stu->gender,stu->major);
    return SUCCESS;
}
STU_RESULT queryStuBySid(redisContext *context, char *sid)
{
    char cmd[CMD_MAX_LENGHT] = {0};
    if(NULL == sid)
    {
        printf("%d NULL pointer\n",__LINE__);
        return FAILURE;
    }
    snprintf(cmd,CMD_MAX_LENGHT,"HGETALL stu:%s",sid);
    if(FAILURE == exeRedisStrCmd(context, cmd))
    {
        printf("%d query stu failue",__LINE__);
        return FAILURE;
    }
    return SUCCESS;
}


redisContext *connectDB(char *password) {
    redisContext *context = redisConnect(REDIS_SERVER_IP,REDIS_SERVER_PORT);
    if(context->err)
    {
        redisFree(context);
        printf("%d connect redis server failure:%s\n",__LINE__, context->errstr);
        return NULL;
    }
    return context;
}

int main(int argc,char *argv[])
{
    Stu_Info_Struct stu =
    {
     "02",
     "tsai",
    2,
    "MATH"
    };

    redisContext *context = connectDB(argv[1]);

    if (context!=NULL) {
    	//addStu(context, &stu);
    	queryStuBySid(context, "02");
        redisFree(context);
    }
    return 0;
}

