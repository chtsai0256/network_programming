#include "../include/defs.h"

int isUserOnline(int id)
{
    for(int i=0; i<USERTABLESIZE; i++)
        if( cuTable.table[i] &&
            cuTable.table[i]->id == id)
            return i;
    return 0;
}

void sendToClient(int pid, int mLen, char *message) // !!!error
{
    char clientFifo[CLIENT_FIFI_LEM] = {'\0'};
    snprintf(clientFifo, CLIENT_FIFI_LEM, CLIENT_FIFO_TMP, (long)pid);
 
    int cfd = open(clientFifo, O_WRONLY);

    _write(cfd, &mLen, 4);
    _write(cfd, message, mLen);
    _close(cfd);
}

/*-------------------- <base command> --------------------*/
void who(int pid)
{
    char buf[MAXLEN] = {'\0'};
    sprintf(buf, "%s", "<ID>    <name>    <IP:port>          <indicate me>\n");
    for(int i=0; i<USERTABLESIZE; i++)
        if(cuTable.table[i])
            sprintf((buf + strlen(buf)), "%4d    %-10s%-15s    %s\n",
                            cuTable.table[i]->id,
                            cuTable.table[i]->name,
                            cuTable.table[i]->ipp,
                            (cuTable.table[i]->pid == pid) ? "<-(me)" : "");

    sendToClient(pid, strlen(buf), buf);
    sendToClient(pid, 0, NULL);
}

void tell(int pid, char *args)
{
    char message[MLEN] = {'\0'};
    int userid = 0;
    int i, base, index;

    for(i=0, base=1; args[i] != '\0'; i++, base*=10)
    {
        userid *= base;
        userid = ((args[i] - '0') + userid);
    }

    for(index=0; index<USERTABLESIZE; index++)
        if(cuTable.table[index] && cuTable.table[index]->pid == pid)
            break;

    if(isUserOnline(userid))
    {
        for(i=0; i<USERTABLESIZE; i++)
            if(cuTable.table[i] && cuTable.table[i]->id == userid)
                break;

        sprintf(message, "<user(%d) told you>: ", cuTable.table[index]->id);
        sprintf(message + strlen(message), "%s\n", &args[i+1]);
        sendToClient(cuTable.table[i]->pid, strlen(message), message);
        sendToClient(cuTable.table[i]->pid, 0, NULL);
        sendToClient(pid, strlen("send accept!\n"), "send accept!\n");
    }else
    {
        sprintf(message, "%s", "Error: user does not exist yet.\n");
        sendToClient(pid, strlen(message), message);
    }
    sendToClient(pid, 0, NULL);
}

void yell(int pid, char *args)
{
    char message[MLEN] = {'\0'};
    int index;

    for(index=0; index<USERTABLESIZE; index++)
        if(cuTable.table[index] && cuTable.table[index]->pid == pid)
            break;

    sprintf(message, "<user(%d) yelled>: ", cuTable.table[index]->id);
    sprintf(message + strlen(message), "%s\n", args);
    
    for(int i=0; i<USERTABLESIZE; i++)
    {
        if(cuTable.table[i])
        {   
            sendToClient(cuTable.table[i]->pid, strlen(message), message);
            sendToClient(cuTable.table[i]->pid, 0, NULL);
        }
    }
}

char* getName(int pid)
{
    int index = 0;

    for(; index < USERTABLESIZE; index++)
        if(cuTable.table[index] && cuTable.table[index]->pid == pid)
            break;

    char *tmp = (char*)malloc(sizeof(char) * strlen(cuTable.table[index]->name));
    strcpy(tmp, cuTable.table[index]->name);

    return tmp;
}

char* getDate()
{
    char *tmp = (char*)malloc(sizeof(char) * STR100);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    memset(tmp, '\0', STR100);
    sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    return tmp;
}

/*-------------------- <mail box> --------------------*/
char* createMail(redisContext *c, char *sender, char *message, char *date)
{
    redisReply *reply;
    char *key = (char*)malloc(sizeof(char) * STR100);
    char buf[MLEN] = {'\0'};
    int mailId;

    for(int i=0; message[i]!='\0'; i+=(strlen(message + i) + 1))
        sprintf(buf + strlen(buf), "%s ", &message[i]);

    reply = redisCommand(c, "get mail_count");
    mailId = atoi(reply->str);
    freeReplyObject(reply);

    reply = redisCommand(c, "incr mail_count");
    freeReplyObject(reply);

    memset(key, '\0', STR100);
    sprintf(key, "%d_mail", mailId);

    reply = redisCommand(c, "lpush %s %d %s %s %s", key, mailId, date, sender, buf);
    freeReplyObject(reply);

    return key;
}

