#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <event.h>
#include <pthread.h>
#include "queue.h"

/*
 *	gdb command to debug the threads
 *		info threads
 *		thread 2
 *
 *
 */

typedef struct tid_thread_essential_s
{
  pthread_t thread_id;
  struct thread_essentials_s *thread_ess;
  TAILQ_ENTRY(tid_thread_essential_s) tid_thread_entry;
} tid_thread_essential_t;
    
typedef struct inter_thread_msg_s
{
  uint8_t format;
  uint8_t msg_type;
  uint8_t msg_len;
  TAILQ_ENTRY(inter_thread_msg_s) thread_msg_entry;
} inter_thread_msg_t;
  
typedef struct thread_essentials_s
{
	/* Core thread msg entry*/
  pthread_mutex_t core_mutex;
  int core_efd[2];
  TAILQ_HEAD(, inter_thread_msg_s) core_msg_entry;

	/* Worker thread msg entry*/
  pthread_mutex_t worker_mutex;
  int worker_efd[2];
  void *proto_private;
  struct event_base *worker_evbase;
  TAILQ_HEAD(, inter_thread_msg_s) worker_msg_entry;
} thread_essentials_t;

/************************* End of structure mapping *************************************/

TAILQ_HEAD(, tid_thread_essential_s) tid_thread_essential_entry;


void worker_on_read(int fd, short ev, void *arg)
{
	printf("%s: reading\n", __func__);
  uint64_t event_data = 0;
  size_t r_size = 0;
  thread_essentials_t *thr_ess = (thread_essentials_t *)arg;

  r_size = read(fd, &event_data, sizeof(uint64_t));

  if(r_size != sizeof(uint64_t))
    printf("Error in reading event !!\n");

  if(!TAILQ_EMPTY(&(thr_ess->worker_msg_entry))) {
    pthread_mutex_lock(&(thr_ess->worker_mutex));
    inter_thread_msg_t *thr_msg = TAILQ_FIRST(&(thr_ess->worker_msg_entry));
    TAILQ_REMOVE(&(thr_ess->worker_msg_entry), thr_msg, thread_msg_entry);
    pthread_mutex_unlock(&(thr_ess->worker_mutex));
#if 0 
    if(FORMAT_MSG == thr_msg->format) {
      if(WORKER_AUDIO == thr_ess->proto->interface) {
        audio_process_handler(thr_msg);
      } else if(WORKER_VIDEO == thr_ess->proto->interface) {
        video_process_handler(thr_msg);
      /*
			} else if(WORKER_ANY == thr_ess->proto->interface) {
        web_proto_mssg_sm_handler(thr_msg);
			*/
      }
    } else if(FORMAT_PULSE == thr_msg->format) {
      //handle_pulse(thr_msg);
    }
#endif		 
  }
}

void *worker_thread(thread_essentials_t *rt_thr_ess)
{
	printf("%s: created worker thread\n", __func__);
  struct event *ev;
  //thread_essentials_t *rt_thr_ess = rt_thr_ess;

  event_init();
  if((rt_thr_ess->worker_evbase = event_base_new()) == NULL) {
    printf("Unable to create event base !!\n");
    return NULL;
  }
  
	/*
  printf("proto->interface=%d\n", proto->interface);
  rt_thr_ess->proto_private = proto_reg[proto->interface].intf->init(proto);
 	*/
  ev = event_new(rt_thr_ess->worker_evbase, rt_thr_ess->worker_efd[0], EV_READ|EV_PERSIST,
         worker_on_read, (void *)rt_thr_ess);
  event_add(ev, NULL);

  event_base_dispatch(rt_thr_ess->worker_evbase);
  event_base_free(rt_thr_ess->worker_evbase);

  return NULL;
} 

void core_on_read(int fd, short ev, void *arg)
{
	printf("%s: core reading\n", __func__);
  uint64_t event_data = 0;
  size_t r_size = 0;
  thread_essentials_t *rt_thr_ess = (thread_essentials_t *)arg;

  r_size = read(fd, &event_data, sizeof(uint64_t));
  if(r_size != sizeof(uint64_t))
    printf("Error in reading event !!\n");
 
  if(!TAILQ_EMPTY(&(rt_thr_ess->core_msg_entry))) {
    pthread_mutex_lock(&(rt_thr_ess->core_mutex));
    inter_thread_msg_t *thr_msg = TAILQ_FIRST(&(rt_thr_ess->core_msg_entry));
    TAILQ_REMOVE(&(rt_thr_ess->core_msg_entry), thr_msg, thread_msg_entry);
    pthread_mutex_unlock(&(rt_thr_ess->core_mutex));
 		/* 
    if(FORMAT_MSG == thr_msg->format) {
      core_mssg_sm_handler(thr_msg, rt_thr_ess);
    } else if(FORMAT_PULSE == thr_msg->format) {
      //handlePulse(thr_msg);
    }
		*/
  }
}

