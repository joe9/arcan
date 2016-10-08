static void zxdg_pos_destroy(struct wl_client* cl, struct wl_resource* res)
{
	trace("zxdg_positioner_destroy()");
}

static void zxdg_pos_arect(struct wl_client* cl, struct wl_resource* res,
	int32_t x, int32_t y, int32_t w, int32_t h)
{
	trace("zxdg_positioner:anchor region(%"PRIi32", %"PRIi32", %"PRIi32
		",%"PRIi32"\n", x, y, w, h);
}

static void zxdg_pos_ofs(struct wl_client* cl, struct wl_resource* res,
	int32_t x, int32_t y)
{
	trace("zxdg_positioner:offset(%"PRIi32", %"PRIi32")\n", x, y);
}

static void zxdg_pos_anchor(struct wl_client* cl, struct wl_resource* res,
	uint32_t anchor)
{
	trace("zxdg_positioner:anchor(%"PRIu32")\n", anchor);
}

static void zxdg_pos_grav(struct wl_client* cl, struct wl_resource* res,
	uint32_t gravity)
{
	trace("zxdg_positioner:gravity(%"PRIu32")\n", gravity);
}

static void zxdg_pos_cadj(struct wl_client* cl, struct wl_resource* res,
	uint32_t adjust)
{
	trace("zxdg_positioner:constraint_adjustment(%"PRIu32")\n", adjust);
}

struct zxdg_positioner_v6_interface xdg_pos_if = {
	.destroy = zxdg_pos_destroy,
	.set_anchor_rect = zxdg_pos_arect,
	.set_anchor = zxdg_pos_anchor,
	.set_gravity = zxdg_pos_grav,
	.set_constraint_adjustment = zxdg_pos_cadj,
	.set_offset = zxdg_pos_ofs
};

static void xdg_positioner(struct wl_client* cl, struct wl_resource* res)
{
/* should translate to VIEWPORT events for subsegments */
	trace("xdg_positioner()");
}

static void xdg_surf_destroy(struct wl_client* cl, struct wl_resource* res)
{
	trace("xdg_surf_destroy()");
}

struct xdg_surface_interface xdg_surf_if {
	.destroy = xdg_surf_destroy,
	.set_parent = xdg_surf_title,
	.set_app_id = xdg_surf_app_id,
	.show_window_menu = xdg_wnd_menu,
	.move = xdg_surf_move,
	.resize = xdg_surf_resize,
	.set_max_size = xdg_surf_size,
	.set_min_size = xdg_surf_min_sz,
	.set_maximized = xdg_surf_maximize,
	.unset_maximized = xdg_surf_demaximize,
	.set_fullscreen = xdg_surf_fs,
	.unset_fullscreen = xdg_surf_defs,
	.set_minimized = xdg_surf_minimize
};

static void xdg_get_surface(struct wl_client* cl,
	struct wl_resource* res, uint32_t id, struct wl_resource* surf)
{
	trace("xdg_get_surface(%"PRIu32")", id);
	struct bridge_surface* new_surf = malloc(sizeof(struct bridge_surface));
	memset(new_surf, '\0', sizeof(struct bridge_surface));
	new_surf->res = wl_resource_create(client, &wl_surface_interface,
		wl_resource_get_version(resource), id);

	wl_resource_set_implementation(new_surf->res,
		&xdg_surf_if, new_surf, xdg_surf_destroy);

	new_surf->cl = find_client(cl);
	wl_list_insert(&wl.cl, &new_surf->link);
}

static void xdg_pong(struct wl_client* wl,
	struct wl_resource* res, uint32_t serial)
{
	trace("xdg_pong(%"PRIu32")", serial);
}

static void xdg_destroy(struct wl_client* cl, struct wl_resource* res)
{
	trace("xdg_destroy()");
}
