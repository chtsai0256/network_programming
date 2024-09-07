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

int dbSearch(redisContext *c char *str)
{

}

/*--------------- <login> ---------------*/
void register(int clientd)
{
    char choose;
    char buf[MLEN];

    send(clientfd, "User not found !", strlen("User not found !"), 0);
    send(clientfd, "Create account or login again ? <1/2> : ", strlen("Create account or login again ? <1/2> : "), 0);
    recv(clientfd, choose, 1, 0);
    
    if(choose != '1')
        return ;
    
    while(1)
    {
        memset(buf, '\0', MLEN);
        send(clientfd, "your user name: ", strlen("your user name: "), 0);
        recv(clientfd, buf, MLEN, 0);
        send(clientfd, "your password: ", strlen("your password: "), 0);
        recv(clientfd, buf + strlen(buf), MLEN, 0);
        
        if(){
            send(clientfd, "User name already exist !", strlen("User name already exist !"), 0);
        }
    }

}

void login(int clientfd)
{
    char buf[MLEN] = {'\0'};
    int errFlag;

    while(1)
    {
        send(clientfd, "user_name: ", strlen("user_name: "), 0);
        recv(clientfd, buf, MLEN, 0);

        send(clientfd, "password: ", strlen("password: "), 0);
        recv(clientfd, buf + strlen(buf), MLEN, 0);
        
        // db check password
        if((errFlag = checkUser(buf)) == 0)
        {
            send(clientfd, "\033[H\033[J", strlen("\033[H\033[J"), 0);
            break;
        }

        if(errFlag == USERUNEXIST)
            register(clientfd);
        else if(errFlag == PASSWORDERR)
            send(clientfd, "Password error !", strlen("Password error !"), 0);
    }
}

