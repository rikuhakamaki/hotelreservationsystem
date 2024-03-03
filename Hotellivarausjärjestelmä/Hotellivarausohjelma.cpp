#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>

// Varauksiin k�ytett�v� tietorakenne ja sen muuttujat
struct Varaus {
	std::vector<int> huoneidennumerot;
	int yomaara = 0;
	std::string huonetyyppi;
};

// Aliohjelmien, eli funktioiden, prototyypit
int satunnaislukugeneraattori(int pienin, int suurin);
int virhetarkistus(const std::string& syote, int min, int max, const std::string& virheteksti);
void muutAsiakkaat(int max1, int min, int max2, std::vector<int>& varatuthuoneet);
void varaaSatunnainenHuone(int huoneidenmaara, int pienin, int suurin, std::vector<int>& varatuthuoneet, std::vector<Varaus>& varauksenhuoneet, Varaus& varaus);
void varaaOmatHuoneet(Varaus& varaus, int huoneidenmaara, int pienin, int suurin, std::vector<Varaus>& varauksenhuoneet, std::vector<int>& varatuthuoneet);
void huoneidentarkastelu(std::vector<Varaus> varauksenhuoneet);
void kassa(std::vector<Varaus> yvarauksenhuoneet, std::vector<Varaus> kvarauksenhuoneet, int kokonaishinta, int yhinta, int khinta, int alennusprosentti);
void poistaVaraus(std::vector<Varaus>& varaukset);

