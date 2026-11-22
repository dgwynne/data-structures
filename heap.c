/* */

/*
 * Copyright (c) 2017 David Gwynne <dlg@openbsd.org>
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

#include "heap.h"

static inline struct _heap_entry *
heap_n2e(const struct _heap_type *t, const void *node)
{
	unsigned long addr = (unsigned long)node;

	return ((struct _heap_entry *)(addr + t->t_offset));
}

static inline void *
heap_e2n(const struct _heap_type *t, const struct _heap_entry *he)
{
	unsigned long addr = (unsigned long)he;

	return ((void *)(addr - t->t_offset));
}

struct _heap_entry *
_heap_merge(const struct _heap_type *t,
    struct _heap_entry *he1, struct _heap_entry *he2)
{
	struct _heap_entry *hi, *lo;
	struct _heap_entry *child;

	if (he1 == NULL)
		return (he2);
	if (he2 == NULL)
		return (he1);

	if (t->t_compare(heap_e2n(t, he1), heap_e2n(t, he2)) < 0) {
		lo = he1;
		hi = he2;
	} else {
		hi = he1;
		lo = he2;
	}

	child = lo->he_child;

	hi->he_left = lo;
	hi->he_nextsibling = child;
	if (child != NULL)
		child->he_left = hi;
	lo->he_child = hi;
	lo->he_left = NULL;
	lo->he_nextsibling = NULL;

	return (lo);
}

static inline void
_heap_sibling_remove(struct _heap_entry *he)
{
	struct _heap_entry *left;
	struct _heap_entry *nextsibling;

	left = he->he_left;
	if (left != NULL) {
		nextsibling = he->he_nextsibling;
		if (left->he_child == he)
			left->he_child = nextsibling;
		else
			left->he_nextsibling = nextsibling;

		if (nextsibling != NULL)
			nextsibling->he_left = left;

		he->he_left = NULL;
	}
	he->he_nextsibling = NULL;
}

static inline struct _heap_entry *
_heap_2pass_merge(const struct _heap_type *t, struct _heap_entry *root)
{
	struct _heap_entry *node, *next = NULL;
	struct _heap_entry *tmp, *list = NULL;

	node = root->he_child;
	if (node == NULL)
		return (NULL);

	root->he_child = NULL;

	/* first pass */
	for (next = node->he_nextsibling; next != NULL;
	    next = (node != NULL ? node->he_nextsibling : NULL)) {
		tmp = next->he_nextsibling;
		node = _heap_merge(t, node, next);

		/* insert head */
		node->he_nextsibling = list;
		list = node;
		node = tmp;
	}

	/* odd child case */
	if (node != NULL) {
		node->he_nextsibling = list;
		list = node;
	}

	/* second pass */
	while (list->he_nextsibling != NULL) {
		tmp = list->he_nextsibling->he_nextsibling;
		list = _heap_merge(t, list, list->he_nextsibling);
		list->he_nextsibling = tmp;
	}

	list->he_left = NULL;
	list->he_nextsibling = NULL;

	return (list);
}

static inline int
_heap_merge_root(const struct _heap_type *t, struct _heap *h,
    struct _heap_entry *he)
{
	struct _heap_entry *hr;
	struct _heap_entry *nhr;

	hr = h->h_root;
	nhr = _heap_merge(t, hr, he);
	if (nhr != hr) {
		h->h_root = nhr;
		return (1);
	}
	return (0);
}

int
_heap_merge_heap(const struct _heap_type *t, struct _heap *h, struct _heap *oh)
{
	struct _heap_entry *ohr;

	ohr = oh->h_root;
	if (ohr == NULL)
		return (0);

	oh->h_root = NULL;
	return (_heap_merge_root(t, h, ohr));
}

int
_heap_insert(const struct _heap_type *t, struct _heap *h, void *node)
{
	struct _heap_entry *he = heap_n2e(t, node);

	he->he_left = NULL;
	he->he_child = NULL;
	he->he_nextsibling = NULL;

	return (_heap_merge_root(t, h, he));
}

int
_heap_decrease(const struct _heap_type *t, struct _heap *h, void *node)
{
	struct _heap_entry *he = heap_n2e(t, node);

	/* already the root */
	if (h->h_root == he)
		return (0);

	/* detach from its current sibling list */
	_heap_sibling_remove(he);

	/* meld with the current root */
	return (_heap_merge_root(t, h, he));
}

int
_heap_remove(const struct _heap_type *t, struct _heap *h, void *node)
{
	struct _heap_entry *he = heap_n2e(t, node);
	struct _heap_entry *lhe;

	lhe = _heap_2pass_merge(t, he);

	if (h->h_root == he) {
		h->h_root = lhe;
		return (1);
	}

	_heap_sibling_remove(he);

	if (lhe == NULL)
		return (0);

	return (_heap_merge_root(t, h, lhe));
}

void *
_heap_first(const struct _heap_type *t, const struct _heap *h)
{
	struct _heap_entry *first = h->h_root;

	if (first == NULL)
		return (NULL);

	return (heap_e2n(t, first));
}

void *
_heap_extract(const struct _heap_type *t, struct _heap *h)
{
	struct _heap_entry *first = h->h_root;

	if (first == NULL)
		return (NULL);

	h->h_root = _heap_2pass_merge(t, first);

	return (heap_e2n(t, first));
}

void *
_heap_cextract(const struct _heap_type *t, struct _heap *h, const void *key)
{
	struct _heap_entry *first = h->h_root;
	void *node;

	if (first == NULL)
		return (NULL);

	node = heap_e2n(t, first);
	if (t->t_compare(key, node) < 0)
		return (NULL);

	h->h_root = _heap_2pass_merge(t, first);

	return (node);
}

void *
_heap_iter_next(const struct _heap_type *t, const void *node)
{
	const struct _heap_entry *he = heap_n2e(t, node);
	const struct _heap_entry *phe;

	/* go down the tree first */
	if (he->he_child != NULL)
		return (heap_e2n(t, he->he_child));

	/* go right when we hit the bottom */
	if (he->he_nextsibling != NULL)
		return (heap_e2n(t, he->he_nextsibling));

	/*
	 * ok, we need to go back to the left and up. if the previous
	 * entry is NULL then we were at the root of the heap.
	 */
	while ((phe = he->he_left) != NULL) {
		/* we're at the far left if the prev node is the parent */
		if (phe->he_child == he) {
			/* try go to the right of the parent */
			if (phe->he_nextsibling != NULL)
				return (heap_e2n(t, phe->he_nextsibling));
		}

		/* oh well, go back again */
		he = phe;
	}

	return (NULL);
}
