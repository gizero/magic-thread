#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define TIMER_SIG SIGRTMAX /* nostro segnale di notifica */

sem_t semTimer, semStart;
unsigned long int tickCount = 0;

static void up_error(const char *message)
{
  printf("%s\n", message);
}

static void up_info(const char *message)
{
  printf("%s\n", message);
}

static void handler(int sig, siginfo_t *si, void *uc)
{
   sem_post(&semTimer);                      // sblocco thread
}

void* thr_TimerMngr(void *arg)
{
   struct itimerspec ts;
   struct sigaction sa;
   struct sigevent sev;
   timer_t timer;

   while(1)
   {
     sleep(10);
     printf("I'm a thread and I'm alive...\n");
   }

#if 0
   /* Stabiliamo handler per il segnale */

   /* impostando SA_SIGINFO, il secondo parametro che viene passato all'handler e` siginfo_t il
   cui campo si_value e di tipo sigval corrispondente alla struttura utilizzata nella chiamata di
   timer_create() (sigev_value e in particolare sival_ptr a cui sara` assegnato indirizzo della
   struttura timer) */
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = handler;
   sigemptyset(&sa.sa_mask);
   if (sigaction(TIMER_SIG, &sa, NULL) == -1)
   {
      up_error("errore sigaction");
      return;
   }

   /* Crea e avvia un timer  */
   sev.sigev_notify = SIGEV_SIGNAL;             /* Notifica via segnale */
   sev.sigev_signo = TIMER_SIG;                 /* Notifica usando TIMER_SIG */
   sev.sigev_value.sival_ptr = &timer;          /* Ottiene ID del timer per l'handler */

   ts.it_value.tv_sec = 0;
   ts.it_value.tv_nsec = 10000000;

   ts.it_interval.tv_sec = 0;
   ts.it_interval.tv_nsec = 10000000;

   if (timer_create(CLOCK_REALTIME, &sev, &timer) == -1)
   {
      up_error("errore timer_create");
      return;
   }
   if (timer_settime(timer, 0, &ts, NULL) == -1)
   {
      up_error("errore timer_settime");
      return;
   }

   //memset(&timerSw,0,sizeof(timerSw));
   //memset(&timeOut,0,sizeof(timeOut));
   sem_init(&semTimer,0,0);

   sem_post(&semStart);                         // riabilito main thread

   while (1)
   {
      sem_wait(&semTimer);                      // attende risveglio ogni 10 ms
//      threads |= TH_TIMER;
     tickCount ++;                             // incrementa tick
if ((tickCount % 60000) == 0)
   up_info("1 minuto");
//      WakeUp_104();                             // per attivazione thread Process_104
   }
#endif
}

int main(void)
{
  pthread_t t;
  void *res;
  int ret;

  ret = pthread_create(&t, NULL, thr_TimerMngr, NULL);
  if (ret != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  printf("gonna wait for thread termination...\n");

  ret = pthread_join(t, res);
  if (ret != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  printf("thread terminated...\n");

  exit(EXIT_SUCCESS);
}
