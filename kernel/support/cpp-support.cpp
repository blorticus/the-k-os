// this is necessary to avoid having gcc complain at link time, if pure virtual functions
// are used
extern "C" void __cxa_pure_virtual() {
}

__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard* g) {
    return !*(char*)(g);
}

extern "C" void __cxa_guard_release(__guard* g) {
    *(char *)g = 1;
}

extern "C" void __cxa_guard_abort (__guard*) {
}

extern "C" void _Unwind_Resume(struct _Unwind_Exception * object) {
	*(char *)object = 1;
}
