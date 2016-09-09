/*
 * Copyright 2016, Björn Ståhl
 * License: 3-Clause BSD, see COPYING file in arcan source repository.
 * Reference: http://arcan-fe.com
 * Description:
 *  This platform header describes the <trusted> interface used for running a
 *  separate process that aggregates all the possible sensor inputs for a
 *  head-mounted display for virtual/augmented reality purposes. It is not
 *  intended for this interface to be used by an untrusted process, so the
 *  normal safe-guards in terms of structure integrity, backing-store resize
 *  etc. will not be in place.
 *
 *  The use is simply that a memory page with the structure below will
 *  be prepared, and a preconfigured process will be launched with a reference
 *  to this structure. It is mapped, the atomic state flags are set and
 *  both sides poll/update accordingly.
 *
 *  The process will be launched with:
 *  ./hmd_server shm_fd device_index (start at 0)
 *
 *  with the command-fifo sending one or two byte commands:
 *  'p' - ping, pushed when sampled
 *  '+' - activate sampling/display
 *  '-' - deactivate sampling/display
 *  's' - retrieve descriptor and map next available device
 *
 *  Open-ended questions:
 *   eye-blink event coordination
 *   avatar- dimensions?
 */
enum avatar_limbs {
	PERSON = 0, /* abstract for global positioning */
	NECK,
	L_EYE,
	R_EYE,
	L_SHOULDER,
	R_SHOULDER,
	L_ELBOW,
	R_ELBOW,
	L_WRIST,
	R_WRIST,
/* might seem overly detailed but with glove- devices some points
 * can be sampled and others can be inferred through kinematics */
	L_THUMB_PROXIMAL,
	L_THUMB_MIDDLE,
	L_THUMB_DISTAL,
	L_POINTER_PROXIMAL,
	L_POINTER_MIDDLE,
	L_POINTER_DISTAL,
	L_MIDDLE_PROXIMAL,
	L_MIDDLE_MIDDLE,
	L_MIDDLE_DISTAL,
	L_RING_PROXIMAL,
	L_RING_MIDDLE,
	L_RING_DISTAL,
	L_PINKY_PROXIMAL,
	L_PINKY_MIDDLE,
	L_PINKY_DISTAL,
	R_THUMB_PROXIMAL,
	R_THUMB_MIDDLE,
	R_THUMB_DISTAL,
	R_POINTER_PROXIMAL,
	R_POINTER_MIDDLE,
	R_POINTER_DISTAL,
	R_MIDDLE_PROXIMAL,
	R_MIDDLE_MIDDLE,
	R_MIDDLE_DISTAL,
	R_RING_PROXIMAL,
	R_RING_MIDDLE,
	R_RING_DISTAL,
	R_PINKY_PROXIMAL,
	R_PINKY_MIDDLE,
	R_PINKY_DISTAL,
	L_HIP,
	R_HIP,
	L_KNEE,
	R_KNEE,
	L_ANKLE,
	R_ANKLE,
	LIMB_LIM
};

/* provided / built-in simple- triangle meshes, allowed to 'fill'
 * the rest of the page assuming that page_sz is not exceeded */
enum mesh_types {
	DISTORTION_LEFT = 1,
	DISTORTION_RIGHT = 2,
	CHAPERONE = 4
};

enum tone_types {
	NONE = 0,
	BINARY = 1,
	DIFFUSE = 2,
	DIFFUSE_PULSE = 4
};

/*
 * uncertain about the skeleton model for this system, what coordinate
 * system would one use to describe the location of each force feedback
 * point for things like haptic suits?
 */
struct hpoint {
	uint8_t type; /* sets the matching tone_type */
	float amplitude; /* binary, clamp to 0 or !0, diffuse (0..1) */
	float frequency;
	uint32_t len; /* pulse period length or (for binary/diffuse, max. len */
};

struct limb {
/* consumer read, producer write */
	_Alignas(16) float matr[16];
	uint64_t update_ts;

/* parent- limb index or (-1) if the transform is in character-space */
	int relative;

	vector position;
	vector forward;
	quat orientation;
	float accuracy;

	uint8_t tone_type; /* mask of supported types */

/* consumer write, producer read */
	bool ignored;
	struct hpoint haptic;
};

struct hmd_shmif
{
	size_t page_sz;
	uint32_t pr_version_cookie;

/* filled in by client during setup */
	uint32_t cl_version_cookie;
	size_t n_limbs;

/* if more distinct characters can be extracted */
	int index;
	size_t limit;

/* NOTE: EDID or other identifier in order to pair with monitor? */

/* devices we take control of */
	struct {
		uint16_t vid, pid;
	} block[8];

/* perspective transform */
	_Alignas(16) float left[16];
	_Alignas(16) float right[16];
	float fov, ipd, ratio;

/* consumer will try- to lock when it is time to render avatar, while
 * held- members are expected not to be modified */
	pthread_mutex_t lock;

/* bitmask of button states */
	uint64_t actuators;
	struct limb limbs[LIMB_LIM];

/* note: need something for meshes and non-limb bound haptics */
};
