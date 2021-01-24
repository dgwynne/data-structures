/*	$OpenBSD: subr_tree.c,v 1.10 2018/10/09 08:28:43 dlg Exp $ */

/*
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 2016 David Gwynne <dlg@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "bst.h"

static inline struct bst_entry *
bst_n2e(const struct bst_type *t, void *node)
{
	unsigned long addr = (unsigned long)node;

	return ((struct bst_entry *)(addr + t->t_offset));
}

static inline void *
bst_e2n(const struct bst_type *t, struct bst_entry *e)
{
	unsigned long addr = (unsigned long)e;

	return ((void *)(addr - t->t_offset));
}

#define BST_CHILD(_bse, _c)	(_bse)->bst_children[(_c)]
#define BST_LEFT(_bse)		BST_CHILD((_bse), 0)
#define BST_RIGHT(_bse)		BST_CHILD((_bse), 1)
#define BST_PARENT(_bse)	(_bse)->bst_parent
#define BST_DATA(_bse)		(_bse)->bst_data

#define BST_ROOT(_bst)		(_bst)->bst_root

/* Finds the node with the same key as elm */
void *
_bst_find(const struct bst_type *t, struct bstree *bst, const void *key)
{
	struct bst_entry *tmp = BST_ROOT(bst);
	void *node;
	int comp;

	while (tmp != NULL) {
		node = bst_e2n(t, tmp);
		comp = (*t->t_compare)(key, node);
		if (comp < 0)
			tmp = BST_LEFT(tmp);
		else if (comp > 0)
			tmp = BST_RIGHT(tmp);
		else
			return (node);
	}

	return (NULL);
}

/* Finds the first node greater than or equal to the search key */
void *
_bst_nfind(const struct bst_type *t, struct bstree *bst, const void *key)
{
	struct bst_entry *tmp = BST_ROOT(bst);
	void *node;
	void *res = NULL;
	int comp;

	while (tmp != NULL) {
		node = bst_e2n(t, tmp);
		comp = (*t->t_compare)(key, node);
		if (comp < 0) {
			res = node;
			tmp = BST_LEFT(tmp);
		} else if (comp > 0)
			tmp = BST_RIGHT(tmp);
		else
			return (node);
	}

	return (res);
}

void *
_bst_next(const struct bst_type *t, void *elm)
{
	struct bst_entry *bste = bst_n2e(t, elm);

	if (BST_RIGHT(bste) != NULL) {
		bste = BST_RIGHT(bste);
		while (BST_LEFT(bste) != NULL)
			bste = BST_LEFT(bste);
	} else {
		if (BST_PARENT(bste) != NULL &&
		    (bste == BST_LEFT(BST_PARENT(bste))))
			bste = BST_PARENT(bste);
		else {
			while (BST_PARENT(bste) != NULL &&
			    (bste == BST_RIGHT(BST_PARENT(bste))))
				bste = BST_PARENT(bste);
			bste = BST_PARENT(bste);
		}
	}

	return (bste == NULL ? NULL : bst_e2n(t, bste));
}

void *
_bst_prev(const struct bst_type *t, void *elm)
{
	struct bst_entry *bste = bst_n2e(t, elm);

	if (BST_LEFT(bste) != NULL) {
		bste = BST_LEFT(bste);
		while (BST_RIGHT(bste) != NULL)
			bste = BST_RIGHT(bste);
	} else {
		if (BST_PARENT(bste) != NULL &&
		    (bste == BST_RIGHT(BST_PARENT(bste))))
			bste = BST_PARENT(bste);
		else {
			while (BST_PARENT(bste) != NULL &&
			    (bste == BST_LEFT(BST_PARENT(bste))))
				bste = BST_PARENT(bste);
			bste = BST_PARENT(bste);
		}
	}

	return (bste == NULL ? NULL : bst_e2n(t, bste));
}

