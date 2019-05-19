#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 0;
	static const long BUILD  = 65;
	static const long REVISION  = 65;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 33;
	#define RC_FILEVERSION 2,0,65,65
	#define RC_FILEVERSION_STRING "2, 0, 65, 65\0"
	static const char FULLVERSION_STRING [] = "2.0.65.65";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 79;
	

#endif //VERSION_H
