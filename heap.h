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

#ifndef _HEAP_H_
#define _HEAP_H_

#include <sys/_null.h>

struct _heap_type {
	int			(*t_compare)(const void *, const void *);
	unsigned int		  t_offset; /* offset of heap_entry in type */
};

struct _heap_entry {
	struct _heap_entry	*he_left;
	struct _heap_entry	*he_child;
	struct _heap_entry	*he_nextsibling;
};
#define HEAP_ENTRY(_entry)	struct _heap_entry

struct _heap {
	struct _heap_entry	*h_root;
};

#define HEAP_HEAD(_name)						\
struct _name {								\
	struct _heap		heap;					\
}

static inline void
_heap_init(struct _heap *h)
{
	h->h_root = NULL;
}

static inline int
_heap_empty(struct _heap *h)
{
	return (h->h_root == NULL);
}

void	 _heap_insert(const struct _heap_type *, struct _heap *, void *);
void	 _heap_remove(const struct _heap_type *, struct _heap *, void *);
void	*_heap_first(const struct _heap_type *, struct _heap *);
struct _heap_entry *
	_heap_merge(const struct _heap_type *,
	    struct _heap_entry *, struct _heap_entry *);
void	*_heap_extract(const struct _heap_type *, struct _heap *);
void	*_heap_cextract(const struct _heap_type *, struct _heap *,
	     const void *);
void	*_heap_iter_next(const struct _heap_type *, const void *);

#define HEAP_INITIALIZER(_head)	{ { NULL } }

#define HEAP_PROTOTYPE(_name, _type)					\
extern const struct _heap_type *const _name##_HEAP_TYPE;		\
									\
static __unused inline void						\
_name##_HEAP_INIT(struct _name *head)					\
{									\
	_heap_init(&head->heap);					\
}									\
									\
static __unused inline void						\
_name##_HEAP_INSERT(struct _name *head, struct _type *elm)		\
{									\
	_heap_insert(_name##_HEAP_TYPE, &head->heap, elm);		\
}									\
									\
static __unused inline void						\
_name##_HEAP_REMOVE(struct _name *head, struct _type *elm)		\
{									\
	_heap_remove(_name##_HEAP_TYPE, &head->heap, elm);		\
}									\
									\
static __unused inline struct _type *					\
_name##_HEAP_FIRST(struct _name *head)					\
{									\
	return _heap_first(_name##_HEAP_TYPE, &head->heap);		\
}									\
									\
static __unused inline void						\
_name##_HEAP_MERGE(struct _name *head1, struct _name *head2)		\
{									\
	head1->heap.h_root = _heap_merge(_name##_HEAP_TYPE,		\
	    head1->heap.h_root, head2->heap.h_root);			\
}									\
									\
static __unused inline struct _type *					\
_name##_HEAP_EXTRACT(struct _name *head)				\
{									\
	return _heap_extract(_name##_HEAP_TYPE, &head->heap);		\
}									\
									\
static __unused inline struct _type *					\
_name##_HEAP_CEXTRACT(struct _name *head, const struct _type *key)	\
{									\
	return _heap_cextract(_name##_HEAP_TYPE, &head->heap, key);	\
}									\
									\
static __unused inline int						\
_name##_HEAP_EMPTY(struct _name *head)					\
{									\
	return _heap_empty(&head->heap);				\
}									\
									\
static __unused inline struct _type *					\
_name##_HEAP_ITER_FIRST(struct _name *head)				\
{									\
	return _heap_first(_name##_HEAP_TYPE, &head->heap);		\
}									\
									\
static __unused inline struct _type *					\
_name##_HEAP_ITER_NEXT(struct _type *elm)				\
{									\
	return _heap_iter_next(_name##_HEAP_TYPE, elm);			\
}

#define HEAP_GENERATE(_name, _type, _field, _cmp)			\
static int								\
_name##_HEAP_COMPARE(const void *lptr, const void *rptr)		\
{									\
	const struct _type *l = lptr, *r = rptr;			\
	return _cmp(l, r);						\
}									\
static const struct _heap_type _name##_HEAP_INFO = {			\
	_name##_HEAP_COMPARE,						\
	offsetof(struct _type, _field),					\
};									\
const struct _heap_type *const _name##_HEAP_TYPE = &_name##_HEAP_INFO

#define HEAP_INIT(_name, _h)		_name##_HEAP_INIT((_h))
#define HEAP_INSERT(_name, _h, _e)	_name##_HEAP_INSERT((_h), (_e))
#define HEAP_REMOVE(_name, _h, _e)	_name##_HEAP_REMOVE((_h), (_e))
#define HEAP_FIRST(_name, _h)		_name##_HEAP_FIRST((_h))
#define HEAP_MERGE(_name, _h1, _h2)	_name##_HEAP_MERGE((_h1), (_h2))
#define HEAP_EXTRACT(_name, _h)		_name##_HEAP_EXTRACT((_h))
#define HEAP_CEXTRACT(_name, _h, _k)	_name##_HEAP_CEXTRACT((_h), (_k))
#define HEAP_EMPTY(_name, _h)		_name##_HEAP_EMPTY((_h))

#define HEAP_ITER_FIRST(_name, _h)	_name##_HEAP_ITER_FIRST((_h))
#define HEAP_ITER_NEXT(_name, _e)	_name##_HEAP_ITER_NEXT((_e))

#endif /* _HEAP_H_ */
