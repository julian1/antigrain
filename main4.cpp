/*

  pixfmt_rgb24 defined like this

    typedef pixfmt_alpha_blend_rgb<blender_rgb24, rendering_buffer, 3> pixfmt_rgb24;
    in agg_pixfmt_rgb.h

*/


#include <stdio.h>
#include <string.h>
// #include "agg_pixfmt_rgb24.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgb_packed.h"  // rgb565


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
    unsigned char* buffer = new unsigned char[frame_width * frame_height * 3];

    memset(buffer, 255, frame_width * frame_height * 3);

    agg::rendering_buffer rbuf(buffer, 
                               frame_width, 
                               frame_height, 
                               frame_width * 3);





    // typedef pixfmt_alpha_blend_rgb<blender_rgb24, rendering_buffer, 3> pixfmt_rgb24;
    typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb24, agg::rendering_buffer, 3> pixfmt_t;

    // typedef agg::pixfmt_rgb565 pixfmt_t;
    // typedef agg::pixfmt_rgb24  pixfmt_t;

    pixfmt_t  pixf(rbuf);


     agg::renderer_base<pixfmt_t>   rb(pixf);

     rb.clear(agg::rgba(1,0,0));     // red.


      unsigned i;
    for(i = 0; i < pixf.height()/2; ++i)
    {
        pixf.copy_pixel(i, i, agg::rgba8(127, 200, 98));
    }


    write_ppm(buffer, frame_width, frame_height, "agg_test.ppm");

    delete [] buffer;
    return 0;
}