void addMailToRecvMailList(redisContext *c, char *recvr, char *mail)
{
    redisReply *reply;
    reply = redisCommand(c, "lpush %s_mailbox %s", recvr, mail);
    freeReplyObject(reply);
}

void mailto(int pid, char *arg)
{
    redisContext *c = dbConnect();
    char *name = NULL;

    if(checkUser(c, arg, &name, 1) == USERUNEXIST)
    {
        sendToClient(pid, strlen("User not found !\n"), "User not found !\n");
        sendToClient(pid, 0, NULL);
        goto END;
    }

    char *user = getName(pid);
    char *date = getDate();
    char *mail = createMail(c, user, (arg + strlen(arg) + 1), date);
    addMailToRecvMailList(c, name, mail);

    free(user);
    free(date);
    free(mail);
    sendToClient(pid, strlen("Send accept !\n"), "Send accept !\n");
    sendToClient(pid, 0, NULL);
END:
    redisFree(c);
    free(name);

}

int removeFromList(redisContext *c, char *listKey, char *del)
{

    char *list = dbList(c, listKey);
    redisReply *reply;
    int index = 0, result = -1;

    if(list)
    {
        for(int i=0; list[i] != '\0'; i+=(strlen(list + i) + 1))
        {
            if(!strcmp(&list[i], del))
                break;
            index ++;
        }

        reply = redisCommand(c, "lrem %s %d %s", listKey, index, del);
        result = reply->integer;
        freeReplyObject(reply);
        free(list);
    }

    return result;
}

void delMail(int pid, char *arg)
{
    char *user = getName(pid);
    char del[STR100] = {'\0'};
    char list[STR100] = {'\0'};
    redisContext *c = dbConnect();
    int result;

    sprintf(list, "%s_mailbox", user);
    sprintf(del, "%s_mail", arg);

    dbDelete(c, del);                           // DEL the mail from db
    result = removeFromList(c, list, del);      // remove from user mail list

    redisFree(c);
    free(user);

    if(result)
    {
        sendToClient(pid, strlen("Delete accept !\n"), "Delete accept !\n");
        sendToClient(pid, 0, NULL);
    }else
    {
        sendToClient(pid, strlen("Mail id unexist !\n"), "Mail id unexist !\n");
        sendToClient(pid, 0, NULL);
    }
}

void listMail(int pid)
{
    redisContext *c = dbConnect();
    redisReply *reply;
    char buf[MLEN] = {'\0'};
    char key[STR100] = {'\0'};
    char *user = getName(pid);

    sprintf(key, "%s_mailbox", user);
    char *list = dbList(c, key);

    if(list)
    {
        sprintf(buf, "%s", "<id>  <date>               <sender>    <message>\n");
        for(int i=0; list[i] != '\0'; i+=(strlen(list + i) + 1))
        {
            reply = redisCommand(c, "lrange %s 0 -1", &list[i]); // need test !!
            if(reply->type == REDIS_REPLY_ARRAY)
                sprintf(buf + strlen(buf), "%-5s %-20s %-11s %-s\n", 
                                                            reply->element[3]->str,
                                                            reply->element[2]->str,
                                                            reply->element[1]->str,
                                                            reply->element[0]->str);
            freeReplyObject(reply);
        }
    }else
        sprintf(buf, "%s", "Empty !\n");

    sendToClient(pid, strlen(buf), buf);
    sendToClient(pid, 0, NULL);

    redisFree(c);
    free(list);
    free(user);
}

