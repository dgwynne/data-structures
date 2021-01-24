/*	$OpenBSD: tree.h,v 1.30 2020/10/10 18:03:41 otto Exp $	*/
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

#ifndef	_BST_H_

#include <sys/_null.h>
#include <stddef.h>

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

struct bst_type {
	int		(*t_compare)(const void *, const void *);
	unsigned int	  t_offset;	/* offset of rb_entry in type */
};

struct bst_entry {
	struct bst_entry *bst_parent;
	struct bst_entry *bst_children[2];
	unsigned int	  bst_data;
};

struct bstree {
	struct bst_entry *bst_root;
};

#define BST_INITIALIZER()	{ NULL }

static inline void
_bst_init(struct bstree *bst)
{
	bst->bst_root = NULL;
}

static inline int
_bst_empty(struct bstree *bst)
{
	return (bst->bst_root == NULL);
}

void	*_bst_insert(const struct bst_type *, struct bstree *, void *);
void	*_bst_remove(const struct bst_type *, struct bstree *, void *);
void	*_bst_find(const struct bst_type *, struct bstree *, const void *);
void	*_bst_nfind(const struct bst_type *, struct bstree *, const void *);
void	*_bst_root(const struct bst_type *, struct bstree *);
void	*_bst_min(const struct bst_type *, struct bstree *);
void	*_bst_max(const struct bst_type *, struct bstree *);
void	*_bst_next(const struct bst_type *, void *);
void	*_bst_prev(const struct bst_type *, void *);
void	*_bst_left(const struct bst_type *, void *);
void	*_bst_right(const struct bst_type *, void *);
void	*_bst_parent(const struct bst_type *, void *);
void	 _bst_set_left(const struct bst_type *, void *, void *);
void	 _bst_set_right(const struct bst_type *, void *, void *);
void	 _bst_set_parent(const struct bst_type *, void *, void *);
void	 _bst_poison(const struct bst_type *, void *, unsigned long);
int	 _bst_check(const struct bst_type *, void *, unsigned long);

/*
 * red-black tree
 */

struct rbt_type {
	void		(*t_augment)(void *);
	struct bst_type	  t_bst;
};

#define RBT_HEAD(_name, _type)						\
struct _name {								\
	struct bstree	  rb_tree;					\
}

#define RBT_ENTRY(_type)	struct bst_entry

#define RBT_INITIALIZER(_head) { BST_INITIALIZER() }

void	*_rbt_insert(const struct rbt_type *, struct bstree *, void *);
void	*_rbt_remove(const struct rbt_type *, struct bstree *, void *);

#define RBT_PROTOTYPE(_name, _type, _field, _cmp)			\
extern const struct rbt_type _name##_RBT_TYPE;				\
									\
