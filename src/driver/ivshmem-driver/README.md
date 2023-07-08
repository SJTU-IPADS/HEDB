# IVSHMEM driver

First, you need to download a copy of the source code `linux/drivers/uio/uio.c`
from `https://elixir.bootlin.com/linux/latest/source`.

Then, please modify here:
```c
	if (idev->info->mem[mi].memtype == UIO_MEM_PHYS)
		; // vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
```

You may also need to modify the Makefile:
```
obj-m += uio-XXX.o uio-ivshmem.o
```

Why comment thie line out?

Because this io memory is pure memory not MMIO space, so cache invalidation is no need.
We eliminate it for performance reason between VM communication.
