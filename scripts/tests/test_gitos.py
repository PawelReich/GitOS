import os
import sys
import pytest
import pexpect

DISK_PATH = os.environ.get("DISK_IMAGE_PATH", "../../build/disk.bin")
QEMU_CMD = f"qemu-system-i386 -drive format=raw,file={DISK_PATH} -nographic -no-reboot -m 32M"

@pytest.fixture
def qemu():
    if not os.path.exists(DISK_PATH):
        pytest.fail(f"Disk image not found at: {DISK_PATH}")

    child = pexpect.spawn(QEMU_CMD, encoding='utf-8')
    child.logfile = sys.stdout
    child.timeout = 5

    child.expect("Booting from Hard Disk...")

    yield child

    child.close()

def test_kernel_boots(qemu):
    qemu.expect("GitOS - operating system as exercise.")

def test_testsuite_starts(qemu):
    qemu.expect("GitOS TestSuite")

def test_printf(qemu):
    qemu.expect("test_printf: 12345 deadbeef")
