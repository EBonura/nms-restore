int 	calculate_vol(int obj_num);
int 	init_sound(void);
void 	clear_sound(void);
void 	stop_all_samples(void);
void 	clear_sample_triggers(void);
int 	init_samples(char filename[]);
int 	add_sample(int number, int obj_num);
int 	add_sample_pos(int number, int x, int y);
int 	calculate_vol(int obj_num);
int 	calculate_vol_pos(int x, int y);
void 	process_samples(void);

#define N_VOICES 8        // # of mixer channels to allocate

char sound_active=FALSE;

HDIGDRIVER dig;            // Handle to digital audio driver

HTIMER pmode_timer_handle;
HSAMPLE sample_handles[N_VOICES]; // Sample handles for each mixer channel

#define NUM_SAMPLES		64

struct samples_st {
	unsigned char *ptr;
	long 	length;
	short priority;
	short type;
} samples[NUM_SAMPLES];

struct samples_playing_st {
	short number;	
	short vol;
	short priority;
	short type;
	short used_for;
	short obj_num;
	char  active;
} samples_playing[N_VOICES];

#define TRIGGER_LIST_SIZE	32

short sample_trigger_pos=0;

struct sample_triggers_st {
	short number;
	short priority;
	short type;
	short obj_num;
	char vol;
	char triggered;
} sample_triggers[TRIGGER_LIST_SIZE];


/* sample types */

enum {
	SAMP_EXPLOSION,
	SAMP_SPEECH,
	SAMP_TANK,
	SAMP_FIRING,
	SAMP_MISC
};
/* sample channel usage */

#define EXPLO_CHANNEL_1		0
#define EXPLO_CHANNEL_2		1
#define EXPLO_CHANNEL_3		2
#define TANK_CHANNEL		3
#define TANK_FIRING_CHANNEL	4
#define MISC_CHANNEL_START	4		// if tank not firing use channel

/* actual sample numbers */

enum {
	SAMP_FRONT_GUN,
	SAMP_HI_EX_GUN,
	SAMP_MORTAR,
	SAMP_MINES,
	SAMP_TIMEBOMB,
	SAMP_FLAMETHROWER,
	SAMP_GUIDED_MISSILE,
	SAMP_MACHINE_GUN,
	SAMP_SLOW_GUN,
	SAMP_SMALL_EXPLO,
	SAMP_MEDIUM_EXPLO,
	SAMP_BIG_EXPLO,
};


/* this will initalise sound drivers with 8 sample channels */

int init_sound(void)
{
	int loop;

	AIL_startup();

/* alter sample trigger rate */
	AIL_set_preference(DIG_LATENCY, 30);
	AIL_set_preference(DIG_HARDWARE_SAMPLE_RATE, 11025);
	AIL_set_preference(DIG_USE_16_BITS,          NO);
	AIL_set_preference(DIG_USE_STEREO,           YES);

	if (AIL_install_DIG_INI(&dig) != AIL_INIT_SUCCESS) {
		AIL_shutdown();
		return -1;
	}

	for (loop=0; loop<N_VOICES; loop++) {
		sample_handles[loop]=AIL_allocate_sample_handle(dig);
		samples_playing[loop].active=FALSE;
		samples_playing[loop].used_for=SAMP_MISC;

		if (sample_handles[loop]==NULL) {
			AIL_shutdown();
			return -1;
		}
	}

	/* sample channel usage */
	samples_playing[EXPLO_CHANNEL_1].used_for=SAMP_EXPLOSION;
	samples_playing[EXPLO_CHANNEL_2].used_for=SAMP_EXPLOSION;
	samples_playing[TANK_CHANNEL].used_for=SAMP_TANK;


	if (init_samples("sounds\\samples.txt")==-1) {
	}

	/* need to setup 70 Hz timer callback function */
//	pmode_timer_handle=AIL_register_timer(&pmode_timer_sound);
//	AIL_set_timer_period(pmode_timer_handle,13500);	// a bit less than 70Hz
//	AIL_start_timer(pmode_timer_handle);


	/* this all looks cool. return ok. */
	sound_active=TRUE;

	return 1;
}