int main() {
	using std::cout, std::cin, std::vector;   // K�ytt�ilmaisut standardikirjaston elementeille

	// Huoneiden m��r�n ja niiden rajojen m��ritt�miseen liittyv�t muuttujat
	const int huoneetkokmaara = satunnaislukugeneraattori(20, 150) * 2;
	const int huoneidenraja = huoneetkokmaara / 2;

	// Muuttujat, jotka liittyv�t huoneiden ja asukkaiden m��r�n seurantaan eri huonetyypeille
	int yhhuoneidenmaara = 0;
	int khhuoneidenmaara = 0;
	int yhyomaara = 0;
	int khyomaara = 0;

	// Muuttujat, jotka liittyv�t huoneiden hintoihin ja kokonaishinnan laskemiseen
	const int yhinta = 100;
	const int khinta = 150;
	int kokonaishinta = 0;
	int alennusprosentti = satunnaislukugeneraattori(1, 3);

	int valikkovalinta = 0;   // K�ytt�j�n valinta valikossa

	// Vektorit, joihin tallennetaan kaikissa varauksissa varatut huoneiden
	// numerot huonetyypin mukaan. Tuo selkeytt� varattujen huoneiden laskemiseen
	// sek� seuraamiseen, ettei samaa huonetta voida varata useasti
	vector<int> varatutyhengenhuoneet;
	vector<int> varatutkhengenhuoneet;

	// Vektorit, joihin tallennetaan yhdess� varauksessa tehdyt huoneiden numerot.
	// K�ytet��n mm. varattujen huonem��rien kertomiseen varauksen j�lkeen sek�
	// jokaisen varauksen huonenumeroiden tarkasteluun
	vector<Varaus> varatutyhuoneet;
	vector<Varaus> varatutkhuoneet;
	
	// Muuttujat, joihin p�ivitet��n my�hemmin arvot vapaana oleville huoneiden
	// m��r�lle. Alustetaan ne aluksi nollaksi. Helpottaa k�ytt�j��, ettei t�m�n
	// tarvitse arvailla, kuinka monta huonetta h�n voi maksimissaan varata.
	int vapaatyhuoneet = 0;
	int vapaatkhuoneet = 0;

	// Generoidaan aliohjelman avulla satunnainen m��r� muiden "asiakkaiden" varaamia
	// yhden ja kahden hengen huoneita, joita k�ytt�j� ei pysty varaamaan itse, ja
	// tallennetaan ne kaikkien varattujen yhden ja kahden hengen huoneiden vektoreihin.
	// Jos sattuu k�ym��n sellainen tilanne, ett� satunnaislukugeneraattori on generoinut
	// sellaiset luvut, ett� muut "asiakkaat" ovat varanneet kaikki huoneet, kerrotaan
	// k�ytt�j�� palaamaan jonain muuna p�iv�n� (eli k�ynnist�m��n ohjelma uudestaan), ja
	// suljetaan ohjelma.
	cout << "Tervetuloa hotelli Paussiin!\n";
	muutAsiakkaat(huoneidenraja, 1, huoneidenraja, varatutyhengenhuoneet);
	muutAsiakkaat(huoneidenraja, huoneidenraja + 1, huoneetkokmaara, varatutkhengenhuoneet);
	if (varatutyhengenhuoneet.size() == huoneidenraja && varatutkhengenhuoneet.size() == huoneidenraja) {
		cout << "Pahoittelut. Hotellimme on varattu jo tayteen.\nPalatkaa uudestaan jonain muuna paivana.\n";
		return 0;
	}
	else {   // "Alkutekstit"
		cout << "Hotellissamme on " << huoneetkokmaara << " huonetta.\n";
		cout << "Huoneistamme 1-" << huoneidenraja << " ovat yhden hengen huoneita, ja " << huoneidenraja + 1 << "-" << huoneetkokmaara << " ovat kahden hengen huoneita.\n";
	}

	// Siirryt��n "ikuiseen" silmukkaan valikkoa varten, jotta ohjelma ei lopu, 
	// kun k�ytt�j� on k�ynyt valikon "l�pi" kerran, vaan vasta kassan j�lkeen
	// T�m�n j�lkeen esitet��n k�ytt�j�lle valikko valintamahdollisuuksineen.
	while (true) {
		cout << "Mita haluaisitte tehda?\n";
		valikkovalinta = virhetarkistus("1: Varata yhden hengen huoneita\n2: Varata kahden hengen huoneita\n3: Tarkastella varaamianne huoneita\n4: Poistaa varauksen\n5: Siirtya kassalle\n",
			1, 5, "Kyseista valintaa ei loydy valikosta.\nSyottakaa uudelleen, olkaa hyva:");

		// K�sitell��n k�ytt�j�n valinta valikkoa varten switch-lauseella. N�in 
		// saadaan luotua simppeli ja helposti toimiva valikkorakenne.
		switch (valikkovalinta) {

			// Yhden hengen huoneiden varaaminen
		case 1:

			// Jos kaikki yhden hengen huoneet on jo varattu, tulostetaan k�ytt�j�lle
			// virheilmoitus ja poistutaan yhden hengen huoneiden varauksesta. Kun verrataan 
			// vektorin, johon on tallennettu kaikien varattujen yhden hengen huoneidet numerot,
			// kokoa 'huoneidenraja' muuttujaan, joka jakaa huoneiden m��r�n puoliksi, on 
			// helppo pysy� per�ll� siit�, milloin kaikki yhden hengen huoneet on varattu. 
			// N�in poistetaan k�ytt�j�lt� mahdollisuus varata liikaa yhden hengen huoneita.
			if (varatutyhengenhuoneet.size() == huoneidenraja) {
				cout << "Kaikki yhden hengen huoneet ovat varattuja.\n";
			}
			else {   // Jos kaikki yhden hengen huoneet ei ole varattu, siirryt��n varaukseen

				// Kerrotaan ensiksi k�ytt�kokemuksen parantamiseksi vapaiden yhden hengen huoneiden m��r�.
				// Pyydet��n k�ytt�j�lt� h�nen haluamansa huoneiden m��r�, �iden m��r� ja valinta siihen,
				// haluaako k�ytt�j�, ett� huoneiden numerot arvotaan koneen toimesta, vai haluaako h�n
				// p��tt�� itse huoneiden numerot. Jokaisen valinnan j�lkeen kyseinen valinta tallennetaan
				// sille kuuluvaan muuttujaan, josta sit� voidaan hy�dynt�� my�hemmin. Jokaisella valinnalla 
				// on suurin ja pienin mahdollinen arvo. Yhden hengen huoneilla pienin arvo on 1, ja suurin 
				// arvo on 'huoneidenraja', josta v�hennet��n jo varattujen yhden hengen huoneiden m��r�, 
				// jotta k�ytt�j� ei voi varata enemm�n huoneita, kuin mit� on saatavilla. Valitsin suurimmaksi
				// mahdolliseksi y�m��r�ksi 31, koska hirve�n suurella todenn�k�isyydell� kukaan ei varaa
				// hotellihuoneita edes kuukaudeksi, joten sill� ei ole mit��n j�rke� olla yht��n sit� suurempi.
				vapaatyhuoneet = (huoneidenraja)-(varatutyhengenhuoneet.size());
				cout << "Yhden hengen huoneita on vapaana " << vapaatyhuoneet << " kappaletta.\n";

				yhhuoneidenmaara = virhetarkistus("Kuinka monta yhden hengen huonetta haluatte varata?\n", 1, huoneidenraja - varatutyhengenhuoneet.size(),
					"Valitsemanne huonemaara on liian suuri tai mahdoton.\nSyottakaa uudelleen, olkaa hyva:");

				yhyomaara = virhetarkistus("Kuinka moneksi yoksi haluatte varata huoneet?\n", 1, 31,
					"Virheellinen yomaara. Varattava yomaara tulee olla valilla 1-31.\nSyottakaa uudelleen, olkaa hyva:");

				valikkovalinta = virhetarkistus("Haluatko, etta 1: arvomme sinulle varattavat huoneet, vai 2: paattaa itse huoneiden numerot?\n",
					1, 2, "Kyseista valintaa ei loydy vaihtoehdoista.\nSyottakaa uudelleen, olkaa hyva:");

				if (valikkovalinta == 1) {   // K�ytt�j� on valinnut satunnaisten huonenumeroiden varaamisen

					// Luodaan varausobjekti, ja liitet��n k�ytt�j�n valitsema y�m��r� varausobjektin y�m��r��n.
					// T�m�n j�lkeen, koska kyseess� on yhden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
					// T�m�n j�lkeen suoritetaan satunnaisten huoneiden varaaminen funktiokutsun avulla.
					Varaus yhvaraus;
					yhvaraus.yomaara = yhyomaara;
					yhvaraus.huonetyyppi = "Yhden hengen huone";
					varaaSatunnainenHuone(yhhuoneidenmaara, 1, huoneidenraja, varatutyhengenhuoneet, varatutyhuoneet, yhvaraus);
				}
			}

			if (valikkovalinta == 2) {   // K�ytt�j� on valinnut p��tt�� itse varattavien huoneiden numerot

				// Luodaan varausobjekti, ja liitet��n k�ytt�j�n valitsema y�m��r� varausobjektin y�m��r��n.
				// T�m�n j�lkeen, koska kyseess� on yhden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
				// T�m�n j�lkeen suoritetaan k�ytt�j�n itse p��tt�mien huoneiden varaaminen funktiokutsun avulla.
				Varaus yhvaraus;
				yhvaraus.yomaara = yhyomaara;
				yhvaraus.huonetyyppi = "Yhden hengen huone";
				varaaOmatHuoneet(yhvaraus, yhhuoneidenmaara, 1, huoneidenraja, varatutyhuoneet, varatutyhengenhuoneet);
			}
			cout << "\n\n";
			break;   // Poistutaan yhden hengen huoneiden varauksen luomisesta

			// Kahden hengen huoneiden varaaminen
		case 2:

			// Jos kaikki kahden hengen huoneet on jo varattu, tulostetaan k�ytt�j�lle
			// virheilmoitus ja poistutaan kahden hengen huoneiden varauksesta. Kun verrataan 
			// vektorin, johon on tallennettu kaikien varattujen kahden hengen huoneidet numerot,
			// kokoa 'huoneidenraja' muuttujaan, joka jakaa huoneiden m��r�n puoliksi, on 
			// helppo pysy� per�ll� siit�, milloin kaikki kahden hengen huoneet on varattu. 
			// N�in poistetaan k�ytt�j�lt� mahdollisuus varata liikaa kahden hengen huoneita.
			if (varatutkhengenhuoneet.size() == huoneidenraja) {
				cout << "Kaikki kahden hengen huoneet ovat varattuja.\n";
			}
			else {   // Jos kaikkia kahden hengen huoneita ei ole varattu, siirryt��n varaukseen

				// Kerrotaan ensiksi k�ytt�kokemuksen parantamiseksi vapaiden kahden hengen huoneiden m��r�.
				// Pyydet��n k�ytt�j�lt� h�nen haluamansa huoneiden m��r�, �iden m��r� ja valinta siihen,
				// haluaako k�ytt�j�, ett� huoneiden numerot arvotaan koneen toimesta, vai haluaako h�n
				// p��tt�� itse huoneiden numerot. Jokaisen valinnan j�lkeen kyseinen valinta tallennetaan
				// sille kuuluvaan muuttujaan, josta sit� voidaan hy�dynt�� my�hemmin. Jokaisella valinnalla 
				// on suurin ja pienin mahdollinen arvo. Kahden hengen huoneilla pienin arvo on 'huoneidenraja' + 1,
				// koska 'huoneidenraja' muuttujaan on tallennettu viimeinen yhden hengen huoneen numero, joten 
				// + 1 t�m�n muuttujan arvoon on tietenkin ensimm�inen kahden hengen huone. Suurin 
				// arvo on 'huoneidenkokmaara' muuttuja, johon on tallennettu huoneiden kokonaism��r�. Valitsin 
				// suurimmaksi mahdolliseksi y�m��r�ksi 31, koska hirve�n suurella todenn�k�isyydell� kukaan ei varaa
				// hotellihuoneita edes kuukaudeksi, joten sill� ei ole mit��n j�rke� olla yht��n sit� suurempi.
				vapaatkhuoneet = (huoneidenraja)-(varatutkhengenhuoneet.size());
				cout << "Kahden hengen huoneita on vapaana " << vapaatkhuoneet << " kappaletta.\n";

				khhuoneidenmaara = virhetarkistus("Kuinka monta kahden hengen huonetta haluatte varata?\n", 1, huoneidenraja - varatutkhengenhuoneet.size(),
					"Valitsemanne huonemaara on liian suuri tai mahdoton.\nSyottakaa uudelleen, olkaa hyva:");

				khyomaara = virhetarkistus("Kuinka moneksi yoksi haluatte varata huoneet?\n", 1, 31,
					"Virheellinen yomaara. Varattava yomaara tulee olla valilla 1-31.\nSyottakaa uudelleen, olkaa hyva:");

				valikkovalinta = virhetarkistus("Haluatko, etta 1: arvomme sinulle varattavat huoneet, vai 2: paattaa itse huoneiden numerot?\n", 1, 2,
					"Kyseista valintaa ei loydy vaihtoehdoista.\nSyottakaa uudelleen, olkaa hyva:");

				if (valikkovalinta == 1) {   // K�ytt�j� on valinnut satunnaisten huonenumeroiden varaamisen

					// Luodaan varausobjekti, ja liitet��n k�ytt�j�n valitsema y�m��r� varausobjektin y�m��r��n.
					// T�m�n j�lkeen, koska kyseess� on kahden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
					// T�m�n j�lkeen suoritetaan k�ytt�j�n itse p��tt�mien huoneiden varaaminen funktiokutsun avulla.
					Varaus khvaraus;
					khvaraus.yomaara = khyomaara;
					khvaraus.huonetyyppi = "Kahden hengen huone";
					varaaSatunnainenHuone(khhuoneidenmaara, huoneidenraja + 1, huoneetkokmaara, 
						varatutkhengenhuoneet, varatutkhuoneet, khvaraus);
				}
			}

			if (valikkovalinta == 2) {   // K�ytt�j� on valinnut p��tt�� itse varattavien huoneiden numerot

				// Luodaan varausobjekti, ja liitet��n k�ytt�j�n valitsema y�m��r� varausobjektin y�m��r��n.
				// T�m�n j�lkeen, koska kyseess� on kahden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
				// T�m�n j�lkeen suoritetaan k�ytt�j�n itse p��tt�mien huoneiden varaaminen funktiokutsun avulla.
				Varaus khvaraus;
				khvaraus.yomaara = khyomaara;
				khvaraus.huonetyyppi = "Kahden hengen huone";
				varaaOmatHuoneet(khvaraus, khhuoneidenmaara, huoneidenraja + 1, huoneetkokmaara, varatutkhuoneet, varatutkhengenhuoneet);
			}
			cout << "\n\n";
			break;   // Poistutaan kahden hengen huoneiden varauksen luomisesta

			// Varattujen huoneiden tarkastelu
		case 3:
			// Jos k�ytt�j� ei ole varannut yht��n huonetta, kerrotaan se k�ytt�j�lle, ja poistutaan huoneiden
			// tarkastelusta. Jos k�ytt�j� on varannut huoneita, toteutetaan huoneiden tarkastelu funktiokutsulla.
			if (varatutyhuoneet.size() == 0 && varatutkhuoneet.size() == 0) {
				cout << "Ette ole varanneet viela yhtaan huonetta.\n";
			}
			else {
				cout << "Tarkastellaan varaamianne huoneita:\n\n";
				huoneidentarkastelu(varatutyhuoneet);
				huoneidentarkastelu(varatutkhuoneet);
			}
			cout << "\n";
			break;   // Poistutaan varattujen huoneiden tarkastelusta

			// Varauksien poistaminen
		case 4:
			// Jos yht��n varausta ei ole viel� tehty, kerrotaan se k�ytt�j�lle, ja poistutaan varauksien poistamisesta. 
			// T�ll� estet��n mahdollisuus tilanteelle, ett� k�ytt�j� j�� jumiin varauksen poistoon, koska saatavilla ei
			// ole varauksia, joita poistaa.
			if (varatutyhuoneet.size() == 0 && varatutkhuoneet.size() == 0) {
				cout << "Tee ensin varaus, jotta voit poistaa sen.\n";
			}
			// Jos varauksia on jo tehty, kysyt��n k�ytt�j�lt�, mink� huonetyypin varauksen t�m� tahtoo poistaa, ja tallennetaan
			// valinta muuttujaan 'valikkovalinta'. Asetetaan valinnan rajaksi 1-2, jotta k�ytt�j� saa virheilmoituksen, jos sy�tt��
			// valinnaksi jotain muuta, kuin mahdollisen valinnan.
			else {
				valikkovalinta = virhetarkistus("Haluatko poistaa 1: yhden hengen huoneen varauksen vai 2: kahden hengen huoneen varauksen?\n",
					1, 2, "Virheellinen syotto.\n");

				if (valikkovalinta == 1) {   // K�ytt�j� on valinnut poistaa yhden hengen huonevarauksen

					// Jos yhden hengen huonevarauksia ei ole, kerrotaan se k�ytt�j�lle, ja poistutaan 
					// varauksien poistamisesta. T�ll� estet��n yhden hengen huoneiden varauksen poistaminen
					// silloin, kun yhden hengen huoneiden varauksia ei ole.
					if (varatutyhuoneet.size() < 1) {
						cout << "Ette ole tehneet yhtaan yhden hengen huoneiden varausta.\n";
					}
					// Jos varauksia on, toteutaan varauksien poistaminen funktiokutsun avulla.
					else {
						poistaVaraus(varatutyhuoneet);
					}
				}
				if (valikkovalinta == 2) {   // K�ytt�j� on valinnut poistaa kahden hengen huonevarauksen

					// Jos kahden hengen huonevarauksia ei ole, kerrotaan se k�ytt�j�lle, ja poistutaan 
					// varauksien poistamisesta. T�ll� estet��n kahden hengen huoneiden varauksen poistaminen
					// silloin, kun yhden hengen huoneiden varauksia ei ole.
					if (varatutkhuoneet.size() < 1) {
						cout << "Ette ole tehneet yhtaan kahden hengen huoneiden varausta.\n";
					}
					// Jos varauksia on, toteutaan varauksien poistaminen funktiokutsun avulla.
					else {
						poistaVaraus(varatutkhuoneet);
					}
				}
			}
			cout << "\n";
			break;   // Poistutaan varausten poistamisesta
			
			//Kassa
		case 5:    // Siirryt��n k�ytt�j�n huoneiden hinnan osioon
			kassa(varatutyhuoneet, varatutkhuoneet, kokonaishinta, yhinta, khinta, alennusprosentti);
			return 0;   // Lopetetaan ohjelma onnistuneella paluukoodilla
		}
	}
}

