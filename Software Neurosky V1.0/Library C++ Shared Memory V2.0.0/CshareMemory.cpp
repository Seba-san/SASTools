/**********************************************************************************************
Author : Ing. Diego Daniel Santiago.
diego.daniel.santiago@gmail.com
dsantiago@inaut.unsj.edu.ar
Facultad de Ingeniería
Universidad Nacional de San Juan
San Juan - Argentina

INAUT - Instituto de Automática
http://www.inaut.unsj.edu.ar/
CONICET - Consejo Nacional de Investigaciones Científicas y Técnicas.
http://www.conicet.gov.ar/

Version Actual: 2.0.1

HISTORIAL DE CAMBIO:
-V1.0.0 01/06/2014 : 	Lanzamiento version inicial
-V2.0.0 18/11/2014 : 	Se agrego Compatibilidad Con Linux
-V2.0.1 06/07/2015 : 	Se modifica Mecanismo de Cierre de memoria compartida.
			Se modifican mensajes de error.
			Se modifica constructor de la clase
			Se unificaros los tipos de datos destinados a tamaño a "size_t"
************************************************************************************************/
#ifndef CSHAREMEMORY_CPP
#define CSHAREMEMORY_CPP

#include <iostream>
#include "CshareMemory.h"
#include <stdio.h>
#include <string.h>

 CshareMemory::CshareMemory(){
     _id=-1;
     pMemory=NULL;
 }

 CshareMemory::CshareMemory(char *Name){
     _id=-1;
     pMemory=NULL;
     setName(Name);
     setSize(DEFAULT_SIZE);
 }

CshareMemory::CshareMemory(char *Name=NULL, size_t size=0){ //initializes and opens the shared memory
    _id=-1;
    pMemory=NULL;
    //Load name and size of share memory
    if (Name)
        setName(Name);
    if (size !=0)
        setSize(size);
}

CshareMemory::~CshareMemory(){
    closeShareMemory();
}

bool CshareMemory::openShareMemory(){

    if( pMemory != NULL){
        printf("ERROR IN  'CshareMemory::openShareMemory()': THE SHARE MEMORY WAS ALREADY OPEN \n");
        return false;
    }
    if (cpName==NULL){
        printf("ERROR IN  'CshareMemory::openShareMemory()': THE SHARE MEMORY WASN'T INITIALIZED \n");
        return false;
    }
    if(_size==0){
        printf("ERROR IN  'CshareMemory::openShareMemory()':MEMORY SIZE = 0 \n");
        return false;
    }

#ifdef _WIN32
    hShare = CreateFileMapping((HANDLE)NULL,NULL, PAGE_READWRITE, 0,(DWORD)_size, cpName);
    if(hShare == NULL){
        printf("ERROR IN  'CshareMemory::openShareMemory()': CreateFileMapping \n");
        return false;
    }else{
        pMemory = MapViewOfFile(hShare,FILE_MAP_ALL_ACCESS,0, 0, _size);
    }
    if(pMemory == NULL){
        printf("ERROR IN  'CshareMemory::openShareMemory()':MapViewOfFile \n");
        return false;
    }

#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
    /* creating the shared memory object    --  shm_open()  */
    shmfd = shm_open(cpName, O_CREAT|O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0) {
        perror("In shm_open()");
        return false;
    }
    //printf(stderr, "Created shared memory object %s\n", cpName);

    /* adjusting mapped file size (make room for the whole segment to map)      --  ftruncate() */
    ftruncate(shmfd, _size);
    /* requesting the shared segment    --  mmap() */
    pMemory = mmap(NULL, _size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (pMemory == NULL) {
        perror("In mmap()");
        return false;
    }
    //printf(stderr, "Shared memory segment allocated correctly (%d bytes).\n", _size);
#endif /* __linux || __APPLE__ */

    return true;
}

bool CshareMemory::closeShareMemory(){
#ifdef _WIN32
    if((pMemory != NULL)) {
        if( FlushViewOfFile(pMemory, _size) == 0){
            printf("ERROR IN 'CshareMemory::closeShareMemory()':FlushViewOfFile \n");
            return false;
        }
        if( UnmapViewOfFile(pMemory) == 0){
            printf("ERROR IN  'CshareMemory::closeShareMemory()':UnmapViewOfFile \n");
            return false;
        }
        if( CloseHandle(hShare) == 0){
            printf("ERROR IN  'CshareMemory::closeShareMemory()':CloseHandle \n");
            return false;
        }
    }
#endif
#if defined (__linux) || defined (__APPLE__)
    if (shm_unlink(cpName) != 0) {
        perror("In shm_unlink()");
        return false;
    }
#endif /* __linux || __APPLE__ */
    pMemory = NULL;
    return true;
}

bool CshareMemory::readShareMemory(void *pfData){

    if(pMemory == NULL){
        printf("ERROR IN 'CshareMemory::readShareMemory' :Read Virtual Device CANT FIND THE SHARE MEMORY \n");
        return false;
    }
    if(pfData==NULL){
        return false;
    }
    memcpy(pfData,pMemory, _size);
    return true;
}

bool CshareMemory::readShareMemory(void *pfData, size_t size){

    if(pMemory == NULL){
        printf("ERROR IN 'CshareMemory::readShareMemory':Read Virtual Device CANT FIND THE SHARE MEMORY \n");
        return false;
    }
    if(pfData==NULL){
        return false;
    }
    if (size>_size){
        printf("ERROR IN 'CshareMemory::readShareMemory':Read Virtual Device INVALID READ SIZE \n");
        return false;
    }
    memcpy(pfData,pMemory, size);
    return true;
}

bool CshareMemory::writeShareMemory(void *pfData){
    if(pMemory == NULL){
        printf("ERROR IN 'CshareMemory::writeShareMemory' :Read Virtual Device CANT FIND THE SHARE MEMORY \n");
        return false;
    }
    memcpy(pMemory,pfData,_size);

    #ifdef _WIN32
    if( FlushViewOfFile(pMemory, _size) == 0){
        printf("ERROR IN 'CshareMemory::writeShareMemory' : Close Virtual Device::FlushViewOfFile \n");
        return false;
    }
    #endif /* _WIN32 */

    return true;
}

bool CshareMemory::writeShareMemory(void *pfData, size_t size){

    if (size>_size){
       printf("ERROR IN  'CshareMemory::writeShareMemory' : size biger than memory size \n");
        return false;
    }
    if(pMemory == NULL){
       printf("ERROR IN  'CshareMemory::writeShareMemory' ): CANT FIND THE SHARE MEMORY \n");
        return false;
    }
    memcpy(pMemory,pfData,size);

#ifdef _WIN32
    if( FlushViewOfFile(pMemory, size) == 0){
       printf("ERROR IN  'CshareMemory::writeShareMemory' : FlushViewOfFile \n");
        return false;
    }
#endif /* _WIN32 */

    return true;
}

void CshareMemory::setID(int newId){
    _id=newId;
}

int  CshareMemory::getID(){
        return(_id);
}

char* CshareMemory::getName(){
    return cpName;
}

void CshareMemory::setName(char *name){
    if(name!=NULL){
        strcpy(cpName,name);
    }
}

size_t CshareMemory::getSize(){
    return _size;
}

void CshareMemory::setSize(size_t size){
    if(size > 0)
        _size=size;
}
#endif