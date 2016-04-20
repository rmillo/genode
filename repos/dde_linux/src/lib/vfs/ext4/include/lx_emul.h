/*
 * \brief  Emulation of the Linux kernel API
 * \author Josef Soentgen
 * \date   2016-04-21
 *
 * The content of this file, in particular data structures, is partially
 * derived from Linux-internal headers.
 */

/*
 * Copyright (C) 2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _LX_EMUL_H_
#define _LX_EMUL_H_

#include <stdarg.h>
#include <base/fixed_stdint.h>

#include <lx_emul/extern_c_begin.h>

/*****************
 ** asm/param.h **
 *****************/

enum { HZ = 100 };

#define DEBUG_LINUX_PRINTK 1

#include <lx_emul/printf.h>

/***************
 ** asm/bug.h **
 ***************/

#include <lx_emul/bug.h>

#define BUILD_BUG_ON(condition)

#define BUILD_BUG_ON_NOT_POWER_OF_2(n)          \
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))


/******************
 ** asm/atomic.h **
 ******************/

#include <lx_emul/atomic.h>


/*******************
 ** linux/types.h **
 *******************/

#include <lx_emul/types.h>

typedef int clockid_t;

typedef size_t __kernel_size_t;
typedef long   __kernel_time_t;
typedef long   __kernel_suseconds_t;

#define __aligned_u64 __u64 __attribute__((aligned(8)))

#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]


/************************
 ** uapi/linux/types.h **
 ************************/

#define __bitwise__

typedef __u16 __le16;
typedef __u32 __le32;
typedef __u64 __le64;
typedef __u16 __be16;
typedef __u32 __be32;
typedef __u64 __be64;

typedef __u16 __sum16;
typedef __u32 __wsum;

/*
 * needed by include/net/cfg80211.h
 */
struct callback_head {
	struct callback_head *next;
	void (*func)(struct callback_head *head);
};
#define rcu_head callback_head


/*******************
 ** asm/barrier.h **
 *******************/

#include <lx_emul/barrier.h>

#define smp_load_acquire(p)     *(p)
#define smp_store_release(p, v) *(p) = v;
#define smp_mb__before_atomic() mb()


/**********************
 ** asm-generic/io.h **
 **********************/

#include <lx_emul/mmio.h>

#define mmiowb() barrier()
struct device;

void *ioremap(resource_size_t offset, unsigned long size);
void  iounmap(volatile void *addr);
void *devm_ioremap(struct device *dev, resource_size_t offset,
                   unsigned long size);
void *devm_ioremap_nocache(struct device *dev, resource_size_t offset,
                           unsigned long size);

void *ioremap_wc(resource_size_t phys_addr, unsigned long size);

#define ioremap_nocache ioremap

void *phys_to_virt(unsigned long address);


/**********************
 ** linux/compiler.h **
 **********************/

#include <lx_emul/compiler.h>

#define __cond_lock(x,c) (c)

#define noinline_for_stack noinline

static inline void __write_once_size(volatile void *p, void *res, int size)
{
	switch (size) {
	case 1: *(volatile __u8  *)p = *(__u8  *)res; break;
	case 2: *(volatile __u16 *)p = *(__u16 *)res; break;
	case 4: *(volatile __u32 *)p = *(__u32 *)res; break;
	case 8: *(volatile __u64 *)p = *(__u64 *)res; break;
	default:
		barrier();
		__builtin_memcpy((void *)p, (const void *)res, size);
		barrier();
	}
}

#define WRITE_ONCE(x, val)                        \
({                                                \
	union { typeof(x) __val; char __c[1]; } __u = \
	{ .__val = (__force typeof(x)) (val) };       \
	__write_once_size(&(x), __u.__c, sizeof(x));  \
	__u.__val;                                    \
})

static inline void __read_once_size(const volatile void *p, void *res, int size)
{
	switch (size) {
		case 1: *(__u8  *)res = *(volatile __u8  *)p; break;
		case 2: *(__u16 *)res = *(volatile __u16 *)p; break;
		case 4: *(__u32 *)res = *(volatile __u32 *)p; break;
		case 8: *(__u64 *)res = *(volatile __u64 *)p; break;
		default:
			barrier();
			__builtin_memcpy((void *)res, (const void *)p, size);
			barrier();
	}
}

#define READ_ONCE(x) \
({                                               \
	union { typeof(x) __val; char __c[1]; } __u; \
	__read_once_size(&(x), __u.__c, sizeof(x));  \
	__u.__val;                                   \
})


