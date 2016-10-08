/*
 * Daemon that translate betweens shmif and Wayland
 * 1. get basic shm- surface to work
 * 2. get input event to work
 * 3. get the normal details (move, resize, ...)
 * 4. get accelerated handle bridging to work
 * 5. multiple clients
 * 6.
 */
#define WANT_ARCAN_SHMIF_HELPER
#include <arcan_shmif.h>
#include <wayland-server.h>
#include "xdg-shell-v6.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

static inline void trace(const char* msg, ...)
{
va_list args;
	va_start( args, msg );
		vfprintf(stderr,  msg, args );
		fprintf(stderr, "\n");
	va_end( args);
	fflush(stderr);
}

struct bridge_client {
	struct arcan_shmif_cont acon;
	struct wl_client* client;
	struct wl_resource* keyboard;
	struct wl_resource* pointer;
	struct wl_list link;
};

struct bridge_surface {
	struct wl_resource* res;
	struct wl_resource* buf;
	struct wl_resource* frame_cb;
	int x, y;
	struct bridge_client* cl;
	struct wl_list link;
};

static struct {
	struct wl_list cl, surf;
	struct wl_display* disp;
} wl;

/*
 * xkbdkeyboard
 */

/*
 * EGL- details needed for handle translation
 */
typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) (EGLenum, EGLImage);
static PFNEGLQUERYWAYLANDBUFFERWL query_buffer;
static PFNEGLBINDWAYLANDDISPLAYWL bind_display;
static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC img_tgt_text;

/*
 * Welcome to callback hell
 */
#include "surf.c"
static struct wl_surface_interface surf_if = {
	.destroy = surf_destroy,
	.attach = surf_attach,
	.damage = surf_damage,
	.frame = surf_frame,
	.set_opaque_region = surf_opaque,
	.set_input_region = surf_inputreg,
	.commit = surf_commit,
	.set_buffer_transform = surf_transform,
	.set_buffer_scale = surf_scale
};

/*
 * missing interfaces:
 *  - wl_region_interface,
 *  - wl_pointer_interface,
 *  - wl_keyboard_interface
 */

static struct bridge_client* find_client(struct wl_client* cl)
{
	struct bridge_client* res;
	wl_list_for_each(res, &wl.cl, link){
		if (res->client == cl)
			return res;
	}
	res = malloc(sizeof(struct bridge_client));
	memset(res, '\0', sizeof(struct bridge_client));
	wl_list_insert(&wl.cl, &res->link);

	return res;
}

static void comp_surf_delete(struct wl_resource* res)
{
	trace("destroy compositor surface\n");
	struct bridge_surface* surf = wl_resource_get_user_data(res);
	wl_list_remove(&surf->link);
	free(surf);
/* BRIDGEME: maybe track cursor/active here */
/* BRIDGEME: drop subsurface unless primary, and if primary, hint-hide */
}

static void comp_surf_create(struct wl_client *client,
	struct wl_resource *resource, uint32_t id)
{
	trace("create compositor surface(%"PRIu32")", id);
/* we need to defer this and make a subsegment connection unless
 * the client has not consumed its primary one */
	struct bridge_surface* new_surf = malloc(sizeof(struct bridge_surface));
	memset(new_surf, '\0', sizeof(struct bridge_surface));
	new_surf->res = wl_resource_create(client, &wl_surface_interface,
		wl_resource_get_version(resource), id);

	wl_resource_set_implementation(new_surf->res,
		&surf_if, new_surf, comp_surf_delete);

	new_surf->cl = find_client(client);
	wl_list_insert(&wl.cl, &new_surf->link);
}

static void comp_create_reg(struct wl_client *client,
	struct wl_resource *resource, uint32_t id)
{
	trace("create region");
/*	struct wl_resource *region = wl_resource_create (client, &wl_region_interface, 1, id);
	wl_resource_set_implementation (region, &region_interface, NULL, NULL); */
}

static struct wl_compositor_interface compositor_if = {
	.create_surface = comp_surf_create,
	.create_region = comp_create_reg,
};

#include "shell.c"
static struct wl_shell_surface_interface shell_surface_if = {
	.pong = ssurf_pong,
	.move = ssurf_move,
	.resize = ssurf_resize,
	.set_toplevel = ssurf_toplevel,
	.set_transient = ssurf_transient,
	.set_fullscreen = ssurf_fullscreen,
	.set_popup = ssurf_popup,
	.set_maximized = ssurf_maximized,
	.set_title = ssurf_title,
	.set_class = ssurf_class
};

static void shell_get_surface(struct wl_client* cl,
	struct wl_resource* res, uint32_t id, struct wl_resource* surface)
{
	trace("shell_get_surface");
	struct wl_resource* surf = wl_resource_create(cl,
		&wl_shell_surface_interface, 1, id);
	wl_resource_set_implementation(surf, &shell_surface_if, NULL, NULL);
}

