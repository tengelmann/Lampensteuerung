# Beschreibung

Das Repository enthält alle archivierten Dokumente, Software und Layout-Dateien einer selber entwickelten digitalen Lampensteuerung.
Die Entwicklung wurde 2006 im Forum von MTB-News.de gestartet.

[Link zu Thread auf MTB-News.de "Elektronische Lampensteuerung selberbauen"](http://www.mtb-news.de/forum/showthread.php?p=2458216)

![Image Single V.3](https://github.com/tengelmann/Lampensteuerung/blob/master/Web/images/singlev3/singlev3_2.jpg)

## Funktionen
* 2 Ausgänge für Halogen oder Led-Treiber
* Lampenleistung bis max. 75W
	* 1-Ausgang-Version: 1 Halogenlampe mit 50W-Spot (70W @16V)
	* 2-Ausgänge-Version: 2 Halogenlampe mit max. 2x20W (65W @16V), oder 1x20W+1x35W (75W @16V)
* Messung der Akkuspannung über Spannungsteiler
	* Bestimmung der Restleuchtdauer
	* Nachregelung der Ausgänge 
* Restlaufzeitanzeige über RGB-Leds
* Ansteuerung von 1 oder 2 Ausgängen mit unterschiedlichen Modi
	* Konstantes PWM-Verhältnis, kein Softstart
	* Konstantes PWM-Verhältnis, Softstart
	* Konstante Spannung, kein Softstart, PWM-Regelung entsprechend der aktuellen Akku-Spannung
	* Konstante Spannung, Softstart, PWM-Regelung entsprechend der aktuellen Akku-Spannung
	* Konstantes PWM-Verhältnis, kein Softstart, Ausngang invertiert 
* Programmiermodus
	* Auswahl 1 oder 2 Helligkeitsstufen
	* Einstellbare Helligkeit je Stufe
	* Ausgangsmodus je Auasgang
	* PWM-Frequenz der Ausgänge
* Kalibriermodus zum Abspeichern der individuellen Entladekurve des Akkus
* Schutzfunktion bei Kabelwacklern
	* Sorgt dafür dass bei einem Wackelkontakt die Lampe sich sofort wieder einschaltet
	* Aktivierung der Stufe 1, nach Unterbrechung der Stromzufuhr, wenn Lampe vorher an war
* Heimkehrfunktion
	* Reduzierung der Helligkeit bei leerem Akku-Spannung
	* Runterschalten auf Stufe 1 wenn der Akku leer wird
	* Deaktivieren der Stufen 2 und 3 bei sehr geringer Restleuchtdauer
* Optionale Temperaturmessung über externen Temperatursensor
	* Reduzierung der Lampenleistung bei steigender Temperaturmessung
	* Abschalten der Lampe bei Überschreitung des Temperaturlimits

## Version "Duo v.2"

* Erstes Design für Testzwecke
* 2 Taster
* 2 Ausgänge

## Version "Single v.3"

* Finales Design
* Reduzierung auf einen Taster
* Größerer Mikrocontroller (AtTiny84)
* 2 Ausgänge, Optional 1 Ausgang
* 2 RGB Leds
* Variante mit Temperaturregelung für LED-Lampenköpfe
* AVRootloader
* RS232 über 1-Wire zum Debuggen der Software

# Anschlussbelegung

![Anschluss Halogen](https://raw.githubusercontent.com/tengelmann/Lampensteuerung/master/Web/images/singlev3/SingleV3_Anschluss_Halogen.gif)
![Ansclhuss Led](https://raw.githubusercontent.com/tengelmann/Lampensteuerung/master/Web/images/singlev3/SingleV3_Anschluss_Leds.gif)