// Funktio, joka generoi ja palauttaa satunnaisen kokonaisluvun annetulla v�lill�
int satunnaislukugeneraattori(int pienin, int suurin) {
	// Luodaan satunnaislukugeneraattorin "siemen", generaattorin moottori sek�
	// tasajakautunut jakauma kokonaisluvuille, ja asetetaan siihen muuttujat
	// generointijakaumaa varten
	static std::random_device rd;
	static std::default_random_engine kone{ rd() };
	static std::uniform_int_distribution<int> luku{ pienin, suurin };

	static int minimi{ pienin };   // Staattinen muuttuja minimiarvon seuraamiseksi
	static int maksimi{ suurin };   // Staattinen muuttuja maksimiarvon seuraamiseksi
	bool valivaihtuu = false;   // Lippu, jolla tarkisetetaan, onko generointiv�li muuttunut

	// Jos jakauman jompi kumpi (tai molemmat) arvoista on muuttunut, p�ivitet��n lippu
	// "h�lyytt�m��n" muutosta eli todeksi, jonka j�lkeen p�ivitet��n vaihtunut arvo.
	// T�m�n j�lkeen, jos v�li on muuttunt, p�ivitet��n viel� jakauman arvot. T�m�n mekanismin
	// avulla v�ltet��n uuden jakaumaobjektin luominen joka kerta, kun funktiota kutsutaan, ja 
	// sen sijaan p�ivitet��n olemassa oleva funktion jakaumaa, kun se muuttuu. T�m� on tehokkaampi
	// tapa kuin uuden jakaumaobjektin luominen joka kerta, kun v�li vaihtuu, ja samalla varmistutaan
	// siit�, ett� luodut satunnaisluvut noudattavat haluttua jakaumaa.
	if (pienin != minimi) {
		valivaihtuu = true;
		minimi = pienin;
	}
	if (suurin != maksimi) {
		valivaihtuu = true;
		maksimi = suurin;
	}
	if (valivaihtuu) luku.param(std::uniform_int_distribution<int>::param_type{ minimi, maksimi });

	return luku(kone);   // Generoi ja palauta satunnainen kokonaisluku annetulla v�lill�
}

