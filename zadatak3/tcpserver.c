/*
    TODO
    tcpserver [-p port]
    tcp server sluša dolazne konekcije na portu (default 1234)
   
    zahtjev oblika:
        prva 4 okteta su offset od početka tražene datoteke (network-byte-order)
        naziv datoteke koji završava znakom \0
    
    odgovor oblika:
        1 oktet status + sadržaj tražene datoteke
        * 0x00 - OK
        * 0x01 - tražena datoteka ne postoji
        * 0x02 - tcpserver nema prava čitati traženu datoteku
        * 0x03 - ostale greške, proizvoljna poruka
    
    nakon odgovora, zatvara se konekcija sa klijentom i čeka se sljedeća konekcija

    slijedno posluživanje, ne treba konkurentno (! možda bonus ako mi bude dosadno)

    tcpserver dohvaća datoteke samo iz trenutnog direktorija u kojem je pozvan
    ne u kojem se nalazi sam file tcpserver te ne prihvaća slanje datoteka iz 
    poddirektorija trenutnog direktorija
    ako u zahtjevu sadrži znak / ili traži nepostojeću datoteku -> 0x01
*/