/**************************
 ** linux/compiler-gcc.h **
 **************************/

#ifdef __aligned
#undef __aligned
#endif
#define __aligned(x)  __attribute__((aligned(x)))
#define __visible     __attribute__((externally_visible))

#define OPTIMIZER_HIDE_VAR(var) __asm__ ("" : "=r" (var) : "0" (var))


/********************
 ** linux/module.h **
 ********************/

#include <lx_emul/module.h>

static inline bool module_sig_ok(struct module *module) { return true; }

#define module_name(mod) "foobar"


/*************************
 ** linux/moduleparam.h **
 *************************/

#define __MODULE_INFO(tag, name, info)

static inline void kernel_param_lock(struct module *mod) { }
static inline void kernel_param_unlock(struct module *mod) { }


/*******************
 ** linux/errno.h **
 *******************/

#include <lx_emul/errno.h>


/*****************
 ** linux/err.h **
 *****************/

static inline int PTR_ERR_OR_ZERO(const void *ptr)
{
	if (IS_ERR(ptr)) return PTR_ERR(ptr);
	else             return 0;
}


/********************
 ** linux/poison.h **
 ********************/

#include <lx_emul/list.h>


/****************
 ** asm/page.h **
 ****************/

/*
 * For now, hardcoded
 */
#define PAGE_SIZE 4096UL
#define PAGE_MASK (~(PAGE_SIZE-1))

enum {
	PAGE_SHIFT = 12,
};

struct page
{
	unsigned long flags;
	int       pfmemalloc;
	int       mapping;
	atomic_t _count;
	void     *addr;
	unsigned long private;
} __attribute((packed));


/************************
 ** linux/page-flags.h **
 ************************/

enum pageflags
{
	PG_slab = 0x1,
};

#define PageSlab(page) test_bit(PG_slab, &(page)->flags)


/**********************
 ** asm/cacheflush.h **
 **********************/

void flush_dcache_page(struct page *page);
enum { ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE = 1 }; /* XXX */


/**********************
 ** linux/mm-types.h **
 **********************/

struct vm_operations_struct;

struct vm_area_struct
{
	unsigned long vm_start;
	unsigned long vm_end;

	const struct vm_operations_struct *vm_ops;

	unsigned long vm_pgoff;

	struct file * vm_file;
};

struct page_frag
{
	struct page *page;
	__u16        offset;
	__u16        size;
};

struct page_frag_cache
{
	bool pfmemalloc;
};


/*****************
 ** linux/gfp.h **
 *****************/

#include <lx_emul/gfp.h>

enum {
	__GFP_DIRECT_RECLAIM = 0x00400000u,
};

struct page *alloc_pages_node(int nid, gfp_t gfp_mask, unsigned int order);

struct page *alloc_pages(gfp_t gfp_mask, unsigned int order);

#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)

unsigned long get_zeroed_page(gfp_t gfp_mask);
#define free_page(p) kfree((void *)p)

bool gfp_pfmemalloc_allowed(gfp_t);
unsigned long __get_free_page(gfp_t);
unsigned long __get_free_pages(gfp_t, unsigned int);
void free_pages(unsigned long, unsigned int);
void __free_pages(struct page *page, unsigned int order);
void __free_page_frag(void *addr);

bool gfpflags_allow_blocking(const gfp_t gfp_flags);

void *__alloc_page_frag(struct page_frag_cache *nc,
                        unsigned int fragsz, gfp_t gfp_mask);


/********************
 ** linux/string.h **
 ********************/

#include <lx_emul/string.h>


/**********************
 ** linux/spinlock.h **
 **********************/

#include <lx_emul/spinlock.h>


/*******************
 ** linux/mutex.h **
 *******************/

#include <lx_emul/mutex.h>


/*******************
 ** linux/rwsem.h **
 *******************/

#include <lx_emul/semaphore.h>


/********************
 ** linux/kernel.h **
 ********************/

#include <lx_emul/kernel.h>

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))

char *kasprintf(gfp_t gfp, const char *fmt, ...);
int kstrtouint(const char *s, unsigned int base, unsigned int *res);

#define PTR_ALIGN(p, a) ({               \
	unsigned long _p = (unsigned long)p; \
	_p = (_p + a - 1) & ~(a - 1);        \
	p = (typeof(p))_p;                   \
	p;                                   \
})

static inline u32 reciprocal_scale(u32 val, u32 ep_ro)
{
	return (u32)(((u64) val * ep_ro) >> 32);
}

int kstrtoul(const char *s, unsigned int base, unsigned long *res);