// Funktio sy�tteen tarkistamiseksi virheiden varalta, palauttaa kokonaisluvun
int virhetarkistus(const std::string& syote, int min, int max, const std::string& virheteksti) {

	// Luodaan palautettavalle kokonaisluvulle muuttuja 'numero' ja alustetaan se nollaksi. 
	// Luodaan my�s virhelippu 'virhe', joka vaihtelee arvojen 'true' ja 'false' v�lill� 
	// riippuen siit�, onnistuuko sy�tteen muuttaminen kokonaisluvuksi vai ei. 
	int numero = 0;
	bool virhe;

	// Esitet��n k�ytt�j�lle haluttu teksti, jonka yhteydess� funktiota k�ytet��n. T�m�n j�lkeen,
	// otetaan k�ytt�j�n sy�te ensiksi paikallisena merkkijonona. Luetaan k�ytt�j�n sy�te, ja
	// tallennetaan se merkkijonomuuttujaan 'syote'. Alustetaan jokaisen toiston alussa virhelippu
	// 'false' muotoon.
	do {
		virhe = false;
		std::cout << syote;
		std::string syote;
		std::getline(std::cin, syote);

		// M��ritell��n tyypin 'size_t' muuttuja 'pos', joka kuvastaa ensimm�isen muuntamattoman
		// merkin paikkaa. Yritet��n muuntaa kyseinen merkki kokonaisluvuksi, jos muunnos onnistuu,
		// tallennetaan muunnettu sy�te muuttujaan 'numero'.
		try {
			size_t pos;
			numero = std::stoi(syote, &pos);

			// Tarkistetaan, onko koko sy�temerkkijono onnistuneesti muutettu kokonaisluvuksi. Jos
			// on, "heitet��n" poikkeus, joka osoittaa virheellisen sy�t�n, ja lopettaa muuntamisen.
			if (pos != syote.size()) {
				throw std::invalid_argument("Virheellinen sy�tt�.");
			}
			// Tarkistetaan, onko muunnettu kokonaisluku m��ritetyn alueen [min, max] ulkopuolella.
			// Jos on, "heitet��n" poikkeus, joka osoittaa, ett� sy�te on rajojen ulkopuolella.
			if (numero < min || numero > max) {
				throw std::out_of_range("Rajojen ulkopuolella.");
			}
		}

		// Jos havaitaan 'invalid_argument' - tai 'out_of_range' -poikkeus, asetetaan virhelippu 
		// muotoon 'true' virheen merkiksi, ja tulostetaan k�ytt�j�lle virheteksti.
		catch (const std::invalid_argument& e) {
			virhe = true;
			std::cout << virheteksti << "\n";
		}
		catch (const std::out_of_range& e) {
			virhe = true;
			std::cout << virheteksti << "\n";
		}
	} while (virhe);
	// Kun virheit� ei havaittu, palautetaan onnistuneesti muutettu kokonaisluku
	return numero;
}