/*-------------------- <group> --------------------*/
void createGroup(int pid, char *arg)
{
    redisContext *c = dbConnect();
    redisReply *reply;
    char *list = dbList(c, "group_list");

    if(list)
    {
        for(int i=0; list[i] != '\0'; i+=(strlen(list + i) + 1))
        {
            if(!strcmp(&list[i], arg))
            {
                sendToClient(pid, strlen("Group already exist !\n"), "Group already exist !\n");
                sendToClient(pid, 0, NULL);
                goto END;
            }
        }
    }

    char *user = getName(pid);

    reply = redisCommand(c, "lpush group_list %s", arg);
    freeReplyObject(reply);

    reply = redisCommand(c, "lpush %s %s", arg, user); // mark for owner !!
    freeReplyObject(reply);

    reply = redisCommand(c, "lpush %s_group_list %s", user, arg);
    freeReplyObject(reply);
    
    sendToClient(pid, strlen("Create success !\n"), "Create success !\n");
    sendToClient(pid, 0, NULL);

    free(user);
    free(list);
END:

    redisFree(c);
}

int isGroupExist(redisContext *c, int pid, char *group, char *cpy)
{
    char *list = dbList(c, "group_list");
    int exist = 0;
    for(int i=0; list[i] != '\0'; i+=(strlen(list + i) + 1))
    {
        if(!strcmp(&list[i], group))
        {
            exist = 1;
            if(cpy)
                strcpy(cpy, &list[i]);
            break;
        }
    }
    free(list);
    if(exist == 0)
    {
        sendToClient(pid, strlen("Group not found !\n"), "Group not found !\n");
        sendToClient(pid, 0, NULL); 
    }
    return exist;
}

void delGroup(int pid, char *arg) // !!
{
    redisContext *c = dbConnect();
    redisReply *reply;
    char *list;
    char delGroup[STR100] = {'\0'};

    if(isGroupExist(c, pid, arg, delGroup) == 0)
    {
        redisFree(c);
        return;
    }

    list = dbList(c, delGroup); // group member list
    for(int i=0, j; list[i] != '\0'; i+=(strlen(list + i) + 1))
    {
        char key[STR100] = {'\0'};
        sprintf(key, "%s_group_list", &list[i]);
        char *tmp = dbList(c, key); // user group list
        int count = 0;

        for(j=0; tmp[j] != '\0'; j+=(strlen(tmp + j) + 1))
        {
            count ++;
            if(!strcmp(&tmp[j], delGroup))
                break;
        }

        reply = redisCommand(c, "lrem %s_group_list %d %s", &list[i], count, &tmp[j]);
        freeReplyObject(reply);
        free(tmp);
    }

    reply = redisCommand(c, "lrem group_list %d %s", index, delGroup);
    freeReplyObject(reply);
    redisFree(c);

    sendToClient(pid, strlen("Group delete success !\n"), "Group delete success !\n");
    sendToClient(pid, 0, NULL); 
}

void addTo(int pid, char *arg)
{
    redisContext *c = dbConnect();
    redisReply *reply;
    char *user = getName(pid);
    char *list = dbList(c, arg);
    char buf[MLEN] = {'\0'};
    char buf1[STR100] = {'\0'};
    char buf2[STR100] = {'\0'};
    char buf3[STR100] = {'\0'};

    if(isGroupExist(c, pid, arg, NULL) == 0)
        goto END;

    if(strcmp(list, user))
    {
        sendToClient(pid, strlen("You don't have permissions !\n"), "You don't have permissions !\n");
        sendToClient(pid, 0, NULL);
        goto END;
    }

    // add group to each user's user group list.
    for(int i=strlen(arg) + 1, unexist; arg[i] != '\0'; i+=(strlen(arg + i) + 1))
    {
        unexist = 1;
        memset(buf, '\0', STR100);
        for(int j=strlen(list) + 1; list[j] != '\0'; j+=(strlen(list + j) + 1))
        {
            if(!strcmp(&arg[i], &list[j]))
            {
                unexist = 0;
                sprintf(buf1 + strlen(buf1), "%s ", &arg[i]);
                break;
            }
        }
        if(unexist)
        {
            if(checkUser(c, &arg[i], NULL, 1) == USERUNEXIST)
                sprintf(buf2 + strlen(buf2), "%s ", &arg[i]);
            else
            {
                reply = redisCommand(c, "rpush %s %s", arg, &arg[i]);
                freeReplyObject(reply);
                reply = redisCommand(c, "lpush %s_group_list %s", &arg[i], arg);
                freeReplyObject(reply);
                
                sprintf(buf3 + strlen(buf3), "%s ", &arg[i]);
            }
        }
    }

    if(strlen(buf1))
        sprintf(buf1 + strlen(buf1), "%s", "already in group !\n");
    if(strlen(buf2))
        sprintf(buf2 + strlen(buf2), "%s", "not found !\n");
    if(strlen(buf3))
        sprintf(buf3 + strlen(buf3), "%s", "add success !\n");
    sprintf(buf, "%s%s%s", buf1, buf2, buf3);
    sendToClient(pid, strlen(buf), buf);
    sendToClient(pid, 0, NULL);

END:
    redisFree(c);
    free(list);
    free(user);
}