int strict_strtoul(const char *s, unsigned int base, unsigned long *res);
long simple_strtoul(const char *cp, char **endp, unsigned int base);
long simple_strtol(const char *,char **,unsigned int);

int hex_to_bin(char ch);

/* needed by drivers/net/wireless/iwlwifi/iwl-drv.c */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args) __attribute__((format(printf, 3, 0)));
int sprintf(char *buf, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
int scnprintf(char *buf, size_t size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

int sscanf(const char *, const char *, ...);

/* XXX */
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)
#define IS_ALIGNED(x, a) (((x) & ((typeof(x))(a) - 1)) == 0)

#define SIZE_MAX (~(size_t)0)

#define U8_MAX   ((u8)~0U)
#define S8_MAX   ((s8)(U8_MAX>>1))
#define S8_MIN   ((s8)(-S8_MAX - 1))
#define U16_MAX  ((u16)~0U)
#define U32_MAX  ((u32)~0U)
#define S32_MAX  ((s32)(U32_MAX>>1))
#define S32_MIN  ((s32)(-S32_MAX - 1))


/*********************
 ** linux/jiffies.h **
 *********************/

#include <lx_emul/jiffies.h>

static inline unsigned int jiffies_to_usecs(const unsigned long j) { return j * JIFFIES_TICK_US; }


/******************
 ** linux/time.h **
 ******************/

#include <lx_emul/time.h>

enum {
	MSEC_PER_SEC  = 1000L,
	USEC_PER_SEC  = MSEC_PER_SEC * 1000L,
	USEC_PER_MSEC = 1000L,
};

unsigned long get_seconds(void);
void          getnstimeofday(struct timespec *);
#define do_posix_clock_monotonic_gettime(ts) ktime_get_ts(ts)

#define ktime_to_ns(kt) ((kt).tv64)

struct timespec ktime_to_timespec(const ktime_t kt);
bool ktime_to_timespec_cond(const ktime_t kt, struct timespec *ts);

int     ktime_equal(const ktime_t, const ktime_t);
s64     ktime_us_delta(const ktime_t, const ktime_t);

static inline s64 ktime_to_ms(const ktime_t kt)
{
	return kt.tv64 / NSEC_PER_MSEC;
}

static inline void ktime_get_ts(struct timespec *ts)
{
	ts->tv_sec  = jiffies * (1000/HZ);
	ts->tv_nsec = 0;
}


/*******************
 ** linux/timer.h **
 *******************/

#include <lx_emul/timer.h>


/*********************
 ** linux/kconfig.h **
 *********************/

#define config_enabled(cfg) 0


/*******************************
 ** linux/byteorder/generic.h **
 *******************************/

#include <lx_emul/byteorder.h>

#define cpu_to_be64  __cpu_to_be64
#define be64_to_cpup __be64_to_cpup

#define htonl(x) __cpu_to_be32(x)
#define htons(x) __cpu_to_be16(x)
#define ntohl(x) __be32_to_cpu(x)
#define ntohs(x) __be16_to_cpu(x)


/*************************************
 ** linux/unaligned/packed_struct.h **
 *************************************/

struct __una_u16 { u16 x; } __attribute__((packed));
struct __una_u32 { u32 x; } __attribute__((packed));
struct __una_u64 { u64 x; } __attribute__((packed));


/*******************************
 ** linux/unaligned/generic.h **
 *******************************/

static inline void put_unaligned_le16(u16 val, void *p) {
	*((__le16 *)p) = cpu_to_le16(val); }

static inline void put_unaligned_be16(u16 val, void *p) {
	*((__be16 *)p) = cpu_to_be16(val); }

static inline void put_unaligned_le32(u32 val, void *p) {
	*((__le32 *)p) = cpu_to_le32(val); }

static inline u16 get_unaligned_le16(const void *p)
{
	const struct __una_u16 *ptr = (const struct __una_u16 *)p;
	return ptr->x;
}

static inline u32 get_unaligned_le32(const void *p)
{
	const struct __una_u32 *ptr = (const struct __una_u32 *)p;
	return ptr->x;
}

static inline u16 get_unaligned_be16(const void *p)
{
	const __u8 *be = (__u8*)p;
	return (be[1]<<0)|(be[0]<<8);
}

void put_unaligned_le64(u64 val, void *p);

#define put_unaligned(val, ptr) ({              \
	void *__gu_p = (ptr);                       \
	switch (sizeof(*(ptr))) {                   \
	case 1:                                     \
		*(u8 *)__gu_p = (u8)(val);              \
		break;                                  \
	case 2:                                     \
		put_unaligned_le16((u16)(val), __gu_p); \
		break;                                  \
	case 4:                                     \
		put_unaligned_le32((u32)(val), __gu_p); \
		break;                                  \
	case 8:                                     \
		put_unaligned_le64((u64)(val), __gu_p); \
		break;                                  \
	}                                           \
	(void)0; })