// Funktio, jolla generoidaan satunnainen m��r� satunnaisia huoneita muille asiakkalle
void muutAsiakkaat(int max1, int min, int max2, std::vector<int>& varatuthuoneet) {
	// Generoidaan muuttujalle 'huoneidenmaara' arvo funktiokutsulla, ja tallennetaan se muuttujaan.
	// T�t� satunnaista arvoa k�ytet��n molempien huonetyyppien, muiden asiakkaiden varaamien, huoneiden
	// m��r�lle. Pienin arvo on aina 0, ja suurin arvo p��tet��n funktiokutsussa. T�m�n generoinnin j�lkeen
	// tehd��n muiden "asiakkaiden" varaamien huonenumeroiden generoinnille silmukka, joka toistetaan yht�
	// monta kertaa, kuin tuo �skett�in generoitu 'huoneidenmaara' muuttuja sai arvokseen.
	int huoneidenmaara = satunnaislukugeneraattori(0, max1);
	for (int i = 0; i < huoneidenmaara; i++) {
		int toisenHuone = 0;   // Alustetaan aina silmukan alussa muuttujan arvo nollaksi, jottei se pid� vanhaa arvoa mukanaan

		// Generoidaan satunnainen huonenumero funktiokutsussa p��tetylt� v�lilt�. Generoitu huonenumero on 
		// validi, jos sit� ei l�ydy viel� varattujen huoneiden vektorista (joka p��tet��n my�s funktiokutsussa).
		// N�in v�ltyt��n samojen huonenumeroiden generoinnilta. Jos huone on validi, se tallennetaan varattujen
		// huoneiden vektoriin (, joka on huonetyyppikohtainen).
		do {
			toisenHuone = satunnaislukugeneraattori(min, max2);
		} while (std::find(varatuthuoneet.begin(), varatuthuoneet.end(), toisenHuone) != varatuthuoneet.end());
		varatuthuoneet.push_back(toisenHuone);
	}
}


