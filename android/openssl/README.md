# README for Android openssl

NOTES:
1. **These instructions executed on Mac. Windows is problematic at best.** You will need:
    * *Build tools* - Most are auto-installed with Visual Studio for Mac.
    * *Command-line tools* - These include `make` and `perl`. Lookup `homebrew` and install other needed tools using `brew` on your Mac if necessary.
    * *Android SDK* - Verify that you have installed an Android SDK via Visual Studio to match the `ANDROID_API` level you specify below.
    * *Android NDK* - Verify that the Android NDK in installed via Visual Studio (NDK is *not* installed by default). The latest NDK appears to be fine regardless of `ANDROID_API` level selected below.
1. Pull down the build project from github:

    ```
    cd [top-level-folder]
    git clone https://github.com/andybrucenet/openssl_for_ios_and_android.git
    cd openssl_for_ios_and_android
    ```
1. Set a few helper variables used below:
    * `OPENSSL_VERSION` - Identifies the openssl version desired
    * `ANDROID_HOME` - The example should work for Visual Studio
    * `ANDROID_API` - This is the minimum API version to support; `29` is required by Xamarin Forms.
    * **Examples** - The following sets working examples as of DEC 2020:

        ```
        export OPENSSL_VERSION='openssl-1.1.1i'
        export ANDROID_HOME="$HOME/Library/Developer/Xamarin/android-sdk-macosx"
        export ANDROID_API='29'
        ```
1. Perform the build:

    ```
    export ANDROID_NDK_ROOT="$ANDROID_HOME/ndk-bundle"
    export version="$(echo $OPENSSL_VERSION | sed -e 's/^openssl-\(.*\)/\1/')"
    export api="$ANDROID_API"
    cd ./tools
    ./build-android-openssl.sh
    cd ..
    ```
1. The build will take less than an hour.

That is all.
