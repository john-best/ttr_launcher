# YATTRL (np)
--------------

_Yet Another Toontown Rewritten Launcher (name pending)_

A launcher made for [Toontown Rewritten](https://www.toontownrewritten.com), because the current launcher (as of Feb 20, 2018) seems a bit out of date. Let it be known that this was a project to learn more about Qt, so things may not be of the highest quality.

### What's different? ###

* Username saving on successful login (so you don't have to reenter your username every time)
* Buttons to Toontown Rewritten's website and also to [ToonHQ's website](https://toonhq.org)
* Only latest news (no photo included, will fix someday)
* That's pretty much it.

### What's next? ###
* Better news view (does anyone even read the news? Probably not, but best to make it better anyways)
* Resource pack support (so you can switch between packs (are there even multiple packs?) before launching)
* Mac and Linux support (Most likely doable, I don't have anything to test with, though)
* A more sophisticated theme/style

## Compiling ##
You're going to need a few things to make this work from scratch.
* Qt (tested on 5.10.1, but may work on others)
* Some compiler (tested on MSVC2015 x86/x64 and MinGW x86, but may work on others)
* A Windows machine
* Boost C++ Libraries (tested on 1.66.0, but may work on others)

## Running ##
Probably just run the ttr_launcher.exe file in the same directory as your TTR install. If TTR isn't installed already, the launcher will start downloading a fresh copy of TTR into the directory that ttr_launcher is in.

You're going to need the Qt dlls and some OpenSSL dlls for this to work as well. I'll probably be able to provide all this in a .zip so things will be easy (need to check).

## Contributing ##
I'm not sure I don't think anyone wants to contribute to this but if you do want to just send in a pull request or something.

# A photo #
![Let's rework this styling someday.](https://i.imgur.com/VrbeCAA.png)
