static void seat_pointer(struct wl_client* cl,
	struct wl_resource* res, uint32_t id)
{
/* wl_resource_create on pointer interface,
 * wl_resource_set_implementation(pointer, pointer_interface,
 * find client, set pointer field to pointer */
	trace("seat_pointer(%"PRIu32")", id);
}

static void seat_keyboard(struct wl_client* cl,
	struct wl_resource* res, uint32_t id)
{
	trace("seat_keyboard(%"PRIu32")", id);
}

static void seat_touch(struct wl_client* cl,
	struct wl_resource* res, uint32_t id)
{
	trace("seat_touch(%"PRIu32")", id);
}
