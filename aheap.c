/*
 * Copyright (c) 2026 David Gwynne <dlg@openbsd.org>
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

#include "aheap.h"

static inline unsigned int
aheap_parent(unsigned int idx)
{
	return ((idx - 1) >> 1);
}

static inline unsigned int
aheap_left(unsigned int idx)
{
	return ((idx << 1) + 1);
}

static inline unsigned int
aheap_right(unsigned int idx)
{
	return ((idx << 1) + 2);
}

static inline int
aheap_cmp(const struct aheap *ah, unsigned int a, unsigned int b)
{
	return (ah->ah_ops->cmp(ah->ah_heap, a, b));
}

static inline void
aheap_swap(const struct aheap *ah, unsigned int a, unsigned int b)
{
	ah->ah_ops->swap(ah->ah_heap, a, b);
}

unsigned int
aheap_heapify(const struct aheap *ah, unsigned int idx,
    unsigned int (*cmp)(const struct aheap *, unsigned int))
{
	unsigned int nidx;

	while ((nidx = cmp(ah, idx)) != idx) {
		aheap_swap(ah, idx, nidx);
		idx = nidx;
	}

	return (idx);
}

unsigned int
aheap_cmp_dn(const struct aheap *ah, unsigned int idx)
{
	unsigned int n, nidx = idx;

	n = aheap_left(idx);
	if (n >= ah->ah_len)
		return (nidx);

	/* is the left child better? */
	if (aheap_cmp(ah, n, idx))
		nidx = n;

	/* is the right child better? */
	n = aheap_right(idx);
	if (n >= ah->ah_len)
		return (nidx);

	if (aheap_cmp(ah, n, nidx))
		nidx = n;

	return (nidx);
}

unsigned int
aheap_cmp_up(const struct aheap *ah, unsigned int idx)
{
	unsigned int parent;

	if (idx == 0)
		return (idx);

	parent = aheap_parent(idx);
	/* is the parent better? */
	if (aheap_cmp(ah, parent, idx))
		return (idx);

	return (parent);
}
