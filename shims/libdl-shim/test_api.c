#include <stdio.h>
#include <stdint.h>

extern uint32_t android_get_application_target_sdk_version();

int main() {
	printf("%d\n", android_get_application_target_sdk_version());
	return 0;
}
