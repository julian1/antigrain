/*
    load and render freetype to span data.
    use type and length to wowk out the covers

    Usage,
    ./build10.sh
    ./a.out fonts/arial.ttf  18 > out.cpp 
    edit out.cpp for the structure name
    g++ -c out.cpp  -I ./agg-svn-r138-agg-2.4/include/
    cp out.cpp  ~/devel/stm32/stm32f4/projects/control-panel-2/src/arial-span-18.cpp

    scanlines for all glyphys is pretty large.

*/




#include <iostream>
#include <vector>
#include <regex>

#include <assert.h>

#include "agg_font_freetype.h"

#include "agg_conv_curve.h"
#include "agg_conv_contour.h"

#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"


#include "agg_conv_transform.h"

#include "agg_pixfmt_rgb_packed.h"  // rgb565


/*
  IMPORTANT.
    an advantage of this simple path structure - is that it can be generated at runtime or compile time.
    would just use a contiguous uint8_t  container. and pass the pointer.
    drawing routines stay the same.
  ---
  TODO 
    - done - get rid of the template arguments .
    - done - implement struct storage - to be able to pass around the fontface with one pointer.
    - done - get rid of the clipbox .
    - done - get rid of the mtx transform - and just use the height/width of the font vertex. - better for hinting etc.
*/

using namespace agg;



class pixfmt_span_generator
{
  // TODO change name pixfmt my_pixfmt pixfmt_span_generator etc.
  // eg. adapted from agg_pixfmt_rgb_packed.h
public:

    // used by renderer_base
    typedef const_row_info<int8u>                     row_data;
    typedef typename agg::blender_rgb565::color_type  color_type;


private:
    int scroll_start;
public:

    explicit pixfmt_span_generator( int scroll_start_ )
      : scroll_start( scroll_start_ )
    {}



    // AGG_INLINE unsigned width()  const { return 480;  }
    // AGG_INLINE unsigned height() const { return 272; }
    // AGG_INLINE int      stride() const { return m_rbuf->stride(); }

    AGG_INLINE void copy_hline(int x, int y,
                               unsigned len,
                               const color_type& c)
    {
      assert(0);
      /*
        // std::cout << "copy_hline       x " << x << " y " << y << " len " << len << " (r " << int(c.r) << " g " << int(c.g) << " b " << int(c.b) << ")"  << std::endl;
      */


    }


    static int squash8u( int x )
    {
      // squash value into an int8_t  type.   
      // should do bounds checking here

      // negative of twos compment
      return (uint8_t ) (int8_t) x; 

      // return (x >= 0 ? x : 0xff - x);
    } 

    void blend_solid_hspan(int x, int y,
                           unsigned len,
                           const color_type& c,
                           const int8u* covers)
    {
      /*
      std::cout << "blend_solid_hspan x " << x << " y " << y << " len " << len << " (r " << int(c.r) << " g " << int(c.g) << " b " << int(c.b) << ")"  ;
      std::cout << " covers ";
      for(unsigned i = 0; i < len; ++i ) {
        std::cout << int(covers[i]) << ", ";
      }
      std::cout << std::endl;
      */

      // just use a flat static array strucutre
      // instead of all the addition handle in the called func

      // write the span type and span data
      std::cout 
        << (0x01 << 7) 
        << ", " << squash8u(x) 
        << ", " << squash8u(y)  
        << ", " << len 
        << ", "  ;

      // write the covers
      for(unsigned i = 0; i < len; ++i ) {
        std::cout << int(covers[i]) << ", ";   // we can add a c terminator
      }
      std::cout << "\n";
    }

    void blend_hline(int x, int y,
                     unsigned len,
                     const color_type& c,
                     int8u cover)
    {

      // std::cout << "blend_hline       x " << x << " y " << y << " len " << len << " (r " << int(c.r) << " g " << int(c.g) << " b " << int(c.b) << ")"  << " cover " << int(cover) << std::endl;

      // write the span type and data
      std::cout 
          << (0x01 << 6) 
          << ", " << squash8u( x )
          << ", " << squash8u( y)  
          << ", " << len 
          << ", "  << int(cover) 
          << ", " << "\n";
    }

};





template<class PixelFormat> class renderer_base_no_clip
{
/*
  change name renderer_base_no_clip
  renderer_base without a clipbox.

  no clip - supports negative coordinates needed for font  spans.
          - also should be faster, if we know drawing will not extend past bounds. 
*/
  // adapted from agg_renderer_base.h
public:
    typedef PixelFormat pixfmt_type;
    typedef typename pixfmt_type::color_type color_type;
    typedef typename pixfmt_type::row_data row_data;


    explicit renderer_base_no_clip(pixfmt_type& ren) :
        m_ren(&ren)
    {}

  ////////

    pixfmt_type* m_ren;


    void blend_solid_hspan(int x, int y, int len, 
                           const color_type& c, 
                           const cover_type* covers)
    {
      // no clip 
      m_ren->blend_solid_hspan(x, y, len, c, covers);
    }


    void blend_hline(int x1, int y, int x2, 
                     const color_type& c, cover_type cover)
    {
      // no clip
      m_ren->blend_hline(x1, y, x2 - x1 + 1, c, cover);
    }
};







