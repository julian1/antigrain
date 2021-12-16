/*
   nix-shell -p gcc10
  
   g++ main.cpp -I agg-svn-r138-agg-2.4/include/
    
  basic renderers
    agg.sourceforge.net/antigrain.com/doc/basic_renderers/basic_renderers.agdoc.html

    
  agg_pixfmt_rgb565.h, 16 bits per pixel, 5 bits for Red, 6 bits for Green, and 5 bits for Blue
*/


#include <stdio.h>
#include <string.h>
// #include "agg_pixfmt_rgb24.h"
#include "agg_pixfmt_rgb.h"


#include "agg_basics.h"
// #include "agg_rendering_buffer.h"

#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"

#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"


enum
{
    frame_width = 320,
    frame_height = 200
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


// [...write_ppm is skipped...]

int main()
{
    //--------------------
    // Allocate the buffer.
    // Clear the buffer, for now "manually"
    // Create the rendering buffer object
    // Create the Pixel Format renderer
    // Create one line (span) of type rgba8.
    // Fill the buffer using blend_color_span
    // Write the buffer to agg_test.ppm
    // Free memory

    unsigned char* buffer = new unsigned char[frame_width * frame_height * 3];

    memset(buffer, 255, frame_width * frame_height * 3);

    agg::rendering_buffer rbuf(buffer, 
                               frame_width, 
                               frame_height, 
                               frame_width * 3);

    agg::pixfmt_rgb24 pixf(rbuf);



  
    // typedef  renderer_base;

    //typedef agg::pixfmt_rgb565 pixfmt;
    // pixfmt pixf(rbuf);

    agg::renderer_base<agg::pixfmt_rgb24>   rb(pixf);


#if 0
    agg::rgba8 span[frame_width];

    unsigned i;
    for(i = 0; i < frame_width; ++i)
    {
        agg::rgba c(380.0 + 400.0 * i / frame_width, 0.8);
        span[i] = agg::rgba8(c);
    }


    for(i = 0; i < frame_height; ++i)
    {
        pixf.blend_color_hspan(0, i, frame_width, span, 0, 0);
    }
#endif


      unsigned i;
    for(i = 0; i < pixf.height()/2; ++i)
    {
        pixf.copy_pixel(i, i, agg::rgba8(127, 200, 98));
    }


    write_ppm(buffer, frame_width, frame_height, "agg_test.ppm");

    delete [] buffer;
    return 0;
}

