# README for iOS openssl

## Build Instructions

Used project https://github.com/krzyzanowskim/OpenSSL

That project creates openssl libraries for iOS device / simulator.
The libraries are separate for device / simulator but we will join them into a single `universal` library below :)

The project built as-is on a Mac with XCode and CLT installed.  The build takes a long time but does work.

## Libraries stored on OneDrive

Please copy the specific built libraries to the OneDrive folder.
Here's a working example from my box (note the differences in the destination folder names):

```
# copy raw files
OPENSSL_IOS_FOLDER="$HOME/proj/git/src/github.com/krzyzanowskim/OpenSSL"
ONEDRIVE_FOLDER="$(find "$HOME" -maxdepth 1 -name 'OneDrive*' -type d | sort --reverse | head -n 1)"
ONEDRIVE_APPLE_FOLDER="$ONEDRIVE_FOLDER/TripleChain/OutsideSoftwarePackages/openssl/apple"
cp "$OPENSSL_IOS_FOLDER"/iphoneos/lib/lib*.a "$ONEDRIVE_APPLE_FOLDER"/iPhone/
cp "$OPENSSL_IOS_FOLDER"/iphonesimulator/lib/lib*.a "$ONEDRIVE_APPLE_FOLDER"/iPhoneSimulator/
#
cd "$ONEDRIVE_APPLE_FOLDER"
files="$(find . -name 'libssl*' -type f) $(find . -name 'libcrypto*' -type f)"
for i in $files ; do
  the_dir=$(dirname "$i")
  the_file=$(basename "$i")
  the_newfile=$(echo "$the_file" | sed -e 's/^lib\(.*\)/lib\1/')
  echo mv "$i" "$the_dir/$the_newfile"
   mv "$i" "$the_dir/$the_newfile"
done
cd -
```

## "Fat" Library (Universal)

**NOTE: This section is historical - we turned out *not* to need a "fat" openssl library.**

~~Once built, we need to build a 'universal' fat library:~~
1. ~~Set a variable to the top-level folder for the `OpenSSL` github project. Example:~~

    ```
    OPENSSL_IOS_FOLDER="$HOME/proj/git/src/github.com/krzyzanowskim/OpenSSL"
    ```
1. ~~Open a Terminal and change to folder `External/openssl/ios/universal` under the TripleCyber solution folder.~~
1. ~~Run these command to combine `iphoneos` and `iphonesimulator` libraries into a single library:~~
   * ~~For `libcrypto`:~~

        ```
        cp "$OPENSSL_IOS_FOLDER/iphoneos/lib/libcrypto.a" ./libcrypto-iphoneos.a
        cp "$OPENSSL_IOS_FOLDER/iphonesimulator/lib/libcrypto.a" ./libcrypto-iphonesimulator.a
        ../combine-static-libraries.sh libcrypto ./libcrypto-iphonesimulator.a ./libcrypto-iphoneos.a
        rm -f ./libcrypto-*.a
        ```
   * ~~For `libssl`:~~

        ```
        cp "$OPENSSL_IOS_FOLDER/iphoneos/lib/libssl.a" ./libssl-iphoneos.a
        cp "$OPENSSL_IOS_FOLDER/iphonesimulator/lib/libssl.a" ./libssl-iphonesimulator.a
        ../combine-static-libraries.sh libssl ./libssl-iphonesimulator.a ./libssl-iphoneos.a
        rm -f ./libssl-*.a
        ```

That is all.
