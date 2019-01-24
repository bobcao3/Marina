#ifndef __INCLUDE_MARINA_SERVER_HPP
#define __INCLUDE_MARINA_SERVER_HPP

#define WLR_USE_UNSTABLE

#include <stdio.h>
#include <assert.h>

#include <string>
#include <map>
#include <list>
#include <set>
#include <iterator>

#include <wayland-server.h>

// "Deal" with those headers that almost designed to break C++ support
#define class class_
#define namespace namespace_
#define static

extern "C" {
    #include <wlr/backend.h>
    #include <wlr/backend/headless.h>
    #include <wlr/backend/multi.h>
    #include <wlr/config.h>
    #include <wlr/util/log.h>

    #include <wlr/types/wlr_cursor.h>
    #include <wlr/types/wlr_keyboard.h>
    #include <wlr/types/wlr_xcursor_manager.h>
    #include <wlr/types/wlr_compositor.h>
    #include <wlr/types/wlr_linux_dmabuf_v1.h>
    #include <wlr/types/wlr_export_dmabuf_v1.h>
    #include <wlr/types/wlr_data_device.h>
    #include <wlr/types/wlr_matrix.h>
    #include <wlr/types/wlr_box.h>
    #include <wlr/types/wlr_output_damage.h>
    #include <wlr/types/wlr_output.h>
    #include <wlr/types/wlr_output_layout.h>
    #include <wlr/types/wlr_wl_shell.h>
    #include <wlr/types/wlr_xdg_shell.h>
    #include <wlr/types/wlr_wl_shell.h>
    #include <wlr/types/wlr_xdg_shell_v6.h>
    #include <wlr/types/wlr_screenshooter.h>
    #include <wlr/types/wlr_gamma_control.h>
    #include <wlr/types/wlr_idle.h>
    #include <wlr/types/wlr_idle_inhibit_v1.h>
    #include <wlr/types/wlr_primary_selection.h>
    #include <wlr/types/wlr_gtk_primary_selection.h>
    #include <wlr/types/wlr_server_decoration.h>
}

#undef static
#undef class
#undef namespace

class MarinaCursor;
class MarinaServer;
class MarinaSeat;
class MarinaView;
class MarinaOutput;

#include "marina-view.hpp"
#include "marina-input.hpp"
#include "marina-output.hpp"
#include "marina-seat.hpp"

#define default_seat_name "seat0"

class MarinaServer {
public:
    struct wl_display* wl_display;
    struct wl_event_loop* wl_event_loop;

    struct wlr_backend* backend;
    struct wlr_compositor* compositor;
    struct wlr_export_dmabuf_manager_v1* export_dmabuf_manager_v1;
    struct wlr_xdg_shell* xdg_shell;
    struct wlr_xdg_shell_v6* xdg_v6_shell;
    struct wlr_wl_shell* wl_shell;
    struct wlr_idle* idle;
    struct wlr_idle_inhibit_manager_v1* idle_inhibit;
    struct wlr_linux_dmabuf_v1* linux_dmabuf;
    struct wlr_gtk_primary_selection_device_manager* primary_selection_device_manager;
	
	std::list<MarinaView*> views;
    std::list<MarinaOutput*> outputs;
    
    struct wlr_output_layout* output_layout;

    struct wlr_server_decoration_manager* server_decoration_manager;

    MarinaCursor* cursor;
    std::map<std::string, MarinaSeat*> seats;

    const char* socket;

    // Listeners
    struct wl_listener new_output;
    struct wl_listener new_xdg_surface;
    struct wl_listener new_xdg_v6_surface;
    struct wl_listener new_wl_surface;
    struct wl_listener new_input;

    static void new_input_notify(struct wl_listener* listener, void* data);
    static void new_output_notify(struct wl_listener* listener, void* data);
    static void new_xdg_surface_notify(struct wl_listener* listener, void* data);
    static void new_xdg_v6_surface_notify(struct wl_listener* listener, void* data);
    static void new_wl_surface_notify(struct wl_listener* listener, void* data);

public:
    int run_server();

    MarinaServer ();
    ~MarinaServer ();
};

#endif