int initialize_eventfd(thread_essentials_t *rt_thr_ess, struct event_base *ap_evbase)
{
	printf("%s: Core thread two way pipe initiliazing..\n", __func__);
  struct event *ev;
  int ret;

  ret = pipe(rt_thr_ess->core_efd);
  if(ret < 0) {
    printf("fails to create pipe for core_efd !!\n");
    return -1;
  }   
  ret = pipe(rt_thr_ess->worker_efd);
  if(ret < 0) {
    printf("fails to create pipe for worker_efd !!\n");
    return -1;
  }
  ev = event_new(ap_evbase, rt_thr_ess->core_efd[0], EV_READ|EV_PERSIST,
         core_on_read, (void *)rt_thr_ess);
  event_add(ev, NULL);
    
  return 0;
}

int create_thread(thread_essentials_t *rt_thr_ess, struct event_base *ap_evbase)
{
  printf("%s:On creating start thread:\n", __func__);
  pthread_t c_thread;
  int ret;

  pthread_mutex_init(&(rt_thr_ess->core_mutex), NULL);
  TAILQ_INIT(&(rt_thr_ess->core_msg_entry));

  pthread_mutex_init(&(rt_thr_ess->worker_mutex), NULL);
  TAILQ_INIT(&(rt_thr_ess->worker_msg_entry));

  if(ap_evbase != NULL)
    initialize_eventfd(rt_thr_ess, ap_evbase);

  ret = pthread_create(&c_thread, NULL, (void *)worker_thread, rt_thr_ess);
  if(ret) {
    printf("Error - pthread_create() return code: %d !!\n", ret);
    return -1;
  }
  add_thread_details_to_queue(c_thread, rt_thr_ess);
  
  return 0;
}

int send_message_to_core_thread(thread_essentials_t *thr_ess, inter_thread_msg_t *ap_thr_msg)
{
  ssize_t w_size;

  pthread_mutex_lock(&(thr_ess->core_mutex));
  TAILQ_INSERT_TAIL(&(thr_ess->core_msg_entry), ap_thr_msg,
    thread_msg_entry);
  pthread_mutex_unlock(&(thr_ess->core_mutex));

  if(thr_ess != NULL) {
    w_size = write(thr_ess->core_efd[1], &ap_thr_msg->msg_type, sizeof(uint64_t));
    if(w_size != sizeof(uint64_t))
      printf("Write size error !!\n");
  }

  return 0;
}

int send_message_to_worker_thread(thread_essentials_t *thr_ess, inter_thread_msg_t *ap_thr_msg)
{
  ssize_t w_size;

  pthread_mutex_lock(&(thr_ess->worker_mutex));
  TAILQ_INSERT_TAIL(&(thr_ess->worker_msg_entry), ap_thr_msg,
    thread_msg_entry);
  pthread_mutex_unlock(&(thr_ess->worker_mutex));

  if(thr_ess != NULL) {
    w_size = write(thr_ess->worker_efd[1], &ap_thr_msg->msg_type, sizeof(uint64_t));
    if(w_size != sizeof(uint64_t))
      printf("Write size error !!\n");
  }

  return 0;
}

int add_thread_details_to_queue(pthread_t ap_thread, thread_essentials_t *ap_thread_ess)
{ 
  tid_thread_essential_t *tid_thread_ess = calloc(sizeof(tid_thread_essential_t), 1);

  tid_thread_ess->thread_id = ap_thread;
  tid_thread_ess->thread_ess = ap_thread_ess;

  TAILQ_INSERT_TAIL(&tid_thread_essential_entry, tid_thread_ess, tid_thread_entry);

  return 0;
} 
 

int main()
{
	struct event_base *core_evbase;
	thread_essentials_t *rt_thr_ess;

  TAILQ_INIT(&tid_thread_essential_entry);
	event_init();
  if((core_evbase = event_base_new()) == NULL) {
      printf("Unable to create event base !!\n");
      return 1;
  }

  rt_thr_ess = (thread_essentials_t *)calloc(1, sizeof(struct thread_essentials_s));
	create_thread(rt_thr_ess, core_evbase);

	event_base_dispatch(core_evbase);
  event_base_free(core_evbase);
}
