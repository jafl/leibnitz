[![Github CI](https://github.com/jafl/leibnitz/actions/workflows/ci.yml/badge.svg)](https://github.com/jafl/leibnitz/actions/workflows/ci.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_leibnitz&metric=alert_status)](https://sonarcloud.io/dashboard?id=jafl_leibnitz&branch=main)

[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_leibnitz&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=jafl_leibnitz&branch=main)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_leibnitz&metric=security_rating)](https://sonarcloud.io/dashboard?id=jafl_leibnitz&branch=main)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_leibnitz&metric=vulnerabilities)](https://sonarcloud.io/dashboard?id=jafl_leibnitz&branch=main)

[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_leibnitz&metric=ncloc)](https://sonarcloud.io/dashboard?id=jafl_leibnitz&branch=main)

# Leibnitz

Leibnitz provides a calculator and the ability to plot functions.  The [on-line help](http://leibnitz.sourceforge.net/help.html) explains all the features of the program.


## Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


## Installing a binary

For macOS, the easiest way to install is via [Homebrew](https://brew.sh):

    brew install --cask xquartz
    brew tap jafl/jx
    brew install jafl/jx/leibnitz

For all other systems, download a package:

[![Download from Github](http://libjx.sourceforge.net/github.png)](https://github.com/jafl/leibnitz/releases/latest)

<a href="https://sourceforge.net/p/leibnitz/"><img alt="Download Leibnitz" src="https://sourceforge.net/sflogo.php?type=17&group_id=170488" width="250"></a>

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-white.svg)](https://snapcraft.io/leibnitz)

If you download a tar, unpack it, cd into the directory, and run `sudo ./install`.

Otherwise, put the program (`leibnitz`) in a directory that is on your execution path.  `~/bin` is typically a good choice.


## Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `./configure`
1. `make`


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).

### Usage

#### How do I clear the expression and start over after pressing '=' without using the mouse?

Use Meta-A. (Select All on the Edit menu)  Typing will replace the selection.


#### How do I avoid saving the contents of the tape between sessions?

Use the Close button instead of the Quit menu item.


#### Why did you call it Leibnitz?

The program is advocating a better way to display calculations, just like Leibnitz advocated a notation for Calculus that proved superior to what Newton had invented.
