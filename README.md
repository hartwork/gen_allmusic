# gen_allmusic

## About

The *Allmusic Hotkey Winamp Plugin* (a.k.a. **gen_allmusic**) started out in response
to a [plug-in request by users `Veej007` and `Spikeismoo`](http://forums.winamp.com/showthread.php?threadid=156390)
at the official Winamp community forum in 2006.

The plugin provides three hotkey actions that can be assigned
by the Global Hotkeys Plugin:

- `Allmusic: Browse for Artist`
- `Allmusic: Browse for Album`
- `Allmusic: Browse for Song`

Additionally, when a new track begins the Minibrowser will navigate
to the artist's page at Allmusic.


## Requirements

- Winamp 5.21
- Winamp Library 2.3 (`gen_ml.dll`)
- Global Hotkeys 1.31 (`gen_hotkeys.dll`)
- Minibrowser support


## History

### 1.4 (2006-04-21)

- Fixed: A closed Minibrowser is not opened on track change


### 1.3 (2006-04-20)

- Added: Artist page is auto-loaded on track change
- Fixed: Escaping now uses `|` instead of `+` (thanks to *osmosis*)


### 1.1 (2006-04-08)

- Added: Album and song support
- Fixed: Reduced URL escaping (`Jay-Z` should work now.)


### 1.0 (2006-04-07)
