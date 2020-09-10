# Spektrogrammi (Skrolli 2020.3)

Toimii ympäristössä Ubuntu 18.04. Ei ulkoisia koodiriippuvuuksia.

## Kääntäminen

```
make spectrogram
```

Binääri tulee tiedostoon bin/spectrogram

## Ajaminen

```
Ottaa syötteenä wav-tiedoston ja kirjoittaa spektrogrammin bmp-tiedostoon
Usage:
  spectrogram [OPTION...]

  -w, --width arg       Kuvan leveys pikseleinä (default: 1920)
  -r, --resolution arg  FFT ikkunan koko. Täytyy olla kahden potenssi.
                        (default: 4096)
  -s, --start arg       Alkuaika äänitiedostossa (default: 0)
  -e, --end arg         Loppuaika äänitiedostossa (default: 1000000000)
      --db-floor arg    Amplitudiasteikon katkaisukynnys alhaalta (log10)
                        (default: -8)
      --db-ceil arg     Amplitudiasteikon katkaisukynnys ylhäältä (log10)
                        (default: -5)
      --fmin arg        Pienin kuvaan piirrettävä taajuus (default: 0)
      --fmax arg        Suurin kuvaan pirrettävä taajuus (default: 22050)
  -i, --input arg       Syöte-audio wav-formaatissa
  -o, --output arg      Pirrettävän spektrogrammin tiedostonimi
                        (bmp-formaatti) (default: out.bmp)
      --help            Tulosta käyttöohjeet


```

## Esimerkki

```
./bin/spectrogram -i data/dial_up.wav -w 4000 -r 1024 --db-floor -5.0 --db-ceil -1.0 -o dial_up.bmp
```

## Rajoitteet

Wav-tiedoston bittileveys tulee olla 16.

## Tekijä

Jarno N. Alanko (https://jnalanko.net)