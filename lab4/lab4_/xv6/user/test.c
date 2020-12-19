#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "param.h"

#define szArrTicks 7
int arrTicks[szArrTicks] = {50, 500, 5000, 10000, 25000, 50000, 100000};

int spin() 
{
   while (1) 
   {
      int i, j;
      for (i = 0; i < 10000000; i++) 
      {
         j = i % 11;
         j--;
      }
   }
}

int main(int argc, char *argv[])
{
   int pids[szArrTicks];
   int ppid = getpid();
   int r, i, j;


   for (i=0; i<szArrTicks; i++)
   {
      pids[i] = fork();

      // runs in child
      if (pids[i] == 0) 
      {
         int tickets = arrTicks[i];
         r = setticket(tickets);
         printf(1, "Ticket Return : %d\n", r);

         if (r != 0) 
         {
            printf(1, "settickets failed\n");
            kill(ppid);
         }
         
         printf(1, "Tickets = %d\n", tickets);
         spin();     
      }
   }

   sleep(1000);
                                         
   int lticks[] = {-1, -1, -1, -1, -1, -1, -1};
   int hticks[] = {-1, -1, -1, -1, -1, -1, -1};
   struct pstat st;

   getpinfo(&st);
   
   for(i = 0; i < NPROC; i++) 
   {
      for(j = 0; j < szArrTicks; j++) 
      {
         if (st.inuse[i] && st.pid[i] == pids[j]) 
         {
            lticks[j] = st.lticks[i];
            hticks[j] = st.hticks[i];
         }
      }
   }

   for (i = 0; i < szArrTicks; i++) 
   {
      kill(pids[i]);
      wait();
   }

   for (i=0; i<szArrTicks; i++)
   {
      printf(1, "H-ticks (chld %d, pid %d, tkt %d): %d\n", i, pids[i], arrTicks[i], hticks[i]);
   }

   for (i=0; i<szArrTicks; i++)
   {
      printf(1, "L-ticks (chld %d, pid %d, tkt %d): %d\n", i, pids[i], arrTicks[i], lticks[i]);
   }
   
   exit();
}
