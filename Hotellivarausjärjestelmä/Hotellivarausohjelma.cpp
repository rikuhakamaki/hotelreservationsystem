#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>

// Varauksiin käytettävä tietorakenne ja sen muuttujat
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
	using std::cout, std::cin, std::vector;   // Käyttöilmaisut standardikirjaston elementeille

	// Huoneiden määrän ja niiden rajojen määrittämiseen liittyvät muuttujat
	const int huoneetkokmaara = satunnaislukugeneraattori(20, 150) * 2;
	const int huoneidenraja = huoneetkokmaara / 2;

	// Muuttujat, jotka liittyvät huoneiden ja asukkaiden määrän seurantaan eri huonetyypeille
	int yhhuoneidenmaara = 0;
	int khhuoneidenmaara = 0;
	int yhyomaara = 0;
	int khyomaara = 0;

	// Muuttujat, jotka liittyvät huoneiden hintoihin ja kokonaishinnan laskemiseen
	const int yhinta = 100;
	const int khinta = 150;
	int kokonaishinta = 0;
	int alennusprosentti = satunnaislukugeneraattori(1, 3);

	int valikkovalinta = 0;   // Käyttäjän valinta valikossa

	// Vektorit, joihin tallennetaan kaikissa varauksissa varatut huoneiden
	// numerot huonetyypin mukaan. Tuo selkeyttä varattujen huoneiden laskemiseen
	// sekä seuraamiseen, ettei samaa huonetta voida varata useasti
	vector<int> varatutyhengenhuoneet;
	vector<int> varatutkhengenhuoneet;

	// Vektorit, joihin tallennetaan yhdessä varauksessa tehdyt huoneiden numerot.
	// Käytetään mm. varattujen huonemäärien kertomiseen varauksen jälkeen sekä
	// jokaisen varauksen huonenumeroiden tarkasteluun
	vector<Varaus> varatutyhuoneet;
	vector<Varaus> varatutkhuoneet;
	
	// Muuttujat, joihin päivitetään myöhemmin arvot vapaana oleville huoneiden
	// määrälle. Alustetaan ne aluksi nollaksi. Helpottaa käyttäjää, ettei tämän
	// tarvitse arvailla, kuinka monta huonetta hän voi maksimissaan varata.
	int vapaatyhuoneet = 0;
	int vapaatkhuoneet = 0;

	// Generoidaan aliohjelman avulla satunnainen määrä muiden "asiakkaiden" varaamia
	// yhden ja kahden hengen huoneita, joita käyttäjä ei pysty varaamaan itse, ja
	// tallennetaan ne kaikkien varattujen yhden ja kahden hengen huoneiden vektoreihin.
	// Jos sattuu käymään sellainen tilanne, että satunnaislukugeneraattori on generoinut
	// sellaiset luvut, että muut "asiakkaat" ovat varanneet kaikki huoneet, kerrotaan
	// käyttäjää palaamaan jonain muuna päivänä (eli käynnistämään ohjelma uudestaan), ja
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

	// Siirrytään "ikuiseen" silmukkaan valikkoa varten, jotta ohjelma ei lopu, 
	// kun käyttäjä on käynyt valikon "läpi" kerran, vaan vasta kassan jälkeen
	// Tämän jälkeen esitetään käyttäjälle valikko valintamahdollisuuksineen.
	while (true) {
		cout << "Mita haluaisitte tehda?\n";
		valikkovalinta = virhetarkistus("1: Varata yhden hengen huoneita\n2: Varata kahden hengen huoneita\n3: Tarkastella varaamianne huoneita\n4: Poistaa varauksen\n5: Siirtya kassalle\n",
			1, 5, "Kyseista valintaa ei loydy valikosta.\nSyottakaa uudelleen, olkaa hyva:");

		// Käsitellään käyttäjän valinta valikkoa varten switch-lauseella. Näin 
		// saadaan luotua simppeli ja helposti toimiva valikkorakenne.
		switch (valikkovalinta) {

			// Yhden hengen huoneiden varaaminen
		case 1:

			// Jos kaikki yhden hengen huoneet on jo varattu, tulostetaan käyttäjälle
			// virheilmoitus ja poistutaan yhden hengen huoneiden varauksesta. Kun verrataan 
			// vektorin, johon on tallennettu kaikien varattujen yhden hengen huoneidet numerot,
			// kokoa 'huoneidenraja' muuttujaan, joka jakaa huoneiden määrän puoliksi, on 
			// helppo pysyä perällä siitä, milloin kaikki yhden hengen huoneet on varattu. 
			// Näin poistetaan käyttäjältä mahdollisuus varata liikaa yhden hengen huoneita.
			if (varatutyhengenhuoneet.size() == huoneidenraja) {
				cout << "Kaikki yhden hengen huoneet ovat varattuja.\n";
			}
			else {   // Jos kaikki yhden hengen huoneet ei ole varattu, siirrytään varaukseen

				// Kerrotaan ensiksi käyttökokemuksen parantamiseksi vapaiden yhden hengen huoneiden määrä.
				// Pyydetään käyttäjältä hänen haluamansa huoneiden määrä, öiden määrä ja valinta siihen,
				// haluaako käyttäjä, että huoneiden numerot arvotaan koneen toimesta, vai haluaako hän
				// päättää itse huoneiden numerot. Jokaisen valinnan jälkeen kyseinen valinta tallennetaan
				// sille kuuluvaan muuttujaan, josta sitä voidaan hyödyntää myöhemmin. Jokaisella valinnalla 
				// on suurin ja pienin mahdollinen arvo. Yhden hengen huoneilla pienin arvo on 1, ja suurin 
				// arvo on 'huoneidenraja', josta vähennetään jo varattujen yhden hengen huoneiden määrä, 
				// jotta käyttäjä ei voi varata enemmän huoneita, kuin mitä on saatavilla. Valitsin suurimmaksi
				// mahdolliseksi yömääräksi 31, koska hirveän suurella todennäköisyydellä kukaan ei varaa
				// hotellihuoneita edes kuukaudeksi, joten sillä ei ole mitään järkeä olla yhtään sitä suurempi.
				vapaatyhuoneet = (huoneidenraja)-(varatutyhengenhuoneet.size());
				cout << "Yhden hengen huoneita on vapaana " << vapaatyhuoneet << " kappaletta.\n";

				yhhuoneidenmaara = virhetarkistus("Kuinka monta yhden hengen huonetta haluatte varata?\n", 1, huoneidenraja - varatutyhengenhuoneet.size(),
					"Valitsemanne huonemaara on liian suuri tai mahdoton.\nSyottakaa uudelleen, olkaa hyva:");

				yhyomaara = virhetarkistus("Kuinka moneksi yoksi haluatte varata huoneet?\n", 1, 31,
					"Virheellinen yomaara. Varattava yomaara tulee olla valilla 1-31.\nSyottakaa uudelleen, olkaa hyva:");

				valikkovalinta = virhetarkistus("Haluatko, etta 1: arvomme sinulle varattavat huoneet, vai 2: paattaa itse huoneiden numerot?\n",
					1, 2, "Kyseista valintaa ei loydy vaihtoehdoista.\nSyottakaa uudelleen, olkaa hyva:");

				if (valikkovalinta == 1) {   // Käyttäjä on valinnut satunnaisten huonenumeroiden varaamisen

					// Luodaan varausobjekti, ja liitetään käyttäjän valitsema yömäärä varausobjektin yömäärään.
					// Tämän jälkeen, koska kyseessä on yhden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
					// Tämän jälkeen suoritetaan satunnaisten huoneiden varaaminen funktiokutsun avulla.
					Varaus yhvaraus;
					yhvaraus.yomaara = yhyomaara;
					yhvaraus.huonetyyppi = "Yhden hengen huone";
					varaaSatunnainenHuone(yhhuoneidenmaara, 1, huoneidenraja, varatutyhengenhuoneet, varatutyhuoneet, yhvaraus);
				}
			}

			if (valikkovalinta == 2) {   // Käyttäjä on valinnut päättää itse varattavien huoneiden numerot

				// Luodaan varausobjekti, ja liitetään käyttäjän valitsema yömäärä varausobjektin yömäärään.
				// Tämän jälkeen, koska kyseessä on yhden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
				// Tämän jälkeen suoritetaan käyttäjän itse päättämien huoneiden varaaminen funktiokutsun avulla.
				Varaus yhvaraus;
				yhvaraus.yomaara = yhyomaara;
				yhvaraus.huonetyyppi = "Yhden hengen huone";
				varaaOmatHuoneet(yhvaraus, yhhuoneidenmaara, 1, huoneidenraja, varatutyhuoneet, varatutyhengenhuoneet);
			}
			cout << "\n\n";
			break;   // Poistutaan yhden hengen huoneiden varauksen luomisesta

			// Kahden hengen huoneiden varaaminen
		case 2:

			// Jos kaikki kahden hengen huoneet on jo varattu, tulostetaan käyttäjälle
			// virheilmoitus ja poistutaan kahden hengen huoneiden varauksesta. Kun verrataan 
			// vektorin, johon on tallennettu kaikien varattujen kahden hengen huoneidet numerot,
			// kokoa 'huoneidenraja' muuttujaan, joka jakaa huoneiden määrän puoliksi, on 
			// helppo pysyä perällä siitä, milloin kaikki kahden hengen huoneet on varattu. 
			// Näin poistetaan käyttäjältä mahdollisuus varata liikaa kahden hengen huoneita.
			if (varatutkhengenhuoneet.size() == huoneidenraja) {
				cout << "Kaikki kahden hengen huoneet ovat varattuja.\n";
			}
			else {   // Jos kaikkia kahden hengen huoneita ei ole varattu, siirrytään varaukseen

				// Kerrotaan ensiksi käyttökokemuksen parantamiseksi vapaiden kahden hengen huoneiden määrä.
				// Pyydetään käyttäjältä hänen haluamansa huoneiden määrä, öiden määrä ja valinta siihen,
				// haluaako käyttäjä, että huoneiden numerot arvotaan koneen toimesta, vai haluaako hän
				// päättää itse huoneiden numerot. Jokaisen valinnan jälkeen kyseinen valinta tallennetaan
				// sille kuuluvaan muuttujaan, josta sitä voidaan hyödyntää myöhemmin. Jokaisella valinnalla 
				// on suurin ja pienin mahdollinen arvo. Kahden hengen huoneilla pienin arvo on 'huoneidenraja' + 1,
				// koska 'huoneidenraja' muuttujaan on tallennettu viimeinen yhden hengen huoneen numero, joten 
				// + 1 tämän muuttujan arvoon on tietenkin ensimmäinen kahden hengen huone. Suurin 
				// arvo on 'huoneidenkokmaara' muuttuja, johon on tallennettu huoneiden kokonaismäärä. Valitsin 
				// suurimmaksi mahdolliseksi yömääräksi 31, koska hirveän suurella todennäköisyydellä kukaan ei varaa
				// hotellihuoneita edes kuukaudeksi, joten sillä ei ole mitään järkeä olla yhtään sitä suurempi.
				vapaatkhuoneet = (huoneidenraja)-(varatutkhengenhuoneet.size());
				cout << "Kahden hengen huoneita on vapaana " << vapaatkhuoneet << " kappaletta.\n";

				khhuoneidenmaara = virhetarkistus("Kuinka monta kahden hengen huonetta haluatte varata?\n", 1, huoneidenraja - varatutkhengenhuoneet.size(),
					"Valitsemanne huonemaara on liian suuri tai mahdoton.\nSyottakaa uudelleen, olkaa hyva:");

				khyomaara = virhetarkistus("Kuinka moneksi yoksi haluatte varata huoneet?\n", 1, 31,
					"Virheellinen yomaara. Varattava yomaara tulee olla valilla 1-31.\nSyottakaa uudelleen, olkaa hyva:");

				valikkovalinta = virhetarkistus("Haluatko, etta 1: arvomme sinulle varattavat huoneet, vai 2: paattaa itse huoneiden numerot?\n", 1, 2,
					"Kyseista valintaa ei loydy vaihtoehdoista.\nSyottakaa uudelleen, olkaa hyva:");

				if (valikkovalinta == 1) {   // Käyttäjä on valinnut satunnaisten huonenumeroiden varaamisen

					// Luodaan varausobjekti, ja liitetään käyttäjän valitsema yömäärä varausobjektin yömäärään.
					// Tämän jälkeen, koska kyseessä on kahden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
					// Tämän jälkeen suoritetaan käyttäjän itse päättämien huoneiden varaaminen funktiokutsun avulla.
					Varaus khvaraus;
					khvaraus.yomaara = khyomaara;
					khvaraus.huonetyyppi = "Kahden hengen huone";
					varaaSatunnainenHuone(khhuoneidenmaara, huoneidenraja + 1, huoneetkokmaara, 
						varatutkhengenhuoneet, varatutkhuoneet, khvaraus);
				}
			}

			if (valikkovalinta == 2) {   // Käyttäjä on valinnut päättää itse varattavien huoneiden numerot

				// Luodaan varausobjekti, ja liitetään käyttäjän valitsema yömäärä varausobjektin yömäärään.
				// Tämän jälkeen, koska kyseessä on kahden hengen huoneen varaus, asetaan se varauksen huonetyypiksi.
				// Tämän jälkeen suoritetaan käyttäjän itse päättämien huoneiden varaaminen funktiokutsun avulla.
				Varaus khvaraus;
				khvaraus.yomaara = khyomaara;
				khvaraus.huonetyyppi = "Kahden hengen huone";
				varaaOmatHuoneet(khvaraus, khhuoneidenmaara, huoneidenraja + 1, huoneetkokmaara, varatutkhuoneet, varatutkhengenhuoneet);
			}
			cout << "\n\n";
			break;   // Poistutaan kahden hengen huoneiden varauksen luomisesta

			// Varattujen huoneiden tarkastelu
		case 3:
			// Jos käyttäjä ei ole varannut yhtään huonetta, kerrotaan se käyttäjälle, ja poistutaan huoneiden
			// tarkastelusta. Jos käyttäjä on varannut huoneita, toteutetaan huoneiden tarkastelu funktiokutsulla.
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
			// Jos yhtään varausta ei ole vielä tehty, kerrotaan se käyttäjälle, ja poistutaan varauksien poistamisesta. 
			// Tällä estetään mahdollisuus tilanteelle, että käyttäjä jää jumiin varauksen poistoon, koska saatavilla ei
			// ole varauksia, joita poistaa.
			if (varatutyhuoneet.size() == 0 && varatutkhuoneet.size() == 0) {
				cout << "Tee ensin varaus, jotta voit poistaa sen.\n";
			}
			// Jos varauksia on jo tehty, kysytään käyttäjältä, minkä huonetyypin varauksen tämä tahtoo poistaa, ja tallennetaan
			// valinta muuttujaan 'valikkovalinta'. Asetetaan valinnan rajaksi 1-2, jotta käyttäjä saa virheilmoituksen, jos syöttää
			// valinnaksi jotain muuta, kuin mahdollisen valinnan.
			else {
				valikkovalinta = virhetarkistus("Haluatko poistaa 1: yhden hengen huoneen varauksen vai 2: kahden hengen huoneen varauksen?\n",
					1, 2, "Virheellinen syotto.\n");

				if (valikkovalinta == 1) {   // Käyttäjä on valinnut poistaa yhden hengen huonevarauksen

					// Jos yhden hengen huonevarauksia ei ole, kerrotaan se käyttäjälle, ja poistutaan 
					// varauksien poistamisesta. Tällä estetään yhden hengen huoneiden varauksen poistaminen
					// silloin, kun yhden hengen huoneiden varauksia ei ole.
					if (varatutyhuoneet.size() < 1) {
						cout << "Ette ole tehneet yhtaan yhden hengen huoneiden varausta.\n";
					}
					// Jos varauksia on, toteutaan varauksien poistaminen funktiokutsun avulla.
					else {
						poistaVaraus(varatutyhuoneet);
					}
				}
				if (valikkovalinta == 2) {   // Käyttäjä on valinnut poistaa kahden hengen huonevarauksen

					// Jos kahden hengen huonevarauksia ei ole, kerrotaan se käyttäjälle, ja poistutaan 
					// varauksien poistamisesta. Tällä estetään kahden hengen huoneiden varauksen poistaminen
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
		case 5:    // Siirrytään käyttäjän huoneiden hinnan osioon
			kassa(varatutyhuoneet, varatutkhuoneet, kokonaishinta, yhinta, khinta, alennusprosentti);
			return 0;   // Lopetetaan ohjelma onnistuneella paluukoodilla
		}
	}
}

// Funktio, joka generoi ja palauttaa satunnaisen kokonaisluvun annetulla välillä
int satunnaislukugeneraattori(int pienin, int suurin) {
	// Luodaan satunnaislukugeneraattorin "siemen", generaattorin moottori sekä
	// tasajakautunut jakauma kokonaisluvuille, ja asetetaan siihen muuttujat
	// generointijakaumaa varten
	static std::random_device rd;
	static std::default_random_engine kone{ rd() };
	static std::uniform_int_distribution<int> luku{ pienin, suurin };

	static int minimi{ pienin };   // Staattinen muuttuja minimiarvon seuraamiseksi
	static int maksimi{ suurin };   // Staattinen muuttuja maksimiarvon seuraamiseksi
	bool valivaihtuu = false;   // Lippu, jolla tarkisetetaan, onko generointiväli muuttunut

	// Jos jakauman jompi kumpi (tai molemmat) arvoista on muuttunut, päivitetään lippu
	// "hälyyttämään" muutosta eli todeksi, jonka jälkeen päivitetään vaihtunut arvo.
	// Tämän jälkeen, jos väli on muuttunt, päivitetään vielä jakauman arvot. Tämän mekanismin
	// avulla vältetään uuden jakaumaobjektin luominen joka kerta, kun funktiota kutsutaan, ja 
	// sen sijaan päivitetään olemassa oleva funktion jakaumaa, kun se muuttuu. Tämä on tehokkaampi
	// tapa kuin uuden jakaumaobjektin luominen joka kerta, kun väli vaihtuu, ja samalla varmistutaan
	// siitä, että luodut satunnaisluvut noudattavat haluttua jakaumaa.
	if (pienin != minimi) {
		valivaihtuu = true;
		minimi = pienin;
	}
	if (suurin != maksimi) {
		valivaihtuu = true;
		maksimi = suurin;
	}
	if (valivaihtuu) luku.param(std::uniform_int_distribution<int>::param_type{ minimi, maksimi });

	return luku(kone);   // Generoi ja palauta satunnainen kokonaisluku annetulla välillä
}

// Funktio syötteen tarkistamiseksi virheiden varalta, palauttaa kokonaisluvun
int virhetarkistus(const std::string& syote, int min, int max, const std::string& virheteksti) {

	// Luodaan palautettavalle kokonaisluvulle muuttuja 'numero' ja alustetaan se nollaksi. 
	// Luodaan myös virhelippu 'virhe', joka vaihtelee arvojen 'true' ja 'false' välillä 
	// riippuen siitä, onnistuuko syötteen muuttaminen kokonaisluvuksi vai ei. 
	int numero = 0;
	bool virhe;

	// Esitetään käyttäjälle haluttu teksti, jonka yhteydessä funktiota käytetään. Tämän jälkeen,
	// otetaan käyttäjän syöte ensiksi paikallisena merkkijonona. Luetaan käyttäjän syöte, ja
	// tallennetaan se merkkijonomuuttujaan 'syote'. Alustetaan jokaisen toiston alussa virhelippu
	// 'false' muotoon.
	do {
		virhe = false;
		std::cout << syote;
		std::string syote;
		std::getline(std::cin, syote);

		// Määritellään tyypin 'size_t' muuttuja 'pos', joka kuvastaa ensimmäisen muuntamattoman
		// merkin paikkaa. Yritetään muuntaa kyseinen merkki kokonaisluvuksi, jos muunnos onnistuu,
		// tallennetaan muunnettu syöte muuttujaan 'numero'.
		try {
			size_t pos;
			numero = std::stoi(syote, &pos);

			// Tarkistetaan, onko koko syötemerkkijono onnistuneesti muutettu kokonaisluvuksi. Jos
			// on, "heitetään" poikkeus, joka osoittaa virheellisen syötön, ja lopettaa muuntamisen.
			if (pos != syote.size()) {
				throw std::invalid_argument("Virheellinen syöttö.");
			}
			// Tarkistetaan, onko muunnettu kokonaisluku määritetyn alueen [min, max] ulkopuolella.
			// Jos on, "heitetään" poikkeus, joka osoittaa, että syöte on rajojen ulkopuolella.
			if (numero < min || numero > max) {
				throw std::out_of_range("Rajojen ulkopuolella.");
			}
		}

		// Jos havaitaan 'invalid_argument' - tai 'out_of_range' -poikkeus, asetetaan virhelippu 
		// muotoon 'true' virheen merkiksi, ja tulostetaan käyttäjälle virheteksti.
		catch (const std::invalid_argument& e) {
			virhe = true;
			std::cout << virheteksti << "\n";
		}
		catch (const std::out_of_range& e) {
			virhe = true;
			std::cout << virheteksti << "\n";
		}
	} while (virhe);
	// Kun virheitä ei havaittu, palautetaan onnistuneesti muutettu kokonaisluku
	return numero;
}

// Funktio, jolla generoidaan satunnainen määrä satunnaisia huoneita muille asiakkalle
void muutAsiakkaat(int max1, int min, int max2, std::vector<int>& varatuthuoneet) {
	// Generoidaan muuttujalle 'huoneidenmaara' arvo funktiokutsulla, ja tallennetaan se muuttujaan.
	// Tätä satunnaista arvoa käytetään molempien huonetyyppien, muiden asiakkaiden varaamien, huoneiden
	// määrälle. Pienin arvo on aina 0, ja suurin arvo päätetään funktiokutsussa. Tämän generoinnin jälkeen
	// tehdään muiden "asiakkaiden" varaamien huonenumeroiden generoinnille silmukka, joka toistetaan yhtä
	// monta kertaa, kuin tuo äskettäin generoitu 'huoneidenmaara' muuttuja sai arvokseen.
	int huoneidenmaara = satunnaislukugeneraattori(0, max1);
	for (int i = 0; i < huoneidenmaara; i++) {
		int toisenHuone = 0;   // Alustetaan aina silmukan alussa muuttujan arvo nollaksi, jottei se pidä vanhaa arvoa mukanaan

		// Generoidaan satunnainen huonenumero funktiokutsussa päätetyltä väliltä. Generoitu huonenumero on 
		// validi, jos sitä ei löydy vielä varattujen huoneiden vektorista (joka päätetään myös funktiokutsussa).
		// Näin vältytään samojen huonenumeroiden generoinnilta. Jos huone on validi, se tallennetaan varattujen
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
	// ja toistetaan silmukka yhtä monta kertaa, kuin käyttäjä halusi varata huoneita.
	// Luodaan huoneen numerolle muuttuja 'huoneenNumero', ja alustetaan se jokaisen silmukan
	// toistokerran alussa nollaksi, jottei se pidä vanhaa huoneen numeroa mukanaan
	for (int i = 0; i < huoneidenmaara; i++) {
		int huoneenNumero = 0;
		// Generoidaan satunnainen huonenumero halutulla välillä. Käytetään generointiin seuraavaa
		// sääntöä: jos generoitua huonenumeroa ei vielä löydy varattujen huoneiden vektorista, se
		// täyttää halutun säännön, ja voidaan lisätä kyseisessä varauksessa varattujen huoneiden 
		// vektoriin, sekä kaikkien varattujen huoneiden vektoriin.
		do {
			huoneenNumero = satunnaislukugeneraattori(pienin, suurin);
		} while (std::find(varatuthuoneet.begin(), varatuthuoneet.end(), huoneenNumero) != varatuthuoneet.end());
		varaus.huoneidennumerot.push_back(huoneenNumero);
		varatuthuoneet.push_back(huoneenNumero);
	}
	// Lopuksi, varauksen päätyttyä, tallennetaan varaus vielä kaikkien varausten vektoriin, ja kerrotaan
	// käyttäjälle tämän juuri varaamien huoneiden määrä.
	varauksenhuoneet.push_back(varaus);
	std::cout << "Varattujen huoneiden maara: " << varaus.huoneidennumerot.size();
}

// Funktio, jolla voidaan toteuttaa käyttäjän itse päättämien huoneiden varaaminen
void varaaOmatHuoneet(Varaus& varaus, int huoneidenmaara, int pienin, int suurin, std::vector<Varaus>& varauksenhuoneet, 
	std::vector<int>& varatuthuoneet) {

	// Kerrotaan käyttäjälle varauksen aluksi, käyttökokemuksen parantamiseksi, varattavien
	// huoneiden jakauma, jonka jälkeen esitetään tälle jo varatut huoneet, jotka on järjestetty
	// suuruusjärjestykseen pienimmästä suurimpaan, jotta käyttäjä voi helposti katsoa syötetystä 
	// listasta huoneet, joita hänen ei tarvitse varata, eikä hänen tarvitse täten huone kerrallaan 
	// kokeilla, mikä huone olisi mahdollisesti vapaa.
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
	// Luodaan huoneiden varaukselle silmukka, ja toistetaan silmukka yhtä monta kertaa, kuin
	// mitä käyttäjä halusi varata huoneita. Luodaan huoneen numerolle muuttuja 'huoneenNumero',
	// ja alustetaan se jokaisen silmukan toistokerran alussa nollaksi, jottei se pidä aikaisempaa 
	// huoneen numeroa mukanaan
	for (int i = 0; i < huoneidenmaara; i++) {
		int huoneenNumero = 0;

		// Niin kauan, kuin edellisen silmukan sääntö (eli toistomäärä) toteutuu, kysytään käyttäjältä,
		// minkä huoneen hän haluaa varata ja tallennetaan tämän päättämä huoneen numero muuttujaan 
		// 'huoneenNumero', jos se täyttää virhetarkistusfunktion säännöt. Seuraava vaihe on etsiä käyttäjän
		// syöttämää huoneen numeroa sekä meneillä olevan varauksen jo varatuiden huoneiden joukosta, sekä 
		// edellisissä varauksissa varatuiden huoneiden joukosta. Jos syötettyä huoneen numeroa ei löydy
		// kummastakaan, käyttäjän syöttämä huoneen numero on läpäissyt kaikki sille esitetyt säännöt, ja
		// se voidaan lisätä meneillä olevan varaukseen, sekä jo kaikkien varauksien varattuihin huoneisiin.
		while (true) {
			huoneenNumero = virhetarkistus("Syota huoneen numero, jonka haluat varata: ", pienin, suurin,
				"Kyseinen huone ei ole ole saatavilla.\nSyottakaa uudelleen, olkaa hyva:");
			if (std::find(varaus.huoneidennumerot.begin(), varaus.huoneidennumerot.end(), huoneenNumero) == varaus.huoneidennumerot.end() &&
				std::find(varatuthuoneet.begin(), varatuthuoneet.end(), huoneenNumero) == varatuthuoneet.end()) {
				varaus.huoneidennumerot.push_back(huoneenNumero);
				varatuthuoneet.push_back(huoneenNumero);
				break;
			}
			// Jos huone kuitenkin löytyy meneillä olevan varauksen jo varatuista huoneista tai edellisissä
			// varauksissa varatuista huoneista, ilmoitetaan tämä käyttäjälle, ja pyydetään käyttäjältä
			// huonetta uudestaan.
			else {
				std::cout << "Kyseinen huone on jo varattu, valitkaa toinen huone, olkaa hyva.\n";
			}
		}
	}
	// Lopuksi, varauksen päätyttyä, tallennetaan varaus vielä kaikkien varausten vektoriin, ja kerrotaan
	// käyttäjälle tämän juuri varaamien huoneiden määrä.
	varauksenhuoneet.push_back(varaus);
	std::cout << "Varattujen huoneiden maara: " << varaus.huoneidennumerot.size();
}

// Funktio, jota voidaan käyttää varattujen huoneiden tarkasteluun
void huoneidentarkastelu(std::vector<Varaus> varauksenhuoneet) {
	// Luodaan huoneiden tarkastelulle silmukka, joka toistetaan yhtä monta kertaa, kuin
	// mitä käyttäjä on varannut huoneita. Erotetaan varaukset toisistaan tulostamalla
	// teksti 'Varaus:' aina uuden varauksen tietojen tulostuksen alussa.
	for (int i = 0; i < varauksenhuoneet.size(); i++) {
		std::cout << "Varaus:\n";
		// Hienosäädetty toiminto sanan 'Huone' tulostukselle, sekä 'tyyppi' sanan
		// yhteydessä, sekä 'numero' sanan yhteydessä. Jos käyttäjä on varannut
		// vain yhden huonetyypin huoneen, tulostetaan sana muodossa 'Huoneen'. Jos
		// huoneita on varattu useampi, tulostetaan sana muodossa 'Huoneiden'. Tämän
		// jälkeen tulostetaan käyttäjälle kyseisen varauksen huoneiden yömäärä. 
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
		// Järjestetään vektorista löytyvät huoneiden numerot suuruusjärjestykseen 
		// pienimmästä suurimpaan. Sen jälkeen esitetään käyttäjälle varaus kerrallaan,
		// mitkä huoneet on kussakin varauksessa varattu. Tulostetaan pilkku huoneiden
		// numeroiden välille, jos varauksessa on vielä vähintään yksi huone.
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
// arvotaan 0 %, 10 % tai 20 % alennus loppusummaan, joka vähennetään loppusummasta
void kassa(std::vector<Varaus> yvarauksenhuoneet, std::vector<Varaus> kvarauksenhuoneet,
	int kokonaishinta, int yhinta, int khinta, int alennusprosentti) {
	std::cout << "Kassa:\n";

	// Jos käyttäjä ei varannut yhtään huonetta, päästetään käyttäjä pois hotellista. 
	if (yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0) {
		std::cout << "Ette varanneet yhtaan huonetta.\n";
		std::cout << "Kiitos käynnistä, ja tervetuloa uudelleen!\n";
	}
	// Tehdään silmukka, joka toistetaan yhden hengen huoneiden varauksien määrän verran.
	// Silmukan sisällä lisätään kokonaishinta muuttujaan aina käsiteltävän varauksen huoneiden 
	// määrä kerrottuna yhden hengen huoneen hinnalla, joka kerrotaan vielä kyseisen varauksen
	// huoneiden yömäärällä.
	for (int i = 0; i < yvarauksenhuoneet.size(); i++) {
		kokonaishinta += yvarauksenhuoneet[i].huoneidennumerot.size() * yhinta * yvarauksenhuoneet[i].yomaara;
	}
	// Tehdään silmukka, joka toistetaan kahden hengen huoneiden varauksien määrän verran.
	// Silmukan sisällä lisätään kokonaishinta muuttujaan aina käsiteltävän varauksen huoneiden 
	// määrä kerrottuna kahden hengen huoneen hinnalla, joka kerrotaan vielä kyseisen varauksen
	// huoneiden yömäärällä.
	for (int i = 0; i < kvarauksenhuoneet.size(); i++) {
		kokonaishinta += kvarauksenhuoneet[i].huoneidennumerot.size() * khinta * kvarauksenhuoneet[i].yomaara;
	}
	// Jos satunnaislukugeneraattori on generoinut alennusprosenttimuuttujalle arvoksi 2, se vastaa
	// 10 % alennusta. Lasketaan alennuksen suuruus, vähennetään alennus kokonaishinnasta, jonka
	// jälkeen kerrotaan käyttäjälle alennuksen suuruus sekä kokonaishinta kyseisellä alennuksella.
	// Generoitua alennusta ei myöskään esitetä, jos käyttäjä ei ole varannut yhtään huoneita.
	if (alennusprosentti == 2 && !(yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0)) {
		int alennus = (kokonaishinta * 10) / 100;
		kokonaishinta -= alennus;
		std::cout << "Sinulla kavi tuuri, sait satunnaisesti arvotun 10 % alennuksen. Alennuksen arvo on: " << alennus << " euroa.\n";
		std::cout << "Loppusummaksenne jaa " << kokonaishinta << " euroa.\n";
	}
	// Sama kuin edellä, mutta jos satunnaislukugeneraattori on generoinut alennusprosenttimuuttujalle
	// arvoksi 3, se vastaa 20 % alennusta.
	else if (alennusprosentti == 3 && !(yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0)) {
		int alennus = (kokonaishinta * 20) / 100;
		kokonaishinta -= alennus;
		std::cout << "Sinulla kavi tuuri, sait satunnaisesti arvotun 20 % alennuksen. Alennuksen arvo on: " << alennus << " euroa.\n";
		std::cout << "Loppusummaksenne jaa " << kokonaishinta << " euroa.\n";
	}
	// Jos käyttäjä on varannut huoneita, mutta käyttäjä ei saanut satunnaisesti generoitua alennusta
	// (eli alennusprosenttimuuttujalle generoitiin arvoksi 1) kerrotaan käyttäjälle vain tämän
	// varakokonaishinta.
	else if (!(yvarauksenhuoneet.size() == 0 && kvarauksenhuoneet.size() == 0)) {
		std::cout << "Varauksienne hinta: " << kokonaishinta << " euroa.\n";
		std::cout << "Toivottavasti viihdytte hotellissamme.\n";
		std::cout << "Muistakaa, jos teillä tulee jotain kysyttävää, yhteystietomme löytyvät nettisivuiltamme, sekä jokaisesta huoneesta.\n";
	}
}

// Funktio, jolla mahdollistetaan halutun varauksen poistaminen.
void poistaVaraus(std::vector<Varaus>& varaukset) {
	// Luodaan poistettavan varauksen valinnalle muuttuja 'poista', ja alustetaan
	// se nollaksi. Esitetään käyttäjälle kaikki tämän tekemät varaukset silmukan 
	// avulla, jota toistetaan yhtä monta kertaa, kuin (huonetyypin) varauksia on 
	// yhteensä. Numeroidaan varaukset, että käyttäjän on helpompi nähdä ennen päätöstä, 
	// minkä varauksen tämä tahtoo poistaa.
	int poista = 0;
	std::cout << "Tassa on yhden hengen huoneiden varauksesi:\n\n";
	for (int i = 0; i < varaukset.size(); i++) {
		std::cout << "Varaus " << i + 1 << ":\n";
		// Hienosäätöä huone-sanan muotoilulle.
		if (varaukset[i].huoneidennumerot.size() == 1) {
			std::cout << "Huoneen tyyppi: " << varaukset[i].huonetyyppi << "\n";
		}
		else {
			std::cout << "Huoneiden tyyppi: " << varaukset[i].huonetyyppi << "\n";
		}
		std::cout << "Oiden maara: " << varaukset[i].yomaara << "\n";
		// Hienosäätöä huone-sanan muotoilulle.
		if (varaukset[i].huoneidennumerot.size() == 1) {
			std::cout << "Huoneen numero: ";
		}
		else {
			std::cout << "Huoneiden numero: ";
		}
		// Tehdään varauskohtaisen huonenumeroiden tulostamiselle silmukka, joka
		// toisetaan yhtä monta kertaa, kuin varauksessa on huoneita. Ennen
		// huonenumeroiden tulostamista järjestellään varauskohtaiset huonenumerot
		// suuruusjärjestykseen pienimmästä suurimpaan käyttökokemuksen parantamiseksi.
		// Lisäksi, jos huonenumeroita on varauksessa vielä lisää jäljellä, tulostetaan
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
	// Kysytään käyttäjältä, minkä varauksen tämä tahtoo poistaa, pienin on valinta voi olla 1, 
	// ja suurin valinta voi olla varauksien kokonaismäärä, eli viimeisen huonetyyppivarauksen
	// numero. Tallennetaan valinta muuttujaan 'poista'. Tämän jälkeen poistetaan varaus vektorista
	// erase-komennon avulla. Selataan vektorin tietoja alustapäin, ja valitaan poistettava varaus 
	// erottamalla käyttäjän valinnasta 1, koska ensimmäisen varauksen numero on 1, mutta ensimmäinen
	// varaus on vektorissa paikkanumerolla 0.
	poista = virhetarkistus("Minka varauksen haluatte poistaa?\n", 1, varaukset.size(), "Kyseista varausta ei loydetty. Syota uudelleen.\n");
	varaukset.erase(varaukset.begin() + (poista - 1));
}