void removeUser(int pid, char *arg) // !!
{
    redisContext *c = dbConnect();
    redisReply *reply;
    char *user = getName(pid);
    char *list = dbList(c, arg);
    char buf[MLEN] = {'\0'};
    char buf1[STR100] = {'\0'};
    char buf2[STR100] = {'\0'};
    char buf3[STR100] = {'\0'};

    if(isGroupExist(c, pid, arg, NULL) == 0)
        goto END;

    if(strcmp(list, user))
    {
        sendToClient(pid, strlen("You don't have permissions !\n"), "You don't have permissions !\n");
        sendToClient(pid, 0, NULL);
        goto END;
    }

    // ignore first element, because it is gronp name !!
    for(int i=strlen(arg) + 1, index, unexist; arg[i] != '\0'; i+=(strlen(arg + i) + 1)) // list for remove
    {
        index = 2;
        unexist = 1;
        if(checkUser(c, &arg[i], NULL, 1) == USERUNEXIST)
            sprintf(buf1 + strlen(buf1), "%s ", &arg[i]);
        else
        {
            // ignore first element, because it is the owner !!
            for(int j=strlen(list) + 1; list[j] != '\0'; j+=(strlen(list + j) + 1)) // list for user's group list
            {
                if(!strcmp(&arg[i], &list[j]))
                {
                    reply = redisCommand(c, "lrem %s %d %s", arg, index, &arg[i]);
                    freeReplyObject(reply);
                    sprintf(buf3 + strlen(buf3), "%s ", &arg[i]);
                    unexist = 0;

                    char uGroup[STR100] = {'\0'};
                    sprintf(uGroup, "%s_group_list", &arg[i]);
                    char *tmp = dbList(c, uGroup);
                    int uIndex = 0;
                    for(int h=0; tmp[h]!='\0'; h+=(strlen(tmp + h) + 1))
                    {
                        if(!strcmp(&tmp[h], arg))
                        {
                            uIndex ++;
                            break;
                        }
                    }
                    reply = redisCommand(c, "lrem %s %d %s", uGroup, uIndex, arg);
                    freeReplyObject(reply);
                    free(tmp);
                    break;
                }
                index ++;
            }
            if(unexist)
                sprintf(buf2 + strlen(buf2), "%s ", &arg[i]);
        }
    }

    if(strlen(buf1))
        sprintf(buf1 + strlen(buf1), "%s", "not found !\n");
    if(strlen(buf2))
        sprintf(buf2 + strlen(buf2), "%s", "not in group !\n");
    if(strlen(buf3))
        sprintf(buf3 + strlen(buf3), "%s", "remove success !\n");
    
    sprintf(buf, "%s%s%s", buf1, buf2, buf3);
    sendToClient(pid, strlen(buf), buf);
    sendToClient(pid, 0, NULL);
END:
    redisFree(c);
    free(list);
    free(user);
}

void leaveGroup(int pid, char *arg)
{
    redisContext *c = dbConnect();
    char *user = getName(pid);
    char key[STR100] = {'\0'};
    int result;

    sprintf(key, "%s_group_list", user);
    free(user);

    if(isGroupExist(c, pid, arg, NULL) == 0)
        goto END;

    result = removeFromList(c, key, arg);
    if(result)
    {
        sendToClient(pid, strlen("Leave accept !\n"), "Leave accept !\n");
        sendToClient(pid, 0, NULL);
    }else
    {
        sendToClient(pid, strlen("Leave fault !\n"), "Leave fault !\n");
        sendToClient(pid, 0, NULL);
    }

END:
    redisFree(c);
}

char* getOwner(redisContext *c, char *key)
{
    char *list = dbList(c, key);
    char *owner = (char*)malloc(sizeof(char) * strlen(list));
    strcpy(owner, list);
    free(list);
    return owner;
}

