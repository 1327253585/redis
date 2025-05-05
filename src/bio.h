/*
 * Copyright (c) 2009-Present, Redis Ltd.
 * All rights reserved.
 *
 * Licensed under your choice of the Redis Source Available License 2.0
 * (RSALv2) or the Server Side Public License v1 (SSPLv1).
 */

#ifndef __BIO_H
#define __BIO_H


typedef void lazy_free_fn(void *args[]);
typedef void comp_fn(uint64_t user_data, void *user_ptr);

typedef enum bio_worker_t {
    BIO_WORKER_CLOSE_FILE = 0,
    BIO_WORKER_AOF_FSYNC,
    BIO_WORKER_LAZY_FREE,
    BIO_WORKER_NUM
} bio_worker_t;

/* Background job opcodes */
typedef enum bio_job_type_t {
    BIO_CLOSE_FILE = 0,     /* Deferred close(2) syscall. */  // 关闭文件
    BIO_AOF_FSYNC,          /* Deferred AOF fsync. */ // 刷盘
    BIO_LAZY_FREE,          /* Deferred objects freeing. */ // 惰性释放
    BIO_CLOSE_AOF,          /* Deferred close(2) syscall for AOF. */ // 关闭AOF文件
    BIO_COMP_RQ_CLOSE_FILE,  /* Job completion request, registered on close-file worker's queue */  // 文件关闭成功的回调请求
    BIO_COMP_RQ_AOF_FSYNC,  /* Job completion request, registered on aof-fsync worker's queue */ // 刷盘完成的回调请求
    BIO_COMP_RQ_LAZY_FREE,  /* Job completion request, registered on lazy-free worker's queue */ // 对象惰性释放完成的请求
    BIO_NUM_OPS
} bio_job_type_t; // 后台任务类型

/* Exported API */
void bioInit(void); // 初始化
unsigned long bioPendingJobsOfType(int type); // 获取后台任务类型的任务数量
void bioDrainWorker(int job_type); //  阻塞等待后台任务类型的任务完成
void bioKillThreads(void); // 杀死所有线程
void bioCreateCloseJob(int fd, int need_fsync, int need_reclaim_cache); // 创建关闭文件任务
void bioCreateCloseAofJob(int fd, long long offset, int need_reclaim_cache); // 创建关闭AOF文件任务
void bioCreateFsyncJob(int fd, long long offset, int need_reclaim_cache); // 创建刷盘任务
void bioCreateLazyFreeJob(lazy_free_fn free_fn, int arg_count, ...); // 创建惰性释放任务
void bioCreateCompRq(bio_worker_t assigned_worker, comp_fn *func, uint64_t user_data, void *user_ptr); // 注册对应任务完成的回调函数


#endif
