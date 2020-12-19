#include "types.h"
#include "user.h"

int test = 0;
lock_t lock1;

void *foo(void *n) {
	lprintf(1, "Value of n: %d\n", *(int*)n);
	int k = 1000000, local_test = 0;
	lock_acquire(&lock1);
	while (--k) {
		local_test = test;
		int j = 1;
		while (--j);
		test = local_test + 1;
	}
	lock_release(&lock1);
	exit();
}

int main() {
	int j = 1, k = 2, l = 3, m = 4;
	lock_init(&lock1);
	set_use_thread_lock(1);

	thread_create((void *) foo, (void *) &j);
	thread_create((void *) foo, (void *) &k);
	thread_create((void *) foo, (void *) &l);
	thread_create((void *) foo, (void *) &m);
	lprintf(1, "Main is waiting for threads to finish.\n");
	thread_join();
	thread_join();
	thread_join();
	thread_join();
	printf(1, "Threads finished executing.\n");
	printf(1, "Final value of test: %d\n", test);
	exit();
}
