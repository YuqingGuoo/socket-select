/* client.c --- 
 * Filename: client.c
 * Created: Mon Jan  8 16:17:14 2018 (+0800)
 */

/* Copyright Hamlet_Jiaxiaoyu.
 * 
 * 允许免费使用，拷贝，修改，发布，但在所有的拷贝上必须保留上述
 * copyright部分和本使用声明部分，除非显示声明，copyright的持有者
 * 不得作为再发布软件的广告。copyright的持有者对使用本软件的适用范围不做任何声明，
 * 
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define MAXLINE 1024
#define IPADDRESS "127.0.0.1"
#define SERV_PORT 8787

#define max(a,b) (a > b) ? a : b

static void handle_recv_msg(int sockfd, char *buf) 
{
    printf("client recv msg is:%s\n", buf);
    sleep(5);
    write(sockfd, buf, strlen(buf) +1);
}

static void handle_connection(int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    int maxfdp,stdineof;
    fd_set readfds;
    int n;
    struct timeval tv;
    int retval = 0;

    while (1) {

        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);
        maxfdp = sockfd;

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(maxfdp+1,&readfds,NULL,NULL,&tv);

        if (retval == -1) {
            return ;
        }

        if (retval == 0) {
            printf("client timeout.\n");
            continue;
        }

        if (FD_ISSET(sockfd, &readfds)) {
            n = read(sockfd,recvline,MAXLINE);
            if (n <= 0) {
                fprintf(stderr,"client: server is closed.\n");
                close(sockfd);
                FD_CLR(sockfd,&readfds);
                return;
            }

            handle_recv_msg(sockfd, recvline);
        }
    }
}

int main(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,IPADDRESS,&servaddr.sin_addr);

    int retval = 0;
    retval = connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    if (retval < 0) {
        fprintf(stderr, "connect fail,error:%s\n", strerror(errno));
        return -1;
    }

    printf("client send to server .\n");
    write(sockfd, "hello server", 32);

    handle_connection(sockfd);

    return 0;
}
