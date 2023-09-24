# IVSHMEM driver

## Step 1

Check your kernel version:
```sh
$ uname -r
```

## Step 2

Download a copy of the source code `linux/drivers/uio/uio.c` from `https://elixir.bootlin.com/linux/vX.Y.Z/source`.

Modify the `X.Y.Z` to your kernel version.

## Step 3

Modify the source code by finding `UIO_MEM_PHYS`:
```c
	if (idev->info->mem[mi].memtype == UIO_MEM_PHYS)
		; // vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
```

### Why comment thie line out?

Because this io memory is pure memory not MMIO space, so cache invalidation is no need.
We eliminate it for performance reason between VM communication.
