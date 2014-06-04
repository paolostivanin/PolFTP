FTPUtils
========
FTPUtils is a project that is made by a FTP client and a FTP (concurrent) server.<br>
The supported functions are: SYST - LIST - PWD - CWD - RETR - STOR - DELE - MKD - RMD - RNM.<br>
It is developed by:

* Paolo Stivanin a.k.a. Polslinux


Version
-------
The **last and stable** version is the **none**


RoadMap
-------
See the ChangeLog file


Requirements
------------
* GCC (>=4.6.0) or Clang (>= 3.2)
* Make
* GNU/Linux
* Gcrypt (>=1.5.0)

Compiling
---------
`git clone https://github.com/polslinux/FTPUtils.git`<br>
`cd /path/to/FTPUtils`<br>
`make`<br>
When the script finished you will find the binaries `ftpclient` and `ftpserver` inside the project root directory.


How to use ftpclient and ftpserver
----------------------------------
`ftpserver` uses the syscall `fork` to generate a new process when a connection arrive **(max 20)**.<br>
To use the `ftpserver`:<br>
`./ftpserver <port number>`<br>
and to use the `ftpclient`:<br>
`./ftpclient <host> <port number>`


Authentication
--------------
The server read the allowed USER (and its PASS) from a file named `auth` located in `/etc/ftputils/`.<br>
The password isn't written in clear text. You must write the hash of the password repeated 50'000 times.<br>
**Before** using the server **you must add the USER and the H(PASS)** inside the file `/etc/ftputils/auth` in this way:<br>
`USERNAME HASH`<br>
`USERNAME HASH`<br>


Generate password hash
----------------------
You must generate the hash of the password if you want to login into the `ftpserver`.<br>
Follow these simple steps to do it:<br>

* Compile the sources
* `cd src` and then execute `./generate_sha256`
* Type and re-type your password and then write the hash into the `auth` file _(near the username)_.

License
-------
All the code is released under the license GNU AGPL v3 and above.<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>


Web Site
--------
Developer web site:	<https://www.paolostivanin.com><br>
Project web site:	<https://github.com/polslinux/FTPUtils>
