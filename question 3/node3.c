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
} dt3;

/* students to write the following two routines, and maybe some others */

void rtinit3() {
  int i, j;
  
  // Initialize distance table
  for(i = 0; i < 4; i++) {
      for(j = 0; j < 4; j++) {
          dt3.costs[i][j] = 999;
      }
  }
  
  // Direct link costs
  dt3.costs[0][3] = 7;   // Cost to node 0
  dt3.costs[2][3] = 2;   // Cost to node 2
  dt3.costs[3][3] = 0;   // Cost to self

  // Send initial distance vector to neighbors (0 and 2)
  struct rtpkt pkt;
  pkt.sourceid = 3;
  int neighbors[] = {0, 2};
  for(i = 0; i < 2; i++) {
      pkt.destid = neighbors[i];
      for(j = 0; j < 4; j++) {
          pkt.mincost[j] = dt3.costs[j][3];
      }
      tolayer2(pkt);
  }
}



void rtupdate3(struct rtpkt *rcvdpkt) {
  int source = rcvdpkt->sourceid;
  int updated = NO;

  // Update distance table
  for(int dest=0; dest<4; dest++) {
      int new_cost = dt3.costs[source][source] + rcvdpkt->mincost[dest];
      if(new_cost < dt3.costs[dest][source]) {
          dt3.costs[dest][source] = new_cost;
          updated = YES;
      }
  }

  if(updated) {
      // Compute new minimum costs
      int new_mincost[4];
      for(int dest=0; dest<4; dest++) {
          new_mincost[dest] = 999;
          for(int via=0; via<4; via++) {
              if(dt3.costs[dest][via] < new_mincost[dest]) {
                  new_mincost[dest] = dt3.costs[dest][via];
              }
          }
      }

      // Send to neighbors (0,2)
      struct rtpkt pkt;
      pkt.sourceid = 3;
      int neighbors[] = {0, 2};
      for(int i=0; i<2; i++) {
          pkt.destid = neighbors[i];
          memcpy(pkt.mincost, new_mincost, sizeof(new_mincost));
          tolayer2(pkt);
      }
      
      printf("Node 3 updated:\n");
      printdt3(&dt3);
  }
}


printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}







