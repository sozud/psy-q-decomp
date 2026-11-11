#ifndef STDARG_LOCAL_H
#define STDARG_LOCAL_H

#define __va_rounded_size(type) \
    (((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
#define va_start(ap, last) \
    (ap = ((char*)&(last) + __va_rounded_size(last)))
#define va_arg(ap, type) \
    (ap = ((char*)(ap)) += __va_rounded_size(type), \
     *((type*)((char*)(ap) - __va_rounded_size(type))))

typedef void* va_list;

#endif
