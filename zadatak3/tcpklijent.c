/*
    TODO
    tcpklijent [-s server] [-p port] [-c] filename

    dohvaćeni podaci spremaju se u datoteku filename (obavezan argument)
    opcija -c znači da se dohvaćeni podaci dodaju na kraj datoteke filename (append)

    ako se program pozove bez -c a u direktoriju već postoji datoteka filename
    ispisuje se poruka na stderr i tcpklijent završava s radom exit status != 0

    ako se pozove s opcijom -c ali korisnik nema dozvolu pisanja u filename
    opet stderr, exit status != 0

    ako se pozove s opcijom -c ali ne postoji datoteka filename u trenutnom direktoriju
    prema poslužitelju se šalje zahtjev s offsetom 0

    kad primi datoteku/server prekine konekciju, tcpklijent završava s radom s 
    statusom koji primi od servera (0x00, 0x01, 0x02, 0x03)
*/
