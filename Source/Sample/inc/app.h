extern "C" void
temple_application_init(void* data);
extern "C" void
temple_application_update(void* data);
extern "C" void
temple_application_set_log_function(void (*func)(const char* msg, unsigned int msg_size));
extern "C" void
temple_application_close();
extern "C" void
temple_application_get_frame(void* data, unsigned int data_size);
