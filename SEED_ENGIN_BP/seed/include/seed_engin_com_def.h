#ifndef SEED_ENGIN_COMM_DEF_H_
#define SEED_ENGIN_COMM_DEF_H_

#ifdef WIN32
	#ifdef _WINDLL  
		#define DLL_API __declspec(dllexport)  
	#else  
		#define DLL_API __declspec(dllimport)  
	#endif
#else
	#define DLL_API 
#endif   


#define IN  

#define OUT 

#define INOUT 

#define CHAR char  

#define ULONG_32  unsigned int 

#endif