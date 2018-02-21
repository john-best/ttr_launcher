# YATTRL (np)
--------------

_Yet Another Toontown Rewritten Launcher (name pending)_

A launcher made for [Toontown Rewritten](https://www.toontownrewritten.com), because the current launcher (as of Feb 20, 2018) seems a bit out of date. Let it be known that this was a project to learn more about Qt, so things may not be of the highest quality.

### What's different? ###

* Username saving on successful login
* Buttons to Toontown Rewritten's website and also [ToonHQ's website](https://toonhq.org)
* News opens up the TTR website to the latest news.
* Resource pack support
* That's pretty much it.

### What's next? ###
* Wait for QWebEngineView to be better for better UI for news
* Better resource pack support
* Mac and Linux support
* A more sophisticated theme/style

## Compiling ##
You're going to need a few things to make this work from scratch.
* Qt (tested on 5.10.1, but may work on others)
* Some compiler (tested on MSVC2015 x86/x64, but may work on others)
* A Windows machine
* Boost C++ Libraries (tested on 1.66.0, but may work on others)

## Running ##
Probably just run the ttr_launcher.exe file in the same directory as your TTR install. If TTR isn't installed already, the launcher will start downloading a fresh copy of TTR into the directory that ttr_launcher is in.

[Here's](https://github.com/john-best/ttr_launcher/releases/tag/v1.0.0) a .zip of everything you need (Maybe missing the OpenSSL dll files? I'm not sure).

## Contributing ##
I'm not sure I don't think anyone wants to contribute to this but if you do want to just send in a pull request or something.

# A photo #
![Let's rework this styling someday.](https://i.imgur.com/zsLAtXS.png)