void *
_bst_root(const struct bst_type *t, struct bstree *bst)
{
	struct bst_entry *bste = BST_ROOT(bst);

	return (bste == NULL ? NULL : bst_e2n(t, bste));
}

void *
_bst_min(const struct bst_type *t, struct bstree *bst)
{
	struct bst_entry *bste = BST_ROOT(bst);
	struct bst_entry *parent = NULL;

	while (bste != NULL) {
		parent = bste;
		bste = BST_LEFT(bste);
	}

	return (parent == NULL ? NULL : bst_e2n(t, parent));
}

void *
_bst_max(const struct bst_type *t, struct bstree *bst)
{
	struct bst_entry *bste = BST_ROOT(bst);
	struct bst_entry *parent = NULL;

	while (bste != NULL) {
		parent = bste;
		bste = BST_RIGHT(bste);
	}

	return (parent == NULL ? NULL : bst_e2n(t, parent));
}

void *
_bst_left(const struct bst_type *t, void *node)
{
	struct bst_entry *bste = bst_n2e(t, node);
	bste = BST_LEFT(bste);
	return (bste == NULL ? NULL : bst_e2n(t, bste));
}

void *
_bst_right(const struct bst_type *t, void *node)
{
	struct bst_entry *bste = bst_n2e(t, node);
	bste = BST_RIGHT(bste);
	return (bste == NULL ? NULL : bst_e2n(t, bste));
}

void *
_bst_parent(const struct bst_type *t, void *node)
{
	struct bst_entry *bste = bst_n2e(t, node);
	bste = BST_PARENT(bste);
	return (bste == NULL ? NULL : bst_e2n(t, bste));
}

void
_bst_set_left(const struct bst_type *t, void *node, void *left)
{
	struct bst_entry *bste = bst_n2e(t, node);
	struct bst_entry *bstl = (left == NULL) ? NULL : bst_n2e(t, left);

	BST_LEFT(bste) = bstl;
}

void
_bst_set_right(const struct bst_type *t, void *node, void *right)
{
	struct bst_entry *bste = bst_n2e(t, node);
	struct bst_entry *bstr = (right == NULL) ? NULL : bst_n2e(t, right);

	BST_RIGHT(bste) = bstr;
}

void
_bst_set_parent(const struct bst_type *t, void *node, void *parent)
{
	struct bst_entry *bste = bst_n2e(t, node);
	struct bst_entry *bstp = (parent == NULL) ? NULL : bst_n2e(t, parent);

	BST_PARENT(bste) = bstp;
}

void
_bst_poison(const struct bst_type *t, void *node, unsigned long poison)
{
	struct bst_entry *bste = bst_n2e(t, node);

	BST_PARENT(bste) = BST_LEFT(bste) = BST_RIGHT(bste) =
	    (struct bst_entry *)poison;
}

int
_bst_check(const struct bst_type *t, void *node, unsigned long poison)
{
	struct bst_entry *bste = bst_n2e(t, node);

	return ((unsigned long)BST_PARENT(bste) == poison &&
	    (unsigned long)BST_LEFT(bste) == poison &&
	    (unsigned long)BST_RIGHT(bste) == poison);
}

/*
 * Red-Black Trees
 */

static inline struct bst_entry *
rbt_n2e(const struct rbt_type *t, void *node)
{
	return bst_n2e(&t->t_bst, node);
}

static inline void *
rbt_e2n(const struct rbt_type *t, struct bst_entry *rbe)
{
	return bst_e2n(&t->t_bst, rbe);
}

#define RBE_BLACK		0
#define RBE_RED			1

#define RBE_CHILD(_rbe, _c)	BST_CHILD((_rbe), (_c))
#define RBE_LEFT(_rbe)		BST_LEFT(_rbe)
#define RBE_RIGHT(_rbe)		BST_RIGHT(_rbe)
#define RBE_PARENT(_rbe)	BST_PARENT(_rbe)
#define RBE_COLOR(_rbe)		BST_DATA(_rbe)

#define RBH_ROOT(_rbt)		BST_ROOT(_rbt)

