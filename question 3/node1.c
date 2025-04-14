#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };


extern int TRACE;
extern int YES;
extern int NO;

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */


void rtinit1() {
  int i, j;
  
  // Initialize distance table with infinity (999)
  for(i = 0; i < 4; i++) {
      for(j = 0; j < 4; j++) {
          dt1.costs[i][j] = 999;
      }
  }
  
  // Set direct link costs
  dt1.costs[0][1] = 1;   // Cost to node 0
  dt1.costs[1][1] = 0;   // Cost to self
  dt1.costs[2][1] = 1;   // Cost to node 2

  // Send initial distance vector to neighbors (0 and 2)
  struct rtpkt pkt;
  pkt.sourceid = 1;
  int neighbors[] = {0, 2};
  for(i = 0; i < 2; i++) {
      pkt.destid = neighbors[i];
      for(j = 0; j < 4; j++) {
          pkt.mincost[j] = dt1.costs[j][1];
      }
      tolayer2(pkt);
  }
}


void rtupdate1(struct rtpkt *rcvdpkt) {
  int source = rcvdpkt->sourceid;
  int updated = NO;

  // Update distance table
  for(int dest=0; dest<4; dest++) {
      int new_cost = dt1.costs[source][source] + rcvdpkt->mincost[dest];
      if(new_cost < dt1.costs[dest][source]) {
          dt1.costs[dest][source] = new_cost;
          updated = YES;
      }
  }

  if(updated) {
      // Compute new minimum costs
      int new_mincost[4];
      for(int dest=0; dest<4; dest++) {
          new_mincost[dest] = 999;
          for(int via=0; via<4; via++) {
              if(dt1.costs[dest][via] < new_mincost[dest]) {
                  new_mincost[dest] = dt1.costs[dest][via];
              }
          }
      }

      // Send to neighbors (0,2)
      struct rtpkt pkt;
      pkt.sourceid = 1;
      int neighbors[] = {0, 2};
      for(int i=0; i<2; i++) {
          pkt.destid = neighbors[i];
          memcpy(pkt.mincost, new_mincost, sizeof(new_mincost));
          tolayer2(pkt);
      }
      
      printf("Node 1 updated:\n");
      printdt1(&dt1);
  }
}



printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



void linkhandler1(int linkid, int newcost) {
  dt1.costs[linkid][linkid] = newcost;
  // Trigger distance vector recomputation
  // Send updates to neighbors if needed
}

