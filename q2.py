#!/usr/bin/env python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.log import setLogLevel, info
import time, os
from mininet.node import OVSController, OVSSwitch, RemoteController

class CustomTopoNAT(Topo):
    def build(self):
        # Add switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')

        # Add hosts with specified IPs
        # Internal hosts 
        h1 = self.addHost('h1', ip='10.1.1.2/24')
        h2 = self.addHost('h2', ip='10.1.1.3/24')
        # External hosts 
        h3 = self.addHost('h3', ip='10.0.0.4/24')
        h4 = self.addHost('h4', ip='10.0.0.5/24')
        h5 = self.addHost('h5', ip='10.0.0.6/24')
        h6 = self.addHost('h6', ip='10.0.0.7/24')
        h7 = self.addHost('h7', ip='10.0.0.8/24')
        h8 = self.addHost('h8', ip='10.0.0.9/24')
        # NAT host H9
        h9 = self.addHost('h9', ip='172.16.10.10/24')

        # Host-to-switch links (5ms delay)
        self.addLink(h3, s2, delay='5ms')
        self.addLink(h4, s2, delay='5ms')
        self.addLink(h5, s3, delay='5ms')
        self.addLink(h6, s3, delay='5ms')
        self.addLink(h7, s4, delay='5ms')
        self.addLink(h8, s4, delay='5ms')

        self.addLink(h9, s1, delay='5ms')
        self.addLink(h1, h9, delay='5ms')
        self.addLink(h2, h9, delay='5ms')
        # Connect H9 to s1 (public interface)

        # Inter-switch links remain unchanged.
        self.addLink(s1, s2, delay='7ms')
        self.addLink(s2, s3, delay='7ms')
        self.addLink(s3, s4, delay='7ms')
        self.addLink(s4, s1, delay='7ms')
        self.addLink(s1, s3, delay='7ms')  # Diagonal link
    
def config_NAT(net):
    h1 = net.get('h1')
    h2 = net.get('h2')
    h9 = net.get('h9')

    # Explicit IP default route for external hosts
    h3= net.get('h3')
    h4= net.get('h4')
    h5= net.get('h5')
    h6= net.get('h6')
    h7= net.get('h7')
    h8= net.get('h8')
    h3.cmd('ip route add default via 10.0.0.1')
    h4.cmd('ip route add default via 10.0.0.1')
    h5.cmd('ip route add default via 10.0.0.1')
    h6.cmd('ip route add default via 10.0.0.1')
    h7.cmd('ip route add default via 10.0.0.1')
    h8.cmd('ip route add default via 10.0.0.1')

    h9.cmd("ip addr add 10.0.0.1/24 dev h9-eth0")  # gateway IP

    # Create a bridge on h9 to join internal interfaces
    h9.cmd("ip link add name br0 type bridge")
    h9.cmd("ip link set dev br0 up")

    # Set up the internal interfaces to the bridge
    h9.cmd("ip link set dev h9-eth1 master br0")
    h9.cmd("ip link set dev h9-eth2 master br0")


    # Internal IP
    h9.cmd("ip addr add 10.1.1.1/24 dev br0")  

    # Explicit IP default route for internal hosts
    h1.cmd("ip route add default via 10.1.1.1")
    h2.cmd("ip route add default via 10.1.1.1")

    # IP forwarding on h9
    h9.cmd("sysctl -w net.ipv4.ip_forward=1")

    # Setup NAT using iptables (masquerade)
    h9.cmd("iptables -t nat -F")
    h9.cmd("iptables -t nat -A POSTROUTING -s 10.1.1.0/24 -o h9-eth0 -j MASQUERADE")
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o br0 -m state --state RELATED,ESTABLISHED -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i br0 -o h9-eth0 -j ACCEPT")


def run():
    # os.system('mn -c')
    topo = CustomTopoNAT()
    # net = Mininet(topo=topo, controller=OVSController, link=TCLink, switch=OVSSwitch)
    # configure_nat_bridge(net)
    # net.start()
    c0 = RemoteController('c0', ip='127.0.0.1', port=6653)
    net = Mininet(topo=topo, link=TCLink, controller=c0, switch=OVSSwitch)
    config_NAT(net)

    net.start()

    for sw in net.switches:
        print(f"Configuring switch {sw.name}...")
        sw.cmd('ovs-vsctl set Bridge %s protocols=OpenFlow13' % sw.name)

    # for sw in [net.switches]:
    #     sw_obj = net.get(sw)
    #     sw_obj.cmd('ovs-vsctl set Bridge {} stp_enable=true'.format(sw))

    print("Waiting 40(Arbitrary) seconds for STP convergence")
    time.sleep(40)
    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    run()