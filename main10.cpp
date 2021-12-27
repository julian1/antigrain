/*
    load and render freetype to span data.
    use type and length to wowk out the covers

    Usage, 
    ./build10.sh 
    ./a.out fonts/arial.ttf  > out.cpp 
    g++ -c out.cpp  -I ./agg-svn-r138-agg-2.4/include/

*/




#include <iostream>
#include <vector>

#include <assert.h>

#include "agg_font_freetype.h"

#include "agg_conv_curve.h"
#include "agg_conv_contour.h"

#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"


#include "agg_conv_transform.h"

#include "agg_pixfmt_rgb_packed.h"  // rgb565



using namespace agg;

//===========================================pixfmt_alpha_blend_rgb_packed
template<class Blender,  class RenBuf> class pixfmt_alpha_blend_rgb_packed
{
  // eg. from agg_pixfmt_rgb_packed.h
public:

    // for consumers
    typedef RenBuf   rbuf_type;
    typedef typename rbuf_type::row_data row_data;
    typedef Blender  blender_type;
    typedef typename blender_type::color_type color_type;
/*
    typedef typename blender_type::pixel_type pixel_type;
    typedef int                               order_type; // A fake one
    typedef typename color_type::value_type   value_type;
    typedef typename color_type::calc_type    calc_type;
    enum base_scale_e
    {
        base_shift = color_type::base_shift,
        base_scale = color_type::base_scale,
        base_mask  = color_type::base_mask,
        pix_width  = sizeof(pixel_type),
    };

*/

private:
    int scroll_start;
public:

    explicit pixfmt_alpha_blend_rgb_packed( int scroll_start_ )
      : scroll_start( scroll_start_ )
    {}



    AGG_INLINE unsigned width()  const { return 480;  }
    AGG_INLINE unsigned height() const { return 272; }
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
      std::cout << (0x01 << 7) << ", " << x << ", " << y << ", " << len << ", "  ;

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
      std::cout << (0x01 << 6) << ", " << x << ", " << y << ", " << len << ", "  << int(cover) << ", " << "\n";
    }

};


// packed rgb565
typedef ::pixfmt_alpha_blend_rgb_packed<agg::blender_rgb565, agg::rendering_buffer> pixfmt_t;

typedef agg::renderer_base<pixfmt_t>   rb_t ;



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

    if(argc != 2) {
      std::cout << "no font argument!" << std::endl;
      exit(1);
    }
    const char *font_filename = argv[1 ];


    // write_c_header();

    // "timesi.ttf"
    if(m_feng.load_font( font_filename, 0, agg::glyph_ren_outline))
    {
        m_feng.hinting(false);
        m_feng.height(40);
        m_feng.width(40);

        m_feng.flip_y( 1 );


        std::cout << "#include <stdint.h> // uint8_t" << std::endl;

        // so we want to iterate all glyps??

        // rather than loop the text we should loop the glpys. but should get this working first
        for(unsigned code = 0; code < glyph_codes.size(); ++code)
        {
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
                  std::cout << "// '" << (code == 10 || code == 13 || code == 92 ? 'x' : char(code)) << std::endl;
                  std::cout << "static uint8_t glyph_" << code << "[] = { ";
                  std::cout << std::endl ;


                  font_path_type &m_path = m_fman.path_adaptor();
                  // s_path.m_scale = 1; must be set.

                  // https://coconut2015.github.io/agg-tutorial/agg__trans__affine_8h_source.html
                  agg::trans_affine mtx;
                  mtx *= agg::trans_affine_translation(50 , 50);   // this moves from above origin, back into the screen.
                  mtx *= agg::trans_affine_scaling(2.0);          // now scale it


                  agg::conv_transform<font_path_type > trans(m_path, mtx);
                  agg::conv_curve<agg::conv_transform< font_path_type> > curve(trans);

                  agg::rasterizer_scanline_aa<> ras;
                  agg::scanline_p8 sl;

                  ras.add_path( curve );
                  agg::render_scanlines_aa_solid(ras, sl, rb, agg::rgba(1,0,0));

                  assert( code >= 0 && code < glyph_codes.size());

                  // write_glyph_c_data( code, m_path);

                  glyph_codes[ code] = 1;
                  glyph_advance_x[code] = glyph->advance_x;
                  glyph_advance_y[code] = glyph->advance_y;

                  // std::cout << "good" << std::endl;
                  // for debugging
                  //  write_glyph_structure ( m_path );

                  std::cout << "};" << "\n" ;


                }
            }
        }


      // change name. glyph_face
        std::cout << "uint8_t *glyph[" << glyph_codes.size() << "] = {" << std::endl;
        for(unsigned i = 0; i < glyph_codes.size(); ++i)
        {
          if(glyph_codes[i])
            std::cout << "glyph_" << i ;
          else
            std::cout << "0" ;

          if(i < glyph_codes.size() - 1)
            std::cout << ", " ;
        }
        std::cout << "};" << std::endl << std::endl;



        // glyph advance x
        std::cout << "int glyph_advance_x[" << glyph_codes.size() << "] = {" << std::endl;
        for(unsigned i = 0; i < glyph_codes.size(); ++i)
        {
          std::cout << glyph_advance_x[i] ;

          if(i < glyph_codes.size() - 1)
                std::cout << ", " ;
        }
        std::cout << "};" << std::endl << std::endl;


        // glyph advance x
        // shouldn't be needed....
        std::cout << "int glyph_advance_y[" << glyph_codes.size() << "] = {" << std::endl;
        for(unsigned i = 0; i < glyph_codes.size(); ++i)
        {
          std::cout << glyph_advance_y[i] ;

          if(i < glyph_codes.size() - 1)
                std::cout << ", " ;
        }
        std::cout << "};" << std::endl << std::endl;



    }
    else
    {
        std::cout << "could not open font \n" ;
    }


    return 0;
}







