/*
    load and render font text to ppm file.

    ./build8.sh
    rm agg_test.ppm  ; ./a.out ;  feh agg_test.ppm
*/

#include <iostream>

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

const char *full_file_name(const char *s)
{
  return s;
}

void message( const char *s)
{
  std::cout << s;
}

// h

agg::int8u  x[ 252] = { 222, 5, 30, 242, 33, 3, 154, 251, 232, 4, 239, 248, 230, 5, 15, 248, 230, 6, 45, 247, 206, 7, 45, 247, 70, 8, 45, 247, 150, 8, 127, 247, 230, 8, 209, 247, 230, 8, 91, 248, 230, 8, 255, 248, 160, 8, 235, 249, 111, 7, 2, 254, 54, 7, 193, 254, 54, 7, 213, 254, 54, 7, 247, 254, 74, 7, 15, 255, 96, 7, 37, 255, 124, 7, 37, 255, 156, 7, 37, 255, 198, 7, 3, 255, 84, 8, 149, 254, 224, 8, 211, 253, 47, 9, 2, 254, 218, 8, 135, 254, 66, 8, 39, 255, 170, 7, 197, 255, 62, 7, 1, 0, 210, 6, 61, 0, 124, 6, 61, 0, 32, 6, 61, 0, 228, 5, 1, 0, 170, 5, 201, 255, 170, 5, 109, 255, 170, 5, 255, 254, 2, 6, 211, 253, 35, 7, 234, 249, 88, 7, 47, 249, 88, 7, 195, 248, 88, 7, 145, 248, 56, 7, 115, 248, 26, 7, 85, 248, 232, 6, 85, 248, 160, 6, 85, 248, 66, 6, 141, 248, 152, 5, 245, 248, 186, 4, 19, 250, 126, 4, 97, 250, 42, 3, 131, 252, 194, 2, 51, 253, 126, 2, 3, 254, 223, 1, 0, 0, 103, 0, 0, 0, 159, 3, 214, 244, 245, 3, 160, 243, 244, 3, 95, 243, 190, 3, 45, 243, 138, 3, 251, 242, 60, 3, 251, 242, 14, 3, 251, 242, 178, 2, 11, 243, 127, 2, 18, 243, 127, 2, 176, 242 };


// OK. it needs m_scale to be set. and it's not exposed in the constructor.
//         void init(const int8u* data, unsigned size, double dx, double dy, double scale=1.0)

                     // short int. is not enough....
 typedef  agg::serialized_integer_path_adaptor<short int, 6> serialized_path_type;


serialized_path_type   s_path(x, 252, 0.0, 0.0, 1.0) ;
// s_path.m_scale = 1;


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


void write_glyph_c_data( serialized_path_type  &x )
{

  std::cout << "agg::int8u  x[ " << ( x.m_end - x.m_data ) << "] = { ";

  for(const agg::int8u *i = x.m_data; i < x.m_end; ++i ) {

    std::cout << unsigned(*i);

    if( i + 1 < x.m_end )
      std::cout <<  ", ";

  }
  std::cout << " };" << std::endl;

}






// So we want to index... and test the load
// we should print out everything... because it's easy to comment it later.

int main()
{

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
    rb.clear(agg::rgba(0,0,1));     // blue.

    for(unsigned i = 0; i < pixf.height()/2; ++i)
    {
        pixf.copy_pixel(i, i, agg::rgba8(127, 200, 98));
    }




    const char *text = "hello12345";
    // const char *text = "abcdefghijk";



    // "timesi.ttf"
    if(m_feng.load_font(full_file_name("arial.ttf"), 0, agg::glyph_ren_outline))
    {


        double x = 0.0;
        double y = 3.0;
        const char* p = text;

        m_feng.hinting(false);
        m_feng.height(40);
        m_feng.width(40);

        m_feng.flip_y( 1 );


        // rather than loop the text we should loop the glpys. but should get this working first
        while(*p)
        {
            const agg::glyph_cache* glyph = m_fman.glyph(*p);
            if(glyph)
            {
                // if(x > tcurve.total_length1()) break;

                // m_fman.add_kerning(&x, &y);
                m_fman.init_embedded_adaptors(glyph, x, y);

                if(glyph->data_type == agg::glyph_data_outline)
                {
                    std::cout << "'" << *p << "'  is outline" << std::endl;

                    serialized_path_type  &m_path = m_fman.path_adaptor();
                    // s_path.m_scale = 1; must be set.

                    // https://coconut2015.github.io/agg-tutorial/agg__trans__affine_8h_source.html
                    agg::trans_affine mtx;
                    mtx *= agg::trans_affine_translation(50, 50);   // this moves from above origin, back into the screen.
                    mtx *= agg::trans_affine_scaling(3.0);          // now scale it


                    agg::conv_transform<serialized_path_type > trans(m_path, mtx);
                    agg::conv_curve<agg::conv_transform< serialized_path_type> > curve(trans);

                    agg::rasterizer_scanline_aa<> ras;
                    agg::scanline_p8 sl;

                    ras.add_path( curve );
                    agg::render_scanlines_aa_solid(ras, sl, rb, agg::rgba(1,0,0));


                    write_glyph_c_data( m_path);

                    // std::cout << "good" << std::endl;
                    // write_glyph_structure ( m_path );


                }

                // so the glyph advance - will actually depend on the font height and width.
                // eg. for drawing.
                x += glyph->advance_x;
                y += glyph->advance_y;
            }
            ++p;
        }
    }
    else
    {
        message("could not open font \n" );
    }





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

