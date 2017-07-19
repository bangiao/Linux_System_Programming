#ifndef __COMSOCK_H_
#define  __COMSOCK_H_

typedef enum
{
	Sck_OK = 0,
	Sck_ErrParam = -3000,
	Sck_ErrRetValue,	// 2999
	Sck_ErrTimeout,		// 2998
	Sck_ErrPeerClose,
}tsck_err_log;

// typedef struct _package
// {
// 	int data_len;
// 	char arr_data[1024];
// }t_package;


/**
* activate_noblock - 设置I/O为非阻塞模式
* @fd: 文件描符符
*/
int activate_nonblock(int fd);

/**
* deactivate_nonblock - 设置I/O为阻塞模式
* @fd: 文件描符符
*/
int deactivate_nonblock(int fd);

int recv_peek(int sockfd, void *buf, int len);

int readline(int sockfd, void *buf, int maxline);

/**
* readn - 读取固定字节数
* @fd: 文件描述符
* @buf: 接收缓冲区
* @count: 要读取的字节数
* 成功返回count，失败返回-1，目标端关闭, 读取的小于cuont
*/
int readn(int fd, void *buf, int count);

/**
* read_select - 读超时检测函数，不含读操作
* @fd: 文件描述符
* @wait_seconds: 等待超时秒数，如果为0表示不检测超时
* 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
*/
int read_select(int fd, unsigned int wait_seconds);

/**
* writen - 发送固定字节数
* @fd: 文件描述符
* @buf: 发送缓冲区
* @count: 要读取的字节数
* 成功返回count，失败返回-1 成功返回 0 客户端关闭返回 写了几个字符
*/
int writen(int fd, const void *buf, int count);

/**
* write_select - 写超时检测函数，不含写操作
* @fd: 文件描述符
* @wait_seconds: 等待超时秒数，如果为0表示不检测超时
* 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
*/
int write_select(int fd, unsigned int wait_seconds);

#endif