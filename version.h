#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "19";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.05";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 0;
	static const long BUILD  = 64;
	static const long REVISION  = 0;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 0;
	#define RC_FILEVERSION 2,0,64,0
	#define RC_FILEVERSION_STRING "2, 0, 64, 0\0"
	static const char FULLVERSION_STRING [] = "2.0.64.0";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 79;
	

#endif //VERSION_H
