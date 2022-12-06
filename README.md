# FA22: CMPE-283 Sec 48 - Virtual Technologies Assignment-2

Here are the steps detailed to execute Assignment-2 on Ubuntu VM hosted on VMWare Workstation.

## Contributors

**`JAYA KRISHNA THUPILI`**

## System & Prerequisites

- System Type: x64-based PC
- Processor: 12th Gen Intel(R) Core (TM) i7-1260P (12 Cores)
- Installed Physical Memory (RAM): 16.0 GB
- Windows 11 Professional
- VMware® Workstation 16 Pro [Trial/Download](https://www.vmware.com/products/workstation-pro.html)
- Ubuntu 22 [Download](https://releases.ubuntu.com/22.10/ubuntu-22.10-live-server-amd64.iso?_ga=2.70008919.652567418.1667472597-1208328021.1667382980)
- Intel® 64 and IA-32 Architectures Software Developer Manuals (SDM) - [Download](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
  - For this assignment, SDM release version **_April 2022_** is referred.

## Enabling Nested Virtualization

To enable nested virtualization for VMs running on Windows 11

- Disable the default security features on Core Isolation.

![win11_core_isolation_off](resources/1.Disable%20Core%20Isolation.png)

- Turn-off the following windows features. Start > Type "**Turn Windows features on**"

![turn_off_windows_features](resources/2.Turn%20off%20windows%20features.png)

## Installing Ubuntu

- Before creating Ubuntu VM using VMWare Workstation, enable the following options on VM > Settings > Processors

![enable_vtx](resources/3.Enable%20Intel%20VT-x.png)

- Create a VM with 16 cores (8x2 cores), 8GB RAM and 200GB HDD and install Ubuntu.
- Once the VM is booted, verify that nested VMM capabilities are available.

![vmware_vm_settings](resources/4.Verify%20nested%20VMM%20features.png)

## Fork Linux github repo

Fork Linux kernel <https://github.com/torvalds/linux> into your github account

![Fork_clone_repo](resources/5.Fork_clone_repo.png)

## Clone your copy of Linux github repo

```bash
git clone https://github.com/4musks/linux.git
```

## Build the kernel using config options from your existing installation

```bash
jay@jay-ubuntu:~$ cd linux/linux/
jay@jay-ubuntu:~/linux/linux$ cp /boot/config-5.19.0-23-generic .config
jay@jay-ubuntu:~/linux/linux$ make oldconfig
jay@jay-ubuntu:~/linux/linux$ make -j 16 modules
jay@jay-ubuntu:~/linux/linux$ sudo make INSTALL_MOD_STRIP=1 modules_install
jay@jay-ubuntu:~/linux/linux$ sudo make install
jay@jay-ubuntu:~/linux/linux$ uname -a
'Linux jay-ubuntu _5.19.0-23-generic_ #24-Ubuntu SMP PREEMPT_DYNAMIC Fri Oct 14 15:39:57 UTC 2022 x86_64 x86_64 x86_64 GNU/Linux'
jay@jay-ubuntu:~/linux/linux$ sudo reboot
```

## Now the VM will boot using New kernel

This would be the **VMM** for Assignment 2 & 3.

```bash
jay@jay-ubuntu:~/linux/linux$ uname -a
'Linux jay-ubuntu 6.1.0-rc7+ #1 SMP PREEMPT_DYNAMIC Wed Nov 30 01:57:10 PST 2022 x86_64 x86_64 x86_64 GNU/Linux'
jay@jay-ubuntu:~/linux/linux$
```

## Make the relevant code changes to cpuid.c and vmc.c as detailed in the video

Video instructions should help you build code for leaf node **_0x4FFFFFFC_**

After code changes, we can just run `"make modules && make -j 16 modules_install"` and skip `"make install"`

## Verify if kvm module is loaded

If loaded, remove and re-load it.

If not, load it now.

```bash
jay@jay-ubuntu:~/linux/linux$ lsmod | grep kvm
kvm_intel             421888  0
kvm                  1134592  1 kvm_intel
```

Remove **KVM**.

```bash
jay@jay-ubuntu:~/linux/linux$ sudo rmmod  kvm_intel
jay@jay-ubuntu:~/linux/linux$ sudo rmmod  kvm
```

Load **KVM**.

```bash
jay@jay-ubuntu:~/linux/linux$ sudo modprobe kvm
jay@jay-ubuntu:~/linux/linux$ sudo modprobe kvm_intel
jay@jay-ubuntu:~/linux/linux$ lsmod | grep kvm
kvm_intel             421888  0
kvm                  1134592  1 kvm_intel
```

## Once all modules are loaded without errors, it is time to test the code

The new Kernel you have built is the **VMM**.

And we have enabled nested virtualization on the HOST system.

Taking advantage of that, you can now create a new VM (say nestedvm) on this VMM.

You can use ubuntu or a smaller sized ISO of debian distro. **antiX Linux** is a good choice.

![NestedVM_creation](resources/6.NestedVM_creation.png)

## Log into nested VM and test the first leaf node `eax=0x4FFFFFFC` using **cpuid** tool

Update packages using `apt update`

Install package `cpuid`

Invoke CPUID instruction for `0x4FFFFFFC`

```bash
$ sudo apt-get update
$ sudo apt-get install cpuid
$ cpuid -1 -l 0x4ffffffc
# Convert HEX value of EAX to DECIMAL
$ echo $((16#<value of %eax>))
```

![7.Test_leafNode_1](resources/7.Test_leafNode_1.png)

## Leaf Node 0x4FFFFFFD

vmx.c: Use function **rdtsc()** in the boundaries of `vmx_handle_exit` to compute cpu cycles. See ref: RDTSC

cpuid.c: Using bitwise _AND_ and _RIGHT SHIFT_ operators, compounded cpu cycles variable can be separated into 32 low and high bits registers. See ref: BITWISE OPERATORS

## Re-build and install modules

```bash
make -j 16 modules
make -j 16 INSTALL_MOD_STRIP=1 modules_install
```

## Re-load KVM modules

Stop nested VM, re-load KVM modules

```bash
jay@jay-ubuntu:~/linux/linux$ sudo rmmod  kvm_intel
jay@jay-ubuntu:~/linux/linux$ sudo rmmod  kvm
jay@jay-ubuntu:~/linux/linux$ sudo modprobe kvm
jay@jay-ubuntu:~/linux/linux$ sudo modprobe kvm_intel
jay@jay-ubuntu:~/linux/linux$ lsmod | grep kvm
kvm_intel             421888  0
kvm                  1134592  1 kvm_intel
```

## Log into nested VM and test the second leaf node `eax=0x4FFFFFFD` using **cpuid** tool

Compare the output with dmesg value on VMM.

![Test_leafNode_2](resources/8.Test_leafNode_2.png)

## Q&A

Does the number of exits increase at a stable rate?

- No. Entirely dependent on the instruction being called and when

Are there more exits performed during certain VM operations?

- Yes. During VM boot procedure.

Approximately how many exits does a full VM boot entail?

- Varies for each VM and distro types. In my setup, I have computed it as **3895807**.

![QandA](resources/9.q_and_a.png)

## Unlicense

[![License: Unlicense](https://camo.githubusercontent.com/a0f44681d578ce545f4614325d26eac4036b273d21a61de5293af355cb969bac/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f6c6963656e73652d556e6c6963656e73652d626c75652e737667)](http://unlicense.org/)

## References

- [VMWare KB 76918: VMware workstation with hyper-v or device/credential guard enabled](https://kb.vmware.com/s/article/76918)
- [Disable Side Channel Mitigation in VMware Workstation (windowsloop.com)](https://windowsloop.com/disable-side-channel-mitigation-in-vmware/#:~:text=1%20First%2C%20open%20the%20VMware%20application.%20You%20can,re-open%20VMware%20Workstation%20to%20fully%20apply%20the%20changes.)
- [Ostechnix.com How-to-enable-nested-virtualization-in-kvm-in-linux](https://ostechnix.com/how-to-enable-nested-virtualization-in-kvm-in-linux/)
- [Create VM using VMM on Ubuntu](https://www.server-world.info/en/note?os=Ubuntu_22.04&p=kvm&f=3)

- [RDTSC](https://www.felixcloutier.com/x86/rdtsc)

- [BITWISE OPERATORS](https://www.programiz.com/c-programming/bitwise-operators)