static struct wl_shell_interface shell_if = {
	.get_shell_surface = shell_get_surface
};

#include "seat.c"
static struct wl_seat_interface seat_if = {
	.get_pointer = seat_pointer,
	.get_keyboard = seat_keyboard,
	.get_touch = seat_touch
};

#include "xdg.c"
static struct zxdg_shell_v6_interface xdg_shell_if = {
	.destroy = xdg_destroy,
	.create_positioner = xdg_positioner,
	.get_xdg_surface = xdg_get_surface,
	.pong = xdg_pong,
	.use_unstable_version = xdg_unstable
};

static void bind_comp(struct wl_client *client,
	void *data, uint32_t version, uint32_t id)
{
	trace("wl_bind(compositor %d:%d)", version, id);
	struct wl_resource* res = wl_resource_create(client,
		&wl_compositor_interface, version, id);
	wl_resource_set_implementation(res, &compositor_if, NULL, NULL);
}

static void bind_seat(struct wl_client *client,
	void *data, uint32_t version, uint32_t id)
{
	trace("wl_bind(seat %d:%d)", version, id);
	struct wl_resource* res = wl_resource_create(client,
		&wl_seat_interface, version, id);
	wl_resource_set_implementation(res, &seat_if, NULL, NULL);
	wl_seat_send_capabilities(res, WL_SEAT_CAPABILITY_POINTER |
		WL_SEAT_CAPABILITY_KEYBOARD | WL_SEAT_CAPABILITY_TOUCH);
}

static void bind_xdg(struct wl_client* client,
	void* data, uint32_t version, uint32_t id)
{
	trace("wl_bind(xdg %d:%d)", version, id);
	struct wl_resource* res = wl_resource_create(client,
		&zxdg_shell_v6_interface, version, id);
	wl_resource_set_implementation(res, &xdg_shell_if, NULL, NULL);
}

static void bind_shell(struct wl_client* client,
	void *data, uint32_t version, uint32_t id)
{
	trace("wl_bind(shell %d:%d)", version, id);
	struct wl_resource* res = wl_resource_create(client,
		&wl_shell_surface_interface, version, id);
	wl_resource_set_implementation(res, &shell_if, NULL, NULL);
}

int main(int argc, char* argv[])
{
/* Control connection identifies as a bridge and translation layer, if that one
 * is terminated, they are all terminated. */
	struct arg_arr* aarr;
	wl.disp = wl_display_create();
	if (!wl.disp){
		fprintf(stderr, "Couldn't create wayland display\n");
		return EXIT_FAILURE;
	}

	struct arcan_shmif_cont control = arcan_shmif_open(
		SEGID_BRIDGE, SHMIF_ACQUIRE_FATALFAIL, &aarr);

	if (SHMIFEXT_OK != arcan_shmifext_headless_setup(
		&control, arcan_shmifext_headless_defaults()
	));

/* Additional GL bindings needed */
	bind_display = (PFNEGLBINDWAYLANDDISPLAYWL)
		eglGetProcAddress ("eglBindWaylandDisplayWL");
	query_buffer = (PFNEGLQUERYWAYLANDBUFFERWL)
		eglGetProcAddress ("eglQueryWaylandBufferWL");
	img_tgt_text = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
		eglGetProcAddress ("glEGLImageTargetTexture2DOES");

/*
 * The decision to not murder xkb but rather make it worse by spreading
 * it everywhere, running out of /facepalm -- when even android does it
 * better, you're really in for a treat.
 */
	wl_list_init(&wl.cl);
	wl_list_init(&wl.surf);

/*
 * FIXME: need a user config- way to set which interfaces should be
 * enabled and which should be disabled
 */
	wl_display_add_socket_auto(wl.disp);
	wl_global_create(wl.disp, &wl_compositor_interface, 3, NULL, &bind_comp);
	wl_global_create(wl.disp, &wl_shell_interface, 1, NULL, &bind_shell);
	wl_global_create(wl.disp, &wl_seat_interface, 1, NULL, &bind_seat);
	wl_global_create(wl.disp, &zxdg_shell_v6_interface, 1, NULL, &bind_xdg);

/* FIXME: (gl) bind_display(backend_get_egl_display(), wl.disp); */
	wl_display_init_shm(wl.disp);
	trace("wl_display() finished");

	while(1){
		struct wl_event_loop* loop = wl_display_get_event_loop(wl.disp);
//		int fd = wl_event_loop_get_fd(loop);

/* FIXME multiplex on conn and shmif-fd and poll */

		wl_event_loop_dispatch(loop, 0);
		wl_display_flush_clients(wl.disp);

		arcan_event ev;
		while (arcan_shmif_poll(&control, &ev) > 0){
			if (ev.tgt.kind == TARGET_COMMAND_EXIT)
				goto out;
		}
	}

out:
	wl_display_destroy(wl.disp);

	return EXIT_SUCCESS;
}
