#ifndef SEMAPHORE_H
#define SEMAPHORE_H



/*************************************************************************
 * TYPEDEFS
 *************************************************************************/

typedef struct semaphore_st
{
    int			count; 		/* current semaphore count */
    void *		waitList;	/* pointer to suspended processes waitlist */
    
    void *		owner;	/* 在销毁该信号量时，必须先使等待进程列表为空才能销毁 */
    					/* 但唤醒这些等待进程时，不能保证这些进程能马上响应退出 */
    					/* 现在做法是：使该进程挂起，直到所有等待进程都退出后，才 */
    					/* 再次唤醒该进程进行信号量释放的动作。owner就是记录该进程 */
    					/* 的。owner只有在OS_DEL_ALWAYS状态的destroy函数中才被赋值， */
    					/* 也就是说，只有在强制销毁该信号量时才可能有值。在等待列表 */
    					/* 进程被唤醒时，如果判断到该值有效，则是强制退出的信号，等待 */
    					/* 进程必须退出 */
    int			cntList;	/* 在waitList中等待进程的个数 */
    
} SEMAPHORE;


#define OS_DEL_NO_PEND 1
#define OS_DEL_ALWAYS  2

/*----------------------------------------------------------------------
* 函数: semaphore_Init
* 说明: 创建信号量
* 参数: initialCount	--	信号量的初始计数
* 返回: 如果创建成功，返回指向信号量的指针；否则返回空指针
*-----------------------------------------------------------------------*/
SEMAPHORE *semaphore_init( int initialCount );

/*----------------------------------------------------------------------
* 函数: semaphore_destroy
* 说明: 销毁信号量
* 参数: psemaphore	-- 指向信号量的指针
*		opt: OS_DEL_NO_PEND -- 如果有进程等待，则该信号量不删除
*			 OS_DEL_ALWAYS --- 如果有进程等待，则设置所有等待进程都进入就绪
*								状态，并且每个任务都得到信号量。
* 返回: 0:SUCCESS or -1:FAIL
* 注意：如果判断到返回值为SUCCESS时，说明销毁成功，请置psemaphore为NULL，
*		防止下次或其他进程的再引用。
*-----------------------------------------------------------------------*/
int semaphore_destroy( SEMAPHORE *psemaphore, int opt );

/*----------------------------------------------------------------------
* 函数: semaphore_wait
* 说明: 信号量计数减少，如果信号量值为0，则调用进程进入等待状态
* 参数: psemaphore	-- 指向信号量的指针
* 返回: 0: success; < 0: fail
*-----------------------------------------------------------------------*/
int semaphore_wait( SEMAPHORE *psemaphore );

/*----------------------------------------------------------------------
* 函数: semaphore_timed_wait
* 说明: 信号量计数减少，如果信号量值为0，则调用进程进入等待状态
*       该函数与semaphore_wait的区别在于: 可以定义等待时间
* 参数: psemaphore: 指向信号量的指针
*		ticks:		等待的最长时间，即timeout时间
* 返回: state. -- 0: 正常返回
*                 -2: timeout返回
*                 other error(<0): 其他错误返回
*				  > 0: 在有效时间内正常获取到信号时，所等待的时间
*-----------------------------------------------------------------------*/
int semaphore_timed_wait( SEMAPHORE *psemaphore, unsigned ticks );

/*----------------------------------------------------------------------
* 函数: semaphore_trywait
* 说明: 若信号量值大于0，则递减之
* 参数: psemaphore	-- 指向信号量的指针
* 返回: 0: success; < 0: fail
*-----------------------------------------------------------------------*/
int semaphore_trywait(SEMAPHORE *psemaphore);

/*----------------------------------------------------------------------
* 函数: semaphore_post
* 说明: 信号量计数增加，如果信号量值原来为0，则唤醒所有等待信号量的进程
* 参数: psemaphore	-- 指向信号量的指针
* 返回: none
*-----------------------------------------------------------------------*/
void semaphore_post( SEMAPHORE *psemaphore );

/*----------------------------------------------------------------------
* 函数: semaphore_peek
* 说明: 获得信号量计数值
* 参数: psemaphore	-- 指向信号量的指针
* 返回: 信号量计数值
*-----------------------------------------------------------------------*/
int semaphore_peek( SEMAPHORE *psemaphore );



#endif /* SEMAPHORE_H */