static inline void le32_add_cpu(__le32 *var, u32 val) {
	*var = cpu_to_le32(le32_to_cpu(*var) + val); }


static inline u32 __get_unaligned_cpu32(const void *p)
{
	const struct __una_u32 *ptr = (const struct __una_u32 *)p;
	return ptr->x;
}


/****************************************
 ** asm-generic/bitops/const_hweight.h **
 ****************************************/

#define __const_hweight8(w)     \
	( (!!((w) & (1ULL << 0))) + \
	  (!!((w) & (1ULL << 1))) + \
	  (!!((w) & (1ULL << 2))) + \
	  (!!((w) & (1ULL << 3))) + \
	  (!!((w) & (1ULL << 4))) + \
	  (!!((w) & (1ULL << 5))) + \
	  (!!((w) & (1ULL << 6))) + \
	  (!!((w) & (1ULL << 7))) )

#define hweight8(w)  (__const_hweight8(w))

unsigned int hweight16(unsigned int w);
unsigned int hweight32(unsigned int w);
unsigned int hweight64(__u64 w);


/**********************************
 ** linux/bitops.h, asm/bitops.h **
 **********************************/

#include <lx_emul/bitops.h>

static inline unsigned long hweight_long(unsigned long w) {
	return sizeof(w) == 4 ? hweight32(w) : hweight64(w); }


/*******************************
 ** asm-generic/bitops/find.h **
 *******************************/

unsigned long find_next_bit(const unsigned long *addr, unsigned long size, unsigned long offset);
unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size, unsigned long offset);
unsigned long find_last_bit(const unsigned long *addr, unsigned long size);

#define find_first_bit(addr, size) find_next_bit((addr), (size), 0)
#define find_first_zero_bit(addr, size) find_next_zero_bit((addr), (size), 0)


/****************
 ** linux/mm.h **
 ****************/

int is_vmalloc_addr(const void *x);

extern unsigned long totalram_pages;
extern unsigned long num_physpages;

static inline struct page *compound_head(struct page *page) { return page; }
static inline void *page_address(struct page *page) { return page->addr; };

void get_page(struct page *page);
void put_page(struct page *page);

#define offset_in_page(p) ((unsigned long)(p) & ~PAGE_MASK)

struct page *virt_to_head_page(const void *x);
struct page *virt_to_page(const void *x);
struct page *vmalloc_to_page(const void *addr);

struct sysinfo;
void si_meminfo(struct sysinfo *);

#define page_private(page)      ((page)->private)
#define set_page_private(page, v)   ((page)->private = (v))

struct vm_operations_struct {
	void (*open)(struct vm_area_struct * area);
	void (*close)(struct vm_area_struct * area);
};

int get_user_pages_fast(unsigned long start, int nr_pages, int write, struct page **pages);
int vm_insert_page(struct vm_area_struct *, unsigned long addr, struct page *);

bool page_is_pfmemalloc(struct page *page);

#define PAGE_ALIGNED(addr) IS_ALIGNED((unsigned long)addr, PAGE_SIZE)


/*********************
 ** linux/kobject.h **
 *********************/

#include <lx_emul/kobject.h>

enum kobject_action
{
	KOBJ_ADD,
	KOBJ_REMOVE,
	KOBJ_CHANGE,
};

void kobject_put(struct kobject *);
int kobject_uevent(struct kobject *, enum kobject_action);
int kobject_uevent_env(struct kobject *kobj, enum kobject_action action, char *envp[]);


/*********************
 ** linux/vmalloc.h **
 *********************/

void *vmalloc(unsigned long size);
void *vzalloc(unsigned long size);
void vfree(const void *addr);


/**********************
 ** linux/highmem.h  **
 **********************/

static inline void *kmap(struct page *page) { return page_address(page); }
static inline void *kmap_atomic(struct page *page) { return kmap(page); }
static inline void kunmap(struct page *page) { }
static inline void kunmap_atomic(void *addr) { }


/******************
 ** linux/slab.h **
 ******************/

#define ARCH_KMALLOC_MINALIGN __alignof__(unsigned long long)

