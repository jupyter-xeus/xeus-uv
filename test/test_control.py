import pytest

from jupyter_client.kernelspec import KernelSpecManager
from jupyter_client import KernelManager
import time
from pathlib import Path

THIS_DIR = Path(__file__).parent
KERNEL_DIR = THIS_DIR / "kernels"

print("KERNEL_DIR:", KERNEL_DIR)

ksm = KernelSpecManager()
ksm.kernel_dirs = [str(KERNEL_DIR)]

def test_kernel_shutdown():

    ksm = KernelSpecManager()
    ksm.kernel_dirs = [str(KERNEL_DIR)]


    km = KernelManager(kernel_name="test_kernel")
    km.start_kernel()

    kc = km.client()
    kc.start_channels()

    # Send shutdown_request explicitly
    kc.shutdown(restart=False)

    # Give the kernel time to respond and exit
    time.sleep(1)

    # Assert kernel is no longer alive
    assert not km.is_alive()

    kc.stop_channels()



def test_shutdown_reply_message():
    km = KernelManager(kernel_name="test_kernel")
    km.start_kernel()
    kc = km.client()
    kc.start_channels()

    kc.control_channel.send(
        kc.session.msg("shutdown_request", {"restart": False})
    )

    reply = kc.control_channel.get_msg(timeout=5)

    assert reply["header"]["msg_type"] == "shutdown_reply"
    

    kc.stop_channels()
