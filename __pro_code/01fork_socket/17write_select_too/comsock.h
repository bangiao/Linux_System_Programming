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
* activate_noblock - ����I/OΪ������ģʽ
* @fd: �ļ������
*/
int activate_nonblock(int fd);

/**
* deactivate_nonblock - ����I/OΪ����ģʽ
* @fd: �ļ������
*/
int deactivate_nonblock(int fd);

int recv_peek(int sockfd, void *buf, int len);

int readline(int sockfd, void *buf, int maxline);

/**
* readn - ��ȡ�̶��ֽ���
* @fd: �ļ�������
* @buf: ���ջ�����
* @count: Ҫ��ȡ���ֽ���
* �ɹ�����count��ʧ�ܷ���-1��Ŀ��˹ر�, ��ȡ��С��cuont
*/
int readn(int fd, void *buf, int count);

/**
* read_select - ����ʱ��⺯��������������
* @fd: �ļ�������
* @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
* �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
*/
int read_select(int fd, unsigned int wait_seconds);

/**
* writen - ���͹̶��ֽ���
* @fd: �ļ�������
* @buf: ���ͻ�����
* @count: Ҫ��ȡ���ֽ���
* �ɹ�����count��ʧ�ܷ���-1 �ɹ����� 0 �ͻ��˹رշ��� д�˼����ַ�
*/
int writen(int fd, const void *buf, int count);

/**
* write_select - д��ʱ��⺯��������д����
* @fd: �ļ�������
* @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
* �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
*/
int write_select(int fd, unsigned int wait_seconds);

#endif