typedef pixfmt_span_generator pixfmt_t;

typedef renderer_base_no_clip<pixfmt_t>   rb_t ;



// serialized path type used by the font manager
typedef  agg::serialized_integer_path_adaptor<short int, 6> font_path_type;



int main(int argc, char **argv)
{

    // see agg-svn-r138-agg-2.4/font_freetype/agg_font_freetype.h
    // don't really need the font_manager.
    typedef agg::font_engine_freetype_int16 font_engine_type;
    typedef agg::font_cache_manager<font_engine_type> font_manager_type;

    font_engine_type             m_feng;
    font_manager_type            m_fman(m_feng);



    ////////////
    std::vector<bool>  glyph_codes;
    std::vector< int> glyph_advance_x;
    std::vector< int> glyph_advance_y;

    glyph_codes.assign(256, false);
    glyph_advance_x.assign(256, 0);
    glyph_advance_y.assign(256, 0);

    assert(glyph_advance_x.size() == 256);

    // how do we decode an integer argument?
    if(argc != 3) {
      std::cout << "usage: a.out <fontfile.ttf> <size>" << std::endl;
      exit(1);
    }
    const char *font_filename = argv[1 ];
    int fontsize = std::stoi( argv[2]);
    // std::string fontname = std::regex_replace(font_filename, std::regex("\\.*"), "");
    // std::cout << "font name " << fontname << std::endl;

    // write_c_header();

    // "timesi.ttf"
    if(m_feng.load_font( font_filename, 0, agg::glyph_ren_outline))
    {
        m_feng.hinting(false);

        m_feng.height( fontsize );
        m_feng.width( fontsize );

        m_feng.flip_y( 1 );


        std::cout << "#include <stdint.h> // uint8_t" << std::endl;

        // all want to iterate all glyps??
        // rather than loop the text we should loop the glpys. but should get this working first

        const char *select = "1234567890 .+-MkmupfVAW";

        // for(unsigned code = 0; code < glyph_codes.size(); ++code)
        for(const char *p = select; *p; ++p)
        {
            unsigned code = *p;
            const agg::glyph_cache* glyph = m_fman.glyph( code );

            if(glyph /*&& isalpha( code ) */ )
            {
                // if(x > tcurve.total_length1()) break;

                // m_fman.add_kerning(&x, &y);
                m_fman.init_embedded_adaptors(glyph, 0, 0);

                if(glyph->data_type == agg::glyph_data_outline)
                {
                  // seem to be a lot of glyphs with the same data, eg. not proper glyphs.

                  pixfmt_t  pixf(  0 );
                  rb_t    rb(pixf);

                  // will need to be uint16_t... if larger than 255.
                  // handlling signedness is a real problem.
                  // and so is invertedness.
                  std::cout << "// '" << (code == 10 || code == 13 || code == 92 ? 'x' : char(code)) << "'" << std::endl;
                  std::cout << "static uint8_t glyph_" << code << "[] = { ";
                  std::cout << std::endl ;


                  font_path_type &m_path = m_fman.path_adaptor();
                  // s_path.m_scale = 1; must be set.

                  agg::conv_curve<font_path_type > curve(m_path );

                  agg::rasterizer_scanline_aa<> ras;
                  agg::scanline_p8 sl;

                  ras.add_path( curve );
                  agg::render_scanlines_aa_solid(ras, sl, rb, agg::rgba(1,0,0));

                  assert( code >= 0 && code < glyph_codes.size());

                  // write_glyph_c_data( code, m_path);

                  // record glyph information
                  glyph_codes[ code] = 1;
                  glyph_advance_x[code] = glyph->advance_x;
                  glyph_advance_y[code] = glyph->advance_y;

                  // sentinel
                  std::cout << (0x01 << 5) << "\n" ;


                  std::cout << "};" << "\n" ;


                }
            }
        }


      

        std::cout << "#include \"fonts.h\"" << "\n\n" ;
        
        std::cout << "const FontSpans " <<  font_filename << "_span_" << fontsize << " = {" << std::endl;

        // glypa is ok
        std::cout << "{" << std::endl;
        for(unsigned i = 0; i < glyph_codes.size(); ++i)
        {
          if(glyph_codes[i])
            std::cout << "glyph_" << i ;
          else
            std::cout << "0" ;

          if(i < glyph_codes.size() - 1)
            std::cout << ", " ;
        }
        std::cout << "}," << "\n\n";



        // glyph advance x
        std::cout << "{" << std::endl;
        for(unsigned i = 0; i < glyph_codes.size(); ++i)
        {
          std::cout << int( glyph_advance_x[i] );

          if(i < glyph_codes.size() - 1)
                std::cout << ", " ;
        }
        std::cout << "}," << "\n\n";


        // glyph advance y. always 0. 
        std::cout << "{" << std::endl;
        for(unsigned i = 0; i < glyph_codes.size(); ++i)
        {
          std::cout << int(glyph_advance_y[i] );

          if(i < glyph_codes.size() - 1)
                std::cout << ", " ;
        }
        std::cout << "}" << "\n\n";


        std::cout << "};" << std::endl;

    }
    else
    {
        std::cout << "could not open font \n" ;
    }


    return 0;
}


