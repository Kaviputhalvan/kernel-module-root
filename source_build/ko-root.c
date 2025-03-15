#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/mount.h>
#include <linux/namei.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kaviputhalvan");
MODULE_DESCRIPTION("Kernel Module for Full Root Privileges and Read/Write Access");
MODULE_VERSION("1.0");

/* Function to elevate process privileges to root */
static void escalate_privileges(void) {
    struct cred *new_cred;
    new_cred = prepare_kernel_cred(NULL);
    if (!new_cred) {
        printk(KERN_ERR "RootModule: Failed to allocate new credentials\n");
        return;
    }

    new_cred->uid.val = 0;
    new_cred->gid.val = 0;
    new_cred->euid.val = 0;
    new_cred->egid.val = 0;
    new_cred->suid.val = 0;
    new_cred->sgid.val = 0;
    new_cred->fsuid.val = 0;
    new_cred->fsgid.val = 0;

    commit_creds(new_cred);
    printk(KERN_INFO "RootModule: Process now running as root\n");
}

/* Function to force remount root filesystem as read/write */
static int force_remount_rw(void) {
    struct path root_path;
    int ret;

    ret = kern_path("/", LOOKUP_FOLLOW, &root_path);
    if (ret) {
        printk(KERN_ERR "RootModule: Failed to get root filesystem path\n");
        return ret;
    }

    /* Force remount with read/write flags */
    ret = vfs_umount(root_path.mnt, MNT_FORCE);
    if (ret) {
        printk(KERN_ERR "RootModule: Failed to unmount root fs (code: %d)\n", ret);
        return ret;
    }

    ret = do_mount("rootfs", "/", NULL, MS_REMOUNT | MS_RDONLY, NULL);
    if (ret) {
        printk(KERN_ERR "RootModule: Failed to remount root as read/write\n");
        return ret;
    }

    printk(KERN_INFO "RootModule: Root filesystem is now read/write\n");
    return 0;
}

/* Function to modify /init.rc even if filesystem is locked */
static int modify_init_rc(void) {
    struct file *file;
    mm_segment_t old_fs;
    char *cmd = "\ninsmod /lib/modules/root_privilege_rw.ko\n";
    loff_t pos = 0;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    file = filp_open("/init.rc", O_WRONLY | O_APPEND, 0644);
    if (IS_ERR(file)) {
        printk(KERN_ERR "RootModule: Failed to open /init.rc\n");
        return PTR_ERR(file);
    }

    kernel_write(file, cmd, strlen(cmd), &pos);
    filp_close(file, NULL);
    set_fs(old_fs);

    printk(KERN_INFO "RootModule: Modified /init.rc to load module on boot\n");
    return 0;
}

/* Function to set /bin/su to root access */
static int set_su_permissions(void) {
    struct file *file;
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    file = filp_open("/bin/su", O_RDWR, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "RootModule: Failed to open /bin/su\n");
        return PTR_ERR(file);
    }

    vfs_fchmod(file, 04755);
    vfs_fchown(file, 0, 0);

    filp_close(file, NULL);
    set_fs(old_fs);

    printk(KERN_INFO "RootModule: /bin/su now has root access\n");
    return 0;
}

/* Initialization function */
static int __init root_module_init(void) {
    printk(KERN_INFO "RootModule: Loading Root Privilege RW Module\n");

    escalate_privileges();   // Elevate process privileges to root
    force_remount_rw();      // Force root filesystem to be read/write
    modify_init_rc();        // Modify init.rc for autoloading
    set_su_permissions();    // Modify /bin/su permissions

    return 0;
}

/* Cleanup function */
static void __exit root_module_exit(void) {
    printk(KERN_INFO "RootModule: Unloading Root Privilege RW Module\n");
}

module_init(root_module_init);
module_exit(root_module_exit);
