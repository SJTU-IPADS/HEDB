#include <linux/device.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>
#include <linux/io.h>

struct ivshmem_info {
    struct uio_info *uio;
    struct pci_dev *dev;
};

static int ivshmem_pci_probe(struct pci_dev *dev,
                    const struct pci_device_id *id)
{
    struct uio_info *info;
    struct ivshmem_info *ivshmem_info;

    info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
    if (!info)
        return -ENOMEM;

    ivshmem_info = kzalloc(sizeof(struct ivshmem_info), GFP_KERNEL);
    if (!ivshmem_info) {
        kfree(info);
        return -ENOMEM;
    }
    info->priv = ivshmem_info;

    if (pci_enable_device(dev))
        goto out_free;

    if (pci_request_regions(dev, "ivshmem"))
        goto out_disable;

    info->mem[0].addr = pci_resource_start(dev, 2);
    if (!info->mem[0].addr)
        goto out_unmap;

    info->mem[0].size = pci_resource_len(dev, 2);
    info->mem[0].memtype = UIO_MEM_PHYS;
    info->mem[0].name = "shmem";

    ivshmem_info->uio = info;
    ivshmem_info->dev = dev;

    pr_info("addr %llx size %llx\n", info->mem[0].addr, info->mem[0].size);

    pci_set_master(dev);

    info->name = "uio_ivshmem";
    info->version = "0.0.1";

    if (uio_register_device(&dev->dev, info)) {
        pr_info("uio_register_device failed");
        goto out_unmap;
    }

    pci_set_drvdata(dev, ivshmem_info);

    return 0;
out_vector:
    pci_free_irq_vectors(dev);
out_unmap:
out_release:
    pci_release_regions(dev);
out_disable:
    pci_disable_device(dev);
out_free:
    kfree(ivshmem_info);
    kfree(info);
    return -ENODEV;
}

static void ivshmem_pci_remove(struct pci_dev *dev)
{
    struct ivshmem_info *ivshmem_info = pci_get_drvdata(dev);
    struct uio_info *info = ivshmem_info->uio;

    pci_set_drvdata(dev, NULL);
    uio_unregister_device(info);
    pci_release_regions(dev);
    pci_disable_device(dev);
    kfree(info);
    kfree(ivshmem_info);
}

static struct pci_device_id ivshmem_pci_ids[] = {
    {
        .vendor =   0x1af4,
        .device =   0x1110,
        .subvendor =    PCI_ANY_ID,
        .subdevice =    PCI_ANY_ID,
    },
    { 0, }
};

static struct pci_driver ivshmem_pci_driver = {
    .name = "uio_ivshmem",
    .id_table = ivshmem_pci_ids,
    .probe = ivshmem_pci_probe,
    .remove = ivshmem_pci_remove,
};

module_pci_driver(ivshmem_pci_driver);
MODULE_DEVICE_TABLE(pci, ivshmem_pci_ids);
MODULE_LICENSE("GPL");
