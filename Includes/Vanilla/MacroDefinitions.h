#ifndef MACRODEFINITIONS_H
#define MACRODEFINITIONS_H

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define PI 3.14159265359

#endif
