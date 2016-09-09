#include "hmd_platform.h"

/*
 * other changes needed:
 *  1. Lua layer
 *   - need to tag model with limb (i.e. figure out limbs),
 *   - need to tag camera with hmd
 *   - need to expose actuators (ff- model)
 *   - need to expose device control (on/off)
 *
 * 2. something with calibration and resets
 */

/*
 * If the feature is enabled (ext_hmd etc.), when this page has been prepared
 * it is never actually removed to avoid having to track references to mapped
 * objects during connect / disconnect - the page is merely re-used.
 */
struct hmd_ctrl;
struct hmd_ctrl {
	bool active;
	int fd_out;
	int shm_fd;
	struct hmd_shmif* map;
	struct hmd_ctrl* next;
} hmd = {
	.fd_out = -1,
	.shm_fd = -1
};

/*
 * open-ended question, how do we expose the mapping to the meshes of a model?
 * should just be a synch- function with some memcpys, but how to deal with
 * creation etc.?
 */
static void init_hmd(struct hmd_ctrl* ctrl, int ind)
{
	char* kv = arcan_db_appl_val(dbhandle, "arcan", "ext_hmd");
	if (!kv)
		return;

	if (!arcan_isfile(kv)){
		arcan_warning("init_hmd(%s): bad hmd executable\n");
		free(kv);
		return;
	}

/* create fd_in (nonblock, socketpair) */

/* if not mempage, allocate */
/* allocate mutex on page */

/* else:
 * reset page state, reset mutex state */

/* spawn new process */

	struct stat statv;
	if (-1 == fifo_out && kv && -1 != stat(kv,&statv) && s_isfifo(statv.st_mode)){
		arcan_warning("arcan_led(), trying to map %s (config arcan:ext_led)"
			" as a led controller\n", kv);
		fifo_out = open(kv, o_nonblock | o_wronly);

	free(kv);

/*
 * Check the status of the HMD connection, if missing, try and re-establish
 */
static void check_hmd()
{
	if (-1 == hmd.cmd_fd)
		init_hmd();

	if (-1 == hmd.cmd_fd)
		return;

/* FIXME: daemonize, send ping, on failure, detach and pick different resources */
}
