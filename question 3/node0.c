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
} dt0;


/* students to write the following two routines, and maybe some others */

void rtinit0() {
  int i, j;

  // Initialize distance table
  for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
          dt0.costs[i][j] = 999; // Set all costs to infinity
      }
  }

  // Set direct link costs
  dt0.costs[0][0] = 0;   // Cost to self
  dt0.costs[1][0] = 1;   // Cost to node 1
  dt0.costs[2][0] = 3;   // Cost to node 2
  dt0.costs[3][0] = 7;   // Cost to node 3

  // Create and send routing packets to neighbors
  struct rtpkt pkt;
  pkt.sourceid = 0;
  for (i = 1; i < 4; i++) {
      pkt.destid = i;
      for (j = 0; j < 4; j++) {
          pkt.mincost[j] = dt0.costs[j][0]; // Minimum cost vector
      }
      tolayer2(pkt); // Send packet to layer2
  }

  printf("Node 0 initialized.\n");
}



void rtupdate0(struct rtpkt *rcvdpkt) {
  int source = rcvdpkt->sourceid;
  int updated = NO;

  // Update distance table using Bellman-Ford equation
  for(int dest=0; dest<4; dest++) {
      int new_cost = dt0.costs[source][source] + rcvdpkt->mincost[dest];
      if(new_cost < dt0.costs[dest][source]) {
          dt0.costs[dest][source] = new_cost;
          updated = YES;
      }
  }

  // Propagate updates if needed
  if(updated) {
      // Recompute minimum costs
      int new_mincost[4];
      for(int dest=0; dest<4; dest++) {
          new_mincost[dest] = 999;
          for(int via=0; via<4; via++) {
              if(dt0.costs[dest][via] < new_mincost[dest]) {
                  new_mincost[dest] = dt0.costs[dest][via];
              }
          }
      }

      // Send updates to neighbors (1,2,3)
      struct rtpkt pkt;
      pkt.sourceid = 0;
      int neighbors[] = {1, 2, 3};
      for(int i=0; i<3; i++) {
          pkt.destid = neighbors[i];
          memcpy(pkt.mincost, new_mincost, sizeof(new_mincost));
          tolayer2(pkt);
      }
      
      printf("Node 0 updated:\n");
      printdt0(&dt0);
  }
}




printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

void linkhandler0(int linkid, int newcost) {
  printf("Node 0: Link cost to Node %d changed to %d.\n", linkid, newcost);

  dt0.costs[linkid][linkid] = newcost;

  struct rtpkt pkt;
  pkt.sourceid = 0;
  for (int i = 1; i < 4; i++) {
      pkt.destid = i;
      for (int j = 0; j < 4; j++) {
          int min_cost = dt0.costs[j][j];
          for (int k = 1; k < 4; k++) {
              if (dt0.costs[j][k] < min_cost) {
                  min_cost = dt0.costs[j][k];
              }
          }
          pkt.mincost[j] = min_cost;
      }
      tolayer2(pkt); // Send updated packet
  }

  printf("Node 0 sent updated packets due to link change.\n");
}