static inline void
rbe_set(struct bst_entry *rbe, struct bst_entry *parent)
{
	RBE_PARENT(rbe) = parent;
	RBE_LEFT(rbe) = RBE_RIGHT(rbe) = NULL;
	RBE_COLOR(rbe) = RBE_RED;
}

static inline void
rbe_set_blackred(struct bst_entry *black, struct bst_entry *red)
{
	RBE_COLOR(black) = RBE_BLACK;
	RBE_COLOR(red) = RBE_RED;
}

static inline void
rbe_augment(const struct rbt_type *t, struct bst_entry *rbe)
{
	(*t->t_augment)(rbt_e2n(t, rbe));
}

static inline void
rbe_if_augment(const struct rbt_type *t, struct bst_entry *rbe)
{
	if (t->t_augment != NULL)
		rbe_augment(t, rbe);
}

static inline void
rbe_rotate_left(const struct rbt_type *t, struct bstree *rbt,
    struct bst_entry *rbe)
{
	struct bst_entry *parent;
	struct bst_entry *tmp;

	tmp = RBE_RIGHT(rbe);
	RBE_RIGHT(rbe) = RBE_LEFT(tmp);
	if (RBE_RIGHT(rbe) != NULL)
		RBE_PARENT(RBE_LEFT(tmp)) = rbe;

	parent = RBE_PARENT(rbe);
	RBE_PARENT(tmp) = parent;
	if (parent != NULL) {
		if (rbe == RBE_LEFT(parent))
			RBE_LEFT(parent) = tmp;
		else
			RBE_RIGHT(parent) = tmp;
	} else
		RBH_ROOT(rbt) = tmp;

	RBE_LEFT(tmp) = rbe;
	RBE_PARENT(rbe) = tmp;

	if (t->t_augment != NULL) {
		rbe_augment(t, rbe);
		rbe_augment(t, tmp);
		parent = RBE_PARENT(tmp);
		if (parent != NULL)
			rbe_augment(t, parent);
	}
}

static inline void
rbe_rotate_right(const struct rbt_type *t, struct bstree *rbt,
    struct bst_entry *rbe)
{
	struct bst_entry *parent;
	struct bst_entry *tmp;

	tmp = RBE_LEFT(rbe);
	RBE_LEFT(rbe) = RBE_RIGHT(tmp);
	if (RBE_LEFT(rbe) != NULL)
		RBE_PARENT(RBE_RIGHT(tmp)) = rbe;

	parent = RBE_PARENT(rbe);
	RBE_PARENT(tmp) = parent;
	if (parent != NULL) {
		if (rbe == RBE_LEFT(parent))
			RBE_LEFT(parent) = tmp;
		else
			RBE_RIGHT(parent) = tmp;
	} else
		RBH_ROOT(rbt) = tmp;

	RBE_RIGHT(tmp) = rbe;
	RBE_PARENT(rbe) = tmp;

	if (t->t_augment != NULL) {
		rbe_augment(t, rbe);
		rbe_augment(t, tmp);
		parent = RBE_PARENT(tmp);
		if (parent != NULL)
			rbe_augment(t, parent);
	}
}

static inline void
rbe_insert_color(const struct rbt_type *t, struct bstree *rbt,
    struct bst_entry *rbe)
{
	struct bst_entry *parent, *gparent, *tmp;

