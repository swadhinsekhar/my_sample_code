#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <event.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define BUFLEN 4096

int set_socket_non_blocking(int fd)
{
	int flags, s;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		printf("fcntl(): %d : %s\n", flags, strerror(errno));
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(fd, F_SETFL, flags);
	if (s == -1)
	{
		printf("fcntl(): %d : %s\n", s, strerror(errno));
		return -1;
	}

	return 0;
}

void netlink_handler(evutil_socket_t netlink_sock, short what, void *arg)
{
	char buf[BUFLEN];
	int msg_len;
	struct nlmsghdr *nl_msg;

	msg_len = recv(netlink_sock, buf, BUFLEN, 0);
	nl_msg = (struct nlmsghdr *) buf;
	for (; NLMSG_OK(nl_msg, msg_len); nl_msg=NLMSG_NEXT(nl_msg, msg_len)) {
		switch(nl_msg->nlmsg_type) {
			case RTM_NEWLINK: {
  				struct ifaddrmsg *ifa = NLMSG_DATA(nl_msg);
  				char ifname[1024];

					if_indextoname(ifa->ifa_index, ifname);
					printf("RTM_NEWLINK: add interface: %s\n", ifname);
				}
				break;
			case RTM_DELLINK: {
  				struct ifaddrmsg *ifa = NLMSG_DATA(nl_msg);
  				char ifname[1024];

					if_indextoname(ifa->ifa_index, ifname);
					printf("RTM_DELLINK: del interface: %s\n", ifname);
				}
				break;
			case NLMSG_DONE:
				printf("RTNETLINK: ignoring an NLM_DONE msg\n");
				break;
			default:
				printf("RTNETLINK: unknown msg of type %d -- ignoring\n",
						nl_msg->nlmsg_type);
		};
	}

	return;
}

int main()
{
	int err;
	struct sockaddr_nl saddr;
	struct event *ev;
  struct event_base *evbase;

	event_init();
  evbase = event_base_new();

	/* Create socket */
	int sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
	if(sock < 0)
		printf(" Create socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE) failed:\n");

	/* Make it non-blocking */
	err = set_socket_non_blocking(sock);
	if(err) {
		printf("Failed to set netlink sock non-blocking: %d\n", err);
		return -1;
	}

	/* Bind it to the sockaddr_nl PID=getpid() */
	bzero(&saddr, sizeof(saddr));
	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid = getpid();
	/* Register for rtnetlink notificatons*/
	saddr.nl_groups =
		RTMGRP_LINK |            /* interfaces add/delete events */
		RTMGRP_IPV4_ROUTE |      /* IPv4 routes */
		RTMGRP_NEIGH |           /* ARP entires */
		RTMGRP_IPV4_IFADDR;      /* IPv4 Address changes */
	if ((err = bind(sock, (struct sockaddr *) &saddr, sizeof(saddr))))
	{   
		printf("Failed to bind netlink sock to pid\n");
		return -1;
	}

	ev = event_new(evbase, sock, EV_READ|EV_PERSIST, netlink_handler, NULL);
  event_add(ev, NULL);
	printf("event base dispatch\n");
  event_base_dispatch(evbase);

	return 0;
}
