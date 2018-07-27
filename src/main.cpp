#include <xcb/xcb.h>

#include <png++/png.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "i3lock_image input_path output_path\n";
        return 1;
    }
    try
    {
        png::image<png::rgb_pixel> image{argv[1]};

        int screen_number;
        xcb_connection_t* const xcb_connection = ::xcb_connect(nullptr, &screen_number);
        if (::xcb_connection_has_error(xcb_connection))
            std::cerr << "failed to establish a connection\n";
        else
        {
            xcb_screen_iterator_t xcb_screen_iter = ::xcb_setup_roots_iterator(::xcb_get_setup(xcb_connection));
            for (; screen_number--; ::xcb_screen_next(&xcb_screen_iter))
                 ;
            xcb_query_tree_reply_t* const xcb_query_tree_reply 
                = ::xcb_query_tree_reply(xcb_connection, ::xcb_query_tree_unchecked(xcb_connection, xcb_screen_iter.data->root), nullptr);
            const xcb_window_t* const xcb_query_tree_children = ::xcb_query_tree_children(xcb_query_tree_reply);
            const int xcb_query_tree_children_length = ::xcb_query_tree_children_length(xcb_query_tree_reply);
            for (int i = 0; i < xcb_query_tree_children_length; ++i)
            {
                xcb_get_geometry_reply_t* const xcb_geometry = 
                    ::xcb_get_geometry_reply(xcb_connection, ::xcb_get_geometry_unchecked(xcb_connection, xcb_query_tree_children[i]), nullptr);
                xcb_get_window_attributes_reply_t* const xcb_window_attributes =
                    ::xcb_get_window_attributes_reply(xcb_connection, ::xcb_get_window_attributes_unchecked(xcb_connection, xcb_query_tree_children[i]), nullptr);
                if (xcb_window_attributes->map_state == XCB_MAP_STATE_VIEWABLE)
                {
                    auto set_pixel = [&image](int x, int y, png::rgb_pixel p) {
                        if (static_cast<unsigned>(x) < image.get_width() &&
                            static_cast<unsigned>(y) < image.get_height()) image[y][x] = p;
                    };
                    auto draw_rect = [&set_pixel](int x, int y, int w, int h, png::rgb_pixel p) {
                        for (int i = 0; i < w * h; ++i)
                            set_pixel(x + (i % w), y + i / w, p);
                    };
                    draw_rect(xcb_geometry->x,     xcb_geometry->y,     xcb_geometry->width, xcb_geometry->height, png::rgb_pixel( 0,  0 , 0));
                    draw_rect(xcb_geometry->x - 5, xcb_geometry->y - 5, xcb_geometry->width, xcb_geometry->height, png::rgb_pixel(40, 40, 40));
                    for (int i = 0; i < xcb_geometry->width; ++i)  set_pixel(xcb_geometry->x + i - 5, xcb_geometry->y - 5,     png::rgb_pixel(80, 80, 80));
                    for (int i = 0; i < xcb_geometry->height; ++i) set_pixel(xcb_geometry->x     - 5, xcb_geometry->y - 5 + i, png::rgb_pixel(80, 80, 80));
                }
                ::free(xcb_window_attributes);
                ::free(xcb_geometry);
            }
            ::free(xcb_query_tree_reply);
        }
        ::xcb_disconnect(xcb_connection);

        image.write(argv[2]);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return 0;
}
