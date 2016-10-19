#include "android_mengine.h"

typedef struct android_app android_app;
typedef struct android_poll_source android_poll_source;

void android_main(android_app * app) {
	int result;
	int events;
	android_poll_source * source;


	/*	TODO : Delete app_dummy() call
		Use this function only to make sure that glue works properly 
	*/
	app_dummy();

	while (1) {
		/* Event processing loop */
		while ((result = ALooper_pollAll(-1, 0, &events, (void**) &source)) >= 0) {
			/* An event has to be processed */
			if (source != 0) {
				source->process(app, source);
			}
			/* Application is getting destroyed */
			if (app->destroyRequested) {
				return;
			}
		}
	}
}
