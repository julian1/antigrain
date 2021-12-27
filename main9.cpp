/*
    load and render freetype font glyphs to ppm file.
    and write c++ agg outline vector structure

    Useage
    ./build9.sh
    ./a.out ./fonts/arial.ttf
    feh agg_test.ppm


    ./a.out > ~/devel/stm32/stm32f4/projects/control-panel-2/src/arial.cpp

*/

#include <iostream>
#include <vector>

#include <assert.h>

#include "agg_font_freetype.h"
// #include "agg_font_cache_manager.h"
// #include "agg_font_cache_manager2.h"


#include "agg_conv_curve.h"
#include "agg_conv_contour.h"

#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"


#include "agg_conv_transform.h"

#include "agg_pixfmt_rgb.h"

// #include "agg_path_storage_integer.h"






enum
{
    frame_width = 800,
    frame_height = 400
};




// Writing the buffer to a .PPM file, assuming it has
// RGB-structure, one byte per color component
//--------------------------------------------------
bool write_ppm(const unsigned char* buf,
               unsigned width,
               unsigned height,
               const char* file_name)
{
    FILE* fd = fopen(file_name, "wb");
    if(fd)
    {
        fprintf(fd, "P6 %d %d 255 ", width, height);
        fwrite(buf, 1, width * height * 3, fd);
        fclose(fd);
        return true;
    }
    return false;
}




typedef  agg::serialized_integer_path_adaptor<short int, 6> serialized_path_type;

void write_glyph_structure ( serialized_path_type  &x )
{
    std::cout << "--------------" << std::endl;
    std::cout << "m_data "  << ((void *)x.m_data)   << std::endl;
    std::cout << "m_end "   <<  ((void *)x.m_end)   << std::endl;
    std::cout << "m_ptr "   << ((void *) x.m_ptr) << std::endl;
    std::cout << "m_dx  "   << x.m_dx << std::endl;     /// hhmmmmm.... is this respected?
    std::cout << "m_dy  "   << x.m_dy  << std::endl;
    std::cout << "m_scale " << x.m_scale  << std::endl;
    std::cout << "m_vertices " <<  x.m_vertices  << std::endl;
}



// ok. so we have to provide the size for the contructer


void write_c_header( )
{

  std::cout << "#include \"agg_path_storage_integer.h\"" << std::endl;
  std::cout << std::endl ;
  std::cout << "typedef agg::serialized_integer_path_adaptor<short int, 6>  path_type; " << std::endl;
  std::cout << std::endl ;
  std::cout << std::endl ;

}


void write_glyph_c_data( int code, serialized_path_type  &x )
{
  // std::cout << "// "  " '" << char(code) << "' (" << code <<  ")"  << std::endl;
  std::cout << "// '" << (code == 10 || code == 13 || code == 92 ? 'x' : char(code)) << std::endl;

  std::cout << "static agg::int8u code_" << code << "[ " << ( x.m_end - x.m_data ) << "] = { ";

  for(const agg::int8u *i = x.m_data; i < x.m_end; ++i ) {

    std::cout << unsigned(*i);

    if( i + 1 < x.m_end )
      std::cout <<  ", ";
  }
  std::cout << " };" << std::endl ;
  std::cout << std::endl ;

  // constructor
  std::cout << "static path_type glyph_" << code << "(code_" << code << ", " << x.m_end - x.m_data << ", 0, 0, 1.0);" << std::endl;
  std::cout << std::endl ;
}






// So we want to index... and test the load
// we should print out everything... because it's easy to comment it later.

