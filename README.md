# Planetary

## 2020-Fixup

The changes included in this branch stop it from being mergeable to master.  Too many dependencies and small things were changed or included.  On a positive note this branch can be checked out and run immediately using Xcode 12.

### Perquisites
1. An Apple Music library or subscription
2. Albums added to your library

### Steps to run:
1. Checkout this branch
2. Open in Xcode 12
3. Set the build device to either Any iOS DEvice or an actual physical device.
4. Build and Run

### Notes:

* **Running via the simulator wont work** as an Apple Music subscription is required.  There are also missing cinder symbols for the simulator.  The version included with this branch is old and a combination of various bits and pieces I hunted down on the internet. If you try to build for the simulator you'll see the following warning: 

`ld: warning: ignoring file ../cinder86/lib/libcinder-iphone-sim_d.a, missing required architecture arm64 in file ../cinder86/lib/libcinder-iphone-sim_d.a (2 slices)`

* App won't build for Mac as is, because Catalyst won't work due to OpenGL depractation.

* When the app is first run it may get stuck or not identify your library, due to music permissions and timing issues.  If this happens rerun it.

### General updates include:

* Update for 64bit architecture
* Updates to project file and build settings
* Update to make work with default clang compiler
* Update to a compatible cinder with some tweaks
* Update boost (cinder/boost combination is a bit Frankenstein)
* Rotation lock to landscape to avoid deprecated setStatusBar code
* Touch fixes
* General things to stop crashes (including some threading :/)
* Some hacks and fixes to make iPhone compile and build and look 'ok'
* A thousand things I've forgotten!

Rendering/textures are the original **bloom** versions.  OpenGL is still deprecated on Apple platforms, but since this code uses glow effects, on modern devices there is no need for any fancy stuff and it hums along quite nicely.  Even though it can run on iOS 9 devices+ I recomend running on fairly new hardware as some optimisations (loading) were removed to facilitate getting it to compile and work.

Any questions get in touch at:

* [Twitter **@kemalenver**](www.twitter.com/kemalenver)
* [GitHub **@kemalenver**](www.github.com/kemalenver)

## See also

* [Planetary: collecting and preserving code as a living object](https://www.cooperhewitt.org/object-of-the-day/2013/08/26/planetary-collecting-and-preserving-code-living-object) (Cooper-Hewitt Object of the Day weblog)
* [Planetary object page on the Cooper-Hewitt collections website](http://collection.cooperhewitt.org/objects/35520989/)
* [Planetary extras](https://github.com/cooperhewitt/PlanetaryExtras)
