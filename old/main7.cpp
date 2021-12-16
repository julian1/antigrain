
/*
  see build7.sh

  - ok. we want a serialize function - to write the geometry, as antigrain primitives move_to etc ....

  see,
  - scanline_storage_aa   <- think this is a scanline serialize. not geometry serializer.

  - important. if agg array is constant size. then might be easier to serialize as raw data.
  - agg_array has bounding_rect agg-svn-r138-agg-2.4/include/agg_path_storage_integer.h
*/

#include <iostream>

#include "agg_font_freetype.h"
// #include "agg_font_cache_manager.h"
// #include "agg_font_cache_manager2.h"


#include "agg_conv_curve.h"
#include "agg_conv_contour.h"


// #include "agg_path_storage_integer.h"


const char *full_file_name(const char *s)
{
  return s;
}

void message( const char *s)
{
  std::cout << s;
}



agg::int8u  x[ 252] = { 222, 5, 228, 13, 33, 3, 104, 4, 232, 4, 21, 7, 230, 5, 245, 7, 230, 6, 215, 8, 206, 7, 215, 8, 70, 8, 215, 8, 150, 8, 133, 8, 230, 8, 51, 8, 230, 8, 169, 7, 230, 8, 5, 7, 160, 8, 25, 6, 111, 7, 0, 2, 54, 7, 67, 1, 54, 7, 47, 1, 54, 7, 13, 1, 74, 7, 245, 0, 96, 7, 223, 0, 124, 7, 223, 0, 156, 7, 223, 0, 198, 7, 1, 1, 84, 8, 111, 1, 224, 8, 49, 2, 47, 9, 0, 2, 218, 8, 125, 1, 66, 8, 221, 0, 170, 7, 63, 0, 62, 7, 1, 0, 210, 6, 199, 255, 124, 6, 199, 255, 32, 6, 199, 255, 228, 5, 1, 0, 170, 5, 59, 0, 170, 5, 151, 0, 170, 5, 5, 1, 2, 6, 49, 2, 35, 7, 24, 6, 88, 7, 213, 6, 88, 7, 65, 7, 88, 7, 115, 7, 56, 7, 145, 7, 26, 7, 175, 7, 232, 6, 175, 7, 160, 6, 175, 7, 66, 6, 119, 7, 152, 5, 15, 7, 186, 4, 241, 5, 126, 4, 163, 5, 42, 3, 129, 3, 194, 2, 209, 2, 126, 2, 1, 2, 223, 1, 0, 0, 103, 0, 0, 0, 159, 3, 44, 11, 245, 3, 98, 12, 244, 3, 165, 12, 190, 3, 215, 12, 138, 3, 9, 13, 60, 3, 9, 13, 14, 3, 9, 13, 178, 2, 249, 12, 127, 2, 240, 12, 127, 2, 82, 13 };


agg::serialized_integer_path_adaptor<short int, 6>  path(x, 252, 0, 0) ;




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


    // Pipeline to process the vectors glyph paths (curves + contour)
    agg::conv_curve<font_manager_type::path_adaptor_type>                     m_curves(m_fman.path_adaptor());
    agg::conv_contour<agg::conv_curve<font_manager_type::path_adaptor_type> > m_contour(m_curves);



    const char *text = "hello";

    if(m_feng.load_font(full_file_name("timesi.ttf"), 0, agg::glyph_ren_outline))
    {


        double x = 0.0;
        double y = 3.0;
        const char* p = text;

        m_feng.hinting(false);
        m_feng.height(40);

        // rather than loop the text we should loop the glpys. but should get this working first
        while(*p)
        {
            const agg::glyph_cache* glyph = m_fman.glyph(*p);
            if(glyph)
            {
                // if(x > tcurve.total_length1()) break;

                m_fman.add_kerning(&x, &y);
                m_fman.init_embedded_adaptors(glyph, x, y);

                if(glyph->data_type == agg::glyph_data_outline)
                {
                    std::cout << "'" << *p << "'  is outline" << std::endl;



                  // m_curves.serialize();
                  // m_curves.bounding_rect() ;

                  // agg::path_storage &x = m_fman.path_adaptor();
                  // m_fman.path_adaptor(). serialize() ;
                    // x.bounding_rect();

                    agg::serialized_integer_path_adaptor<short int, 6> &x = m_fman.path_adaptor();


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

                    // So.  we should be able to write this as a char string... and reload it.
                    // use hex or octal encoding.
  
                    // but should test that we can genuinely render the text.
                    // 


                    // path should be m_curves
                  // should have a length function somewhere...

                  /*
                        ras.add_path(m_contour);
                        ren_solid.color(agg::srgba8(0, 0, 0));
                        agg::render_scanlines(ras, sl, ren_solid);
                  */

                  // ras.add_path(m_curves);

                }

                // increment pen position
                // eg. for drawing.
                x += glyph->advance_x;
                y += glyph->advance_y;
            }
            ++p;
        }
    }
    else
    {
        message("Please copy file timesi.ttf to the current directory\n"
                "or unzip it from ../art/timesi.zip");
    }



}

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