enum {
	SLAB_HWCACHE_ALIGN = 0x00002000ul,
	SLAB_CACHE_DMA     = 0x00004000ul,
	SLAB_PANIC         = 0x00040000ul,

	SLAB_LX_DMA        = 0x80000000ul,
};

void *kzalloc(size_t size, gfp_t flags);
void  kfree(const void *);
void  kzfree(const void *);
void *kmalloc(size_t size, gfp_t flags);
void *kcalloc(size_t n, size_t size, gfp_t flags);
void *kmalloc_array(size_t n, size_t size, gfp_t flags);
void  kvfree(const void *);

struct kmem_cache;
struct kmem_cache *kmem_cache_create(const char *, size_t, size_t, unsigned long, void (*)(void *));
void   kmem_cache_destroy(struct kmem_cache *);
void  *kmem_cache_alloc(struct kmem_cache *, gfp_t);
void *kmem_cache_zalloc(struct kmem_cache *k, gfp_t flags);
void  kmem_cache_free(struct kmem_cache *, void *);
void *kmalloc_node_track_caller(size_t size, gfp_t flags, int node);

static inline void *kmem_cache_alloc_node(struct kmem_cache *s, gfp_t flags, int node)
{
	return kmem_cache_alloc(s, flags);
}


/*************************
 ** linux/irq_cpustat.h **
 *************************/

int local_softirq_pending(void);


/**********************
 ** linux/irqflags.h **
 **********************/

#define local_irq_enable(a )     do { } while (0)
#define local_irq_disable()      do { } while (0)
#define local_irq_save(flags)    do { (void)flags; } while (0)
#define local_irq_restore(flags) do { (void)flags; } while (0)


/********************
 ** linux/printk.h **
 ********************/

static inline int _printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

static inline int no_printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static inline int no_printk(const char *fmt, ...) { return 0; }

#define printk_ratelimit(x) (0)

#define printk_ratelimited(fmt, ...) printk(fmt, ##__VA_ARGS__)

#define pr_emerg(fmt, ...)    printk(KERN_EMERG  fmt, ##__VA_ARGS__)
#define pr_alert(fmt, ...)    printk(KERN_ALERT  fmt, ##__VA_ARGS__)
#define pr_crit(fmt, ...)     printk(KERN_CRIT   fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...)      printk(KERN_ERR    fmt, ##__VA_ARGS__)
#define pr_warning(fmt, ...)  printk(KERN_WARN   fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...)     printk(KERN_WARN   fmt, ##__VA_ARGS__)
#define pr_notice(fmt, ...)   printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...)     printk(KERN_INFO   fmt, ##__VA_ARGS__)
#define pr_cont(fmt, ...)     printk(KERN_CONT   fmt, ##__VA_ARGS__)
/* pr_devel() should produce zero code unless DEBUG is defined */
#ifdef DEBUG
#define pr_devel(fmt, ...)    printk(KERN_DEBUG  fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...)    printk(KERN_DEBUG  fmt, ##__VA_ARGS__)
#else
#define pr_devel(fmt, ...) no_printk(KERN_DEBUG  fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...) no_printk(KERN_DEBUG  fmt, ##__VA_ARGS__)
#endif

#define pr_warn_ratelimited(fmt, ...) printk(KERN_WARNING fmt, ##__VA_ARGS__)

enum {
	DUMP_PREFIX_OFFSET
};

int snprintf(char *str, size_t size, const char *format, ...) __attribute__((format(printf, 3, 4)));

static inline void print_hex_dump(const char *level, const char *prefix_str,
                                  int prefix_type, int rowsize, int groupsize,
                                  const void *buf, size_t len, bool ascii)
{
	_printk("hex_dump: ");
	size_t i;
	for (i = 0; i < len; i++) {
		_printk("%x ", ((char*)buf)[i]);
	}
	_printk("\n");
}

void hex_dump_to_buffer(const void *buf, size_t len, int rowsize, int groupsize,
                        char *linebuf, size_t linebuflen, bool ascii);
void dump_stack(void);

unsigned long int_sqrt(unsigned long);


/***********************
 ** linux/workqueue.h **
 ***********************/

struct workqueue_struct; /* XXX fix in lx_emul/work.h */

#include <lx_emul/work.h>

enum {
	WQ_UNBOUND        = 1<<1,
	// WQ_MEM_RECLAIM    = 1<<3,
	WQ_HIGHPRI        = 1<<4,
	// WQ_CPU_INTENSIVE  = 1<<5,
};

#include <lx_emul/extern_c_end.h>

#endif /* _LX_EMUL_H_ */
