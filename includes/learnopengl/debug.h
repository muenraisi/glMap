#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//begin for memory leak detection
#ifdef _DEBUG
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define new new
#endif


void detect_memory_leaks(bool on_off);

// end for memory leak dection