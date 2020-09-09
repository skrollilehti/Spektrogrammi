# Spektrogrammi (Skrolli 2020.3)

Toimii ympäristössä Ubuntu 18.04. Ei ulkoisia koodiriippuvuuksia.

## Kääntäminen

```
make build_image
```

## Ajaminen

```
Spectrogram [OPTION...]

-w, --width arg       Image width (default: 1920)
-r, --resolution arg  SFTP window size (must be a power of 2). A window of
                      size r samples r/2 different frequencies (default:
                      4096)
-s, --start arg       Start time in seconds (default: 0)
-e, --end arg         End time in seconds (default: 1000000000)
    --db-floor arg    Only frequencies louder than this are drawn (dB)
                    (default: -8)
    --db-ceil arg     Frequencies louder than this are clamped to this
                    value (dB) (default: -5)
    --fmin arg        Crop minimum frequency to be shown on the image
                    (default: 0)
    --fmax arg        Crop maximum frequency to be shown on the image
                    (default: 22050)
-i, --input arg       Input wav-file with 44100Hz sample rate
-o, --output arg      Output bmp file (default: out.bmp)
    --help            Print usage instructions

```

## Esimerkki

```
./build_image -i data/dial_up.wav -w 4000 -r 1024 --db-floor -5.0 --db-ceil -1.0 -o dial_up.bmp
```

## Rajoitteet

Wav-tiedoston bittileveys tulee olla 16.

## Tekijä

Jarno N. Alanko (https://jnalanko.net)