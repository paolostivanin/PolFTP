FTPUtils
========
FTPUtils è sviluppato come progetto per corso di Reti di Calcolatori tenutosi presso il DTI di Crema durante l'anno scolastico 2011/2012.
Esso è composto da un server e un client FTP che supportano le funzioni: SYST - LIST - PWD - CWD - RETR - STOR - DELE - MKD - RMD - RNM.<br>
Lo studente sviluppatore del progetto è:

* Paolo Stivanin (SSRI in presenza, DM270)


Versione
--------
La versione **stabile più recente** è la **v1.0-rc** scaricabile da questo link: https://github.com/polslinux/FTPUtils/zipball/v1.0-rc<br>
Se invece si desidera utilizzare la versione in sviluppo bisognare dare i seguenti comandi:<br>

- `git clone https://github.com/polslinux/FTPUtils.git`<br>
- `cd FTPUtils`<br>
- `git checkout experimental`<br>

e quindi seguire le istruzioni riportate in *Compilazione (dal punto 3)*.

Requisiti
---------

* GCC/Clang	: per compilare i programmi (per Clang **obbligatoria versione >= 3.1**, per GCC consigliata v4.4.0 e maggiori)
* Make      	: per lanciare il Makefile
* GNU/Linux	: i software sono stati sviluppati su ambienti GNU/Linux e quindi **NON** compileranno su ambienti MS Windows e, *molto probabilmente*, nemmeno su ambienti MAC OSX.
* Build Tools	: ovvero Autoconf e Automake **necessari** per controllare le dipendenze richieste durante la compilazione

Compilazione
------------
* Clonare il repository<br>
`git clone https://github.com/polslinux/FTPUtils.git`<br>
* entrare nella root del repository appena clonato:<br>
`cd $PWD/FTPUtils`<br>
* dare i permessi di esecuzione allo script autogen.sh:<br>
`chmod 744 autogen.sh`<br>
* infine eseguire lo script sopra citato con:<br>
`./autogen.sh`<br>
Verranno quindi creati due files: ftpclient *(in src/client)* e ftpserver *(in src/server/concurrent)*.

Come usare client e server
--------------------------
Il server usa la chiamata di sistema "fork" per generare un nuovo processo ad ogni connessione (max 20 connessioni).<br>
Per avviare il server basta quindi dare il seguente comando:<br>
`./ftpserver <numporta>`<br>
e similarmente per il client:<br>
`./ftpclient <host> <numporta>`

Autenticazione
--------------
Il server supporta una forma di autenticazione basata su un file di testo contenente il nome utente e lo sha256 (ripetuto 50'000 volte) della password.<br>
Tale file di testo **deve** chiamarsi `auth` e **deve** trovarsi in `/etc/ftputils/` (percorso completo: `/etc/ftputils/auth`).<br>
Prima di usare il server bisogna quindi **aggiungere nel file auth il nome utente e l'hash della password** delle persone a cui si vuole concedere l'accesso FTP. Le informazioni devono essere inserite nel file nella seguente forma:<br>
`USERNAME HASH`<br>
`USERNAME HASH`<br>

Generazione hash password
-------------------------
Per generare l'hash della password bisogna **prima** compilare i sorgenti *(per il come vedere sopra)* e **poi** spostarsi nella cartella `src` del progetto e dare il seguente comando: `./generate_sha256`. A questo punto basta inserire 2 volte la password e ne verrà generato l'hash.

Licenza
-------
**Tutto** il codice presente è rilasciato sotto licenza **GNU AGPL v3** e maggiori.<br>
<http://www.gnu.org/licenses/agpl-3.0.html><br>

Sito web
--------
Sito web dello sviluppatore:	<http://www.polslinux.it><br>
Sito web del progetto:			<https://github.com/polslinux/FTPUtils>
