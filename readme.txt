1. Folder - uri:
	
	Fiecare nivel are propriul sau folder in care sunt (trebuie) incluse materialele necesare nivelului dat.
	Exemplu: Pentru nivelul "starter" avem mapa dedicata "starter" in care avem incluse file-uri pentru grafica (starter.jpg, panou1.jpg, etc.) 
				si pentru logica (board.in, shapes.in, challenge.in).
	De asemenea avem mapa "aux_img" unde sunt incluse imagini auxiliare utilizate in cadrul jocului.

2. File-uri de input:
	
	Pentru fiecare nivel trebuie creat in mapa sa cate un fisier cu denumirea predefinita:
		"board.in" - cele 4 matrici ale tablei de joc, in care fiecare desen are atribuit un numar aparte (este indicat in documentul legenda.txt);

		"challenge.in" - trebuie scrise cifrele ce indica obiectele incercarii corespunzatoare nivelului (din documentul legenda.txt). Maxim se admit 16 obiecte, iar 
						 in cazul in care sunt mai putin de 16 obiecte, dupa ultimul obiect indicat, trebuie scrisa valoarea -1 (vezi "starter/challenge.in" pentru exemplu).
						 Atentie: nivelul "generated" nu are nevoie de acest fisier incat genereaza singur incercarea;

		"shapes.in" - cele 4 matrici ale pieselor de joc.