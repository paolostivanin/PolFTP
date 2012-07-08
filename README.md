FTPUtils
========

FTPUtils è sviluppato come progetto per corso di Reti di Calcolatori tenutosi presso il DTI di Crema durante l'anno scolastico 2011/2012.
Esso è composto da un server e un client FTP che supportano solo le funzioni base quali USER, PASS, LIST, PWD, CD, RETR. 
L'interfaccia del client (se sarà sviluppata completamente) è sviluppata con il toolkit GTK+.<br>
Lo studente sviluppatore del progetto è:

* Paolo Stivanin

Requisiti
---------

* GCC		: per compilare i programmi (consigliata versione >= 4.4.0)
* Make      : per lanciare il Makefile
* GTK+	 	: per l'interfaccia grafica (**obbligatoria versione >=3.4.0**)
* GNU/Linux	: i software sono stati sviluppati su ambienti GNU/Linux e quindi **NON** compileranno su ambienti MS Windows. Per quanto riguarda ambienti Mac OS X non ho la possibilità di verificare anche se la presenza di GCC 4.2.0 potrebbe causare problemi in fase di compilazione.

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
`./ftpclient <host> <numporta> <username> <password>`

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
Sito web del progetto:		 	<https://github.com/polslinux/FTPUtils>