void listGroup(int pid)
{
    redisContext *c = dbConnect();
    char key[STR100] = {'\0'};
    char buf[MLEN] = {'\0'};
    char *user = getName(pid);

    sprintf(key, "%s_group_list", user);
    free(user);
    char *list = dbList(c, key);

    if(list)
    {
        sprintf(buf, "<owner>    <group>\n");
        for(int i=0; list[i] != '\0'; i+=(strlen(list + i) + 1))
        {
            char *owner = getOwner(c, &list[i]);
            sprintf(buf + strlen(buf), "%-10s %-7s\n", owner, &list[i]);
            free(owner);
        }
    }else
        sprintf(buf, "%s", "Empty !\n");

    sendToClient(pid, strlen(buf), buf);
    sendToClient(pid, 0, NULL);
    redisFree(c);
    if(list)
        free(list);
}

int isUserInGroup(redisContext *c, char *list, char *user)
{
    for(int i=0; list[i]!='\0'; i+=strlen(list + i) + 1)
        if(!strcmp(&list[i], user))
            return 1;
    return 0;
}

void gyell(int pid, char *arg)
{
    redisContext *c = dbConnect();
    char *user = getName(pid);
    char *list = dbList(c, arg);
    char buf[STR100] = {'\0'};

    if(isGroupExist(c, pid, arg, NULL) == 0)
        goto END;

    if(list && isUserInGroup(c, list, user) == 0)
    {
        sendToClient(pid, strlen("You are not in group !\n"), "You are not in group !\n");
        sendToClient(pid, 0, NULL);
        goto END;
    }

    sprintf(buf, "<%s-%s>: ", arg, user);
    for(int i=strlen(arg) + 1; arg[i]!='\0'; i+=strlen(arg + i) + 1)
        sprintf(buf + strlen(buf), "%s ", &arg[i]);
    sprintf(buf + strlen(buf), "%c", '\n');

    for(int i=0; list[i]!='\0'; i+=strlen(list + i) + 1)
    {
        for(int j=0; j<USERTABLESIZE; j++)
        {
            if(cuTable.table[j] && !strcmp(cuTable.table[j]->name, &list[i]))
            {
                sendToClient(cuTable.table[j]->pid, strlen(buf), buf);
                sendToClient(cuTable.table[j]->pid, 0, NULL);
            }
        }
    }

END:
    redisFree(c);
    free(user);
    free(list);
}

void requestHandler(int recvfd)
{
    int aLen, cmd, pid;
    char args[MLEN] = {'\0'};
    
    _read(recvfd, &pid, 4);
    _read(recvfd, &cmd, 4);
    _read(recvfd, &aLen, 4);
    _read(recvfd, args, aLen);

    switch (cmd)
    {
    case PRINTENV:
        break;
    case SETENV:
        break;
    case WHO:
        who(pid);
        break;
    case TELL:
        tell(pid, args);
        break;
    case YELL:
        yell(pid, args);
        break;
    case MAILTO:
        mailto(pid, args);
        break;
    case DELMAIL:
        delMail(pid, args);
        break;
    case LISTMAIL:
        listMail(pid);
        break;
    case CREATEGROUP:
        createGroup(pid, args);
        break;
    case DELGROUP:
        delGroup(pid, args);
        break;
    case ADDTO:
        addTo(pid, args);
        break;
    case LEAVEGROUP:
        leaveGroup(pid, args);
        break;
    case LISTGROUP:
        listGroup(pid);
        break;
    case REMOVE:
        removeUser(pid, args);
        break;
    case GYELL:
        gyell(pid, args);
        break;
    default:
        break;
    }
}

// void name(int pid, char *args)
// {
//     int index;

//     for(index=0; index<USERTABLESIZE; index++)
//         if(cuTable.table[index] && cuTable.table[index]->pid == pid)
//             break;
    
//     if(isNameExist(args))
//     {
//         char buf[MLEN] = {'\0'};
//         sprintf(buf, "User %s already exist!\n", args);
//         sendToClient(pid, strlen(buf), buf);
//         sendToClient(pid, 0, NULL);
//     }else
//     {
//         memset(cuTable.table[index]->name, '\0', USERNAMESIZE);
//         strcpy(cuTable.table[index]->name, args);
//         sendToClient(pid, strlen("name change accept!\n"), "name change accept!\n");
//         sendToClient(pid, 0, NULL);
//     }
// }