	while ((parent = RBE_PARENT(rbe)) != NULL &&
	    RBE_COLOR(parent) == RBE_RED) {
		gparent = RBE_PARENT(parent);

		if (parent == RBE_LEFT(gparent)) {
			tmp = RBE_RIGHT(gparent);
			if (tmp != NULL && RBE_COLOR(tmp) == RBE_RED) {
				RBE_COLOR(tmp) = RBE_BLACK;
				rbe_set_blackred(parent, gparent);
				rbe = gparent;
				continue;
			}

			if (RBE_RIGHT(parent) == rbe) {
				rbe_rotate_left(t, rbt, parent);
				tmp = parent;
				parent = rbe;
				rbe = tmp;
			}

			rbe_set_blackred(parent, gparent);
			rbe_rotate_right(t, rbt, gparent);
		} else {
			tmp = RBE_LEFT(gparent);
			if (tmp != NULL && RBE_COLOR(tmp) == RBE_RED) {
				RBE_COLOR(tmp) = RBE_BLACK;
				rbe_set_blackred(parent, gparent);
				rbe = gparent;
				continue;
			}

			if (RBE_LEFT(parent) == rbe) {
				rbe_rotate_right(t, rbt, parent);
				tmp = parent;
				parent = rbe;
				rbe = tmp;
			}

			rbe_set_blackred(parent, gparent);
			rbe_rotate_left(t, rbt, gparent);
		}
	}

	RBE_COLOR(RBH_ROOT(rbt)) = RBE_BLACK;
}

static inline void
rbe_remove_color(const struct rbt_type *t, struct bstree *rbt,
    struct bst_entry *parent, struct bst_entry *rbe)
{
	struct bst_entry *tmp;

	while ((rbe == NULL || RBE_COLOR(rbe) == RBE_BLACK) &&
	    rbe != RBH_ROOT(rbt)) {
		if (RBE_LEFT(parent) == rbe) {
			tmp = RBE_RIGHT(parent);
			if (RBE_COLOR(tmp) == RBE_RED) {
				rbe_set_blackred(tmp, parent);
				rbe_rotate_left(t, rbt, parent);
				tmp = RBE_RIGHT(parent);
			}
			if ((RBE_LEFT(tmp) == NULL ||
			     RBE_COLOR(RBE_LEFT(tmp)) == RBE_BLACK) &&
			    (RBE_RIGHT(tmp) == NULL ||
			     RBE_COLOR(RBE_RIGHT(tmp)) == RBE_BLACK)) {
				RBE_COLOR(tmp) = RBE_RED;
				rbe = parent;
				parent = RBE_PARENT(rbe);
			} else {
				if (RBE_RIGHT(tmp) == NULL ||
				    RBE_COLOR(RBE_RIGHT(tmp)) == RBE_BLACK) {
					struct bst_entry *oleft;

					oleft = RBE_LEFT(tmp);
					if (oleft != NULL)
						RBE_COLOR(oleft) = RBE_BLACK;

					RBE_COLOR(tmp) = RBE_RED;
					rbe_rotate_right(t, rbt, tmp);
					tmp = RBE_RIGHT(parent);
				}

				RBE_COLOR(tmp) = RBE_COLOR(parent);
				RBE_COLOR(parent) = RBE_BLACK;
				if (RBE_RIGHT(tmp))
					RBE_COLOR(RBE_RIGHT(tmp)) = RBE_BLACK;

				rbe_rotate_left(t, rbt, parent);
				rbe = RBH_ROOT(rbt);
				break;
			}
		} else {
			tmp = RBE_LEFT(parent);
			if (RBE_COLOR(tmp) == RBE_RED) {
				rbe_set_blackred(tmp, parent);
				rbe_rotate_right(t, rbt, parent);
				tmp = RBE_LEFT(parent);
			}

			if ((RBE_LEFT(tmp) == NULL ||
			     RBE_COLOR(RBE_LEFT(tmp)) == RBE_BLACK) &&
			    (RBE_RIGHT(tmp) == NULL ||
			     RBE_COLOR(RBE_RIGHT(tmp)) == RBE_BLACK)) {
				RBE_COLOR(tmp) = RBE_RED;
				rbe = parent;
				parent = RBE_PARENT(rbe);
			} else {
				if (RBE_LEFT(tmp) == NULL ||
				    RBE_COLOR(RBE_LEFT(tmp)) == RBE_BLACK) {
					struct bst_entry *oright;

					oright = RBE_RIGHT(tmp);
					if (oright != NULL)
						RBE_COLOR(oright) = RBE_BLACK;

					RBE_COLOR(tmp) = RBE_RED;
					rbe_rotate_left(t, rbt, tmp);
					tmp = RBE_LEFT(parent);
				}

				RBE_COLOR(tmp) = RBE_COLOR(parent);
				RBE_COLOR(parent) = RBE_BLACK;
				if (RBE_LEFT(tmp) != NULL)
					RBE_COLOR(RBE_LEFT(tmp)) = RBE_BLACK;

				rbe_rotate_right(t, rbt, parent);
				rbe = RBH_ROOT(rbt);
				break;
			}
		}
	}