void clear_sound(void)
{
	if (sound_active) {
		AIL_shutdown();
		sound_active=FALSE;
	}

	return;
}

void stop_all_samples(void)
{
	int loop;

	for (loop=0; loop<N_VOICES; loop++) {
		AIL_end_sample(sample_handles[loop]);
		samples_playing[loop].active=FALSE;
	}
}

void clear_sample_triggers(void)
{
	sample_trigger_pos=0;
}

int init_samples(char filename[])
{
	FILE *fp;
	char string[80];
	int position;
	
 	if (!sound_active) return -1;

	clear_sample_triggers();
	
	if((fp = fopen(filename,"rt"))==NULL)	{
		AIL_shutdown();
		sound_active=FALSE;
		return -1;
	}
	
	while (!feof(fp)) {
		fscanf( fp, "%s", string);

		if (strcmp(string,"number")==0) {
			fscanf( fp, "%s", string);
			position=atoi(string);
		}
		if (strcmp(string,"filename")==0) {
			fscanf( fp, "%s", string);
			samples[position].ptr=loadfile(string);
			samples[position].length=_get_file_length(string);
		}
		if (strcmp(string,"type")==0) {
			fscanf( fp, "%s", string);
			samples[position].type=atoi(string);
		}
		if (strcmp(string,"priority")==0) {
			fscanf( fp, "%s", string);
			samples[position].priority=atoi(string);
		}
	}
	fclose(fp);

	return 1;
}

int add_sample(int number, int obj_num)
{
	if (!sound_active) return -1;

	if (sample_trigger_pos>=TRIGGER_LIST_SIZE) {
		return -1;
	}
 	if ((sample_triggers[sample_trigger_pos].vol=calculate_vol(obj_num))==0) {
		/* sample too quiet */
		return -1;
	}
	sample_triggers[sample_trigger_pos].triggered=FALSE;
	sample_triggers[sample_trigger_pos].number=number;
	sample_triggers[sample_trigger_pos].obj_num=obj_num;
	sample_triggers[sample_trigger_pos].priority=samples[number].priority;
	sample_triggers[sample_trigger_pos].type=samples[number].type;

	sample_trigger_pos++;

	return 1;
}

int add_sample_pos(int number, int x, int y)
{
	if (!sound_active) return -1;

	if (sample_trigger_pos>=TRIGGER_LIST_SIZE) {
		return -1;
	}
 	if ((sample_triggers[sample_trigger_pos].vol=calculate_vol_pos(x,y))==0) {
		/* sample too quiet */
		return -1;
	}
	sample_triggers[sample_trigger_pos].triggered=FALSE;
	sample_triggers[sample_trigger_pos].number=number;
//	sample_triggers[sample_trigger_pos].obj_num=obj_num;
	sample_triggers[sample_trigger_pos].priority=samples[number].priority;
	sample_triggers[sample_trigger_pos].type=samples[number].type;

	sample_trigger_pos++;

	return 1;
}


#define 	MAX_VOLUME		100
#define		VOL_DECREASE	5

/* get distance from player */

int calculate_vol(int obj_num)
{
	int vol, delta_x, delta_y, total_dist;

	if (obj_num==your_tank) {
		return MAX_VOLUME;
	}
	
	delta_x=(obj_list[your_tank].vars[ai_x]-obj_list[obj_num].vars[ai_x])>>8;
	delta_x*=delta_x;
	delta_y=(obj_list[your_tank].vars[ai_z]-obj_list[obj_num].vars[ai_z])>>8;
	delta_y*=delta_y;

	total_dist=sqrt(delta_x+delta_y);

	vol=MAX_VOLUME-(total_dist/VOL_DECREASE);
	if (vol<16) vol=0;

	return vol;
}

