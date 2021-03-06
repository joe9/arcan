#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <arcan_shmif.h>

const char* msub_to_lbl(int ind)
{
	switch(ind){
	case MBTN_LEFT_IND: return "left";
	case MBTN_RIGHT_IND: return "right";
	case MBTN_MIDDLE_IND: return "middle";
	case MBTN_WHEEL_UP_IND: return "wheel-up";
	case MBTN_WHEEL_DOWN_IND: return "wheel-down";
	default:
		return "unknown";
	}
}

int main(int argc, char** argv)
{
	int id = SEGID_APPLICATION;
	struct arg_arr* aarr;
	if (argc > 1){
		if (strcmp(argv[1], "-game") == 0)
			id = SEGID_GAME;
		else if (strcmp(argv[1], "-terminal") == 0)
			id = SEGID_TERMINAL;
		else if (strcmp(argv[1], "-vm") == 0)
			id = SEGID_VM;
		else{
			printf("usage: \n\tiodump to identify as normal application"
				"\n\tiodump -game to identify as game"
				"\n\tiodump -terminal to identify as terminal"
				"\n\tiodump -vm to identify as vm\n"
			);
				return EXIT_FAILURE;
			}
		}

	struct arcan_shmif_cont cont = arcan_shmif_open(
		id, SHMIF_ACQUIRE_FATALFAIL, &aarr);

	arcan_event ev;

	arcan_shmif_resize(&cont, 320, 200);
	arcan_shmif_signal(&cont, SHMIF_SIGVID);

	while (arcan_shmif_wait(&cont, &ev)){
		if (ev.category == EVENT_TARGET)
		switch (ev.tgt.kind){
		case TARGET_COMMAND_EXIT:
			return EXIT_SUCCESS;
		break;
		default:
		break;
		}
		else if (ev.category == EVENT_IO){
			switch (ev.io.datatype){
				case EVENT_IDATATYPE_TRANSLATED:
					printf("(%s)[kbd(%d):%s] %d:mask=%d,sym:%d,code:%d,utf8:%s\n",
						ev.io.label,
						ev.io.devid, ev.io.input.translated.active ? "pressed" : "released",
						(int)ev.io.subid, (int)ev.io.input.translated.modifiers,
						(int)ev.io.input.translated.keysym,
						(int)ev.io.input.translated.scancode,
						ev.io.input.translated.utf8
					);

				break;
				case EVENT_IDATATYPE_ANALOG:
					printf("(%s)[%s(%d):%d] rel: %s, v(%d:%d){%d, %d, %d, %d}\n",
						ev.io.label,
						ev.io.devkind == EVENT_IDEVKIND_MOUSE ? "mouse" : "analog",
						ev.io.devid, ev.io.subid,
						ev.io.input.analog.gotrel ? "yes" : "no",
						(int)ev.io.input.analog.nvalues,
						(int)ev.io.input.analog.idcount,
						(int)ev.io.input.analog.axisval[0],
						(int)ev.io.input.analog.axisval[1],
						(int)ev.io.input.analog.axisval[2],
						(int)ev.io.input.analog.axisval[3]
					);
				break;
				case EVENT_IDATATYPE_TOUCH:
					printf("(%s)[touch(%d)] %d: @%d,%d pressure: %f, size: %f\n",
					ev.io.label,
					ev.io.devid,
					ev.io.subid,
					(int) ev.io.input.touch.x,
					(int) ev.io.input.touch.y,
					ev.io.input.touch.pressure,
					ev.io.input.touch.size
				);
				break;
				case EVENT_IDATATYPE_DIGITAL:
					if (ev.io.devkind == EVENT_IDEVKIND_MOUSE)
						printf("[mouse(%d):%d], %s:%s\n", ev.io.devid,
							ev.io.subid, msub_to_lbl(ev.io.subid),
							ev.io.input.digital.active ? "pressed" : "released"
						);
					else
						printf("[digital(%d):%d], %s\n", ev.io.devid,
							ev.io.subid, ev.io.input.digital.active ? "pressed" : "released");
				break;
				default:
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