	if (rbe != NULL)
		RBE_COLOR(rbe) = RBE_BLACK;
}

static inline struct bst_entry *
rbe_remove(const struct rbt_type *t, struct bstree *rbt, struct bst_entry *rbe)
{
	struct bst_entry *child, *parent, *old = rbe;
	unsigned int color;

	if (RBE_LEFT(rbe) == NULL)
		child = RBE_RIGHT(rbe);
	else if (RBE_RIGHT(rbe) == NULL)
		child = RBE_LEFT(rbe);
	else {
		struct bst_entry *tmp;

		rbe = RBE_RIGHT(rbe);
		while ((tmp = RBE_LEFT(rbe)) != NULL)
			rbe = tmp;

		child = RBE_RIGHT(rbe);
		parent = RBE_PARENT(rbe);
		color = RBE_COLOR(rbe);
		if (child != NULL)
			RBE_PARENT(child) = parent;
		if (parent != NULL) {
			if (RBE_LEFT(parent) == rbe)
				RBE_LEFT(parent) = child;
			else
				RBE_RIGHT(parent) = child;

			rbe_if_augment(t, parent);
		} else
			RBH_ROOT(rbt) = child;
		if (RBE_PARENT(rbe) == old)
			parent = rbe;
		*rbe = *old;

		tmp = RBE_PARENT(old);
		if (tmp != NULL) {
			if (RBE_LEFT(tmp) == old)
				RBE_LEFT(tmp) = rbe;
			else
				RBE_RIGHT(tmp) = rbe;

			rbe_if_augment(t, tmp);
		} else
			RBH_ROOT(rbt) = rbe;

		RBE_PARENT(RBE_LEFT(old)) = rbe;
		if (RBE_RIGHT(old))
			RBE_PARENT(RBE_RIGHT(old)) = rbe;

		if (t->t_augment != NULL && parent != NULL) {
			tmp = parent;
			do {
				rbe_augment(t, tmp);
				tmp = RBE_PARENT(tmp);
			} while (tmp != NULL);
		}

		goto color;
	}

	parent = RBE_PARENT(rbe);
	color = RBE_COLOR(rbe);

	if (child != NULL)
		RBE_PARENT(child) = parent;
	if (parent != NULL) {
		if (RBE_LEFT(parent) == rbe)
			RBE_LEFT(parent) = child;
		else
			RBE_RIGHT(parent) = child;

		rbe_if_augment(t, parent);
	} else
		RBH_ROOT(rbt) = child;
color:
	if (color == RBE_BLACK)
		rbe_remove_color(t, rbt, parent, child);

	return (old);
}

void *
_rbt_remove(const struct rbt_type *t, struct bstree *rbt, void *elm)
{
	struct bst_entry *rbe = rbt_n2e(t, elm);
	struct bst_entry *old;

	old = rbe_remove(t, rbt, rbe);

	return (old == NULL ? NULL : rbt_e2n(t, old));
}

