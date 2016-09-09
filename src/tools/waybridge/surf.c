static void surf_destroy(struct wl_client* cl, struct wl_resource* res)
{
	trace("surf_destroy()");
}

static void surf_attach(struct wl_client* cl, struct wl_resource* res,
	struct wl_resource* buf, int32_t x, int32_t y)
{
	trace("surf_attach(%d, %d)", (int)x, (int)y);
}

static void surf_damage(struct wl_client* cl, struct wl_resource* res,
	int32_t x, int32_t y, int32_t w, int32_t h)
{
	trace("surf_damage(%d+%d, %d+%d)", (int)x, (int)w, (int)y, (int)h);
}

static void surf_frame(
	struct wl_client* cl, struct wl_resource* res, uint32_t cb)
{
	trace("surf_damage()");
}

static void surf_opaque(struct wl_client* cl,
	struct wl_resource* res, struct wl_resource* reg)
{
	trace("surf_opaqaue()");
}

static void surf_inputreg(struct wl_client* cl,
	struct wl_resource* res, struct wl_resource* reg)
{
	trace("surf_inputreg()");
}

static void surf_commit(struct wl_client* cl, struct wl_resource* res)
{
	struct bridge_surface* surf = wl_resource_get_user_data(res);
/* EGLInt dfmt;
 * if eglQueryWaylandBufferWL(get_egl_disp(), surf->buf, EGL_TEXTURE_FORMAT,
 * 	&textfmt){
 * 		EGLAttrib attrs;
 * 		EGLInt dw, dh;
 * 		eglQueryWaylandBufferWL(..., EGL_WIDTH), EGL_HEIGHT
 * 		EGLImage image = eglCreateImage(get_egl_disp(), EGL_NO_CONTEXT,
 * 			EGL_WAYLAND_BUFFER_WL, surf->buf, &attrs)
 * 		then forward buffer through the normal, ...
 * 	} else */
	{
		trace("surf_commit(shm)");
/*		struct wl_shm_buffer* buf = wl_shm_buffer_get(surf->buf);
		uint32_t w = wl_shm_buffer_get_width(buf);
		uint32_t h = wl_shm_buffer_get_height(buf);
		void* data = wl_shm_buffer_get_data(buf); */
/* two options, we create a texture, grab a handle to it and forward,
 * or we copy to the normal shm */
	}
	wl_buffer_send_release(surf->buf);
}

static void surf_transform(struct wl_client* cl,
	struct wl_resource* res, int32_t transform)
{
	trace("surf_transform(%d)", (int) transform);
}

static void surf_scale(struct wl_client* cl,
	struct wl_resource* res, int32_t scale)
{
	trace("surf_scale(%d)", (int) scale);
}
