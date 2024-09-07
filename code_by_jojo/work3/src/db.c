 #include "../include/defs.h"

/*--------------- <db operation> ---------------*/
redisContext* dbConnect()
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if(c == NULL || c->err)
    {
        if(c)
            err_sys("Redis error: %s", c->errstr);
        else
            err_sys("Can't allocate redis context");
    }
    return c;
}

char* dbList(redisContext *c, char *list)
{
    redisReply *reply;
    char *buf = (char*)malloc(sizeof(char) * MLEN);
    int offset = 0;
    int i = 0;

    memset(buf, '\0', MLEN);
    reply = redisCommand(c, "lrange %s 0 -1", list);

    if(reply->elements != 0 && reply->type == REDIS_REPLY_ARRAY)
    {
        for(; i<reply->elements; i++)
        {
            sprintf((i==0) ? buf : buf + offset, "%s\0", reply->element[i]->str);
            offset+=(strlen((char*)reply->element[i]->str) + 1);
        }
    }else{
        free(buf);
        freeReplyObject(reply);
        return NULL;
    }

    freeReplyObject(reply);
    return buf;
}

void dbDelete(redisContext *c, char *key)
{
    redisReply *reply;
    reply = redisCommand(c, "del %s", key);
    freeReplyObject(reply);
}

/*
    create a key:value pair, then set value to the key
*/
void dbSet(redisContext *c, char *key, char *value)
{
    redisReply *reply;
    reply = redisCommand(c, "SET %s %s", key, value);

    if(reply->type == REDIS_REPLY_INTEGER && reply->integer == 0)
        printf("Key not found.\n");
     
    freeReplyObject(reply);
}

/*
    push value to the list.
*/
void dbPushToList(redisContext *c, char *listKey, char *vKeys)
{
    redisReply *reply;
    reply = redisCommand(c, "lpush %s %s", listKey, vKeys);
    freeReplyObject(reply);
}

/*--------------- <login page> ---------------*/
int checkUser(redisContext *dbContext, char *str, char **user, int skip)
{
    redisReply *reply;
    char *userList = dbList(dbContext, "user_list");

    for(int i=0; userList[i] != '\0'; i+=(strlen(userList + i) + 1))
    {
        if(!strcmp(str, &userList[i]))
        {
            if(user)
            {
                char *tmp = (char*)malloc(sizeof(char) * strlen(&userList[i]));
                strcpy(tmp, &userList[i]);
                *user = tmp;
            }

            if(skip)
            {
                free(userList);
                freeReplyObject(reply);
                return USEREXIST;
            }

            reply = redisCommand(dbContext, "lrange %s 0 -1", &userList[i]);
            if( reply->type == REDIS_REPLY_ARRAY &&
                !strcmp(str + strlen(str) + 1, (char*)reply->element[2]->str))
            {
                free(userList);
                freeReplyObject(reply);
                return USEREXIST;
            }
            free(userList);
            freeReplyObject(reply);
            return PASSWORDERR;
        }
    }

    free(userList);
    freeReplyObject(reply);
    return USERUNEXIST;
}

void createUser(redisContext *dbContext, char *buf)
{
    redisReply *reply;
    int userCount;
    char mailbox[STR100] = {'\0'};
    char grouplist[STR100] = {'\0'};

    sprintf(mailbox, "%s_mailbox", buf);
    sprintf(grouplist, "%s_group_list", buf);

    reply = redisCommand(dbContext, "get user_count");
    userCount = atoi(reply->str);
    freeReplyObject(reply);

    reply = redisCommand(dbContext, "incr user_count");
    freeReplyObject(reply);

    reply = redisCommand(dbContext, "lpush user_list %s", buf);
    freeReplyObject(reply);

    reply = redisCommand(dbContext, "lpush %s %d %s %s %s", 
                                                buf,
                                                userCount,
                                                buf + strlen(buf) + 1,
                                                mailbox,
                                                grouplist);
    freeReplyObject(reply);
}

void inputLoginInfo(int fd, char *a, char *b, char *buf)
{
    send(fd, a, strlen(a), 0);
    recv(fd, buf, MLEN, 0);
    eliReturnChar(buf);

    send(fd, b, strlen(b), 0);
    recv(fd, buf + strlen(buf) + 1, MLEN, 0);
    eliReturnChar(buf + strlen(buf) + 1);
}

void registerUser(redisContext *c, int clientfd)
{
    char choose[5];             // set for new line
    char buf[MLEN] = {'\0'};

    send(clientfd, "User not found !\n", strlen("User not found !\n"), 0);
    send(clientfd, "Create account or login again ? <1/2> : ", strlen("Create account or login again ? <1/2> : "), 0);
    recv(clientfd, &choose, 5, 0);

    if(choose[0] != '1')
        return;

    while(1)
    {
        inputLoginInfo(clientfd, "your user name: ", "your password: ", buf);
        if(checkUser(c, buf, NULL, 1) == USEREXIST)
            send(clientfd, "User name already exist !\n", strlen("User name already exist !\n"), 0);
        else
        {
            createUser(c, buf);
            send(clientfd, "Create success !\n", strlen("Create success !\n"), 0);
            break;
        }
    }
}

char* login(int clientfd, int serrfd, control *pack)
{
    redisContext *c = dbConnect();
    char buf[MLEN] = {'\0'};
    char *user;
    int len, pass = 1;

    while(pass)
    {
        memset(buf, '\0', MLEN);
        inputLoginInfo(clientfd, "user_name: ", "password: ", buf);

        switch (checkUser(c, buf, &user, 0))
        {
        case USERUNEXIST:
            registerUser(c, clientfd);
            break;
        case PASSWORDERR:
            free(user);
            send(clientfd, "Password error !\n", strlen("Password error !\n"), 0);
            break;
        default:
            send(clientfd, "\033[H\033[J", strlen("\033[H\033[J"), 0);
            pass = 0;
            break;
        }
    }
    
    redisFree(c);
    len = strlen(user);

    _write(serrfd, &(pack->upOption), 4);
    _write(serrfd, &(pack->isLogin), 4);
    _write(serrfd, &(pack->len), 4);         
    _write(serrfd, pack->ipp, pack->len);
    _write(serrfd, &(pack->pid), 4);
    _write(serrfd, (void*)&len, 4);
    _write(serrfd, (void*)user, len);
    _close(serrfd);
    return user;
}