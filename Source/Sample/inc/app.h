extern "C" void
sisyphus_application_init(void* data);
extern "C" void
sisyphus_application_update(void* data);
extern "C" void
sisyphus_application_set_log_function(void (*func)(const char* msg, unsigned int msg_size));
extern "C" void
sisyphus_application_close();
extern "C" void
sisyphus_application_get_frame(void* data, unsigned int data_size);
