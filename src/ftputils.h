#ifndef FTPUTILS_H_INCLUDED
#define FTPUTILS_H_INCLUDED

#define VERSION "v1.99.1 (2.0-alpha1)"
#define BUFSIZE 4096
#define ACTBUFSIZE 384

#define LIST 30		//200 (type I/A),227 (pasv), 150 (open data conn), 226 (close data conn)
#define CWD 31		//250 <message>
#define CDUP 32		//250 <message>
#define MKD 33		//257 "<directory-name>" <message>
#define RMD 34		//250 <message>
//#define STOR 35	//200 (type I/A),227 (pasv), 150 (open data conn), 226 (close data conn)
//#define RETR 36	//200 (type I/A), 227 (pasv), 150 (open data conn), 226 (close data conn)
#define SIZE 37		//200 (type I), 213 <size-in-byte>
#define SYST 38		//215 <system-type>
#define DELE 39		//250 <message>
#define FEAT 40		//211-Features:
					//list-of-features
 					//211 End
#define PWD 41		//257 "<directory-name>"
#define TYPE 42		//200 <message> (choose between type A or type I)
#define QUIT 50		//221 <message>

#endif
