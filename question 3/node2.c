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

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

void rtinit2() {
  int i, j;
  
  // Initialize distance table
  for(i = 0; i < 4; i++) {
      for(j = 0; j < 4; j++) {
          dt2.costs[i][j] = 999;
      }
  }
  
  // Direct link costs
  dt2.costs[0][2] = 3;   // Cost to node 0
  dt2.costs[1][2] = 1;   // Cost to node 1
  dt2.costs[2][2] = 0;   // Cost to self
  dt2.costs[3][2] = 2;   // Cost to node 3

  // Send initial distance vector to neighbors (0,1,3)
  struct rtpkt pkt;
  pkt.sourceid = 2;
  int neighbors[] = {0, 1, 3};
  for(i = 0; i < 3; i++) {
      pkt.destid = neighbors[i];
      for(j = 0; j < 4; j++) {
          pkt.mincost[j] = dt2.costs[j][2];
      }
      tolayer2(pkt);
  }
}


void rtupdate2(struct rtpkt *rcvdpkt) {
  int source = rcvdpkt->sourceid;
  int updated = NO;

  // Update distance table
  for(int dest=0; dest<4; dest++) {
      int new_cost = dt2.costs[source][source] + rcvdpkt->mincost[dest];
      if(new_cost < dt2.costs[dest][source]) {
          dt2.costs[dest][source] = new_cost;
          updated = YES;
      }
  }

  if(updated) {
      // Recompute minimum costs
      int new_mincost[4];
      for(int dest=0; dest<4; dest++) {
          new_mincost[dest] = 999;
          for(int via=0; via<4; via++) {
              if(dt2.costs[dest][via] < new_mincost[dest]) {
                  new_mincost[dest] = dt2.costs[dest][via];
              }
          }
      }

      // Send to neighbors (0,1,3)
      struct rtpkt pkt;
      pkt.sourceid = 2;
      int neighbors[] = {0, 1, 3};
      for(int i=0; i<3; i++) {
          pkt.destid = neighbors[i];
          memcpy(pkt.mincost, new_mincost, sizeof(new_mincost));
          tolayer2(pkt);
      }
      
      printf("Node 2 updated:\n");
      printdt2(&dt2);
  }
}


printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}







