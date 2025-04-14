# CS 331 – Computer Networks Assignment 03

Team ID: 19  
Chandrabhan Patel: 22110057  
Parth Govale: 22110087  

---

##  Prerequisites

- **Mininet** (v2.3.0 or later)  
- **Open vSwitch**  
- **Ryu** controller framework  
- **iperf3**  
- **Python 3** (for Mininet scripts)  

All these except ryu are preinstalled on mininet VM. To install ryu, run the following command in the terminal:
```bash
pip instll ryu
```

---



##  Common Setup / Cleanup

```bash
# Clean up any leftover Mininet state
sudo mn -c
```
## Q1: Network Loops & STP

### 1. Start the Ryu STP app

```bash
sudo ryu-manager ryu.app.simple_switch_stp_13
```

Leave this running in one terminal.

### 2. Launch the Mininet topology

```bash
# in another terminal
sudo mn -c
sudo python3 q1_stp.py
```
### 3. In the Mininet CLI

```bash
mininet> pingall
mininet> h1 ping h3 -c 3
mininet> h5 ping h7 -c 3
mininet> h8 ping h2 -c 3
mininet> h1 tcpdump -i h1-eth0  #to see broadcast packets storms(in case of no stp)
```

## Q2: NAT Functionality via Host H9

### 1. (Re)Start Ryu if needed

```bash
sudo ryu-manager ryu.app.simple_switch_stp_13
```

### 2. Launch the NAT topology

```bash
sudo mn -c
sudo python3 q2.py
```

`q2.py` should:

- Build the same 4‐switch ring + diag topology.
- Add host `h9` with `172.16.10.10/24` on `h9-eth0`.
- Bridge `h9-eth1` + `h9-eth2` into `br0` with `10.1.1.1/24`.
- Enable IP forwarding + `iptables` MASQUERADE rules.
- Configure default routes on all hosts.
- Wait ~40 s for STP, then drop to `CLI(net)`.

### 3. In the Mininet CLI

#### a) Ping tests

```bash
mininet> h1 ping h5 -c 3
mininet> h2 ping h3 -c 3
mininet> h8 ping h1 -c 3
mininet> h6 ping h2 -c 3
```

#### b) iperf3 throughput tests

```bash
mininet> h1 iperf3 -s &
mininet> h6 iperf3 -c 10.1.1.2 -t 120

mininet> h8 iperf3 -s &
mininet> h2 iperf3 -c 10.0.0.9 -t 120

mininet> exit
```
## Q3: 
Compilation of the code is done using the following command:
```bash
cc distance_vector.c node0.c node1.c node2.c node3.c
```