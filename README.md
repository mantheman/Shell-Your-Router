# Shell-Your-Router
C backdoor with shell and tftp servers, specifically meant to **run on Linux** routers.

## Installation
Cloning the repo:
```bash
git clone https://github.com/ykrinsky/Shell-Your-Router.git
```
You can compile the C code found at /backdoor with a simple make:
```bash
cd backdoor
# For using gcc as your compiler - recommended.
make
# For using mipsel-unkown-linux-uclibc as your compiler.
make release 
```
You should run the backdoor with root privileges, as it's opens a socket on port 69 (TFTP server).

# Shell
After running the backdoor, you can connect to it's host with the beloved netcat.
* The backdoor opens shell server that listen to new connections on **port 1337**.
* The backdoor supports many shell connections in parallel.
* The shell connection is based on TCP and it uses plain text for communication.
* The log of the shell server can be found at /tmp/bd.log.

# TFTP Server
The backdoor has 2 processes, one for handling new shell connections and one for handling TFTP (Trivial File Transfer Protocol) requests. 
* The TFTP server listens for requests on port 69.
* The server can handle requests of both uploading and downloading files.
* The server supports one connection at a time.
* The log of the TFTP server can be found at /tmp/tftp.log.

# Edimax BR-6524n
I developed this backdoor as I wanted to learn more on how embedded linux machines works, and specifically my router (Edimax BR-6524n).

## Scripts
### update_checksum.py
After modifying Edimax BR-6524n firmware, you need to fix it's checksum otherwise the web interface won't allow you to upload it. This script fixes the checksum of a given firmware file.

Before running this script, you need to run:
```bash
pip3 install construct click
```
### build_firmware.sh
I've used [Firmware-Mod-Kit](https://github.com/rampageX/firmware-mod-kit) to extract and build my router's firmware. 
build_firmware.sh is a script I wrote that compiles the backdoor, builds the firmware (with Firmware-Mod-Kit) and updates it's checksum. 

This script saved me a lot of time. If you want to use it, you just need to change a few global variables at the start of the script.

## General Notes

### Starting Backdoor at Boot
If you want to run your backdoor after boot of Edimax BR-6524 (otherwise you won't be able to run it), you need to add the following line to /etc/profile:
```bash
/bin/bd &
```
Add this line after the start of the router's web server.

### Clearing Space in the Firmware
As Firmware-Mod-Kit suggests - you don't want to increase your firmware size. So to add my backdoor, I needed to clear some space by running:
```bash
# Clears ~400 KB
mips-linux-gnu-strip rootfs/bin/rt2860ap.o 
```
And then you can use the backdoor to upload more files after the router is running, as the router has 4 MB flash and 16 MB RAM.

### Toolchain
mipsel-unknown-linux-uclibc.zip is a toolchain I created with [crosstool-NG](https://github.com/crosstool-ng/crosstool-ng). 

I successfully compiled my code with this toolchain and it run fine on the router, but the gdbserver created by crosstool-NG didn't work on my router. You can use this toolchain by extracting the zip file, and adding mipsel-unknown-linux-uclibc/bin to your $PATH.

Sadly I didn't found any toolchain that compiles gdbserver successfully for this router, and if someone does find/build such thing, please contact me :smile: