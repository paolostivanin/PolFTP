FTPUtils
========
FTPUtils is a project that is made by a FTP client and a FTP (concurrent) server.<br>
The supported functions are: SYST - LIST - PWD - CWD - RETR - STOR - DELE - MKD - RMD - RNM.<br>
It is developed by:

* Paolo Stivanin a.k.a. Polslinux


Version
-------
The **last and stable** version is the **1.0.1** and you can download it from: https://github.com/polslinux/FTPUtils/zipball/v1.0.1<br>
If you want to use the *dev release* you have to do the following things:<br>

- `git clone https://github.com/polslinux/FTPUtils.git`<br>
- `cd FTPUtils`<br>
- `git checkout experimental`<br>

and then follow the instructions that are in *Compiling (from number 3)*.

Requirements
------------

* GCC or Clang	_(if you use Clang you **must use** a version **>= 3.1**)_
* Make
* GNU/Linux		_(FTPUtils **will not** run under MS Windows and maybe it **won't** run also under MAC OSX)_
* Build Tools	_(Autoconf and Automake)_

Compiling
---------
* Clone the repo<br>
`git clone https://github.com/polslinux/FTPUtils.git`<br>
* move inside the directory you've just downloaded:<br>
`cd $PWD/FTPUtils`<br>
* give the right permission to autogen.sh:<br>
`chmod 744 autogen.sh`<br>
* and now execute the script:<br>
`./autogen.sh`<br>
When the script finished you will find `ftpclient` and `ftpserver` in the project root directory.

How to use ftpclient and ftpserver
----------------------------------
`ftpserver` uses the syscall `fork` to generate a new process when a connection arrives **(max 20)**.<br>
To use the `ftpserver`:<br>
`./ftpserver <port number>`<br>
and to use the `ftpclient`:<br>
`./ftpclient <host> <port number>`

Authentication
--------------
The server will read the USER and the PASS from a file named `auth` located in `/etc/ftputils/`.<br>
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
Developer web site:	<http://www.polslinux.it><br>
Project web site:	<https://github.com/polslinux/FTPUtils>