int calculate_vol_pos(int x, int y)
{
	int vol, delta_x, delta_y, total_dist;

	
	delta_x=(obj_list[your_tank].vars[ai_x]-x)>>8;
	delta_x*=delta_x;
	delta_y=(obj_list[your_tank].vars[ai_z]-y)>>8;
	delta_y*=delta_y;

	total_dist=sqrt(delta_x+delta_y);

	vol=MAX_VOLUME-(total_dist/VOL_DECREASE);
	if (vol<16) vol=0;

	return vol;
}
	
	
int find_free_sample_handle(int type, int priority)
{
	int loop;

	if (type==SAMP_EXPLOSION) {
		if (AIL_sample_status(sample_handles[EXPLO_CHANNEL_1])!=SMP_PLAYING) {
			return EXPLO_CHANNEL_1;
		}
		if (AIL_sample_status(sample_handles[EXPLO_CHANNEL_2])!=SMP_PLAYING) {
			return EXPLO_CHANNEL_2;
		}
		if (AIL_sample_status(sample_handles[EXPLO_CHANNEL_3])!=SMP_PLAYING) {
			return EXPLO_CHANNEL_3;
		}
	/* no free handles */
		return -1;
	}

	for (loop=MISC_CHANNEL_START; loop<N_VOICES; loop++) {
		if (AIL_sample_status(sample_handles[loop])!=SMP_PLAYING) {
			return loop;
		}
	}
/* no free handles, try and find 1 with a free handle */
	for (loop=0; loop<N_VOICES; loop++) {
/* sample has lower priority */
		if (samples_playing[loop].priority<priority) {
			return loop;
		}
	}

	/* no free samples */
	return -1;
}
		

#define TANK_FIRING_PRIORITY		255	
			
void process_samples(void)
{
	int loop, changed=TRUE, list_pos, handle;
	struct sample_triggers_st temp;


	if (!sound_active) return;

	/* sort trigger list in order of priority */
/*
	while (changed) {
		changed=FALSE;
		for (loop=1; loop<sample_trigger_pos; loop++) {
			if (samples_playing[loop].priority>samples_playing[loop-1].priority) {
				memcpy(&temp, &samples_playing[loop], sizeof(temp));
				memcpy(&samples_playing[loop], &samples_playing[loop-1], sizeof(temp));
				memcpy(&samples_playing[loop-1], &temp, sizeof(temp));
				changed=TRUE;
			}
		}
	}
*/

	/* scan list */
	for (list_pos=0;list_pos<sample_trigger_pos; list_pos++) {
		if ((sample_triggers[list_pos].obj_num==your_tank)&&(sample_triggers[list_pos].type==SAMP_FIRING)) {
            AIL_init_sample              ( sample_handles[TANK_FIRING_CHANNEL]                  );
            AIL_set_sample_type          ( sample_handles[TANK_FIRING_CHANNEL], DIG_F_MONO_8, 0 );
            AIL_set_sample_address       ( sample_handles[TANK_FIRING_CHANNEL], samples[sample_triggers[list_pos].number].ptr, samples[sample_triggers[list_pos].number].length  );
            AIL_set_sample_playback_rate ( sample_handles[TANK_FIRING_CHANNEL], 11025           );
            
            AIL_set_sample_volume        ( sample_handles[TANK_FIRING_CHANNEL], sample_triggers[list_pos].vol );

            //
            // Finally, activate sample
            //

            AIL_start_sample(sample_handles[TANK_FIRING_CHANNEL]);
/* stop any other smaples overriding  tank firing */
			samples_playing[TANK_FIRING_CHANNEL].priority=TANK_FIRING_PRIORITY;

		} else if ((handle=find_free_sample_handle(sample_triggers[list_pos].type,sample_triggers[list_pos].priority))!=-1) {
            AIL_init_sample              ( sample_handles[handle]                  );
            AIL_set_sample_type          ( sample_handles[handle], DIG_F_MONO_8, 0 );
            AIL_set_sample_address       ( sample_handles[handle], samples[sample_triggers[list_pos].number].ptr, samples[sample_triggers[list_pos].number].length  );
            AIL_set_sample_playback_rate ( sample_handles[handle], 11025           );
            
            AIL_set_sample_volume        ( sample_handles[handle], sample_triggers[list_pos].vol );

            //
            // Finally, activate sample
            //
            AIL_start_sample(sample_handles[handle]);
			samples_playing[handle].priority=sample_triggers[list_pos].priority;
		}
	}

	clear_sample_triggers();

	return;
}
	
		
				
				
	
		
