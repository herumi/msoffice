#!/bin/sh
# combine-static-libraries.sh, ABr
#
# Combined all provided static libaries from command line into a single static library.
# Inspired by:
# * Evan Schoenberg, Regular Rate and Rhythm Software
# * Claudiu Ursache (see http://www.cvursache.com/2013/10/06/Combining-Multi-Arch-Binaries/)
#####
# $1 = Name of output archive
# $* = List of library files to combine
#####

# handle input
[ x"$1" = x ] && echo 'Must provide basename of output fat library' && exit 1
the_output_library="$1"; shift
rm -f "$the_output_library.a"

libraries="$*"
[ x"$libraries" = x ] && echo 'Pass in list of libraries to combine' && exit 1

# these are the architectures that we know we need - add more as necessary
archs=(x86_64 arm64 arm64e)

# get latest libtool installed
libtool="`which -a libtool | sort -r | head -n 1 | dos2unix`"

echo "Combining ${libraries}..."

for library in ${libraries}
do
  # this command shows all the architectures contained within a static library
  lipo -info $library
  all_archs="|`lipo -info $library 2>&1 | sed -e "s#^.* $library are: \(.*\)#\1#" | sed -e 's# #|#g' | dos2unix`"
  library_name="`echo $(basename "$library") | sed -e 's#\.a$##' | dos2unix`"

  # Extract individual architectures we require from this this library 
  for arch in ${archs[*]}; do
    # a given library may not include all of our architectures
    if echo "$all_archs" | grep -e "|$arch|" >/dev/null 2>&1 ; then
      lipo -thin $arch $library -o ${library_name}_${arch}.a
    fi
  done
done

# retrieve a list of all the individual architectures we found from all input libraries
source_combined=""
for arch in ${archs[*]}
do
  source_libraries=""
   
  mkdir $arch
  cd $arch
  for library in ${libraries}; do
    library_name="`echo $(basename "$library") | sed -e 's#\.a$##' | dos2unix`"
    #echo "library='$library'; library_name='$library_name'"

    if [ -s ../${library_name}_${arch}.a ]; then
      # Extract the libraries, overwriting duplicated files with the same name in each lib
      ar x ../${library_name}_${arch}.a
      rm -f __.SYMDEF
      source_libraries="${source_libraries} ${library_name}_${arch}.a"
    fi
  done
  cd ..
  
  $libtool -static ${arch}/*.o -o "${the_output_library}_${arch}.a"
  source_combined="${source_combined} ${the_output_library}_${arch}.a"
  
  # Delete intermediate files
  rm -f ${source_libraries}
  rm -rf ${arch}
done

# Merge the combined library for each architecture into a single fat binary
lipo -create $source_combined -o "$the_output_library.a"

# Delete intermediate files
rm ${source_combined}

# Show info on the output library as confirmation
echo "Combination complete."
lipo -info "$the_output_library.a"

