#ifndef _MYIPC_SHM_H_
#define _MYIPC_SHM_H_

#ifdef __cplusplus
extern "C" {
#endif 

	int IPC_CreatShm(char *shmfilepath, int shmsize, int *shmhande);

	int IPC_MapShm(int shmhande, void **mapaddr);

	int IPC_UnMapShm(void *unmapaddr);

	int IPC_DelShm(int shmhande);

#ifdef __cplusplus
}
#endif
#endif