/*
    actually we want a much better structure.
    because we want to be able to work with arbitrary

    we just put it in a structure.
*/
    // this isnt enough we want the advance also.
    // ahhh this is a bit messy
    // and wasteful of memory. but it will do.





// something weird.



#if 0

void drawChar(rb_t & rb, agg::trans_affine &mtx, const agg::rgba &color, font_path_type & path )
{
  // not even sure if it makes sense to factor this


  path.rewind(0);

  std::cout << "drawChar() "  << std::endl;

  // we can move these out of loop if desired...
  agg::rasterizer_scanline_aa<> ras;
  agg::scanline_p8 sl;

  int x  = 0;

  assert( & path );

  // TODO add a translate() method... or add an adapter
  // should add a method translate( dx, dy );
  path.m_dx = x;


  agg::conv_transform<font_path_type> trans( path, mtx);
  agg::conv_curve<agg::conv_transform<  font_path_type > > curve(trans);

  ras.reset();
  ras.add_path( curve );
  // agg::render_scanlines_aa_solid(ras, sl, rb, agg::rgba(0,0,1));
  agg::render_scanlines_aa_solid(ras, sl, rb, color );

}


#endif










#if 0
                    // think these are just offsets - to enable printing
                    std::cout << "m_dx " << x.m_dx << std::endl ;
                    std::cout << "m_dy " << x.m_dy << std::endl ;
                    std::cout << "m_vertices " << x.m_vertices << std::endl ;

                    // agg::rect_d rec = x.bounding_rect ();



                    // eg. iterate the path...
                  // unsigned vertex(double* x, double* y)
                    double x1, y1;
                    unsigned  cmd = x.vertex(&x1, &y1);

                    // std::cout << "bytes " << ( x.m_end - x.m_data ) << std::endl;
                    std::cout << "agg::int8u  x[ " << ( x.m_end - x.m_data ) << "] = { ";

                    for(const agg::int8u *i = x.m_data; i < x.m_end; ++i ) {

                      std::cout << unsigned(*i);

                      if( i + 1 < x.m_end )
                        std::cout <<  ", ";

                    }
                    std::cout << " };" << std::endl;
#endif


// ok. i think the serialized integer path storage doesn't have curves.

#if 0


        //--------------------------------------------------------------------
        rect_d bounding_rect() const
        {
            rect_d bounds(1e100, 1e100, -1e100, -1e100);
            if(m_storage.size() == 0)
            {
                bounds.x1 = bounds.y1 = bounds.x2 = bounds.y2 = 0.0;
            }
            else
            {
                unsigned i;
                for(i = 0; i < m_storage.size(); i++)
                {
                    double x, y;
                    m_storage[i].vertex(&x, &y);
                    if(x < bounds.x1) bounds.x1 = x;
                    if(y < bounds.y1) bounds.y1 = y;
                    if(x > bounds.x2) bounds.x2 = x;
                    if(y > bounds.y2) bounds.y2 = y;
                }
            }
            return bounds;



                    if(m_kerning.status())
                    {
                        m_fman.add_kerning(&x, &y);
                    }

                    if(x >= width() - m_height.value())
                    {
                        x = 10.0;
                        y0 -= m_height.value();
                        if(y0 <= 120) break;
                        y = y0;
                    }

#endif

