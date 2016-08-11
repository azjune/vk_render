//
// Created by ld016 on 16-8-11.
//

#include <xcb/xcb.h>
#include "v_window.h"
#include "v_renderer.h"
#include <assert.h>
#include <iostream>

#if VK_USE_PLATFORM_XCB_KHR

void v_window::_initOSWindow() {
    int screenNumber = 0;
    _xcb_connection = xcb_connect(nullptr,&screenNumber);
    const xcb_setup_t* setup = xcb_get_setup(_xcb_connection);
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = it.data;

    _xcb_window = xcb_generate_id(_xcb_connection);

    uint32_t value_mask, value_list[ 32 ];

    int16_t x=0,y=0;
    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_RELEASE;

    xcb_create_window(_xcb_connection,
                      XCB_COPY_FROM_PARENT,
                      _xcb_window,
                      screen->root,
                      x,y,
                      _width,_height,
                      1,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      value_mask,
                      value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom( _xcb_connection, 1, 12, "WM_PROTOCOLS" );
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply( _xcb_connection, cookie, 0 );

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom( _xcb_connection, 0, 16, "WM_DELETE_WINDOW" );
    _xcb_atom_window_reply = xcb_intern_atom_reply( _xcb_connection, cookie2, 0 );

    xcb_change_property( _xcb_connection, XCB_PROP_MODE_REPLACE, _xcb_window,
                         ( *reply ).atom, 4, 32, 1,
                         &( *_xcb_atom_window_reply ).atom );
    free( reply );

    xcb_map_window(_xcb_connection,_xcb_window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive
    const uint32_t coords[] = { 100, 100 };
    xcb_configure_window( _xcb_connection, _xcb_window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords );
    xcb_flush(_xcb_connection);
}

void v_window::_deInitOSWindow() {
    xcb_destroy_window(_xcb_connection,_xcb_window);
    xcb_disconnect(_xcb_connection);

    _xcb_connection = nullptr;
    _xcb_window = 0;
}

void v_window::_updateOSWindow() {
    auto event = xcb_poll_for_event(_xcb_connection);
    if(!event){
        return;
    }
    switch(event->response_type & ~0x80){
        case XCB_CLIENT_MESSAGE:
            if( ( (xcb_client_message_event_t*)event )->data.data32[ 0 ] == _xcb_atom_window_reply->atom ) {
                close();
            }
            break;
        default:
            break;
    }

    free(event);
}

void v_window::_initOSSurface() {
    VkXcbSurfaceCreateInfoKHR createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        createInfo.connection = _xcb_connection;
        createInfo.window = _xcb_window;
    }
    vkCreateXcbSurfaceKHR(_renderer->getInstance(),&createInfo, nullptr,&_surface);
    assert(_surface && "create surface faild");
}

#endif