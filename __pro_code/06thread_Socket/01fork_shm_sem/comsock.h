/* 
 * File:   comsock.h
 * Author: luv letter
 *
 * Created on 2017年3月21日, 上午12:05
 */

#ifndef COMSOCK_H
#define	COMSOCK_H

#ifdef	__cplusplus
extern "C"
{
#endif
	
	
	enum SckErrType
	{
		Sck_Ok = 0,
		Sck_BaseErr = 3000,
		Sck_ErrParam,
		Sck_ErrTimeOut,
		Sck_ErrPeerClosed,
		Sck_ErrMalloc,
		Sck_ErrDefalut
	};
	
	// 客户端初始化
	enum SckErrType  sck_client_init(void **handle, const int conntime, const int sendtime, const int recvtime, const int nConNum);
	// 客户端 文件句柄关闭
	enum SckErrType  sck_client_close(void *handle);
	// 客户端连接 把连接另外拿出来, 就是为了防止多次连接的情况出现
	enum SckErrType  sck_client_getconn(void *handle, const char *ip, const int port);
	// 客户端发送消息
	enum SckErrType  sck_client_send(void *handle, const char *buf, const unsigned int buflen);
	// 客户端接收消息
	enum SckErrType  sck_client_recv(void *handle, char *buf, unsigned int *buflen);
	// 客户端销毁
	enum SckErrType  sck_client_destroy(void *handle);
	
	
	// 服务器端初始化
	enum SckErrType  sck_server_init(const int port, int *listenfd);
	// 服务器端拿数据
	enum SckErrType sck_server_accept(const int listenfd, const int timeout, int *acceptfd, char ip[], int *port);
	// 服务器端发送消息
	enum SckErrType sck_server_send(const int acceptfd, const int timeout, const char *buf, const unsigned int buflen);
	// 服务器端接收消息
	enum SckErrType  sck_server_recv(const int acceptfd, const int timeout, char *buf, unsigned int *buflen);
	// 服务端关闭套接字句柄
	enum SckErrType  sck_server_close(const int sockfd);
	// 服务器端销毁
	enum SckErrType sck_server_destroy(int *sockfd);

#ifdef	__cplusplus
}
#endif

#endif	/* COMSOCK_H */

