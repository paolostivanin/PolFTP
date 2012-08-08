FTPUtils
========

FTPUtils è sviluppato come progetto per corso di Reti di Calcolatori tenutosi presso il DTI di Crema durante l'anno scolastico 2011/2012.
Esso è composto da un server e un client FTP che supportano solo le funzioni base quali USER, PASS, SYST, PWD, CD, LIST, RETR, DELETE, MKDIR, RMDIR. 
L'interfaccia del client (se sarà sviluppata completamente) è sviluppata con il toolkit GTK+.<br>
Lo studente sviluppatore del progetto è:

* Paolo Stivanin (SSRI in presenza, DM270)

Requisiti
---------

* GCC/Clang	: per compilare i programmi (per Clang **obbligatoria v3.1**, per GCC consigliata v4.4.0 e maggiori)
* Make          : per lanciare il Makefile
* GTK+	 	: (facoltativo) per l'interfaccia grafica (**obbligatoria versione >=3.4.0**)
* GNU/Linux	: i software sono stati sviluppati su ambienti GNU/Linux e quindi **NON** compileranno nè su ambienti MS Windows nè su ambienti MAC OSX.

Compilazione
------------
* Clonare il repository<br>
`git clone https://github.com/polslinux/FTPUtils.git`<br>
* entrare nella root del repository appena clonato:<br>
`cd $PWD/FTPUtils`<br>
* infine dare il comando make:<br>
`make`<br>
Verranno quindi creati due files: ftpclient e ftpserver.

Come usare client e server
--------------------------
Attualmente il server è solo iterativo (1 connessione alla volta) ma è in sviluppo anche il server parallelo che userà la chiamata "fork"
`./ftpserver <numporta>`<br>
`./ftpclient <host> <numporta>`

Licenza
-------

Il software è rilasciato sotto due tipi di licenza, una per il client e l'altra per il server.<br>
Il client FTP è rilasciato sotto licenza GPL:<br>
<http://www.gnu.org/licenses/gpl-3.0.html><br>
mentre il server FTP è rilasciato sotto licenza AGPL:<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>

Sito web
--------

Sito web dello sviluppatore:	<http://www.polslinux.it><br>
Sito web del progetto:		<https://github.com/polslinux/FTPUtils>
