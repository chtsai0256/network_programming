/*
    init some data will use in database.
*/

#include "../include/defs.h"

redisContext* dbConnect()
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if(c == NULL || c->err)
        return NULL;
    return c;
}

int main()
{
    redisContext *c = dbConnect();
    redisReply *reply;
    int x;
    reply = redisCommand(c, "set user_count %d", 0);
    freeReplyObject(reply);

    reply = redisCommand(c, "set mail_count %d", 0);
    freeReplyObject(reply);

    redisFree(c);
    return 0;
}