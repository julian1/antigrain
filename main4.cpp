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


using namespace agg;





    //==================================================pixfmt_alpha_blend_rgb
    template<class Blender, class RenBuf, unsigned Step, unsigned Offset = 0> 
    class pixfmt_alpha_blend_rgb
    {
    public:
        typedef pixfmt_rgb_tag pixfmt_category;
        typedef RenBuf   rbuf_type;
        typedef Blender  blender_type;
        typedef typename rbuf_type::row_data row_data;
        typedef typename blender_type::color_type color_type;
        typedef typename blender_type::order_type order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;
        enum 
        {
            num_components = 3,
            pix_step = Step,
            pix_offset = Offset,
            pix_width = sizeof(value_type) * pix_step
        };
        struct pixel_type
        {
            value_type c[num_components];

            void set(value_type r, value_type g, value_type b)
            {
                c[order_type::R] = r;
                c[order_type::G] = g;
                c[order_type::B] = b;
            }

            void set(const color_type& color)
            {
                set(color.r, color.g, color.b);
            }

            void get(value_type& r, value_type& g, value_type& b) const
            {
                r = c[order_type::R];
                g = c[order_type::G];
                b = c[order_type::B];
            }

            color_type get() const
            {
                return color_type(
                    c[order_type::R], 
                    c[order_type::G], 
                    c[order_type::B]);
            }

            pixel_type* next()
            {
                return (pixel_type*)(c + pix_step);
            }

            const pixel_type* next() const
            {
                return (const pixel_type*)(c + pix_step);
            }

            pixel_type* advance(int n)
            {
                return (pixel_type*)(c + n * pix_step);
            }

            const pixel_type* advance(int n) const
            {
                return (const pixel_type*)(c + n * pix_step);
            }
        };

    private:
        //--------------------------------------------------------------------
        AGG_INLINE void blend_pix(pixel_type* p, 
            value_type r, value_type g, value_type b, value_type a, 
            unsigned cover)
        {
            m_blender.blend_pix(p->c, r, g, b, a, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pix(pixel_type* p, 
            value_type r, value_type g, value_type b, value_type a)
        {
            m_blender.blend_pix(p->c, r, g, b, a);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pix(pixel_type* p, const color_type& c, unsigned cover)
        {
            m_blender.blend_pix(p->c, c.r, c.g, c.b, c.a, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pix(pixel_type* p, const color_type& c)
        {
            m_blender.blend_pix(p->c, c.r, c.g, c.b, c.a);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_or_blend_pix(pixel_type* p, const color_type& c, unsigned cover)
        {
            if (!c.is_transparent())
            {
                if (c.is_opaque() && cover == cover_mask)
                {
                    p->set(c);
                }
                else
                {
                    blend_pix(p, c, cover);
                }
            }
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_or_blend_pix(pixel_type* p, const color_type& c)
        {
            if (!c.is_transparent())
            {
                if (c.is_opaque())
                {
                    p->set(c);
                }
                else
                {
                    blend_pix(p, c);
                }
            }
        }

    public:
        //--------------------------------------------------------------------
        explicit pixfmt_alpha_blend_rgb(rbuf_type& rb) :
            m_rbuf(&rb)
        {}
        void attach(rbuf_type& rb) { m_rbuf = &rb; }

        //--------------------------------------------------------------------
        template<class PixFmt>
        bool attach(PixFmt& pixf, int x1, int y1, int x2, int y2)
        {
            rect_i r(x1, y1, x2, y2);
            if (r.clip(rect_i(0, 0, pixf.width()-1, pixf.height()-1)))
            {
                int stride = pixf.stride();
                m_rbuf->attach(pixf.pix_ptr(r.x1, stride < 0 ? r.y2 : r.y1), 
                               (r.x2 - r.x1) + 1,
                               (r.y2 - r.y1) + 1,
                               stride);
                return true;
            }
            return false;
        }

#if 0

        //--------------------------------------------------------------------
        Blender& blender() { return m_blender; }

        //--------------------------------------------------------------------
        AGG_INLINE unsigned width()  const { return m_rbuf->width();  }
        AGG_INLINE unsigned height() const { return m_rbuf->height(); }
        AGG_INLINE int      stride() const { return m_rbuf->stride(); }

        //--------------------------------------------------------------------
        AGG_INLINE       int8u* row_ptr(int y)       { return m_rbuf->row_ptr(y); }
        AGG_INLINE const int8u* row_ptr(int y) const { return m_rbuf->row_ptr(y); }
        AGG_INLINE row_data     row(int y)     const { return m_rbuf->row(y); }

        //--------------------------------------------------------------------
        AGG_INLINE int8u* pix_ptr(int x, int y) 
        { 
            return m_rbuf->row_ptr(y) + sizeof(value_type) * (x * pix_step + pix_offset);
        }

        AGG_INLINE const int8u* pix_ptr(int x, int y) const 
        { 
            return m_rbuf->row_ptr(y) + sizeof(value_type) * (x * pix_step + pix_offset);
        }

        // Return pointer to pixel value, forcing row to be allocated.
        AGG_INLINE pixel_type* pix_value_ptr(int x, int y, unsigned len) 
        {
            return (pixel_type*)(m_rbuf->row_ptr(x, y, len) + sizeof(value_type) * (x * pix_step + pix_offset));
        }

        // Return pointer to pixel value, or null if row not allocated.
        AGG_INLINE const pixel_type* pix_value_ptr(int x, int y) const 
        {
            int8u* p = m_rbuf->row_ptr(y);
            return p ? (pixel_type*)(p + sizeof(value_type) * (x * pix_step + pix_offset)) : 0;
        }

        // Get pixel pointer from raw buffer pointer.
        AGG_INLINE static pixel_type* pix_value_ptr(void* p) 
        {
            return (pixel_type*)((value_type*)p + pix_offset);
        }

        // Get pixel pointer from raw buffer pointer.
        AGG_INLINE static const pixel_type* pix_value_ptr(const void* p) 
        {
            return (const pixel_type*)((const value_type*)p + pix_offset);
        }

        //--------------------------------------------------------------------
        AGG_INLINE static void write_plain_color(void* p, color_type c)
        {
            // RGB formats are implicitly premultiplied.
            c.premultiply();
            pix_value_ptr(p)->set(c);
        }

        //--------------------------------------------------------------------
        AGG_INLINE static color_type read_plain_color(const void* p)
        {
            return pix_value_ptr(p)->get();
        }

        //--------------------------------------------------------------------
        AGG_INLINE static void make_pix(int8u* p, const color_type& c)
        {
            ((pixel_type*)p)->set(c);
        }

        //--------------------------------------------------------------------
        AGG_INLINE color_type pixel(int x, int y) const
        {
            if (const pixel_type* p = pix_value_ptr(x, y))
            {
                return p->get();
            }
            return color_type::no_color();
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_pixel(int x, int y, const color_type& c)
        {
            pix_value_ptr(x, y, 1)->set(c);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pixel(int x, int y, const color_type& c, int8u cover)
        {
            copy_or_blend_pix(pix_value_ptr(x, y, 1), c, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_hline(int x, int y, 
                                   unsigned len, 
                                   const color_type& c)
        {
            pixel_type* p = pix_value_ptr(x, y, len);
            do
            {
                p->set(c);
                p = p->next();
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        AGG_INLINE void copy_vline(int x, int y,
                                   unsigned len, 
                                   const color_type& c)
        {
            do
            {
                pix_value_ptr(x, y++, 1)->set(c);
            }
            while (--len);
        }

        //--------------------------------------------------------------------
        void blend_hline(int x, int y,
                         unsigned len, 
                         const color_type& c,
                         int8u cover)
        {
            if (!c.is_transparent())
            {
                pixel_type* p = pix_value_ptr(x, y, len);

                if (c.is_opaque() && cover == cover_mask)
                {
                    do
                    {
                        p->set(c);
                        p = p->next();
                    }
                    while (--len);
                }
                else
                {
                    do
                    {
                        blend_pix(p, c, cover);
                        p = p->next();
                    }
                    while (--len);
                }
            }
        }


        //--------------------------------------------------------------------
        void blend_vline(int x, int y,
                         unsigned len, 
                         const color_type& c,
                         int8u cover)
        {
            if (!c.is_transparent())
            {
                if (c.is_opaque() && cover == cover_mask)
                {
                    do
                    {
                        pix_value_ptr(x, y++, 1)->set(c);
                    }
                    while (--len);
                }
                else
                {
                    do
                    {
                        blend_pix(pix_value_ptr(x, y++, 1), c, cover);
                    }
                    while (--len);
                }
            }
        }

        //--------------------------------------------------------------------
        void blend_solid_hspan(int x, int y,
                               unsigned len, 
                               const color_type& c,
                               const int8u* covers)
        {
            if (!c.is_transparent())
            {
                pixel_type* p = pix_value_ptr(x, y, len);

                do 
                {
                    if (c.is_opaque() && *covers == cover_mask)
                    {
                        p->set(c);
                    }
                    else
                    {
                        blend_pix(p, c, *covers);
                    }
                    p = p->next();
                    ++covers;
                }
                while (--len);
            }
        }


        //--------------------------------------------------------------------
        void blend_solid_vspan(int x, int y,
                               unsigned len, 
                               const color_type& c,
                               const int8u* covers)
        {
            if (!c.is_transparent())
            {
                do 
                {
                    pixel_type* p = pix_value_ptr(x, y++, 1);

                    if (c.is_opaque() && *covers == cover_mask)
                    {
                        p->set(c);
                    }
                    else
                    {
                        blend_pix(p, c, *covers);
                    }
                    ++covers;
                }
                while (--len);
            }
        }

        //--------------------------------------------------------------------
        void copy_color_hspan(int x, int y,
                              unsigned len, 
                              const color_type* colors)
        {
            pixel_type* p = pix_value_ptr(x, y, len);

            do 
            {
                p->set(*colors++);
                p = p->next();
            }
            while (--len);
        }


        //--------------------------------------------------------------------
        void copy_color_vspan(int x, int y,
                              unsigned len, 
                              const color_type* colors)
        {
            do 
            {
                pix_value_ptr(x, y++, 1)->set(*colors++);
            }
            while (--len);
        }

        //--------------------------------------------------------------------
        void blend_color_hspan(int x, int y,
                               unsigned len, 
                               const color_type* colors,
                               const int8u* covers,
                               int8u cover)
        {
            pixel_type* p = pix_value_ptr(x, y, len);

            if (covers)
            {
                do 
                {
                    copy_or_blend_pix(p, *colors++, *covers++);
                    p = p->next();
                }
                while (--len);
            }
            else
            {
                if (cover == cover_mask)
                {
                    do 
                    {
                        copy_or_blend_pix(p, *colors++);
                        p = p->next();
                    }
                    while (--len);
                }
                else
                {
                    do 
                    {
                        copy_or_blend_pix(p, *colors++, cover);
                        p = p->next();
                    }
                    while (--len);
                }
            }
        }

        //--------------------------------------------------------------------
        void blend_color_vspan(int x, int y,
                               unsigned len, 
                               const color_type* colors,
                               const int8u* covers,
                               int8u cover)
        {
            if (covers)
            {
                do 
                {
                    copy_or_blend_pix(pix_value_ptr(x, y++, 1), *colors++, *covers++);
                }
                while (--len);
            }
            else
            {
                if (cover == cover_mask)
                {
                    do 
                    {
                        copy_or_blend_pix(pix_value_ptr(x, y++, 1), *colors++);
                    }
                    while (--len);
                }
                else
                {
                    do 
                    {
                        copy_or_blend_pix(pix_value_ptr(x, y++, 1), *colors++, cover);
                    }
                    while (--len);
                }
            }
        }

        //--------------------------------------------------------------------
        template<class Function> void for_each_pixel(Function f)
        {
            for (unsigned y = 0; y < height(); ++y)
            {
                row_data r = m_rbuf->row(y);
                if (r.ptr)
                {
                    unsigned len = r.x2 - r.x1 + 1;
                    pixel_type* p = pix_value_ptr(r.x1, y, len);
                    do
                    {
                        f(p->c);
                        p = p->next();
                    }
                    while (--len);
                }
            }
        }

        //--------------------------------------------------------------------
        template<class GammaLut> void apply_gamma_dir(const GammaLut& g)
        {
            for_each_pixel(apply_gamma_dir_rgb<color_type, order_type, GammaLut>(g));
        }

        //--------------------------------------------------------------------
        template<class GammaLut> void apply_gamma_inv(const GammaLut& g)
        {
            for_each_pixel(apply_gamma_inv_rgb<color_type, order_type, GammaLut>(g));
        }

        //--------------------------------------------------------------------
        template<class RenBuf2>
        void copy_from(const RenBuf2& from, 
                       int xdst, int ydst,
                       int xsrc, int ysrc,
                       unsigned len)
        {
            if (const int8u* p = from.row_ptr(ysrc))
            {
                std::memmove(m_rbuf->row_ptr(xdst, ydst, len) + xdst * pix_width, 
                        p + xsrc * pix_width, 
                        len * pix_width);
            }
        }

        //--------------------------------------------------------------------
        // Blend from an RGBA surface.
        template<class SrcPixelFormatRenderer>
        void blend_from(const SrcPixelFormatRenderer& from, 
                        int xdst, int ydst,
                        int xsrc, int ysrc,
                        unsigned len,
                        int8u cover)
        {
            typedef typename SrcPixelFormatRenderer::pixel_type src_pixel_type;
            typedef typename SrcPixelFormatRenderer::order_type src_order;

            if (const src_pixel_type* psrc = from.pix_value_ptr(xsrc, ysrc))
            {
                pixel_type* pdst = pix_value_ptr(xdst, ydst, len);

                if (cover == cover_mask)
                {
                    do 
                    {
                        value_type alpha = psrc->c[src_order::A];
                        if (alpha <= color_type::empty_value())
                        {
                            if (alpha >= color_type::full_value())
                            {
                                pdst->c[order_type::R] = psrc->c[src_order::R];
                                pdst->c[order_type::G] = psrc->c[src_order::G];
                                pdst->c[order_type::B] = psrc->c[src_order::B];
                            }
                            else
                            {
                                blend_pix(pdst, 
                                    psrc->c[src_order::R],
                                    psrc->c[src_order::G],
                                    psrc->c[src_order::B],
                                    alpha);
                            }
                        }
                        psrc = psrc->next();
                        pdst = pdst->next();
                    }
                    while(--len);
                }
                else
                {
                    do 
                    {
                        copy_or_blend_pix(pdst, psrc->get(), cover);
                        psrc = psrc->next();
                        pdst = pdst->next();
                    }
                    while (--len);
                }
            }
        }

        //--------------------------------------------------------------------
        // Blend from single color, using grayscale surface as alpha channel.
        template<class SrcPixelFormatRenderer>
        void blend_from_color(const SrcPixelFormatRenderer& from, 
                              const color_type& color,
                              int xdst, int ydst,
                              int xsrc, int ysrc,
                              unsigned len,
                              int8u cover)
        {
            typedef typename SrcPixelFormatRenderer::pixel_type src_pixel_type;
            typedef typename SrcPixelFormatRenderer::color_type src_color_type;

            if (const src_pixel_type* psrc = from.pix_value_ptr(xsrc, ysrc))
            {
                pixel_type* pdst = pix_value_ptr(xdst, ydst, len);

                do 
                {
                    copy_or_blend_pix(pdst, color, src_color_type::scale_cover(cover, psrc->c[0]));
                    psrc = psrc->next();
                    pdst = pdst->next();
                }
                while (--len);
            }
        }

        //--------------------------------------------------------------------
        // Blend from color table, using grayscale surface as indexes into table. 
        // Obviously, this only works for integer value types.
        template<class SrcPixelFormatRenderer>
        void blend_from_lut(const SrcPixelFormatRenderer& from, 
                            const color_type* color_lut,
                            int xdst, int ydst,
                            int xsrc, int ysrc,
                            unsigned len,
                            int8u cover)
        {
            typedef typename SrcPixelFormatRenderer::pixel_type src_pixel_type;

            if (const src_pixel_type* psrc = from.pix_value_ptr(xsrc, ysrc))
            {
                pixel_type* pdst = pix_value_ptr(xdst, ydst, len);

                if (cover == cover_mask)
                {
                    do 
                    {
                        const color_type& color = color_lut[psrc->c[0]];
                        blend_pix(pdst, color);
                        psrc = psrc->next();
                        pdst = pdst->next();
                    }
                    while(--len);
                }
                else
                {
                    do 
                    {
                        copy_or_blend_pix(pdst, color_lut[psrc->c[0]], cover);
                        psrc = psrc->next();
                        pdst = pdst->next();
                    }
                    while(--len);
                }
            }
        }

  #endif
    private:
        rbuf_type* m_rbuf;
        Blender    m_blender;
    };
    
















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
    typedef ::pixfmt_alpha_blend_rgb<agg::blender_rgb24, agg::rendering_buffer, 3> pixfmt_t;

    // typedef agg::pixfmt_rgb565 pixfmt_t;
    // typedef agg::pixfmt_rgb24  pixfmt_t;

    pixfmt_t  pixf(rbuf);


     agg::renderer_base<pixfmt_t>   rb(pixf);

     rb.clear(agg::rgba(1,0,0));     // red.



    write_ppm(buffer, frame_width, frame_height, "agg_test.ppm");

    delete [] buffer;
    return 0;
}