void *
_rbt_insert(const struct rbt_type *t, struct bstree *rbt, void *elm)
{
	struct bst_entry *rbe = rbt_n2e(t, elm);
	struct bst_entry *tmp;
	struct bst_entry *parent = NULL;
	void *node;
	int comp = 0;

	tmp = RBH_ROOT(rbt);
	while (tmp != NULL) {
		parent = tmp;

		node = rbt_e2n(t, tmp);
		comp = (*t->t_bst.t_compare)(elm, node);
		if (comp == 0)
 			return (node);
		comp = comp > 0;
		tmp = RBE_CHILD(tmp, comp);
	}

	rbe_set(rbe, parent);

	if (parent != NULL) {
		RBE_CHILD(parent, comp) = rbe;
		rbe_if_augment(t, parent);
	} else
		RBH_ROOT(rbt) = rbe;

	rbe_insert_color(t, rbt, rbe);

	return (NULL);
}

/*
 * AVL Trees
 */

#define avl_n2e(_t, _elm)	bst_n2e((_t), (_elm))
#define avl_e2n(_t, _avle)	bst_e2n((_t), (_avle))

#define AVLT_ROOT(_avlt)	BST_ROOT(_avlt)

#define AVLE_CHILD(_avle, _c)	BST_CHILD((_avle), (_c))
#define AVLE_LEFT(_avle)	BST_LEFT(_avle)
#define AVLE_RIGHT(_avle)	BST_RIGHT(_avle)
#define AVLE_PARENT(_avle)	BST_PARENT(_avle)
#define AVLE_BALANCE(_avle)	BST_DATA(_avle)

static const int avl_balances[2] = { -1, 1 };

/*
 * rotate "left"
 *
 *     /             /
 *    B             D
 *   / \           / \
 *  A   D   ==>   B   E
 *     / \       / \
 *    C   E     A   C
 */

static inline struct bst_entry *
avl_rotate(struct bst_entry *avle, int d)
{
	struct bst_entry *child, *gchild;
 
	child = AVLE_CHILD(avle, !d);
	gchild = AVLE_CHILD(child, d);

	AVLE_CHILD(child, d) = avle;
	AVLE_PARENT(avle) = child;

	AVLE_CHILD(avle, !d) = gchild;
	if (gchild != NULL)
		AVLE_PARENT(gchild) = avle;

	return (child);
}

static inline int
avl_rebalance(struct bstree *avlt, struct bst_entry *parent,
    struct bst_entry **avlep, int balance)
{
	struct bst_entry *avle = *avlep;
	struct bst_entry *child;
	int cbalance;
	int diff;
	int d;

	d = balance > 0;
	child = AVLE_CHILD(avle, d);

	cbalance = AVLE_BALANCE(child);
	if (avl_balances[!d] == cbalance) {
		struct bst_entry *gchild;

		gchild = AVLE_CHILD(child, !d);

		cbalance = AVLE_BALANCE(gchild);
		AVLE_BALANCE(avle) =
		    (cbalance == avl_balances[d]) ? avl_balances[!d] : 0;
		AVLE_BALANCE(child) =
		    (cbalance == avl_balances[!d]) ? avl_balances[d] : 0;
		AVLE_BALANCE(gchild) = 0;

		child = avl_rotate(child, d);
		AVLE_CHILD(avle, d) = child;
		AVLE_PARENT(child) = avle;

		diff = 1; /* tree is always shorter after double rotation */
	} else {
		cbalance += avl_balances[!d];

		AVLE_BALANCE(child) = cbalance;
		AVLE_BALANCE(avle) = -cbalance;

		diff = (cbalance == 0) ? 1 : 0;
 	}
 
	child = avl_rotate(avle, !d);
	if (parent != NULL) {
		d = AVLE_RIGHT(parent) == avle;
		AVLE_CHILD(parent, d) = child;
	} else
		AVLT_ROOT(avlt) = child;
	AVLE_PARENT(child) = parent;

	*avlep = child;
	return (diff);
}