__unused static inline void						\
_name##_RBT_INIT(struct _name *head)					\
{									\
	_bst_init(&head->rb_tree);					\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_INSERT(struct _name *head, struct _type *elm)		\
{									\
	return _rbt_insert(&_name##_RBT_TYPE, &head->rb_tree, elm);	\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_REMOVE(struct _name *head, struct _type *elm)		\
{									\
	return _rbt_remove(&_name##_RBT_TYPE, &head->rb_tree, elm);	\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_FIND(struct _name *head, const struct _type *key)		\
{									\
	return _bst_find(&_name##_RBT_TYPE.t_bst,			\
	    &head->rb_tree, key);					\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_NFIND(struct _name *head, const struct _type *key)		\
{									\
	return _bst_nfind(&_name##_RBT_TYPE.t_bst,			\
	    &head->rb_tree, key);					\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_ROOT(struct _name *head)					\
{									\
	return _bst_root(&_name##_RBT_TYPE.t_bst, &head->rb_tree);	\
}									\
									\
__unused static inline int						\
_name##_RBT_EMPTY(struct _name *head)					\
{									\
	return _bst_empty(&head->rb_tree);				\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_MIN(struct _name *head)					\
{									\
	return _bst_min(&_name##_RBT_TYPE.t_bst, &head->rb_tree);	\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_MAX(struct _name *head)					\
{									\
	return _bst_max(&_name##_RBT_TYPE.t_bst, &head->rb_tree);	\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_NEXT(struct _type *elm)					\
{									\
	return _bst_next(&_name##_RBT_TYPE.t_bst, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_PREV(struct _type *elm)					\
{									\
	return _bst_prev(&_name##_RBT_TYPE.t_bst, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_LEFT(struct _type *elm)					\
{									\
	return _bst_left(&_name##_RBT_TYPE.t_bst, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_RIGHT(struct _type *elm)					\
{									\
	return _bst_right(&_name##_RBT_TYPE.t_bst, elm);		\
}									\
									\
__unused static inline struct _type *					\
_name##_RBT_PARENT(struct _type *elm)					\
{									\
	return _bst_parent(&_name##_RBT_TYPE.t_bst, elm);		\
}									\
									\
__unused static inline void						\
_name##_RBT_SET_LEFT(struct _type *elm, struct _type *left)		\
{									\
	_bst_set_left(&_name##_RBT_TYPE.t_bst, elm, left);		\
}									\
									\
__unused static inline void						\
_name##_RBT_SET_RIGHT(struct _type *elm, struct _type *right)		\
{									\
	_bst_set_right(&_name##_RBT_TYPE.t_bst, elm, right);		\
}									\
									\
__unused static inline void						\
_name##_RBT_SET_PARENT(struct _type *elm, struct _type *parent)		\
{									\
	_bst_set_parent(&_name##_RBT_TYPE.t_bst, elm, parent);		\
}									\
									\
__unused static inline void						\
_name##_RBT_POISON(struct _type *elm, unsigned long poison)		\
{									\
	_bst_poison(&_name##_RBT_TYPE.t_bst, elm, poison);		\
}									\
									\
__unused static inline int						\
_name##_RBT_CHECK(struct _type *elm, unsigned long poison)		\
{									\
	return _bst_check(&_name##_RBT_TYPE.t_bst, elm, poison);	\
}

#define RBT_GENERATE_INTERNAL(_name, _type, _field, _cmp, _aug)		\
static int								\
_name##_RBT_COMPARE(const void *lptr, const void *rptr)			\
{									\
	const struct _type *l = lptr, *r = rptr;			\
	return _cmp(l, r);						\
}									\
const struct rbt_type _name##_RBT_TYPE = {				\
	_aug,								\
	{								\
		_name##_RBT_COMPARE,					\
		offsetof(struct _type, _field),				\
	},								\
}

#define RBT_GENERATE_AUGMENT(_name, _type, _field, _cmp, _aug)		\
static void								\
_name##_RBT_AUGMENT(void *ptr)						\
{									\
	struct _type *p = ptr;						\
	return _aug(p);							\
}									\
RBT_GENERATE_INTERNAL(_name, _type, _field, _cmp, _name##_RBT_AUGMENT)

#define RBT_GENERATE(_name, _type, _field, _cmp)			\
    RBT_GENERATE_INTERNAL(_name, _type, _field, _cmp, NULL)

#define RBT_INIT(_name, _head)		_name##_RBT_INIT(_head)
#define RBT_INSERT(_name, _head, _elm)	_name##_RBT_INSERT(_head, _elm)
#define RBT_REMOVE(_name, _head, _elm)	_name##_RBT_REMOVE(_head, _elm)
#define RBT_FIND(_name, _head, _key)	_name##_RBT_FIND(_head, _key)
#define RBT_NFIND(_name, _head, _key)	_name##_RBT_NFIND(_head, _key)
#define RBT_ROOT(_name, _head)		_name##_RBT_ROOT(_head)
#define RBT_EMPTY(_name, _head)		_name##_RBT_EMPTY(_head)
#define RBT_MIN(_name, _head)		_name##_RBT_MIN(_head)
#define RBT_MAX(_name, _head)		_name##_RBT_MAX(_head)
#define RBT_NEXT(_name, _elm)		_name##_RBT_NEXT(_elm)
#define RBT_PREV(_name, _elm)		_name##_RBT_PREV(_elm)
#define RBT_LEFT(_name, _elm)		_name##_RBT_LEFT(_elm)
#define RBT_RIGHT(_name, _elm)		_name##_RBT_RIGHT(_elm)
#define RBT_PARENT(_name, _elm)		_name##_RBT_PARENT(_elm)
#define RBT_SET_LEFT(_name, _elm, _l)	_name##_RBT_SET_LEFT(_elm, _l)
#define RBT_SET_RIGHT(_name, _elm, _r)	_name##_RBT_SET_RIGHT(_elm, _r)
#define RBT_SET_PARENT(_name, _elm, _p)	_name##_RBT_SET_PARENT(_elm, _p)
#define RBT_POISON(_name, _elm, _p)	_name##_RBT_POISON(_elm, _p)
#define RBT_CHECK(_name, _elm, _p)	_name##_RBT_CHECK(_elm, _p)

#define RBT_FOREACH(_e, _name, _head)					\
	for ((_e) = RBT_MIN(_name, (_head));				\
	     (_e) != NULL;						\
	     (_e) = RBT_NEXT(_name, (_e)))

#define RBT_FOREACH_SAFE(_e, _name, _head, _n)				\
	for ((_e) = RBT_MIN(_name, (_head));				\
	     (_e) != NULL && ((_n) = RBT_NEXT(_name, (_e)), 1);	\
	     (_e) = (_n))

#define RBT_FOREACH_REVERSE(_e, _name, _head)				\
	for ((_e) = RBT_MAX(_name, (_head));				\
	     (_e) != NULL;						\
	     (_e) = RBT_PREV(_name, (_e)))

#define RBT_FOREACH_REVERSE_SAFE(_e, _name, _head, _n)			\
	for ((_e) = RBT_MAX(_name, (_head));				\
	     (_e) != NULL && ((_n) = RBT_PREV(_name, (_e)), 1);	\
	     (_e) = (_n))

/*
 * AVL tree
 */

#define AVL_HEAD(_name, _type)						\
struct _name {								\
	struct bstree	 avl_tree;					\
}

#define AVL_ENTRY(_type)	struct bst_entry

#define AVL_INITIALIZER(_head)	{ BST_INITIALIZER() }

void	*_avl_insert(const struct bst_type *, struct bstree *, void *);
void	*_avl_remove(const struct bst_type *, struct bstree *, void *);

#define AVL_PROTOTYPE(_name, _type, _field, _cmp)			\
extern const struct bst_type _name##_AVL_TYPE;				\
									\
__unused static inline void						\
_name##_AVL_INIT(struct _name *head)					\
{									\
	_bst_init(&head->avl_tree);					\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_INSERT(struct _name *head, struct _type *elm)		\
{									\
	return _avl_insert(&_name##_AVL_TYPE, &head->avl_tree, elm);	\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_REMOVE(struct _name *head, struct _type *elm)		\
{									\
	return _avl_remove(&_name##_AVL_TYPE, &head->avl_tree, elm);	\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_FIND(struct _name *head, const struct _type *key)		\
{									\
	return _bst_find(&_name##_AVL_TYPE, &head->avl_tree, key);	\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_NFIND(struct _name *head, const struct _type *key)		\
{									\
	return _bst_nfind(&_name##_AVL_TYPE, &head->avl_tree, key);	\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_ROOT(struct _name *head)					\
{									\
	return _bst_root(&_name##_AVL_TYPE, &head->avl_tree);		\
}									\
									\
__unused static inline int						\
_name##_AVL_EMPTY(struct _name *head)					\
{									\
	return _bst_empty(&head->avl_tree);				\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_MIN(struct _name *head)					\
{									\
	return _bst_min(&_name##_AVL_TYPE, &head->avl_tree);		\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_MAX(struct _name *head)					\
{									\
	return _bst_max(&_name##_AVL_TYPE, &head->avl_tree);		\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_NEXT(struct _type *elm)					\
{									\
	return _bst_next(&_name##_AVL_TYPE, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_PREV(struct _type *elm)					\
{									\
	return _bst_prev(&_name##_AVL_TYPE, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_LEFT(struct _type *elm)					\
{									\
	return _bst_left(&_name##_AVL_TYPE, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_RIGHT(struct _type *elm)					\
{									\
	return _bst_right(&_name##_AVL_TYPE, elm);			\
}									\
									\
__unused static inline struct _type *					\
_name##_AVL_PARENT(struct _type *elm)					\
{									\
	return _bst_parent(&_name##_AVL_TYPE, elm);			\
}									\
									\
__unused static inline void						\
_name##_AVL_POISON(struct _type *elm, unsigned long poison)		\
{									\
	return _bst_poison(&_name##_AVL_TYPE, elm, poison);		\
}									\
									\
__unused static inline int						\
_name##_AVL_CHECK(struct _type *elm, unsigned long poison)		\
{									\
	return _bst_check(&_name##_AVL_TYPE, elm, poison);		\
}

#define AVL_GENERATE(_name, _type, _field, _cmp)			\
static int								\
_name##_AVL_COMPARE(const void *lptr, const void *rptr)			\
{									\
	const struct _type *l = lptr, *r = rptr;			\
	return _cmp(l, r);						\
}									\
const struct bst_type _name##_AVL_TYPE = {				\
	_name##_AVL_COMPARE,						\
	offsetof(struct _type, _field),					\
}

#define AVL_INIT(_name, _head)		_name##_AVL_INIT(_head)
#define AVL_INSERT(_name, _head, _elm)	_name##_AVL_INSERT(_head, _elm)
#define AVL_REMOVE(_name, _head, _elm)	_name##_AVL_REMOVE(_head, _elm)
#define AVL_FIND(_name, _head, _key)	_name##_AVL_FIND(_head, _key)
#define AVL_NFIND(_name, _head, _key)	_name##_AVL_NFIND(_head, _key)
#define AVL_ROOT(_name, _head)		_name##_AVL_ROOT(_head)
#define AVL_EMPTY(_name, _head)		_name##_AVL_EMPTY(_head)
#define AVL_MIN(_name, _head)		_name##_AVL_MIN(_head)
#define AVL_MAX(_name, _head)		_name##_AVL_MAX(_head)
#define AVL_NEXT(_name, _elm)		_name##_AVL_NEXT(_elm)
#define AVL_PREV(_name, _elm)		_name##_AVL_PREV(_elm)
#define AVL_LEFT(_name, _elm)		_name##_AVL_LEFT(_elm)
#define AVL_RIGHT(_name, _elm)		_name##_AVL_RIGHT(_elm)
#define AVL_PARENT(_name, _elm)		_name##_AVL_PARENT(_elm)
#define AVL_POISON(_name, _elm, _p)	_name##_AVL_POISON(_elm, _p)
#define AVL_CHECK(_name, _elm, _p)	_name##_AVL_CHECK(_elm, _p)

#define AVL_FOREACH(_e, _name, _head)					\
	for ((_e) = AVL_MIN(_name, (_head));				\
	     (_e) != NULL;						\
	     (_e) = AVL_NEXT(_name, (_e)))

#define AVL_FOREACH_SAFE(_e, _name, _head, _n)				\
	for ((_e) = AVL_MIN(_name, (_head));				\
	     (_e) != NULL && ((_n) = AVL_NEXT(_name, (_e)), 1);	\
	     (_e) = (_n))

#define AVL_FOREACH_REVERSE(_e, _name, _head)				\
	for ((_e) = AVL_MAX(_name, (_head));				\
	     (_e) != NULL;						\
	     (_e) = AVL_PREV(_name, (_e)))

#define AVL_FOREACH_REVERSE_SAFE(_e, _name, _head, _n)			\
	for ((_e) = AVL_MAX(_name, (_head));				\
	     (_e) != NULL && ((_n) = AVL_PREV(_name, (_e)), 1);	\
	     (_e) = (_n))
 
#endif /* _BST.H_ */
