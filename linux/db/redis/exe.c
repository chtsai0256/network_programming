#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<hiredis/hiredis.h>
#include"test.h"

/**************************************
input :cmd  redis命令
return:STU_RESULT
*************************************/
STU_RESULT exeRedisIntCmd(redisContext *context, char *cmd)
{
    redisReply *reply = (redisReply *)redisCommand(context, cmd);
    if(NULL == reply)
    {
        printf("%d execute command:%s failure\n",__LINE__,cmd);
        redisFree(context);
        return FAILURE;
     }
    //return type == DIS_REPLY_INTEGER and >0 success
    if(!(reply->type == REDIS_REPLY_INTEGER && reply->integer > 0))
    {
        printf("%d execute command:%s failure\n",__LINE__, cmd);
        freeReplyObject(reply);
        return FAILURE;
    }
    freeReplyObject(reply);
    printf("%d execute command:%s success\n",__LINE__,cmd);
    return SUCCESS;

}

STU_RESULT exeRedisStrCmd(redisContext *context, char *cmd)
{
    redisReply *reply = (redisReply *)redisCommand(context, cmd);
    if(NULL == reply)
    {
        printf("%d execute command:%s failure\n",__LINE__,cmd);
        return FAILURE;
     }
    if(!(reply->type == REDIS_REPLY_ARRAY && reply->elements > 0))
    {
        printf("%d execute command:%s failure\n",__LINE__, cmd);
        freeReplyObject(reply);
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
    return SUCCESS;
}


STU_RESULT exeRedisAuthCmd(redisContext *context, char *cmd)
{

    redisReply *reply = (redisReply *)redisCommand(context, cmd);
    if(NULL == reply)
    {
        printf("%d execute command:%s failure\n",__LINE__,cmd);
        return FAILURE;
     }

    if(reply->type == REDIS_REPLY_ERROR)
    {
        printf("%d execute command:%s failure\n",__LINE__, cmd);
        freeReplyObject(reply);
        return FAILURE;

    }

    freeReplyObject(reply);
    return SUCCESS;
}
