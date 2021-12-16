
#
#  need to pick up freetype-config for include and lib paths.
#  nix-shell -p  freetype gcc10
#
#  // need a couple of patches to agg . 

# edit, agg_font_freetype2.cpp
#  // JA
#  #define stricmp strcasecmp
#
#  rm agg_test.ppm  ; ./a.out ;  feh agg_test.ppm
#


AGG=./agg-svn-r138-agg-2.4

g++ main9.cpp     \
  $AGG/src/agg_curves.cpp $AGG/src/agg_vcgen_contour.cpp   \
  $AGG/font_freetype/agg_font_freetype2.cpp $AGG/font_freetype/agg_font_freetype.cpp  $AGG/src/agg_trans_affine.cpp   \
  -I $AGG/include/    -I $AGG/font_freetype/ \
  $(freetype-config  --cflags) \
  $(freetype-config  --libs)




#   ${AGGSRC}/agg_trans_affine.cpp  ${AGGSRC}//agg_curves.cpp