void *
_avl_insert(const struct bst_type *t, struct bstree *avlt, void *elm)
{
	struct bst_entry *avle = avl_n2e(t, elm);
	struct bst_entry *tmp;
	struct bst_entry *parent = NULL;
 	void *node;
	int comp = 0;
 
	tmp = AVLT_ROOT(avlt);
 	while (tmp != NULL) {
		parent = tmp;

		node = avl_e2n(t, tmp);
		comp = (*t->t_compare)(elm, node);
		if (comp == 0)
 			return (node);
 
		comp = (comp > 0);
		tmp = AVLE_CHILD(tmp, comp);
	}
 
	AVLE_PARENT(avle) = parent;
	AVLE_LEFT(avle) = AVLE_RIGHT(avle) = NULL;
	AVLE_BALANCE(avle) = 0;
 
	if (parent == NULL) {
		AVLT_ROOT(avlt) = avle;
		return (NULL);
 	}
 
	AVLE_CHILD(parent, comp) = avle;
 
	for (;;) {
		int obalance, nbalance;
 
		avle = parent;

		obalance = AVLE_BALANCE(avle);
		nbalance = obalance + avl_balances[comp];

		/* the balance has gone from a lean to equality */
		if (nbalance == 0) {
			AVLE_BALANCE(avle) = 0;
			break;
		}

		parent = AVLE_PARENT(avle);

		/* an existing lean has increased, so rebalance */
		if (obalance != 0) {
			avl_rebalance(avlt, parent, &avle, nbalance);
			break;
		}

		AVLE_BALANCE(avle) = nbalance;

		if (parent == NULL)
			break;

		comp = AVLE_RIGHT(parent) == avle;
	}

	return (NULL);
}

void *
_avl_remove(const struct bst_type *t, struct bstree *avlt, void *elm)
{
	struct bst_entry *avle = avl_n2e(t, elm);
	struct bst_entry *parent;
	struct bst_entry *next;
	struct bst_entry sentinel;
	int comp;

	parent = AVLE_PARENT(avle);

	if (AVLE_LEFT(avle) != NULL && AVLE_RIGHT(avle) != NULL) {
		/* two children exist, so shuffle tree around */
		struct bst_entry *nparent = parent;

		/* find the next (well, previous) ordered node */
		next = AVLE_LEFT(avle);
		if (AVLE_RIGHT(next) == NULL) {
			/* avle is going to swap with its immediate child */

			parent = next; /* so next will be the avle parent */
			AVLE_PARENT(next) = parent;

			AVLE_LEFT(avle) = &sentinel;
		} else {
			do {
				parent = next;
				next = AVLE_RIGHT(next);
			} while (AVLE_RIGHT(next) != NULL);

			AVLE_RIGHT(parent) = &sentinel;
		}

		/* swap the target entry with the next entry */
		sentinel = *next;
		*next = *avle;

		/* patch the next node into the surrounding tree */
		AVLE_PARENT(AVLE_LEFT(avle)) = next;
		AVLE_PARENT(AVLE_RIGHT(avle)) = next;
		if (nparent != NULL) {
			comp = AVLE_RIGHT(nparent) == avle;
			AVLE_CHILD(nparent, comp) = next;
		} else
			AVLT_ROOT(avlt) = next;

		avle = &sentinel;
	}

	next = AVLE_LEFT(avle);
	if (next == NULL)
		next = AVLE_RIGHT(avle);

	if (next != NULL)
		AVLE_PARENT(next) = parent;

	if (parent == NULL) {
		AVLT_ROOT(avlt) = next;
		return (elm);
	}

	comp = AVLE_RIGHT(parent) == avle;
	AVLE_CHILD(parent, comp) = next;

	for (;;) {
		int obalance, nbalance;

		avle = parent;

		obalance = AVLE_BALANCE(avle);
		nbalance = obalance - avl_balances[comp];

		if (obalance == 0) {
			AVLE_BALANCE(avle) = nbalance;
			break;
		}

		parent = AVLE_PARENT(avle);

		if (nbalance == 0)
			AVLE_BALANCE(avle) = 0;
		else if (avl_rebalance(avlt, parent, &avle, nbalance) == 0)
			break;

		if (parent == NULL)
			break;

		comp = AVLE_RIGHT(parent) == avle;
	}

	return (elm);
}
