# TripleCyber MSOC for Apple (iOS)

## Build

On a *compliant* Mac (XCode, CLT, GNU Make) simply use:

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
ONEDRIVE_APPLE_FOLDER="$ONEDRIVE_MSOC_FOLDER/apple"
mkdir -p "$ONEDRIVE_APPLE_FOLDER"/iPhone "$ONEDRIVE_APPLE_FOLDER"/iPhoneSimulator "$ONEDRIVE_APPLE_FOLDER"/universal
cp ./build_dir/iphoneos/lib/lib*.a "$ONEDRIVE_APPLE_FOLDER"/iPhone/
cp ./build_dir/iphonesimulator/lib/lib*.a "$ONEDRIVE_APPLE_FOLDER"/iPhoneSimulator/
cp ./build_dir/universal/lib*.a "$ONEDRIVE_APPLE_FOLDER"/universal/
```

That is all.
