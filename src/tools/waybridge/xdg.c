static void xdg_destroy(struct wl_client* cl, struct wl_resource* res)
{
	trace("xdg_destroy()");
}

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

static void xdg_get_surface(struct wl_client* wl,
	struct wl_resource* res, uint32_t id, struct wl_resource* surf)
{
	trace("xdg_get_surface(%"PRIu32")", id);
}

static void xdg_pong(struct wl_client* wl,
	struct wl_resource* res, uint32_t serial)
{
	trace("xdg_pong(%"PRIu32")", serial);
}
