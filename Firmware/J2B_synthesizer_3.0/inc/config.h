#ifndef __CONFIG_H__
#define __CONFIG_H__


#define ATMEGATRON  (0)
#define ATMEGADRUM  (1)
#define ATCYCLOTRON  (2)
#define DELAYERTRON  (3)
#define DUOTRON  (4)
#define ATPOLYTRON  (5)


#define PRODUCT  ATMEGATRON
//#define PRODUCT  ATMEGADRUM
//#define PRODUCT  ATCYCLOTRON
//#define PRODUCT  DELAYERTRON


#if PRODUCT==ATMEGATRON
	#define PRODUCT_NAME  ("Atmegatron")
	#define FIRMWARE_VERSION  ("Firmware v1.4")
#elif PRODUCT==ATMEGADRUM
	#define PRODUCT_NAME  ("Atmegadrum")
	#define FIRMWARE_VERSION  ("Firmware v1.2c")
#elif PRODUCT==ATCYCLOTRON
	#define PRODUCT_NAME  ("Atcyclotron")
	#define FIRMWARE_VERSION  ("Firmware v1.1")
#elif PRODUCT==DELAYERTRON
	#define PRODUCT_NAME  ("Delayertron")
	#define FIRMWARE_VERSION  ("Firmware v1.1")
#elif PRODUCT==DUOTRON
	#define PRODUCT_NAME  ("Duotron")
	#define FIRMWARE_VERSION  ("Firmware v1.1")
#elif PRODUCT==ATPOLYTRON
	#define PRODUCT_NAME  ("Atpolytron")
	#define FIRMWARE_VERSION  ("Firmware v0.1")
#else
	#define PRODUCT_NAME  ("Prod. undefined")
	#define FIRMWARE_VERSION  ("Firmware v0.0")
#endif


#endif /* __CONFIG_H__ */
