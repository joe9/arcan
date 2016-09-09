static void ssurf_pong(struct wl_client *client,
	struct wl_resource *resource, uint32_t serial)
{
/* protocol: parent pings, child must pong or be deemed unresponsive */
	trace("pong (%"PRIu32")", serial);
}

static void ssurf_move(struct wl_client *client,
	struct wl_resource *resource, struct wl_resource *seat, uint32_t serial)
{
	trace("drag_move (%"PRIu32")", serial);
/* mouse drag move, s'ppose the CSD approach forces you to that .. */
}

static void ssurf_resize(struct wl_client *client,
	struct wl_resource *resource, struct wl_resource *seat,
	uint32_t serial, uint32_t edges)
{
	trace("drag_resize (%"PRIu32")", serial);
/* mouse drag resize, again because of the whole CSD */
}

static void ssurf_toplevel(struct wl_client *client,
	struct wl_resource *resource)
{
/* toplevel seem to match what we mean with a primary surface, thus
 * we need some method to reassign wayland surfaces with segments */
	trace("toplevel()");
}

static void ssurf_transient(struct wl_client *client,
	struct wl_resource *resource, struct wl_resource *parent,
	int32_t x, int32_t y, uint32_t flags)
{
/*
 * so this is reparenting a surface, same as with toplevel
 */
	trace("viewport/relative %d, %d\n", (int)x, (int)y);
}

static void ssurf_fullscreen(struct wl_client *client,
	struct wl_resource *resource, uint32_t method,
	uint32_t framerate, struct wl_resource *output)
{
	trace("fullscreen\n");
/*
 * so a client is supposed to be allowed to direct which display
 * output and some synchronization rules for that? yeah right..
 */
}

static void ssurf_popup(struct wl_client *client,
	struct wl_resource *resource, struct wl_resource *seat,
	uint32_t serial, struct wl_resource *parent,
	int32_t x, int32_t y, uint32_t flags)
{
	trace("popup\n");
/* that any surface can fullfill any role is such a pain */
}

static void ssurf_maximized(struct wl_client *client,
	struct wl_resource *resource, struct wl_resource *output)
{
	trace("surf_maximize()\n");
/* try resize to last DISPLAYHINT? */
}

static void ssurf_title(struct wl_client *client,
	struct wl_resource *resource, const char *title)
{
/* normal title-hint event */
	trace("title(%s)", title ? title : "no title");
}

static void ssurf_class(struct wl_client *client,
struct wl_resource *resource, const char *class_)
{
/* normal ident */
	trace("class(%s)", class_ ? class_ : "no class"); /* indeed */
}