// Funkti, jolla voidaan toteuttaa satunnaisten huoneiden varaaminen
void varaaSatunnainenHuone(int huoneidenmaara, int pienin, int suurin, std::vector<int>& varatuthuoneet, 
	std::vector<Varaus>& varauksenhuoneet, Varaus& varaus) {
	// Luodaan huoneiden varaukselle ja satunnaisen huonenumeron generoinnille silmukka,
	// ja toistetaan silmukka yht� monta kertaa, kuin k�ytt�j� halusi varata huoneita.
	// Luodaan huoneen numerolle muuttuja 'huoneenNumero', ja alustetaan se jokaisen silmukan
	// toistokerran alussa nollaksi, jottei se pid� vanhaa huoneen numeroa mukanaan
	for (int i = 0; i < huoneidenmaara; i++) {
		int huoneenNumero = 0;
		// Generoidaan satunnainen huonenumero halutulla v�lill�. K�ytet��n generointiin seuraavaa
		// s��nt��: jos generoitua huonenumeroa ei viel� l�ydy varattujen huoneiden vektorista, se
		// t�ytt�� halutun s��nn�n, ja voidaan lis�t� kyseisess� varauksessa varattujen huoneiden 
		// vektoriin, sek� kaikkien varattujen huoneiden vektoriin.
		do {
			huoneenNumero = satunnaislukugeneraattori(pienin, suurin);
		} while (std::find(varatuthuoneet.begin(), varatuthuoneet.end(), huoneenNumero) != varatuthuoneet.end());
		varaus.huoneidennumerot.push_back(huoneenNumero);
		varatuthuoneet.push_back(huoneenNumero);
	}
	// Lopuksi, varauksen p��tytty�, tallennetaan varaus viel� kaikkien varausten vektoriin, ja kerrotaan
	// k�ytt�j�lle t�m�n juuri varaamien huoneiden m��r�.
	varauksenhuoneet.push_back(varaus);
	std::cout << "Varattujen huoneiden maara: " << varaus.huoneidennumerot.size();
}

// Funktio, jolla voidaan toteuttaa k�ytt�j�n itse p��tt�mien huoneiden varaaminen
void varaaOmatHuoneet(Varaus& varaus, int huoneidenmaara, int pienin, int suurin, std::vector<Varaus>& varauksenhuoneet, 
	std::vector<int>& varatuthuoneet) {

	// Kerrotaan k�ytt�j�lle varauksen aluksi, k�ytt�kokemuksen parantamiseksi, varattavien
	// huoneiden jakauma, jonka j�lkeen esitet��n t�lle jo varatut huoneet, jotka on j�rjestetty
	// suuruusj�rjestykseen pienimm�st� suurimpaan, jotta k�ytt�j� voi helposti katsoa sy�tetyst� 
	// listasta huoneet, joita h�nen ei tarvitse varata, eik� h�nen tarvitse t�ten huone kerrallaan 
	// kokeilla, mik� huone olisi mahdollisesti vapaa.
	std::cout << "Varattavat huoneet ovat numeroltaan " << pienin << "-" << suurin << "\n";
	if (!(varatuthuoneet.empty())) {
		std::cout << "Varatut huoneet: ";
		for (int i = 0; i < varatuthuoneet.size(); i++) {
			sort(varatuthuoneet.begin(), varatuthuoneet.end());
			std::cout << varatuthuoneet[i];
			if (i < (varatuthuoneet.size() - 1)) {
				std::cout << ", ";
			}
		}
	}
	std::cout << "\n";
	// Luodaan huoneiden varaukselle silmukka, ja toistetaan silmukka yht� monta kertaa, kuin
	// mit� k�ytt�j� halusi varata huoneita. Luodaan huoneen numerolle muuttuja 'huoneenNumero',
	// ja alustetaan se jokaisen silmukan toistokerran alussa nollaksi, jottei se pid� aikaisempaa 
	// huoneen numeroa mukanaan
	for (int i = 0; i < huoneidenmaara; i++) {
		int huoneenNumero = 0;

		// Niin kauan, kuin edellisen silmukan s��nt� (eli toistom��r�) toteutuu, kysyt��n k�ytt�j�lt�,
		// mink� huoneen h�n haluaa varata ja tallennetaan t�m�n p��tt�m� huoneen numero muuttujaan 
		// 'huoneenNumero', jos se t�ytt�� virhetarkistusfunktion s��nn�t. Seuraava vaihe on etsi� k�ytt�j�n
		// sy�tt�m�� huoneen numeroa sek� meneill� olevan varauksen jo varatuiden huoneiden joukosta, sek� 
		// edellisiss� varauksissa varatuiden huoneiden joukosta. Jos sy�tetty� huoneen numeroa ei l�ydy
		// kummastakaan, k�ytt�j�n sy�tt�m� huoneen numero on l�p�issyt kaikki sille esitetyt s��nn�t, ja
		// se voidaan lis�t� meneill� olevan varaukseen, sek� jo kaikkien varauksien varattuihin huoneisiin.
		while (true) {
			huoneenNumero = virhetarkistus("Syota huoneen numero, jonka haluat varata: ", pienin, suurin,
				"Kyseinen huone ei ole ole saatavilla.\nSyottakaa uudelleen, olkaa hyva:");
			if (std::find(varaus.huoneidennumerot.begin(), varaus.huoneidennumerot.end(), huoneenNumero) == varaus.huoneidennumerot.end() &&
				std::find(varatuthuoneet.begin(), varatuthuoneet.end(), huoneenNumero) == varatuthuoneet.end()) {
				varaus.huoneidennumerot.push_back(huoneenNumero);
				varatuthuoneet.push_back(huoneenNumero);
				break;
			}
			// Jos huone kuitenkin l�ytyy meneill� olevan varauksen jo varatuista huoneista tai edellisiss�
			// varauksissa varatuista huoneista, ilmoitetaan t�m� k�ytt�j�lle, ja pyydet��n k�ytt�j�lt�
			// huonetta uudestaan.
			else {
				std::cout << "Kyseinen huone on jo varattu, valitkaa toinen huone, olkaa hyva.\n";
			}
		}
	}
	// Lopuksi, varauksen p��tytty�, tallennetaan varaus viel� kaikkien varausten vektoriin, ja kerrotaan
	// k�ytt�j�lle t�m�n juuri varaamien huoneiden m��r�.
	varauksenhuoneet.push_back(varaus);
	std::cout << "Varattujen huoneiden maara: " << varaus.huoneidennumerot.size();
}

