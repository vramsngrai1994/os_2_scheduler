/*
 * Dummy scheduling class, mapped to range of 5 levels of SCHED_NORMAL policy
 */

#include "sched.h"

/*
 * Timeslice and age threshold are represented in jiffies. Default timeslice
 * is 100ms. Both parameters can be tuned from /proc/sys/kernel.
 */

#define DUMMY_TIMESLICE		(100 * HZ / 1000)
#define DUMMY_AGE_THRESHOLD	(3 * DUMMY_TIMESLICE)

unsigned int sysctl_sched_dummy_timeslice = DUMMY_TIMESLICE;
static inline unsigned int get_timeslice()
{
	return sysctl_sched_dummy_timeslice;
}

unsigned int sysctl_sched_dummy_age_threshold = DUMMY_AGE_THRESHOLD;
static inline unsigned int get_age_threshold()
{
	return sysctl_sched_dummy_age_threshold;
}

/*
 * Init
 */

void init_dummy_rq(struct dummy_rq *dummy_rq, struct rq *rq)
{
	int i;
	for(i = 0; i < 5; i++) {
		INIT_LIST_HEAD(&dummy_rq->queues[i]);
	}
}

/*
 * Helper functions
 */

static inline struct task_struct *dummy_task_of(struct sched_dummy_entity *dummy_se)
{
	return container_of(dummy_se, struct task_struct, dummy_se);
}

static inline void _enqueue_task_dummy(struct rq *rq, struct task_struct *p)
{
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
	int prio = p->prio - 131; // to get index between 0 and 4 
	struct list_head *queue = &rq->dummy.queues[prio];
	list_add_tail(&dummy_se->run_list, queue);
	printk(KERN_CRIT "enqueue: %d\n",p->pid);
}

static inline void _dequeue_task_dummy(struct task_struct *p)
{
	struct sched_dummy_entity *dummy_se = &p->dummy_se;
	list_del_init(&dummy_se->run_list);
	printk(KERN_CRIT "dequeue: %d\n",p->pid);
}

/*
 * Scheduling class functions to implement
 */

static void enqueue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	_enqueue_task_dummy(rq, p);
	inc_nr_running(rq);
}

static void dequeue_task_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	_dequeue_task_dummy(p);
	dec_nr_running(rq);
}

static void yield_task_dummy(struct rq *rq)
{
	dequeue_task_dummy(rq, rq->curr, rq->curr->flags);
	enqueue_task_dummy(rq, rq->curr, rq->curr->flags);
	// resched_task ? 
	printk(KERN_CRIT "yield: %d\n",p->pid);
}

static void check_preempt_curr_dummy(struct rq *rq, struct task_struct *p, int flags)
{
	if(p->prio < rq->curr->prio) {
		// dequeue_task_dummy(rq, rq->curr, flags);
		// enqueue_task_dummy(rq, rq->curr, flags);
		printk(KERN_CRIT "preempt: %d\n",p->pid);
		resched_task(rq->curr);	
	}

}

static struct task_struct *pick_next_task_dummy(struct rq *rq)
{
	struct dummy_rq *dummy_rq = &rq->dummy;
	struct sched_dummy_entity *next;
	int j;
	for(j = 0; j < 5; j++) {
		if (!list_empty(&dummy_rq->queues[j])) {
			next = list_first_entry(&dummy_rq->queues[j], struct sched_dummy_entity, run_list);
		printk(KERN_CRIT "pick_next: %d\n",p->pid);		
		return dummy_task_of(next);
		}
	}

	return NULL;
}

static void put_prev_task_dummy(struct rq *rq, struct task_struct *prev)
{
}

static void set_curr_task_dummy(struct rq *rq)
{
}

static void task_tick_dummy(struct rq *rq, struct task_struct *curr, int queued)
{
}

static void switched_from_dummy(struct rq *rq, struct task_struct *p)
{
}

static void switched_to_dummy(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_dummy(struct rq *rq, struct task_struct *p, int oldprio)
{
}

static unsigned int get_rr_interval_dummy(struct rq *rq, struct task_struct *p)
{
	return get_timeslice();
}

/*
 * Scheduling class
 */

const struct sched_class dummy_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_dummy,
	.dequeue_task		= dequeue_task_dummy,
	.yield_task		= yield_task_dummy,

	.check_preempt_curr 	= check_preempt_curr_dummy,

	.pick_next_task		= pick_next_task_dummy,
	.put_prev_task		= put_prev_task_dummy,

	.set_curr_task		= set_curr_task_dummy,
	.task_tick		= task_tick_dummy,

	.switched_from		= switched_from_dummy,
	.switched_to		= switched_to_dummy,
	.prio_changed		= prio_changed_dummy,

	.get_rr_interval	= get_rr_interval_dummy,
};

