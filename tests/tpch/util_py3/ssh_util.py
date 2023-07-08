# Natacha Crooks - 2014
# Contails utility function related to SSH
############################################

import os
import subprocess
import sys
import threading
import textwrap
from tqdm import tqdm

# Functions include
# executeCommand
# executeCommandNoCheck
# executeCommandWithOutputReturn
# executeRemoteCommand
# executeRemoteCommandNoCheck
# executeRemoteCommandWithOutputReturn
# executeSequenceBlockingRemoteCommand
# executeParallelBlockingRemoteCommand
# executeNonBlockingRemoteCommand
# executeNonBlockingCommand
# killRemoteProcess
# mkdirRemote
# mkdirRemoteHosts
# rmDirRemote
# rmDirRemoteHosts
# rmDirRemoteIfExists
# rmFileRemoteIfExists
# setupTC/deleteTC
# gitPull
# svnUp
# getDirectory
# getDirectoryHosts
# getFile
# getFileHosts

# Executes a command, call is blocking
# Throws a CalledProcessError if
# doesn't succeed.
# Returns the result of the call
def executeCommandWithOutputReturn(command, printCmd=True):
    if printCmd:
        print(command)
    p=subprocess.Popen(command,shell=True,stdout=subprocess.PIPE)
    out, err = p.communicate()
    return out.rstrip()

# Executes command on remote host (synchronously).
# Return output of the process
def executeRemoteCommandWithOutputReturn(host, command, key=None, flags=""):
    flags = "" if len(flags) == 0 else flags + " "
    if not key:
        cmd = "ssh -o StrictHostKeyChecking=no -t " + flags + host + " \"" + command + "\""
    else:
        cmd = "ssh -o StrictHostKeyChecking=no -t -i " + key + " " + flags + host + " \"" + command + "\""
    return executeCommandWithOutputReturn(cmd)

# Returns network interface of the remote host
def getRemoteNetInterface(host, key=None):
	cmd = "ifconfig | awk \'NR==1{print \$1 }\'"
	return executeRemoteCommandWithOutputReturn(host,cmd,key)

def executeCommand(command, printfn=print, printCmd=True, prefix=""):
    if printCmd:
        printfn("Calling %s" % command)
    if prefix:
        prefix = " (%s)" % prefix
    try:
    	#subprocess.check_call(command, shell=True, stdout=stdout, stderr=stdout)
        # sp = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        # if sp.stdout:
        #     printfn(str(sp.stdout, "utf-8"))
        with subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as p:
            for line in iter(p.stdout.readline, b''):
                printfn('>>>%s %s' % (prefix, str(line, "utf-8").rstrip()))
    except Exception as e:
        printfn("Terminated " +  command + " " + str(e))


# Executes a command, call is blocking
# Does not check for errors
def executeCommandNoCheck(command, printfn=print):
    printfn(("Calling " + command))
    try:
        subprocess.call(command, shell=True, stdout=sys.stdout, stderr=sys.stdout)
    except Exception as e:
        printfn("Terminated " + command + " " + str(e))

# Executes command on remote host. Call is synchronous
def executeRemoteCommand(host, command, key=None, flags="", printfn=print):
    flags = "" if len(flags) == 0 else flags + " "
    if not key:
        cmd = "ssh -o StrictHostKeyChecking=no -t " + flags + host + " \"" + command + "\""
    else:
        cmd = "ssh -o StrictHostKeyChecking=no -i " + key + " " + flags + host + " \"" + command + "\""
    executeCommand(cmd, printfn)


# Executes command on remote host without
# waiting for reply
def executeRemoteCommandNoCheck(host, command, key=None, printfn=print, tty=False):
    tty_flag = ""
    if tty:
        tty_flag = "-t"
    key_flag = ""
    if key:
        key_flag = f"-i {key}"
    cmd = f"ssh -o StrictHostKeyChecking=no {tty_flag} {key_flag} {host} \"{command}\""
    #printfn("[" + cmd + "]")
    executeCommand(cmd, printfn)


# Executes, in sequence, specified command
# on each of the hosts in the list
# If returns an error throws an exception
def executeSequenceBlockingRemoteCommand(hosts, command, key=None):
    for h in hosts:
        if not key:
            cmd = "ssh -o StrictHostKeyChecking=no -t " + h + "'" + command + "'"
        else:
            cmd = "ssh -o StrictHostKeyChecking=no -t -i " + \
                  key + " " + h + "'" + command + "'"
        subprocess.check_call(cmd, shell=True)


