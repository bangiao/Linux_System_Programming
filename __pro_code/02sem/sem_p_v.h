#ifndef __SEM_P_V_H__
#define __SEM_P_V_H__

#ifdef __cplusplus
extern "C" {
#endif

	// 创建当前信号			 返回值就是返回值, 不需要占用 int 返回值
	int sem_creat(int key, int *semid);
	 
	// 打开已有的信号
	int sem_open(int key, int *semid);

	// 设置当前信号资源的数量
	int sem_setval(int semid, int val);

	// 读取当前信号资源的数量
	int sem_getval(int semid, int *val);

	// 信号的 P 加锁操作 就是信号量的val --
	int sem_p(int semid);

	// 信号的 V 解锁操作, 就是信号量的val ++
	int sem_v(int semid);

#ifdef __cplusplus
}
#endif
#endif