// Funktio, jota voidaan k�ytt�� varattujen huoneiden tarkasteluun
void huoneidentarkastelu(std::vector<Varaus> varauksenhuoneet) {
	// Luodaan huoneiden tarkastelulle silmukka, joka toistetaan yht� monta kertaa, kuin
	// mit� k�ytt�j� on varannut huoneita. Erotetaan varaukset toisistaan tulostamalla
	// teksti 'Varaus:' aina uuden varauksen tietojen tulostuksen alussa.
	for (int i = 0; i < varauksenhuoneet.size(); i++) {
		std::cout << "Varaus:\n";
		// Hienos��detty toiminto sanan 'Huone' tulostukselle, sek� 'tyyppi' sanan
		// yhteydess�, sek� 'numero' sanan yhteydess�. Jos k�ytt�j� on varannut
		// vain yhden huonetyypin huoneen, tulostetaan sana muodossa 'Huoneen'. Jos
		// huoneita on varattu useampi, tulostetaan sana muodossa 'Huoneiden'. T�m�n
		// j�lkeen tulostetaan k�ytt�j�lle kyseisen varauksen huoneiden y�m��r�. 
		if (varauksenhuoneet[i].huoneidennumerot.size() == 1) {
			std::cout << "Huoneen tyyppi: " << varauksenhuoneet[i].huonetyyppi << "\n";
		}
		else {
			std::cout << "Huoneiden tyyppi: " << varauksenhuoneet[i].huonetyyppi << "\n";
		}
		std::cout << "Oiden maara: " << varauksenhuoneet[i].yomaara << "\n";
		if (varauksenhuoneet[i].huoneidennumerot.size() == 1) {
			std::cout << "Huoneen numero: ";
		}
		else {
			std::cout << "Huoneiden numero: ";
		}
		// J�rjestet��n vektorista l�ytyv�t huoneiden numerot suuruusj�rjestykseen 
		// pienimm�st� suurimpaan. Sen j�lkeen esitet��n k�ytt�j�lle varaus kerrallaan,
		// mitk� huoneet on kussakin varauksessa varattu. Tulostetaan pilkku huoneiden
		// numeroiden v�lille, jos varauksessa on viel� v�hint��n yksi huone.
		for (int j = 0; j < varauksenhuoneet[i].huoneidennumerot.size(); j++) {
			sort(varauksenhuoneet[i].huoneidennumerot.begin(), varauksenhuoneet[i].huoneidennumerot.end());
			std::cout << varauksenhuoneet[i].huoneidennumerot[j];
			if (j < varauksenhuoneet[i].huoneidennumerot.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << "\n\n";
	}
}

// Funktio, jolla lasketaan varattujen huoneiden hinnat yhteen, ja satunnaisesti 
// arvotaan 0 %, 10 % tai 20 % alennus loppusummaan, joka v�hennet��n loppusummasta
void kassa(std::vector<Varaus> yvarauksenhuoneet, std::vector<Varaus> kvarauksenhuoneet,
	int kokonaishinta, int yhinta, int khinta, int alennusprosentti) {
	std::cout << "Kassa:\n";

	// Jos k�ytt�j� ei varannut yht��n huonetta, p��stet��n k�ytt�j� pois hotellista. 
	if (yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0) {
		std::cout << "Ette varanneet yhtaan huonetta.\n";
		std::cout << "Kiitos k�ynnist�, ja tervetuloa uudelleen!\n";
	}
	// Tehd��n silmukka, joka toistetaan yhden hengen huoneiden varauksien m��r�n verran.
	// Silmukan sis�ll� lis�t��n kokonaishinta muuttujaan aina k�sitelt�v�n varauksen huoneiden 
	// m��r� kerrottuna yhden hengen huoneen hinnalla, joka kerrotaan viel� kyseisen varauksen
	// huoneiden y�m��r�ll�.
	for (int i = 0; i < yvarauksenhuoneet.size(); i++) {
		kokonaishinta += yvarauksenhuoneet[i].huoneidennumerot.size() * yhinta * yvarauksenhuoneet[i].yomaara;
	}
	// Tehd��n silmukka, joka toistetaan kahden hengen huoneiden varauksien m��r�n verran.
	// Silmukan sis�ll� lis�t��n kokonaishinta muuttujaan aina k�sitelt�v�n varauksen huoneiden 
	// m��r� kerrottuna kahden hengen huoneen hinnalla, joka kerrotaan viel� kyseisen varauksen
	// huoneiden y�m��r�ll�.
	for (int i = 0; i < kvarauksenhuoneet.size(); i++) {
		kokonaishinta += kvarauksenhuoneet[i].huoneidennumerot.size() * khinta * kvarauksenhuoneet[i].yomaara;
	}
	// Jos satunnaislukugeneraattori on generoinut alennusprosenttimuuttujalle arvoksi 2, se vastaa
	// 10 % alennusta. Lasketaan alennuksen suuruus, v�hennet��n alennus kokonaishinnasta, jonka
	// j�lkeen kerrotaan k�ytt�j�lle alennuksen suuruus sek� kokonaishinta kyseisell� alennuksella.
	// Generoitua alennusta ei my�sk��n esitet�, jos k�ytt�j� ei ole varannut yht��n huoneita.
	if (alennusprosentti == 2 && !(yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0)) {
		int alennus = (kokonaishinta * 10) / 100;
		kokonaishinta -= alennus;
		std::cout << "Sinulla kavi tuuri, sait satunnaisesti arvotun 10 % alennuksen. Alennuksen arvo on: " << alennus << " euroa.\n";
		std::cout << "Loppusummaksenne jaa " << kokonaishinta << " euroa.\n";
	}
	// Sama kuin edell�, mutta jos satunnaislukugeneraattori on generoinut alennusprosenttimuuttujalle
	// arvoksi 3, se vastaa 20 % alennusta.
	else if (alennusprosentti == 3 && !(yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0)) {
		int alennus = (kokonaishinta * 20) / 100;
		kokonaishinta -= alennus;
		std::cout << "Sinulla kavi tuuri, sait satunnaisesti arvotun 20 % alennuksen. Alennuksen arvo on: " << alennus << " euroa.\n";
		std::cout << "Loppusummaksenne jaa " << kokonaishinta << " euroa.\n";
	}
	// Jos k�ytt�j� on varannut huoneita, mutta k�ytt�j� ei saanut satunnaisesti generoitua alennusta
	// (eli alennusprosenttimuuttujalle generoitiin arvoksi 1) kerrotaan k�ytt�j�lle vain t�m�n
	// varakokonaishinta.
	else if (!(yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0)) {
		std::cout << "Varauksienne hinta: " << kokonaishinta << " euroa.\n";
		std::cout << "Toivottavasti viihdytte hotellissamme.\n";
		std::cout << "Muistakaa, jos teill� tulee jotain kysytt�v��, yhteystietomme l�ytyv�t nettisivuiltamme, sek� jokaisesta huoneesta.\n";
	}
}

// Funktio, jolla mahdollistetaan halutun varauksen poistaminen.
void poistaVaraus(std::vector<Varaus>& varaukset) {
	// Luodaan poistettavan varauksen valinnalle muuttuja 'poista', ja alustetaan
	// se nollaksi. Esitet��n k�ytt�j�lle kaikki t�m�n tekem�t varaukset silmukan 
	// avulla, jota toistetaan yht� monta kertaa, kuin (huonetyypin) varauksia on 
	// yhteens�. Numeroidaan varaukset, ett� k�ytt�j�n on helpompi n�hd� ennen p��t�st�, 
	// mink� varauksen t�m� tahtoo poistaa.
	int poista = 0;
	std::cout << "Tassa on yhden hengen huoneiden varauksesi:\n\n";
	for (int i = 0; i < varaukset.size(); i++) {
		std::cout << "Varaus " << i + 1 << ":\n";
		// Hienos��t�� huone-sanan muotoilulle.
		if (varaukset[i].huoneidennumerot.size() == 1) {
			std::cout << "Huoneen tyyppi: " << varaukset[i].huonetyyppi << "\n";
		}
		else {
			std::cout << "Huoneiden tyyppi: " << varaukset[i].huonetyyppi << "\n";
		}
		std::cout << "Oiden maara: " << varaukset[i].yomaara << "\n";
		// Hienos��t�� huone-sanan muotoilulle.
		if (varaukset[i].huoneidennumerot.size() == 1) {
			std::cout << "Huoneen numero: ";
		}
		else {
			std::cout << "Huoneiden numero: ";
		}
		// Tehd��n varauskohtaisen huonenumeroiden tulostamiselle silmukka, joka
		// toisetaan yht� monta kertaa, kuin varauksessa on huoneita. Ennen
		// huonenumeroiden tulostamista j�rjestell��n varauskohtaiset huonenumerot
		// suuruusj�rjestykseen pienimm�st� suurimpaan k�ytt�kokemuksen parantamiseksi.
		// Lis�ksi, jos huonenumeroita on varauksessa viel� lis�� j�ljell�, tulostetaan
		// pilkku ennen seuraavaa huonenumeroa.
		for (int j = 0; j < varaukset[i].huoneidennumerot.size(); j++) {
			sort(varaukset[i].huoneidennumerot.begin(), varaukset[i].huoneidennumerot.end());
			std::cout << varaukset[i].huoneidennumerot[j];
			if (j < varaukset[i].huoneidennumerot.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << "\n\n";
	}
	// Kysyt��n k�ytt�j�lt�, mink� varauksen t�m� tahtoo poistaa, pienin on valinta voi olla 1, 
	// ja suurin valinta voi olla varauksien kokonaism��r�, eli viimeisen huonetyyppivarauksen
	// numero. Tallennetaan valinta muuttujaan 'poista'. T�m�n j�lkeen poistetaan varaus vektorista
	// erase-komennon avulla. Selataan vektorin tietoja alustap�in, ja valitaan poistettava varaus 
	// erottamalla k�ytt�j�n valinnasta 1, koska ensimm�isen varauksen numero on 1, mutta ensimm�inen
	// varaus on vektorissa paikkanumerolla 0.
	poista = virhetarkistus("Minka varauksen haluatte poistaa?\n", 1, varaukset.size(), "Kyseista varausta ei loydetty. Syota uudelleen.\n");
	varaukset.erase(varaukset.begin() + (poista - 1));
}