int main(int argc, char **argv)
{

    // see agg-svn-r138-agg-2.4/font_freetype/agg_font_freetype.h
    // don't really need the font_manager.
    typedef agg::font_engine_freetype_int16 font_engine_type;
    typedef agg::font_cache_manager<font_engine_type> font_manager_type;

    font_engine_type             m_feng;
    font_manager_type            m_fman(m_feng);




    // examples <- think this is the best. does not use embedded adapators.
    // agg-svn-r138-agg-2.4/examples/freetype_test.cpp
    // [...write_ppm is skipped...]

    /////////////////////////////////////////////////////
    unsigned char* buffer = new unsigned char[frame_width * frame_height * 3];

    memset(buffer, 255, frame_width * frame_height * 3);

    agg::rendering_buffer rbuf(buffer, frame_width, frame_height, frame_width * 3);

    // typedef agg::pixfmt_rgb565 pixfmt_t;
    typedef agg::pixfmt_rgb24  pixfmt_t;
    pixfmt_t  pixf(rbuf);
    agg::renderer_base<pixfmt_t>   rb(pixf);

    rb.clear(agg::rgba(1,1,1));     // blue.

/*
    // draw a diagonal line on the pixf, using copy_pixel()
    for(unsigned i = 0; i < pixf.height()/2; ++i)
    {
        pixf.copy_pixel(i, i, agg::rgba8(127, 200, 98));
    }
*/

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


    write_c_header();

    // "timesi.ttf"
    if(m_feng.load_font( font_filename, 0, agg::glyph_ren_outline))
    {


        double x = 0.0;
        double y = 3.0;
        // const char* p = text;

        m_feng.hinting(false);
        m_feng.height(40);
        m_feng.width(40);

        m_feng.flip_y( 1 );



        // so we want to iterate all glyps??

        // rather than loop the text we should loop the glpys. but should get this working first
        for(unsigned code = 0; code < glyph_codes.size(); ++code)
        // while(*p)
        {
            const agg::glyph_cache* glyph = m_fman.glyph( code );

            if(glyph /*&& isalpha( code ) */ )
            {
                // if(x > tcurve.total_length1()) break;

                // m_fman.add_kerning(&x, &y);
                m_fman.init_embedded_adaptors(glyph, x, y);

                if(glyph->data_type == agg::glyph_data_outline)
                {
                    // seem to be a lot of glyphs with the same data, eg. not proper glyphs.

                    // std::cout << "// '" << *p << "'  is outline" << std::endl;

                    serialized_path_type  &m_path = m_fman.path_adaptor();
                    // s_path.m_scale = 1; must be set.

                    // https://coconut2015.github.io/agg-tutorial/agg__trans__affine_8h_source.html
                    agg::trans_affine mtx;
                    mtx *= agg::trans_affine_translation(50 , 50);   // this moves from above origin, back into the screen.
                    mtx *= agg::trans_affine_scaling(1.0);          // now scale it


                    agg::conv_transform<serialized_path_type > trans(m_path, mtx);
                    agg::conv_curve<agg::conv_transform< serialized_path_type> > curve(trans);

                    agg::rasterizer_scanline_aa<> ras;
                    agg::scanline_p8 sl;

                    ras.add_path( curve );
                    agg::render_scanlines_aa_solid(ras, sl, rb, agg::rgba(1,0,0));

                    assert( code >= 0 && code < glyph_codes.size());

                    write_glyph_c_data( code, m_path);

                    glyph_codes[ code] = 1;
                    glyph_advance_x[code] = glyph->advance_x;
                    glyph_advance_y[code] = glyph->advance_y;

                    // std::cout << "good" << std::endl;
                    // for debugging
                    //  write_glyph_structure ( m_path );


                }

                // so the glyph advance - will actually depend on the font height and width.
                // eg. for drawing.
                x += glyph->advance_x;

                // newline
                if( x > (frame_width - 100)) {
                  x = 0;
                  y += 40;
                }

                y += glyph->advance_y;
            }
            // ++p;
        }
    }
    else
    {
        std::cout << "could not open font \n" ;
    }


/*
    actually we want a much better structure.
    because we want to be able to work with arbitrary

    we just put it in a structure.
*/
    // this isnt enough we want the advance also.
    // ahhh this is a bit messy
    // and wasteful of memory. but it will do.


    // glyph structure
  // change name. glyph_face
    std::cout << "path_type *glyph[" << glyph_codes.size() << "] = {" << std::endl;
    for(unsigned i = 0; i < glyph_codes.size(); ++i)
    {
      if(glyph_codes[i])
        std::cout << "&glyph_" << i ;
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








    write_ppm(buffer, frame_width, frame_height, "agg_test.ppm");

    delete [] buffer;
    return 0;
    /////////////////////////////////////////////////////


}
















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

