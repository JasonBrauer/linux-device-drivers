### kernel tree
Modules can only be successfully loaded if created within the context of the kernel tree running on the host. Determine the kernel tree running on the host with:
```bash
uname -r
```

### building an out of tree module
From the directory containing the out of tree module
```bash
make -C </path/to/kernel/tree> M=`pwd` modules
```
The `-C` specifies the kernel source tree directory and the `M=` option specifies the module source directory

to clean out of tree module directory
```bash
make -C </path/to/kernel/tree> M=`pwd` clean
```

### loading an out of tree module
From directory with the modules kernel object file (.ko)
```bash
insmod <module file name>.ko
```

Remove the module with `rmmod` and check installed modules with `lsmod`

Kernel logs print to the the kernel ring buffer and can be viewed with `dmesg`