/**
 * \brief  BSD style socket helpers
 * \author Sebastian Sumpf
 * \date   2013-08-28
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */
#include <linux/net.h>
#include <net/sock.h>
#include <net/tcp_states.h>

#include <lx_emul.h>

static const struct net_proto_family *net_families[NPROTO];


int sock_register(const struct net_proto_family *ops)
{
	if (ops->family >= NPROTO) {
		printk("protocol %d >=  NPROTO (%d)\n", ops->family, NPROTO);
		return -ENOBUFS;
	}

	net_families[ops->family] = ops;
	pr_info("NET: Registered protocol family %d\n", ops->family);
	return 0;
}


struct socket *sock_alloc(void)
{
	struct socket *sock = (struct socket *)kzalloc(sizeof(struct socket), 0);

	/*
	 * Linux normally allocates the socket_wq when calling
	 * sock_alloc_inode() while we do it here hoping for the best.
	 */
	sock->wq = (struct socket *)kzalloc(sizeof(*sock->wq), 0);
	if (!sock->wq) {
		kfree(sock);
		return NULL;
	}

	return sock;
}


int sock_create_lite(int family, int type, int protocol, struct socket **res)

{
	struct socket *sock = sock_alloc();

	if (!sock)
		return -ENOMEM;

	sock->type = type;
	*res = sock;
	return 0;
}


int sock_create_kern(struct net *net, int family, int type, int proto,
                     struct socket **res)
{
	struct socket *sock;
	const struct net_proto_family *pf;
	int err;

	if (family < 0 || family > NPROTO)
		return -EAFNOSUPPORT;

	if (type < 0 || type > SOCK_MAX)
		return -EINVAL;

	pf = net_families[family];

	if (!pf) {
		printk("No protocol found for family %d\n", family);
		return -ENOPROTOOPT;
	}

	sock = sock_alloc();
	if (!sock) {
		printk("Could not allocate socket\n");
		return -ENFILE;
	}

	sock->type = type;

	err = pf->create(&init_net, sock, proto, 1);
	if (err) {
		kfree(sock);
		return err;
	}

	*res = sock;

	return 0;
}


int socket_check_state(struct socket *socket)
{
	if (socket->sk->sk_state == TCP_CLOSE_WAIT)
		return -EINTR;

	return 0;
}


void log_sock(struct socket *socket)
{
	printk("\nNEW socket %p sk %p fsk %x &sk %p &fsk %p\n\n",
	       socket, socket->sk, socket->flags, &socket->sk, &socket->flags);
}


static void sock_init(void)
{
	skb_init();
}

core_initcall(sock_init);
