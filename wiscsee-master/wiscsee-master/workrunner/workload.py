import collections
import os
import pprint
import random
import time
import sys
import subprocess

from commons import *
import config
import multiwriters
from utilities import utils

from pyreuse.helpers import *
from pyreuse.apputils.parseleveldboutput import parse_file

class Workload(object):
    def __init__(self, confobj, workload_conf_key = None):
        """
        workload_conf is part of confobj. But we may need to run
        multiple workloads with different configurations in our
        experiements. So we need workload_conf to specify which
        configuration we will use in a Workload instance.

        Since workload_conf has a default value, it should be
        compatible with previous code. However, new classes based
        one Workload should use this new __init__() with two parameters.
        """
        if not isinstance(confobj, config.Config):
            raise TypeError("confobj is not of type class config.Config".
                format(type(confobj).__name__))

        self.conf = confobj
        if workload_conf_key != None and workload_conf_key != 'None':
            self.workload_conf = confobj[workload_conf_key]

    def run(self):
        raise NotImplementedError

    def stop(self):
        raise NotImplementedError

class NoOp(Workload):
    """
    This is a workload class that does nothing. It may be used to skip
    the file system aging stage. To skip aging workload, set
    conf['age_workload_class'] = "NoOp"
    """
    def run(self):
        pass

    def stop(self):
        pass


class SimpleRandReadWrite(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(SimpleRandReadWrite, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        datafile = os.path.join(mnt, "datafile")
        region = 2 * MB
        chunksize = 64 * KB
        n_chunks = region / chunksize
        chunkids = range(n_chunks)

        buf = "a" * chunksize
        f = open(datafile, "w+")
        random.shuffle(chunkids)
        for chunkid in chunkids:
            offset = chunkid * chunksize
            f.seek(offset)
            f.write(buf)
            os.fsync(f)

        random.shuffle(chunkids)
        for chunkid in chunkids:
            offset = chunkid * chunksize
            f.seek(offset)
            buf = f.read()
            os.fsync(f)

        f.close()

    def stop(self):
        pass

##########################DIR DEL#################################
class Directory_Purge(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(Directory_Purge, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/directory_purge".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass
        
##################################################################

####################REQUEST SCALE#################################
class LinuxRequestScaleAgingWrite(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxRequestScaleAgingWrite, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "../../request_size_aging_workload".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass


class LinuxRequestScaleWorkloadPositive(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxRequestScaleWorkloadPositive, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "../../request_size_workload_positive".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass

class LinuxRequestScaleWorkloadNegative(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxRequestScaleWorkloadNegative, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "../../request_size_workload_negative".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass
##################################################################

#########################LOCALITY#################################
class LinuxLocalityAgingWrite(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxLocalityAgingWrite, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/locality_aging_workload".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass


class LinuxLocalityWorkloadPositive(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxLocalityWorkloadPositive, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/locality_workload_positive".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass

class LinuxLocalityWorkloadNegative(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxLocalityWorkloadNegative, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/locality_workload_negative".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass

##################################################################

#########################DEATH TIME###############################
class LinuxDeathAgingWrite(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxDeathAgingWrite, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/death_group_aging_workload".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass


class LinuxDeathWorkloadPositive(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxDeathWorkloadPositive, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/death_group_workload_positive".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass

class LinuxDeathWorkloadNegative(Workload):
    def __init__(self, confobj, workload_conf_key = None):
        super(LinuxDeathWorkloadNegative, self).__init__(confobj, workload_conf_key)

    def run(self):
        mnt = self.conf["fs_mount_point"]
        cmd = "~/WiscSee_Clean/death_group_workload_negative".format(mnt)
        print cmd
        subprocess.call(cmd, shell=True)
        subprocess.call("sync")

    def stop(self):
        pass
##################################################################

# class LinuxDD(Workload):
    # def __init__(self, confobj, workload_conf_key = None):
        # super(LinuxDD, self).__init__(confobj, workload_conf_key)

    # def run(self):
        # mnt = self.conf["fs_mount_point"]
        # cmd = "dd if=/dev/zero of={}/datafile bs=64k count=128".format(mnt)
        # print cmd
        # subprocess.call(cmd, shell=True)
        # subprocess.call("sync")

    # def stop(self):
        # pass


