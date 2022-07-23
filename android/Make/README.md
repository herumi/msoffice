# TripleCyber MSOC for Android

## Build

On a *compliant* Mac (NDK, GNU Make) simply use:

```
make
```

That should build all libraries. The built libraries are not checked into
source control; instead they must be copied to the OneDrive share.

This is the same technique used to store OpenSSL libraries for use during
Xamarin builds - libraries are built out-of-band and copied to the OneDrive.

## Copy to OneDrive

Use the following or something close to it (run in this directory):

```
# copy raw files
ONEDRIVE_FOLDER="$(find "$HOME" -maxdepth 1 -name 'OneDrive*' | sort --reverse | head -n 1)"
ONEDRIVE_MSOC_FOLDER="$ONEDRIVE_FOLDER/TripleChain/OutsideSoftwarePackages/msoc"
ONEDRIVE_ANDROID_FOLDER="$ONEDRIVE_MSOC_FOLDER/android"
mkdir -p "$ONEDRIVE_ANDROID_FOLDER"/arm64-v8a "$ONEDRIVE_ANDROID_FOLDER"/armeabi-v7a "$ONEDRIVE_ANDROID_FOLDER"/x86 "$ONEDRIVE_ANDROID_FOLDER"/x86_64
cp ./build_dir/arm64-v8a/lib/lib* "$ONEDRIVE_ANDROID_FOLDER"/arm64-v8a/
cp ./build_dir/armeabi-v7a/lib/lib* "$ONEDRIVE_ANDROID_FOLDER"/armeabi-v7a/
cp ./build_dir/x86/lib/lib* "$ONEDRIVE_ANDROID_FOLDER"/x86/
cp ./build_dir/x86_64/lib/lib* "$ONEDRIVE_ANDROID_FOLDER"/x86_64/
```

That is all.