# Executes, in parallel, specified comm)nd
# on each of the hosts in the list
# If returns an error throws an exception
def executeParallelBlockingRemoteCommand(hosts, command, key=None, printCmd=True):
    thread_list = list()
    for h in hosts:
        if not key:
            cmd = "ssh -o StrictHostKeyChecking=no -t " + h + " '" + command + "'"
        else:
            cmd = "ssh -o StrictHostKeyChecking=no -i " + \
                  key + " " + h + " '" + command + "'"
        t = threading.Thread(target=executeCommand, args=(cmd, tqdm.write, printCmd, h))
        thread_list.append(t)
    for t in thread_list:
        t.start()
    for t in tqdm(thread_list, desc="[CMD: %s]" % command):
        t.join()


# Executes a remote command in a new thread
# Does not check the error code returned
def executeNonBlockingRemoteCommand(host, command, key=None, printfn=print, tty=False):
    t = threading.Thread(target=executeRemoteCommandNoCheck,
        args=(host, command, key, printfn, tty))
    t.start()


# Executes a command in a new thread
def executeNonBlockingCommand(command, printfn=print, printCmd=True):
    t = threading.Thread(target=executeCommand,
        args=(command, printfn, printCmd))
    t.start()


def executeCommandNoOutput(command, printfn=print, printCmd=True, prefix=""):
    if printCmd:
        printfn("Calling %s" % command)
    if prefix:
        prefix = " (%s)" % prefix
    try:
        subprocess.Popen(command, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
 
    except Exception as e:
        printfn("Terminated " +  command + " " + str(e))

def executeNonBlockingCommandNoOutput(command,printfn=print,  printCmd=True):
    t = threading.Thread(target=executeCommandNoOutput,
        args=(command, printfn,printCmd))
    t.start()

# Setups TC. TC is a network tool to simulate latency. This call will execute on the remote host and
# setup simulated latency between the remote host and the destHosts
def setupTC(host,latency,destHosts,key=None):
    max_bandwidth="10gibps"
    latency = latency/2
    interface = getNetInterface(host,key)
    command = 'sudo tc qdisc del dev %s root; ' % interface
    command += 'sudo tc qdisc add dev %s root handle 1: htb; ' % interface
    command += 'sudo tc class add dev %s parent 1: classid 1:1 htb rate %s; ' % (interface, max_bandwidth) # we want unlimited bandwidth
    idx = 2
    for d in destHosts:
        command += 'sudo tc class add dev %s parent 1:1 classid 1:%d htb rate %s; ' % (interface, idx, max_bandwidth)
        command += 'sudo tc qdisc add dev %s handle %d: parent 1:%d netem delay %dms; ' % (interface, idx, idx, latency)
        command += 'sudo tc filter add dev %s pref %d protocol ip u32 match ip dst %s flowid 1:%d; ' % (interface, idx, d, idx)
        idx += 1
    print("----------")
    print(command)
    print("----------")
    executeRemoteCommand(host,command,key)

# Deletes TC commands
def deleteTC(host, destHost,key=None):
    interface = getNetInterface(host,key)
    command = 'sudo tc qdisc del dev %s root; ' % interface
    executeRemoteCommand(host,command,key)


## Executes a command asynchronously.
# Call is asynchronous, output is piped
# Args are supplied as a list of args
def startProcess(args):
    return subprocess.Popen(args, stdout=PIPE, stderr=PIPE)


# Kills Process that matches string (using grep)
# Throws CalledProcessError for exception
def killRemoteProcess(host, process, user, key=None):
    try:
        cmd = "kill $(ps aux | grep " + process + \
              " | grep -v grep |  awk '{print $2}') "
        cmd = "killall " + process
        executeRemoteCommand(host, cmd, key)
    except Exception as e:
        print("Killed " + str(e) + " " + cmd)


def mkdirCmd(directory):
    return "mkdir -p %s" % directory

# Creates Directory on remote host
def mkdirRemote(host, directory, key=None):
    if not key:
        cmd = "ssh -o StrictHostKeyChecking=no " + host + " \'mkdir -p " + directory + " \'"
    else:
        cmd = "ssh -o StrictHostKeyChecking=no -i " + key + \
              " " + host + " \'mkdir -p " + directory + " \'"
    executeCommand(cmd)

# Creates Directory on remote host
def mkdirRemote(username, host, directory, key=None):
    executeCommand(mkdirRemoteCmd(username, host, directory, key))

def mkdirRemoteCmd(username, host, directory, key=None):
    if not key:
        cmd = f"ssh -o StrictHostKeyChecking=no {username}@{host} 'mkdir -p {directory}'"
    else:
        cmd = f"ssh -o StrictHostKeyChecking=no -i {key} {username}@{host} 'mkdir -p {directory}'"
    return cmd


# Creates Directory on remote hosts
def mkdirRemoteHosts(hosts, directory, key=None):
    for host in hosts:
        mkdirRemote(host, directory, key)

# Creates Directory on remote hosts
def mkdirRemoteHosts(username, hosts, directory, key=None):
    directory = str(directory)
    threads = []
    for host in hosts:
        cmd = mkdirRemoteCmd(username, host, directory, key)
        t = executeNonBlockingCommand(cmd, tqdm.write)
        t.start()
        threads.append(t)
    short_d = directory
    max_d_len = 30
    if len(directory) > max_d_len:
        short_d = "..."+short_d[len(directory)-max_d_len:]
    for t in tqdm(threads, desc="[CMD: mkdir %s]" % short_d):
        t.join()


# Deletes remote dir, command fails if it does
# not exist
def rmdirRemoteHosts(hosts, directory, key=None):
    print(hosts)
    for host in hosts:
        try:
            rmdirRemote(host, directory, key)
        except Exception as e:
            print("Directory did not exist")


# Deletes remote dir, command fails if it does not exist
def rmdirRemote(host, directory, key=None):
    if not key:
        cmd = "ssh -o StrictHostKeyChecking=no " + host + " \'rm -r " + directory + "\'"
    else:
        cmd = "ssh -o StrictHostKeyChecking=no -i " + key + " " + host + " \'rm -r " + directory + "\'"
    executeCommand(cmd)


# Deletes remote dir, if it exists, otherwise, do nothing
def rmdirRemoteIfExists(host, directory, key=None):
    if not key:
        cmd = "ssh -o StrictHostKeyChecking=no " + host + " \'rm -r " + directory + "\'"
    else:
        cmd = "ssh -o StrictHostKeyChecking=no -i " + key + " " + host + " \'rm -r " + directory + "\'"
    executeCommandNoCheck(cmd)


# Deletes remote dir, if it exists, otherwise, do nothing
def rmfileRemoteIfExists(host, filee, key=None):
    if not key:
        cmd = "ssh -o StrictHostKeyChecking=no " + host + " \'rm " + filee + "\'"
    else:
        cmd = "ssh -o StrictHostKeyChecking=no -i " + key + " " + host + " \'rm " + filee + "\'"
    executeCommandNoCheck(cmd)


# Updates repository (GIT)
def gitPull(directory, remote="origin", branch="main"):
    cwd = os.getcwd()
    os.chdir(directory)
    cmd = "git stash; git pull %s %s" % (remote, branch)
    executeCommand(cmd)
    os.chdir(cwd)

def gitSetOriginCmd(directory, origin):
    cmd = "cd " + directory + "; git remote set-url origin git@github.com:" + origin
    return cmd

def gitRemoteSetOrigin(directory, origin, host, key):
    cmd = "ssh -o StrictHostKeyChecking=no -i " + key + " -A " + host + "'" + gitSetOriginCmd(directory, origin) + "'"
    executeCommand(cmd)

def gitPullCmd(directory, remote="origin", branch="main"):
    cmd = "cd " + directory + "; git pull %s %s" % (remote, branch)
    return cmd

def gitPullRemote(directory, host, key, remote="origin", branch="main"):
    cmd = "ssh -o StrictHostKeyChecking=no -i " + key + " -A " + host + "'" + gitPullCmd(directory, remote, branch) + "'"
    executeCommand(cmd)

# Updates repository (SVN)
def svnUp(directory):
    cwd = os.getcwd()
    os.chdir(directory)
    cmd = "svn up"
    executeCommand(cmd)

# Downloads folder remote_dir from all hosts in to local_dir
def getDirectory(local_dir, hosts, remote_dir, key=None, printfn=print):
    printfn(str(hosts))
    for h in hosts:
        if not key:
            cmd = "scp -o StrictHostKeyChecking=no -r " + \
                  h + ":" + remote_dir + " " + local_dir
        else:
            cmd = "scp -o StrictHostKeyChecking=no -i " + key + \
                  " -r " + h + ":" + remote_dir + " " + local_dir
        executeCommand(cmd, printfn)

def getDirectoryCmd(local_dir, username, host, remote_dir, key=None):
    if not key:
        cmd = f"scp -o StrictHostKeyChecking=no -r {username}@{host}:{remote_dir} {local_dir}"
    else:
        cmd = f"scp -o StrictHostKeyChecking=no -i {key} -r {username}@{host}:{remote_dir} {local_dir}"
    return cmd

# Downloads folder remote_dir from all hosts in to local_dir
def getDirectory(local_dir, username, hosts, remote_dir, key=None, printfn=print):
    threads = []
    for h in hosts:
        cmd = getDirectoryCmd(local_dir, username, h, remote_dir, key)
        t = executeNonBlockingCommand(cmd, printfn=printfn, printCmd=False)
        t.start()
        threads.append(t)
    return threads

# Downloads file from all hosts into local_dir
def getFile(local_dir, hosts, remote_file, key=None):
    for h in hosts:
        if not key:
            cmd = f"scp  -o StrictHostKeyChecking=no {h}:{remote_file} {local_dir}"
        else:
            cmd = f"scp  -o StrictHostKeyChecking=no -i {key} {h}:{remote_file} {local_dir}"
        executeCommand(cmd)


# Sends file to remote host
def sendFile(local_file, h, remote_dir, key=None, printfn=print):
    if not key:
        cmd = f"scp -o StrictHostKeyChecking=no {local_file} {h}:{remote_dir}"
    else:
        cmd = f"scp -o StrictHostKeyChecking=no -i {key} {local_file} {h}:{remote_dir}"
    executeCommand(cmd, printfn)


def sendFileCmd(local_file, h, username, remote_dir, key=None):
    if not key:
        cmd = f"scp -o StrictHostKeyChecking=no {local_file} {username}@{h}:{remote_dir}"
    else:
        cmd = f"scp -o StrictHostKeyChecking=no -i {key} {local_file} {username}@{h}:{remote_dir}"
    return cmd

# Sends file to remote host
def sendFile(local_file, h, username, remote_dir, key=None, printfn=print, printCmd=True):
    executeCommand(sendFileCmd(local_file, h, username, remote_dir, key), printfn, printCmd=printCmd)


# Sends file to list of remote hosts
def sendFileHosts(local_file, hosts, remote_dir, key=None):
    threads = []
    for h in hosts:
        if not key:
            cmd = f"scp -o StrictHostKeyChecking=no {local_file} {h}:{remote_dir}"
        else:
            cmd = f"scp -o StrictHostKeyChecking=no -i {key} {local_file} {h}:{remote_dir}"
        t = executeNonBlockingCommand(cmd)
        t.start()
        threads.append(t)
    for t in threads:
        t.join()

# Sends file to list of remote hosts
def sendFileHosts(local_file, username, hosts, remote_dir, key=None, join_threads=True, printCmd=True):
    threads = []
    for h in hosts:
        cmd = sendFileCmd(local_file, h, username, remote_dir, key)
        #executeCommand(cmd)
        t = executeNonBlockingCommand(cmd, tqdm.write, printCmd=printCmd)
        t.start()
        threads.append(t)
    if join_threads:
        for t in tqdm(threads, desc="[CMD: scp %s]" % local_file):
            t.join()
    else:
        return threads



# Install list of packages on list of hosts
def installPackages(hosts, package_list, key=None, assumeTrue = True):
    for package in package_list:
        if (assumeTrue):
            cmd = "sudo apt-get --yes install " + package
        else:
            cmd = "sudo apt-get install " + package
        try:
            executeParallelBlockingRemoteCommand(hosts, cmd, key)
        except Exception as e:
            print("Failed to install package " + package + " " + str(e))

def bordered(*lines, side_border=True):
    rows, _ = os.popen('stty size', 'r').read().split()
    rows = int(rows)
    padding = 1
    width = max(len(s) for s in lines)
    total_width = width + padding*2
    if total_width > rows:
        total_width = rows
        if side_border:
            width = total_width - 2*padding
    wrapped_lines = []
    for l in lines: 
        wrapped_lines.extend(textwrap.wrap(l, width=width, subsequent_indent='  '))
    res = ['', '+' + '-' * (total_width) + '+']
    for s in lines:
        if side_border:
            res.append('|' + ' '*padding + (s + ' ' * width)[:width] + ' '*padding + '|')
        else:
            res.append(' '*padding + s)
    res.append('+' + '-' * (total_width) + '+')
    return '\n'.join(res)