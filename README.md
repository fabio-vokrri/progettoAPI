# Progetto di Algoritmi e Principi dell'Informatica - 2023

Questa è l'implementazione della soluzione al progetto di *Algoritmi e Principi dell'Informatica* (anno scolastico 2022-2023) dello studente *Vokrri Fabio*, con un risultato di *30 con Lode*.

## Specifica di Progetto

Tale progetto prevedeva la seguente specifica:

> Si consideri un’autostrada descritta come una sequenza di stazioni di servizio.  
> Ogni stazione di servizio si trova ad una distanza dall’inizio dell’autostrada espressa in chilometri da un numero intero positivo o nullo.  
> Non esistono due stazioni di servizio aventi la stessa distanza: ogni stazione di servizio è quindi univocamente identificata dalla sua distanza dall’inizio dell’autostrada.  
> Ogni stazione di servizio è dotata di un parco veicoli elettrici a noleggio. Ogni veicolo è contraddistinto dall’autonomia data da una carica delle batterie, espressa in chilometri, da un numero intero positivo. Il parco veicoli di una singola stazione comprende al più 512 veicoli. Presa a noleggio un’auto da una stazione *s*, è possibileraggiungere tutte quelle la cui distanza da *s* è minore o uguale all’autonomia dell’automobile.  
> Un viaggio è identificato da una sequenza di stazioni di servizio in cui il conducente fa tappa. Ha quindi inizio in una stazione di servizio e termina in un’altra, passando per zero o più stazioni intermedie. Si assuma che il conducente noleggi un’auto nuova ogniqualvolta faccia tappa in una stazione di servizio *s*: il conducente è quindi in grado di raggiungere come prossima tappa una qualunque stazione di servizio a distanza da *s* minore o uguale a quella dell’automobile con autonomia massima disponibile in *s*.  
> L’obiettivo del progetto è il seguente: data una coppia di stazioni, pianificare il percorso con il minor numero di tappe tra di esse. Nel caso siano presenti più percorsi con lo stesso numero minimo di tappe (cioè a pari merito), deve essere scelto il percorso che predilige le tappe con distanza più breve dall’inizio dell’autostrada. In altre parole, si consideri l’insieme degli n percorsi a pari merito P = {p1, p2, . . . pn} dove ogni percorso è una tupla di m elementi pi = ⟨pi,1, pi,2, . . . pi,m⟩ che equivalgono alla distanza da inizio autostrada di ogni tappa in ordine di percorrenza. Deve essere scelto l’unico percorso pi tale che non esista un altro pj con le stesse k tappe finali precedute da una tappa con distanza minore, cioè ∄j, k : ⟨pi,m−k+1, . . . pi,m⟩ = ⟨pj,m−k+1, . . . pj,m⟩ ∧ pj,m−k < pi,m−k.  

Altre informazioni di specifica possono essere trovate nel file PDF [specifications](specifications.pdf) linkato.

## Limiti e Progettazione

Per il progetto in questione i limiti di tempo e di memoria per avere un valutazione di 30 con lode erano i seguenti:

- limite di tempo di esecuzione: 1 secondo
- limite di spazio in memoria: 70 MiB  

per un file di input con approssimativamente 530 mila righe di comando.

Per rispettare tali limiti, è stata utilizzata la struttura dati *Albero Red Black*, mentre per la memorizzazione delle macchine all'interno nelle relative stazioni, un semplice array di 512 interi.

## Utilizzo

La soluzione del progetto si può trovare all'interno del file [main](main.c) linkato, eseguibile tramite il comando

``` bash
gcc -Wall -Werror -Wmaybe-uninitialized -Wuninitialized -pedantic -g -O0 -o main main.c
```

e, in seguito

```bash
./main.c < input.txt > output.txt
```

da terminale linux.

**NB**: è necessario installare il compilatore ``gcc`` sul calcolatore utilizzato per poter eseguire tale comando.

**NB**: il file ``input.txt`` deve essere formattato nel modo corretto, come da specifica. Si può scegliere come input uno dei file con estension ``.txt`` (e non ``output.txt``, che rappresentano l'output corretto al corrispondente input) presenti nella cartella [opens](opens/).

In alternativa è possibile utilizzare il file bash [run.sh](/scripts/run.sh), all'interno della cartella [scripts](/scripts/), tramiete il comando:

``` bash
bash run.sh main.c input.txt
```

avendo cura di trovarsi nella directory precedentemente specificata.

Ad esempio:

``` bash
gcc -Wall -Werror -Wmaybe-uninitialized -Wuninitialized -pedantic -g -O0 -o main main.c
```

``` bash
./main < opens/open_111.txt > outputs/main.output.txt
```

oppure

```bash
bash run.sh main.c open_111.txt
```

Per vedere la differenza fra l'output del programma e l'output atteso, relativo al file di input selezionato, è necessario eseguire il comando:

```bash
diff outputs/main.output.txt opens/open_111.output.txt
```

All'interno della cartella [scripts](/scripts/) sono presenti altri script di compilazione ed esecuzione, che involvono altri strumenti come *Callgrind*, *Valgrind* o *Massif*, di cui bisogna aver installato i relativi pacchetti linux. I passaggi per l'installazione di tali pacchetti e il loro utilizzo vengono spiegati in lezioni extra tenute dai professori del Politecnico di Milano.  
