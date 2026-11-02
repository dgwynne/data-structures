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

#ifndef _AHEAP_H_
#define _AHEAP_H_

struct aheap_ops {
	int	(*cmp)(const void *, unsigned int, unsigned int);
	void	(*swap)(void *, unsigned int, unsigned int);
};

struct aheap {
	const struct aheap_ops	*ah_ops;
	void			*ah_heap;
	unsigned int		 ah_len;
};

unsigned int	aheap_heapify(const struct aheap *, unsigned int,
		    unsigned int (*)(const struct aheap *, unsigned int));
unsigned int	aheap_cmp_up(const struct aheap *, unsigned int);
unsigned int	aheap_cmp_dn(const struct aheap *, unsigned int);

static inline unsigned int
aheap_heapify_up(const struct aheap *ah, unsigned int idx)
{
        return (aheap_heapify(ah, idx, aheap_cmp_up));
}

static inline unsigned int
aheap_heapify_dn(const struct aheap *ah, unsigned int idx)
{
        return (aheap_heapify(ah, idx, aheap_cmp_dn));
}

#endif /* _AHEAP_H_ */
